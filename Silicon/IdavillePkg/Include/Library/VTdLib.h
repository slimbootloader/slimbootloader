/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
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

/**
  Update DMAR ACPI table

  Only the header is found until now.
  Append the rest of table here.

  @param[in,out]  Table       The start address of DMAR header from ACPI tables.

  @retval         EFI_SUCCESS, if DMAR update successful.
                  Status,      if error in update.
**/
EFI_STATUS
UpdateDmarTable (
  IN OUT  VOID  *Table
  );

#endif
