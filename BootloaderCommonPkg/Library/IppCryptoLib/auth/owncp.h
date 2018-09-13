/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __OWNCP_H__
#define __OWNCP_H__

#ifndef __OWNDEFS_H__
  #include "owndefs.h"
#endif

#ifndef __IPPCP_H__
  #include "ippcp.h"
#endif

/*
// modes of the CPU feature
*/
#define _FEATURE_OFF_      (0)   /* feature is OFF a priori */
#define _FEATURE_ON_       (1)   /* feature is ON  a priori */
#define _FEATURE_TICKTOCK_ (2)   /* dectect is feature OFF/ON */

#define _ABL_
#if defined(_ABL_)
#include "pcpvariant_abl.h"
#endif

//#define _XMM7560_
#if defined(_XMM7560_)
#  include "pcpvariant_xmm7560.h"
#  pragma message ("standard configuration (pcpvariant.h) will be changed")
#endif

#include "pcpvariant.h"

// #pragma warning( disable : 4996 4324 4206)

/* ippCP length */
typedef int cpSize;

/*
// Common ippCP Macros
*/

/* size of cache line (bytes) */
#if (_IPP==_IPP_M5)
#define CACHE_LINE_SIZE      (16)
#define LOG_CACHE_LINE_SIZE   (4)
#else
#define CACHE_LINE_SIZE      (64)
#define LOG_CACHE_LINE_SIZE   (6)
#endif

/* swap data & pointers */
#define SWAP_PTR(ATYPE, pX,pY)   { ATYPE* aPtr=(pX); (pX)=(pY); (pY)=aPtr; }
#define SWAP(x,y)                {(x)^=(y); (y)^=(x); (x)^=(y);}

/* alignment value */
#define ALIGN_VAL ((int)sizeof(void*))

/* bitsize */
#define BYTESIZE     (8)
#define BITSIZE(x)   ((int)(sizeof(x)*BYTESIZE))

/* bit length -> byte/word length conversion */
#define BITS2WORD8_SIZE(x)  (((x)+ 7)>>3)
#define BITS2WORD16_SIZE(x) (((x)+15)>>4)
#define BITS2WORD32_SIZE(x) (((x)+31)>>5)
#define BITS2WORD64_SIZE(x) (((x)+63)>>6)

/* WORD and DWORD manipulators */
#define LODWORD(x)    ((Ipp32u)(x))
#define HIDWORD(x)    ((Ipp32u)(((Ipp64u)(x) >>32) & 0xFFFFFFFF))

#define MAKEHWORD(bLo,bHi) ((Ipp16u)(((Ipp8u)(bLo))  | ((Ipp16u)((Ipp8u)(bHi))) << 8))
#define MAKEWORD(hLo,hHi)  ((Ipp32u)(((Ipp16u)(hLo)) | ((Ipp32u)((Ipp16u)(hHi))) << 16))
#define MAKEDWORD(wLo,wHi) ((Ipp64u)(((Ipp32u)(wLo)) | ((Ipp64u)((Ipp32u)(wHi))) << 32))

/* extract byte */
#define EBYTE(w,n) ((Ipp8u)((w) >> (8 * (n))))

/* hexString <-> Ipp32u conversion */
#define HSTRING_TO_U32(ptrByte)  \
         (((ptrByte)[0]) <<24)   \
        +(((ptrByte)[1]) <<16)   \
        +(((ptrByte)[2]) <<8)    \
        +((ptrByte)[3])
#define U32_TO_HSTRING(ptrByte, x)  \
   (ptrByte)[0] = (Ipp8u)((x)>>24); \
   (ptrByte)[1] = (Ipp8u)((x)>>16); \
   (ptrByte)[2] = (Ipp8u)((x)>>8);  \
   (ptrByte)[3] = (Ipp8u)(x)

/* 32- and 64-bit masks for MSB of nbits-sequence */
#define MAKEMASK32(nbits) (0xFFFFFFFF >>((32 - ((nbits)&0x1F)) &0x1F))
#define MAKEMASK64(nbits) (0xFFFFFFFFFFFFFFFF >>((64 - ((nbits)&0x3F)) &0x3F))

/* Logical Shifts (right and left) of WORD */
#define LSR32(x,nBits)  ((x)>>(nBits))
#define LSL32(x,nBits)  ((x)<<(nBits))

/* Rorate (right and left) of WORD */
#if defined(_SLIMBOOT_OPT)

  #define ROR32(x, nBits)  RRotU32((x),(nBits))
  #define ROL32(x, nBits)  LRotU32((x),(nBits))

  /* Logical Shifts (right and left) of DWORD */
  #define LSR64(x,nBits)   RShiftU64((x),(nBits))
  #define LSL64(x,nBits)   LShiftU64((x),(nBits))

  /* Rorate (right and left) of DWORD */
  #define ROR64(x, nBits)  RRotU64((x),(nBits))
  #define ROL64(x, nBits)  LRotU64((x),(nBits))

  #define ENDIANNESS(x)    SwapBytes32((x))
  #define ENDIANNESS32(x)  ENDIANNESS((x))
  #define ENDIANNESS64(x)  SwapBytes64((x))

