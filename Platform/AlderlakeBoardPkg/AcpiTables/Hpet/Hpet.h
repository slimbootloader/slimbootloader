/** @file
  This file describes the contents of the ACPI High Precision Event Timer Description Table
  (HPET).  All changes to the HPET contents should be done in this file.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _HPET_H_
#define _HPET_H_


#include <IndustryStandard/Acpi30.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <Acpi/AcpiCommon.h>

#define EFI_ACPI_OEM_HPET_REVISION                      0x00000001

#define EFI_ACPI_EVENT_TIMER_BLOCK_ID                   0x8086A201

//
// Event Timer Block Base Address Information
//
#define EFI_ACPI_EVENT_TIMER_BLOCK_ADDRESS_SPACE_ID     EFI_ACPI_3_0_SYSTEM_MEMORY
#define EFI_ACPI_EVENT_TIMER_BLOCK_BIT_WIDTH            0x40
#define EFI_ACPI_EVENT_TIMER_BLOCK_BIT_OFFSET           0x00
#define EFI_ACPI_EVENT_TIMER_ACCESS_SIZE                0x00
#define EFI_ACPI_EVENT_TIMER_BLOCK_ADDRESS              0x00000000FED00000

#define EFI_ACPI_MIN_CLOCK_TICK                         0x0080

#define EFI_ACPI_HPET_ATTRIBUTES                        0x00

#endif
