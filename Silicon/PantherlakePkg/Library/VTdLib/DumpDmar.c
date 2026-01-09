/** @file
  Source file for DMAR Dump

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DmaRemappingTable.h>


#define EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT           0x01
#define EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE             0x02
#define EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_IOAPIC                 0x03
#define EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_MSI_CAPABLE_HPET       0x04
#define EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ACPI_NAMESPACE_DEVICE  0x05
#define EFI_ACPI_DMAR_DRHD_FLAGS_INCLUDE_PCI_ALL                BIT0
#define EFI_ACPI_DMAR_FLAGS_INTR_REMAP                          BIT0
#define EFI_ACPI_DMAR_FLAGS_X2APIC_OPT_OUT                      BIT1
#define EFI_ACPI_DMAR_FLAGS_DMA_CTRL_PLATFORM_OPT_IN_FLAG       BIT2

///
/// Remapping Structure Types definitions from section 8.2
///@{
#define EFI_ACPI_DMAR_TYPE_DRHD  0x00
#define EFI_ACPI_DMAR_TYPE_RMRR  0x01
#define EFI_ACPI_DMAR_TYPE_ATSR  0x02
#define EFI_ACPI_DMAR_TYPE_RHSA  0x03
#define EFI_ACPI_DMAR_TYPE_ANDD  0x04
#define EFI_ACPI_DMAR_TYPE_SATC  0x05
#define EFI_ACPI_DMAR_TYPE_SIDP  0x06
///@}

#define EFI_ACPI_DMAR_ATSR_FLAGS_ALL_PORTS                      BIT0





#pragma pack(1)

/**
  Dump DMAR DeviceScopeEntry.

  @param[in]  DmarDeviceScopeEntry  DMAR DeviceScopeEntry
**/
VOID
DumpDmarDeviceScopeEntry (
  IN EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER     *DmarDeviceScopeEntry
  )
{
  UINTN   PciPathNumber;
  UINTN   PciPathIndex;
  EFI_ACPI_DMAR_PCI_PATH  *PciPath;

  if (DmarDeviceScopeEntry == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO,
    "    *************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    "    *       DMA-Remapping Device Scope Entry Structure                      *\n"
    ));
  DEBUG ((DEBUG_INFO,
    "    *************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    (sizeof(UINTN) == sizeof(UINT64)) ?
    "    DMAR Device Scope Entry address ...................... 0x%016lx\n" :
    "    DMAR Device Scope Entry address ...................... 0x%08x\n",
    DmarDeviceScopeEntry
    ));
  DEBUG ((DEBUG_INFO,
    "      Device Scope Entry Type ............................ 0x%02x\n",
    DmarDeviceScopeEntry->Type
    ));
  switch (DmarDeviceScopeEntry->Type) {
  case EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT:
    DEBUG ((DEBUG_INFO,
      "        PCI Endpoint Device\n"
      ));
    break;
  case EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_BRIDGE:
    DEBUG ((DEBUG_INFO,
      "        PCI Sub-hierachy\n"
      ));
    break;
  case EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_IOAPIC:
    DEBUG ((DEBUG_INFO,
      "        IOAPIC\n"
      ));
    break;
  case EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_MSI_CAPABLE_HPET:
    DEBUG ((DEBUG_INFO,
      "        MSI Capable HPET\n"
      ));
    break;
  case EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ACPI_NAMESPACE_DEVICE:
    DEBUG ((DEBUG_INFO,
      "        ACPI Namespace Device\n"
      ));
    break;
  default:
    break;
  }
  DEBUG ((DEBUG_INFO,
    "      Length ............................................. 0x%02x\n",
    DmarDeviceScopeEntry->Length
    ));
  DEBUG ((DEBUG_INFO,
    "      Flags .............................................. 0x%02x\n",
    DmarDeviceScopeEntry->Flags
    ));
  DEBUG ((DEBUG_INFO,
    "      Enumeration ID ..................................... 0x%02x\n",
    DmarDeviceScopeEntry->EnumerationId
    ));
  DEBUG ((DEBUG_INFO,
    "      Starting Bus Number ................................ 0x%02x\n",
    DmarDeviceScopeEntry->StartBusNumber
    ));

  PciPathNumber = (DmarDeviceScopeEntry->Length - sizeof(EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER)) / sizeof(EFI_ACPI_DMAR_PCI_PATH);
  PciPath = (EFI_ACPI_DMAR_PCI_PATH *)(DmarDeviceScopeEntry + 1);
  for (PciPathIndex = 0; PciPathIndex < PciPathNumber; PciPathIndex++) {
    DEBUG ((DEBUG_INFO,
      "      Device ............................................. 0x%02x\n",
      PciPath[PciPathIndex].Device
      ));
    DEBUG ((DEBUG_INFO,
      "      Function ........................................... 0x%02x\n",
      PciPath[PciPathIndex].Function
      ));
  }

  DEBUG ((DEBUG_INFO,
    "    *************************************************************************\n\n"
    ));

  return;
}

