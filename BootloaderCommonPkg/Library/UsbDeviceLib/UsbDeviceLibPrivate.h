/** @file
Copyright (c) 2009 - 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _USB_DEVICE_LIB_PRIVATE_H_
#define _USB_DEVICE_LIB_PRIVATE_H_

#include <Library/TimerLib.h>

//
// Dummy functions to replace the timer related function calls in library
//
#define BS_STALL             MicroSecondDelay
#define BS_RAISE_TPL(x)       0
#define BS_RESTORE_TPL(x)
#define BS_CREAT_EVENT(a,b,c,d,e)  EFI_SUCCESS
#define BS_CLOSE_EVENT(...)   EFI_SUCCESS
#define BS_SET_TIMER(...)     EFI_SUCCESS

#define USB_ADDRESS_TABLE_SIZE      16  //4

//
// Endpoint Zero
//
#define USB_EP0_MAX_PKT_SIZE_HS     0x40 // High Speed mode is explicitly set as 64 bytes
#define USB_EP0_MAX_PKT_SIZE_SS     0x9  // Must be 0x9 (2^9 = 512 Bytes) in SuperSpeed mode
#define USB_EPO_MAX_PKT_SIZE_ALL    512  // Overall max bytes for any type

//
// Bulk Endpoints
//
#define USB_BULK_EP_PKT_SIZE_HS     0x200 // Bulk-Endpoint HighSpeed
#define USB_BULK_EP_PKT_SIZE_SS     0x400 // Bulk-Endpoint SuperSpeed
#define USB_BULK_EP_PKT_SIZE_MAX    USB_BULK_EP_PKT_SIZE_SS

//
// Transmit Direction Bits
//
#define USB_ENDPOINT_DIR_OUT                 0x00

//
// Endpoint Companion Bulk Attributes
//
#define USB_EP_BULK_BM_ATTR_MASK    0x1F

//
// Configuration Modifiers (Attributes)
//
#define USB_BM_ATTR_RESERVED        0x80
#define USB_BM_ATTR_SELF_POWERED    0x40
#define USB_BM_ATTR_REMOTE_WAKE     0X20

//
// USB BCD version
//
#define USB_BCD_VERSION_LS          0x0110
#define USB_BCD_VERSION_HS          0x0200
#define USB_BCD_VERSION_SS          0x0300

//
// Device RequestType Flags
//
#define USB_RT_TX_DIR_H_TO_D        (0x0)       // Tx direction Host to Device
#define USB_RT_TX_DIR_D_TO_H        (0x1 << 7)  // Tx direction Device to Host
#define USB_RT_TX_DIR_MASK          (0x80)

//
// USB request type
//
#define USB_REQ_TYPE_MASK           (0x60)

//
// Usb control transfer target
//
#define USB_TARGET_MASK             (0x1F)

//
// Device GetStatus bits
//
#define USB_STATUS_SELFPOWERED      (0x01)
#define USB_STATUS_REMOTEWAKEUP     (0x02)

//
// USB Device class identifiers
//
#define USB_DEVICE_MS_CLASS         (0x08)
#define USB_DEVICE_VENDOR_CLASS     (0xFF)

//
// USB Descriptor types
//
#define USB_DESC_TYPE_BOS                    0x0F
#define USB_DESC_TYPE_DEVICE_CAPABILITY      0x10
#define USB_DESC_TYPE_SS_ENDPOINT_COMPANION  0x30


/**
  This function performs board specific device initialization.

  @param[in,out] USB_DEVICE_PLATFORM_INFO  Platform information specific to USB device.

  @retval EFI_SUCCESS        Successfully polled the value.
  @retval EFI_TIMEOUT        Timeout while polling the value.
**/
EFI_STATUS
EFIAPI
UsbDeviceBoardLibInit (
  IN OUT USB_DEVICE_PLATFORM_INFO   *UsbInfo
  );

#endif
