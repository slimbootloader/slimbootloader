/** @file
  Provides a initialization of intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _VT_D_LIB_H_
#define _VT_D_LIB_H_

#define MAX_PHYSICAL_ADDRESS_SIZE_WITH_MKTME   42
#define VTD_ENGINE_NUMBER         3

typedef enum {
  GFX_VTD     = 0, // Individual VT-d for GFX device
  NON_GFX_VTD = 1, // Individual VT-d for non-GFX devices (NPU, IPU)
  IOP_VTD     = 2  // Global VT-d for Segment 0
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
