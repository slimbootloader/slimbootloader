/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  //
  // Define TSE NVS Area operation region.
  //
#ifndef _TSE_NVS_AREA_DEF_H_
#define _TSE_NVS_AREA_DEF_H_

#pragma pack (push,1)
typedef struct {
  UINT8    TseEnable;                               ///< Offset 0       TSE Device Enable
} TSE_NVS_AREA;

#pragma pack(pop)
#endif