#else
  #if defined(_MSC_VER) && !defined( __ICL )
  #  include <stdlib.h>
  #  define ROR32(x, nBits)  _lrotr((x),(nBits))
  #  define ROL32(x, nBits)  _lrotl((x),(nBits))
  #else
  #  define ROR32(x, nBits) (LSR32((x),(nBits)) | LSL32((x),32-(nBits)))
  #  define ROL32(x, nBits) ROR32((x),(32-(nBits)))
  #endif

  /* Logical Shifts (right and left) of DWORD */
  #define LSR64(x,nBits)  ((x)>>(nBits))
  #define LSL64(x,nBits)  ((x)<<(nBits))

  /* Rorate (right and left) of DWORD */
  #define ROR64(x, nBits) (LSR64((x),(nBits)) | LSL64((x),64-(nBits)))
  #define ROL64(x, nBits) ROR64((x),(64-(nBits)))

  /* change endian */
  #if defined(_MSC_VER)
  #  define ENDIANNESS(x)   _byteswap_ulong((x))
  #  define ENDIANNESS32(x)  ENDIANNESS((x))
  #  define ENDIANNESS64(x) _byteswap_uint64((x))
  #elif defined(__ICL)
  #  define ENDIANNESS(x)   _bswap((x))
  #  define ENDIANNESS32(x)  ENDIANNESS((x))
  #  define ENDIANNESS64(x) _bswap64((x))
  #else
  #  define ENDIANNESS(x) ((ROR32((x), 24) & 0x00ff00ff) | (ROR32((x), 8) & 0xff00ff00))
  #  define ENDIANNESS32(x) ENDIANNESS((x))
  #  define ENDIANNESS64(x) MAKEDWORD(ENDIANNESS(HIDWORD((x))), ENDIANNESS(LODWORD((x))))
  #endif
#endif

#define IPP_MAKE_MULTIPLE_OF_8(x) ((x) = ((x)+7)&(~7))
#define IPP_MAKE_MULTIPLE_OF_16(x) ((x) = ((x)+15)&(~15))

/* define 64-bit constant */
#if !defined(__GNUC__)
   #define CONST_64(x)  (x) /*(x##i64)*/
#else
   #define CONST_64(x)  (x##LL)
#endif

/* define 64-bit constant or pair of 32-bit dependding on architecture */
#if ((_IPP_ARCH == _IPP_ARCH_EM64T) || (_IPP_ARCH == _IPP_ARCH_LP64) || (_IPP_ARCH == _IPP_ARCH_LRB) || (_IPP_ARCH == _IPP_ARCH_LRB2))
#define LL(lo,hi) ((Ipp64u)(((Ipp32u)(lo)) | ((Ipp64u)((Ipp32u)(hi))) << 32))
#define L_(lo)    ((Ipp64u)(lo))
#else
#define LL(lo,hi) (lo),(hi)
#define L_(lo)    (lo)
#endif


/* crypto NI */
#define AES_NI_ENABLED        (ippCPUID_AES)
#define CLMUL_NI_ENABLED      (ippCPUID_CLMUL)
#define AES_CLMUL_NI_ENABLED  (ippCPUID_AES|ippCPUID_CLMUL)
#define ADCOX_ENABLED         (ippCPUID_ADCOX)
#define SHA_NI_ENABLED        (ippCPUID_SHA)
#define RDRAND_NI_ENABLED     (ippCPUID_RDRAND)
#define RDSEED_NI_ENABLED     (ippCPUID_RDSEED)

int cpGetFeature( Ipp64u Feature );
/* test CPU crypto features */
__INLINE Ipp32u IsFeatureEnabled(Ipp64u niMmask)
{
   return cpGetFeature(niMmask);
}

#define IPPCP_GET_NUM_THREADS() ( ippcpGetEnabledNumThreads() )
#define IPPCP_OMP_NUM_THREADS() num_threads( IPPCP_GET_NUM_THREADS() )
#define IPPCP_OMP_LIMIT_MAX_NUM_THREADS(n)  num_threads( IPP_MIN(IPPCP_GET_NUM_THREADS(),(n)))

/* copy under mask */
#define MASKED_COPY_BNU(dst, mask, src1, src2, len) { \
   cpSize i; \
   for(i=0; i<(len); i++) (dst)[i] = ((mask) & (src1)[i]) | (~(mask) & (src2)[i]); \
}

#endif /* __OWNCP_H__ */
