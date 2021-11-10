/*******************************************************************************
* Copyright 2002-2020 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/*
//  Purpose:
//     Intel(R) Integrated Performance Primitives. Cryptography Primitives.
//     Internal BNU32 arithmetic.
//
//  Contents:
//
//     cpAdd_BNU32()
//     cpSub_BNU32()
//
//     cpMulDgt_BNU32()
//     cpSubMulDgt_BNU32()
//
//     cpDiv_BNU32()
//
//
*/

#include "owncp.h"
#include "pcpbnumisc.h"
#include "pcpbnu32misc.h"
#include "pcpbnu32arith.h"

/*
// BNU32 addition
*/
Ipp32u cpAdd_BNU32(Ipp32u* pR, const Ipp32u* pA, const Ipp32u* pB, cpSize ns)
{
   Ipp32u carry = 0;
   cpSize i;
   for(i=0; i<ns; i++) {
      Ipp64u t = (Ipp64u)carry +pA[i] + pB[i];
      pR[i] = IPP_LODWORD(t);
      carry = IPP_HIDWORD(t);
   }
   return carry;
}

/*
// BNU32 subtraction
*/
Ipp32u cpSub_BNU32(Ipp32u* pR, const Ipp32u* pA, const Ipp32u* pB, cpSize ns)
{
   Ipp32u borrow = 0;
   cpSize i;
   for(i=0; i<ns; i++) {
      Ipp64u t = (Ipp64u)(pA[i]) - pB[i] - borrow;
      pR[i] = IPP_LODWORD(t);
      borrow = 0-IPP_HIDWORD(t);
   }
   return borrow;
}

/*
// BNU32 increment
*/
Ipp32u cpInc_BNU32(Ipp32u* pR, const Ipp32u* pA, cpSize ns, Ipp32u v)
{
   Ipp32u carry = v;
   cpSize i;
   for(i=0; i<ns && carry; i++) {
      Ipp64u t = (Ipp64u)carry +pA[i];
      pR[i] = IPP_LODWORD(t);
      carry = IPP_HIDWORD(t);
   }
   return carry;
}

/*
// BNU32 decrement
*/
Ipp32u cpDec_BNU32(Ipp32u* pR, const Ipp32u* pA, cpSize ns, Ipp32u v)
{
   Ipp32u borrow = v;
   int n;
   for(n=0; n<ns; n++) {
      Ipp64u t = (Ipp64u)(pA[n]) - (Ipp64u)borrow;
      pR[n] = IPP_LODWORD(t);
      borrow = IPP_HIDWORD(t)>>(32-1);
   }
   return borrow;
}

/*
// BNU32 mul_by_digit
*/
Ipp32u cpMulDgt_BNU32(Ipp32u* pR, const Ipp32u* pA, cpSize nsA, Ipp32u val)
{
   Ipp32u carry = 0;
   cpSize i;
   for(i=0; i<nsA; i++) {
#ifdef _SLIMBOOT_OPT
      Ipp64u t = (Ipp64u)MultU64x32(pA[i], val) + carry;
#else
      Ipp64u t = (Ipp64u)val * (Ipp64u)pA[i] + carry;
#endif
      pR[i] = IPP_LODWORD(t);
      carry = IPP_HIDWORD(t);
    }
    return carry;
}

/*
// BNU32 mul_by_digit_subtract
*/
#if !((_IPP32E==_IPP32E_M7) || \
      (_IPP32E==_IPP32E_U8) || \
      (_IPP32E==_IPP32E_Y8) || \
      (_IPP32E>=_IPP32E_E9) || \
      (_IPP32E==_IPP32E_N8))
Ipp32u cpSubMulDgt_BNU32(Ipp32u* pR, const Ipp32u* pA, cpSize nsA, Ipp32u val)
{
   Ipp32u carry = 0;
   for(; nsA>0; nsA--) {
#ifdef _SLIMBOOT_OPT
      Ipp64u r = (Ipp64u)*pR - MultU64x32(*pA++ , val) - carry;
#else
      Ipp64u r = (Ipp64u)*pR - (Ipp64u)(*pA++) * val - carry;
#endif
      *pR++  = IPP_LODWORD(r);
      carry  = 0-IPP_HIDWORD(r);
   }
   return carry;
}
#endif

/*
// BNU32 division
*/
#if !((_IPP32E==_IPP32E_M7) || \
      (_IPP32E==_IPP32E_U8) || \
      (_IPP32E==_IPP32E_Y8) || \
      (_IPP32E>=_IPP32E_E9) || \
      (_IPP32E==_IPP32E_N8))
