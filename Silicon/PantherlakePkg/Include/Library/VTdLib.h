/** @file
  This code provides a initialization of intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 1999 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _VT_D_LIB_H_
#define _VT_D_LIB_H_

#include <IndustryStandard/Acpi.h>

typedef UINT32  EFI_ACPI_TABLE_VERSION;
#define EFI_ACPI_TABLE_VERSION_1_0B (1 << 1)
#define EFI_ACPI_TABLE_VERSION_2_0  (1 << 2)
#define EFI_ACPI_TABLE_VERSION_3_0  (1 << 3)
/**
  Update DMAR ACPI table at the end of stage 2
**/
VOID
UpdateDmarAcpi (
  EFI_ACPI_DESCRIPTION_HEADER *Table
  );

/**
  Read VTD Engine Base Address from VTD BAR Offsets.

  @param[in]  VtdEngineNumber        - Engine number for which VTD Base Adderess is required.

  @retval   VTD Engine Base Address
**/
UINT32
ReadVtdBaseAddress (
  IN UINT8        VtdEngineNumber
  );

typedef enum {
  GFX_VTD     = 0,  // Individual VT-d for GFX device
  IGD_VTD     = 0,  // other name for Gfx VT-d engine
  NON_GFX_VTD = 1,  // Individual VT-d for non-GFX devices (NPU, IPU)
  GLOBAL_VTD  = 2,  // Global VT-d for Segment 0
  IOP_VTD     = 2,  // other name for Global VT-d engine
  VTD_ENGINE_NUMBER
} VTD_ENGINE;
#endif
