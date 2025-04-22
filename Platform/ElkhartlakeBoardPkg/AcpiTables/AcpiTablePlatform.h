/**@file
  File contains platform specific ACPI defines for use in ACPI tables

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _Platform_h_INCLUDED_
#define _Platform_h_INCLUDED_

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <PlatformBase.h>
#include <Mcfg.h>

//
// ACPI table information used to initialize tables.
//
#define EFI_ACPI_OEM_ID           'O','E','M','I','D',' '   // OEMID 6 bytes long
#define EFI_ACPI_OEM_TABLE_ID     SIGNATURE_64('O','E','M','T','A','B','L','E') // OEM table id 8 bytes long
#define EFI_ACPI_OEM_REVISION     0x00000005
#define EFI_ACPI_CREATOR_ID       SIGNATURE_32('C','R','E','A')
#define EFI_ACPI_CREATOR_REVISION 0x0100000D

#define ICH_IOAPIC_ID   0x02
#define INT_MODEL       0x01

#define PLATFORM_MAX_BUS_NUM                    0xFF
#define MAIN_COUNTER_MIN_PERIODIC_CLOCK_TICKS   0x80        //approx 1ms

#endif
