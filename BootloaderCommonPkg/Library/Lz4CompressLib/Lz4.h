/*
   LZ4 - Fast LZ compression algorithm
   Copyright (C) 2011-present, Yann Collet.

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
    - LZ4 homepage : http://www.lz4.org
    - LZ4 source repository : https://github.com/lz4/lz4

*/
#ifndef __LZ4_H__
#define __LZ4_H__

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

/*========== Version =========== */
#define LZ4_VERSION_MAJOR     1    /* for breaking interface changes  */
#define LZ4_VERSION_MINOR     7    /* for new (non-breaking) interface capabilities */
#define LZ4_VERSION_RELEASE   4    /* for tweaks, bug-fixes, or development */

#define FORCE_INLINE
#define USE_DICT              0

typedef unsigned char         BYTE;
typedef unsigned int          U32;
typedef unsigned short        U16;
typedef UINT64                U64;
typedef UINTN                 size_t;
typedef UINTN                 reg_t;
typedef UINTN*                uptrval;

#define memcpy                CopyMem
#define memmove               CopyMem
#define LZ4_readLE16(x)       (*(UINT16 *)(x))
#define LZ4_read16(x)         (*(UINT16 *)(x))
#define LZ4_writeLE16(x, y)   *(UINT16*)(x) = y
#define LZ4_read32(x)         (*(UINT32*)(x))
#define LZ4_write32(x, y)     *(UINT32*)(x) = y
#define expect(expr,value)    (expr)
#define likely(expr)          expect((expr) != 0, 1)
#define unlikely(expr)        expect((expr) != 0, 0)
#define LZ4_wildCopy(d,s,e)   {UINT64 *d1 = (UINT64 *)(d); UINT64 *s1 = (UINT64 *)(s); do {*d1++=*s1++;} while ((BYTE *)d1<e);}
#define LZ4_copy8(d,s)        *(UINT64 *)(d) = *(UINT64 *)(s)

/*-************************************
*  Common Constants
**************************************/
#define MINMATCH 4

#define WILDCOPYLENGTH 8
#define LASTLITERALS 5
#define MFLIMIT (WILDCOPYLENGTH+MINMATCH)

#define KB *(1 <<10)
#define MB *(1 <<20)
#define GB *(1U<<30)

#define MAXD_LOG 16
#define MAX_DISTANCE ((1 << MAXD_LOG) - 1)

#define ML_BITS  4
#define ML_MASK  ((1U<<ML_BITS)-1)
#define RUN_BITS (8-ML_BITS)
#define RUN_MASK ((1U<<RUN_BITS)-1)

/**
 * This enum distinguishes several different modes of accessing previous
 * content in the stream.
 *
 * - noDict        : There is no preceding content.
 * - withPrefix64k : Table entries up to ctx->dictSize before the current blob
 *                   blob being compressed are valid and refer to the preceding
 *                   content (of length ctx->dictSize), which is available
 *                   contiguously preceding in memory the content currently
 *                   being compressed.
 * - usingExtDict  : Like withPrefix64k, but the preceding content is somewhere
 *                   else in memory, starting at ctx->dictionary with length
 *                   ctx->dictSize.
 * - usingDictCtx  : Like usingExtDict, but everything concerning the preceding
 *                   content is in a separate context, pointed to by
 *                   ctx->dictCtx. ctx->dictionary, ctx->dictSize, and table
 *                   entries in the current context that refer to positions
 *                   preceding the beginning of the current compression are
 *                   ignored. Instead, ctx->dictCtx->dictionary and ctx->dictCtx
 *                   ->dictSize describe the location and size of the preceding
 *                   content, and matches are found by looking in the ctx
 *                   ->dictCtx->hashTable.
 */
typedef enum { noDict = 0, withPrefix64k, usingExtDict, usingDictCtx } dict_directive;
typedef enum { noDictIssue = 0, dictSmall } dictIssue_directive;
typedef enum { endOnOutputSize = 0, endOnInputSize = 1 } endCondition_directive;
typedef enum { full = 0, partial = 1 } earlyEnd_directive;

int LZ4_compressBound(int isize);
int LZ4_decompress_safe(const char* source, char* dest, int compressedSize, int maxDecompressedSize);
int LZ4_get_ctx_size (void);
int LZ4_compressHC_withStateHC (void* state, const char* src, char* dst, int srcSize);

#endif
