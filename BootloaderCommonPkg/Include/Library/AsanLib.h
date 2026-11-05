/** @file
  AddressSanitizer runtime interface used by the memory allocators, Stage2 and payload entry.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __ASAN_LIB_H__
#define __ASAN_LIB_H__

#define ASAN_SHADOW_HEAP_REDZONE  0xFA
#define ASAN_SHADOW_HEAP_FREED    0xFD

/**
  Get the shadow memory size needed to track a region.

  @param[in] RegionSize   Size in bytes of the region to track.

  @retval    Shadow size in bytes, or 0 if AddressSanitizer is not enabled.

**/
UINTN
EFIAPI
AsanGetShadowSize (
  IN UINTN  RegionSize
  );

/**
  Start tracking the heap. The whole region is initially poisoned.

  @param[in] RegionBase   Region base address, 8-byte aligned.
  @param[in] RegionSize   Region size in bytes.
  @param[in] Shadow       Shadow buffer of AsanGetShadowSize (RegionSize) bytes.

**/
VOID
EFIAPI
AsanInitialize (
  IN UINTN  RegionBase,
  IN UINTN  RegionSize,
  IN VOID   *Shadow
  );

/**
  Start tracking a stack so instrumented stack frames get redzones.

  @param[in] StackBase    Lowest stack address, 8-byte aligned.
  @param[in] StackSize    Stack size in bytes.
  @param[in] Shadow       Shadow buffer of AsanGetShadowSize (StackSize) bytes.

**/
VOID
EFIAPI
AsanTrackStack (
  IN UINTN  StackBase,
  IN UINTN  StackSize,
  IN VOID   *Shadow
  );

/**
  Get the address range spanned by the instrumented globals.

  @param[out] Base        Range base address, 8-byte aligned.

  @retval    Range size in bytes, or 0 if there are no instrumented globals.

**/
UINTN
EFIAPI
AsanGetGlobalsRange (
  OUT UINTN  *Base
  );

/**
  Start tracking the instrumented globals and poison their redzones.

  @param[in] Base         Base returned by AsanGetGlobalsRange ().
  @param[in] Size         Size returned by AsanGetGlobalsRange ().
  @param[in] Shadow       Shadow buffer of AsanGetShadowSize (Size) bytes.

**/
VOID
EFIAPI
AsanTrackGlobals (
  IN UINTN  Base,
  IN UINTN  Size,
  IN VOID   *Shadow
  );

/**
  Mark the 8-byte granules fully inside a range as not addressable.

  @param[in] Address      Range start.
  @param[in] Size         Range size in bytes.
  @param[in] Value        Shadow value, ASAN_SHADOW_HEAP_REDZONE or ASAN_SHADOW_HEAP_FREED.

**/
VOID
EFIAPI
AsanPoison (
  IN CONST VOID  *Address,
  IN UINTN       Size,
  IN UINT8       Value
  );

/**
  Mark a range as addressable.

  @param[in] Address      Range start, 8-byte aligned.
  @param[in] Size         Range size in bytes.

**/
VOID
EFIAPI
AsanUnpoison (
  IN CONST VOID  *Address,
  IN UINTN       Size
  );

/**
  Check for a double free using only shadow memory, before reading pool metadata.

  Does not validate allocator metadata or insert the buffer into quarantine.

  @param[in] Buffer       Pool buffer being freed.

  @retval TRUE            Buffer is not known to be freed, or tracking is inactive.
  @retval FALSE           Buffer is freed; a double-free report has been issued.

**/
BOOLEAN
EFIAPI
AsanCheckFree (
  IN CONST VOID  *Buffer
  );

/**
  Defer the release of a freed pool buffer so later accesses are still caught.

  Reports a double free if Buffer is already in the freed state.

  @param[in] Buffer       Pool buffer being freed.
  @param[in] Size         Size of the pool block that holds Buffer.

  @retval    The buffer the caller must release now: Buffer itself, an older
             buffer evicted from quarantine, or NULL if nothing is to be released.

**/
VOID *
EFIAPI
AsanQuarantine (
  IN VOID   *Buffer,
  IN UINTN  Size
  );

/**
  Use another module's runtime, normally the one Stage2 publishes, instead of this one.

  Call it before any other AsanLib function.

  @param[in] Service      ASAN_SERVICE found in the service list.

  @retval TRUE            The service is used.
  @retval FALSE           The service is not used.

**/
BOOLEAN
EFIAPI
AsanUseService (
  IN CONST VOID  *Service
  );

/**
  Get this module's runtime so it can be published with RegisterService ().

  @retval    This module's ASAN_SERVICE, or NULL if AddressSanitizer is not enabled.

**/
VOID *
EFIAPI
AsanGetService (
  VOID
  );

#endif
