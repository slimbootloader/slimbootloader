/** @file
  This header file provides functionality to print DMAR table

  Copyright (c) 1999 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _DUMP_DMAR_H_
#define _DUMP_DMAR_H_

#include <Library/DmaRemappingTable.h>
/**/
VOID
DumpAcpiDMAR (
  IN EFI_ACPI_DMAR_HEADER  *Dmar
  );

#endif
