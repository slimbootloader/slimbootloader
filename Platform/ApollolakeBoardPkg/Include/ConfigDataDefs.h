/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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

#endif
