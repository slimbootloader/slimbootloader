/*******************************************************************************
* Copyright 2018-2020 Intel Corporation
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
//
//  Purpose:
//     Cryptography Primitive.
//     RSA Internal Functions
//
//
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcpscramble.h"
#include "pcpngrsa.h"
#include "pcpngrsamontstuff.h"


/*
// Montgomery engine preparation (GetSize/init/Set)
*/
void rsaMontExpGetSize(int maxLen32, int* pSize)
{
    int size = 0;
    int maxBitSize = maxLen32 << 5;
    gsModEngineGetSize(maxBitSize, MOD_ENGINE_RSA_POOL_SIZE, &size);

    *pSize = size + MONT_ALIGNMENT;
}


/*
// "fast" binary montgomery exponentiation
//
// scratch buffer structure:
//    dataT[nsM]     copy of base (in case of inplace operation)
//    product[nsM*2]
*/
cpSize gsMontExpBin_BNU(BNU_CHUNK_T* dataY,
                  const BNU_CHUNK_T* dataX, cpSize nsX,
                  const BNU_CHUNK_T* dataE, cpSize nsE,
                        gsModEngine* pMont,
                        BNU_CHUNK_T* pBuffer)
{
   cpSize nsM = MOD_LEN(pMont);

   /*
   // test for special cases:
   //    x^0 = 1
   //    0^e = 0
   */
   if( cpEqu_BNU_CHUNK(dataE, nsE, 0) ) {
      COPY_BNU(dataY, MOD_MNT_R(pMont), nsM);
   }
   else if( cpEqu_BNU_CHUNK(dataX, nsX, 0) ) {
      ZEXPAND_BNU(dataY, 0, nsM);
   }

   /* general case */
   else {
      /* allocate buffers */
      BNU_CHUNK_T* dataT = pBuffer;

      /* copy and expand base to the modulus length */
      ZEXPAND_COPY_BNU(dataT, nsM, dataX, nsX);
      /* convert base to Montgomery domain */
      MOD_METHOD( pMont )->encode(dataT, dataT, (gsModEngine*)pMont);
      /* and copy */
      COPY_BNU(dataY, dataT, nsM);

      FIX_BNU(dataE, nsE);

      /* execute most significant part pE */
      {
         BNU_CHUNK_T eValue = dataE[nsE-1];
         int n = cpNLZ_BNU(eValue)+1;

         eValue <<= n;
         for(; n<BNU_CHUNK_BITS; n++, eValue<<=1) {
            /* squaring R = R*R mod Modulus */
            MOD_METHOD( pMont )->sqr(dataY, dataY, pMont);
            /* and multiply R = R*X mod Modulus */
            if(eValue & ((BNU_CHUNK_T)1<<(BNU_CHUNK_BITS-1)))
               MOD_METHOD( pMont )->mul(dataY, dataY, dataT, pMont);
         }

         /* execute rest bits of E */
         for(--nsE; nsE>0; nsE--) {
            eValue = dataE[nsE-1];

            for(n=0; n<BNU_CHUNK_BITS; n++, eValue<<=1) {
               /* squaring: R = R*R mod Modulus */
               MOD_METHOD( pMont )->sqr(dataY, dataY, pMont);

               if(eValue & ((BNU_CHUNK_T)1<<(BNU_CHUNK_BITS-1)))
                  MOD_METHOD( pMont )->mul(dataY, dataY, dataT, pMont);
            }
         }
      }

      /* convert result back to regular domain */
      MOD_METHOD( pMont )->decode(dataY, dataY, pMont);
   }

   return nsM;
}

