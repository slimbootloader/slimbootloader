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
#include "pcptool.h"

/* convert bitsize nbits into  the number of BNU_CHUNK_T */
#define BITS_BNU_IPP32U(nbits) (((nbits)+31)/32)

/*
// size of ModEngine context (Montgomery)
*/
IppStatus gsModEngineGetSize(int modulusBitSize, int numpe, int* pSize)
{
   int modLen  = BITS_BNU_CHUNK(modulusBitSize);
   int pelmLen = BITS_BNU_CHUNK(modulusBitSize);

   IPP_BADARG_RET(modulusBitSize<1, ippStsLengthErr);
   IPP_BADARG_RET(numpe<MOD_ENGINE_MIN_POOL_SIZE, ippStsLengthErr);

   /* allocates mimimal necessary to Montgomery based methods */
   *pSize = sizeof(gsModEngine)
           + modLen*sizeof(BNU_CHUNK_T)         /* modulus  */
           + modLen*sizeof(BNU_CHUNK_T)         /* mont_R   */
           + modLen*sizeof(BNU_CHUNK_T)         /* mont_R^2 */
           + pelmLen*sizeof(BNU_CHUNK_T)*numpe; /* buffers  */

   return ippStsNoErr;
}

/*
// montfomery factor k0 = -((modulus^-1 mod B) %B)
*/
BNU_CHUNK_T gsMontFactor(BNU_CHUNK_T m0)
{
   BNU_CHUNK_T y = 1;
   BNU_CHUNK_T x = 2;
   BNU_CHUNK_T mask = 2*x-1;

   int i;
   for(i=2; i<=BNU_CHUNK_BITS; i++, x<<=1) {
      BNU_CHUNK_T rH, rL;
      MUL_AB(rH, rL, m0, y);
      if( x < (rL & mask) ) /* x < ((m0*y) mod (2*x)) */
         y+=x;
      mask += mask + 1;
   }
   return (BNU_CHUNK_T)(0U - y);
}

/*
// initialization of the ModEngine context (Montgomery)
*/
IppStatus gsModEngineInit(gsModEngine* pME, const Ipp32u* pModulus, int modulusBitSize, int numpe, const gsModMethod* method)
{
   IPP_BADARG_RET(modulusBitSize<1, ippStsLengthErr);
   IPP_BADARG_RET((pModulus) && (pModulus[0] & 1) == 0, ippStsBadModulusErr);
   IPP_BADARG_RET(numpe<MOD_ENGINE_MIN_POOL_SIZE, ippStsLengthErr);

   {
      int pelmLen = BITS_BNU_CHUNK(modulusBitSize);
      int modLen   = BITS_BNU_CHUNK(modulusBitSize);
      int modLen32 = BITS_BNU_IPP32U(modulusBitSize);
      Ipp8u* ptr = (Ipp8u*)pME;

      /* clear whole context */
      PadBlock(0, pME, sizeof(gsModEngine));

      MOD_PARENT(pME)   = NULL;
      MOD_EXTDEG(pME)   = 1;
      MOD_BITSIZE(pME)  = modulusBitSize;
      MOD_LEN(pME)      = modLen;
      MOD_PELEN(pME)    = pelmLen;
      MOD_METHOD(pME)   = method;
      MOD_MODULUS(pME)  = (BNU_CHUNK_T*)(ptr += sizeof(gsModEngine));
      MOD_MNT_R(pME)    = (BNU_CHUNK_T*)(ptr += modLen*sizeof(BNU_CHUNK_T));
      MOD_MNT_R2(pME)   = (BNU_CHUNK_T*)(ptr += modLen*sizeof(BNU_CHUNK_T));
      MOD_POOL_BUF(pME) = (BNU_CHUNK_T*)(ptr += modLen*sizeof(BNU_CHUNK_T));
      MOD_MAXPOOL(pME)  = numpe;
      MOD_USEDPOOL(pME) = 0;

      if (pModulus) {
         /* store modulus */
         ZEXPAND_COPY_BNU((Ipp32u*)MOD_MODULUS(pME), modLen * (cpSize)(sizeof(BNU_CHUNK_T) / sizeof(Ipp32u)), pModulus, modLen32);

         /* montgomery factor */
         MOD_MNT_FACTOR(pME) = gsMontFactor(MOD_MODULUS(pME)[0]);

         /* montgomery identity (R) */
         ZEXPAND_BNU(MOD_MNT_R(pME), 0, modLen);
         MOD_MNT_R(pME)[modLen] = 1;
         cpMod_BNU(MOD_MNT_R(pME), modLen+1, MOD_MODULUS(pME), modLen);

         /* montgomery domain converter (RR) */
         ZEXPAND_BNU(MOD_MNT_R2(pME), 0, modLen);
         COPY_BNU(MOD_MNT_R2(pME)+modLen, MOD_MNT_R(pME), modLen);
         cpMod_BNU(MOD_MNT_R2(pME), 2*modLen, MOD_MODULUS(pME), modLen);
      }
   }

   return ippStsNoErr;
}

