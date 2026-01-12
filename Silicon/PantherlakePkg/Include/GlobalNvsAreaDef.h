/**@file


  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __GLOBAL_NVS_AREA_DEF_H__
#define __GLOBAL_NVS_AREA_DEF_H__

#include <HostBridgeNvsAreaDef.h>
#include <CpuNvsAreaDef.h>
#include <EcNvsAreaDef.h>
#include <IGpuNvsAreaDef.h>
#include <IpuNvsAreaDef.h>
#include <PchNvsAreaDef.h>
#include <PlatformBase.h>
#include <PlatformNvsAreaDef.h>
#include <TcssNvsAreaDef.h>
#include <TseNvsAreaDef.h>
#include <UcsiNvsAreaDef.h>
#include <VmdNvsAreaDef.h>
#include <ITbtNvsAreaDef.h>
#include <PepNvsAreaDef.h>
#include <Ucsi.h>


#pragma pack (push,1)
typedef struct {
   PLATFORM_NVS_AREA     PlatformNvs;
   CPU_NVS_AREA          CpuNvs;
   HOST_BRIDGE_NVS_AREA  HostBridgeNvs;
   PCH_NVS_AREA          PchNvs;
   EC_NVS_AREA           EcNvs;
   IGPU_NVS_AREA         IgpuNvs;
   PEP_NVS_AREA          PepNvs;
   TCSS_NVS_AREA         TcssNvs;
   IPU_NVS_AREA          IpuNvs;
   TSE_NVS_AREA          TseNvs;
   UCSI_NVS_AREA         UcsiNvs;
   VMD_NVS_AREA          VmdNvs;
   ITBT_NVS_AREA         ItbtNvs;
   UCSI_OPM_BUF          OpmBuffer;
} GLOBAL_NVS_AREA;
#pragma pack(pop)

#endif /* __GLOBAL_NVS_AREA_DEF_H__ */
