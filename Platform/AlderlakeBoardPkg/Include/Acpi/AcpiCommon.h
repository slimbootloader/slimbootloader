/** @file
  This file provides common definations used by ACPI and C code.

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _ACPI_COMMON_H_
#define _ACPI_COMMON_H_

// For the Enumeration ID in the Device Scope Structure
// When the scope 'Type' field indicates 'IOAPIC', this field provides
// the I/O APICID as provided in the I/O APIC structure in the ACPI MADT.
// When the scope 'Type' field indicates 'MSI_CAPABLE_HPET', this field
// provides the 'HPET Number' as provided in the ACPI HPET structure for
// the corresponding Timer Block.
#define ICH_IOAPIC_ID                           0x02
#define EFI_ACPI_HPET_NUMBER                    0x00

#endif
