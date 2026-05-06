/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PLATFORM_BASE_H_
#define _PLATFORM_BASE_H_

#define ACPI_BASE_ADDRESS           0x500
#define TCO_BASE_ADDRESS            0x400
#define MAX_SOCKET                  1
#define MAX_IO_APICS                1
#define MAX_IMC                     2                       // Maximum memory controllers per socket

#define MAX_IIO_STACK               14
#define IIO_STACK0                  0

#define PCH_IOAPIC                  (1 << 0)

#define PCH_IOAPIC_ID               0x08
#define PCH_IOAPIC_ADDRESS          0xFEC00000
#define HPET_BLOCK_ADDRESS          0xFED00000
#define PCH_INTERRUPT_BASE          0

#define NGN_MAX_SERIALNUMBER_STRLEN 4  // Max serial number string length
#define NGN_MAX_PARTNUMBER_STRLEN   30 // Max part number string length
#define MAX_DIMM                    2  // Max DIMM per channel
#define MAX_MC_CH                   1  // Max number of channels per MC
#endif
