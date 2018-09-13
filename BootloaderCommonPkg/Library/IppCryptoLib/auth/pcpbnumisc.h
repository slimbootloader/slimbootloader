/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/* 
//  Purpose:
//     Intel(R) Integrated Performance Primitives. Cryptography Primitives.
//     Internal Miscellaneous BNU Definitions & Function Prototypes
// 
// 
*/

#if !defined(_PCP_BNUMISC_H)
#define _PCP_BNUMISC_H

#include "pcpbnuimpl.h"


/* bit operations */
#define BITSIZE_BNU(p,ns)  ((ns)*BNU_CHUNK_BITS-cpNLZ_BNU((p)[(ns)-1]))
#define BIT_BNU(bnu, ns,nbit) ((((nbit)>>BNU_CHUNK_LOG2) < (ns))? ((((bnu))[(nbit)>>BNU_CHUNK_LOG2] >>((nbit)&(BNU_CHUNK_BITS))) &1) : 0)
//#define TST_BIT(bnu, nbit)    ((((bnu))[(nbit)>>BNU_CHUNK_LOG2]) &  ((BNU_CHUNK_T)1<<((nbit)&(BNU_CHUNK_BITS-1))))
//#define SET_BIT(bnu, nbit)    ((((bnu))[(nbit)>>BNU_CHUNK_LOG2]) |= ((BNU_CHUNK_T)1<<((nbit)&(BNU_CHUNK_BITS-1))))
//#define CLR_BIT(bnu, nbit)    ((((bnu))[(nbit)>>BNU_CHUNK_LOG2]) &=~((BNU_CHUNK_T)1<<((nbit)&(BNU_CHUNK_BITS-1))))
/**/
#define TST_BIT(bnu, nbit)    (((Ipp8u*)(bnu))[(nbit)/8] &  ((1<<((nbit)%8)) &0xFF))
#define SET_BIT(bnu, nbit)    (((Ipp8u*)(bnu))[(nbit)/8] |= ((1<<((nbit)%8)) &0xFF))
#define CLR_BIT(bnu, nbit)    (((Ipp8u*)(bnu))[(nbit)/8] &=~((1<<((nbit)%8)) &0xFF))

/* convert bitsize nbits into  the number of BNU_CHUNK_T */
#define BITS_BNU_CHUNK(nbits) (((nbits)+BNU_CHUNK_BITS-1)/BNU_CHUNK_BITS)

/* mask for top BNU_CHUNK_T */
#define MASK_BNU_CHUNK(nbits) ((BNU_CHUNK_T)(-1) >>((BNU_CHUNK_BITS- ((nbits)&(BNU_CHUNK_BITS-1))) &(BNU_CHUNK_BITS-1)))

/* copy BNU content */
#define COPY_BNU(dst, src, len) \
{ \
   cpSize __idx; \
   for(__idx=0; __idx<(len); __idx++) (dst)[__idx] = (src)[__idx]; \
}

/* expand by zeros */
#define ZEXPAND_BNU(srcdst,srcLen, dstLen) \
{ \
   cpSize __idx; \
   for(__idx=(srcLen); __idx<(dstLen); __idx++) (srcdst)[__idx] = 0; \
}

/* copy and expand by zeros */
#define ZEXPAND_COPY_BNU(dst,dstLen, src,srcLen) \
{ \
   cpSize __idx; \
   for(__idx=0; __idx<(srcLen); __idx++) (dst)[__idx] = (src)[__idx]; \
   for(; __idx<(dstLen); __idx++)    (dst)[__idx] = 0; \
}

/* fix actual length */
#define FIX_BNU(src,srcLen) \
   {for(; ((srcLen)>1) && (0==(src)[(srcLen)-1]); (srcLen)--);}


/* copy and set */
__INLINE void cpCpy_BNU(BNU_CHUNK_T* pDst, const BNU_CHUNK_T* pSrc, cpSize ns)
{  COPY_BNU(pDst, pSrc, ns); }

__INLINE void cpSet_BNU(BNU_CHUNK_T* pDst, cpSize ns, BNU_CHUNK_T val)
{
   ZEXPAND_BNU(pDst, 0, ns);
   pDst[0] = val;
}

/* fix up */
__INLINE int cpFix_BNU(const BNU_CHUNK_T* pA, int nsA)
{
   FIX_BNU(pA, nsA);
   return nsA;
}

/* comparison
//
// returns
//    negative, if A < B
//           0, if A = B
//    positive, if A > B
*/
__INLINE int cpCmp_BNU(const BNU_CHUNK_T* pA, cpSize nsA, const BNU_CHUNK_T* pB, cpSize nsB)
{
   if(nsA!=nsB)
      return nsA>nsB? 1 : -1;
   else {
      for(; nsA>0; nsA--) {
         if(pA[nsA-1] > pB[nsA-1])
            return 1;
         else if(pA[nsA-1] < pB[nsA-1])
            return -1;
      }
      return 0;
   }
}
__INLINE int cpEqu_BNU_CHUNK(const BNU_CHUNK_T* pA, cpSize nsA, BNU_CHUNK_T b)
{
   return (pA[0]==b && 1==cpFix_BNU(pA, nsA));
}

/*
// test
//
// returns
//     0, if A = 0
//    >0, if A > 0
//    <0, looks like impossible (or error) case
*/
__INLINE int cpTst_BNU(const BNU_CHUNK_T* pA, int nsA)
{
   for(; (nsA>0) && (0==pA[nsA-1]); nsA--) ;
   return nsA;
}

/* number of leading/trailing zeros */
#define cpNLZ_BNU OWNAPI(cpNLZ_BNU)
cpSize cpNLZ_BNU(BNU_CHUNK_T x);

#define cpNTZ_BNU OWNAPI(cpNTZ_BNU)
cpSize cpNTZ_BNU(BNU_CHUNK_T x);

/* logical shift left/right */
#define cpLSR_BNU OWNAPI(cpLSR_BNU)
    int cpLSR_BNU(BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, cpSize nsA, cpSize nBits);

/* least and most significant BNU bit */
#define cpMSBit_BNU OWNAPI(cpMSBit_BNU)
int cpMSBit_BNU(const BNU_CHUNK_T* pA, cpSize nsA);

/* BNU <-> hex-string conversion */
#define cpToOctStr_BNU OWNAPI(cpToOctStr_BNU)
int cpToOctStr_BNU(Ipp8u* pStr, cpSize strLen, const BNU_CHUNK_T* pA, cpSize nsA);
#define cpFromOctStr_BNU OWNAPI(cpFromOctStr_BNU)
int cpFromOctStr_BNU(BNU_CHUNK_T* pA, const Ipp8u* pStr, cpSize strLen);

#endif /* _PCP_BNUMISC_H */
