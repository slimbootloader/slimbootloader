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
//     RSA Functions
//
//  Contents:
//     ippsRSA_GetBufferSizePublicKey()
//     ippsRSA_GetBufferSizePrivateKey()
//     ippsRSA_Encrypt()
//     ippsRSA_Decrypt()
//
//
*/

#include "owncp.h"
#include "pcpbn.h"
#include "pcpngrsa.h"
#include "pcpngrsamontstuff.h"

#if (_IPP32E>=_IPP32E_K0) && defined(_RSA_AVX512)
#include "pcpngrsamontstuff_avx512.h"
#endif

#if (_IPP32E>=_IPP32E_L9) && defined(_RSA_AVX2)
#include "pcpngrsamontstuff_avx2.h"
#endif

#if (_IPP>=_IPP_W7) && defined(_RSA_SSE2)
#include "pcpngrsamontstuff_sse2.h"
#endif

/* get default method based on CPU's features */
static gsMethod_RSA* getDefaultMethod_RSA_public(int modulusBitSize)
{
   gsMethod_RSA* m;

   #if(_IPP32E>=_IPP32E_K0) && defined(_RSA_AVX512)
   m = IsFeatureEnabled(ippCPUID_AVX512IFMA)? gsMethod_RSA_avx512() : gsMethod_RSA_avx2();
   #elif(_IPP32E>=_IPP32E_L9) && defined(_RSA_AVX2)
   m = gsMethod_RSA_avx2();
   #elif(_IPP>=_IPP_W7) && defined(_RSA_SSE2)
   m = gsMethod_RSA_sse2();
   #else
   m = gsMethod_RSA_gpr_public();
#endif

   if( !(m->loModulusBisize <= modulusBitSize && modulusBitSize <= m->hiModulusBisize) )
      m = gsMethod_RSA_gpr_public();
   return m;
}

/*F*
// Name: ippsRSA_GetBufferSizePublicKey
//
// Purpose: Returns size of temporary buffer (in bytes) for public key operation
//
// Returns:                   Reason:
//    ippStsNullPtrErr           NULL == pKey
//                               NULL == pBufferSize
//
//    ippStsContextMatchErr     !RSA_PUB_KEY_VALID_ID()
//
//    ippStsIncompleteContextErr no ippsRSA_SetPublicKey() call
//
//    ippStsNoErr                no error
//
// Parameters:
//    pBufferSize pointer to size of temporary buffer
//    pKey        pointer to the key context
*F*/
IPPFUN(IppStatus, ippsRSA_GetBufferSizePublicKey,(int* pBufferSize, const IppsRSAPublicKeyState* pKey))
{
   IPP_BAD_PTR1_RET(pKey);
   pKey = (IppsRSAPublicKeyState*)( IPP_ALIGNED_PTR(pKey, RSA_PUBLIC_KEY_ALIGNMENT) );
   IPP_BADARG_RET(!RSA_PUB_KEY_VALID_ID(pKey), ippStsContextMatchErr);
   IPP_BADARG_RET(!RSA_PUB_KEY_IS_SET(pKey), ippStsIncompleteContextErr);

   IPP_BAD_PTR1_RET(pBufferSize);

   {
      cpSize bitSizeN = RSA_PUB_KEY_BITSIZE_N(pKey);
      cpSize nsN = BITS_BNU_CHUNK(bitSizeN);

      gsMethod_RSA* m = getDefaultMethod_RSA_public(bitSizeN);

      cpSize bufferNum = ((nsN+1)*2)*2       /* (1)2 BN for RSA (enc)/sign schemes */
                        + 1;                 /* BNU_CHUNK_T alignment */
      bufferNum += m->publicBufNum(bitSizeN);/* RSA public key operation */

      *pBufferSize = bufferNum*sizeof(BNU_CHUNK_T);

      return ippStsNoErr;
   }
}


