/** @file
  Struct and GUID definitions for CpuInitDataHob.


  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _CPU_INIT_DATA_HOB_H_
#define _CPU_INIT_DATA_HOB_H_

#include <PowerMgmtNvsStruct.h>
#include <CpuPowerMgmt.h>

#define TDP_MAX_LEVEL 3
#define CPU_OC_MAX_VF_POINTS   0xF
#define CPU_MAX_BIG_CORES      8
#define CPU_MAX_ATOM_CLUSTERS  4

extern EFI_GUID gCpuInitDataHobGuid;

///
/// This HOB is used to pass only the required information from PEI for DXE consumption.
///
typedef struct {
  UINT32                 PpmFlags;                                /// PPM Flags Values.
  UINT8                  CtdpLevelsSupported;                     /// ConfigTdp Number Of Levels.
  UINT8                  LowestMaxPerf;                           /// Max ratio of the slowest core.
  UINT8                  ConfigurablePpc;                         /// Boot Mode vlues for _PPC
  UINT8                  CtdpTar[TDP_MAX_LEVEL];                  /// CTDP Levels TAR
  UINT8                  ConfigTdpBootModeIndex;                  /// CTDP Boot Mode Index
  UINT16                 CtdpPowerLimit1[TDP_MAX_LEVEL];          /// CTDP Levels Power Limit1
  UINT16                 CtdpPowerLimit2[TDP_MAX_LEVEL];          /// CTDP Levels Power Limit2
  UINT8                  CtdpPowerLimitWindow[TDP_MAX_LEVEL];     /// CTDP Levels Power Limit1 Time Window
  UINT8                  CtdpCtc[TDP_MAX_LEVEL];                  /// CTDP Levels CTC
  UINT8                  CtdpPpc[TDP_MAX_LEVEL];                  /// CTDP Levels PPC
  UINT8                  EnableItbm;                              /// Enable/Disable Intel Turbo Boost Max Technology 3.0.
  UINT8                  EnableItbmDriver;                        /// @Deprecated: Intel Turbo Boost Max Technology 3.0 Driver doesn't support for TGL.
  UINT8                  EnableAllThermalFunctions;
  UINT8                  PerCoreRatioOverride;                    ///< Enable or disable Per Core PState OC supported.

  FVID_TABLE   FvidTable[LPSS_FVID_MAX_STATES+1];                               /// FVID Table
  /**
  Number of supported Core Voltage & Frequency Point Offset.
  **/
  UINT8                  CoreVfPointCount;
  /**
  Array for the each selected Core VF Point to display the ration.
  **/
  UINT8                  CoreVfPointRatio[CPU_OC_MAX_VF_POINTS];
  /**
  Array used to specific max ratio applied to each selected Core.
  **/
  UINT8                  PerCoreRatio[CPU_MAX_BIG_CORES];
  /**
  Number of supported Ring Voltage & Frequency Point Offset.
  **/
  UINT8                  RingVfPointCount;
  /**
  Array for the each selected Ring VF Point to display the ration.
  **/
  UINT8                  RingVfPointRatio[CPU_OC_MAX_VF_POINTS];
  /**
  Array used to specific max ratio applied to each selected Atom Cluster.
  **/
  UINT8                  PerAtomClusterRatio[CPU_MAX_ATOM_CLUSTERS];
} CPU_INIT_DATA_HOB;
#endif
