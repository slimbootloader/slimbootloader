/*
   LZ4 - Fast LZ compression algorithm
   Copyright (C) 2011-2014, Yann Collet.
   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   You can contact the author at :
   - LZ4 source repository : http://code.google.com/p/lz4/
   - LZ4 public forum : https://groups.google.com/forum/#!forum/lz4c
*/

/**************************************
   Tuning parameters
**************************************/
/*
 * HEAPMODE :
 * Select how default compression functions will allocate memory for their hash table,
 * in memory stack (0:default, fastest), or in memory heap (1:requires memory allocation (malloc)).
 */
#define HEAPMODE 0

#define memcpy   CopyMem

/**************************************
   CPU Feature Detection
**************************************/
/* 32 or 64 bits ? */
#if (defined(__x86_64__) || defined(_M_X64) || defined(_WIN64) \
  || defined(__64BIT__)  || defined(__mips64) \
  || defined(__powerpc64__) || defined(__powerpc64le__) \
  || defined(__ppc64__) || defined(__ppc64le__) \
  || defined(__PPC64__) || defined(__PPC64LE__) \
  || defined(__ia64) || defined(__itanium__) || defined(_M_IA64) \
  || defined(__s390x__) )   /* Detects 64 bits mode */
#  define LZ4_ARCH64 1
#else
#  define LZ4_ARCH64 0
#endif
#define LZ4_32BITS (sizeof(void*)==4)
#define LZ4_64BITS (sizeof(void*)==8)

/* Little Endian assumed. PDP Endian and other very rare endian format are unsupported. */

/*
 * Unaligned memory access is automatically enabled for "common" CPU, such as x86.
 * For others CPU, such as ARM, the compiler may be more cautious, inserting unnecessary extra code to ensure aligned access property
 * If you know your target CPU supports unaligned memory access, you want to force this option manually to improve performance
 */
#if defined(__ARM_FEATURE_UNALIGNED)
#  define LZ4_FORCE_UNALIGNED_ACCESS 1
#endif

/* Define this parameter if your target system or compiler does not support hardware bit count */
#if defined(_MSC_VER) && defined(_WIN32_WCE)   /* Visual Studio for Windows CE does not support Hardware bit count */
#  define LZ4_FORCE_SW_BITCOUNT
#endif

/*
 * BIG_ENDIAN_NATIVE_BUT_INCOMPATIBLE :
 * This option may provide a small boost to performance for some big endian cpu, although probably modest.
 * You may set this option to 1 if data will remain within closed environment.
 * This option is useless on Little_Endian CPU (such as x86)
 */

/* #define BIG_ENDIAN_NATIVE_BUT_INCOMPATIBLE 1 */


/**************************************
 Compiler Options
**************************************/
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)   /* C99 */
/* "restrict" is a known keyword */
#else
#  define restrict /* Disable restrict */
#endif

#ifdef _MSC_VER    /* Visual Studio */
#  define FORCE_INLINE static __forceinline
//#  include <intrin.h>                    /* For Visual 2005 */
#  if LZ4_ARCH64   /* 64-bits */
#    pragma intrinsic(_BitScanForward64) /* For Visual 2005 */
#    pragma intrinsic(_BitScanReverse64) /* For Visual 2005 */
#  else            /* 32-bits */
#    pragma intrinsic(_BitScanForward)   /* For Visual 2005 */
#    pragma intrinsic(_BitScanReverse)   /* For Visual 2005 */
#  endif
#  pragma warning(disable : 4127)        /* disable: C4127: conditional expression is constant */
#else
#  ifdef __GNUC__
#    define FORCE_INLINE static inline __attribute__((always_inline))
#  else
#    define FORCE_INLINE static inline
#  endif
#endif

#ifdef _MSC_VER  /* Visual Studio */
#  define lz4_bswap16(x) _byteswap_ushort(x)
#else
#  define lz4_bswap16(x) ((unsigned short int) ((((x) >> 8) & 0xffu) | (((x) & 0xffu) << 8)))
#endif

#define GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)

#if (GCC_VERSION >= 302) || (__INTEL_COMPILER >= 800) || defined(__clang__)
#  define expect(expr,value)    (__builtin_expect ((expr),(value)) )
#else
#  define expect(expr,value)    (expr)
#endif

