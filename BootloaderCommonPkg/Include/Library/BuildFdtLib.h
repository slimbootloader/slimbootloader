/** @file
  This library will Build the FDT (flat device tree) table information.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef BUILD_FDT_LIB_H_
#define BUILD_FDT_LIB_H_

/**
  Build FDT node table for universal payload.

  @retval         Start address of FDT node table. NULL if there is failure to build FDT node.
**/
VOID *
EFIAPI
BuildFdtForUpl (
  VOID
  );

#endif
