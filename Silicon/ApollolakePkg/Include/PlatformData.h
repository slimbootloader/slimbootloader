/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PLATFORM_DATA_H__
#define __PLATFORM_DATA_H__

#include <Library/BootGuardLib20.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/VtdPmrLib.h>

typedef enum {
  BootModeTargetNormal    = 0,
  BootModeTargetRecovery  = 1,
  BootModeTargetProvision = 2,
  BootModeTargetMax
} BOOT_MODE_TARGET;

typedef enum {
  BootModeActionBoot  = 0,
  BootModeActionShell = 1,
  BootModeActionHalt  = 2,
  BootModeActionReset = 3,
  BootModeActionMax
} BOOT_MODE_ACTION;

typedef enum {
  PlatformSkuPremium  = 0,  // 4C @ 1.9 / 2.4 GHz
  PlatformSkuHigh     = 1,  // 4C @ 1.6 / 2.0 GHz
  PlatformSkuMid      = 2,  // 4C @ 1.6 / 1.8 GHz
  PlatformSkuLow      = 3,  // 2C @ 1.3 / 1.8 GHz
  PlatformSkuMax
} PLATFORM_SKU_ID;

typedef enum {
  MemorySkuMin      = 8,
  MemorySku1x4Gb    = MemorySkuMin,
  MemorySku2x4Gb    = 9,
  MemorySku4x4Gb    = 10,
  MemorySku4x12Gb   = 11,
  MemorySku2x8Gb    = 12,
  MemorySku4x8Gb    = 13,
  MemorySku4x16Gb_2 = 14,
  MemorySku4x16Gb   = 15,
  MemorySkuMax
} MEMORY_SKU_ID;

typedef enum {
  FabIdMin    = 1,
  FabIdE      = FabIdMin,
  FabIdD      = 3,
  FabIdC2     = 4,
  FabIdC1     = 5,
  FabIdB      = 6,
  FabIdA      = 7,
  FabIdMax
} FAB_ID;

typedef union {
  struct {
    UINT16  Target        : 5;
    UINT16  DoMrcTraining : 1;
    UINT16  SaveMrcData   : 1;
    UINT16  DoFlashUpdate : 1;
    UINT16  Silent        : 1;
    UINT16  Rsvd          : 1;
    UINT16  Action        : 2;
    UINT16  DipSw         : 4;
  } Bits;
  UINT16    Uint16;
} BOOT_MODE_OPTION;

typedef union {
  struct {
    UINT16  MemSkuId      : 4;
    UINT16  FabId         : 4;
    UINT16  PlatformSkuId : 4;
    UINT16  Rsvd          : 4;
  } Bits;
  UINT16  ModuleId;
} MODULE_ID_INFO;

#define MRC_PARAM_SIGNATURE  SIGNATURE_32 ('$', 'M', 'P', 'A')
#define MRC_VAR_SIGNATURE    SIGNATURE_32 ('$', 'M', 'V', 'A')
#define MMC_TUNING_SIGNATURE SIGNATURE_32 ('$', 'M', 'T', 'A')
#define MRC_VAR_LENGTH       0x18
#define MRC_VAR_SLOT_LENGTH  0x20

typedef struct {
  UINT32  Signature;
  UINT32  Length;
  UINT32  Crc[3];
} MRC_PARAM_HDR;

typedef struct {
  UINT32  Signature;
  UINT32  HdrLen;
  UINT32  SlotLen;
  UINT32  SlotNum;
  UINT8   SlotMap[16];
} MRC_VAR_HDR;

typedef struct {
  UINT32  Signature;
  UINT32  Length;
} MMC_TUNING_HDR;

typedef struct {
  UINT32  Vt          : 1;
  UINT32  eMMCTuning  : 1;
  UINT32  DciDebug    : 1;
  UINT32  Rsvd        : 29;
} PLAT_FEATURES;

typedef struct {
  BOOT_MODE_OPTION    BootModeOption;
  MODULE_ID_INFO      ModuleIdInfo;
  UINT32              RmrrUsbAddress;
  MRC_PARAM_HDR       MrcParamHdr;
  UINT8               MrcVarData[MRC_VAR_LENGTH];
  BOOT_GUARD_INFO     BtGuardInfo;
  VOID               *LoaderSeedList;
  PLAT_FEATURES       PlatformFeatures;
  VTD_INFO            VtdInfo;
} PLATFORM_DATA;

typedef struct {
  UINT8               PlatformId : 5;
  UINT8               Reserved1  : 3;
  UINT8               DebugUart  : 2;
  UINT8               Reserved2  : 6;
  UINT8               Reserved3;
  UINT8               Marker;
} STITCH_DATA;

#define PLAT_DATA                         ((PLATFORM_DATA *)GetPlatformDataPtr ())
#define PLAT_FEAT                         (PLAT_DATA->PlatformFeatures)
#define VT_ENABLED()                      (FeaturePcdGet (PcdVtdEnabled) && ACPI_FEATURE_ENABLED() && PLAT_FEAT.Vt)
#define EMMC_TUNE_FEATURE_ENABLED()       (PLAT_FEAT.eMMCTuning)
#define DCI_DBG_FEATURE_ENABLED()         (PLAT_FEAT.DciDebug)


#endif /* __PLATFORM_DATA_H__ */
