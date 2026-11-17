/** @file
  AddressSanitizer callback stub linked into every module of an ASAN build.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __ASAN_STUB_LIB_H__
#define __ASAN_STUB_LIB_H__

#include <Service/AsanService.h>

/**
  Route this module's compiler callbacks and CopyMem/SetMem checks to a runtime.

  Until this is called memory operations run unchecked. NULL disconnects both
  compiler callbacks and direct API checking; it does not change memory semantics.

  @param[in] Service    Runtime to use, or NULL to stop checking.

**/
VOID
EFIAPI
AsanStubConnect (
  IN CONST ASAN_SERVICE  *Service
  );

//
// Unchecked BaseMemoryLib backends, available only in ENABLE_ASAN builds.
// The DSC renames the original wrappers, preserving their assertions and
// overlap/zero-length semantics. Only runtime shadow maintenance and the
// checked entry points should bypass checking through these functions.
//
VOID *
EFIAPI
AsanInternalCopyMem (
  OUT VOID       *DestinationBuffer,
  IN CONST VOID  *SourceBuffer,
  IN UINTN       Length
  );

VOID *
EFIAPI
AsanInternalSetMem (
  OUT VOID  *Buffer,
  IN UINTN  Length,
  IN UINT8  Value
  );

#endif
