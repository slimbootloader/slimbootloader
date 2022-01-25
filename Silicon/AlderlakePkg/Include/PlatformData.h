/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PLATFORM_DATA_H__
#define __PLATFORM_DATA_H__

#include <Library/BootGuardLib.h>
#include <Library/BootloaderCoreLib.h>

#define VTD_BASEADDRESS_0       0xFED90000
#define VTD_BASEADDRESS_1       0xFED92000
#define VTD_BASEADDRESS_2       0xFED91000

typedef struct {
  UINT8               PlatformId : 5;
  UINT8               Reserved1  : 3;
  UINT8               DebugUart;
  UINT8               Reserved3;
  UINT8               Marker;
} STITCH_DATA;

typedef struct {
  UINT32  TccDsoTuning : 1;
  UINT32  VtdEnable    : 1;
  UINT32  Rsvd         : 30;
  UINT8   TccRtd3Support;
  UINT8   TccLowPowerS0Idle;
} PLAT_FEATURES;

typedef struct {
  BOOT_GUARD_INFO     BtGuardInfo;
  PLAT_FEATURES       PlatformFeatures;
} PLATFORM_DATA;

#endif /* __PLATFORM_DATA_H__ */
