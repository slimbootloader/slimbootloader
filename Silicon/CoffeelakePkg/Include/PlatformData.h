/** @file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PLATFORM_DATA_H__
#define __PLATFORM_DATA_H__

#include <Library/BootGuardLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/VtdPmrLib.h>

typedef struct {
  UINT8               PlatformId : 5;
  UINT8               Reserved1  : 3;
  UINT8               DebugUart;
  UINT8               Reserved3;
  UINT8               Marker;
} STITCH_DATA;

typedef struct {
  BOOT_GUARD_INFO     BtGuardInfo;
  VTD_INFO            VtdInfo;
} PLATFORM_DATA;

#endif /* __PLATFORM_DATA_H__ */
