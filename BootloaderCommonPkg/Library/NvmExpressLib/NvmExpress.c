/** @file
  NvmExpress driver is used to manage non-volatile memory subsystem which follows
  NVM Express specification.

  Copyright (c) 2013 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include "NvmExpress.h"

NVME_CONTROLLER_PRIVATE_DATA        *mNvmeCtrlPrivate;

NVME_DEVICE_PRIVATE_DATA            *mMultiNvmeDrive[10]; //maxium 10

NVM_CTRL_PLATFORM_INFO NvmCtrlInfo = { {1, 0, 0} };


//
// Template for NVM Express Pass Thru Mode data structure.
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_NVM_EXPRESS_PASS_THRU_MODE gEfiNvmExpressPassThruMode = {
  EFI_NVM_EXPRESS_PASS_THRU_ATTRIBUTES_PHYSICAL   |
  EFI_NVM_EXPRESS_PASS_THRU_ATTRIBUTES_LOGICAL    |
  EFI_NVM_EXPRESS_PASS_THRU_ATTRIBUTES_NONBLOCKIO |
  EFI_NVM_EXPRESS_PASS_THRU_ATTRIBUTES_CMD_SET_NVM,
  sizeof (UINTN),
  0x10100
};

/**
  Check if the specified Nvm Express device namespace is active, and create child handles
  for them with BlockIo and DiskInfo protocol instances.

  @param[in] Private         The pointer to the NVME_CONTROLLER_PRIVATE_DATA data structure.
  @param[in] NamespaceId     The NVM Express namespace ID  for which a device path node is to be
                             allocated and built. Caller must set the NamespaceId to zero if the
                             device path node will contain a valid UUID.

  @retval EFI_SUCCESS        All the namespaces in the device are successfully enumerated.
  @return Others             Some error occurs when enumerating the namespaces.

**/
EFI_STATUS
EnumerateNvmeDevNamespace (
  IN NVME_CONTROLLER_PRIVATE_DATA       *Private,
  UINT32                                NamespaceId
  )
{
  NVME_ADMIN_NAMESPACE_DATA             *NamespaceData;
  NVME_DEVICE_PRIVATE_DATA              *Device;
  EFI_STATUS                            Status;
  UINT32                                Lbads;
  UINT32                                Flbas;
  UINT32                                LbaFmtIdx;
  UINT8                                 Sn[21];
  UINT8                                 Mn[41];
  Device            = NULL;

  //
  // Allocate a buffer for Identify Namespace data
  //
  NamespaceData = AllocateZeroPool (sizeof (NVME_ADMIN_NAMESPACE_DATA));
  if (NamespaceData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Identify Namespace
  //
  Status = NvmeIdentifyNamespace (
             Private,
             NamespaceId,
             (VOID *)NamespaceData
             );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  //
  // Validate Namespace
  //
  if (NamespaceData->Ncap == 0) {
    Status = EFI_DEVICE_ERROR;
  } else {
    //
    // allocate device private data for each discovered namespace
    //
    Device = AllocateZeroPool (sizeof (NVME_DEVICE_PRIVATE_DATA));
    if (Device == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }

    //
    // Initialize SSD namespace instance data
    //
    Device->Signature           = NVME_DEVICE_PRIVATE_DATA_SIGNATURE;
    Device->NamespaceId         = NamespaceId;
    Device->NamespaceUuid       = NamespaceData->Eui64;
    Device->Controller          = Private;

    //
    // Build BlockIo media structure
    //
    Device->Media.MediaId        = 0;
    Device->Media.RemovableMedia = FALSE;
    Device->Media.MediaPresent   = TRUE;
    Device->Media.LogicalPartition = FALSE;
    Device->Media.ReadOnly       = FALSE;
    Device->Media.WriteCaching   = FALSE;
    Device->Media.IoAlign        = Private->PassThruMode.IoAlign;

    Flbas     = NamespaceData->Flbas;
    LbaFmtIdx = Flbas & 0xF;
    Lbads     = NamespaceData->LbaFormat[LbaFmtIdx].Lbads;
    Device->Media.BlockSize = (UINT32)1 << Lbads;

    Device->Media.LastBlock                     = NamespaceData->Nsze - 1;
    Device->Media.LogicalBlocksPerPhysicalBlock = 1;
    Device->Media.LowestAlignedLba              = 1;

    //
    // Create BlockIo Protocol instance
    //
    Device->BlockIo.Revision     = EFI_BLOCK_IO_PROTOCOL_REVISION2;
    Device->BlockIo.Media        = &Device->Media;
    Device->BlockIo.Reset        = NvmeBlockIoReset;
    Device->BlockIo.ReadBlocks   = NvmeBlockIoReadBlocks;
    Device->BlockIo.WriteBlocks  = NvmeBlockIoWriteBlocks;
    Device->BlockIo.FlushBlocks  = NvmeBlockIoFlushBlocks;

    InitializeListHead (&Device->AsyncQueue);

    //
    // Create StorageSecurityProtocol Instance
    //
    Device->StorageSecurity.ReceiveData = NvmeStorageSecurityReceiveData;
    Device->StorageSecurity.SendData    = NvmeStorageSecuritySendData;

    //
    // Create DiskInfo Protocol instance
    //
    CopyMem (&Device->NamespaceData, NamespaceData, sizeof (NVME_ADMIN_NAMESPACE_DATA));
    InitializeDiskInfo (Device);
    mMultiNvmeDrive[NamespaceId - 1] = Device; //NamespaceId is 1 based

    if ((Private->ControllerData->Oacs & SECURITY_SEND_RECEIVE_SUPPORTED) == 0) {
      Device->StorageSecurity.ReceiveData = NULL;
      Device->StorageSecurity.SendData = NULL;
    }

    //
    // Dump NvmExpress Identify Namespace Data
    //
    DEBUG ((DEBUG_VERBOSE, " == NVME IDENTIFY NAMESPACE [%d] DATA ==\n", NamespaceId));
    DEBUG ((DEBUG_VERBOSE, "    NSZE        : 0x%x\n", NamespaceData->Nsze));
    DEBUG ((DEBUG_VERBOSE, "    NCAP        : 0x%x\n", NamespaceData->Ncap));
    DEBUG ((DEBUG_VERBOSE, "    NUSE        : 0x%x\n", NamespaceData->Nuse));
    DEBUG ((DEBUG_VERBOSE, "    LBAF0.LBADS : 0x%x\n", (NamespaceData->LbaFormat[0].Lbads)));

    //
    // Build controller name for Component Name (2) protocol.
    //
    CopyMem (Sn, Private->ControllerData->Sn, sizeof (Private->ControllerData->Sn));
    Sn[20] = 0;
    CopyMem (Mn, Private->ControllerData->Mn, sizeof (Private->ControllerData->Mn));
    Mn[40] = 0;
    UnicodeSPrintAsciiFormat (Device->ModelName, sizeof (Device->ModelName), "%a-%a-%x", Sn, Mn, NamespaceData->Eui64);
  }

Exit:
  if (NamespaceData != NULL) {
    FreePool (NamespaceData);
  }

  if (EFI_ERROR (Status) && (Device != NULL)) {
    FreePool (Device);
  }
  return Status;
}

/**
  Discover all Nvm Express device namespaces, and create child handles for them with BlockIo
  and DiskInfo protocol instances.

  @param[in] Private         The pointer to the NVME_CONTROLLER_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS        All the namespaces in the device are successfully enumerated.
  @return Others             Some error occurs when enumerating the namespaces.

**/
EFI_STATUS
DiscoverAllNamespaces (
  IN NVME_CONTROLLER_PRIVATE_DATA       *Private
  )
{
  EFI_STATUS                            Status;
  UINT32                                NamespaceId;
  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL    *Passthru;

  NamespaceId   = 0xFFFFFFFF;
  Passthru      = &Private->Passthru;

  while (TRUE) {
    Status = Passthru->GetNextNamespace (
               Passthru,
               (UINT32 *)&NamespaceId
               );

    if (EFI_ERROR (Status)) {
      break;
    }

    Status = EnumerateNvmeDevNamespace (
               Private,
               NamespaceId
               );

    if (EFI_ERROR (Status)) {
      continue;
    }
  }

  return EFI_SUCCESS;
}

/**
  De-initialize the NVMe controller.

  @param[in]  Private        NVMe controller private data pointer.

  @retval EFI_SUCCESS        Always return EFI_SUCCESS.

**/
EFI_STATUS
NvmeDeInitialize (
  IN  NVME_CONTROLLER_PRIVATE_DATA        *Private
  )
{
  UINTN   Index;

  NvmeDisableController (Private);

  for (Index = 0; Index < ARRAY_SIZE (mMultiNvmeDrive); Index++) {
    if (mMultiNvmeDrive[Index] != NULL) {
      FreePool (mMultiNvmeDrive[Index]);
      mMultiNvmeDrive[Index] = NULL;
    }
  }

  if (Private->Buffer != NULL) {
    IoMmuFreeBuffer (6, Private->Buffer, Private->Mapping);
  }

  if (Private->ControllerData != NULL) {
    FreePool (Private->ControllerData);
  }

  FreePool (Private);

  return EFI_SUCCESS;
}

/**
  Starts a device controller or a bus controller.

  The Start() function is designed to be invoked from the EFI boot service ConnectController().
  As a result, much of the error checking on the parameters to Start() has been moved into this
  common boot service. It is legal to call Start() from other locations,
  but the following calling restrictions must be followed or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE.
  2. If RemainingDevicePath is not NULL, then it must be a pointer to a naturally aligned
     EFI_DEVICE_PATH_PROTOCOL.
  3. Prior to calling Start(), the Supported() function for the driver specified by This must
     have been called with the same calling parameters, and Supported() must have returned EFI_SUCCESS.

  @param[in]  NvmeHcPciBase        Nvme Host Controller's PCI ConfigSpace Base address.
  @param[in]  NvmeInitMode         Nvme mode this parameter to have compatibility with DevInit in
                                   LinuxLoader.

  @retval EFI_SUCCESS              The device was started.
  @retval EFI_DEVICE_ERROR         The device could not be started due to a device error.Currently not implemented.
  @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.
  @retval Others                   The driver failded to start the device.

**/
EFI_STATUS
EFIAPI
NvmeInitialize (
  IN  UINTN               NvmeHcPciBase,
  IN  DEVICE_INIT_PHASE   NvmeInitMode
  )
{
  EFI_STATUS                          Status;
  NVME_CONTROLLER_PRIVATE_DATA        *Private;
  EFI_PHYSICAL_ADDRESS                MappedAddr;
  UINTN                               Index;
  UINTN                               NameSpaceCnt;

  DEBUG ((DEBUG_INFO, "%a NVMe controller\n", (NvmeInitMode == DevDeinit) ? "Deinit" : "Init"));

  if (NvmeInitMode == DevDeinit) {
    if (mNvmeCtrlPrivate != NULL) {
      NvmeDeInitialize (mNvmeCtrlPrivate);
      // Disable Bus Master
      MmioAnd16 (mNvmeCtrlPrivate->PciBaseAddr + PCI_COMMAND_OFFSET,
                 (UINT16)~(EFI_PCI_COMMAND_IO_SPACE | EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER));
      mNvmeCtrlPrivate = NULL;
    }
    return EFI_SUCCESS;
  }

  // Enable Bus Master
  MmioOr16 (NvmeHcPciBase + PCI_COMMAND_OFFSET,
            (UINT16)(EFI_PCI_COMMAND_IO_SPACE | EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER));

  Private          = NULL;

  //
  // Check EFI_ALREADY_STARTED to reuse the original NVME_CONTROLLER_PRIVATE_DATA.
  //
  Private = AllocateZeroPool (sizeof (NVME_CONTROLLER_PRIVATE_DATA));
  if (Private == NULL) {
    DEBUG ((DEBUG_VERBOSE, "NvmExpressDriverBindingStart: allocating pool for Nvme Private Data failed!\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // 6 x 4kB aligned buffers will be carved out of this buffer.
  // 1st 4kB boundary is the start of the admin submission queue.
  // 2nd 4kB boundary is the start of the admin completion queue.
  // 3rd 4kB boundary is the start of I/O submission queue #1.
  // 4th 4kB boundary is the start of I/O completion queue #1.
  // 5th 4kB boundary is the start of I/O submission queue #2.
  // 6th 4kB boundary is the start of I/O completion queue #2.
  //
  // Allocate 6 pages of memory, then map it for bus master read and write.
  //
  Status = IoMmuAllocateBuffer (
             6,
             (VOID**)&Private->Buffer,
             &MappedAddr,
             &Private->Mapping
             );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }
  Private->BufferPciAddr             = (UINT8 *)(UINTN)MappedAddr;
  Private->Signature                 = NVME_CONTROLLER_PRIVATE_DATA_SIGNATURE;
  Private->NvmeHCBase                = MmioRead32 (NvmeHcPciBase + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFF000;
  Private->PciBaseAddr               = NvmeHcPciBase;
  Private->Passthru.Mode             = &Private->PassThruMode;
  Private->Passthru.PassThru         = NvmExpressPassThru;
  Private->Passthru.GetNextNamespace = NvmExpressGetNextNamespace;
  CopyMem (&Private->PassThruMode, &gEfiNvmExpressPassThruMode, sizeof (EFI_NVM_EXPRESS_PASS_THRU_MODE));
  InitializeListHead (&Private->AsyncPassThruQueue);
  InitializeListHead (&Private->UnsubmittedSubtasks);

  Status = NvmeControllerInit (Private);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }
  mNvmeCtrlPrivate = Private;

  Status = DiscoverAllNamespaces (
             Private
             );

  NameSpaceCnt = 0;
  for (Index = 0; Index < ARRAY_SIZE (mMultiNvmeDrive); Index++) {
    if (mMultiNvmeDrive[Index] != NULL) {
      NameSpaceCnt++;
    }
  }

  DEBUG ((DEBUG_INFO, "Found %d NVMe namespace\n", NameSpaceCnt));
  return EFI_SUCCESS;

Exit:
  if ((Private != NULL) && (Private->Buffer != NULL)) {
    IoMmuFreeBuffer (6, Private->Buffer, Private->Mapping);
  }

  if (EFI_ERROR (Status)) {
    if ((Private != NULL) && (Private->ControllerData != NULL)) {
      FreePool (Private->ControllerData);
    }
  }

  MmioAnd16 (NvmeHcPciBase + PCI_COMMAND_OFFSET,
            (UINT16)~(EFI_PCI_COMMAND_IO_SPACE | EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER));

  DEBUG ((DEBUG_INFO, "NVMe initialization failed - %r\n", Status));

  return Status;
}


/**
  Gets a block device's media information.

  This function will provide the caller with the specified block device's media
  information. If the media changes, calling this function will update the media
  information accordingly.

  @param[in]  DeviceIndex    Specifies the block device to which the function wants
                             to talk.
  @param[out] DevBlockInfo   The Block Io information of the specified block partition.

  @retval EFI_SUCCESS        The Block Io information about the specified block device
                             was obtained successfully.
  @retval EFI_DEVICE_ERROR   Cannot get the media information due to a hardware
                             error.

**/
EFI_STATUS
EFIAPI
NvmeGetMediaInfo (
  IN  UINTN                          DeviceIndex,
  OUT DEVICE_BLOCK_INFO              *DevBlockInfo
  )
{
  if (DeviceIndex >= ARRAY_SIZE (mMultiNvmeDrive)) {
    return EFI_INVALID_PARAMETER;
  }
  if (mMultiNvmeDrive[DeviceIndex] == NULL) {
    return EFI_NOT_FOUND;
  }
  DevBlockInfo->BlockNum  = mMultiNvmeDrive[DeviceIndex]->Media.LastBlock + 1;
  DevBlockInfo->BlockSize = mMultiNvmeDrive[DeviceIndex]->Media.BlockSize;

  return EFI_SUCCESS;
}

/**
  This function reads data from Nvme device to Memory.

  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk.
  @param[in]  StartLBA      The starting logical block address (LBA) to read from
                            on the device
  @param[in]  BufferSize    The size of the Buffer in bytes. This number must be
                            a multiple of the intrinsic block size of the device.
  @param[out] Buffer        A pointer to the destination buffer for the data.
                            The caller is responsible for the ownership of the
                            buffer.

  @retval EFI_SUCCESS             The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR        The device reported an error while attempting
                                  to perform the read operation.
  @retval EFI_INVALID_PARAMETER   The read request contains LBAs that are not
                                  valid, or the buffer is not properly aligned.
  @retval EFI_NO_MEDIA            There is no media in the device.
  @retval EFI_BAD_BUFFER_SIZE     The BufferSize parameter is not a multiple of
                                  the intrinsic block size of the device.

**/
EFI_STATUS
EFIAPI
NvmeReadBlocks (
  IN  UINTN                         DeviceIndex,
  IN  EFI_PEI_LBA                   StartLBA,
  IN  UINTN                         BufferSize,
  OUT VOID                          *Buffer
  )
{
  EFI_STATUS Status;

  if (DeviceIndex >= ARRAY_SIZE (mMultiNvmeDrive)) {
    return EFI_INVALID_PARAMETER;
  }
  if (mMultiNvmeDrive[DeviceIndex] == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = NvmeBlockIoReadBlocks (&mMultiNvmeDrive[DeviceIndex]->BlockIo, 0, StartLBA, BufferSize, Buffer);

  return Status;
}

/**
  This function writes data from Memory to Nvme device

  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk.
  @param[in]  StartLBA      Target Nvme block number(LBA) where data will be written
  @param[in]  DataSize      Total data size to be written in bytes unit
  @param[in] DataAddress   Data address in Memory to be copied to EMMC

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
EFIAPI
NvmeWriteBlocks (
  IN  UINTN                         DeviceIndex,
  IN  EFI_LBA                       StartLBA,
  IN  UINTN                         DataSize,
  IN  VOID                          *DataAddress
  )
{
  EFI_STATUS Status;

  if (DeviceIndex >= ARRAY_SIZE (mMultiNvmeDrive)) {
    return EFI_INVALID_PARAMETER;
  }
  if (mMultiNvmeDrive[DeviceIndex] == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = NvmeBlockIoWriteBlocks (&mMultiNvmeDrive[DeviceIndex]->BlockIo, 0, StartLBA, DataSize, DataAddress);

  return Status;
}
