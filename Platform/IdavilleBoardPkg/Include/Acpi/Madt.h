/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef _MADT_H_
#define _MADT_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi.h>
#include <PlatformBase.h>

//
// MADT Definitions
//
#define EFI_ACPI_OEM_MADT_REVISION                    0x00000000
//
// Multiple APIC Flags are defined in AcpiX.0.h
//
#define EFI_ACPI_6_2_MULTIPLE_APIC_FLAGS              (EFI_ACPI_6_2_PCAT_COMPAT)

//
// Local APIC address
//
#define EFI_ACPI_LOCAL_APIC_ADDRESS                   0xFEE00000
//
// Define the number of each table type.
// This is where the table layout is modified.
//
#define EFI_ACPI_PROCESSOR_LOCAL_APIC_COUNT           1 // for initial madt.aslc
#define EFI_ACPI_LOCAL_APIC_NMI_COUNT                 1 // for initial madt.aslc
#define EFI_ACPI_PROCESSOR_LOCAL_X2APIC_COUNT         0 // for initial madt.aslc
#define EFI_ACPI_LOCAL_X2APIC_NMI_COUNT               0 // for initial madt.aslc
#define EFI_ACPI_IO_APIC_COUNT                        MAX_IO_APICS
#define EFI_ACPI_INTERRUPT_SOURCE_OVERRIDE_COUNT      2
#define EFI_ACPI_NON_MASKABLE_INTERRUPT_SOURCE_COUNT  0
#define EFI_ACPI_LOCAL_APIC_ADDRESS_OVERRIDE_COUNT    0
#define EFI_ACPI_IO_SAPIC_COUNT                       0
#define EFI_ACPI_PROCESSOR_LOCAL_SAPIC_COUNT          0
#define EFI_ACPI_PLATFORM_INTERRUPT_SOURCES_COUNT     0

//
// Define interrupt polarity and trigger mode
//
#define POLARITY_ACTIVE_HIGH                          0x01
#define TRIGGERMODE_EDGE                              0x04

//
// MADT structure
//
//
// Ensure proper structure formats
//
#pragma pack(1)
//
// ACPI 4.0 Table structure
//
typedef struct {
  EFI_ACPI_6_2_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER   Header;

#if EFI_ACPI_PROCESSOR_LOCAL_APIC_COUNT > 0              // Type 0x00
  EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_STRUCTURE           LocalApic[EFI_ACPI_PROCESSOR_LOCAL_APIC_COUNT];
#endif

#if EFI_ACPI_IO_APIC_COUNT > 0                          // Type 0x01
  EFI_ACPI_6_2_IO_APIC_STRUCTURE                        IoApic[EFI_ACPI_IO_APIC_COUNT];
#endif

#if EFI_ACPI_INTERRUPT_SOURCE_OVERRIDE_COUNT > 0        // Type 0x02
  EFI_ACPI_6_2_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE      Iso[EFI_ACPI_INTERRUPT_SOURCE_OVERRIDE_COUNT];
#endif

#if EFI_ACPI_NON_MASKABLE_INTERRUPT_SOURCE_COUNT > 0    // Type 0x03
  EFI_ACPI_6_2_NON_MASKABLE_INTERRUPT_SOURCE_STRUCTURE  NmiSource[EFI_ACPI_NON_MASKABLE_INTERRUPT_SOURCE_COUNT];
#endif

#if EFI_ACPI_LOCAL_APIC_NMI_COUNT > 0                    // Type 0x04
  EFI_ACPI_6_2_LOCAL_APIC_NMI_STRUCTURE                 LocalApicNmi;
#endif

#if EFI_ACPI_LOCAL_APIC_ADDRESS_OVERRIDE_COUNT > 0      // Type 0x05
  EFI_ACPI_6_2_LOCAL_APIC_ADDRESS_OVERRIDE_STRUCTURE    LocalApicOverride[EFI_ACPI_LOCAL_APIC_OVERRIDE_COUNT];
#endif

#if EFI_ACPI_IO_SAPIC_COUNT > 0                          // Type 0x06
  EFI_ACPI_6_2_IO_SAPIC_STRUCTURE                       IoSapic[EFI_ACPI_IO_SAPIC_COUNT];
#endif

#if EFI_ACPI_PROCESSOR_LOCAL_SAPIC_COUNT > 0            // Type 0x07 : This table changes in madt 2.0
  EFI_ACPI_6_2_PROCESSOR_LOCAL_SAPIC_STRUCTURE          LocalSapic[EFI_ACPI_PROCESSOR_LOCAL_SAPIC_COUNT];
#endif

#if EFI_ACPI_PLATFORM_INTERRUPT_SOURCES_COUNT > 0        // Type 0x08
  EFI_ACPI_6_2_PLATFORM_INTERRUPT_SOURCES_STRUCTURE     PlatformInterruptSources[
    EFI_ACPI_PLATFORM_INTERRUPT_SOURCES_COUNT];
#endif

#if EFI_ACPI_PROCESSOR_LOCAL_X2APIC_COUNT  > 0          //Type 0x09
  EFI_ACPI_6_2_PROCESSOR_LOCAL_X2APIC_STRUCTURE         LocalX2Apic[EFI_ACPI_PROCESSOR_LOCAL_X2APIC_COUNT];
#endif

#if EFI_ACPI_LOCAL_X2APIC_NMI_COUNT > 0                  //Type 0x0A
  EFI_ACPI_6_2_LOCAL_X2APIC_NMI_STRUCTURE                X2ApicNmi;
#endif
} EFI_ACPI_6_2_MULTIPLE_APIC_DESCRIPTION_TABLE;

#pragma pack()

#endif
