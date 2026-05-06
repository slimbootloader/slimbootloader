/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _VT_D_LIB_H_
#define _VT_D_LIB_H_

#define IR                    BIT3
#define SA_VTD_ENGINE_NUMBER  7

#include <IndustryStandard/Acpi.h>
#include <PiPei.h>
#include <Library/BaseLib.h>

typedef UINT32  EFI_ACPI_TABLE_VERSION;

///
/// DMAR table signature
///
#define EFI_ACPI_VTD_DMAR_TABLE_SIGNATURE   0x52414D44  ///< "DMAR"
#define EFI_ACPI_DMAR_TABLE_REVISION        1

//
// Definition for DMA-Remapping Reporting ACPI Table
//
#define EFI_ACPI_DMAR_TABLE_FLAGS_INTR_REMAP_CLEAR                0x00
#define EFI_ACPI_DMAR_TABLE_FLAGS_INTR_REMAP_SET                  0x01
#define EFI_ACPI_DMAR_TABLE_FLAGS_X2APIC_OPT_OUT                  0x02
#define EFI_ACPI_DMAR_TABLE_FLAGS_DMA_CTRL_PLATFORM_OPT_IN_FLAG   0x04

//
// Flags
//
#define EFI_ACPI_DMAR_DRHD_FLAGS_INCLUDE_ALL_SET      0x1
#define EFI_ACPI_DMAR_DRHD_FLAGS_INCLUDE_ALL_CLEAR    0x0
#define EFI_ACPI_DMAR_SATC_FLAGS_ATC_REQUIRED_SET     0x1
#define EFI_ACPI_DMAR_SATC_FLAGS_ATC_REQUIRED_CLEAR   0x0
#define EFI_ACPI_DEVICE_SCOPE_FLAG_ATC_HARDENED       BIT3
#define EFI_ACPI_DEVICE_SCOPE_FLAG_ATC_REQUIRED       BIT4
/**
  Update DMAR ACPI table

  Only the header is found until now.
  Append the rest of table here.

  @param[in,out]  AcpiHeader   The start address of DMAR header from ACPI tables.

  @retval         EFI_SUCCESS, if DMAR update successful.
                  Status,      if error in update.
**/
EFI_STATUS
EFIAPI
UpdateDmarTable (
  IN OUT   EFI_ACPI_DESCRIPTION_HEADER       *AcpiHeader
  );

#endif
