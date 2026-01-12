/** @file
  General GPIO ChipsetId for all platforms

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIOV2_CHIPSET_ID_H_
#define _GPIOV2_CHIPSET_ID_H_

#if FixedPcdGetBool (PcdWclSupport) == 1
#define GPIOV2_WCL_PCD_CHIPSET_ID     1
#else
#define GPIOV2_PTL_PCD_CHIPSET_ID     1
#endif

#endif // _GPIOV2_CHIPSET_ID_H_