#define likely(expr)     expect((expr) != 0, 1)
#define unlikely(expr)   expect((expr) != 0, 0)


/**************************************
   Memory routines
**************************************/
//#include <stdlib.h>   /* malloc, calloc, free */
#define ALLOCATOR(n,s) calloc(n,s)
#define FREEMEM        free
//#include <string.h>   /* memset, memcpy */
#define MEM_INIT       memset


/**************************************
   Includes
**************************************/
#include "Lz4.h"
#include <Library/BaseMemoryLib.h>
//#include "bootloader.h"
//#include "sa/string.h"      /* memcpy */

/**************************************
   Basic Types
**************************************/
typedef unsigned char       BYTE;
typedef unsigned short      U16;
typedef unsigned int        U32;
typedef   signed int        S32;
typedef unsigned long long  U64;

#if defined(__GNUC__)  && !defined(LZ4_FORCE_UNALIGNED_ACCESS)
#  define _PACKED __attribute__ ((packed))
#else
#  define _PACKED
#endif

#if !defined(LZ4_FORCE_UNALIGNED_ACCESS) && !defined(__GNUC__)
#  if defined(__IBMC__) || defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#    pragma pack(1)
#  else
#    pragma pack(push, 1)
#  endif
#endif

typedef struct {
  U16 v;
}  _PACKED U16_S;
typedef struct {
  U32 v;
}  _PACKED U32_S;
typedef struct {
  U64 v;
}  _PACKED U64_S;
typedef struct {
  size_t v;
} _PACKED size_t_S;

#if !defined(LZ4_FORCE_UNALIGNED_ACCESS) && !defined(__GNUC__)
#  if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#    pragma pack(0)
#  else
#    pragma pack(pop)
#  endif
#endif

#define A16(x)   (((U16_S *)(x))->v)
#define A32(x)   (((U32_S *)(x))->v)
#define A64(x)   (((U64_S *)(x))->v)
#define AARCH(x) (((size_t_S *)(x))->v)


/**************************************
   Constants
**************************************/
#define LZ4_HASHLOG   (LZ4_MEMORY_USAGE-2)
#define HASHTABLESIZE (1 << LZ4_MEMORY_USAGE)
#define HASH_SIZE_U32 (1 << LZ4_HASHLOG)

#define MINMATCH 4

#define COPYLENGTH 8
#define LASTLITERALS 5
#define MFLIMIT (COPYLENGTH+MINMATCH)
//static const int LZ4_minLength = (MFLIMIT+1);

#define KB *(1U<<10)
#define MB *(1U<<20)
#define GB *(1U<<30)

#define LZ4_64KLIMIT ((64 KB) + (MFLIMIT-1))
#define SKIPSTRENGTH 6   /* Increasing this value will make the compression run slower on incompressible data */

#define MAXD_LOG 16
#define MAX_DISTANCE ((1 << MAXD_LOG) - 1)

#define ML_BITS  4
#define ML_MASK  ((1U<<ML_BITS)-1)
#define RUN_BITS (8-ML_BITS)
#define RUN_MASK ((1U<<RUN_BITS)-1)


/**************************************
   Structures and local types
**************************************/
typedef struct {
  U32  hashTable[HASH_SIZE_U32];
  U32  currentOffset;
  U32  initCheck;
  const BYTE *dictionary;
  const BYTE *bufferStart;
  U32  dictSize;
} LZ4_stream_t_internal;

typedef enum { notLimited = 0, limitedOutput = 1 } limitedOutput_directive;
typedef enum { byPtr, byU32, byU16 } tableType_t;

typedef enum { noDict = 0, withPrefix64k, usingExtDict } dict_directive;
typedef enum { noDictIssue = 0, dictSmall } dictIssue_directive;

typedef enum { endOnOutputSize = 0, endOnInputSize = 1 } endCondition_directive;
typedef enum { full = 0, partial = 1 } earlyEnd_directive;


/**************************************
   Architecture-specific macros
**************************************/
#define STEPSIZE                  sizeof(size_t)
#define LZ4_COPYSTEP(d,s)         { AARCH(d) = AARCH(s); d+=STEPSIZE; s+=STEPSIZE; }
#define LZ4_COPY8(d,s)            { LZ4_COPYSTEP(d,s); if (STEPSIZE<8) LZ4_COPYSTEP(d,s); }

