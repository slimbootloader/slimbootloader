/*++

  Copyright (c)  2019 - 2021, Intel Corporation. All rights reserved

  SPDX-License-Identifier: BSD-2-Clause-Patent


Module Name:

  AcpiTablePlatform.h

Abstract: File contains platform specific ACPI defines for use in ACPI tables

--*/

#ifndef __ACPI_PLATFORM_H__
#define __ACPI_PLATFORM_H__

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <PlatformBase.h>
#include <Mcfg.h>
#include "Facp.h"
#include "Hpet.h"
#include "Madt.h"
#include "WatchdogDescriptionTable.h"
#include "Wddt.h"
#include "Wsmt.h"

//
// ACPI table information used to initialize tables.
//
#define EFI_ACPI_OEM_ID             'O','E','M','I','D',' '   // OEMID 6 bytes long
#define EFI_ACPI_OEM_TABLE_ID       SIGNATURE_64('O','E','M','T','A','B','L','E') // OEM table id 8 bytes long
#define EFI_ACPI_OEM_REVISION       0x00000005
#define EFI_ACPI_CREATOR_ID         SIGNATURE_32('C','R','E','A')
#define EFI_ACPI_CREATOR_REVISION   0x0100000D

#define OEM1_SSDT_TABLE_SIGNATURE   SIGNATURE_32('O', 'E', 'M', '1')

#define ME_SPS_SUPPORT              1

#endif /* __ACPI_PLATFORM_H__ */
