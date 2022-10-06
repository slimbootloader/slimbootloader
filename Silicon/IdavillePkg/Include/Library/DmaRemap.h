/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _EFI_DMA_REMAPPING_REPORTING_TABLE_H_
#define _EFI_DMA_REMAPPING_REPORTING_TABLE_H_

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/DmaRemappingReportingTable.h>

//
// "DMAR" DMAR Description Table Signature
//
#define EFI_ACPI_DMAR_DESCRIPTION_TABLE_SIGNATURE  0x52414d44


//
// Ensure proper structure formats
//
#pragma pack (1)

//
// Definition for DMA Remapping Structure Types
//
#define EFI_ACPI_DMA_REMAPPING_STRUCTURE_TYPE_DRHD  0
#define EFI_ACPI_DMA_REMAPPING_STRUCTURE_TYPE_RMRR  1
#define EFI_ACPI_DMA_REMAPPING_STRUCTURE_TYPE_ATSR  2
#define EFI_ACPI_DMA_REMAPPING_STRUCTURE_TYPE_RHSA  3


typedef struct {
  UINT8         Device;
  UINT8         Function;
} PCI_NODE;

typedef struct {
  UINT8         DeviceType;
  UINT8         EnumerationID;
  UINT8         StartBusNumber;
  CONST PCI_NODE *PciNode;
} DEVICE_SCOPE;

//
// Definition for DMA-Remapping Device Scope Entry Structure
//
#define EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT                 0x01
#define EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE                   0x02
#define EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_IOAPIC                   0x03
#define EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_MSI_CAPABLE_HPET         0x04
typedef struct {
  UINT8                   DeviceScopeEntryType;
  UINT8                   Length;
  UINT16                  Reserved_2;
  UINT8                   EnumerationID;
  UINT8                   StartingBusNumber;
} EFI_ACPI_DMAR_DEVICE_SCOPE_ENTRY_STRUCTURE;

//
// Definition for DMA-Remapping Hardware Definition (DRHD) Structure
//
#define EFI_ACPI_DMAR_DRHD_FLAGS_INCLUDE_ALL_SET     0x1
#define EFI_ACPI_DMAR_DRHD_FLAGS_INCLUDE_ALL_CLEAR   0x0
typedef struct {
  UINT16                                      Type;
  UINT16                                      Length;
  UINT8                                       Flags;
  UINT8                                       Reserved_5;
  UINT16                                      SegmentNumber;
  UINT64                                      RegisterBaseAddress;
} EFI_ACPI_DMAR_HARDWARE_UNIT_DEFINITION_STRUCTURE;

//
// Definition for Reserved Memory Region Reporting (RMRR) Structure
//
typedef struct {
  UINT16                                      Type;
  UINT16                                      Length;
  UINT8                                       Reserved_4[2];
  UINT16                                      SegmentNumber;
  UINT64                                      ReservedMemoryRegionBaseAddress;
  UINT64                                      ReservedMemoryRegionLimitAddress;
} EFI_ACPI_DMAR_RESERVED_MEMORY_REGION_REPORTING_STRUCTURE;

//
// Definition for Root Port ATS Capability Reporting (ATSR) Structure
//
typedef struct {
  UINT16                                      Type;
  UINT16                                      Length;
  UINT8                                       Flags;
  UINT8                                       Reserved_5;
  UINT16                                      SegmentNumber;
} EFI_ACPI_DMAR_ROOT_PORT_ATS_CAPABILITY_REPORTING_STRUCTURE;

//
// Definition for Remapping Hardware Static Affinity(RHSA) Structure
//
typedef struct {
  UINT16                                      Type;
  UINT16                                      Length;
  UINT32                                      Reserved;
  UINT64                                      RegisterBaseAddress;
  UINT32                                      ProximityDomain;
} EFI_ACPI_DMAR_REMAPPING_HARDWARE_STATIC_AFFINITY_STRUCTURE;

//
// Definition for DMA Remapping Structure
//
typedef union {
  EFI_ACPI_DMAR_STRUCTURE_HEADER                               DMARStructureHeader;
  EFI_ACPI_DMAR_HARDWARE_UNIT_DEFINITION_STRUCTURE             DMARHardwareUnitDefinition;
  EFI_ACPI_DMAR_RESERVED_MEMORY_REGION_REPORTING_STRUCTURE     DMARReservedMemoryRegionReporting;
  EFI_ACPI_DMAR_ROOT_PORT_ATS_CAPABILITY_REPORTING_STRUCTURE   DMARRootPortATSCapabilityReporting;
} EFI_ACPI_DMA_REMAPPING_STRUCTURE;

//
// Definition for DMA-Remapping Reporting ACPI Table
//
#define EFI_ACPI_DMAR_TABLE_FLAGS_INTR_REMAP_CLEAR              0x00
#define EFI_ACPI_DMAR_TABLE_FLAGS_INTR_REMAP_SET                0x01
#define EFI_ACPI_DMAR_TABLE_FLAGS_X2APIC_OPT_OUT                0x02


typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER                               Header;
  UINT8                                                     HostAddressWidth;
  UINT8                                                     Flags;
  UINT8                                                     Reserved_38[10];
} EFI_ACPI_DMAR_DESCRIPTION_TABLE;


#pragma pack()

#endif
