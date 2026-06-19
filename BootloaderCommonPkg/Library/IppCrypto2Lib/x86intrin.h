/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

// Dummy include file needed to resolve GCC build errors

#if defined(__GNUC__) && !defined(_MSC_VER)
// Block mm_malloc.h from pulling in stdlib.h, which would conflict with
// IppCryptoSupport.h's 'size_t' typedef (UINTN vs system size_t)
#define _MM_MALLOC_H_INCLUDED
// Provide LZCNT and other GP-register intrinsics (_lzcnt_u32 etc.)
// x86gprintrin.h might be missing on some GCC toolchain images.
#if defined(__has_include) && __has_include(<x86gprintrin.h>)
#include <x86gprintrin.h>
#else
// Fallback: provide only the GP-register intrinsics used by ipp-crypto.
#ifndef IPP_SBL_LZCNT_INTRINSICS_DEFINED
#define IPP_SBL_LZCNT_INTRINSICS_DEFINED

static __inline__ unsigned int __attribute__((__always_inline__))
_lzcnt_u32 (unsigned int __X)
{
  return __builtin_ia32_lzcnt_u32 (__X);
}

static __inline__ unsigned long long __attribute__((__always_inline__))
_lzcnt_u64 (unsigned long long __X)
{
  return __builtin_ia32_lzcnt_u64 (__X);
}
#endif /* IPP_SBL_LZCNT_INTRINSICS_DEFINED */
#endif
// Provide SSE2 vector types (__m128i etc.) and intrinsics needed by ipp-crypto
#include <emmintrin.h>
#endif