/*
// pool management methods
*/
#if 0
BNU_CHUNK_T* gsModPoolAlloc(gsModEngine* pME, int poolReq)
{
   BNU_CHUNK_T* pPool = MOD_BUFFER(pME, pME->poolLenUsed);

   if(pME->poolLenUsed + poolReq > pME->poolLen)
      pPool = NULL;
   else
      pME->poolLenUsed += poolReq;

   return pPool;
}
#endif
#if 0
void gsModPoolFree(gsModEngine* pME, int poolReq)
{
   if(pME->poolLenUsed < poolReq)
      poolReq = pME->poolLenUsed;
   pME->poolLenUsed -= poolReq;
}
#endif

BNU_CHUNK_T*   gsModGetPool(gsModEngine* pME)
{
   BNU_CHUNK_T*
   pPool = (pME->poolLenUsed >= pME->poolLen)? NULL : MOD_BUFFER(pME, pME->poolLenUsed);
   return pPool;
}

/*
// Pack/Unpack methods
*/

void gsPackModEngineCtx(const gsModEngine* pCtx, Ipp8u* pBuffer)
{
   gsModEngine* pAlignedBuffer = (gsModEngine*)pBuffer;

   /* max modulus length */
   int modSize = MOD_LEN(pCtx);
   /* size of context (bytes) without cube and pool buffers */
   int ctxSize = sizeof(gsModEngine)
                +sizeof(BNU_CHUNK_T)*(modSize*3);

   CopyBlock(pCtx, pAlignedBuffer, ctxSize);
   MOD_MODULUS(pAlignedBuffer) = (BNU_CHUNK_T*)((Ipp8u*)NULL + IPP_UINT_PTR(MOD_MODULUS(pCtx))-IPP_UINT_PTR(pCtx));
   MOD_MNT_R(pAlignedBuffer)   = (BNU_CHUNK_T*)((Ipp8u*)NULL + IPP_UINT_PTR(MOD_MNT_R(pCtx))-IPP_UINT_PTR(pCtx));
   MOD_MNT_R2(pAlignedBuffer)  = (BNU_CHUNK_T*)((Ipp8u*)NULL + IPP_UINT_PTR(MOD_MNT_R2(pCtx))-IPP_UINT_PTR(pCtx));
}

void gsUnpackModEngineCtx(const Ipp8u* pBuffer, gsModEngine* pCtx)
{
   gsModEngine* pAlignedBuffer = (gsModEngine*)pBuffer;

   /* max modulus length */
   int modSize = MOD_LEN(pAlignedBuffer);
   /* size of context (bytes) without cube and pool buffers */
   int ctxSize = sizeof(gsModEngine)
                +sizeof(BNU_CHUNK_T)*(modSize*3);

   CopyBlock(pAlignedBuffer, pCtx, ctxSize);
   MOD_MODULUS(pCtx)  = (BNU_CHUNK_T*)((Ipp8u*)pCtx + IPP_UINT_PTR(MOD_MODULUS(pAlignedBuffer)));
   MOD_MNT_R(pCtx)    = (BNU_CHUNK_T*)((Ipp8u*)pCtx + IPP_UINT_PTR(MOD_MNT_R(pAlignedBuffer)));
   MOD_MNT_R2(pCtx)   = (BNU_CHUNK_T*)((Ipp8u*)pCtx + IPP_UINT_PTR(MOD_MNT_R2(pAlignedBuffer)));
   MOD_POOL_BUF(pCtx) = MOD_MNT_R2(pCtx) + modSize;
}

