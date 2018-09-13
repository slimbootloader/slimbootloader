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
//     Intel(R) Integrated Performance Primitives.
//     Internal Unsigned internal arithmetic
// 
// 
*/

#if !defined(_CP_BNU_ARITH_H)
#define _CP_BNU_ARITH_H

#include "pcpbnuimpl.h"
#include "pcpbnu32arith.h"
#include "pcpmulbnukara.h"

#define     cpAdd_BNU OWNAPI(cpAdd_BNU)
BNU_CHUNK_T cpAdd_BNU(BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, const BNU_CHUNK_T* pB, cpSize ns);
#define     cpSub_BNU OWNAPI(cpSub_BNU)
BNU_CHUNK_T cpSub_BNU(BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, const BNU_CHUNK_T* pB, cpSize ns);
#define     cpInc_BNU OWNAPI(cpInc_BNU)
BNU_CHUNK_T cpInc_BNU(BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, cpSize ns, BNU_CHUNK_T val);
#define     cpDec_BNU OWNAPI(cpDec_BNU)
BNU_CHUNK_T cpDec_BNU(BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, cpSize ns, BNU_CHUNK_T val);

#if defined(_USE_KARATSUBA_)
#define     cpAddAdd_BNU OWNAPI(cpAddAdd_BNU)
BNU_CHUNK_T cpAddAdd_BNU(BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, const BNU_CHUNK_T* pB, const BNU_CHUNK_T* pC, cpSize size);
#define     cpAddSub_BNU OWNAPI(cpAddSub_BNU)
BNU_CHUNK_T cpAddSub_BNU(BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, const BNU_CHUNK_T* pB, const BNU_CHUNK_T* pC, cpSize size);
#endif

#define     cpAddMulDgt_BNU OWNAPI(cpAddMulDgt_BNU)
BNU_CHUNK_T cpAddMulDgt_BNU(BNU_CHUNK_T* pR, const BNU_CHUNK_T* pA, cpSize ns, BNU_CHUNK_T val);


#define     cpMulAdc_BNU_school OWNAPI(cpMulAdc_BNU_school)
BNU_CHUNK_T cpMulAdc_BNU_school(BNU_CHUNK_T* pR,
                         const BNU_CHUNK_T* pA, cpSize nsA,
                         const BNU_CHUNK_T* pB, cpSize nsB);
#define     cpMulAdx_BNU_school OWNAPI(cpMulAdx_BNU_school)
BNU_CHUNK_T cpMulAdx_BNU_school(BNU_CHUNK_T* pR,
                         const BNU_CHUNK_T* pA, cpSize nsA,
                         const BNU_CHUNK_T* pB, cpSize nsB);

__INLINE BNU_CHUNK_T cpMul_BNU_school(BNU_CHUNK_T* pR,
                                const BNU_CHUNK_T* pA, cpSize nsA,
                                const BNU_CHUNK_T* pB, cpSize nsB)
{
#if(_ADCOX_NI_ENABLING_==_FEATURE_ON_)
   return cpMulAdx_BNU_school(pR, pA,nsA, pB,nsB);
#elif(_ADCOX_NI_ENABLING_==_FEATURE_TICKTOCK_)
   return IsFeatureEnabled(ADCOX_ENABLED)? cpMulAdx_BNU_school(pR, pA,nsA, pB,nsB)
                                         : cpMulAdc_BNU_school(pR, pA,nsA, pB,nsB);
#else
   return cpMulAdc_BNU_school(pR, pA,nsA, pB,nsB);
#endif
}


#define     cpSqrAdc_BNU_school OWNAPI(cpSqrAdc_BNU_school)
BNU_CHUNK_T cpSqrAdc_BNU_school(BNU_CHUNK_T * pR, const BNU_CHUNK_T * pA, cpSize nsA);

#define     cpSqrAdx_BNU_school OWNAPI(cpSqrAdx_BNU_school)
BNU_CHUNK_T cpSqrAdx_BNU_school(BNU_CHUNK_T * pR, const BNU_CHUNK_T * pA, cpSize nsA);

