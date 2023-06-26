/** @file
  This file contains CPU Gnvs Struct specific to processor

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _POWER_MGMT_NVS_STRUCT_H_
#define _POWER_MGMT_NVS_STRUCT_H_

#include <CpuNvsAreaDef.h>

//
// Processor Power Management GlobalNvs Revisions
//
#define CPU_NVS_AREA_REVISION 1


typedef struct _CPU_NVS_AREA_CONFIG {
  CPU_NVS_AREA *Area;
} CPU_NVS_AREA_CONFIG;

typedef struct _FVID_HEADER {
  UINT32 Stepping;    ///< Matches value returned by CPUID function 1
  UINT16 MaxBusRatio; ///< Matches BUS_RATIO_MAX field in PERF_STS_MSR
  UINT16 EistStates;  ///< Number of states of FVID (N)
} FVID_HEADER;

typedef struct _FVID_STATE {
  UINT32 State;           ///< State Number (0 - N-1)
  UINT16 BusRatio;        ///< BUS_RATIO_SEL value to be written to PERF_CTL
  UINT32 Power;           ///< Typical power consumed by CPU in this state
  UINT32 Limit16State;    ///< State Number (0 - N-1) with limit 16 P-states
  UINT16 Limit16BusRatio; ///< BUS_RATIO_SEL value to be written to PERF_CTL with limit 16 P-states
  UINT32 Limit16Power;    ///< Typical power consumed by CPU in this state with limit 16 P-states
} FVID_STATE;

typedef union _FVID_TABLE {
  FVID_HEADER FvidHeader;
  FVID_STATE  FvidState;
  UINT64      FvidData;
} FVID_TABLE;

#endif
