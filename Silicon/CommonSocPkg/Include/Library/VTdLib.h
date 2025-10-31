/** @file
  Provides a initialization of intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _COMMON_VT_D_LIB_H_
#define _COMMON_VT_D_LIB_H_

#define VTD_ENGINE_NUMBER         2

typedef enum {
  IGD_VTD = 0, // Individual VT-d for GFX device
  IOP_VTD = 1  // Global VT-d for Segment 0
} VTD_ENGINE;

/**
  Read VTD Engine Base Address from VTD BAR Offsets.

  @param[in]  VtdEngineNumber        - Engine number for which VTD Base Adderess is required.

  @retval   VTD Engine Base Address
**/
UINT32
EFIAPI
ReadVtdBaseAddress (
  IN UINT8            VtdEngineNumber
  );

#endif
