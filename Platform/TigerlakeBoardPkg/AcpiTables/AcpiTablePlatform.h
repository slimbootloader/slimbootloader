/*++

Copyright (c)  1999  - 2019, Intel Corporation. All rights reserved

  SPDX-License-Identifier: BSD-2-Clause-Patent


Module Name:

  AcpiTablePlatform.h


Abstract: File contains platform specific ACPI defines for use in ACPI tables


--*/
#ifndef __ACPI_PLATFORM_H__
#define __ACPI_PLATFORM_H__

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <Mcfg.h>
#include "PlatformBase.h"

//
// ACPI table information used to initialize tables.
//
#define EFI_ACPI_OEM_ID           'O','E','M','I','D',' '   // OEMID 6 bytes long
#define EFI_ACPI_OEM_TABLE_ID     SIGNATURE_64('O','E','M','T','A','B','L','E') // OEM table id 8 bytes long
#define EFI_ACPI_OEM_REVISION     0x00000005
#define EFI_ACPI_CREATOR_ID       SIGNATURE_32('C','R','E','A')
#define EFI_ACPI_CREATOR_REVISION 0x0100000D

#define IAPC_BOOT_ARCH  ( EFI_ACPI_4_0_VGA_NOT_PRESENT | EFI_ACPI_4_0_8042 | EFI_ACPI_4_0_LEGACY_DEVICES)

#define RESERVED        0x00

#define HPET_BASE_ADDRESS                       0xFED00000


#define LOCAL_APIC_BASE_ADDRESS                 0xFEE00000

#define IO_APIC_BASE_ADDRESS                    0xFEC00000
#define ICH_IOAPIC_ID                           0x02

#define EFI_ACPI_EVENT_TIMER_BLOCK_ID           0x8086A201

#endif /* __ACPI_PLATFORM_H__ */
