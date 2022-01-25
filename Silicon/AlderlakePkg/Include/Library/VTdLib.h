/** @file
  This code provides a initialization of intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 1999 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _VT_D_LIB_H_
#define _VT_D_LIB_H_

#define VTD_ENGINE_NUMBER         3

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
#endif
