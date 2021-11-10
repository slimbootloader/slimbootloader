/*******************************************************************************
* Copyright 2017-2020 Intel Corporation
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

#include "owndefs.h"
#include "owncp.h"
#include "pcpbnumisc.h"
#include "pcpbnuarith.h"

#include "gsmodstuff.h"
#include "gsmodmethod.h"
#include "pcpmontred.h"

#ifndef _SLIMBOOT_OPT
//gres: temporary excluded: #include <assert.h>

/* r = (a+m) mod m */
/*
 * Requirements:
 *   Length of pr data buffer:   modLen
 *   Length of pa data buffer:   modLen
 *   Length of pb data buffer:   modLen
 *   Memory size from the pool:  modLen * sizeof(BNU_CHUNK_T)
 */
static BNU_CHUNK_T* gs_mont_add(BNU_CHUNK_T* pr, const BNU_CHUNK_T* pa, const BNU_CHUNK_T* pb, gsModEngine* pME)
{
   const BNU_CHUNK_T* pm = MOD_MODULUS(pME);
   int mLen = MOD_LEN(pME);

   const int polLength  = 1;
   BNU_CHUNK_T* pBuffer = gsModPoolAlloc(pME, polLength);
   //gres: temporary excluded: assert(NULL!=pBuffer);
   {
      BNU_CHUNK_T extension = cpAdd_BNU(pr, pa, pb, mLen);
      extension -= cpSub_BNU(pBuffer, pr, pm, mLen);
      cpMaskMove_gs(pr, pBuffer, mLen, cpIsZero(extension));
   }
   gsModPoolFree(pME, polLength);
   return pr;
}

/* r = (a-m) mod m */
/*
 * Requirements:
 *   Length of pr data buffer:   modLen
 *   Length of pa data buffer:   modLen
 *   Length of pb data buffer:   modLen
 *   Memory size from the pool:  modLen * sizeof(BNU_CHUNK_T)
 */
static BNU_CHUNK_T* gs_mont_sub(BNU_CHUNK_T* pr, const BNU_CHUNK_T* pa, const BNU_CHUNK_T* pb, gsModEngine* pME)
{
   const BNU_CHUNK_T* pm = MOD_MODULUS(pME);
   int mLen = MOD_LEN(pME);

   const int polLength  = 1;
   BNU_CHUNK_T* pBuffer = gsModPoolAlloc(pME, polLength);
   //gres: temporary excluded: assert(NULL!=pBuffer);
   {
      BNU_CHUNK_T extension = cpSub_BNU(pr, pa, pb, mLen);
      cpAdd_BNU(pBuffer, pr, pm, mLen);
      cpMaskMove_gs(pr, pBuffer, mLen, cpIsNonZero(extension));
   }
   gsModPoolFree(pME, polLength);
   return pr;
}

/*
 * Requirements:
 *   Length of pr data buffer:   modLen
 *   Length of pa data buffer:   modLen
 *   Memory size from the pool:  modLen * sizeof(BNU_CHUNK_T)
 */
static BNU_CHUNK_T* gs_mont_neg(BNU_CHUNK_T* pr, const BNU_CHUNK_T* pa, gsModEngine* pME)
{
   const BNU_CHUNK_T* pm = MOD_MODULUS(pME);
   int mLen = MOD_LEN(pME);

   const int polLength  = 1;
   BNU_CHUNK_T* pBuffer = gsModPoolAlloc(pME, polLength);
   //gres: temporary excluded: assert(NULL!=pBuffer);
   {
      BNU_CHUNK_T extension = cpSub_BNU(pr, pm, pa, mLen);
      extension -= cpSub_BNU(pBuffer, pr, pm, mLen);
      cpMaskMove_gs(pr, pBuffer, mLen, cpIsZero(extension));
   }
   gsModPoolFree(pME, polLength);
   return pr;
}

/*
 * Requirements:
 *   Length of pr data buffer:   modLen
 *   Length of pa data buffer:   modLen
 *   Memory size from the pool:  modLen * sizeof(BNU_CHUNK_T)
 */
static BNU_CHUNK_T* gs_mont_div2(BNU_CHUNK_T* pr, const BNU_CHUNK_T* pa, gsModEngine* pME)
{
   const BNU_CHUNK_T* pm = MOD_MODULUS(pME);
   int mLen = MOD_LEN(pME);

   const int polLength  = 1;
   BNU_CHUNK_T* pBuffer = gsModPoolAlloc(pME, polLength);
   //gres: temporary excluded: assert(NULL!=pBuffer);
   {
      cpSize i;
      BNU_CHUNK_T mask = 0 - (pa[0]&1);
      for(i=0; i<mLen; i++) pBuffer[i] = pm[i] & mask;

      pBuffer[mLen] = cpAdd_BNU(pBuffer, pBuffer, pa, mLen);
      cpLSR_BNU(pBuffer, pBuffer, mLen+1, 1);

      for(i=0; i<mLen; i++) pr[i] = pBuffer[i];
   }
   gsModPoolFree(pME, polLength);
   return pr;
}