/*
// "safe" binary montgomery exponentiation
//
// scratch buffer structure:
//    sscm[nsM]
//    dataT[nsM]
//    product[nsM*2]
*/
cpSize gsMontExpBin_BNU_sscm(BNU_CHUNK_T* dataY,
                  const BNU_CHUNK_T* dataX, cpSize nsX,
                  const BNU_CHUNK_T* dataE, cpSize nsE,
                        gsModEngine* pMont,
                        BNU_CHUNK_T* pBuffer)
{
   cpSize nsM = MOD_LEN(pMont);

   /*
   // test for special cases:
   //    x^0 = 1
   //    0^e = 0
   */
   if( cpEqu_BNU_CHUNK(dataE, nsE, 0) ) {
      COPY_BNU(dataY, MOD_MNT_R(pMont), nsM);
   }
      else if( cpEqu_BNU_CHUNK(dataX, nsX, 0) ) {
      ZEXPAND_BNU(dataY, 0, nsM);
   }

   /* general case */
   else {

      /* allocate buffers */
      //BNU_CHUNK_T* sscmBuffer = pBuffer;
      //BNU_CHUNK_T* dataT = sscmBuffer+nsM;
      BNU_CHUNK_T* dataT = pBuffer;
      BNU_CHUNK_T* sscmB = dataT + nsM;

      cpSize i;
      BNU_CHUNK_T mask_pattern;

      /* execute most significant part pE */
      BNU_CHUNK_T eValue = dataE[nsE-1];
      int j = BNU_CHUNK_BITS - cpNLZ_BNU(eValue)-1;

      int back_step = 0;

      /* copy and expand base to the modulus length */
      ZEXPAND_COPY_BNU(dataT, nsM, dataX, nsX);
      /* convert base to Montgomery domain */
      MOD_METHOD( pMont )->encode(dataT, dataT, (gsModEngine*)pMont);
      /* and copy */
      COPY_BNU(dataY, dataT, nsM);

      for(j-=1; j>=0; j--) {
         mask_pattern = (BNU_CHUNK_T)(back_step-1);

         /* safeBuffer = (Y[] and mask_pattern) or (X[] and ~mask_pattern) */
         for(i=0; i<nsM; i++)
            sscmB[i] = (dataY[i] & mask_pattern) | (dataT[i] & ~mask_pattern);

         /* squaring/multiplication: R = R*T mod Modulus */
         MOD_METHOD( pMont )->mul(dataY, dataY, sscmB, pMont);

         /* update back_step and j */
         back_step = ((eValue>>j) & 0x1) & (back_step^1);
         j += back_step;
      }

      /* execute rest bits of E */
      for(--nsE; nsE>0; nsE--) {
         eValue = dataE[nsE-1];

         for(j=BNU_CHUNK_BITS-1; j>=0; j--) {
            mask_pattern = (BNU_CHUNK_T)(back_step-1);

            /* safeBuffer = (Y[] and mask_pattern) or (X[] and ~mask_pattern) */
            for(i=0; i<nsM; i++)
               sscmB[i] = (dataY[i] & mask_pattern) | (dataT[i] & ~mask_pattern);

            /* squaring/multiplication: R = R*T mod Modulus */
            MOD_METHOD( pMont )->mul(dataY, dataY, sscmB, pMont);

            /* update back_step and j */
            back_step = ((eValue>>j) & 0x1) & (back_step^1);
            j += back_step;
         }
      }

      /* convert result back to regular domain */
      MOD_METHOD( pMont )->decode(dataY, dataY, pMont);
   }

   return nsM;
}


