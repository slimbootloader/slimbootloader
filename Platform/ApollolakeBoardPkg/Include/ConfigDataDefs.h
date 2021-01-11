/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _CONFIG_DATA_DEFS_H_
#define _CONFIG_DATA_DEFS_H_

#include <ConfigDataCommonDefs.h>


#define  PLATFORM_ID_OXH                0x0006  // Oxbow Hill CRB (DDR3)
#define  PLATFORM_ID_LFH                0x0007  // Leaf Hill CRB (BXT-P, LPDDR4)
#define  PLATFORM_ID_JNH                0x0008  // Juniper Hill CRB (DDR3)
#define  PLATFORM_ID_UP2                0x000E  // UP2
#define  PLATFORM_ID_GPMRB              0x000F  // GP MRB
#define  PLATFORM_ID_MB3                0x000C

#define  ABL_CDATA_BOOT_MODE_GPIO_TAG   0x04E

#include "ConfigDataStruct.h"

#include "ConfigDataDynamic.h"

#pragma pack(1)
typedef struct {
  PCIE_RP_PIN_CTRL_PWR        PcieRpPower0;
  PCIE_RP_PIN_CTRL_RST        PcieRpReset0;
} PCIE_RP_PIN_CTRL;
#pragma pack()

#endif
