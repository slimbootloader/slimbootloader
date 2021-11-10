/** @file

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __ACPI_INIT_LIB_H__
#define __ACPI_INIT_LIB_H__

#include <Uefi/UefiBaseType.h>
#include <IndustryStandard/Acpi.h>

#pragma pack(1)
///
/// Firmware Performance Data Table.
///
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER                             Header;            ///< Common ACPI description table header.
  EFI_ACPI_5_0_FPDT_BOOT_PERFORMANCE_TABLE_POINTER_RECORD BootPointerRecord; ///< Basic Boot Performance Table Pointer record.
  EFI_ACPI_5_0_FPDT_S3_PERFORMANCE_TABLE_POINTER_RECORD   S3PointerRecord;   ///< S3 Performance Table Pointer record.
} FIRMWARE_PERFORMANCE_TABLE;

///
/// Basic Boot Performance Data Table.
/// This structure contains BasicBoot performance record.
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_TABLE_HEADER   Header;     ///< Common ACPI table header.
  EFI_ACPI_5_0_FPDT_FIRMWARE_BASIC_BOOT_RECORD BasicBoot;  ///< Basic Boot Resume performance record.
  //
  // one or more boot performance records.
  //
} BOOT_PERFORMANCE_TABLE;

///
/// S3 Performance Data Table.
/// This structure contains S3 performance records which will be updated in S3
/// suspend and S3 resume boot path.
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_TABLE_HEADER  Header;    ///< Common ACPI table header.
  EFI_ACPI_5_0_FPDT_S3_RESUME_RECORD          S3Resume;  ///< Basic S3 Resume performance record.
} S3_PERFORMANCE_TABLE;

#pragma pack()

/**
  This function is called on S3 boot flow only.

  It will locate the S3 waking vector from the ACPI table and then
  jump into it. The control will never return.

  @param  AcpiBase   ACPI table base address

**/
VOID
EFIAPI
FindAcpiWakeVectorAndJump (
  IN  UINT32    AcpiBase
  );

/**
  This function creates necessary ACPI tables and puts the RSDP
  table in F segment so that OS can locate it.

  @param[in]  AcpiMemTop      ACPI memory top address.

  @retval     EFI_SUCCESS     ACPI tables are created successfully.
              EFI_NOT_FOUND   Required ACPI tables could not be found.

**/
EFI_STATUS
EFIAPI
AcpiInit (
  IN  UINT32    *AcpiMemTop
  );

/**
  Update ACPI FPDT S3 performance record table.

  @param[in] AcpiTableBase     ACPI base address

  @retval EFI_SUCCESS          Update the S3 performance table successfully.
  @retval Others               Failed to update the table.
 **/
EFI_STATUS
EFIAPI
UpdateFpdtS3Table (
  IN  UINT32                   AcpiTableBase
  );

#endif
