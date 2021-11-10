/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __TCC_LIB_H__
#define __TCC_LIB_H__

#define EFI_ACPI_RTCT_SIGNATURE           SIGNATURE_32('R', 'T', 'C', 'T')

#include <Base.h>
#include <IndustryStandard/Acpi60.h>

/**
  Update RTCT  (Platform Tuning Configuration Table) ACPI Table

  @param[in] RtctTable          PTCT ACPI Table entry

  @retval EFI_SUCCESS           The RTCT table is updated.
  @retval EFI_NOT_FOUND         Loader Global Pointer or TccHob not found
  @retval EFI_INVALID_PARAMETER RTCT table is NULL
**/
EFI_STATUS
EFIAPI
UpdateAcpiRtctTable (
  IN EFI_ACPI_DESCRIPTION_HEADER *RtctTable
  );

#endif
