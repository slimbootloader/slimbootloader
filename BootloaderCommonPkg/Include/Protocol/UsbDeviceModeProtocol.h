/** @file
  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#ifndef _USB_DEVICE_MODE_PROTOCOL_H_
#define _USB_DEVICE_MODE_PROTOCOL_H_

#include <Library/UsbDeviceLib.h>

///
/// UsbDeviceMode Protocol GUID.
///
#define EFI_USB_DEVICE_MODE_PROTOCOL_GUID \
  {0xC9923F7E, 0x1746, 0x4802, { 0x86, 0x2e, 0x1, 0x1c, 0x2c, 0x2d, 0x9d, 0x86 } }

typedef struct _EFI_USB_DEVICE_MODE_PROTOCOL EFI_USB_DEVICE_MODE_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_USB_DEVICE_MODE_INIT_XDCI) (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_DEVICE_MODE_CONNECT) (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_DEVICE_MODE_DISCONNECT) (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_DEVICE_EP_TX_DATA) (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This,
  IN USB_DEVICE_IO_REQ                          *IoRequest
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_DEVICE_EP_RX_DATA) (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This,
  IN USB_DEVICE_IO_REQ                          *IoRequest
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_DEVICE_MODE_BIND) (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This,
  IN USB_DEVICE_OBJ                             *UsbdDevObj
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_DEVICE_MODE_UNBIND) (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_DEVICE_MODE_STOP) (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_DEVICE_MODE_RUN) (
  IN EFI_USB_DEVICE_MODE_PROTOCOL               *This,
  IN UINT32                                     TimeoutMs
  );

///
/// Usb Device Mode Protocol Structure.
///
struct _EFI_USB_DEVICE_MODE_PROTOCOL {
  EFI_USB_DEVICE_MODE_INIT_XDCI            InitXdci;
  EFI_USB_DEVICE_MODE_CONNECT              Connect;
  EFI_USB_DEVICE_MODE_DISCONNECT           DisConnect;
  EFI_USB_DEVICE_EP_TX_DATA                EpTxData;
  EFI_USB_DEVICE_EP_RX_DATA                EpRxData;
  EFI_USB_DEVICE_MODE_BIND                 Bind;
  EFI_USB_DEVICE_MODE_UNBIND               UnBind;
  EFI_USB_DEVICE_MODE_RUN                  Run;
  EFI_USB_DEVICE_MODE_STOP                 Stop;
};

extern EFI_GUID gEfiUsbDeviceModeProtocolGuid;

#endif

