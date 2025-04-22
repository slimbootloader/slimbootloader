/**@file


  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __GLOBAL_NVS_AREA_DEF_H__
#define __GLOBAL_NVS_AREA_DEF_H__

#include <PlatformBase.h>
#include <PlatformNvsAreaDef.h>
#include <EcNvsAreaDef.h>
#include <DptfNvsAreaDef.h>
#include <PchNvsAreaDef.h>
#include <VmdNvsAreaDef.h>
#include <CpuNvsAreaDef.h>
#include <SaNvsAreaDef.h>

#pragma pack (push,1)
typedef struct {
  PLATFORM_NVS_AREA     PlatformNvs;
  PCH_NVS_AREA          PchNvs;
  CPU_NVS_AREA          CpuNvs;
  SYSTEM_AGENT_NVS_AREA SaNvs;
  EC_NVS_AREA           EcNvs;
  DPTF_NVS_AREA         DptfNvs;
  VMD_NVS_AREA          VmdNvs;
} GLOBAL_NVS_AREA;
#pragma pack(pop)

#endif /* __GLOBAL_NVS_AREA_DEF_H__ */
