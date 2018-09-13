/** @file
  The file provides AHCI block I/O interfaces.

  Copyright (c) 2010 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "AhciDevice.h"
#include <BlockDevice.h>

EFI_AHCI_CONTROLLER  *mAhciPrivateData;


/**
  Allocate device info data structure to contain device info.
  And insert the data structure to the tail of device list for tracing.

  @param[in]  AhciCtrlData      The AHCI controller instance.
  @param[in]  Port                The port number of the ATA device to send the command.
  @param[in]  PortMultiplierPort  The port multiplier port number of the ATA device to send the command.
                                  If there is no port multiplier, then specify 0xFFFF.
  @param[in]  DeviceType          The device type of the ATA device.
  @param[in]  IdentifyData        The data buffer to store the output of the IDENTIFY cmd.

  @retval EFI_SUCCESS             Successfully insert the ATA device to the tail of device list.
  @retval EFI_OUT_OF_RESOURCES    Can not allocate enough resource for use.

**/
EFI_STATUS
EFIAPI
CreateNewDeviceInfo (
  IN  EFI_AHCI_CONTROLLER           *AhciCtrlData,
  IN  UINT16                         Port,
  IN  UINT16                         PortMultiplier,
  IN  EFI_ATA_DEVICE_TYPE            DeviceType,
  IN  EFI_IDENTIFY_DATA              *IdentifyData
  )
{
  EFI_ATA_DEVICE_INFO    *DeviceInfo;
  EFI_ATA_IDENTIFY_DATA  *AtaData;

  DeviceInfo = AllocateZeroPool (sizeof (EFI_ATA_DEVICE_INFO));

  if (DeviceInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  DeviceInfo->Signature      = ATA_ATAPI_DEVICE_SIGNATURE;
  DeviceInfo->Port           = Port;
  DeviceInfo->PortMultiplier = PortMultiplier;
  DeviceInfo->Type           = DeviceType;
  DeviceInfo->Controller     = AhciCtrlData;

  CopyMem (&DeviceInfo->IdentifyData,  IdentifyData, sizeof (EFI_IDENTIFY_DATA));

  if (DeviceType == EfiIdeHarddisk) {
    AtaData = &DeviceInfo->IdentifyData.AtaData;
    if ((AtaData->Command_set_feature_enb_86 & LBA_48_BIT_ADDRESS_FEATURE_SET_SUPPORTED) != 0) {
      CopyMem (
        &DeviceInfo->TotalBlockNumber,
        (UINT8 *)AtaData + 100,
        4 * sizeof (UINT16)
        );
      DeviceInfo->DeviceFeature |= DEVICE_LBA_48_SUPPORT;
    } else {
      CopyMem (
        &DeviceInfo->TotalBlockNumber,
        &AtaData->User_addressable_sectors_lo,
        2 * sizeof (UINT16)
        );
    }
    DeviceInfo->BlockSize        = ATA_BLOCK_SIZE;
  } else if (DeviceType == EfiIdeCdrom) {
    DeviceInfo->BlockSize        = ATAPI_BLOCK_SIZE;
    DeviceInfo->TotalBlockNumber = ATAPI_INVALID_MAX_LBA_ADDRESS;
  }

  InsertTailList (&AhciCtrlData->DeviceList, &DeviceInfo->Link);

  return EFI_SUCCESS;
}

/**
  Find the AHCI device private data via giving controller, port
  and port multiplier number.

  @param[in]  AhciController  The pointer of parent AHCI controller
  @param[in]  PortNumber      The port number of device
  @param[in]  PortMulNumber   The port multiplier number of device

Returns:

  @retval NULL   Special device private doesn't found
  @retval other  The pointer to device private data

**/
EFI_ATA_DEVICE_INFO *
AhciFindDeviceData (
  IN EFI_AHCI_CONTROLLER  *AhciController,
  IN UINT32                PortNumber,
  IN UINT32                PortMulNumber
  )
{
  EFI_ATA_DEVICE_INFO           *AhciDeviceData;
  LIST_ENTRY                    *Link;

  for (Link  = AhciController->DeviceList.ForwardLink;
       Link != &AhciController->DeviceList;
       Link  = Link->ForwardLink) {
    AhciDeviceData = EFI_ATA_DEVICE_FROM_LINK (Link);
    if (((AhciDeviceData->Port == PortNumber) || (PortNumber == 0xFFFFFFFF)) &&
        ((AhciDeviceData->PortMultiplier == PortMulNumber) || (PortMulNumber == 0xFFFFFFFF))) {
      return AhciDeviceData;
    }
  }
  return NULL;
}

/**
  Reads the requested number of blocks from the specified block device.

  The function reads the requested number of blocks from the device. All the
  blocks are read, or an error is returned. If there is no media in the device,
  the function returns EFI_NO_MEDIA.

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
AhciReadBlocks (
  IN  UINTN                         DeviceIndex,
  IN  EFI_LBA                       StartLBA,
  IN  UINTN                         BufferSize,
  OUT VOID                          *Buffer
  )
{
  EFI_STATUS             Status;
  EFI_ATA_DEVICE_INFO   *AtaDeviceData;

  if (mAhciPrivateData == NULL) {
    return EFI_UNSUPPORTED;
  }

  AtaDeviceData = AhciFindDeviceData (mAhciPrivateData, DeviceIndex, 0xFFFFFFFF);

  if (AtaDeviceData == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = AhciReadWriteBlock (AtaDeviceData, TRUE, StartLBA, BufferSize, Buffer);

  return Status;
}

/**
  This function writes data from Memory to AHCI

  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk.
  @param[in]  StartLBA      Target AHCI block number(LBA) where data will be written
  @param[in]  BufferSize    Total data size to be written in bytes unit
  @param[in] Buffer        Data address in Memory to be copied to AHCI

  @retval EFI_SUCCESS            The operation is done correctly.
  @retval EFI_NOT_FOUND          The device index cannot be found.
  @retval EFI_UNSUPPORTED        AHCI system is not ready yet.
  @retval EFI_INVALID_PARAMETER  Input parameters are not valid.
  @retval EFI_DEVICE_ERROR       The write failed.
  @retval EFI_TIMEOUT            The operation is time out.
**/
EFI_STATUS
EFIAPI
AhciWriteBlocks (
  IN  UINTN                         DeviceIndex,
  IN  EFI_LBA                       StartLBA,
  IN  UINTN                         BufferSize,
  IN  VOID                         *Buffer
  )
{
  EFI_STATUS             Status;
  EFI_ATA_DEVICE_INFO   *AtaDeviceData;

  if (mAhciPrivateData == NULL) {
    return EFI_UNSUPPORTED;
  }

  AtaDeviceData = AhciFindDeviceData (mAhciPrivateData, DeviceIndex, 0xFFFFFFFF);

  if (AtaDeviceData == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = AhciReadWriteBlock (AtaDeviceData, FALSE, StartLBA, BufferSize, Buffer);

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
  @retval EFI_UNSUPPORTED    AHCI system is not ready yet.
  @retval EFI_NOT_FOUND      The device index cannot be found.

**/
EFI_STATUS
EFIAPI
AhciGetMediaInfo (
  IN  UINTN                          DeviceIndex,
  OUT DEVICE_BLOCK_INFO              *DevBlockInfo
  )
{
  EFI_ATA_DEVICE_INFO   *AtaDeviceData;

  if (mAhciPrivateData == NULL) {
    return EFI_UNSUPPORTED;
  }

  AtaDeviceData = AhciFindDeviceData (mAhciPrivateData, DeviceIndex, 0xFFFFFFFF);

  if (AtaDeviceData == NULL) {
    return EFI_NOT_FOUND;
  }

  DevBlockInfo->BlockNum  = AtaDeviceData->TotalBlockNumber;
  DevBlockInfo->BlockSize = AtaDeviceData->BlockSize;

  return EFI_SUCCESS;
}

/**
  The function will deinitialize AHCI resource.

  Based on AhciHcPciBase, this function will initialize AHCI host controller, allocate
  necessary resources, and enumerate all the AHCI bus.

  @param[in]  AhciController     AHCI Host Controller's data pointer

  @retval EFI_SUCCESS            The resource is successfully deinitialized.
  @retval EFI_INVALID_PARAMETER  Error occurs during deinitialization.

**/
EFI_STATUS
EFIAPI
AhciDeinitialize (
  IN  EFI_AHCI_CONTROLLER  *AhciController
  )
{
  EFI_ATA_DEVICE_INFO           *AhciDeviceData;
  EFI_AHCI_REGISTERS            *AhciRegisters;
  LIST_ENTRY                    *Link;

  if ((AhciController == NULL) || (AhciController->Signature != AHCI_CONTROLLER_SIGNATURE)) {
    return EFI_INVALID_PARAMETER;
  }

  Link  = AhciController->DeviceList.ForwardLink;
  while (Link != &AhciController->DeviceList) {
    AhciDeviceData = EFI_ATA_DEVICE_FROM_LINK (Link);
    Link  = Link->ForwardLink;
    FreePool (AhciDeviceData);
  }

  AhciRegisters = &AhciController->AhciRegisters;
  if (AhciRegisters->AhciCommandTable != NULL) {
    FreePages (
      AhciController->AhciRegisters.AhciCommandTable,
      EFI_SIZE_TO_PAGES ((UINTN) AhciRegisters->MaxCommandTableSize)
      );
  }

  if (AhciRegisters->AhciCommandTable != NULL) {
    FreePages (
      AhciRegisters->AhciCmdList,
      EFI_SIZE_TO_PAGES ((UINTN) AhciRegisters->MaxCommandListSize)
      );
  }

  if (AhciRegisters->AhciCommandTable != NULL) {
    FreePages (
      AhciRegisters->AhciRFis,
      EFI_SIZE_TO_PAGES ((UINTN) AhciRegisters->MaxReceiveFisSize)
      );
  }

  FreePool (AhciController);

  return EFI_SUCCESS;
}

/**
  The function will initialize AHCI device.

  Based on AhciHcPciBase, this function will initialize AHCI host controller, allocate
  necessary resources, and enumerate all the AHCI bus.

  @param[in]  AhciHcPciBase      AHCI Host Controller's PCI ConfigSpace Base address
  @param[in]  DevInitPhase       For the performance optimization,
                                 Device initialization is separated to several phases.

  @retval EFI_SUCCESS            The driver is successfully initialized.
  @retval EFI_NOT_FOUND          Can't find any AHCI block devices for boot.
  @retval EFI_OUT_OF_RESOURCES   Not sufficient memory.
  @retval EFI_DEVICE_ERROR       Hardware error during the initialization.
**/
EFI_STATUS
EFIAPI
AhciInitialize (
  IN  UINTN                     AhciHcPciBase,
  IN  DEVICE_INIT_PHASE         DevInitPhase
  )
{
  EFI_STATUS            Status;
  EFI_AHCI_CONTROLLER  *AhciPrivateData;

  DEBUG ((EFI_D_INFO, "Init AHCI Device %X\n", AhciHcPciBase));

  if (mAhciPrivateData != NULL) {
    DEBUG ((EFI_D_INFO, "Re-init AHCI controller\n"));
    AhciDeinitialize (mAhciPrivateData);
    mAhciPrivateData = NULL;
  }

  AhciPrivateData = AllocatePool (sizeof (EFI_AHCI_CONTROLLER));
  if (AhciPrivateData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  AhciPrivateData->Signature = AHCI_CONTROLLER_SIGNATURE;

  InitializeListHead (&AhciPrivateData->DeviceList);

  //
  // Enable AHCI controller
  //
  AhciPrivateData->AhciMemAddr = MmioRead32 (AhciHcPciBase + EFI_AHCI_BAR_OFFSET) & ~0xF;
  MmioOr8 (AhciHcPciBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);

  Status = AhciModeInitialization (AhciPrivateData);
  if (!EFI_ERROR (Status)) {
    mAhciPrivateData = AhciPrivateData;
  }

  return Status;
}
