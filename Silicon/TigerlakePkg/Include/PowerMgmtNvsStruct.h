/** @file
  This file contains CPU Gnvs Struct specific to processor

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>
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

#endif