/**
  Dump DMAR SIDP table.

  @param[in]  Sidp  DMAR SIDP table
**/
VOID
DumpDmarSidp (
  IN EFI_ACPI_DMAR_SIDP_HEADER *Sidp
  )
{
  EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER   *DmarDeviceScopeEntry;
  INTN                                          SidpLen;

  if (Sidp == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  *       SoC Integrated Device Property Reporting Structure                *\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    (sizeof(UINTN) == sizeof(UINT64)) ?
    "  SIDP address ........................................... 0x%016lx\n" :
    "  SIDP address ........................................... 0x%08x\n",
    Sidp
    ));
  DEBUG ((DEBUG_INFO,
    "    Type ................................................. 0x%04x\n",
    Sidp->Header.Type
    ));
  DEBUG ((DEBUG_INFO,
    "    Length ............................................... 0x%04x\n",
    Sidp->Header.Length
    ));
  DEBUG ((DEBUG_INFO,
    "    Segment Number ....................................... 0x%04x\n",
    Sidp->SegmentNumber
    ));

  SidpLen  = Sidp->Header.Length - sizeof(EFI_ACPI_DMAR_SIDP_HEADER);
  DmarDeviceScopeEntry = (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER *)(Sidp + 1);
  while (SidpLen > 0) {
    DumpDmarDeviceScopeEntry (DmarDeviceScopeEntry);
    SidpLen -= DmarDeviceScopeEntry->Length;
    DmarDeviceScopeEntry = (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER *)((UINTN)DmarDeviceScopeEntry + DmarDeviceScopeEntry->Length);
  }

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n\n"
    ));

  return;
}

/**
  Dump DMAR SATC table.

  @param[in]  Satc  DMAR SATC table
**/
VOID
DumpDmarSatc (
  IN EFI_ACPI_DMAR_SATC_HEADER *Satc
  )
{
  EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER       *DmarDeviceScopeEntry;
  INTN                                    SatcLen;

  if (Satc == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  *       ACPI Soc Integrated Address Translation Cache reporting Structure *\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    (sizeof(UINTN) == sizeof(UINT64)) ?
    "  SATC address ........................................... 0x%016lx\n" :
    "  SATC address ........................................... 0x%08x\n",
    Satc
    ));
  DEBUG ((DEBUG_INFO,
    "    Type ................................................. 0x%04x\n",
    Satc->Header.Type
    ));
  DEBUG ((DEBUG_INFO,
    "    Length ............................................... 0x%04x\n",
    Satc->Header.Length
    ));
  DEBUG ((DEBUG_INFO,
    "    Flags ................................................ 0x%02x\n",
    Satc->Flags
    ));
  DEBUG ((DEBUG_INFO,
    "    Segment Number ....................................... 0x%04x\n",
    Satc->SegmentNumber
    ));

  SatcLen  = Satc->Header.Length - sizeof(EFI_ACPI_DMAR_SATC_HEADER);
  DmarDeviceScopeEntry = (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER *)(Satc + 1);
  while (SatcLen > 0) {
    DumpDmarDeviceScopeEntry (DmarDeviceScopeEntry);
    SatcLen -= DmarDeviceScopeEntry->Length;
    DmarDeviceScopeEntry = (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER *)((UINTN)DmarDeviceScopeEntry + DmarDeviceScopeEntry->Length);
  }

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n\n"
    ));

  return;
}

