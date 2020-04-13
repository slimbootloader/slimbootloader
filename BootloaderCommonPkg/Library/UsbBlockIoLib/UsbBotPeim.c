/** @file

Copyright (c) 2006 - 2020, Intel Corporation. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UsbBotPeim.h"
#include "BotPeim.h"

USB_BLK_IO_CALLBACK  mUsbBlkIoCb;

//
// Global function
//
EFI_PEI_RECOVERY_BLOCK_IO_PPI    mRecoveryBlkIoPpi = {
  BotGetNumberOfBlockDevices,
  BotGetMediaInfo,
  BotReadBlocks
};

EFI_PEI_PPI_DESCRIPTOR           mPpiList[2] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    NULL,
    NULL
  }
};

/**
  Detect whether the removable media is present and whether it has changed.

  @param[in]  PeiServices   General-purpose services that are available to every
                            PEIM.
  @param[in]  PeiBotDev     Indicates the PEI_BOT_DEVICE instance.

  @retval EFI_SUCCESS       The media status is successfully checked.
  @retval Other             Failed to detect media.

**/
EFI_STATUS
PeiBotDetectMedia (
  IN  EFI_PEI_SERVICES                          **PeiServices,
  IN  PEI_BOT_DEVICE                            *PeiBotDev
  );


/**
  UsbIo installation notification function.

  This function finds out all the current USB IO PPIs in the system and add them
  into private data.

  @param  PeiServices      Indirect reference to the PEI Services Table.
  @param  NotifyDesc       Address of the notification descriptor data structure.
  @param  InvokePpi        Address of the PPI that was invoked.

  @retval EFI_SUCCESS      The function completes successfully.

**/
EFI_STATUS
EFIAPI
NotifyOnUsbIoPpi (
  IN  EFI_PEI_SERVICES                              **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR                     *NotifyDesc,
  IN  VOID                                          *InvokePpi
  )
{
  PEI_USB_IO_PPI  *UsbIoPpi;

  UsbIoPpi = (PEI_USB_IO_PPI *) InvokePpi;

  InitUsbBot (PeiServices, UsbIoPpi);

  return EFI_SUCCESS;
}

