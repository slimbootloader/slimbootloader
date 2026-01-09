/** @file

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _VTD_INFO_H_
#define _VTD_INFO_H_

//
// The order of members in the below enum represents the order of enabling
// bits in VT-d BAR register.
//


typedef enum {
  GFX_VTD     = 0,  // Individual VT-d for GFX device
  NON_GFX_VTD = 1,  // Individual VT-d for non-GFX devices (NPU, IPU)
  GLOBAL_VTD  = 2,  // Global VT-d for Segment 0
  VTD_ENGINE_NUMBER
} VTD_ENGINE;

#endif
