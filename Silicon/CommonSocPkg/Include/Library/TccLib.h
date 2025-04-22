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

/**
  Check if the TCC DSO is a bad binary by checking its signature.

  @retval TRUE     TCC DSO was marked as a bad binary
  @retval FALSE    TCC DSO was not found or not marked as a bad binary

 */
BOOLEAN
EFIAPI
IsMarkedBadDso (
  VOID
  );

/**
  Mark TCC DSO as a bad binary by changing its signature to 0.

  @retval EFI_SUCCESS
  @retval EFI_NOT_FOUND          Unable to find IPFW/TCCT
  @retval Others                 Errors during SPI operations

 */
EFI_STATUS
EFIAPI
InvalidateBadDso (
  VOID
  );

#endif
