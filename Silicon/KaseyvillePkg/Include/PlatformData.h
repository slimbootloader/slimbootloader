/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PLATFORM_DATA_H__
#define __PLATFORM_DATA_H__

#include <Library/BootGuardLib.h>
#include <Library/BootloaderCoreLib.h>

#define BMC_UART_PORT   0xFF
#define S3M_UART_PORT   0x02

typedef struct {
  UINT8               PlatformId : 5;
  UINT8               Reserved1  : 3;
  UINT8               DebugUart;
  UINT8               Reserved3;
  UINT8               Marker;
} STITCH_DATA;

typedef struct {
  UINT32  VtdEnable       : 1;
  UINT32  SgxSblExec      : 1;
  UINT32  SgxSblState     : 2;
  UINT32  HyperThreading  : 1;  // Will be updated at runtime
  UINT32  BMCPresent      : 1;  // Updated by checking if BMC is present during runtime
  UINT32  Rsvd            : 26;
} PLAT_FEATURES;

typedef struct {
  BOOT_GUARD_INFO     BtGuardInfo;
  PLAT_FEATURES       PlatformFeatures;
} PLATFORM_DATA_BL;

#endif /* __PLATFORM_DATA_H__ */