/*F*
// Name: ippsRSA_GetBufferSizePrivateKey
//
// Purpose: Returns size of temporary buffer (in bytes) for private key operation
//
// Returns:                   Reason:
//    ippStsNullPtrErr           NULL == pKey
//                               NULL == pBufferSize
//
//    ippStsContextMatchErr     !RSA_PRV_KEY_VALID_ID()
//
//    ippStsIncompleteContextErr (type1) private key is not set up
//
//    ippStsNoErr                no error
//
// Parameters:
//    pBufferSize pointer to size of temporary buffer
//    pKey        pointer to the key context
*F*/

IPPFUN(IppStatus, ippsRSA_GetBufferSizePrivateKey,(int* pBufferSize, const IppsRSAPrivateKeyState* pKey))
{
   IPP_BAD_PTR1_RET(pKey);
   pKey = (IppsRSAPrivateKeyState*)( IPP_ALIGNED_PTR(pKey, RSA_PUBLIC_KEY_ALIGNMENT) );
   IPP_BADARG_RET(!RSA_PRV_KEY_VALID_ID(pKey), ippStsContextMatchErr);
   IPP_BADARG_RET(RSA_PRV_KEY1_VALID_ID(pKey) && !RSA_PRV_KEY_IS_SET(pKey), ippStsIncompleteContextErr);

   IPP_BAD_PTR1_RET(pBufferSize);

   {
      cpSize modulusBits = (RSA_PRV_KEY1_VALID_ID(pKey))? RSA_PRV_KEY_BITSIZE_N(pKey) :
                                                  IPP_MAX(RSA_PRV_KEY_BITSIZE_P(pKey), RSA_PRV_KEY_BITSIZE_Q(pKey));
      gsMethod_RSA* m = getDefaultMethod_RSA_public(modulusBits);

      cpSize bitSizeN = (RSA_PRV_KEY1_VALID_ID(pKey))? modulusBits : modulusBits*2;
      cpSize nsN = BITS_BNU_CHUNK(bitSizeN);

      cpSize bn_scheme = (nsN+1)*2;    /* BN for RSA schemes */
      cpSize bn3_gen = (RSA_PRV_KEY2_VALID_ID(pKey))? (nsN+1)*2*3 : 0; /* 3 BN for generation/validation */

      cpSize bufferNum = bn_scheme*2               /* (1)2 BN for RSA (enc)/sign schemes */
                       + 1;                        /* BNU_CHUNK_T alignment */
      bufferNum += m->privateBufNum(modulusBits);  /* RSA private key operation */

      bufferNum = IPP_MAX(bufferNum, bn3_gen); /* generation/validation resource overlaps RSA resource  */

      *pBufferSize = bufferNum*sizeof(BNU_CHUNK_T);

      #if defined(_USE_WINDOW_EXP_)
      /* pre-computed table should be CACHE_LINE aligned*/
      *pBufferSize += CACHE_LINE_SIZE;
      #endif

      return ippStsNoErr;
   }
}


void gsRSApub_cipher(IppsBigNumState* pY,
               const IppsBigNumState* pX,
               const IppsRSAPublicKeyState* pKey,
                     BNU_CHUNK_T* pBuffer)
{
   gsMethod_RSA* m = getDefaultMethod_RSA_public(RSA_PRV_KEY_BITSIZE_N(pKey));

   BNU_CHUNK_T* dataY = BN_NUMBER(pY);

   cpSize nsY = m->publicExpFun(dataY,
                               BN_NUMBER(pX), BN_SIZE(pX),
                               RSA_PUB_KEY_E(pKey), BITS_BNU_CHUNK(RSA_PUB_KEY_BITSIZE_E(pKey)),
                               RSA_PUB_KEY_NMONT(pKey),
                               pBuffer);
   FIX_BNU(dataY, nsY);
   BN_SIZE(pY) = nsY;
   BN_SIGN(pY) = ippBigNumPOS;
}