#if (defined(LZ4_BIG_ENDIAN) && !defined(BIG_ENDIAN_NATIVE_BUT_INCOMPATIBLE))
#  define LZ4_READ_LITTLEENDIAN_16(d,s,p) { U16 v = A16(p); v = lz4_bswap16(v); d = (s) - v; }
#  define LZ4_WRITE_LITTLEENDIAN_16(p,i)  { U16 v = (U16)(i); v = lz4_bswap16(v); A16(p) = v; p+=2; }
#else      /* Little Endian */
#  define LZ4_READ_LITTLEENDIAN_16(d,s,p) { d = (s) - A16(p); }
#  define LZ4_WRITE_LITTLEENDIAN_16(p,v)  { A16(p) = v; p+=2; }
#endif


/**************************************
   Macros
**************************************/
#define LZ4_STATIC_ASSERT(c)    { enum { LZ4_static_assert = 1/(int)(!!(c)) }; }   /* use only *after* variable declarations */
#if LZ4_ARCH64 || !defined(__GNUC__)
#  define LZ4_WILDCOPY(d,s,e)   { do { LZ4_COPY8(d,s) } while (d<e); }        /* at the end, d>=e; */
#else
#  define LZ4_WILDCOPY(d,s,e)   { if (likely(e-d <= 8)) LZ4_COPY8(d,s) else do { LZ4_COPY8(d,s) } while (d<e); }
#endif


/****************************
   Decompression functions
****************************/
/*
 * This generic decompression function cover all use cases.
 * It shall be instantiated several times, using different sets of directives
 * Note that it is essential this generic function is really inlined,
 * in order to remove useless branches during compilation optimization.
 */
