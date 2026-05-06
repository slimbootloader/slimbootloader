/** @file
  Basic SoC IIO topology definitions.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _IIOSOCCFGGNRD_H_
#define _IIOSOCCFGGNRD_H_

/**
 * Maximum number of IIO stacks per socket.
 * Logical stacks include uncore stacks (UBOX).
 */

#define MAX_IIO_STACKS_PER_SOCKET    14
#define MAX_IIO_PCIROOTS_PER_STACK   5   // PCI roots that can be created for a stack
#define MAX_IIO_PCIE_PER_SOCKET      3
#define MAX_IIO_IOAT_PER_SOCKET      8   // Needs to be updated
#define MAX_IIO_STACKS_AMONG_SOC     18

#endif // _IIOSOCCFGGNRD_H_