/**
  Initialize the usb bot device.

  @param[in]  PeiServices   General-purpose services that are available to every
                            PEIM.
  @param[in]  UsbIoPpi      Indicates the PEI_USB_IO_PPI instance.

  @retval EFI_SUCCESS       The usb bot device is initialized successfully.
  @retval Other             Failed to initialize media.

**/
EFI_STATUS
InitUsbBot (
  IN  EFI_PEI_SERVICES                          **PeiServices,
  IN  PEI_USB_IO_PPI                            *UsbIoPpi
  )
{
  PEI_BOT_DEVICE                *PeiBotDevice;
  EFI_STATUS                    Status;
  EFI_USB_INTERFACE_DESCRIPTOR  *InterfaceDesc;
  UINTN                         MemPages;
  EFI_PHYSICAL_ADDRESS          AllocateAddress;
  EFI_USB_ENDPOINT_DESCRIPTOR   *EndpointDesc;
  UINT8                         Index;

  //
  // Check its interface
  //
  Status = UsbIoPpi->UsbGetInterfaceDescriptor (
             PeiServices,
             UsbIoPpi,
             &InterfaceDesc
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Check if it is the BOT device we support
  //
  if ((InterfaceDesc->InterfaceClass != 0x08) || (InterfaceDesc->InterfaceProtocol != 0x50)) {

    return EFI_NOT_FOUND;
  }

  MemPages = sizeof (PEI_BOT_DEVICE) / EFI_PAGE_SIZE + 1;
  Status = PeiServicesAllocatePages (
             EfiBootServicesCode,
             MemPages,
             &AllocateAddress
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PeiBotDevice                  = (PEI_BOT_DEVICE *) ((UINTN) AllocateAddress);

  PeiBotDevice->Signature       = PEI_BOT_DEVICE_SIGNATURE;
  PeiBotDevice->UsbIoPpi        = UsbIoPpi;
  PeiBotDevice->AllocateAddress = (UINTN) AllocateAddress;
  PeiBotDevice->BotInterface    = InterfaceDesc;

  //
  // Default value
  //
  PeiBotDevice->Media.DeviceType  = UsbMassStorage;
  PeiBotDevice->Media.BlockSize   = 0x200;

  //
  // Check its Bulk-in/Bulk-out endpoint
  //
  for (Index = 0; Index < 2; Index++) {
    Status = UsbIoPpi->UsbGetEndpointDescriptor (
               PeiServices,
               UsbIoPpi,
               Index,
               &EndpointDesc
               );

    if (EFI_ERROR (Status)) {
      return Status;
    }

    if ((EndpointDesc->EndpointAddress & 0x80) != 0) {
      PeiBotDevice->BulkInEndpoint = EndpointDesc;
    } else {
      PeiBotDevice->BulkOutEndpoint = EndpointDesc;
    }
  }

  CopyMem (
    & (PeiBotDevice->BlkIoPpi),
    &mRecoveryBlkIoPpi,
    sizeof (EFI_PEI_RECOVERY_BLOCK_IO_PPI)
    );
  CopyMem (
    & (PeiBotDevice->BlkIoPpiList),
    &mPpiList[0],
    sizeof (EFI_PEI_PPI_DESCRIPTOR)
    );
  PeiBotDevice->BlkIoPpiList.Ppi  = &PeiBotDevice->BlkIoPpi;

  Status                          = PeiUsbInquiry (PeiServices, PeiBotDevice);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PeiServicesAllocatePages (
             EfiBootServicesCode,
             1,
             &AllocateAddress
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PeiBotDevice->SensePtr = (ATAPI_REQUEST_SENSE_DATA *) ((UINTN) AllocateAddress);

  Status = PeiServicesInstallPpi (&PeiBotDevice->BlkIoPpiList);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Gets the count of block I/O devices that one specific block driver detects.

  This function is used for getting the count of block I/O devices that one
  specific block driver detects.  To the PEI ATAPI driver, it returns the number
  of all the detected ATAPI devices it detects during the enumeration process.
  To the PEI legacy floppy driver, it returns the number of all the legacy
  devices it finds during its enumeration process. If no device is detected,
  then the function will return zero.

  @param[in]  PeiServices          General-purpose services that are available
                                   to every PEIM.
  @param[in]  This                 Indicates the EFI_PEI_RECOVERY_BLOCK_IO_PPI
                                   instance.
  @param[out] NumberBlockDevices   The number of block I/O devices discovered.

  @retval     EFI_SUCCESS          Operation performed successfully.

**/
EFI_STATUS
EFIAPI
BotGetNumberOfBlockDevices (
  IN  EFI_PEI_SERVICES                         **PeiServices,
  IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI            *This,
  OUT UINTN                                    *NumberBlockDevices
  )
{
  //
  // For Usb devices, this value should be always 1
  //
  *NumberBlockDevices = 1;
  return EFI_SUCCESS;
}

/**
  Gets a block device's media information.

  This function will provide the caller with the specified block device's media
  information. If the media changes, calling this function will update the media
  information accordingly.

  @param[in]  PeiServices   General-purpose services that are available to every
                            PEIM
  @param[in]  This          Indicates the EFI_PEI_RECOVERY_BLOCK_IO_PPI instance.
  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk. Because the driver that implements Block I/O
                            PPIs will manage multiple block devices, the PPIs that
                            want to talk to a single device must specify the
                            device index that was assigned during the enumeration
                            process. This index is a number from one to
                            NumberBlockDevices.
  @param[out] MediaInfo     The media information of the specified block media.
                            The caller is responsible for the ownership of this
                            data structure.

  @retval EFI_SUCCESS        Media information about the specified block device
                             was obtained successfully.
  @retval EFI_DEVICE_ERROR   Cannot get the media information due to a hardware
                             error.

**/
EFI_STATUS
EFIAPI
BotGetMediaInfo (
  IN  EFI_PEI_SERVICES                          **PeiServices,
  IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI             *This,
  IN  UINTN                                     DeviceIndex,
  OUT EFI_PEI_BLOCK_IO_MEDIA                    *MediaInfo
  )
{
  PEI_BOT_DEVICE  *PeiBotDev;
  EFI_STATUS      Status;

  PeiBotDev = PEI_BOT_DEVICE_FROM_THIS (This);

  //
  // First test unit ready
  //
  PeiUsbTestUnitReady (
    PeiServices,
    PeiBotDev
    );

  Status = PeiBotDetectMedia (
             PeiServices,
             PeiBotDev
             );

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  CopyMem (
    MediaInfo,
    & (PeiBotDev->Media),
    sizeof (EFI_PEI_BLOCK_IO_MEDIA)
    );

  return EFI_SUCCESS;
}

/**
  Reads the requested number of blocks from the specified block device.

  The function reads the requested number of blocks from the device. All the
  blocks are read, or an error is returned. If there is no media in the device,
  the function returns EFI_NO_MEDIA.

  @param[in]  PeiServices   General-purpose services that are available to
                            every PEIM.
  @param[in]  This          Indicates the EFI_PEI_RECOVERY_BLOCK_IO_PPI instance.
  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk. Because the driver that implements Block I/O
                            PPIs will manage multiple block devices, the PPIs that
                            want to talk to a single device must specify the device
                            index that was assigned during the enumeration process.
                            This index is a number from one to NumberBlockDevices.
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
BotReadBlocks (
  IN  EFI_PEI_SERVICES                          **PeiServices,
  IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI             *This,
  IN  UINTN                                     DeviceIndex,
  IN  EFI_PEI_LBA                               StartLBA,
  IN  UINTN                                     BufferSize,
  OUT VOID                                      *Buffer
  )
{
  PEI_BOT_DEVICE  *PeiBotDev;
  EFI_STATUS      Status;
  UINTN           BlockSize;
  UINTN           NumberOfBlocks;

  Status    = EFI_SUCCESS;
  PeiBotDev = PEI_BOT_DEVICE_FROM_THIS (This);

  //
  // Check parameters
  //
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize == 0) {
    return EFI_SUCCESS;
  }

  if (!PeiBotDev->Media.MediaPresent) {
    return EFI_NO_MEDIA;
  }

  BlockSize = PeiBotDev->Media.BlockSize;

  if (BufferSize % BlockSize != 0) {
    Status = EFI_BAD_BUFFER_SIZE;
  }

  if (StartLBA > PeiBotDev->Media.LastBlock) {
    Status = EFI_INVALID_PARAMETER;
  }

  NumberOfBlocks = BufferSize / (PeiBotDev->Media.BlockSize);

  if (Status == EFI_SUCCESS) {

    Status = PeiUsbTestUnitReady (
               PeiServices,
               PeiBotDev
               );
    if (Status == EFI_SUCCESS) {
      Status = PeiUsbRead10 (
                 PeiServices,
                 PeiBotDev,
                 Buffer,
                 StartLBA,
                 1
                 );
    }
  } else {
    //
    // To generate sense data for DetectMedia use.
    //
    PeiUsbTestUnitReady (
      PeiServices,
      PeiBotDev
      );
  }

  if (EFI_ERROR (Status)) {
    //
    // if any error encountered, detect what happened to the media and
    // update the media info accordingly.
    //
    Status = PeiBotDetectMedia (
               PeiServices,
               PeiBotDev
               );
    if (Status != EFI_SUCCESS) {
      return EFI_DEVICE_ERROR;
    }

    NumberOfBlocks = BufferSize / PeiBotDev->Media.BlockSize;

    if (! (PeiBotDev->Media.MediaPresent)) {
      return EFI_NO_MEDIA;
    }

    if (BufferSize % (PeiBotDev->Media.BlockSize) != 0) {
      return EFI_BAD_BUFFER_SIZE;
    }

    if (StartLBA > PeiBotDev->Media.LastBlock) {
      return EFI_INVALID_PARAMETER;
    }

    if ((StartLBA + NumberOfBlocks - 1) > PeiBotDev->Media.LastBlock) {
      return EFI_INVALID_PARAMETER;
    }

    Status = PeiUsbRead10 (
               PeiServices,
               PeiBotDev,
               Buffer,
               StartLBA,
               NumberOfBlocks
               );

    switch (Status) {

    case EFI_SUCCESS:
      return EFI_SUCCESS;

    default:
      return EFI_DEVICE_ERROR;
    }
  } else {
    StartLBA += 1;
    NumberOfBlocks -= 1;
    Buffer = (UINT8 *) Buffer + PeiBotDev->Media.BlockSize;

    if (NumberOfBlocks == 0) {
      return EFI_SUCCESS;
    }

    Status = PeiUsbRead10 (
               PeiServices,
               PeiBotDev,
               Buffer,
               StartLBA,
               NumberOfBlocks
               );
    switch (Status) {

    case EFI_SUCCESS:
      return EFI_SUCCESS;

    default:
      return EFI_DEVICE_ERROR;

    }
  }
}

/**
  Detect whether the removable media is present and whether it has changed.

  @param[in]  PeiServices   General-purpose services that are available to every
                            PEIM.
  @param[in]  PeiBotDev     Indicates the PEI_BOT_DEVICE instance.

  @retval EFI_SUCCESS       The media status is successfully checked.
  @retval Other             Failed to detect media.

**/
EFI_STATUS
PeiBotDetectMedia (
  IN  EFI_PEI_SERVICES        **PeiServices,
  IN  PEI_BOT_DEVICE          *PeiBotDev
  )
{
  EFI_STATUS                  Status;
  EFI_STATUS                  FloppyStatus;
  UINTN                       SenseCounts;
  BOOLEAN                     NeedReadCapacity;
  ATAPI_REQUEST_SENSE_DATA    *SensePtr;
  UINTN                       Retry;

  //
  // if there is no media present,or media not changed,
  // the request sense command will detect faster than read capacity command.
  // read capacity command can be bypassed, thus improve performance.
  //
  SenseCounts       = 0;
  NeedReadCapacity  = TRUE;

  SensePtr = PeiBotDev->SensePtr;
  ZeroMem (SensePtr, EFI_PAGE_SIZE);

  Status = PeiUsbRequestSense (
             PeiServices,
             PeiBotDev,
             &SenseCounts,
             (UINT8 *) SensePtr
             );

  if (Status == EFI_SUCCESS) {
    //
    // No Media
    //
    if (IsNoMedia (SensePtr, SenseCounts)) {
      NeedReadCapacity              = FALSE;
      PeiBotDev->Media.MediaPresent = FALSE;
      PeiBotDev->Media.LastBlock    = 0;
    } else {
      //
      // Media Changed
      //
      if (IsMediaChange (SensePtr, SenseCounts)) {
        PeiBotDev->Media.MediaPresent  = TRUE;
      }
      //
      // Media Error
      //
      if (IsMediaError (SensePtr, SenseCounts)) {
        //
        // if media error encountered, make it look like no media present.
        //
        PeiBotDev->Media.MediaPresent = FALSE;
        PeiBotDev->Media.LastBlock    = 0;
      }

    }

  }

  if (NeedReadCapacity) {
    //
    // Retry at most 4 times to detect media info
    //
    for (Retry = 0; Retry < 4; Retry++) {
      switch (PeiBotDev->DeviceType) {
      case USBCDROM:
        Status = PeiUsbReadCapacity (
                   PeiServices,
                   PeiBotDev
                   );
        break;

      case USBFLOPPY2:
        Status = PeiUsbReadFormattedCapacity (
                   PeiServices,
                   PeiBotDev
                   );
        if (EFI_ERROR (Status) ||
            !PeiBotDev->Media.MediaPresent) {
          //
          // retry the ReadCapacity command
          //
          PeiBotDev->DeviceType = USBFLOPPY;
          Status = EFI_DEVICE_ERROR;
        }
        break;

      case USBFLOPPY:
        Status = PeiUsbReadCapacity (
                   PeiServices,
                   PeiBotDev
                   );
        if (EFI_ERROR (Status)) {
          //
          // retry the ReadFormatCapacity command
          //
          PeiBotDev->DeviceType = USBFLOPPY2;
        }
        break;

      default:
        return EFI_INVALID_PARAMETER;
      }

      SenseCounts = 0;
      ZeroMem (SensePtr, EFI_PAGE_SIZE);

      if (Status == EFI_SUCCESS) {
        break;
      }

      FloppyStatus = PeiUsbRequestSense (
                       PeiServices,
                       PeiBotDev,
                       &SenseCounts,
                       (UINT8 *) SensePtr
                       );

      //
      // If Request Sense data failed,retry.
      //
      if (EFI_ERROR (FloppyStatus)) {
        continue;
      }
      //
      // No Media
      //
      if (IsNoMedia (SensePtr, SenseCounts)) {
        PeiBotDev->Media.MediaPresent = FALSE;
        PeiBotDev->Media.LastBlock    = 0;
        break;
      }

      if (IsMediaError (SensePtr, SenseCounts)) {
        //
        // if media error encountered, make it look like no media present.
        //
        PeiBotDev->Media.MediaPresent = FALSE;
        PeiBotDev->Media.LastBlock    = 0;
        break;
      }
    }
    //
    // ENDFOR
    //
    // tell whether the readcapacity process is successful or not
    // ("Status" variable record the latest status returned
    // by ReadCapacity )
    //
    if (Status != EFI_SUCCESS) {
      return EFI_DEVICE_ERROR;
    }
  }

  return EFI_SUCCESS;
}

/**
  Initialize the usb bot device and installation of callback function.

  This function finds out all the current USB IO PPIs in the system and add them
  into private data.

  @param[in]  UsbIoPpi      Indicates the PEI_USB_IO_PPI instance.
  @param      UsbBlkCb      USB block IO callback function.

  @retval EFI_SUCCESS      The function completes successfully.

**/
EFI_STATUS
EFIAPI
UsbFindBlockDevice (
  IN  PEI_USB_IO_PPI                    *UsbIoPpi,
  USB_BLK_IO_CALLBACK                    UsbBlkCb
  )
{
  mUsbBlkIoCb = UsbBlkCb;
  return InitUsbBot (NULL, UsbIoPpi);
}