FORCE_INLINE int LZ4_decompress_generic (
  const char *const source,
  char *const dest,
  int inputSize,
  int outputSize,         /* If endOnInput==endOnInputSize, this value is the max size of Output Buffer. */

  int endOnInput,         /* endOnOutputSize, endOnInputSize */
  int partialDecoding,    /* full, partial */
  int targetOutputSize,   /* only used if partialDecoding==partial */
  int dict,               /* noDict, withPrefix64k, usingExtDict */
  const BYTE *const lowPrefix,  /* == dest if dict == noDict */
  const BYTE *const dictStart,  /* only if dict==usingExtDict */
  const size_t dictSize         /* note : = 0 if noDict */
  )
{
  /* Local Variables */
  const BYTE *restrict ip = (const BYTE *) source;
  const BYTE *const iend = ip + inputSize;

  BYTE *op = (BYTE *) dest;
  BYTE *const oend = op + outputSize;
  BYTE *cpy;
  BYTE *oexit = op + targetOutputSize;
  const BYTE *const lowLimit = lowPrefix - dictSize;

  const BYTE *const dictEnd = (const BYTE *)dictStart + dictSize;
  const size_t dec32table[] = {4, 1, 2, 1, 4, 4, 4, 4};
  const size_t dec64table[] = {0, 0, 0, (size_t) -1, 0, 1, 2, 3};

  const int safeDecode = (endOnInput == endOnInputSize);
  const int checkOffset = ((safeDecode) && (dictSize < (int) (64 KB)));


  /* Special cases */
  if ((partialDecoding) && (oexit > oend - MFLIMIT)) {
    oexit = oend - MFLIMIT;  /* targetOutputSize too high => decode everything */
  }
  if ((endOnInput) && (unlikely (outputSize == 0))) {
    return ((inputSize == 1) && (*ip == 0)) ? 0 : -1;  /* Empty output buffer */
  }
  if ((!endOnInput) && (unlikely (outputSize == 0))) {
    return (*ip == 0 ? 1 : -1);
  }


  /* Main Loop */
  while (1) {
    unsigned token;
    size_t length;
    const BYTE *match;

    /* get literal length */
    token = *ip++;
    if ((length = (token >> ML_BITS)) == RUN_MASK) {
      unsigned s;
      do {
        s = *ip++;
        length += s;
      } while (likely ((endOnInput) ? ip < iend - RUN_MASK : 1) && (s == 255));
      if ((safeDecode) && unlikely ((size_t) (op + length) < (size_t) (op))) {
        goto _output_error;  /* overflow detection */
      }
      if ((safeDecode) && unlikely ((size_t) (ip + length) < (size_t) (ip))) {
        goto _output_error;  /* overflow detection */
      }
    }

    /* copy literals */
    cpy = op + length;
    if (((endOnInput) && ((cpy > (partialDecoding ? oexit : oend - MFLIMIT))
                          || (ip + length > iend - (2 + 1 + LASTLITERALS))) )
        || ((!endOnInput) && (cpy > oend - COPYLENGTH))) {
      if (partialDecoding) {
        if (cpy > oend) {
          goto _output_error;  /* Error : write attempt beyond end of output buffer */
        }
        if ((endOnInput) && (ip + length > iend)) {
          goto _output_error;  /* Error : read attempt beyond end of input buffer */
        }
      } else {
        if ((!endOnInput) && (cpy != oend)) {
          goto _output_error;  /* Error : block decoding must stop exactly there */
        }
        if ((endOnInput) && ((ip + length != iend) || (cpy > oend))) {
          goto _output_error;  /* Error : input must be consumed */
        }
      }
      memcpy (op, ip, length);
      ip += length;
      op += length;
      break;     /* Necessarily EOF, due to parsing restrictions */
    }
    LZ4_WILDCOPY (op, ip, cpy);
    ip -= (op - cpy);
    op = cpy;

    /* get offset */
    LZ4_READ_LITTLEENDIAN_16 (match, cpy, ip);
    ip += 2;
    if ((checkOffset) && (unlikely (match < lowLimit))) {
      goto _output_error;  /* Error : offset outside destination buffer */
    }

    /* get matchlength */
    if ((length = (token & ML_MASK)) == ML_MASK) {
      unsigned s;
      do {
        if ((endOnInput) && (ip > iend - LASTLITERALS)) {
          goto _output_error;
        }
        s = *ip++;
        length += s;
      } while (s == 255);
      if ((safeDecode) && unlikely ((size_t) (op + length) < (size_t)op)) {
        goto _output_error;  /* overflow detection */
      }
    }
    length += MINMATCH;

    /* check external dictionary */
    if ((dict == usingExtDict) && (match < lowPrefix)) {
      if (unlikely (op + length > oend - LASTLITERALS)) {
        goto _output_error;  /* doesn't respect parsing restriction */
      }

      if (length <= (size_t) (lowPrefix - match)) {
        /* match can be copied as a single segment from external dictionary */
        match = dictEnd - (lowPrefix - match);
        memcpy (op, match, length);
        op += length;
      } else {
        /* match encompass external dictionary and current segment */
        size_t copySize = (size_t) (lowPrefix - match);
        memcpy (op, dictEnd - copySize, copySize);
        op += copySize;
        copySize = length - copySize;
        if (copySize > (size_t) (op - lowPrefix)) { /* overlap within current segment */
          BYTE *const endOfMatch = op + copySize;
          const BYTE *copyFrom = lowPrefix;
          while (op < endOfMatch) {
            *op++ = *copyFrom++;
          }
        } else {
          memcpy (op, lowPrefix, copySize);
          op += copySize;
        }
      }
      continue;
    }

    /* copy repeated sequence */
    cpy = op + length;
    if (unlikely ((op - match) < (int)STEPSIZE)) {
      const size_t dec64 = dec64table[op - match];
      op[0] = match[0];
      op[1] = match[1];
      op[2] = match[2];
      op[3] = match[3];
      match += dec32table[op - match];
      A32 (op + 4) = A32 (match);
      op += 8;
      match -= dec64;
    } else {
      LZ4_COPY8 (op, match);
    }

    if (unlikely (cpy > oend - 12)) {
      if (cpy > oend - LASTLITERALS) {
        goto _output_error;  /* Error : last 5 bytes must be literals */
      }
      if (op < oend - COPYLENGTH) {
        LZ4_WILDCOPY (op, match, (oend - COPYLENGTH));
      }
      while (op < cpy) {
        *op++ = *match++;
      }
    } else {
      LZ4_WILDCOPY (op, match, cpy);
    }
    op = cpy; /* correction */
  }

  /* end of decoding */
  if (endOnInput) {
    return (int) (((char *)op) - dest);  /* Nb of output bytes decoded */
  } else {
    return (int) (((char *)ip) - source);  /* Nb of input bytes read */
  }

  /* Overflow error detected */
_output_error:
  return (int) (- (((char *)ip) - source)) - 1;
}

