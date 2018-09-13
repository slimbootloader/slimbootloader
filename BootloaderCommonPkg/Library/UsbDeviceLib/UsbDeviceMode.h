/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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

