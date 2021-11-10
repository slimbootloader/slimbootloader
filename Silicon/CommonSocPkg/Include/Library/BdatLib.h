/** @file


  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _BDAT_LIB_H_
#define _BDAT_LIB_H_

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/Acpi30.h>

#define EFI_BDAT_TABLE_SIGNATURE  SIGNATURE_32 ('B', 'D', 'A', 'T')
#define ACPI_BDAT_OEM_REV         0x00001000
#define EFI_BDAT_ACPI_POINTER     0x0

#define BDAT_PRIMARY_VERSION      4  ///< The BDAT structure that is currently supported.
#define BDAT_SECONDARY_VERSION    0  ///< The BDAT structure that is currently supported.
#define OEM_OFFSET                0  ///< The current offset to the OEM data in the BDAT structure.

///
/// Ensure proper structure formats
///
#pragma pack(push, 1)

///
/// BIOS Data ACPI structure
///
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE  BdatGas;
} EFI_BDAT_ACPI_DESCRIPTION_TABLE;

/**
  Update BDAT ACPI table.

  @param[in] Table              BDAT ACPI table which needs to be updated
  @param[in] HobList        A pointer to the HOB list

  @retval EFI_SUCCESS:              Driver initialized successfully
  @exception EFI_UNSUPPORTED:       A needed driver was not located
  @retval EFI_OUT_OF_RESOURCES:     Could not allocate needed resources
**/
EFI_STATUS
UpdateBdatAcpiTable (
  IN EFI_ACPI_DESCRIPTION_HEADER    *Table,
  IN VOID                           *HobList
  );

///
/// This is copied from Include\Acpi.h
///
#define CREATOR_ID_INTEL  0x4C544E49  /// "LTNI""INTL"(Intel)
#define CREATOR_REV_INTEL 0x20090903

#pragma pack(pop)

#endif /// _BDAT_H_
