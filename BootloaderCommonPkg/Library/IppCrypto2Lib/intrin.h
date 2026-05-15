/** @file
  Include file to support building the Intel IPPCrypto library.

Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

// This stub shadows the real MSVC <intrin.h> because EDK2 adds the module
// directory to the compiler include path before the VSXX system include path.
//
// For GCC: intrin.h is MSVC-specific; an empty stub prevents file-not-found.
// For MSVC: provide the __m128i type and SSE2/SSSE3/SSE4.1 intrinsic
//   declarations that ipp-crypto v2.0.0 requires (primarily pcptool.h).
//   MSVC recognises these names as hardware intrinsics and emits the
//   corresponding SSE instructions inline — no function body is needed.

#ifndef IPP_SBL_INTRIN_H_
#define IPP_SBL_INTRIN_H_

#if defined(_MSC_VER)

// -----------------------------------------------------------------------
// __m128i type  (normally from emmintrin.h / intrin.h)
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

// -----------------------------------------------------------------------
// SSE2 load / store
// -----------------------------------------------------------------------
__m128i __cdecl _mm_load_si128  (__m128i const*);
__m128i __cdecl _mm_loadu_si128 (__m128i const*);
void    __cdecl _mm_store_si128 (__m128i*, __m128i);
void    __cdecl _mm_storeu_si128(__m128i*, __m128i);

// -----------------------------------------------------------------------
// SSE2 set / convert
// -----------------------------------------------------------------------
__m128i __cdecl _mm_setzero_si128   (void);
__m128i __cdecl _mm_set1_epi8       (char);
__m128i __cdecl _mm_set1_epi16      (short);
__m128i __cdecl _mm_set1_epi32      (int);
__m128i __cdecl _mm_set1_epi64x     (__int64);
__m128i __cdecl _mm_set_epi8        (char,char,char,char,char,char,char,char,
                                     char,char,char,char,char,char,char,char);
__m128i __cdecl _mm_set_epi16       (short,short,short,short,short,short,short,short);
__m128i __cdecl _mm_set_epi32       (int,int,int,int);
__m128i __cdecl _mm_set_epi64x      (__int64,__int64);
__m128i __cdecl _mm_cvtsi32_si128   (int);
__m128i __cdecl _mm_cvtsi64_si128   (__int64);
int     __cdecl _mm_cvtsi128_si32   (__m128i);
__int64 __cdecl _mm_cvtsi128_si64   (__m128i);

// -----------------------------------------------------------------------
// SSE2 logical
// -----------------------------------------------------------------------
__m128i __cdecl _mm_and_si128   (__m128i, __m128i);
__m128i __cdecl _mm_andnot_si128(__m128i, __m128i);
__m128i __cdecl _mm_or_si128    (__m128i, __m128i);
__m128i __cdecl _mm_xor_si128   (__m128i, __m128i);

// -----------------------------------------------------------------------
// SSE2 integer arithmetic
// -----------------------------------------------------------------------
__m128i __cdecl _mm_add_epi8 (__m128i, __m128i);
__m128i __cdecl _mm_add_epi16(__m128i, __m128i);
__m128i __cdecl _mm_add_epi32(__m128i, __m128i);
__m128i __cdecl _mm_add_epi64(__m128i, __m128i);
__m128i __cdecl _mm_sub_epi8 (__m128i, __m128i);
__m128i __cdecl _mm_sub_epi16(__m128i, __m128i);
__m128i __cdecl _mm_sub_epi32(__m128i, __m128i);
__m128i __cdecl _mm_sub_epi64(__m128i, __m128i);
__m128i __cdecl _mm_mul_epu32   (__m128i, __m128i);
__m128i __cdecl _mm_mullo_epi16 (__m128i, __m128i);
__m128i __cdecl _mm_mulhi_epu16 (__m128i, __m128i);
__m128i __cdecl _mm_madd_epi16  (__m128i, __m128i);

// -----------------------------------------------------------------------
// SSE2 shift
// -----------------------------------------------------------------------
__m128i __cdecl _mm_slli_epi16(__m128i, int);
__m128i __cdecl _mm_slli_epi32(__m128i, int);
__m128i __cdecl _mm_slli_epi64(__m128i, int);
__m128i __cdecl _mm_srli_epi16(__m128i, int);
__m128i __cdecl _mm_srli_epi32(__m128i, int);
__m128i __cdecl _mm_srli_epi64(__m128i, int);
__m128i __cdecl _mm_srai_epi16(__m128i, int);
__m128i __cdecl _mm_srai_epi32(__m128i, int);
__m128i __cdecl _mm_slli_si128(__m128i, int);
__m128i __cdecl _mm_srli_si128(__m128i, int);
__m128i __cdecl _mm_sll_epi16 (__m128i, __m128i);
__m128i __cdecl _mm_sll_epi32 (__m128i, __m128i);
__m128i __cdecl _mm_sll_epi64 (__m128i, __m128i);
__m128i __cdecl _mm_srl_epi16 (__m128i, __m128i);
__m128i __cdecl _mm_srl_epi32 (__m128i, __m128i);
__m128i __cdecl _mm_srl_epi64 (__m128i, __m128i);
__m128i __cdecl _mm_sra_epi16 (__m128i, __m128i);
__m128i __cdecl _mm_sra_epi32 (__m128i, __m128i);

// -----------------------------------------------------------------------
// SSE2 compare
// -----------------------------------------------------------------------
__m128i __cdecl _mm_cmpeq_epi8 (__m128i, __m128i);
__m128i __cdecl _mm_cmpeq_epi16(__m128i, __m128i);
__m128i __cdecl _mm_cmpeq_epi32(__m128i, __m128i);
__m128i __cdecl _mm_cmpgt_epi8 (__m128i, __m128i);
__m128i __cdecl _mm_cmpgt_epi16(__m128i, __m128i);
__m128i __cdecl _mm_cmpgt_epi32(__m128i, __m128i);
__m128i __cdecl _mm_cmplt_epi8 (__m128i, __m128i);
__m128i __cdecl _mm_cmplt_epi16(__m128i, __m128i);
__m128i __cdecl _mm_cmplt_epi32(__m128i, __m128i);

// -----------------------------------------------------------------------
// SSE2 shuffle / permute / unpack / pack
// -----------------------------------------------------------------------
__m128i __cdecl _mm_shuffle_epi32   (__m128i, int);
__m128i __cdecl _mm_shufflehi_epi16 (__m128i, int);
__m128i __cdecl _mm_shufflelo_epi16 (__m128i, int);
__m128i __cdecl _mm_unpackhi_epi8   (__m128i, __m128i);
__m128i __cdecl _mm_unpackhi_epi16  (__m128i, __m128i);
__m128i __cdecl _mm_unpackhi_epi32  (__m128i, __m128i);
__m128i __cdecl _mm_unpackhi_epi64  (__m128i, __m128i);
__m128i __cdecl _mm_unpacklo_epi8   (__m128i, __m128i);
__m128i __cdecl _mm_unpacklo_epi16  (__m128i, __m128i);
__m128i __cdecl _mm_unpacklo_epi32  (__m128i, __m128i);
__m128i __cdecl _mm_unpacklo_epi64  (__m128i, __m128i);
__m128i __cdecl _mm_packs_epi16     (__m128i, __m128i);
__m128i __cdecl _mm_packs_epi32     (__m128i, __m128i);
__m128i __cdecl _mm_packus_epi16    (__m128i, __m128i);
int     __cdecl _mm_movemask_epi8   (__m128i);

// -----------------------------------------------------------------------
// SSE3
// -----------------------------------------------------------------------
__m128i __cdecl _mm_lddqu_si128(__m128i const*);

// -----------------------------------------------------------------------
// SSSE3
// -----------------------------------------------------------------------
__m128i __cdecl _mm_shuffle_epi8 (__m128i, __m128i);
__m128i __cdecl _mm_alignr_epi8  (__m128i, __m128i, int);
__m128i __cdecl _mm_abs_epi8     (__m128i);
__m128i __cdecl _mm_abs_epi16    (__m128i);
__m128i __cdecl _mm_abs_epi32    (__m128i);
__m128i __cdecl _mm_hadd_epi16   (__m128i, __m128i);
__m128i __cdecl _mm_hadd_epi32   (__m128i, __m128i);
__m128i __cdecl _mm_maddubs_epi16(__m128i, __m128i);
__m128i __cdecl _mm_mulhrs_epi16 (__m128i, __m128i);

// -----------------------------------------------------------------------
// SSE4.1
// -----------------------------------------------------------------------
__m128i __cdecl _mm_mullo_epi32 (__m128i, __m128i);
__m128i __cdecl _mm_cmpeq_epi64 (__m128i, __m128i);
__m128i __cdecl _mm_blendv_epi8 (__m128i, __m128i, __m128i);
__m128i __cdecl _mm_blend_epi16 (__m128i, __m128i, int);
__m128i __cdecl _mm_packus_epi32(__m128i, __m128i);
int     __cdecl _mm_extract_epi8 (__m128i, int);
int     __cdecl _mm_extract_epi32(__m128i, int);
__int64 __cdecl _mm_extract_epi64(__m128i, int);
__m128i __cdecl _mm_insert_epi8  (__m128i, int, int);
__m128i __cdecl _mm_insert_epi32 (__m128i, int, int);
__m128i __cdecl _mm_insert_epi64 (__m128i, __int64, int);
__m128i __cdecl _mm_min_epi8     (__m128i, __m128i);
__m128i __cdecl _mm_min_epu16    (__m128i, __m128i);
__m128i __cdecl _mm_min_epi32    (__m128i, __m128i);
__m128i __cdecl _mm_min_epu32    (__m128i, __m128i);
__m128i __cdecl _mm_max_epi8     (__m128i, __m128i);
__m128i __cdecl _mm_max_epu16    (__m128i, __m128i);
__m128i __cdecl _mm_max_epi32    (__m128i, __m128i);
__m128i __cdecl _mm_max_epu32    (__m128i, __m128i);
__m128i __cdecl _mm_minpos_epu16 (__m128i);
__m128i __cdecl _mm_mpsadbw_epu8 (__m128i, __m128i, int);
__m128i __cdecl _mm_stream_load_si128(__m128i*);

// -----------------------------------------------------------------------
// SSE4.2 — SHA-NI / CLMUL / CRC
// -----------------------------------------------------------------------
unsigned int    __cdecl _mm_crc32_u8 (unsigned int, unsigned char);
unsigned int    __cdecl _mm_crc32_u16(unsigned int, unsigned short);
unsigned int    __cdecl _mm_crc32_u32(unsigned int, unsigned int);
unsigned __int64 __cdecl _mm_crc32_u64(unsigned __int64, unsigned __int64);

#endif /* _MSC_VER */

#endif /* IPP_SBL_INTRIN_H_ */

