/** @file
  Library instances for Memory Mapped Configuration Base Address Table (MCFG).

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _MCFG_LIB_H_
#define _MCFG_LIB_H_

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <Mcfg.h>

/**
  Initialize and add an ACPI MCFG (Memory Mapped Configuration) table header.

  This function initializes the ACPI MCFG table header with standard fields such as
  signature, length, revision, OEM information, and creator details.

  @param[out] AcpiHeader        Pointer to the ACPI table header to be initialized.

  @retval EFI_SUCCESS           The header was initialized successfully.
  @retval EFI_INVALID_PARAMETER Invalid parameter provided.
**/
EFI_STATUS
EFIAPI
AddAcpiMcfgHdr (
  OUT EFI_ACPI_DESCRIPTION_HEADER *AcpiHeader
  );

/**
  Add a Memory Mapped Configuration Space Base Address Structure to MCFG table.

  This function appends a configuration space structure to the provided ACPI MCFG table.
  The structure describes the base address, PCI segment group, and bus number range
  for PCI Express enhanced configuration access mechanism.

  @param[in, out] AcpiHeader            Pointer to the ACPI table header to which the
                                        structure will be appended.
  @param[in]      BaseAddress           Base address of the memory mapped configuration space.
  @param[in]      PciSegmentGroupNumber PCI Segment Group number.
  @param[in]      StartBusNumber        Start PCI Bus number decoded by this memory range.
  @param[in]      EndBusNumber          End PCI Bus number decoded by this memory range.

  @retval EFI_SUCCESS           The structure was added successfully.
  @retval EFI_INVALID_PARAMETER Invalid parameters provided.
**/
EFI_STATUS
EFIAPI
AddMcfgConfigSpaceStruct (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER *AcpiHeader,
  IN     UINT64                      BaseAddress,
  IN     UINT16                      PciSegmentGroupNumber,
  IN     UINT8                       StartBusNumber,
  IN     UINT8                       EndBusNumber
  );

#endif // _MCFG_LIB_H_