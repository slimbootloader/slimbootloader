/** @file
  This code provides a initialization of intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 1999 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _VT_D_LIB_H_
#define _VT_D_LIB_H_

#define VTD_ECAP_REG  0x10
#define IR            BIT3
#define SA_VTD_ENGINE_NUMBER  7

#include <IndustryStandard/Acpi.h>

typedef UINT32  EFI_ACPI_TABLE_VERSION;
#define EFI_ACPI_TABLE_VERSION_1_0B (1 << 1)
#define EFI_ACPI_TABLE_VERSION_2_0  (1 << 2)
#define EFI_ACPI_TABLE_VERSION_3_0  (1 << 3)


#define  DMAR_TABLE_FLAGS_INT_REMAPPING_SUPPORT       BIT0
#define  DMAR_TABLE_FLAGS_X2APIC_OPT_OUT              BIT1
#define  DMAR_TABLE_FLAGS_DMA_CONTROL_GUARANTEE       BIT2

/**
  Update the DMAR table

  @param[in, out] Table              The pointer to ACPI DMAR table
  @param[in] DmarTableFlags          The flags related with DMAR table
**/
VOID
EFIAPI
UpdateDmarAcpi (
  EFI_ACPI_DESCRIPTION_HEADER *Table,
  UINTN                       DmarTableFlags
  );

/**
  Read VTD Engine Base Address from VTD BAR Offsets.

  @param[in]  VtdEngineNumber        - Engine number for which VTD Base Adderess is required.

  @retval   VTD Engine Base Address
**/
UINT32
EFIAPI
ReadVtdBaseAddress (
  IN UINT8        VtdEngineNumber
  );
#endif
