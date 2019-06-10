/**@file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __GLOBAL_NVS_AREA_DEF_H__
#define __GLOBAL_NVS_AREA_DEF_H__

#include <PlatformBase.h>
#include <PlatformNvsAreaDef.h>
#include <PchNvsAreaDef.h>
#include <CpuNvsAreaDef.h>
#include <SaNvsAreaDef.h>

#pragma pack (push,1)
typedef struct {
  PLATFORM_NVS_AREA     PlatformNvs;
  PCH_NVS_AREA          PchNvs;
  CPU_NVS_AREA          CpuNvs;
  SYSTEM_AGENT_NVS_AREA SaNvs;
} GLOBAL_NVS_AREA;
#pragma pack(pop)

#endif /* __GLOBAL_NVS_AREA_DEF_H__ */
