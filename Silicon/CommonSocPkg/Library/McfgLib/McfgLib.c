/** @file
  Library instances for Memory Mapped Configuration Base Address Table (MCFG).

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/McfgLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <IndustryStandard/Acpi.h>

//
// ACPI MCFG table template
//
CONST
EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER mAcpiMcfgTableTemplate = {
  .Header.Signature       = EFI_ACPI_6_5_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE,
  .Header.Length          = sizeof (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER),
  .Header.Revision        = EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_SPACE_ACCESS_TABLE_REVISION,
  .Header.OemTableId      = FixedPcdGet64 (PcdAcpiDefaultOemTableId),
  .Header.OemRevision     = FixedPcdGet32 (PcdAcpiDefaultOemRevision),
  .Header.CreatorId       = FixedPcdGet32 (PcdAcpiDefaultCreatorId),
  .Header.CreatorRevision = FixedPcdGet32 (PcdAcpiDefaultCreatorRevision),
  .Reserved               = EFI_ACPI_RESERVED_QWORD
};

/**
  Initialize and add an ACPI MCFG (Memory Mapped Configuration) table header.

  This function initializes the ACPI MCFG table header with standard fields such as
  signature, length, revision, OEM information, and creator details.

  @param[out] AcpiHeader  Pointer to the ACPI table header to be initialized.

  @retval EFI_SUCCESS     The header was initialized successfully.
  @retval EFI_INVALID_PARAMETER  Invalid parameter provided.
**/
EFI_STATUS
EFIAPI
AddAcpiMcfgHdr (
  OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader
  )
{
  EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER  *Mcfg;

  if (AcpiHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Mcfg = (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER *)AcpiHeader;
  CopyMem (Mcfg, &mAcpiMcfgTableTemplate, sizeof (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER));
  CopyMem (Mcfg->Header.OemId, FixedPcdGetPtr (PcdAcpiDefaultOemId), 6);

  DEBUG ((DEBUG_INFO, "MCFG table header initialized\n"));
  return EFI_SUCCESS;
}

/**
  Add a Memory Mapped Configuration Space Base Address Structure to MCFG table.

  This function appends a configuration space structure to the provided ACPI MCFG table.
  The structure describes the base address, PCI segment group, and bus number range
  for PCI Express enhanced configuration access mechanism.

  @param[in, out] AcpiHeader          Pointer to the ACPI table header to which the
                                      structure will be appended.
  @param[in]      BaseAddress         Base address of the memory mapped configuration space.
  @param[in]      PciSegmentGroupNumber PCI Segment Group number.
  @param[in]      StartBusNumber      Start PCI Bus number decoded by this memory range.
  @param[in]      EndBusNumber        End PCI Bus number decoded by this memory range.

  @retval EFI_SUCCESS     The structure was added successfully.
  @retval EFI_INVALID_PARAMETER  Invalid parameters provided.
**/
EFI_STATUS
EFIAPI
AddMcfgConfigSpaceStruct (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN     UINT64                       BaseAddress,
  IN     UINT16                       PciSegmentGroupNumber,
  IN     UINT8                        StartBusNumber,
  IN     UINT8                        EndBusNumber
  )
{
  EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE  *ConfigSpaceStruct;

  if (AcpiHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (EndBusNumber < StartBusNumber) {
    DEBUG ((DEBUG_ERROR, "MCFG: EndBusNumber (0x%x) < StartBusNumber (0x%x)\n", EndBusNumber, StartBusNumber));
    return EFI_INVALID_PARAMETER;
  }

  ConfigSpaceStruct = (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE *)
                      ((UINT8 *)AcpiHeader + AcpiHeader->Length);

  ZeroMem (ConfigSpaceStruct, sizeof (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE));
  ConfigSpaceStruct->BaseAddress            = BaseAddress;
  ConfigSpaceStruct->PciSegmentGroupNumber  = PciSegmentGroupNumber;
  ConfigSpaceStruct->StartBusNumber         = StartBusNumber;
  ConfigSpaceStruct->EndBusNumber           = EndBusNumber;
  ConfigSpaceStruct->Reserved               = EFI_ACPI_RESERVED_DWORD;

  DEBUG ((DEBUG_INFO, "MCFG Config Space: BaseAddr=0x%llx, Segment=0x%x, Bus=0x%x-0x%x\n",
          BaseAddress, PciSegmentGroupNumber, StartBusNumber, EndBusNumber));

  // Update the ACPI table length
  AcpiHeader->Length += sizeof (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE);
  return EFI_SUCCESS;
}
