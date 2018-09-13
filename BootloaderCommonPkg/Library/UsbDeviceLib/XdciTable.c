/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/UsbDeviceLib.h>
#include "UsbDeviceLibPrivate.h"
#include "XdciCommon.h"
#include "XdciDevice.h"
#include "XdciInterface.h"
#include "XdciDWC.h"
#include "UsbDeviceMode.h"

STATIC CONST struct UsbDeviceCoreDriver CoreDriverTbl[USB_CORE_ID_MAX] = {
  DwcXdciCoreInit,
  DwcXdciCoreDeinit,
  DwcXdciCoreRegisterCallback,
  DwcXdciCoreUnregisterCallback,
  DwcXdciCoreIsrRoutine,
  DwcXdciCoreIsrRoutineTimerBased,
  DwcXdciCoreConnect,
  DwcXdciCoreDisconnect,
  DwcXdciCoreGetSpeed,
  DwcXdciCoreSetAddress,
  DwcXdciCoreSetConfig,
  DwcXdciSetLinkState,
  DwcXdciInitEp,
  DwcXdciEpEnable,
  DwcXdciEpDisable,
  DwcXdciEpStall,
  DwcXdciEpClearStall,
  DwcXdciEpSetNrdy,
  DwcXdciEp0ReceiveSetupPkt,
  DwcXdciEp0ReceiveStatusPkt,
  DwcXdciEp0SendStatusPkt,
  DwcXdciEpTxData,
  DwcXdciEpRxData,
  DwcXdciEpCancelTransfer
};

CONST struct  UsbDeviceCoreDriver *
UsbDeviceGetCoreDriver (
  USB_CONTROLLER_ID id
  )
{
  if (id >= USB_CORE_ID_MAX) {
    return NULL;
  }

  return &CoreDriverTbl[id];
}

