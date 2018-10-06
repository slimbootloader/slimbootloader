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
#include <Library/XhciLib.h>
#include <Library/UsbBusLib.h>

#define  MAX_USB_DEVICE_NUMBER  32

BOOLEAN                         mUsbIoInitDone;
UINTN                           mUsbIoCount;
PEI_USB_IO_PPI                 *mUsbIoArray[MAX_USB_DEVICE_NUMBER];


/**
  This function register each USB device into an array.

  When a new USB device is found during USB bus enumeration, it will
  be called to register this device.

  @param[in]  UsbIoPpi           The USB device interface instance.

  @retval EFI_SUCCESS            This routinue alwasy return success.

**/
EFI_STATUS
EFIAPI
RegisterUsbDevice (
  IN PEI_USB_IO_PPI   *UsbIoPpi
  )
{
  if (mUsbIoCount < ARRAY_SIZE (mUsbIoArray)) {
    mUsbIoArray[mUsbIoCount++] = UsbIoPpi;
  }
  return EFI_SUCCESS;
}


/**
  The function will initialize USB device on bus.

  Based on UsbHcPciBase, this function will initialize USB host controller, allocate
  necessary resources, and enumarate all devcies on the USB bus.

  @param[in]  UsbHcPciBase       USB Host Controller's PCI ConfigSpace Base address

  @retval EFI_SUCCESS            The driver is successfully initialized.
  @retval EFI_NOT_FOUND          Can't find any USB block devices for boot.

**/
EFI_STATUS
EFIAPI
InitUsbDevices (
  IN  UINTN                     UsbHcPciBase
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  UsbHostHandle;
  UINT32      PcieAddress;
  UINT32      XhciMmioBase;

  if (!mUsbIoInitDone) {
    // Enable XHCI controller
    PcieAddress = UsbHcPciBase;
    MmioOr8 (PcieAddress + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
    XhciMmioBase = MmioRead32 (PcieAddress + PCI_BASE_ADDRESSREG_OFFSET) & ~0xF;

    Status = UsbInitCtrl (XhciMmioBase, &UsbHostHandle);
    DEBUG ((DEBUG_INFO, "Init USB XHCI - %r\n", Status));

    Status = UsbEnumBus (UsbHostHandle, RegisterUsbDevice);
    DEBUG ((DEBUG_INFO, "Enumerate Bus - %r\n", Status));
    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Found %d USB devices on bus\n", mUsbIoCount));
    }

    mUsbIoInitDone = TRUE;
  } else {
    Status = EFI_SUCCESS;
  }

  return Status;
}


/**
  The function will return USB device array.

  @param[in, out]  UsbIoArray    Address to receive USB I/O instance array.
  @param[in, out]  UsbIoCount    Address to receive USB I/O device count.

  @retval EFI_SUCCESS            USB I/O instancs are returned successfully.
  @retval EFI_NOT_AVAILABLE_YET  USB bus has not been enumerated yet.

**/
EFI_STATUS
EFIAPI
GetUsbDevices (
  IN OUT   PEI_USB_IO_PPI  **UsbIoArray,
  IN OUT   UINT32           *UsbIoCount
  )
{
  if (!mUsbIoInitDone) {
    return EFI_NOT_AVAILABLE_YET;
  }

  if (UsbIoArray != NULL) {
    *UsbIoArray = (PEI_USB_IO_PPI *)mUsbIoArray;
  }

  if (UsbIoCount != NULL) {
    *UsbIoCount = mUsbIoCount;
  }

  return EFI_SUCCESS;
}
