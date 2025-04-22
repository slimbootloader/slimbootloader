/** @file
  Private data structures and function prototypes.

  Copyright (c) 1999 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#define EFI_IDIV_ROUND(r, s)         ((r) / (s) + (((2 * ((r) % (s))) < (s)) ? 0 : 1))
#define MWAIT_C6                     0x20
#define MWAIT_C6_1                   0x21
#define MWAIT_C7                     0x30
#define MWAIT_C7_1                   0x31
#define MWAIT_C7_2                   0x32
#define MWAIT_C7_3                   0x33
#define MWAIT_CD                     0x40
#define MWAIT_CD_1                   0x50
#define MWAIT_CD_2                   0x60
#define DTS_IO_TRAP_ADDRESS          0x810
#define DTS_IO_TRAP_LENGTH           4
#define DTS_ACPI_DISABLE             0
#define LATENCY_C6                   127
#define LATENCY_C7                   253
#define LATENCY_C8                   260
#define LATENCY_C9                   487
#define LATENCY_C10                  1048