/*
 * Requirements:
 *   Length of pr data buffer:   modLen
 *   Length of pa data buffer:   modLen
 *   Memory size from the pool:  modLen * sizeof(BNU_CHUNK_T)
 */
static BNU_CHUNK_T* gs_mont_mul2(BNU_CHUNK_T* pr, const BNU_CHUNK_T* pa, gsModEngine* pME)
{
   return gs_mont_add(pr, pa, pa, pME);
}

/*
 * Requirements:
 *   Length of pr data buffer:   modLen
 *   Length of pa data buffer:   modLen
 *   Memory size from the pool:  modLen * sizeof(BNU_CHUNK_T) * 2
 */
static BNU_CHUNK_T* gs_mont_mul3(BNU_CHUNK_T* pr, const BNU_CHUNK_T* pa, gsModEngine* pME)
{
   const int polLength  = 1;
   BNU_CHUNK_T* pBuffer = gsModPoolAlloc(pME, polLength);
   //gres: temporary excluded: assert(NULL!=pBuffer);

   gs_mont_add(pBuffer, pa, pa, pME);
   gs_mont_add(pr, pa, pBuffer, pME);

   gsModPoolFree(pME, polLength);
   return pr;
}

#if !((_IPP==_IPP_W7) || \
      (_IPP==_IPP_T7) || \
      (_IPP==_IPP_V8) || \
      (_IPP==_IPP_P8) || \
      (_IPP>=_IPP_G9) || \
      (_IPP==_IPP_S8) || \
      (_IPP32E==_IPP32E_M7) || \
      (_IPP32E==_IPP32E_U8) || \
      (_IPP32E==_IPP32E_Y8) || \
      (_IPP32E>=_IPP32E_E9) || \
      (_IPP32E==_IPP32E_N8)) || \
      defined(_USE_C_cpMontRedAdc_BNU_)

/*
 * Requirements:
 *   Length of pr data buffer:   modLen
 *   Length of prod data buffer: modLen * 2
 *   Memory size from the pool:  n/a
 */
static BNU_CHUNK_T* gs_mont_red(BNU_CHUNK_T* pr, BNU_CHUNK_T* prod, gsModEngine* pME)
{
   const BNU_CHUNK_T* pm = MOD_MODULUS(pME);
   BNU_CHUNK_T k0 = MOD_MNT_FACTOR(pME);
   int mLen = MOD_LEN(pME);

   /* mont mul */
   BNU_CHUNK_T carry = 0;
   int i, j;

   for(i=0; i<mLen; i++, prod++) {
      BNU_CHUNK_T muL, muH, ex_mu;
      BNU_CHUNK_T t;

      /* u = prod[0]*k0 mod B */
      BNU_CHUNK_T u = prod[0] * k0;

      /* (ex_mu, t) = prod[0] + m[0]*u (note t ==0) */
      MUL_AB(muH, muL, pm[0], u);
      ADD_AB(ex_mu, t, prod[0], muL);
      ex_mu += muH;

      for(j=1; j<mLen; j++) {
         BNU_CHUNK_T c;
         MUL_AB(muH, muL, pm[j], u);         /* (H,L) = m[j]*u */
         ADD_AB(ex_mu, t, prod[j], ex_mu);   /* carry in ex_mu,t */
         ADD_AB(c, prod[j], t, muL);         /* carry in c */
         ex_mu += muH+c;                     /* accumulates both carrys above */
      }
      ADD_ABC(carry, prod[mLen], prod[mLen], ex_mu, carry);
   }

   {
      carry -= cpSub_BNU(pr, prod, pm, mLen);
      cpMaskMove_gs(pr, prod, mLen, cpIsNonZero(carry));
      return pr;
   }
}

#else

/*
 * Requirements:
 *   Length of pr data buffer:   modLen
 *   Length of prod data buffer: modLen * 2
 *   Memory size from the pool:  n/a
 */
static BNU_CHUNK_T* gs_mont_red(BNU_CHUNK_T* pr, BNU_CHUNK_T* prod, gsModEngine* pME)
{
   const BNU_CHUNK_T* pm = MOD_MODULUS(pME);
   BNU_CHUNK_T k0 = MOD_MNT_FACTOR(pME);
   int mLen = MOD_LEN(pME);

   cpMontRed_BNU_opt(pr, prod, pm, mLen, k0);

   return pr;
}
#endif
#endif

