/** @file
  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef  __USB_INIT_LIB_H__
#define  __USB_INIT_LIB_H__

#include <Ppi/UsbIo.h>

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
  );


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
  );

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
  );

#endif
