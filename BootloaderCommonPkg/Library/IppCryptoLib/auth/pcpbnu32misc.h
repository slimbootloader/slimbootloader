/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/* 
//  Purpose:
//     Intel(R) Integrated Performance Primitives. Cryptography Primitives.
//     Internal Miscellaneous BNU 32 bit Definitions & Function Prototypes
// 
// 
*/

#if !defined(_CP_BNU32_MISC_H)
#define _CP_BNU32_MISC_H


/* bit operations */
#define BITSIZE_BNU32(p,ns)  ((ns)*BNU_CHUNK_32BIT-cpNLZ_BNU32((p)[(ns)-1]))

/* number of leading/trailing zeros */
#define cpNLZ_BNU32 OWNAPI(cpNLZ_BNU32)
cpSize cpNLZ_BNU32(Ipp32u x);

/* most significant BNU bit */
__INLINE int cpMSBit_BNU32(const Ipp32u* pA, cpSize nsA)
{
   FIX_BNU(pA, nsA);
   return nsA*BITSIZE(Ipp32u) - cpNLZ_BNU32(pA[nsA-1]) -1;
}


__INLINE int cpCmp_BNU32(const Ipp32u* pA, cpSize nsA, const Ipp32u* pB, cpSize nsB)
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

/* to/from oct string conversion */
#define cpToOctStr_BNU32 OWNAPI(cpToOctStr_BNU32)
cpSize cpToOctStr_BNU32(Ipp8u* pStr, cpSize strLen, const Ipp32u* pBNU, cpSize bnuSize);
#define cpFromOctStr_BNU32 OWNAPI(cpFromOctStr_BNU32)
cpSize cpFromOctStr_BNU32(Ipp32u* pBNU, const Ipp8u* pOctStr, cpSize strLen);

#endif /* _CP_BNU32_MISC_H */
