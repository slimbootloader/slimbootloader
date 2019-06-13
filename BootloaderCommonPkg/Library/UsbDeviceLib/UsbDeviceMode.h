/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef _USB_DEVICE_MODE_H_
#define _USB_DEVICE_MODE_H_

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/UsbDeviceLib.h>
#include <Protocol/UsbDeviceModeProtocol.h>
#include "UsbDeviceLibPrivate.h"
#include "XdciCommon.h"
#include "XdciDevice.h"


/**
  Handles Setup device requests. Standard requests are immediately
  handled here, and any Class/Vendor specific requests are forwarded
  to the class driver.

  @param CtrlRequest  Pointer to a device request

  @return EFI_SUCCESS if request successfully handled, FALSE otherwise

**/
EFI_STATUS
UsbdSetupHdlr (
  IN EFI_USB_DEVICE_REQUEST    *CtrlRequest
  );

extern EFI_USB_DEVICE_MODE_PROTOCOL  mUsbDeviceModeProtocol;

#endif