#if !((_IPP==_IPP_W7) || \
      (_IPP==_IPP_T7) || \
      (_IPP==_IPP_V8) || \
      (_IPP==_IPP_P8) || \
      (_IPP>=_IPP_G9) || \
      (_IPP==_IPP_S8) || \
      (_IPP32E==_IPP32E_M7) || \
      (_IPP32E==_IPP32E_U8) || \
      (_IPP32E==_IPP32E_Y8) || \
      (_IPP32E>=_IPP32E_E9) || \
      (_IPP32E==_IPP32E_N8))

/*
 * Requirements:
 *   Length of pr data buffer:   modLen
 *   Length of pa data buffer:   modLen
 *   Length of pb data buffer:   modLen
 *   Memory size from the pool:  modLen * sizeof(BNU_CHUNK_T) * 2
 */
static BNU_CHUNK_T* gs_mont_mul(BNU_CHUNK_T* pr, const BNU_CHUNK_T* pa, const BNU_CHUNK_T* pb, gsModEngine* pME)
{
   const BNU_CHUNK_T* pm = MOD_MODULUS(pME);
   BNU_CHUNK_T m0 = MOD_MNT_FACTOR(pME);
   int mLen = MOD_LEN(pME);

   const int polLength  = 1;
   BNU_CHUNK_T* pBuffer = gsModPoolAlloc(pME, polLength);
   //gres: temporary excluded: assert(NULL!=pBuffer);

   if (pBuffer != NULL) {
      BNU_CHUNK_T carry = 0;
      int i, j;

      /* clear buffer */
      ZEXPAND_BNU (pBuffer, 0, mLen);

      /* mont mul */
      for(i=0; i<mLen; i++) {
         BNU_CHUNK_T b = pb[i];

         BNU_CHUNK_T ex_ab = 0;
         BNU_CHUNK_T ex_mu = 0;

         BNU_CHUNK_T u;
         BNU_CHUNK_T abL, abH;
         BNU_CHUNK_T muL, muH;

         // (ex_ab, abL) = T = buffer[0] + a[0]*b
         MUL_AB(abH, abL, pa[0], b);
         ADD_ABC(ex_ab, abL, pBuffer[0], abL, ex_ab);
         ex_ab += abH;

         // u = t*m0 mod B
         u = abL * m0;

         // (ex_mu, abL) = T + m[0]*u (note abL ==0)
         MUL_AB(muH, muL, pm[0], u);
         ADD_ABC(ex_mu, abL, abL, muL, ex_mu);
         ex_mu += muH;

         for(j=1; j<mLen; j++) {
            MUL_AB(abH, abL, pa[j], b);
            ADD_ABC(ex_ab, abL, pBuffer[j], abL, ex_ab);
            ex_ab += abH;

            MUL_AB(muH, muL, pm[j], u);
            ADD_ABC(ex_mu, abL, abL, muL, ex_mu);
            ex_mu += muH;

            pBuffer[j-1] = abL;
         }
         ADD_ABC(carry, pBuffer[mLen-1], ex_ab, ex_mu, carry);
      }

      carry -= cpSub_BNU(pr, pBuffer, pm, mLen);
      cpMaskMove_gs(pr, pBuffer, mLen, cpIsNonZero(carry));
   }

   gsModPoolFree(pME, polLength);
   return pr;
}

#else
/*
 * Requirements:
 *   Length of pr data buffer:   modLen
 *   Length of pa data buffer:   modLen
 *   Length of pb data buffer:   modLen
 *   Memory size from the pool:  modLen * sizeof(BNU_CHUNK_T) * 2
 */
static BNU_CHUNK_T* gs_mont_mul(BNU_CHUNK_T* pr, const BNU_CHUNK_T* pa, const BNU_CHUNK_T* pb, gsModEngine* pME)
{
   const BNU_CHUNK_T* pm = MOD_MODULUS(pME);
   BNU_CHUNK_T m0 = MOD_MNT_FACTOR(pME);
   int mLen = MOD_LEN(pME);

   const int polLength  = 2;
   BNU_CHUNK_T* pProduct = gsModPoolAlloc(pME, polLength);
   //gres: temporary excluded: assert(NULL!=pProduct);

   cpMul_BNU(pProduct, pa,mLen, pb,mLen, 0);
   cpMontRed_BNU_opt(pr, pProduct, pm, mLen, m0);

   gsModPoolFree(pME, polLength);
   return pr;
}
#endif

/*
 * Requirements:
 *   Length of pr data buffer:   modLen
 *   Length of pa data buffer:   modLen
 *   Memory size from the pool:  modLen * sizeof(BNU_CHUNK_T)
 */
