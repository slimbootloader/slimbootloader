/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
