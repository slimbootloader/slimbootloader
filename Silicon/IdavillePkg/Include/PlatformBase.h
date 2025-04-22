/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PLATFORM_BASE_H_
#define _PLATFORM_BASE_H_

#define ACPI_BASE_ADDRESS           0x500
#define TCO_BASE_ADDRESS            0x400
#define PCH_PWRM_BASE_ADDRESS       0xFE000000     ///< PMC MBAR MMIO base address

#define MAX_SOCKET                  1
#define MAX_IO_APICS                1
#define MAX_IMC                     2                       // Maximum memory controllers per socket
#define MC_MAX_NODE                 (MAX_SOCKET * MAX_IMC)  // Max number of memory nodes

#define MAX_IIO_STACK               6
#define IIO_STACK0                  0
#define IIO_STACK1                  1
#define IIO_STACK2                  2
#define IIO_STACK3                  3
#define IIO_STACK5                  5
#define MAX_LOGIC_IIO_STACK         8

#define PCH_IOAPIC                  (1 << 0)

#define PCH_IOAPIC_ID               0x08
#define PCH_IOAPIC_ADDRESS          0xFEC00000
#define HPET_BLOCK_ADDRESS          0xFED00000
#define PCH_INTERRUPT_BASE          0

#endif