static BNU_CHUNK_T* gs_mont_sqr(BNU_CHUNK_T* pr, const BNU_CHUNK_T* pa, gsModEngine* pME)
{
   return gs_mont_mul(pr, pa, pa, pME);
}

/*
 * Requirements:
 *   Length of pr data buffer:   modLen
 *   Length of pa data buffer:   modLen
 *   Memory size from the pool:  modLen * sizeof(BNU_CHUNK_T)
 */
static BNU_CHUNK_T* gs_mont_encode(BNU_CHUNK_T* pr, const BNU_CHUNK_T* pa, gsModEngine* pME)
{
   return gs_mont_mul(pr, pa, MOD_MNT_R2(pME), pME);
}

#if !((_IPP==_IPP_W7) || \
      (_IPP==_IPP_T7) || \
      (_IPP==_IPP_V8) || \
      (_IPP==_IPP_P8) || \
      (_IPP>=_IPP_G9) || \
      (_IPP==_IPP_S8) || \
      (_IPP32E==_IPP32E_M7) || \
      (_IPP32E==_IPP32E_U8) || \
      (_IPP32E==_IPP32E_Y8) || \
      (_IPP32E>=_IPP32E_E9) || \
      (_IPP32E==_IPP32E_N8))

/*
 * Requirements:
 *   Length of pr data buffer:   modLen
 *   Length of pa data buffer:   modLen
 *   Memory size from the pool:  modLen * sizeof(BNU_CHUNK_T) * 2
 */
static BNU_CHUNK_T* gs_mont_decode(BNU_CHUNK_T* pr, const BNU_CHUNK_T* pa, gsModEngine* pME)
{
   int mLen = MOD_LEN(pME);

   const int polLength  = 1;
   BNU_CHUNK_T* t = gsModPoolAlloc(pME, polLength);
   //gres: temporary excluded: assert(NULL!=t);

   if (t != NULL) {
      ZEXPAND_BNU(t, 0, mLen);
      t[0] = 1;
      gs_mont_mul(pr, pa, t, pME);
      gsModPoolFree(pME, polLength);
   }

   return pr;
}

#else

/*
 * Requirements:
 *   Length of pr data buffer:   modLen
 *   Length of pa data buffer:   modLen
 *   Memory size from the pool:  modLen * sizeof(BNU_CHUNK_T)
 */
static BNU_CHUNK_T* gs_mont_decode(BNU_CHUNK_T* pr, const BNU_CHUNK_T* pa, gsModEngine* pME)
{
   int mLen = MOD_LEN(pME);

   const int polLength  = 2;
   BNU_CHUNK_T* pProduct = gsModPoolAlloc(pME, polLength);
   //gres: temporary excluded: assert(NULL!=pProduct);

   ZEXPAND_COPY_BNU(pProduct, 2*mLen, pa, mLen);
   gs_mont_red(pr, pProduct, pME);

   gsModPoolFree(pME, polLength);
   return pr;
}

#endif

#ifndef _SLIMBOOT_OPT
/**** methods ****/
gsModMethod* gsModArith(void)
{
   static gsModMethod m = {
      gs_mont_encode,
      gs_mont_decode,
      gs_mont_mul,
      gs_mont_sqr,
      gs_mont_red,
      gs_mont_add,
      gs_mont_sub,
      gs_mont_neg,
      gs_mont_div2,
      gs_mont_mul2,
      gs_mont_mul3,
   };
   return &m;
}
#endif

gsModMethod* gsModArithRSA(void)
{
   static gsModMethod m = {
      gs_mont_encode,
      gs_mont_decode,
      gs_mont_mul,
      gs_mont_sqr,
#ifdef _SLIMBOOT_OPT
      NULL,
#else
      gs_mont_red,
#endif
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
   };
   return &m;
}

#ifndef _SLIMBOOT_OPT
gsModMethod* gsModArithMont(void)
{
   static gsModMethod m = {
      gs_mont_encode,
      NULL,
      gs_mont_mul,
      gs_mont_sqr,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
   };
   return &m;
}

gsModMethod* gsModArithDLP(void)
{
   static gsModMethod m = {
      gs_mont_encode,
      gs_mont_decode,
      gs_mont_mul,
      gs_mont_sqr,
      gs_mont_red,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
   };
   return &m;
}

gsModMethod* gsArithGFp(void)
{
   static gsModMethod m = {
      gs_mont_encode,
      gs_mont_decode,
      gs_mont_mul,
      gs_mont_sqr,
      NULL,//gs_mont_red,
      gs_mont_add,
      gs_mont_sub,
      gs_mont_neg,
      gs_mont_div2,
      gs_mont_mul2,
      gs_mont_mul3,
   };
   return &m;
}
#endif