/**
  Dump DMAR ANDD table.

  @param[in]  Andd  DMAR ANDD table
**/
VOID
DumpDmarAndd (
  IN EFI_ACPI_DMAR_ANDD_HEADER *Andd
  )
{
  if (Andd == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  *       ACPI Name-space Device Declaration Structure                      *\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    (sizeof(UINTN) == sizeof(UINT64)) ?
    "  ANDD address ........................................... 0x%016lx\n" :
    "  ANDD address ........................................... 0x%08x\n",
    Andd
    ));
  DEBUG ((DEBUG_INFO,
    "    Type ................................................. 0x%04x\n",
    Andd->Header.Type
    ));
  DEBUG ((DEBUG_INFO,
    "    Length ............................................... 0x%04x\n",
    Andd->Header.Length
    ));
  DEBUG ((DEBUG_INFO,
    "    ACPI Device Number ................................... 0x%02x\n",
    Andd->AcpiDeviceNumber
    ));
  DEBUG ((DEBUG_INFO,
    "    ACPI Object Name ..................................... '%a'\n",
    (Andd + 1)
    ));

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n\n"
    ));

  return;
}

/**
  Dump DMAR RHSA table.

  @param[in]  Rhsa  DMAR RHSA table
**/
VOID
DumpDmarRhsa (
  IN EFI_ACPI_DMAR_RHSA_HEADER *Rhsa
  )
{
  if (Rhsa == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  *       Remapping Hardware Status Affinity Structure                      *\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    (sizeof(UINTN) == sizeof(UINT64)) ?
    "  RHSA address ........................................... 0x%016lx\n" :
    "  RHSA address ........................................... 0x%08x\n",
    Rhsa
    ));
  DEBUG ((DEBUG_INFO,
    "    Type ................................................. 0x%04x\n",
    Rhsa->Header.Type
    ));
  DEBUG ((DEBUG_INFO,
    "    Length ............................................... 0x%04x\n",
    Rhsa->Header.Length
    ));
  DEBUG ((DEBUG_INFO,
    "    Register Base Address ................................ 0x%016lx\n",
    Rhsa->RegisterBaseAddress
    ));
  DEBUG ((DEBUG_INFO,
    "    Proximity Domain ..................................... 0x%08x\n",
    Rhsa->ProximityDomain
    ));

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n\n"
    ));

  return;
}

/**
  Dump DMAR ATSR table.

  @param[in]  Atsr  DMAR ATSR table
**/
VOID
DumpDmarAtsr (
  IN EFI_ACPI_DMAR_ATSR_HEADER *Atsr
  )
{
  EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER       *DmarDeviceScopeEntry;
  INTN                                    AtsrLen;

  if (Atsr == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  *       Root Port ATS Capability Reporting Structure                      *\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    (sizeof(UINTN) == sizeof(UINT64)) ?
    "  ATSR address ........................................... 0x%016lx\n" :
    "  ATSR address ........................................... 0x%08x\n",
    Atsr
    ));
  DEBUG ((DEBUG_INFO,
    "    Type ................................................. 0x%04x\n",
    Atsr->Header.Type
    ));
  DEBUG ((DEBUG_INFO,
    "    Length ............................................... 0x%04x\n",
    Atsr->Header.Length
    ));
  DEBUG ((DEBUG_INFO,
    "    Flags ................................................ 0x%02x\n",
    Atsr->Flags
    ));
  DEBUG ((DEBUG_INFO,
    "      ALL_PORTS .......................................... 0x%02x\n",
    Atsr->Flags & EFI_ACPI_DMAR_ATSR_FLAGS_ALL_PORTS
    ));
  DEBUG ((DEBUG_INFO,
    "    Segment Number ....................................... 0x%04x\n",
    Atsr->SegmentNumber
    ));

  AtsrLen  = Atsr->Header.Length - sizeof(EFI_ACPI_DMAR_ATSR_HEADER);
  DmarDeviceScopeEntry = (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER *)(Atsr + 1);
  while (AtsrLen > 0) {
    DumpDmarDeviceScopeEntry (DmarDeviceScopeEntry);
    AtsrLen -= DmarDeviceScopeEntry->Length;
    DmarDeviceScopeEntry = (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER *)((UINTN)DmarDeviceScopeEntry + DmarDeviceScopeEntry->Length);
  }

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n\n"
    ));

  return;
}

