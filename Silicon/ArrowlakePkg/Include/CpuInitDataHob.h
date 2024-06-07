/** @file
  Struct and GUID definitions for CpuInitDataHob.


  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _CPU_INIT_DATA_HOB_H_
#define _CPU_INIT_DATA_HOB_H_

#include <PowerMgmtNvsStruct.h>
#include <CpuPowerMgmt.h>

#define TDP_MAX_LEVEL 3
#define CPU_MAX_ATOM_CLUSTERS  4
#define TPSS_FVID_MAX_STATES   40

extern EFI_GUID gCpuInitDataHobGuid;

///
/// This HOB is used to pass only the required information from PEI for DXE consumption.
///
typedef struct {
  UINT32                 PpmFlags;                                /// PPM Flags Values.
  UINT8                  CtdpLevelsSupported;                     /// ConfigTdp Number Of Levels.
  UINT8                  ApplyConfigTdp;                          /// Enable Configurable TDP
  UINT8                  ConfigurablePpc;                         /// Boot Mode vlues for _PPC
  UINT8                  ConfigTdpLevel;                          /// Configuration for boot TDP selection; <b>0: TDP Nominal</b>; 1: TDP level1; 2: TDP level2.
  UINT8                  CtdpTar[TDP_MAX_LEVEL];                  /// CTDP Levels TAR
  UINT8                  ConfigTdpBootModeIndex;                  /// CTDP Boot Mode Index
  UINT16                 CtdpPowerLimit1[TDP_MAX_LEVEL];          /// CTDP Levels Power Limit1
  UINT16                 CtdpPowerLimit2[TDP_MAX_LEVEL];          /// CTDP Levels Power Limit2
  UINT8                  CtdpPowerLimitWindow[TDP_MAX_LEVEL];     /// CTDP Levels Power Limit1 Time Window
  UINT8                  CtdpCtc[TDP_MAX_LEVEL];                  /// CTDP Levels CTC
  UINT8                  CtdpPpc[TDP_MAX_LEVEL];                  /// CTDP Levels PPC
  UINT8                  EnableAllThermalFunctions;
  BOOLEAN                CtdpSupport;                             /// CTDP Support
  FVID_TABLE             FvidTable[TPSS_FVID_MAX_STATES+1];       /// FVID Table
} CPU_INIT_DATA_HOB;

///
/// This HOB is used to pass only the required information from PEI for DXE consumption.
///
typedef struct {
  UINT8                         Revision;
  UINT8                         EnableRp;                                         /// Enable/Disable Resource Priority Feature
} CPU_POWER_MANAGEMENT_DATA_HOB;
#endif