#if defined(_USE_WINDOW_EXP_)
//#define GS_DEBUG  // test buffer alignment
#if defined(GS_DEBUG)
#include <stdio.h>
#endif
/*
// "safe" fixed-size window montgomery exponentiation
//
// scratch buffer structure:
//    dataT[nsM]
//    dataE[nsM+1]
//    product[nsM*2]
//    precomutation resource[(1<<w)nsM]
*/
cpSize gsMontExpWin_BNU_sscm(BNU_CHUNK_T* dataY,
                       const BNU_CHUNK_T* dataX, cpSize nsX,
                       const BNU_CHUNK_T* dataExp, cpSize nsE,
                             gsModEngine* pMont,
                             BNU_CHUNK_T* pBuffer)
{
   cpSize nsM = MOD_LEN(pMont);

   /*
   // test for special cases:
   //    x^0 = 1
   //    0^e = 0
   */
   if( cpEqu_BNU_CHUNK(dataExp, nsE, 0) ) {
      COPY_BNU(dataY, MOD_MNT_R(pMont), nsM);
   }
   else if( cpEqu_BNU_CHUNK(dataX, nsX, 0) ) {
      ZEXPAND_BNU(dataY, 0, nsM);
   }

   /* general case */
   else {
      cpSize bitsizeE = BITSIZE_BNU(dataExp, nsE);
      cpSize bitsizeEwin = gsMontExp_WinSize(bitsizeE);
      cpSize nPrecomute= 1<<bitsizeEwin;
      BNU_CHUNK_T mask = nPrecomute -1;
      cpSize chunkSize = CACHE_LINE_SIZE/nPrecomute;
      int n;

      BNU_CHUNK_T* pTable = (BNU_CHUNK_T*)(IPP_ALIGNED_PTR((pBuffer), CACHE_LINE_SIZE));
      BNU_CHUNK_T* dataT = pTable + gsPrecompResourcelen(nPrecomute, nsM);
      BNU_CHUNK_T* dataE = dataT + nsM;

      /* copy and expand base to the modulus length */
      ZEXPAND_COPY_BNU(dataY, nsM, dataX, nsX);
      /* convert base to Montgomery domain */
      MOD_METHOD( pMont )->encode(dataY, dataY, (gsModEngine*)pMont);

      /* initialize recource */
      #if defined(GS_DEBUG)
      {
         if(0 != IPP_BYTES_TO_ALIGN(pResource, 64))
            printf("gsMontExpWin_BNU_sscm(): alignment test fail!!\n");
      }
      #endif
      cpScramblePut(((Ipp8u*)pTable)+0, chunkSize, (Ipp32u*)MOD_MNT_R(pMont), nsM*sizeof(BNU_CHUNK_T)/sizeof(Ipp32u));
      COPY_BNU(dataT, dataY, nsM);
      cpScramblePut(((Ipp8u*)pTable)+chunkSize, chunkSize, (Ipp32u*)dataT, nsM*sizeof(BNU_CHUNK_T)/sizeof(Ipp32u));
      for(n=2; n<nPrecomute; n++) {
         MOD_METHOD( pMont )->mul(dataT, dataY, dataT, pMont);
         cpScramblePut(((Ipp8u*)pTable)+n*chunkSize, chunkSize, (Ipp32u*)dataT, nsM*sizeof(BNU_CHUNK_T)/sizeof(Ipp32u));
      }

      /* expand exponent*/
      ZEXPAND_COPY_BNU(dataE, nsE+1, dataExp, nsE);
      bitsizeE = ((bitsizeE+bitsizeEwin-1)/bitsizeEwin) *bitsizeEwin;

      /* exponentiation */
      {
         /* position of the 1-st (left) window */
         int eBit = bitsizeE-bitsizeEwin;

         /* extract 1-st window value */
         Ipp32u eChunk = *((Ipp32u*)((Ipp16u*)dataE + eBit/BITSIZE(Ipp16u)));
         int shift = eBit & 0xF;
         Ipp32u windowVal = (eChunk>>shift) &mask;

         /* initialize result */
         cpScrambleGet((Ipp32u*)dataY, nsM*sizeof(BNU_CHUNK_T)/sizeof(Ipp32u), ((Ipp8u*)pTable)+windowVal*chunkSize, chunkSize);

         for(eBit-=bitsizeEwin; eBit>=0; eBit-=bitsizeEwin) {
            /* do square window times */
            for(n=0,windowVal=0; n<bitsizeEwin; n++) {
               MOD_METHOD( pMont )->sqr(dataY, dataY, pMont);
            }

            /* extract next window value */
            eChunk = *((Ipp32u*)((Ipp16u*)dataE + eBit/BITSIZE(Ipp16u)));
            shift = eBit & 0xF;
            windowVal = (eChunk>>shift) &mask;

            /* exptact precomputed value and muptiply */
            cpScrambleGet((Ipp32u*)dataT, nsM*sizeof(BNU_CHUNK_T)/sizeof(Ipp32u), ((Ipp8u*)pTable)+windowVal*chunkSize, chunkSize);

            MOD_METHOD( pMont )->mul(dataY, dataY, dataT, pMont);
         }

      }

      /* convert result back to regular domain */
      MOD_METHOD( pMont )->decode(dataY, dataY, pMont);
   }

   return nsM;
}
#endif

/*
// definition of RSA exponentiation (PX/GPR based)
*/
cpSize gsPubBuffer_gpr(int modulusBits)
{
   cpSize nsM = BITS_BNU_CHUNK(modulusBits);
   cpSize bufferNum = nsM;
   return bufferNum;
}
static cpSize gsPrvBuffer_gpr(int modulusBits)
{
   cpSize w = gsMontExp_WinSize(modulusBits);
   //cpSize tbl_num = (1==w)? 0 : (1<<w);
   cpSize tbl_num = (1<<w);

   cpSize nsM = BITS_BNU_CHUNK(modulusBits);

   cpSize bufferNum = gsPrecompResourcelen(tbl_num, nsM) /* pre-computed table */
                    + nsM                                /* copy of base X */
                    + nsM;                               /* zero expanded exponent | "masked" multipler (X|1) */
   //#if !defined(_USE_WINDOW_EXP_)
   //bufferNum += nsM;
   //#endif

   return bufferNum;
}

gsMethod_RSA* gsMethod_RSA_gpr_public(void)
{
   static gsMethod_RSA m = {
      MIN_RSA_SIZE, MAX_RSA_SIZE,      // appication area

      gsPubBuffer_gpr,
      gsPrvBuffer_gpr,

      gsMontExpBin_BNU,                // public key exp
#ifdef _SLIMBOOT_OPT
      NULL,
#else
      #if defined(_USE_WINDOW_EXP_)
      gsMontExpWin_BNU_sscm            // window private key exp
      #else
      gsMontExpBin_BNU_sscm,           // binary private key exp
      #endif
#endif
   };
   return &m;
}
