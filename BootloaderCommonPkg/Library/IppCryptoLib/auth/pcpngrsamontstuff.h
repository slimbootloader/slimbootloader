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
// 
//  Purpose:
//     Cryptography Primitive.
//     Internal Definitions and
//     Internal ng RSA Function Prototypes
// 
// 
*/

#if !defined(_CP_NG_RSA_MONT_STUFF_H)
#define _CP_NG_RSA_MONT_STUFF_H

#include "pcpbn.h"
#include "pcpmulbnukara.h"
#include "pcpmontgomery.h"

#define MOD_MONTGOMERY_TWO_STAGES_MUL

#define MOD_ENGINE_RSA_POOL_SIZE 2

/*
// whether Karatsuba algorithm in use
//
// note MUL and SQR operations could have different "threshold" bounndary
*/
__INLINE int gsIsKaratsubaApplicable(cpSize ns, cpSize threshold)
{
   #if !defined(_USE_KARATSUBA_)
   UNREFERENCED_PARAMETER(ns);
   UNREFERENCED_PARAMETER(threshold);
   return 0;
   #else
   return ns>=threshold;
   #endif
}

/*
// length (in BNU_CHUNK_T) of buffer for Karatsuba mul/sqr operations
*/
__INLINE cpSize gsKaratsubaBufferLen(cpSize ns)
{
   #if !defined(_USE_KARATSUBA_)
   UNREFERENCED_PARAMETER(ns);
   return 0;
   #else
   cpSize len= 0;
   while(ns>=CP_KARATSUBA_MUL_THRESHOLD) {
      ns = ns - ns/2;
      len += ns*2;
   }
   return len;
   #endif
}

/*
// Montgomery engine preparation (GetSize/init/Set)
*/
#define gsMontGetSize OWNAPI(gsMontGetSize)
void gsMontGetSize(IppsExpMethod method, int length, int* pSize);

/*
// optimal size of fixed window exponentiation
*/
__INLINE cpSize gsMontExp_WinSize(cpSize bitsize)
{
   #if defined(_USE_WINDOW_EXP_)
   // new computations
   return
      #if (_IPP !=_IPP_M5)     /*limited by 6 or 4 (LOG_CACHE_LINE_SIZE); we use it for windowing-exp imtigation */
         bitsize> 4096? 6 :    /* 4096- .. .  */
         bitsize> 2666? 5 :    /* 2666 - 4095 */
      #endif
         bitsize>  717? 4 :    /*  717 - 2665 */
         bitsize>  178? 3 :    /*  178 - 716  */
         bitsize>   41? 2 : 1; /*   41 - 177  */
   #else
   UNREFERENCED_PARAMETER(bitsize);
   return 1;
   #endif
}

/*
// Montgomery encoding/decoding
*/
__INLINE cpSize gsMontEnc_BNU(BNU_CHUNK_T* pR,
                        const BNU_CHUNK_T* pXreg, cpSize nsX,
                        const gsModEngine* pMont)
{
   cpSize nsM = MOD_LEN( pMont );
   ZEXPAND_COPY_BNU(pR, nsM, pXreg, nsX);
   MOD_METHOD( pMont )->encode(pR, pR, (gsModEngine*)pMont);
   return nsM;
}

__INLINE cpSize gsMontDec_BNU(BNU_CHUNK_T* pR,
                        const BNU_CHUNK_T* pXmont,
                              gsModEngine* pMont)
{
   cpSize nsM = MOD_LEN(pMont);
   MOD_METHOD( pMont )->decode(pR, pXmont, (gsModEngine*)pMont);
   return nsM;
}

__INLINE void gsMontEnc_BN(IppsBigNumState* pRbn,
                     const IppsBigNumState* pXbn,
                           gsModEngine* pMont)
{
   BNU_CHUNK_T* pR = BN_NUMBER(pRbn);
   cpSize nsM = MOD_LEN(pMont);

   gsMontEnc_BNU(pR, BN_NUMBER(pXbn), BN_SIZE(pXbn), pMont);

   FIX_BNU(pR, nsM);
   BN_SIZE(pRbn) = nsM;
   BN_SIGN(pRbn) = ippBigNumPOS;
}

/*
// binary montgomery exponentiation ("fast" version)
*/
#define gsMontExpBin_BNU OWNAPI(gsMontExpBin_BNU)
cpSize gsMontExpBin_BNU(BNU_CHUNK_T* dataY,
                  const BNU_CHUNK_T* dataX, cpSize nsX,
                  const BNU_CHUNK_T* dataE, cpSize nsE,
                        gsModEngine* pMont,
                        BNU_CHUNK_T* pBuffer);

/*
// fixed-size window montgomery exponentiation ("fast" version)
*/
#if defined(_USE_WINDOW_EXP_)
cpSize gsMontExpWin_BNU(BNU_CHUNK_T* pY,
                 const BNU_CHUNK_T* pX, cpSize nsX,
                 const BNU_CHUNK_T* dataE, cpSize nsE,
                       gsModEngine* pMont,
                       BNU_CHUNK_T* pBuffer);
#endif

/*
// binary montgomery exponentiation ("safe" version)
*/
__INLINE cpSize gsPrecompResourcelen(int n, cpSize nsM)
{
//   cpSize nsR = (((sizeof(BNU_CHUNK_T)*nsM*n + (CACHE_LINE_SIZE-1)))/CACHE_LINE_SIZE)  /* num of cashe lines */
//                *MASK_BNU_CHUNK(CACHE_LINE_SIZE*BYTESIZE);                             /* length of line in BNU_CHUNK_T */
   cpSize nsR = sizeof(BNU_CHUNK_T)*nsM*n + (CACHE_LINE_SIZE-1);
   nsR /=CACHE_LINE_SIZE;  /* num of cashe lines */
   nsR *= (CACHE_LINE_SIZE/sizeof(BNU_CHUNK_T));
   return nsR;
}

#define gsMontExpBin_BNU_sscm OWNAPI(gsMontExpBin_BNU_sscm)
cpSize gsMontExpBin_BNU_sscm(BNU_CHUNK_T* pY,
                       const BNU_CHUNK_T* pX, cpSize nsX,
                       const BNU_CHUNK_T* pE, cpSize nsE,
                             gsModEngine* pMont,
                             BNU_CHUNK_T* pBuffer);

/*
// fixed-size window montgomery exponentiation ("safe" version)
*/
#if defined(_USE_WINDOW_EXP_)
#define gsMontExpWin_BNU_sscm OWNAPI(gsMontExpWin_BNU_sscm)
cpSize gsMontExpWin_BNU_sscm(BNU_CHUNK_T* dataY,
                       const BNU_CHUNK_T* dataX, cpSize nsX,
                       const BNU_CHUNK_T* dataE, cpSize nsE,
                             gsModEngine* pMont,
                             BNU_CHUNK_T* pBuffer);

#endif


#define gsPubBuffer_gpr OWNAPI(gsPubBuffer_gpr)
cpSize  gsPubBuffer_gpr(int modulusBits);

#define       gsMethod_RSA_gpr OWNAPI(gsMethod_RSA_gpr)
gsMethod_RSA* gsMethod_RSA_gpr(void);

#endif /* _CP_NG_RSA_MONT_STUFF_H */
