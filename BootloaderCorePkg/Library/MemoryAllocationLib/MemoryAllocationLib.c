/** @file
  Support routines for memory allocation routines
  based on PeiService for PEI phase drivers.

  Copyright (c) 2006 - 2015, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include <PiPei.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <BootloaderCoreGlobal.h>

#define   POOL_MIN_ALIGNMENT    0x10

/**
  Update the Memory pool top address.

  @param [in] Top Top address to update.
 **/
VOID
InternalUpdateMemPoolTop (
  IN UINT32  Top
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;
  UINT32               PoolUsed;

  LdrGlobal = GetLoaderGlobalDataPointer();
  PoolUsed = (LdrGlobal->MemPoolEnd - Top) +
              (LdrGlobal->MemPoolCurrBottom - LdrGlobal->MemPoolStart);
  if (LdrGlobal->MemPoolMaxUsed < PoolUsed) {
    LdrGlobal->MemPoolMaxUsed = PoolUsed;
  }
  ASSERT (Top >= LdrGlobal->MemPoolCurrBottom);
  LdrGlobal->MemPoolCurrTop = Top;
}

/**
  Update the Memory pool bottom address.

  @param [in] Bottom bottom address to update.
 **/
VOID
InternalUpdateMemPoolBottom (
  IN UINT32  Bottom
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;
  UINT32               PoolUsed;

  LdrGlobal = GetLoaderGlobalDataPointer();
  PoolUsed = (LdrGlobal->MemPoolEnd - LdrGlobal->MemPoolCurrTop) +
              (Bottom - LdrGlobal->MemPoolStart);
  if (LdrGlobal->MemPoolMaxUsed < PoolUsed) {
    LdrGlobal->MemPoolMaxUsed = PoolUsed;
  }
  ASSERT (LdrGlobal->MemPoolCurrTop >= Bottom);
  LdrGlobal->MemPoolCurrBottom = Bottom;
}

/**
  Allocates a buffer of type EfiBootServicesData.

  Allocates the number bytes specified by AllocationSize of type EfiBootServicesData and returns a
  pointer to the allocated buffer.  If AllocationSize is 0, LdrGlobal->MemPoolCurrTop is returned.
  If there is not enough memory remaining to satisfy the request, InternalUpdateMemPoolTop ASSERTS
  and the function does not return.

  @param  AllocationSize        The number of bytes to allocate.

  @return A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
EFIAPI
AllocatePool (
  IN UINTN  AllocationSize
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;
  UINT32               Top;

  LdrGlobal = GetLoaderGlobalDataPointer();
  Top  = LdrGlobal->MemPoolCurrTop;
  Top -= (UINT32)AllocationSize;
  Top  = ALIGN_DOWN (Top, POOL_MIN_ALIGNMENT);
  InternalUpdateMemPoolTop (Top);
  return (VOID *)(UINTN)Top;
}

/**
  Allocates and zeros a buffer of type EfiBootServicesData.

  Allocates the number bytes specified by AllocationSize of type EfiBootServicesData, clears the
  buffer with zeros, and returns a pointer to the allocated buffer.  If AllocationSize is 0,
  LdrGlobal->MemPoolCurrTop is returned. If there is not enough memory remaining to satisfy the
  request, InternalUpdateMemPoolTop ASSERTS and the function does not return.

  @param  AllocationSize        The number of bytes to allocate and zero.

  @return A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
EFIAPI
AllocateZeroPool (
  IN UINTN  AllocationSize
  )
{
  VOID  *Memory;

  Memory = AllocatePool (AllocationSize);
  if (Memory != NULL) {
    Memory = ZeroMem (Memory, AllocationSize);
  }
  return Memory;
}

/**
  Allocates one or more 4KB pages of type EfiBootServicesData.

  Allocates the number of 4KB pages of type EfiBootServicesData and returns a pointer to the
  allocated buffer.  The buffer returned is aligned on a 4KB boundary.  If Pages is 0, then
  NULL is returned.  If there is not enough memory remaining to satisfy the request,
  InternalUpdateMemPoolTop ASSERTS and the function does not return.

  @param  Pages                 The number of 4 KB pages to allocate.

  @return A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
EFIAPI
AllocatePages (
  IN UINTN  Pages
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;
  UINT32               Top;

  if (Pages == 0) {
    return NULL;
  }

  LdrGlobal = GetLoaderGlobalDataPointer();
  Top  = LdrGlobal->MemPoolCurrTop;
  Top  = ALIGN_DOWN (Top, EFI_PAGE_SIZE);
  Top -= (UINT32)(Pages * EFI_PAGE_SIZE);
  InternalUpdateMemPoolTop (Top);
  return (VOID *)(UINTN)Top;
}

/**
  Allocates one or more 4KB pages of type EfiBootServicesData at a specified alignment.

  Allocates the number of 4KB pages specified by Pages of type EfiBootServicesData with an
  alignment specified by Alignment.  The allocated buffer is returned.  If Pages is 0, then
  NULL is returned.  If there is not enough memory remaining to satisfy the request,
  InternalUpdateMemPoolTop ASSERTS and the function does not return.

  If Alignment is not a power of two and Alignment is not zero, then ASSERT().
  If Pages plus EFI_SIZE_TO_PAGES (Alignment) overflows, then ASSERT().

  @param  Pages                 The number of 4 KB pages to allocate.
  @param  Alignment             The requested alignment of the allocation.  Must be a power of two.
                                If Alignment is zero, then byte alignment is used.

  @return A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
EFIAPI
AllocateAlignedPages (
  IN UINTN  Pages,
  IN UINTN  Alignment
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;
  UINT32               Top;

  if (Pages == 0) {
    return NULL;
  }

  LdrGlobal = GetLoaderGlobalDataPointer();
  Top  = LdrGlobal->MemPoolCurrTop;
  Top  = ALIGN_DOWN (Top, EFI_PAGE_SIZE);
  Top -= (UINT32)(Pages * EFI_PAGE_SIZE);
  if (Alignment > EFI_PAGE_SIZE) {
    Top  = ALIGN_DOWN (Top, Alignment);
  }
  Top  = ALIGN_DOWN (Top, EFI_PAGE_SIZE);
  InternalUpdateMemPoolTop (Top);
  return (VOID *)(UINTN)Top;
}

/**
  This function allocates temporary memory pool.

  @param[in]  AllocationSize    The memory pool size to allocate. If AllocationSize is 0,
  LdrGlobal->MemPoolCurrBottom is returned. If there is not enough memory remaining to satisfy the
  request, InternalUpdateMemPoolBottom ASSERTS and the function does not return.

  @retval   A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
EFIAPI
AllocateTemporaryMemory (
  IN UINTN  AllocationSize
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;
  UINT32               Bottom;
  UINT32               NewBottom;

  LdrGlobal = GetLoaderGlobalDataPointer();
  Bottom  = LdrGlobal->MemPoolCurrBottom;
  Bottom  = ALIGN_UP (Bottom, POOL_MIN_ALIGNMENT);
  NewBottom = Bottom + (UINT32)AllocationSize;
  InternalUpdateMemPoolBottom (NewBottom);
  return (VOID *)(UINTN)Bottom;
}

/**
  This function frees temporary memory pool.

  @param[in] Buffer  Temporary memory pool to free.
                     NULL indicates to free all temporary memory pool previously allocated
**/
VOID
EFIAPI
FreeTemporaryMemory (
  IN VOID   *Buffer
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;
  UINT32               NewBottom;

  LdrGlobal = GetLoaderGlobalDataPointer();
  if (Buffer == NULL) {
    LdrGlobal->MemPoolCurrBottom = LdrGlobal->MemPoolStart;
  } else {
    NewBottom = (UINT32)(UINTN)Buffer;
    if (NewBottom < LdrGlobal->MemPoolCurrBottom) {
      InternalUpdateMemPoolBottom (NewBottom);
    }
  }
}

