/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __USB_BUS_LIB_H__
#define __USB_BUS_LIB_H__

#include <Ppi/UsbIo.h>

typedef EFI_STATUS (*USB_IO_CALLBACK) (PEI_USB_IO_PPI *UsbIoPpi);

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

#endif