/**
  Dump DMAR RMRR table.

  @param[in]  Rmrr  DMAR RMRR table
**/
VOID
DumpDmarRmrr (
  IN EFI_ACPI_DMAR_RMRR_HEADER *Rmrr
  )
{
  EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER       *DmarDeviceScopeEntry;
  INTN                                    RmrrLen;

  if (Rmrr == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  *       Reserved Memory Region Reporting Structure                        *\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    (sizeof(UINTN) == sizeof(UINT64)) ?
    "  RMRR address ........................................... 0x%016lx\n" :
    "  RMRR address ........................................... 0x%08x\n",
    Rmrr
    ));
  DEBUG ((DEBUG_INFO,
    "    Type ................................................. 0x%04x\n",
    Rmrr->Header.Type
    ));
  DEBUG ((DEBUG_INFO,
    "    Length ............................................... 0x%04x\n",
    Rmrr->Header.Length
    ));
  DEBUG ((DEBUG_INFO,
    "    Segment Number ....................................... 0x%04x\n",
    Rmrr->SegmentNumber
    ));
  DEBUG ((DEBUG_INFO,
    "    Reserved Memory Region Base Address .................. 0x%016lx\n",
    Rmrr->ReservedMemoryRegionBaseAddress
    ));
  DEBUG ((DEBUG_INFO,
    "    Reserved Memory Region Limit Address ................. 0x%016lx\n",
    Rmrr->ReservedMemoryRegionLimitAddress
    ));

  RmrrLen  = Rmrr->Header.Length - sizeof(EFI_ACPI_DMAR_RMRR_HEADER);
  DmarDeviceScopeEntry = (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER *)(Rmrr + 1);
  while (RmrrLen > 0) {
    DumpDmarDeviceScopeEntry (DmarDeviceScopeEntry);
    RmrrLen -= DmarDeviceScopeEntry->Length;
    DmarDeviceScopeEntry = (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER *)((UINTN)DmarDeviceScopeEntry + DmarDeviceScopeEntry->Length);
  }

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n\n"
    ));

  return;
}

/**
  Dump DMAR DRHD table.

  @param[in]  Drhd  DMAR DRHD table
**/
VOID
DumpDmarDrhd (
  IN EFI_ACPI_DMAR_DRHD_HEADER *Drhd
  )
{
  EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER       *DmarDeviceScopeEntry;
  INTN                                    DrhdLen;

  if (Drhd == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  *       DMA-Remapping Hardware Definition Structure                       *\n"
    ));
  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    (sizeof(UINTN) == sizeof(UINT64)) ?
    "  DRHD address ........................................... 0x%016lx\n" :
    "  DRHD address ........................................... 0x%08x\n",
    Drhd
    ));
  DEBUG ((DEBUG_INFO,
    "    Type ................................................. 0x%04x\n",
    Drhd->Header.Type
    ));
  DEBUG ((DEBUG_INFO,
    "    Length ............................................... 0x%04x\n",
    Drhd->Header.Length
    ));
  DEBUG ((DEBUG_INFO,
    "    Flags ................................................ 0x%02x\n",
    Drhd->Flags
    ));
  DEBUG ((DEBUG_INFO,
    "      INCLUDE_PCI_ALL .................................... 0x%02x\n",
    Drhd->Flags & EFI_ACPI_DMAR_DRHD_FLAGS_INCLUDE_PCI_ALL
    ));
  DEBUG ((DEBUG_INFO,
    "    Size ................................................. 0x%02x\n",
    Drhd->Size
    ));
  DEBUG ((DEBUG_INFO,
    "    Segment Number ....................................... 0x%04x\n",
    Drhd->SegmentNumber
    ));
  DEBUG ((DEBUG_INFO,
    "    Register Base Address ................................ 0x%016lx\n",
    Drhd->RegisterBaseAddress
    ));

  DrhdLen  = Drhd->Header.Length - sizeof(EFI_ACPI_DMAR_DRHD_HEADER);
  DmarDeviceScopeEntry = (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER *)(Drhd + 1);
  while (DrhdLen > 0) {
    DumpDmarDeviceScopeEntry (DmarDeviceScopeEntry);
    DrhdLen -= DmarDeviceScopeEntry->Length;
    DmarDeviceScopeEntry = (EFI_ACPI_DMAR_DEVICE_SCOPE_STRUCTURE_HEADER *)((UINTN)DmarDeviceScopeEntry + DmarDeviceScopeEntry->Length);
  }

  DEBUG ((DEBUG_INFO,
    "  ***************************************************************************\n\n"
    ));

  return;
}