/*F*
// Name: ippsRSA_Encrypt
//
// Purpose: Performs RSA Encryprion
//
// Returns:                   Reason:
//    ippStsNullPtrErr           NULL == pKey
//                               NULL == pPtxt
//                               NULL == pCtxt
//                               NULL == pBuffer
//
//    ippStsContextMatchErr     !RSA_PUB_KEY_VALID_ID()
//                              !BN_VALID_ID(pPtxt)
//                              !BN_VALID_ID(pCtxt)
//
//    ippStsIncompleteContextErr public key is not setup
//
//    ippStsOutOfRangeErr        pPtxt >= modulus
//                               pPtxt <0
//
//    ippStsSizeErr              BN_ROOM(pCtxt) is not enough
//
//    ippStsNoErr                no error
//
// Parameters:
//    pPtxt          pointer to the plaintext
//    pCtxt          pointer to the ciphertext
//    pKey           pointer to the key context
//    pScratchBuffer pointer to the temporary buffer
*F*/
IPPFUN(IppStatus, ippsRSA_Encrypt,(const IppsBigNumState* pPtxt,
                                         IppsBigNumState* pCtxt,
                                   const IppsRSAPublicKeyState* pKey,
                                         Ipp8u* pBuffer))
{
   IPP_BAD_PTR2_RET(pKey, pBuffer);
   pKey = (IppsRSAPublicKeyState*)( IPP_ALIGNED_PTR(pKey, RSA_PUBLIC_KEY_ALIGNMENT) );
   IPP_BADARG_RET(!RSA_PUB_KEY_VALID_ID(pKey), ippStsContextMatchErr);
   IPP_BADARG_RET(!RSA_PUB_KEY_IS_SET(pKey), ippStsIncompleteContextErr);

   IPP_BAD_PTR1_RET(pPtxt);
   pPtxt = (IppsBigNumState*)( IPP_ALIGNED_PTR(pPtxt, BN_ALIGNMENT) );
   IPP_BADARG_RET(!BN_VALID_ID(pPtxt), ippStsContextMatchErr);
   IPP_BADARG_RET(BN_NEGATIVE(pPtxt), ippStsOutOfRangeErr);
   IPP_BADARG_RET(0 <= cpCmp_BNU(BN_NUMBER(pPtxt), BN_SIZE(pPtxt),
                                 MOD_MODULUS(RSA_PUB_KEY_NMONT(pKey)), MOD_LEN(RSA_PUB_KEY_NMONT(pKey))), ippStsOutOfRangeErr);

   IPP_BAD_PTR1_RET(pCtxt);
   pCtxt = (IppsBigNumState*)( IPP_ALIGNED_PTR(pCtxt, BN_ALIGNMENT) );
   IPP_BADARG_RET(!BN_VALID_ID(pCtxt), ippStsContextMatchErr);
   IPP_BADARG_RET(BN_ROOM(pCtxt) < BITS_BNU_CHUNK(RSA_PUB_KEY_BITSIZE_N(pKey)), ippStsSizeErr);

   {
      BNU_CHUNK_T* pScratchBuffer = (BNU_CHUNK_T*)(IPP_ALIGNED_PTR(pBuffer, (int)sizeof(BNU_CHUNK_T)) );
      gsRSApub_cipher(pCtxt, pPtxt, pKey, pScratchBuffer);
   return ippStsNoErr;
   }
}



void gsRSAprv_cipher(IppsBigNumState* pY,
               const IppsBigNumState* pX,
               const IppsRSAPrivateKeyState* pKey,
                     BNU_CHUNK_T* pBuffer)
{
   gsMethod_RSA* m = getDefaultMethod_RSA_public(RSA_PRV_KEY_BITSIZE_N(pKey));

   if (m->privateExpFun != NULL) {
      BNU_CHUNK_T* dataY = BN_NUMBER(pY);
      cpSize nsY = m->privateExpFun(dataY,
                                    BN_NUMBER(pX), BN_SIZE(pX),
                                    RSA_PRV_KEY_D(pKey), BITS_BNU_CHUNK(RSA_PRV_KEY_BITSIZE_D(pKey)),
                                    RSA_PRV_KEY_NMONT(pKey),
                                    pBuffer);
      FIX_BNU(dataY, nsY);
      BN_SIZE(pY) = nsY;
      BN_SIGN(pY) = ippBigNumPOS;
   }
}

