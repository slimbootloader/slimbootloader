/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __BL_MEMORY_ALLOCATION_LIB_H__
#define __BL_MEMORY_ALLOCATION_LIB_H__

#include <Library/MemoryAllocationLib.h>

/**
  This function Allocates temporary memory pool.

  @param  Pages  The number of 4 KB pages to allocate.
**/
VOID *
EFIAPI
AllocateTemporaryPages (
  IN UINTN  Pages
  );

/**
  This function reset temporary memory pool.

  @param[in]  Bottom  memory pool bottom pointer.
                      NULL indicates to free all temporary memory pool
**/
VOID
EFIAPI
FreeTemporaryMemory (
  IN VOID   *Bottom
  );

#endif
