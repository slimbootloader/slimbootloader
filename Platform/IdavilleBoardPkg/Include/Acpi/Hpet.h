/** @file
  This file describes the contents of the ACPI High Precision Event Timer Description Table
  (HPET).  All changes to the HPET contents should be done in this file.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __ACPI_HPET_H__
#define __ACPI_HPET_H__

//
// HPET Definitions
//
#define EFI_ACPI_OEM_HPET_REVISION                  0x00000001

#define EFI_ACPI_EVENT_TIMER_BLOCK_ID               0x8086A701

//
// Event Timer Block Base Address Information
//
#define EFI_ACPI_EVENT_TIMER_BLOCK_ADDRESS_SPACE_ID EFI_ACPI_3_0_SYSTEM_MEMORY
#define EFI_ACPI_EVENT_TIMER_BLOCK_BIT_WIDTH        0x00
#define EFI_ACPI_EVENT_TIMER_BLOCK_BIT_OFFSET       0x00
#define EFI_ACPI_EVENT_TIMER_ACCESS_SIZE            0x00
#define EFI_ACPI_EVENT_TIMER_BLOCK_ADDRESS          0x00000000FED00000

#define EFI_ACPI_HPET_NUMBER                        0x00

#define EFI_ACPI_MIN_CLOCK_TICK                     0x0080

#define EFI_ACPI_HPET_ATTRIBUTES                    0x00

#endif /* __ACPI_HPET_H__ */
