/** @file
  USB keyboard library header file.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef  __USB_KB_LIB_INTERNAL_H__
#define  __USB_KB_LIB_INTERNAL_H__

#include <PiPei.h>
#include <IndustryStandard/Pci.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/UsbInitLib.h>

#define USB_KB_DEVICE_SIG      SIGNATURE_32 ('U', 'S', 'K', 'B')

#define QUEUE_MAX_COUNT        64
#define KEY_REPEAT_DELAY       10

#define USBKBD_VALID_KEYCODE(Key) ((UINT8) (Key) > 3)

typedef struct {
  UINTN                                      Front;
  UINTN                                      Rear;
  CHAR8                                      Buffer[QUEUE_MAX_COUNT];
} SIMPLE_QUEUE;

typedef struct {
  UINT8 NumLock : 1;
  UINT8 CapsLock : 1;
  UINT8 ScrollLock : 1;
  UINT8 Resrvd : 5;
} LED_MAP;

typedef struct {
  UINT32                              Signature;
  PEI_USB_IO_PPI                     *UsbIo;
  EFI_USB_INTERFACE_DESCRIPTOR        InterfaceDescriptor;
  EFI_USB_ENDPOINT_DESCRIPTOR         EndpointDescriptor;
  BOOLEAN                             NumLockOn;
  BOOLEAN                             CapsOn;
  BOOLEAN                             ScrollOn;
  SIMPLE_QUEUE                        Queue;
  UINT32                              RepeatCounter;
  CHAR16                              RepeatChar;
  UINT8                               RepeatKey;
  UINT8                               LastKeyCodeArray[8];
  UINT32                              TimeStampFreqKhz;
  UINT64                              LastTransferTimeStamp;
} USB_KB_DEV;

#endif
