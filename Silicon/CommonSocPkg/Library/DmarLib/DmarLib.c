/** @file
  Library instances for DMA Remapping Reporting (DMAR) Table which supports Intel
  Virtualization Technology for Directed I/O (VT-d).

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/DmarLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/VTdLib.h>
#include <Library/PciLib.h>
#include <Library/BaseLib.h>
#include <Register/Intel/Cpuid.h>

//
// ACPI DMAR table template
//
CONST
EFI_ACPI_DMAR_HEADER mAcpiDmarTableTemplate = {
  .Header.Signature       = EFI_ACPI_6_4_DMA_REMAPPING_TABLE_SIGNATURE,
  .Header.Length          = sizeof (EFI_ACPI_DMAR_HEADER),
  .Header.Revision        = 1,
  .Header.OemTableId      = FixedPcdGet64 (PcdAcpiDefaultOemTableId),
  .Header.OemRevision     = FixedPcdGet32 (PcdAcpiDefaultOemRevision),
  .Header.CreatorId       = FixedPcdGet32 (PcdAcpiDefaultCreatorId),
  .Header.CreatorRevision = FixedPcdGet32 (PcdAcpiDefaultCreatorRevision),
  .HostAddressWidth       = 0,
  .Flags                  = 0,
  .Reserved = {0}
};


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
  )
{
  EFI_ACPI_DMAR_HEADER                         *Dmar;
  UINT32                                       RegEax;

  if (!FeaturePcdGet (PcdVtdEnabled)) {
    return EFI_UNSUPPORTED;
  }

  Dmar = (EFI_ACPI_DMAR_HEADER *)AcpiHeader;
  CopyMem(Dmar->Header.OemId, FixedPcdGetPtr(PcdAcpiDefaultOemId), 6);
  Dmar->Flags = Flags;

  // Set the host physical address width using CPUID.
  AsmCpuid (CPUID_VIR_PHY_ADDRESS_SIZE, &RegEax, NULL, NULL, NULL);
  Dmar->HostAddressWidth = (UINT8)((RegEax & 0xFF) - 1);
  DEBUG ((DEBUG_INFO, "Dmar header HostAddressWidth = 0x%x\n", Dmar->HostAddressWidth));

  return EFI_SUCCESS;
}


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
  )
{
  EFI_ACPI_DMAR_DRHD_HEADER           *DrhdHeader;
  UINT8                               SizeField;

  DrhdHeader = (EFI_ACPI_DMAR_DRHD_HEADER *) ((UINT8 *)AcpiHeader + AcpiHeader->Length);
  if (DrhdHeader == NULL || SizeInBytes == 0) {
    return NULL;
  }

  // Ensure SizeInBytes is a multiple of 4 KB.
  if ((SizeInBytes % 4096) != 0) {
    DEBUG((DEBUG_ERROR, "SizeInBytes must be a multiple of 4 KB.\n"));
    return NULL;
  }

  // Calculate the Size field value (2^N * 4 KB pages).
  SizeField = 0;
  while ((1U << SizeField) * 4096 < SizeInBytes) {
    SizeField++;
  }

  // Validate that SizeInBytes matches a valid 2^N * 4 KB size.
  if ((1U << SizeField) * 4096 != SizeInBytes) {
    DEBUG((DEBUG_ERROR, "SizeInBytes does not match a valid 2^N * 4 KB size.\n"));
    return NULL;
  }

  ZeroMem(DrhdHeader, sizeof(EFI_ACPI_DMAR_DRHD_HEADER));
  DrhdHeader->Header.Type         = EFI_ACPI_DMAR_TYPE_DRHD;
  DrhdHeader->Header.Length       = sizeof(EFI_ACPI_DMAR_DRHD_HEADER);
  DrhdHeader->Flags               = Flags;
  DrhdHeader->Size                = SizeField;
  DrhdHeader->SegmentNumber       = SegmentNumber;
  DrhdHeader->RegisterBaseAddress = RegisterBaseAddress;

  DEBUG((DEBUG_INFO,
    "DMAR DRHD: Flags=0x%x, Size=2^%u * 4 KB, SegNum=0x%x, RegBaseAddr=0x%llx\n",
    Flags, SizeField, SegmentNumber, RegisterBaseAddress));

  // Update the ACPI table length.
  AcpiHeader->Length += sizeof (EFI_ACPI_DMAR_DRHD_HEADER);
  return &DrhdHeader->Header;
}


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
  )
{
  EFI_ACPI_DMAR_RMRR_HEADER             *RmrrHeader;

  RmrrHeader = (EFI_ACPI_DMAR_RMRR_HEADER *) ((UINT8 *)AcpiHeader + AcpiHeader->Length);
  if (RmrrHeader == NULL || RsvdMemRegionLimit <= RsvdMemRegionBase) {
    return NULL;
  }

  RmrrHeader->Header.Type                      = EFI_ACPI_DMAR_TYPE_RMRR;
  RmrrHeader->Header.Length                    = sizeof(EFI_ACPI_DMAR_RMRR_HEADER);
  RmrrHeader->SegmentNumber                    = SegmentNumber;
  ZeroMem(&RmrrHeader->Reserved, sizeof(RmrrHeader->Reserved));
  RmrrHeader->ReservedMemoryRegionBaseAddress  = RsvdMemRegionBase;
  RmrrHeader->ReservedMemoryRegionLimitAddress = RsvdMemRegionLimit;

  // Update the ACPI table length.
  AcpiHeader->Length += sizeof(EFI_ACPI_DMAR_RMRR_HEADER);
  return &RmrrHeader->Header;
}


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
  )
{
  EFI_ACPI_DMAR_SATC_HEADER             *SatcHeader;

  SatcHeader = (EFI_ACPI_DMAR_SATC_HEADER *) ((UINT8 *)AcpiHeader + AcpiHeader->Length);
  if (SatcHeader == NULL) {
    return NULL;
  }

  // Initialize the SATC structure.
  SatcHeader->Header.Type   = EFI_ACPI_DMAR_TYPE_SATC;
  SatcHeader->Header.Length = sizeof(EFI_ACPI_DMAR_SATC_HEADER);
  SatcHeader->Flags         = Flags;
  SatcHeader->Reserved      = 0;
  SatcHeader->SegmentNumber = SegmentNumber;

  // Update the ACPI table length.
  AcpiHeader->Length += sizeof(EFI_ACPI_DMAR_SATC_HEADER);
  return &SatcHeader->Header;
}


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
  )
{
  EFI_ACPI_DMAR_SIDP_HEADER           *SidpHeader;

  SidpHeader = (EFI_ACPI_DMAR_SIDP_HEADER *) ((UINT8 *)AcpiHeader + AcpiHeader->Length);
  if (SidpHeader == NULL) {
    return NULL;
  }

  // Initialize the SIDP structure.
  SidpHeader->Header.Type   = EFI_ACPI_DMAR_TYPE_SIDP;
  SidpHeader->Header.Length = sizeof(EFI_ACPI_DMAR_SIDP_HEADER);
  SidpHeader->Reserved      = 0;
  SidpHeader->SegmentNumber = SegmentNumber;

  // Update the ACPI table length.
  AcpiHeader->Length += sizeof(EFI_ACPI_DMAR_SIDP_HEADER);
  return &SidpHeader->Header;
}


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
  )
{
  EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER *ScopeHeader;
  EFI_ACPI_DMAR_PCI_PATH                      *PciPath;
  UINT16                                      ScopeSize;

  ScopeHeader = (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER *) ((UINT8 *)AcpiHeader + AcpiHeader->Length);
  if (ScopeHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((ScopeType == EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT) ||
      (ScopeType == EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE)) {
    // Check if the PCI device is enabled.
    if (PciRead32 (PCI_LIB_ADDRESS (BusNumber, Device, Function, 0)) == 0xFFFFFFFF) {
      DEBUG((DEBUG_INFO, "device (%x:%x:%x) is disabled\n", BusNumber, Device, Function));
      return EFI_NO_MEDIA;
    }
  }

  // Calculate the size of the new scope structure.
  ScopeSize = (UINT16)(sizeof (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER) + sizeof (EFI_ACPI_DMAR_PCI_PATH));

  ScopeHeader->Type           = ScopeType;
  ScopeHeader->Length         = (UINT8)ScopeSize;
  ScopeHeader->Flags          = Flags;
  ScopeHeader->Reserved       = 0;
  ScopeHeader->EnumerationId  = EnumerationId;
  ScopeHeader->StartBusNumber = BusNumber;

  PciPath = (EFI_ACPI_DMAR_PCI_PATH *) ((UINT8 *)ScopeHeader + sizeof (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER));
  PciPath->Device   = Device;
  PciPath->Function = Function;

  // Update the ACPI table length.
  AcpiHeader->Length += ScopeSize;
  DmarHdr->Length    += ScopeSize;

  return EFI_SUCCESS;
}

