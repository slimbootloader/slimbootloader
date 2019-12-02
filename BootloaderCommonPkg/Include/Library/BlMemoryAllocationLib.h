/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __BL_MEMORY_ALLOCATION_LIB_H__
#define __BL_MEMORY_ALLOCATION_LIB_H__

#include <Library/MemoryAllocationLib.h>

/**
  This function allocates temporary memory pool.

  @param[in]  AllocationSize    The memory pool size to allocate.

  @retval     A pointer to the allocated temporary buffer or NULL if allocation fails.

**/
VOID *
EFIAPI
AllocateTemporaryMemory (
  IN UINTN  AllocationSize
  );

/**
  This function frees temporary memory pool.

  @param[in] Buffer  Temporary memory pool to free.
                     NULL indicates to free all temporary memory pool previously allocated
**/
VOID
EFIAPI
FreeTemporaryMemory (
  IN VOID   *Buffer
  );

#endif