int LZ4_decompress_fast (const char *source, char *dest, int originalSize)
{
  return LZ4_decompress_generic (source, dest, 0, originalSize, endOnOutputSize, full, 0, withPrefix64k,
                                 (BYTE *) (dest - 64 KB), NULL, 64 KB);
}

int LZ4_decompress_safe (const char *source, char *dest, int compressedSize, int maxDecompressedSize)
{
  return LZ4_decompress_generic (source, dest, compressedSize, maxDecompressedSize, endOnInputSize, full, 0, noDict,
                                 (BYTE *)dest, NULL, 0);
}

/**
  Given a LZ4 compressed source buffer, this function retrieves the size of
  the uncompressed buffer and the size of the scratch buffer required
  to decompress the compressed source buffer.

  Retrieves the size of the uncompressed buffer and the temporary scratch buffer
  required to decompress the buffer specified by Source and SourceSize.
  The size of the uncompressed buffer is returned in DestinationSize,
  the size of the scratch buffer is returned in ScratchSize, and RETURN_SUCCESS is returned.
  This function does not have scratch buffer available to perform a thorough
  checking of the validity of the source data. It just retrieves the "Original Size"
  field from the LZMA_HEADER_SIZE beginning bytes of the source data and output it as DestinationSize.
  And ScratchSize is specific to the decompression implementation.

  If SourceSize is less than LZMA_HEADER_SIZE, then ASSERT().

  @param  Source          The source buffer containing the compressed data.
  @param  SourceSize      The size, in bytes, of the source buffer.
  @param  DestinationSize A pointer to the size, in bytes, of the uncompressed buffer
                          that will be generated when the compressed buffer specified
                          by Source and SourceSize is decompressed.
  @param  ScratchSize     A pointer to the size, in bytes, of the scratch buffer that
                          is required to decompress the compressed buffer specified
                          by Source and SourceSize.

  @retval  RETURN_SUCCESS The size of the uncompressed data was returned
                          in DestinationSize and the size of the scratch
                          buffer was returned in ScratchSize.

**/
RETURN_STATUS
EFIAPI
Lz4DecompressGetInfo (
  IN  CONST VOID  *Source,
  IN  UINT32       SourceSize,
  OUT UINT32      *DestinationSize,
  OUT UINT32      *ScratchSize
  )
{
  if (DestinationSize) {
    *DestinationSize = * (UINT32 *)Source;
  }

  if (ScratchSize) {
    *ScratchSize = 0;
  }

  return RETURN_SUCCESS;
}

/**
  Decompresses a Lzma compressed source buffer.

  Extracts decompressed data to its original form.
  If the compressed source data specified by Source is successfully decompressed
  into Destination, then RETURN_SUCCESS is returned.  If the compressed source data
  specified by Source is not in a valid compressed data format,
  then RETURN_INVALID_PARAMETER is returned.

  @param  Source      The source buffer containing the compressed data.
  @param  SourceSize  The size of source buffer.
  @param  Destination The destination buffer to store the decompressed data
  @param  Scratch     A temporary scratch buffer that is used to perform the decompression.
                      This is an optional parameter that may be NULL if the
                      required scratch buffer size is 0.

  @retval  RETURN_SUCCESS Decompression completed successfully, and
                          the uncompressed buffer is returned in Destination.
  @retval  RETURN_INVALID_PARAMETER
                          The source buffer specified by Source is corrupted
                          (not in a valid compressed format).
**/
RETURN_STATUS
EFIAPI
Lz4Decompress (
  IN CONST VOID  *Source,
  IN UINTN       SourceSize,
  IN OUT VOID    *Destination,
  IN OUT VOID    *Scratch
  )
{
  UINT32       DecompressedSize;
  UINT32       Size;

  DecompressedSize  = * (UINT32 *)Source;
  Source = (VOID *) ((CHAR8 *)Source + sizeof (UINT32));
  SourceSize -= sizeof (UINT32);
  Size = (UINT32)LZ4_decompress_safe (Source, Destination, SourceSize, DecompressedSize);
  if ((UINT32)Size == DecompressedSize) {
    return RETURN_SUCCESS;
  } else {
    return RETURN_INVALID_PARAMETER;
  }
}
