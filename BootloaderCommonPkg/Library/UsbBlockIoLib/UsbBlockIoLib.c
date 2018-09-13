/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
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
#include <Library/XhciLib.h>
#include <Library/UsbBusLib.h>
#include <UsbBotPeim.h>
#include <BlockDevice.h>

#define  MAX_USB_DEVICE_NUMBER         16
#define  MAX_USB_BLOCK_DEVICE_NUMBER   1

UINTN                           mUsbIoCount;
PEI_USB_IO_PPI                 *mUsbIoArray[MAX_USB_DEVICE_NUMBER];

UINTN                           mUsbBlkCount;
EFI_PEI_RECOVERY_BLOCK_IO_PPI  *mUsbBlkArray[MAX_USB_BLOCK_DEVICE_NUMBER];

/**
  This is a USB device callback function.

  When a new USB device is found during USB bus enumeration, it will
  be called to register this device.

  @param[in]  UsbIoPpi           The USB device interface instance.

  @retval EFI_SUCCESS            This routinue alwasy return success.

**/
EFI_STATUS
EFIAPI
UsbDevCallback (
  IN PEI_USB_IO_PPI   *UsbIoPpi
  )
{
  if (mUsbIoCount < sizeof (mUsbIoArray) / sizeof (mUsbIoArray[0])) {
    mUsbIoArray[mUsbIoCount++] = UsbIoPpi;
  }
  return EFI_SUCCESS;
}

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
  if (mUsbBlkCount < sizeof (mUsbBlkArray) / sizeof (mUsbBlkArray[0])) {
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
  EFI_HANDLE  UsbHostHandle;
  EFI_STATUS  Status;
  UINTN       Idx;
  UINT32      PcieAddress;
  UINT32      XhciMmioBase;

  mUsbIoCount  = 0;
  mUsbBlkCount = 0;

  // Enable XHCI controller
  PcieAddress = UsbHcPciBase;
  MmioOr8 (PcieAddress + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
  XhciMmioBase = MmioRead32 (PcieAddress + PCI_BASE_ADDRESSREG_OFFSET) & ~0xF;

  Status = UsbInitCtrl (XhciMmioBase, &UsbHostHandle);
  DEBUG ((DEBUG_INFO, "Init USB XHCI - %r\n", Status));

  Status = UsbEnumBus (UsbHostHandle, UsbDevCallback);
  DEBUG ((DEBUG_INFO, "Enumerate Bus - %r\n", Status));
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Found %d USB devices on bus\n", mUsbIoCount));
  }

  for (Idx = 0; Idx < mUsbIoCount; Idx++) {
    Status = UsbFindBlockDevice (mUsbIoArray[Idx], UsbBlkCallback);
    if (!EFI_ERROR (Status) && (mUsbBlkCount > 0)) {
      DEBUG ((DEBUG_INFO, "Found mass storage on devices %d\n", Idx));
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