/**
  Frees one or more 4KB pages that were previously allocated with one of the page allocation
  functions in the Memory Allocation Library.

  Frees the number of 4KB pages specified by Pages from the buffer specified by Buffer.  Buffer
  must have been allocated on a previous call to the page allocation services of the Memory
  Allocation Library.  If it is not possible to free allocated pages, then this function will
  perform no actions.

  If Buffer was not allocated with a page allocation function in the Memory Allocation Library,
  then ASSERT().
  If Pages is zero, then ASSERT().

  @param  Buffer                The pointer to the buffer of pages to free.
  @param  Pages                 The number of 4 KB pages to free.

**/
VOID
EFIAPI
FreePages (
  IN VOID   *Buffer,
  IN UINTN  Pages
  )
{
}

/**
  Frees a buffer that was previously allocated with one of the pool allocation functions in the
  Memory Allocation Library.

  Frees the buffer specified by Buffer.  Buffer must have been allocated on a previous call to the
  pool allocation services of the Memory Allocation Library.  If it is not possible to free pool
  resources, then this function will perform no actions.

  If Buffer was not allocated with a pool allocation function in the Memory Allocation Library,
  then ASSERT().

  @param  Buffer                The pointer to the buffer to free.

**/
VOID
EFIAPI
FreePool (
  IN VOID   *Buffer
  )
{
}

/**
  Allocates one or more 4KB pages of type EfiRuntimeServicesData.

  Allocates the number of 4KB pages of type EfiRuntimeServicesData and returns a pointer to the
  allocated buffer.  The buffer returned is aligned on a 4KB boundary.  If Pages is 0, then NULL
  is returned.  If there is not enough memory remaining to satisfy the request, then NULL is
  returned.

  @param  Pages                 The number of 4 KB pages to allocate.

  @return A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
EFIAPI
AllocateRuntimePages (
  IN UINTN  Pages
  )
{
  return NULL;
}

/**
  Allocates a buffer of type EfiRuntimeServicesData.

  Allocates the number bytes specified by AllocationSize of type EfiRuntimeServicesData and returns
  a pointer to the allocated buffer.  If AllocationSize is 0, then a valid buffer of 0 size is
  returned.  If there is not enough memory remaining to satisfy the request, then NULL is returned.

  @param  AllocationSize        The number of bytes to allocate.

  @return A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
EFIAPI
AllocateRuntimePool (
  IN UINTN  AllocationSize
  )
{
  return NULL;
}
