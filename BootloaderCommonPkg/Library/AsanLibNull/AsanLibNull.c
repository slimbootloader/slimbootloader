/** @file
  Null AddressSanitizer runtime used when AddressSanitizer is disabled.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/AsanLib.h>

/**
  Get the shadow memory size needed to track a region.

  @param[in] RegionSize   Size in bytes of the region to track.

  @retval    0, AddressSanitizer is not enabled.

**/
UINTN
EFIAPI
AsanGetShadowSize (
  IN UINTN  RegionSize
  )
{
  return 0;
}

/**
  Start tracking a region.

  @param[in] RegionBase   Region base address.
  @param[in] RegionSize   Region size in bytes.
  @param[in] Shadow       Shadow buffer.

**/
VOID
EFIAPI
AsanInitialize (
  IN UINTN  RegionBase,
  IN UINTN  RegionSize,
  IN VOID   *Shadow
  )
{
}

/**
  Mark a range as not addressable.

  @param[in] Address      Range start.
  @param[in] Size         Range size in bytes.
  @param[in] Value        Shadow value.

**/
VOID
EFIAPI
AsanPoison (
  IN CONST VOID  *Address,
  IN UINTN       Size,
  IN UINT8       Value
  )
{
}

/**
  Mark a range as addressable.

  @param[in] Address      Range start.
  @param[in] Size         Range size in bytes.

**/
VOID
EFIAPI
AsanUnpoison (
  IN CONST VOID  *Address,
  IN UINTN       Size
  )
{
}

/**
  Check freed heap shadow before allocator metadata is read.

  @param[in] Buffer       Pool buffer being freed.

  @retval TRUE            AddressSanitizer is disabled.

**/
BOOLEAN
EFIAPI
AsanCheckFree (
  IN CONST VOID  *Buffer
  )
{
  return TRUE;
}

/**
  Defer the release of a freed pool buffer.

  @param[in] Buffer       Pool buffer being freed.
  @param[in] Size         Size of the pool block that holds Buffer.

  @retval    Buffer, which the caller releases immediately.

**/
VOID *
EFIAPI
AsanQuarantine (
  IN VOID   *Buffer,
  IN UINTN  Size
  )
{
  return Buffer;
}

/**
  Start tracking a stack.

  @param[in] StackBase    Lowest stack address.
  @param[in] StackSize    Stack size in bytes.
  @param[in] Shadow       Shadow buffer.

**/
VOID
EFIAPI
AsanTrackStack (
  IN UINTN  StackBase,
  IN UINTN  StackSize,
  IN VOID   *Shadow
  )
{
}

/**
  Get the address range spanned by the instrumented globals.

  @param[out] Base        Range base address.

  @retval    0, AddressSanitizer is not enabled.

**/
UINTN
EFIAPI
AsanGetGlobalsRange (
  OUT UINTN  *Base
  )
{
  *Base = 0;
  return 0;
}

/**
  Start tracking the instrumented globals.

  @param[in] Base         Range base address.
  @param[in] Size         Range size in bytes.
  @param[in] Shadow       Shadow buffer.

**/
VOID
EFIAPI
AsanTrackGlobals (
  IN UINTN  Base,
  IN UINTN  Size,
  IN VOID   *Shadow
  )
{
}

/**
  Use another module's runtime.

  @param[in] Service      ASAN_SERVICE found in the service list.

  @retval FALSE           AddressSanitizer is not enabled.

**/
BOOLEAN
EFIAPI
AsanUseService (
  IN CONST VOID  *Service
  )
{
  return FALSE;
}

/**
  Get this module's runtime.

  @retval NULL            AddressSanitizer is not enabled.

**/
VOID *
EFIAPI
AsanGetService (
  VOID
  )
{
  return NULL;
}
