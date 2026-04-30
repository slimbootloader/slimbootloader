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
#include <x86gprintrin.h>
// Provide SSE2 vector types (__m128i etc.) and intrinsics needed by ipp-crypto
#include <emmintrin.h>
#endif