__INLINE BNU_CHUNK_T cpSqr_BNU_school(BNU_CHUNK_T * pR, const BNU_CHUNK_T * pA, cpSize nsA)
{
#if(_ADCOX_NI_ENABLING_==_FEATURE_ON_)
   return cpSqrAdx_BNU_school(pR, pA,nsA);
#elif(_ADCOX_NI_ENABLING_==_FEATURE_TICKTOCK_)
   return IsFeatureEnabled(ADCOX_ENABLED)? cpSqrAdx_BNU_school(pR, pA,nsA)
                                         : cpSqrAdc_BNU_school(pR, pA,nsA);
#else
   return cpSqrAdc_BNU_school(pR, pA,nsA);
#endif
}

#define     cpGcd_BNU OWNAPI(cpGcd_BNU)
BNU_CHUNK_T cpGcd_BNU(BNU_CHUNK_T a, BNU_CHUNK_T b);

#define cpModInv_BNU OWNAPI(cpModInv_BNU)
int cpModInv_BNU(BNU_CHUNK_T* pInv,
           const BNU_CHUNK_T* pA, cpSize nsA,
           const BNU_CHUNK_T* pM, cpSize nsM,
                 BNU_CHUNK_T* bufInv, BNU_CHUNK_T* bufA, BNU_CHUNK_T* bufM);


/*
// multiplication/squaring wrappers
*/
__INLINE cpSize cpMul_BNU_BufferSize(cpSize opLen)
{
#if !defined (_USE_KARATSUBA_)
   UNREFERENCED_PARAMETER(opLen);
   return 0;
#else
   return cpKaratsubaBufferSize(opLen);
#endif
}
__INLINE BNU_CHUNK_T cpMul_BNU(BNU_CHUNK_T* pR,
                         const BNU_CHUNK_T* pA, cpSize nsA,
                         const BNU_CHUNK_T* pB, cpSize nsB,
                               BNU_CHUNK_T* pBuffer)
{
#if !defined(_USE_KARATSUBA_)
   UNREFERENCED_PARAMETER(pBuffer);
   return cpMul_BNU_school(pR, pA,nsA, pB,nsB);
#else
   if(nsA!=nsB || nsA<CP_KARATSUBA_MUL_THRESHOLD || !pBuffer)
      return cpMul_BNU_school(pR, pA,nsA, pB,nsB);
   else
      return cpMul_BNU_karatsuba(pR, pA,pB,nsA, pBuffer);
#endif
}
__INLINE BNU_CHUNK_T cpSqr_BNU(BNU_CHUNK_T * pR,
                         const BNU_CHUNK_T * pA, cpSize nsA,
                               BNU_CHUNK_T* pBuffer)
{
#if !defined(_USE_KARATSUBA_)
   UNREFERENCED_PARAMETER(pBuffer);
   return cpSqr_BNU_school(pR, pA,nsA);
#else
   if(nsA<CP_KARATSUBA_SQR_THRESHOLD || !pBuffer)
      return cpSqr_BNU_school(pR, pA,nsA);
   else
      return cpSqr_BNU_karatsuba(pR, pA,nsA, pBuffer);
#endif
}

/*
// division/reduction wrappers
*/
__INLINE cpSize cpDiv_BNU(BNU_CHUNK_T* pQ, cpSize* pnsQ, BNU_CHUNK_T* pA, cpSize nsA, BNU_CHUNK_T* pB, cpSize nsB)
{
   int nsR = cpDiv_BNU32((Ipp32u*)pQ, pnsQ,
                         (Ipp32u*)pA, nsA*(sizeof(BNU_CHUNK_T)/sizeof(Ipp32u)),
                         (Ipp32u*)pB, nsB*(sizeof(BNU_CHUNK_T)/sizeof(Ipp32u)));
   #if (BNU_CHUNK_BITS == BNU_CHUNK_64BIT)
   if(nsR&1) ((Ipp32u*)pA)[nsR] = 0;
   nsR = INTERNAL_BNU_LENGTH(nsR);
   if(pQ) {
      if(*pnsQ&1) ((Ipp32u*)pQ)[*pnsQ] = 0;
      *pnsQ = INTERNAL_BNU_LENGTH(*pnsQ);
   }
   #endif
   return nsR;
}

//#define cpMod_BNU(pX,sizeX, pM,sizeM) cpDiv_BNU(NULL,NULL, (pX),(sizeX), (pM),(sizeM))
__INLINE cpSize cpMod_BNU(BNU_CHUNK_T* pX, cpSize nsX, BNU_CHUNK_T* pModulus, cpSize nsM)
{
   return cpDiv_BNU(NULL,NULL, pX,nsX, pModulus, nsM);
}

#endif /* _CP_BNU_ARITH_H */
