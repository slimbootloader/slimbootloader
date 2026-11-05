/** @file
  AddressSanitizer compiler callbacks and checked CopyMem/SetMem entry points.

  Every module of an ASAN build links this library, so any library can be instrumented
  whichever modules link it. The callbacks forward to the runtime connected with
  AsanStubConnect (). Without one they only perform the memory operations they wrap.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseMemoryLib.h>
#include <Library/AsanStubLib.h>

//
// Referenced by every instrumented function prologue. Kept at 0 so callbacks receive Address >> 3.
//
UINTN  __asan_shadow_memory_dynamic_address;

STATIC CONST ASAN_SERVICE  *mAsanService;

/**
  Route this module's compiler callbacks and direct memory APIs to a runtime.

  @param[in] Service    Runtime to use, or NULL to stop checking.

**/
VOID
EFIAPI
AsanStubConnect (
  IN CONST ASAN_SERVICE  *Service
  )
{
  mAsanService = Service;
}

//
// Callbacks emitted with -mllvm -asan-instrumentation-with-call-threshold=0.
//
#define ASAN_ACCESS_CALLBACKS(Size)                                         \
  VOID __asan_load##Size (UINTN Address)                                    \
  {                                                                         \
    if (mAsanService != NULL) {                                             \
      mAsanService->Check (Address, Size, FALSE, RETURN_ADDRESS (0));       \
    }                                                                       \
  }                                                                         \
  VOID __asan_store##Size (UINTN Address)                                   \
  {                                                                         \
    if (mAsanService != NULL) {                                             \
      mAsanService->Check (Address, Size, TRUE, RETURN_ADDRESS (0));        \
    }                                                                       \
  }

ASAN_ACCESS_CALLBACKS (1)
ASAN_ACCESS_CALLBACKS (2)
ASAN_ACCESS_CALLBACKS (4)
ASAN_ACCESS_CALLBACKS (8)
ASAN_ACCESS_CALLBACKS (16)

VOID
__asan_loadN (
  UINTN  Address,
  UINTN  Size
  )
{
  if (mAsanService != NULL) {
    mAsanService->Check (Address, Size, FALSE, RETURN_ADDRESS (0));
  }
}

VOID
__asan_storeN (
  UINTN  Address,
  UINTN  Size
  )
{
  if (mAsanService != NULL) {
    mAsanService->Check (Address, Size, TRUE, RETURN_ADDRESS (0));
  }
}

STATIC
VOID *
AsanCopyMem (
  OUT VOID       *Destination,
  IN CONST VOID  *Source,
  IN UINTN       Length,
  IN VOID        *Pc
  )
{
  if ((Length != 0) && (mAsanService != NULL)) {
    mAsanService->Check ((UINTN)Source, Length, FALSE, Pc);
    mAsanService->Check ((UINTN)Destination, Length, TRUE, Pc);
  }

  return AsanInternalCopyMem (Destination, Source, Length);
}

STATIC
VOID *
AsanSetMem (
  OUT VOID  *Buffer,
  IN UINTN  Length,
  IN UINT8  Value,
  IN VOID   *Pc
  )
{
  if ((Length != 0) && (mAsanService != NULL)) {
    mAsanService->Check ((UINTN)Buffer, Length, TRUE, Pc);
  }

  return AsanInternalSetMem (Buffer, Length, Value);
}

//
// The ASAN DSC renames both selected BaseMemoryLib instances' original public
// wrappers to AsanInternal*. Do not instrument these raw backends or this stub.
// Check even a self-copy: its source/destination range must be addressable.
//
VOID *
EFIAPI
CopyMem (
  OUT VOID       *Destination,
  IN CONST VOID  *Source,
  IN UINTN       Length
  )
{
  return AsanCopyMem (Destination, Source, Length, RETURN_ADDRESS (0));
}

VOID *
EFIAPI
SetMem (
  OUT VOID  *Buffer,
  IN UINTN  Length,
  IN UINT8  Value
  )
{
  return AsanSetMem (Buffer, Length, Value, RETURN_ADDRESS (0));
}

VOID *
__asan_memcpy (
  VOID        *Destination,
  CONST VOID  *Source,
  UINTN       Length
  )
{
  return AsanCopyMem (Destination, Source, Length, RETURN_ADDRESS (0));
}

VOID *
__asan_memmove (
  VOID        *Destination,
  CONST VOID  *Source,
  UINTN       Length
  )
{
  return AsanCopyMem (Destination, Source, Length, RETURN_ADDRESS (0));
}

VOID *
__asan_memset (
  VOID   *Buffer,
  INT32  Value,
  UINTN  Length
  )
{
  return AsanSetMem (Buffer, Length, (UINT8)Value, RETURN_ADDRESS (0));
}

VOID
__asan_handle_no_return (
  VOID
  )
{
  if (mAsanService != NULL) {
    mAsanService->HandleNoReturn ((UINTN)__builtin_frame_address (0));
  }
}

//
// Stack frame poisoning callbacks emitted with -mllvm -asan-max-inline-poisoning-size=0.
//
#define ASAN_SET_SHADOW_CALLBACK(Value)                                     \
  VOID __asan_set_shadow_##Value (UINTN ShadowAddress, UINTN Count)        \
  {                                                                         \
    if (mAsanService != NULL) {                                             \
      mAsanService->SetShadow (ShadowAddress, Count, 0x##Value);            \
    }                                                                       \
  }

ASAN_SET_SHADOW_CALLBACK (00)
ASAN_SET_SHADOW_CALLBACK (01)
ASAN_SET_SHADOW_CALLBACK (02)
ASAN_SET_SHADOW_CALLBACK (03)
ASAN_SET_SHADOW_CALLBACK (04)
ASAN_SET_SHADOW_CALLBACK (05)
ASAN_SET_SHADOW_CALLBACK (06)
ASAN_SET_SHADOW_CALLBACK (07)
ASAN_SET_SHADOW_CALLBACK (f1)
ASAN_SET_SHADOW_CALLBACK (f2)
ASAN_SET_SHADOW_CALLBACK (f3)
ASAN_SET_SHADOW_CALLBACK (f5)
ASAN_SET_SHADOW_CALLBACK (f8)
