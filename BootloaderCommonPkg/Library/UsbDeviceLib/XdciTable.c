/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

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

