/** @file

  Copyright (c) 2018 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PLATFORM_DATA_H__
#define __PLATFORM_DATA_H__

#include <Library/BootGuardLib.h>
#include <Library/BootloaderCoreLib.h>

typedef struct {
  UINT8               PlatformId : 5;
  UINT8               Reserved1  : 3;
  UINT8               DebugUart;
  UINT8               Reserved3;
  UINT8               Marker;
} STITCH_DATA;

typedef struct {
  UINT32  VtdEnable    : 1;
  UINT32  S0ixEnable   : 1;
  UINT32  Rsvd         : 30;
} PLAT_FEATURES;

typedef struct {
  BOOT_GUARD_INFO     BtGuardInfo;
  PLAT_FEATURES       PlatformFeatures;
} PLATFORM_DATA;

#define PLAT_DATA                         ((PLATFORM_DATA *)GetPlatformDataPtr ())
#define PLAT_FEAT                         (PLAT_DATA->PlatformFeatures)
#define S0IX_STATUS()                     (BOOLEAN) (PLAT_FEAT.S0ixEnable)

#endif /* __PLATFORM_DATA_H__ */
