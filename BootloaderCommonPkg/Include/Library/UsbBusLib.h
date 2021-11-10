/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __USB_BUS_LIB_H__
#define __USB_BUS_LIB_H__

#include <Ppi/UsbIo.h>

typedef EFI_STATUS (EFIAPI *USB_IO_CALLBACK) (PEI_USB_IO_PPI *UsbIoPpi);

/**
  Enumerate devices on the USB bus.
  It will call the callback function for each device enumerated.

  @param  UsbHostHandle     USB host controller handle.
  @param  UsbIoCb           Callback function for each USB device detected.

  @retval EFI_SUCCESS       The usb is enumerated successfully.
  @retval Others            Other failure occurs.

**/
EFI_STATUS
EFIAPI
UsbEnumBus (
  EFI_HANDLE                             UsbHostHandle,
  USB_IO_CALLBACK                        UsbIoCb
  );

/**
  Retrieve the concatenated manufacturer and product string for a given USB device.

  @param[in]  UsbIo for a USB device.

  @retval     The constant manufacturer and product string.
              NULL if UsbIo is not valid.
**/
CONST
CHAR16 *
EFIAPI
GetUsbDeviceNameString (
  IN PEI_USB_IO_PPI  *UsbIo
  );

#endif