/**
  Dump DMAR ACPI table.

  @param[in]  Dmar  DMAR ACPI table
**/
VOID
DumpAcpiDMAR (
  IN EFI_ACPI_DMAR_HEADER  *Dmar
  )
{
  EFI_ACPI_DMAR_STRUCTURE_HEADER *DmarHeader;
  INTN                  DmarLen;

  if (Dmar == NULL) {
    return;
  }

  //
  // Dump Dmar table
  //
  DEBUG ((DEBUG_INFO,
    "*****************************************************************************\n"
    ));
  DEBUG ((DEBUG_INFO,
    "*         DMAR Table                                                        *\n"
    ));
  DEBUG ((DEBUG_INFO,
    "*****************************************************************************\n"
    ));

  DEBUG ((DEBUG_INFO,
    (sizeof(UINTN) == sizeof(UINT64)) ?
    "DMAR address ............................................. 0x%016lx\n" :
    "DMAR address ............................................. 0x%08x\n",
    Dmar
    ));

  DEBUG ((DEBUG_INFO,
    "  Table Contents:\n"
    ));
  DEBUG ((DEBUG_INFO,
    "    Host Address Width ................................... 0x%02x\n",
    Dmar->HostAddressWidth
    ));
  DEBUG ((DEBUG_INFO,
    "    Flags ................................................ 0x%02x\n",
    Dmar->Flags
    ));
  DEBUG ((DEBUG_INFO,
    "      INTR_REMAP ......................................... 0x%02x\n",
    Dmar->Flags & EFI_ACPI_DMAR_FLAGS_INTR_REMAP
    ));
  DEBUG ((DEBUG_INFO,
    "      X2APIC_OPT_OUT_SET ................................. 0x%02x\n",
    Dmar->Flags & EFI_ACPI_DMAR_FLAGS_X2APIC_OPT_OUT
    ));
  DEBUG ((DEBUG_INFO,
    "      DMA_CTRL_PLATFORM_OPT_IN_FLAG ...................... 0x%02x\n",
    Dmar->Flags & EFI_ACPI_DMAR_FLAGS_DMA_CTRL_PLATFORM_OPT_IN_FLAG
    ));

  DmarLen  = Dmar->Header.Length - sizeof(EFI_ACPI_DMAR_HEADER);
  DmarHeader = (EFI_ACPI_DMAR_STRUCTURE_HEADER *)(Dmar + 1);
  while (DmarLen > 0) {
    switch (DmarHeader->Type) {
    case EFI_ACPI_DMAR_TYPE_DRHD:
      DumpDmarDrhd ((EFI_ACPI_DMAR_DRHD_HEADER *)DmarHeader);
      break;
    case EFI_ACPI_DMAR_TYPE_RMRR:
      DumpDmarRmrr ((EFI_ACPI_DMAR_RMRR_HEADER *)DmarHeader);
      break;
    case EFI_ACPI_DMAR_TYPE_ATSR:
      DumpDmarAtsr ((EFI_ACPI_DMAR_ATSR_HEADER *)DmarHeader);
      break;
    case EFI_ACPI_DMAR_TYPE_RHSA:
      DumpDmarRhsa ((EFI_ACPI_DMAR_RHSA_HEADER *)DmarHeader);
      break;
    case EFI_ACPI_DMAR_TYPE_ANDD:
      DumpDmarAndd ((EFI_ACPI_DMAR_ANDD_HEADER *)DmarHeader);
      break;
    case EFI_ACPI_DMAR_TYPE_SATC:
      DumpDmarSatc ((EFI_ACPI_DMAR_SATC_HEADER *)DmarHeader);
      break;
    case EFI_ACPI_DMAR_TYPE_SIDP:
      DumpDmarSidp ((EFI_ACPI_DMAR_SIDP_HEADER *)DmarHeader);
      break;
    default:
      DEBUG ((DEBUG_INFO, "Unknown DMAR Table Type : %d\n", DmarHeader->Type));
      break;
    }
    DmarLen -= DmarHeader->Length;
    DmarHeader = (EFI_ACPI_DMAR_STRUCTURE_HEADER *)((UINTN)DmarHeader + DmarHeader->Length);
  }

  DEBUG ((DEBUG_INFO,
    "*****************************************************************************\n\n"
    ));

  return;
}
