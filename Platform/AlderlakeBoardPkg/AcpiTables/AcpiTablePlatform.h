/**@file
  File contains platform specific ACPI defines for use in ACPI tables

  Copyright (c)  2021, Intel Corporation. All rights reserved
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _ACPI_PLATFORM_H_
#define _ACPI_PLATFORM_H_

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <PlatformBase.h>
#include <Mcfg.h>
#include <Acpi/AcpiCommon.h>

//
// ACPI table information used to initialize tables.
//
#define EFI_ACPI_OEM_ID             'O','E','M','I','D',' '   // OEMID 6 bytes long
#define EFI_ACPI_OEM_TABLE_ID       SIGNATURE_64('O','E','M','T','A','B','L','E') // OEM table id 8 bytes long
#define EFI_ACPI_OEM_REVISION       0x00000005
#define EFI_ACPI_CREATOR_ID         SIGNATURE_32('C','R','E','A')
#define EFI_ACPI_CREATOR_REVISION   0x0100000D


#define MAX_THREAD                  2
#define MAX_CORE                    12 // 24

#endif
