/** @file
  Helper functions for IppCryptoLib -- X64 build support.

  Provides memset/memcpy wrappers and the MSVC linker alias that maps the
  ipp-crypto v2.0.0 PurgeBlockInternal symbol to the PurgeBlock export from
  pcppurgeblkm7as.nasm.  All X64 CPU feature detection is handled at runtime
  by cpinit.c; no stubs are needed here.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

#define IPP_CRYPTO_SUPPORT_IMPLEMENTATION
#include "IppCryptoSupport.h"

// Tell MSVC not to treat memset/memcpy as intrinsics in this file so that
// the definitions below are accepted. Without this, MSVC emits C2169.
#ifdef _MSC_VER
#pragma function(memset, memcpy)
// In ipp-crypto v2.0.0 the C interface was split: PurgeBlock() became an inline
// volatile-pointer wrapper that calls PurgeBlockInternal() (the real symbol).
// The NASM file (pcppurgeblkm7as.nasm) still exports the old name 'PurgeBlock'.
// In SBL non-merged builds OWNAPI() is the identity macro, so the linker
// needs bare 'PurgeBlockInternal'.  Map it to the existing NASM export.
#pragma comment(linker, "/alternatename:PurgeBlockInternal=PurgeBlock")
#endif

void *
memset (
  void *dest,
  int ch,
  size_t count
  )
{
  return SetMem(dest, (UINTN)(count),(UINT8)(ch));
}

void *
memcpy (
  void *dest_str,
  const void * src_str,
  size_t n
  )
{
  return CopyMem(dest_str,src_str, (UINTN)(n));
}
