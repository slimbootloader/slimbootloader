/** @file

  Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
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
  UINT32  DebugConsent : 3;
  UINT32  Rsvd         : 28;
} PLAT_FEATURES;

typedef struct {
  BOOT_GUARD_INFO     BtGuardInfo;
  PLAT_FEATURES       PlatformFeatures;
} PLATFORM_DATA;

#define PLAT_DATA                         ((PLATFORM_DATA *)GetPlatformDataPtr ())
#define PLAT_FEAT                         (PLAT_DATA->PlatformFeatures)
#define DEBUG_CONSENT_FEATURE_ENABLED()   (UINT8) (PLAT_FEAT.DebugConsent)

#endif /* __PLATFORM_DATA_H__ */
