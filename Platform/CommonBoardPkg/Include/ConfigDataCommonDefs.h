/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _CONFIG_DATA_COMMON_DEFS_H_
#define _CONFIG_DATA_COMMON_DEFS_H_

// Note: Any Config TAG that is less than 0x100 will be considered as Common TAG
// and it is applicable for all platforms.

// 0x050 to 0x06F is reserved for boot option tag
#define  MAX_BOOT_OPTION_CFGDATA_ENTRY  32
#define  CDATA_BOOT_OPTION_TAG          0x050

#define  CDATA_CAPSULE_TAG              0x080

#include "ConfigDataCommonStruct.h"

#endif
