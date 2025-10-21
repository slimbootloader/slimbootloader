/** @file
  Library instances for DMA Remapping Reporting (DMAR) Table which supports Intel
  Virtualization Technology for Directed I/O (VT-d).

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _DMAR_LIB_H_
#define _DMAR_LIB_H_

#include <IndustryStandard/DmaRemappingReportingTable.h>

/**
  Initialize and add an ACPI DMAR (DMA Remapping) table header.

  This function initializes the ACPI DMAR table header with standard fields such as
  signature, length, revision, OEM information, and creator details. It also sets
  the host physical address width and DMAR-specific flags.

  @param[out] AcpiHeader  Pointer to the ACPI table header to be initialized.
  @param[in]  Flags       The flags for the DMAR table

  @retval EFI_SUCCESS           The header was initialized successfully.
  @retval EFI_UNSUPPORTED       VT-d is not enabled.
**/
EFI_STATUS
EFIAPI
AddAcpiDmarHdr (
  OUT EFI_ACPI_DESCRIPTION_HEADER              *AcpiHeader,
  IN  UINT8                                     Flags
  );

/**
  Add a DMAR DRHD (DMA Remapping Hardware Unit Definition) structure to an ACPI table.

  This function appends a DRHD structure to the provided ACPI table. The DRHD structure
  describes a DMA remapping hardware unit, including its flags, register set size,
  PCI segment number, and the base address of the remapping hardware register set.
  The function validates the register set size to ensure it is a valid 2^N * 4 KB value,
  initializes the DRHD structure, and updates the ACPI table length accordingly.

  @param[in, out] AcpiHeader          Pointer to the ACPI table header to which the DRHD
                                      structure will be appended.
  @param[in]      Flags               Flags for the DRHD structure, indicating specific
                                      properties or capabilities.
  @param[in]      SizeInBytes         Size of the remapping hardware register set in bytes.
                                      Must be a power-of-two multiple of 4 KB.
  @param[in]      SegmentNumber       PCI Segment number associated with this DMAR unit.
  @param[in]      RegisterBaseAddress Base address of the remapping hardware register set.

  @retval Pointer to the newly added DRHD structure header if successful.
  @retval NULL         One or more parameters are invalid (e.g., SizeInBytes is not valid).
**/
EFI_ACPI_DMAR_STRUCTURE_HEADER *
EFIAPI
AddDrhdHdr (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN     UINT8                        Flags,
  IN     UINT32                       SizeInBytes,
  IN     UINT16                       SegmentNumber,
  IN     UINT64                       RegisterBaseAddress
  );

/**
  Add an RMRR (Reserved Memory Region Reporting) structure to an ACPI DMAR table.

  This function appends an RMRR structure to the provided ACPI table. The RMRR structure
  describes a reserved memory region that may be used as a DMA target, along with the
  PCI segment number associated with the devices that require access to this memory region.
  The function validates the memory region range, initializes the RMRR structure, and
  updates the ACPI table length accordingly.

  @param[in, out] AcpiHeader          Pointer to the ACPI table header to which the RMRR
                                      structure will be appended.
  @param[in]      SegmentNumber       PCI Segment number associated with the devices
                                      requiring access to the reserved memory.
  @param[in]      RsvdMemRegionBase   Base address of the reserved memory region.
  @param[in]      RsvdMemRegionLimit  Limit address of the reserved memory region. Must
                                      be greater than the base address.

  @retval Pointer to the newly added RMRR structure header if successful.
  @retval NULL                       A parameter is invalid.
**/
EFI_ACPI_DMAR_STRUCTURE_HEADER *
EFIAPI
AddRmrrHdr (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER    *AcpiHeader,
  IN     UINT16                         SegmentNumber,
  IN     UINT64                         RsvdMemRegionBase,
  IN     UINT64                         RsvdMemRegionLimit
  );

/**
  Append a DMAR SATC (Storage and Access Table Cache) structure to an ACPI DMAR table.

  This function creates and appends a SATC structure to the provided ACPI table.
  The SATC structure describes storage and access table cache properties for a specific
  PCI segment, including associated flags that define its behavior for DMA remapping.
  The function initializes the SATC structure and updates the ACPI table length accordingly.

  @param[in, out] AcpiHeader    Pointer to the ACPI table header to which the SATC
                                structure will be appended.
  @param[in]      Flags         Flags for the SATC structure, indicating specific properties
                                or behaviors.
  @param[in]      SegmentNumber PCI Segment number associated with the SATC structure.

  @retval Pointer to the newly added SATC structure header if successful.
  @retval NULL                  A parameter is invalid (e.g., AcpiHeader is NULL).
**/
EFI_ACPI_DMAR_STRUCTURE_HEADER *
EFIAPI
AddSatcHdr (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER    *AcpiHeader,
  IN     UINT8                          Flags,
  IN     UINT16                         SegmentNumber
  );

/**
  Create and append a DMAR SIDP (System Interrupt Distribution Protocol) structure
  to an ACPI DMAR table.

  This function creates a SIDP structure and appends it to the provided ACPI table.
  The SIDP structure describes interrupt distribution protocol information for a specific
  PCI segment, including the segment number. The function initializes the SIDP structure
  and updates the ACPI table length accordingly.

  @param[in, out] AcpiHeader        Pointer to the ACPI table header to which the SIDP
                                    structure will be appended.
  @param[in]      SegmentNumber     PCI Segment number associated with the SIDP structure.

  @retval Pointer to the newly added SIDP structure header if successful.
  @retval NULL                      A parameter is invalid (e.g., AcpiHeader is NULL).
**/
EFI_ACPI_DMAR_STRUCTURE_HEADER *
EFIAPI
AddSidpHdr (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *AcpiHeader,
  IN     UINT16                       SegmentNumber
  );

/**
  Add a device scope structure to an ACPI DMAR table.

  This function appends a device scope structure to an ACPI DMAR table. The device scope
  structure specifies the scope of DMA remapping or reserved memory access for a specific
  device, identified by its PCI bus, device, and function numbers.

  @param[in, out] AcpiHeader          Pointer to the ACPI table header where the scope
                                      structure will be added.
  @param[in]      ScopeType           Type of the device scope (e.g., PCI endpoint, PCI bridge).
  @param[in]      Flags               Flags for the device scope, indicating specific properties.
  @param[in]      EnumerationId       Enumeration ID for the device scope.
  @param[in]      BusNumber           The bus number for the device scope.
  @param[in]      Device              PCI device number for the target device.
  @param[in]      Function            PCI function number for the target device.

  @retval EFI_SUCCESS                 The device scope structure was added successfully.
  @retval EFI_INVALID_PARAMETER       A parameter is invalid (e.g., NULL pointer for AcpiHeader).
**/
EFI_STATUS
EFIAPI
AddScopeData (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER          *AcpiHeader,
  IN OUT EFI_ACPI_DMAR_STRUCTURE_HEADER       *DmarHdr,
  IN  UINT8                                   ScopeType,
  IN  UINT8                                   Flags,
  IN  UINT8                                   EnumerationId,
  IN  UINT8                                   BusNumber,
  IN  UINT8                                   Device,
  IN  UINT8                                   Function
  );

#endif

