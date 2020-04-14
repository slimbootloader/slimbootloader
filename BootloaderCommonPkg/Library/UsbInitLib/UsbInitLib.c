/** @file
  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <UsbInitLibInternal.h>

USB_INIT_INSTANCE  mUsbInit;

/**
  This function register each USB device into an array.

  When a new USB device is found during USB bus enumeration, it will
  be called to register this device.

  @param[in]  UsbIoPpi           The USB device interface instance.

  @retval EFI_UNSUPPORTED        USB device registeration failed due to insufficant entry.
  @retval EFI_SUCCESS            This routinue alwasy return success.

**/
EFI_STATUS
EFIAPI
RegisterUsbDevice (
  IN PEI_USB_IO_PPI   *UsbIoPpi
  )
{
  EFI_STATUS  Status;

  if (mUsbInit.UsbIoCount < ARRAY_SIZE (mUsbInit.UsbIoArray)) {
    mUsbInit.UsbIoArray[mUsbInit.UsbIoCount++] = UsbIoPpi;
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_UNSUPPORTED;
  }
  return Status;
}

/**
  The function will de-initialize USB device.

  For USB system, it is required to do de-initialization at the end of
  the boot stage so that the host controller will stop transactions.

  @retval EFI_SUCCESS            The driver is successfully deinitialized.
  @retval EFI_NOT_FOUND          Can't find any USB block devices for boot.

**/
EFI_STATUS
EFIAPI
DeinitUsbDevices (
  VOID
  )
{
  EFI_STATUS  Status;
  UINTN                Index;

  if (mUsbInit.UsbHostHandle == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = UsbDeinitCtrl (mUsbInit.UsbHostHandle);

  UsbDeInitBot ();

  for (Index = 0; Index < mUsbInit.UsbIoCount; Index++) {
    UsbDeinitDevice (mUsbInit.UsbIoArray[Index]);
  }

  ZeroMem (&mUsbInit, sizeof(mUsbInit));
  return Status;
}


/**
  The function will initialize USB device on bus.

  Based on UsbHcPciBase, this function will initialize USB host controller, allocate
  necessary resources, and enumarate all devcies on the USB bus.

  @param[in]  UsbHcPciBase       USB Host Controller's PCI ConfigSpace Base address

  @retval EFI_SUCCESS            The driver is successfully initialized.
  @retval EFI_NOT_FOUND          Can't find any USB block devices for boot.
  @retval EFI_UNSUPPORTED        Device is not XHCI controller.

**/
EFI_STATUS
EFIAPI
InitUsbDevices (
  IN  UINTN                     UsbHcPciBase
  )
{
  EFI_STATUS  Status;
  UINTN       PcieAddress;
  UINT32      XhciMmioBase;
  UINT32      Data;
  UINT8      *Class;

  Status = EFI_SUCCESS;
  if (mUsbInit.UsbHostHandle == NULL) {
    // Verify XHCI controller
    PcieAddress = UsbHcPciBase;
    Data  = MmioRead32 (PcieAddress + PCI_REVISION_ID_OFFSET) >> 8;
    Class = (UINT8 *)&Data;
    if (((Class[0] == PCI_IF_XHCI) && (Class[1] == PCI_CLASS_SERIAL_USB) &&
        (Class[2] == PCI_CLASS_SERIAL))) {
      // Enable XHCI controller
      MmioOr8 (PcieAddress + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
      XhciMmioBase = MmioRead32 (PcieAddress + PCI_BASE_ADDRESSREG_OFFSET) & ~0xF;

      Status = UsbInitCtrl (XhciMmioBase, &mUsbInit.UsbHostHandle);
      DEBUG ((DEBUG_INFO, "Init USB XHCI - %r\n", Status));
      if (!EFI_ERROR (Status)) {
        // Enumerate USB bus to register all devices
        Status = UsbEnumBus (mUsbInit.UsbHostHandle, RegisterUsbDevice);
        DEBUG ((DEBUG_INFO, "Enumerate Bus - %r\n", Status));
        if (!EFI_ERROR (Status)) {
          DEBUG ((DEBUG_INFO, "Found %d USB devices on bus\n", mUsbInit.UsbIoCount));
        }
      }
    } else {
      Status = EFI_UNSUPPORTED;
    }
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
  if (mUsbInit.UsbHostHandle == NULL) {
    return EFI_NOT_AVAILABLE_YET;
  }

  if (UsbIoArray != NULL) {
    *UsbIoArray = (PEI_USB_IO_PPI *)mUsbInit.UsbIoArray;
  }

  if (UsbIoCount != NULL) {
    *UsbIoCount = mUsbInit.UsbIoCount;
  }

  return EFI_SUCCESS;
}
