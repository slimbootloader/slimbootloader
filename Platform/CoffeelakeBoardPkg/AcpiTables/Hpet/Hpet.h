/** @file
  This file describes the contents of the ACPI High Precision Event Timer Description Table
  (HPET).  All changes to the HPET contents should be done in this file.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#ifndef _HPET_H_
#define _HPET_H_

//
// Statements that include other files
//

#include <IndustryStandard/Acpi30.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>

//
// HPET Definitions
//

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

#define EFI_ACPI_HPET_NUMBER                            0x00

#define EFI_ACPI_MIN_CLOCK_TICK                         0x0080

#define EFI_ACPI_HPET_ATTRIBUTES                        0x00

#endif
