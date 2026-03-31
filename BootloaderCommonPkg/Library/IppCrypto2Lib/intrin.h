/** @file
  Include file to support building the Intel IPPCrypto library.

Copyright (c) 2024 - 2026, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

// This stub shadows the real MSVC <intrin.h> because EDK2 adds the module
// directory to the compiler include path before the VSXX system include path.
//
// For GCC: intrin.h is MSVC-specific; an empty stub prevents file-not-found.
// For MSVC (Y8 only): owndefs.h includes "intrin.h" (quoted, not angle-bracket)
//   when _IPP32E == _IPP32E_Y8 && _MSC_FULL_VER >= 140050110. The only caller
//   is pcptool.h, which uses three SSE2 intrinsics confirmed by build:
//     _mm_loadu_si128, _mm_storeu_si128, _mm_xor_si128.
//   MSVC recognises these names as hardware intrinsics and emits the corresponding
//   SSE instructions inline — no function body is needed.
//   M7/U8: below the _IPP32E >= _IPP32E_Y8 outer guard — this block is skipped.
//   E9:    above Y8 but below L9 — neither inner branch fires on MSVC.
//   L9:    uses "immintrin.h" (MSVC) or "x86intrin.h" (GCC) instead.
//   All GCC variants: use "x86intrin.h" via a separate __GNUC__ block.

#ifndef IPP_SBL_INTRIN_H_
#define IPP_SBL_INTRIN_H_

#if defined(_MSC_VER)

// -----------------------------------------------------------------------
// __m128i type  (normally from emmintrin.h / intrin.h)
// Confirmed required: pcptool.h:55 uses it unconditionally.
// -----------------------------------------------------------------------
#if !defined(__m128i_DEFINED)
#define __m128i_DEFINED
typedef union __declspec(intrin_type) __declspec(align(16)) __m128i {
    __int8           m128i_i8[16];
    __int16          m128i_i16[8];
    __int32          m128i_i32[4];
    __int64          m128i_i64[2];
    unsigned __int8  m128i_u8[16];
    unsigned __int16 m128i_u16[8];
    unsigned __int32 m128i_u32[4];
    unsigned __int64 m128i_u64[2];
} __m128i;
#endif

// Required by pcptool.h (lines 55, 171-174):
__m128i __cdecl _mm_loadu_si128 (__m128i const*);
void    __cdecl _mm_storeu_si128(__m128i*, __m128i);
__m128i __cdecl _mm_xor_si128   (__m128i, __m128i);

#endif /* _MSC_VER */

#endif /* IPP_SBL_INTRIN_H_ */

