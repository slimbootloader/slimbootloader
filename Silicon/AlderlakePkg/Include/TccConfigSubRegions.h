/** @file
  TCC Config Sub Region's Struct Data

  Copyright (c) 2019 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _TCC_CONFIG_SUB_REGION_H_
#define _TCC_CONFIG_SUB_REGION_H_


#pragma pack(1)

///
/// TCC BIOS Configuration
///
typedef struct {
  UINT32 Version;
  UINT32 Size;
  UINT8  Sstates;        ///< S states
  UINT8  Cstates;        ///< C States
  UINT8  Pstates;        ///< P States
  UINT8  Dstates;        ///< D States
  UINT8  CoreRapl;       ///< Core RAPL
  UINT8  MemoryRapl;     ///< Memory RAPL
  UINT8  Turbo;          ///< Turbo Mode
  UINT8  SaGv;           ///< SaGv
  UINT8  MemPowerDown;   ///< Memory Power Down
  UINT8  DisPgCloseIdle; ///< Disable Page Close Idle
  UINT8  FabricPm;       ///< Fabric PM
  UINT8  GtRstRc6;       ///< RC6
  UINT8  CpuPcieAspm;    ///< CPU PCIE ASPM
  UINT8  PchPcieAspm;    ///< PCH PCIE ASPM
  UINT8  PcieClkGate;    ///< PCIE Clock Gate
  UINT8  PchPwrClkGate;  ///< PCh Power Clock gate
  UINT8  DmiAspm;        ///< DMI ASPM
  UINT8  CpuPcieRpL1;    ///< CPU PCIE Root Port L1
  UINT8  PchPcieRpL1;    ///< PCH PCIE Root Port L1
  UINT8  HyperThreading; ///< Hyper Threading
  UINT8  FastGv;         ///< Fast GV
  UINT8  HwpEn;          ///< speed shift
} BIOS_SETTINGS;


#define TCC_STREAM_CONFIGURATION_VERSION   2
///
/// TCC Stream Configuration
///
typedef struct  {
  UINT32 Version;                                  ///< determines Version of the struct
  BIOS_SETTINGS        BiosSettings;               ///< BIOS Settings
} TCC_STREAM_CONFIGURATION;

#pragma pack()

#endif
