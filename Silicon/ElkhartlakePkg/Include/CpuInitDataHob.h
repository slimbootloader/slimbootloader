/** @file
  Struct and GUID definitions for CpuInitDataHob.

  Copyright (c) 2013 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _CPU_INIT_DATA_HOB_H_
#define _CPU_INIT_DATA_HOB_H_

#define TDP_MAX_LEVEL 3
///
/// CPU Configuration Structure passed from PEI to DXE phase
///
typedef struct {
  UINT32 HdcControl                : 2;
  UINT32 AutoThermalReporting      : 1;
  UINT32 Hwp                       : 2;
  UINT32 ConfigTdpBios             : 1;
  UINT32 EnableItbmPeriodicSmi     : 1;     ///< Enable for OC capable systems with Intel Turbo Boost Max Technology 3.0.
  UINT32 RsvdBits                  :25;
  UINT8  Rsvd[4];
} CPU_CONFIG_DATA;

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
  UINT8                  EnableItbmDriver;                        /// Enable/Disable Intel Turbo Boost Max Technology 3.0 Driver.
  EFI_PHYSICAL_ADDRESS   CpuConfigData;                           /// CPU RC Config for DXE consumption
  EFI_PHYSICAL_ADDRESS   FvidTable;                               /// FVID Table
} CPU_INIT_DATA_HOB;

#endif
