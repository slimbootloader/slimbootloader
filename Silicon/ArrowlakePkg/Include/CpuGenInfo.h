/** @file
  Header file for Cpu Information

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _CPU_GEN_INFO_H_
#define _CPU_GEN_INFO_H_

#include <CpuRegs.h>

//
// Processor Definitions
//
#define CPUID_FULL_FAMILY_MODEL_METEORLAKE_MOBILE   0x000A06A0
#define CPUID_FULL_FAMILY_MODEL_METEORLAKE_DT_HALO  0x000A06C0
#define CPUID_FULL_FAMILY_MODEL_ARROWLAKE_MOBILE_U  0x000B0650
#define CPUID_FULL_FAMILY_MODEL_ARROWLAKE_MOBILE_H  0x000C0650
#define CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_HALO   0x000C0660    // For ARL S and HX Segments
#define CPUID_FULL_FAMILY_MODEL_ARROWLAKE_DT_20A    0x000C06A0    // For ARL S and HX Segments 20A

//
// Processor Definitions
//
#define CPUID_FULL_STEPPING                        0x0000000F
#define CPUID_FULL_FAMILY_MODEL                    0x0FFF0FF0

#endif
