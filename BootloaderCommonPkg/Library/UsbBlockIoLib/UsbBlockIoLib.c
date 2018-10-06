/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <IndustryStandard/Pci.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UsbIoLib.h>
#include <UsbBotPeim.h>
#include <BlockDevice.h>

#define  MAX_USB_BLOCK_DEVICE_NUMBER   1

UINTN                           mUsbBlkCount;
EFI_PEI_RECOVERY_BLOCK_IO_PPI  *mUsbBlkArray[MAX_USB_BLOCK_DEVICE_NUMBER];


/**
  This is a USB block device callback function.

  When a new USB block device is found, it will be called to register this block
  device instance.

  @param[in]  BlkIoPpi           The block device interface instance.

  @retval EFI_SUCCESS            This routinue alwasy return success.

**/
EFI_STATUS
EFIAPI
UsbBlkCallback (
  IN EFI_PEI_RECOVERY_BLOCK_IO_PPI    *BlkIoPpi
  )
{
  if (mUsbBlkCount < ARRAY_SIZE (mUsbBlkArray)) {
    mUsbBlkArray[mUsbBlkCount++] = BlkIoPpi;
  }
  return EFI_SUCCESS;
}


/**
  The function will initialize USB device.

  Based on UsbHcPciBase, this function will initialize USB host controller, allocate
  necessary resources, and enumarate all the USB bus.

  @param[in]  UsbHcPciBase       USB Host Controller's PCI ConfigSpace Base address
  @param[in]  DevInitPhase       For the performance optimization,
                                 Device initialization is separated to several phases.

  @retval EFI_SUCCESS            The driver is successfully initialized.
  @retval EFI_NOT_FOUND          Can't find any USB block devices for boot.

**/
EFI_STATUS
EFIAPI
InitializeUsb (
  IN  UINTN                     UsbHcPciBase,
  IN  DEVICE_INIT_PHASE         DevInitPhase
  )
{
  EFI_STATUS        Status;
  UINTN             Index;
  UINT32            UsbIoCount;
  PEI_USB_IO_PPI  **UsbIoArray;

  mUsbBlkCount = 0;
  Status = InitUsbDevices (UsbHcPciBase);
  if (!EFI_ERROR(Status)) {
    Status = GetUsbDevices ((PEI_USB_IO_PPI **)&UsbIoArray, &UsbIoCount);
  }
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_INFO, "Failed to initialize USB bus !\n"));
    return Status;
  }

  for (Index = 0; Index < UsbIoCount; Index++) {
    Status = UsbFindBlockDevice (UsbIoArray[Index], UsbBlkCallback);
    if (!EFI_ERROR (Status) && (mUsbBlkCount > 0)) {
      DEBUG ((DEBUG_INFO, "Found mass storage on device %d\n", Index));
      break;
    }
  }

  if (mUsbBlkCount > 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
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
UsbGetMediaInfo (
  IN  UINTN                          DeviceIndex,
  OUT DEVICE_BLOCK_INFO              *DevBlockInfo
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_BLOCK_IO_MEDIA          MediaInfo;

  if (mUsbBlkCount == 0) {
    Status = EFI_DEVICE_ERROR;
  } else {
    Status = mUsbBlkArray[0]->GetBlockDeviceMediaInfo (NULL, mUsbBlkArray[0], 0, &MediaInfo);
    if (!EFI_ERROR (Status)) {
      if (DevBlockInfo != NULL) {
        DevBlockInfo->BlockNum  = MediaInfo.LastBlock + 1;
        DevBlockInfo->BlockSize = MediaInfo.BlockSize;
      }
    }
  }
  return Status;
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
UsbReadBlocks (
  IN  UINTN                         DeviceIndex,
  IN  EFI_LBA                       StartLBA,
  IN  UINTN                         BufferSize,
  OUT VOID                          *Buffer
  )
{
  EFI_STATUS  Status;

  if (mUsbBlkCount == 0) {
    Status = EFI_DEVICE_ERROR;
  } else {
    Status = mUsbBlkArray[0]->ReadBlocks (NULL, mUsbBlkArray[0], DeviceIndex, StartLBA, BufferSize, Buffer);
  }
  return Status;
}