int cpDiv_BNU32(Ipp32u* pQ, cpSize* sizeQ,
                 Ipp32u* pX, cpSize sizeX,
                 Ipp32u* pY, cpSize sizeY)
{
   FIX_BNU(pY,sizeY);
   FIX_BNU(pX,sizeX);

   /* special case */
   if(sizeX < sizeY) {

      if(pQ) {
         pQ[0] = 0;
         *sizeQ = 1;
      }

      return sizeX;
   }

   /* special case */
   if(1 == sizeY) {
      int i;
      Ipp32u r = 0;
      for(i=(int)sizeX-1; i>=0; i--) {
         Ipp64u tmp = IPP_MAKEDWORD(pX[i],r);
#ifdef _SLIMBOOT_OPT
         Ipp32u q = IPP_LODWORD(DivU64x32 (tmp , pY[0]));
         r = IPP_LODWORD(tmp - MultU64x32(pY[0], q));
#else
         Ipp32u q = IPP_LODWORD(tmp / pY[0]);
         r = IPP_LODWORD(tmp - q*pY[0]);
#endif
         if(pQ) pQ[i] = q;
      }

      pX[0] = r;

      if(pQ) {
         FIX_BNU(pQ,sizeX);
         *sizeQ = sizeX;
      }

      return 1;
   }


   /* common case */
   {
      cpSize qs = sizeX-sizeY+1;

      cpSize nlz = cpNLZ_BNU32(pY[sizeY-1]);

      /* normalization */
      pX[sizeX] = 0;
      if(nlz) {
         cpSize ni;

         pX[sizeX] = pX[sizeX-1] >> (32-nlz);
         for(ni=sizeX-1; ni>0; ni--)
            pX[ni] = (pX[ni]<<nlz) | (pX[ni-1]>>(32-nlz));
         pX[0] <<= nlz;

         for(ni=sizeY-1; ni>0; ni--)
            pY[ni] = (pY[ni]<<nlz) | (pY[ni-1]>>(32-nlz));
         pY[0] <<= nlz;
      }

      /*
      // division
      */
      {
         Ipp32u yHi = pY[sizeY-1];

         int i;
         for(i=(int)qs-1; i>=0; i--) {
            Ipp32u extend;

            /* estimate digit of quotient */
            Ipp64u tmp = IPP_MAKEDWORD(pX[i+sizeY-1], pX[i+sizeY]);
#ifdef _SLIMBOOT_OPT
            Ipp64u q = DivU64x32 (tmp, yHi);
            Ipp64u r = tmp - MultU64x32 (q , yHi);
#else
            Ipp64u q = tmp / yHi;
            Ipp64u r = tmp - q*yHi;
#endif


            /* tune estimation above */
            //for(; (q>=CONST_64(0x100000000)) || (Ipp64u)q*pY[sizeY-2] > MAKEDWORD(pX[i+sizeY-2],r); ) {
#ifdef _SLIMBOOT_OPT
            for(; IPP_HIDWORD(q) || (Ipp64u)MultU64x32 (q, pY[sizeY-2]) > IPP_MAKEDWORD(pX[i+sizeY-2],r); ) {
#else
            for(; IPP_HIDWORD(q) || (Ipp64u)q*pY[sizeY-2] > IPP_MAKEDWORD(pX[i+sizeY-2],r); ) {
#endif
               q -= 1;
               r += yHi;
               if( IPP_HIDWORD(r) )
                  break;
            }

            /* multiply and subtract */
            extend = cpSubMulDgt_BNU32(pX+i, pY, sizeY, (Ipp32u)q);
            extend = (pX[i+sizeY] -= extend);

            if(extend) { /* subtracted too much */
               q -= 1;
               extend = cpAdd_BNU32(pX+i, pY, pX+i, sizeY);
               pX[i+sizeY] += extend;
            }

            /* store quotation digit */
            if(pQ) pQ[i] = IPP_LODWORD(q);
         }
      }

      /* de-normalization */
      if(nlz) {
         cpSize ni;
         for(ni=0; ni<sizeX; ni++)
            pX[ni] = (pX[ni]>>nlz) | (pX[ni+1]<<(32-nlz));
         for(ni=0; ni<sizeY-1; ni++)
            pY[ni] = (pY[ni]>>nlz) | (pY[ni+1]<<(32-nlz));
         pY[sizeY-1] >>= nlz;
      }

      FIX_BNU(pX,sizeX);

      if(pQ) {
         FIX_BNU(pQ,qs);
         *sizeQ = qs;
      }

      return sizeX;
   }
}
#endif

#define FE_MUL(R,A,B,LEN) { \
   int aidx, bidx; \
   \
   for(aidx=0; aidx<(LEN); aidx++) (R)[aidx] = 0; \
   \
   for(bidx=0; bidx<(LEN); bidx++) { \
      Ipp64u b = (B)[bidx]; \
      Ipp32u c = 0; \
      for(aidx=0; aidx<(LEN); aidx++) { \
         Ipp64u t = (R)[bidx+aidx] + (A)[aidx] * b + c; \
         (R)[bidx+aidx] = IPP_LODWORD(t); \
         c = IPP_HIDWORD(t); \
      } \
      (R)[bidx+aidx] = c; \
   } \
}