void gsRSAprv_cipher_crt(IppsBigNumState* pY,
               const IppsBigNumState* pX,
               const IppsRSAPrivateKeyState* pKey,
                     BNU_CHUNK_T* pBuffer)
{
   const BNU_CHUNK_T* dataX = BN_NUMBER(pX);
   cpSize nsX = BN_SIZE(pX);
   BNU_CHUNK_T* dataXp = BN_NUMBER(pY);
   BNU_CHUNK_T* dataXq = BN_BUFFER(pY);

   /* P- and Q- montgometry engines */
   gsModEngine* pMontP = RSA_PRV_KEY_PMONT(pKey);
   gsModEngine* pMontQ = RSA_PRV_KEY_QMONT(pKey);
   cpSize nsP = MOD_LEN(pMontP);
   cpSize nsQ = MOD_LEN(pMontQ);
   cpSize bitSizeDP = BITSIZE_BNU(RSA_PRV_KEY_DP(pKey), nsP);
   cpSize bitSizeDQ = BITSIZE_BNU(RSA_PRV_KEY_DQ(pKey), nsQ);

   gsMethod_RSA* m;

   /* compute xq = x^dQ mod Q */
   COPY_BNU(dataXq, dataX, nsX);
   cpMod_BNU(dataXq, nsX, MOD_MODULUS(pMontQ), nsQ);

   m = getDefaultMethod_RSA_public(bitSizeDQ);
   if (m->privateExpFun != NULL) {
      m->privateExpFun(dataXq, dataXq, nsQ, RSA_PRV_KEY_DQ(pKey), BITS_BNU_CHUNK(bitSizeDQ), pMontQ, pBuffer);
   }

   /* compute xp = x^dP mod P */
   COPY_BNU(dataXp, dataX, nsX);
   cpMod_BNU(dataXp, nsX, MOD_MODULUS(pMontP), nsP);

   m = getDefaultMethod_RSA_public(bitSizeDP);
   if (m->privateExpFun != NULL) {
      m->privateExpFun(dataXp, dataXp, nsP, RSA_PRV_KEY_DP(pKey), BITS_BNU_CHUNK(bitSizeDP), pMontP, pBuffer);
   }

   /*
   // recombination
   */
   {
      cpSize nsQP;
      BNU_CHUNK_T* dataXqp = pBuffer+nsP+nsQ;
      /* compute xp = x^dP mod P */
      COPY_BNU(dataXqp, dataXq, nsQ);
      nsQP = cpMod_BNU(dataXqp, nsQ, MOD_MODULUS(pMontP), nsP);

      {
         /* xp -= xq */
         BNU_CHUNK_T cf = cpSub_BNU(dataXp, dataXp, dataXqp, nsQP);
         if(nsP-nsQP)
            cf = cpDec_BNU(dataXp+nsQP, dataXp+nsQP, (nsP-nsQP), cf);
         if(cf)
            cpAdd_BNU(dataXp, dataXp, MOD_MODULUS(pMontP), nsP);

         /* xp = xp*qInv mod P */
         MOD_METHOD( pMontP )->mul(dataXp, dataXp, RSA_PRV_KEY_INVQ(pKey), pMontP);

         /* Y = xq + xp*Q */
         cpMul_BNU_school(pBuffer,
                    dataXp, nsP,
                    MOD_MODULUS(pMontQ), nsQ);
         cf = cpAdd_BNU(BN_NUMBER(pY), pBuffer, dataXq, nsQ);
         cpInc_BNU(BN_NUMBER(pY)+nsQ, pBuffer+nsQ, nsP, cf);
      }
   }

   nsX = nsP+nsQ;
   FIX_BNU(BN_NUMBER(pY), nsX);
   BN_SIZE(pY) = nsX;
   BN_SIGN(pY) = ippBigNumPOS;
}

