/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __ACPI_INIT_LIB_PRIVATE_H__
#define __ACPI_INIT_LIB_PRIVATE_H__

/**
  Get the size of MADT

  @retval                   0 if CpuInfo is invalid, otherwise the size of MADT

 **/
UINT32
EFIAPI
GetMadtSize (
  IN  VOID
  );

/**
  Create MADT(Multiple APIC Description Table).

  @param[in] TableAddress   Pointer to create ACPI MADT
  @param[in] TableLength    Size of MADT

  @retval EFI_SUCCESS       Created ACPI MADT successfully.
  @retval Others            Failed to create MADT

 **/
EFI_STATUS
EFIAPI
CreateMadt (
  IN  UINT8     *TableAddress,
  IN  UINT32     TableLength
  );

#endif /* __ACPI_INIT_LIB_PRIVATE_H__ */
