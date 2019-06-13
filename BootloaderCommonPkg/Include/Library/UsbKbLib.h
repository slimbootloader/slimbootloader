/** @file
  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef  __USB_KB_LIB_H__
#define  __USB_KB_LIB_H__

/**
  The function will initialize USB keyboard device.

  Based on UsbHcPciBase, this function will initialize USB host controller, allocate
  necessary resources, and enumarate all the USB bus.

  @param[in]  UsbHcPciBase       USB Host Controller's PCI ConfigSpace Base address

  @retval EFI_SUCCESS            The driver is successfully initialized.
  @retval EFI_NOT_FOUND          Can't find any USB block devices for boot.

**/
EFI_STATUS
EFIAPI
InitUsbKeyBoard (
  IN  UINTN                     UsbHcPciBase
  );


/**
  Polls a keyboard to see if there is any data waiting to be read.

  @retval TRUE             Data is waiting to be read from the device.
  @retval FALSE            There is no data waiting to be read from the device.

**/
BOOLEAN
EFIAPI
KeyboardPoll (
  VOID
  );


/**
  Reads data from a USB keyboard into a buffer.
  This function will wait till specified number of bytes are filled.

  @param  Buffer           Pointer to the data buffer to store the data read from the device.
  @param  NumberOfBytes    Number of bytes to read from the device.

  @retval                  The number of bytes read from the device.

**/
UINTN
EFIAPI
KeyboardRead (
  OUT UINT8   *Buffer,
  IN  UINTN   NumberOfBytes
  );


#endif
