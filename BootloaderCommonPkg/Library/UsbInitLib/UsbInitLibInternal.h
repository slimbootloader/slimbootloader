/** @file
  USB init library header file.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef  __USB_INIT_LIB_INTERNAL_H__
#define  __USB_INIT_LIB_INTERNAL_H__


#include <PiPei.h>
#include <IndustryStandard/Pci.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Library/XhciLib.h>
#include <Library/UsbBusLib.h>

#define  MAX_USB_DEVICE_NUMBER  32

typedef struct {
  EFI_HANDLE                      UsbHostHandle;
  UINT32                          UsbIoCount;
  PEI_USB_IO_PPI                 *UsbIoArray[MAX_USB_DEVICE_NUMBER];
} USB_INIT_INSTANCE;

/**
  This funciton de-allocate memory allocated for USB BOT devices.

  @retval EFI_SUCCESS            This routinue alwasy return success.

**/
EFI_STATUS
EFIAPI
UsbDeInitBot (
  VOID
);

/**
  Free USB device memory.

  @param  UsbIo     UsbIo instance associated with the device.

  @retval EFI_SUCCESS            The usb device memory was de-allocated successfully.
  @retval EFI_INVALID_PARAMETER  Invalid UsbIo instance.

**/
EFI_STATUS
EFIAPI
UsbDeinitDevice (
  IN VOID     *UsbIo
  );

/**
  USB deinitialization interface.

  @param UsbHostHandle   USB host controller handle to deinitalize.

  @retval EFI_SUCCESS              USB host controller was deinitialized successfully.
  @retval EFI_INVALID_PARAMETER    USB host controller handle is invalid.
**/
EFI_STATUS
EFIAPI
UsbDeinitCtrl (
  IN  EFI_HANDLE      UsbHostHandle
  );

#endif