/*F*
// Name: ippsRSA_Decrypt
//
// Purpose: Performs RSA Decryprion
//
// Returns:                   Reason:
//    ippStsNullPtrErr           NULL == pKey
//                               NULL == pCtxt
//                               NULL == pPtxt
//                               NULL == pBuffer
//
//    ippStsContextMatchErr     !RSA_PUB_KEY_VALID_ID()
//                              !BN_VALID_ID(pCtxt)
//                              !BN_VALID_ID(pPtxt)
//
//    ippStsIncompleteContextErr private key is not set up
//
//    ippStsOutOfRangeErr        pCtxt >= modulus
//                               pCtxt <0
//
//    ippStsSizeErr              BN_ROOM(pPtxt) is not enough
//
//    ippStsNoErr                no error
//
// Parameters:
//    pCtxt          pointer to the ciphertext
//    pPtxt          pointer to the plaintext
//    pKey           pointer to the key context
//    pScratchBuffer pointer to the temporary buffer
*F*/
IPPFUN(IppStatus, ippsRSA_Decrypt,(const IppsBigNumState* pCtxt,
                                         IppsBigNumState* pPtxt,
                                   const IppsRSAPrivateKeyState* pKey,
                                         Ipp8u* pBuffer))
{
   IPP_BAD_PTR2_RET(pKey, pBuffer);
   pKey = (IppsRSAPrivateKeyState*)( IPP_ALIGNED_PTR(pKey, RSA_PRIVATE_KEY_ALIGNMENT) );
   IPP_BADARG_RET(!RSA_PRV_KEY_VALID_ID(pKey), ippStsContextMatchErr);
   IPP_BADARG_RET(!RSA_PRV_KEY_IS_SET(pKey), ippStsIncompleteContextErr);

   IPP_BAD_PTR1_RET(pCtxt);
   pCtxt = (IppsBigNumState*)( IPP_ALIGNED_PTR(pCtxt, BN_ALIGNMENT) );
   IPP_BADARG_RET(!BN_VALID_ID(pCtxt), ippStsContextMatchErr);
   IPP_BADARG_RET(BN_NEGATIVE(pCtxt), ippStsOutOfRangeErr);
   IPP_BADARG_RET(0 <= cpCmp_BNU(BN_NUMBER(pCtxt), BN_SIZE(pCtxt),
                                 MOD_MODULUS(RSA_PRV_KEY_NMONT(pKey)), MOD_LEN(RSA_PRV_KEY_NMONT(pKey))), ippStsOutOfRangeErr);

   IPP_BAD_PTR1_RET(pPtxt);
   pPtxt = (IppsBigNumState*)( IPP_ALIGNED_PTR(pPtxt, BN_ALIGNMENT) );
   IPP_BADARG_RET(!BN_VALID_ID(pPtxt), ippStsContextMatchErr);
   IPP_BADARG_RET(BN_ROOM(pPtxt) < BITS_BNU_CHUNK(RSA_PRV_KEY_BITSIZE_N(pKey)), ippStsSizeErr);

   {
      BNU_CHUNK_T* pScratchBuffer = (BNU_CHUNK_T*)( IPP_ALIGNED_PTR(pBuffer, (int)sizeof(BNU_CHUNK_T)) );

   if(RSA_PRV_KEY1_VALID_ID(pKey))
         gsRSAprv_cipher(pPtxt, pCtxt, pKey, pScratchBuffer);
   else
         gsRSAprv_cipher_crt(pPtxt, pCtxt, pKey, pScratchBuffer);

   return ippStsNoErr;
   }
}
