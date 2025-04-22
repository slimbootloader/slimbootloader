/*******************************************************************************
* Copyright 2016-2020 Intel Corporation
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
//     HMAC General Functionality
//
//  Contents:
//     ippsHMACGetSize_rmf()
//     ippsHMACInit_rmf()
//     ippsHMACPack_rmf()
//     ippsHMACUnpack_rmf()
//     ippsHMACDuplicate_rmf()
//     ippsHMACUpdate_rmf()
//     ippsHMACGetTag_rmf()
//     ippsHMACFinal_rmf()
//     ippsHMACMessage_rmf()
//
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcphmac.h"
#include "pcphmac_rmf.h"
#include "pcptool.h"

/*F*
//    Name: ippsHMACGetSize_rmf
//
// Purpose: Returns size of HMAC state (bytes).
//
// Returns:                Reason:
//    ippStsNullPtrErr        pSzie == NULL
//    ippStsNoErr             no errors
//
// Parameters:
//    pSize       pointer to the HMAC state size
//
*F*/
IPPFUN(IppStatus, ippsHMACGetSize_rmf,(int* pSize))
{
   /* test size's pointer */
   IPP_BAD_PTR1_RET(pSize);

   *pSize = sizeof(IppsHMACState_rmf) +(HASH_ALIGNMENT-1);
   return ippStsNoErr;
}

/*F*
//    Name: ippsHMACInit_rmf
//
// Purpose: Init HMAC state.
//
// Returns:                Reason:
//    ippStsNullPtrErr           pKey == NULL
//                               pState == NULL
//    ippStsLengthErr            keyLen <0
//    ippStsNoErr                no errors
//
// Parameters:
//    pKey        pointer to the secret key
//    keyLen      length (bytes) of the secret key
//    pState      pointer to the HMAC state
//    pMethod     hash method
//
*F*/
IPPFUN(IppStatus, ippsHMACInit_rmf,(const Ipp8u* pKey, int keyLen,
                                    IppsHMACState_rmf* pCtx,
                                    const IppsHashMethod* pMethod))
{
   /* test pointer */
   IPP_BAD_PTR2_RET(pCtx, pMethod);
   pCtx = (IppsHMACState_rmf*)( IPP_ALIGNED_PTR(pCtx, HASH_ALIGNMENT) );

   /* test key pointer and key length */
   IPP_BAD_PTR1_RET(pKey);
   IPP_BADARG_RET(0>keyLen, ippStsLengthErr);

   /* set state ID */
   HMAC_CTX_ID(pCtx) = idCtxHMAC;

   /* init hash context */
   ippsHashInit_rmf(&HASH_CTX(pCtx), pMethod);

   {
      int n;

      /* hash specific */
      IppsHashState_rmf* pHashCtx = &HASH_CTX(pCtx);
      int mbs = pMethod->msgBlkSize;
      int hashSize = pMethod->hashLen;

      /* copyMask = keyLen>mbs? 0xFF : 0x00 */
      int copyMask = (mbs-keyLen) >>(BITSIZE(int)-1);

      /* actualKeyLen = keyLen>mbs? hashSize:keyLen */
      int actualKeyLen = (hashSize & copyMask) | (keyLen & ~copyMask);

      /* compute hash(key, keyLen) just in case */
      ippsHashUpdate_rmf(pKey, keyLen, pHashCtx);
      ippsHashFinal_rmf(HASH_BUFF(pHashCtx), pHashCtx);

      /* copy either key or hash(key) into ipad- and opad- buffers */
      MASKED_COPY_BNU(pCtx->ipadKey, (Ipp8u)copyMask, HASH_BUFF(pHashCtx), pKey, actualKeyLen);
      MASKED_COPY_BNU(pCtx->opadKey, (Ipp8u)copyMask, HASH_BUFF(pHashCtx), pKey, actualKeyLen);

      /* XOR-ing key */
      for(n=0; n<actualKeyLen; n++) {
         pCtx->ipadKey[n] ^= (Ipp8u)IPAD;
         pCtx->opadKey[n] ^= (Ipp8u)OPAD;
      }
      for(; n<mbs; n++) {
         pCtx->ipadKey[n] = (Ipp8u)IPAD;
         pCtx->opadKey[n] = (Ipp8u)OPAD;
      }

      /* ipad key processing */
      ippsHashUpdate_rmf(pCtx->ipadKey, mbs, pHashCtx);

      return ippStsNoErr;
   }
}

/*F*
//    Name: ippsHMACPack_rmf
//
// Purpose: Copy initialized context to the buffer.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pCtx == NULL
//                            pBuffer == NULL
//    ippStsNoErr             no errors
//
// Parameters:
//    pCtx        pointer keyed hash state
//    pBuffer     pointer to the destination buffer
//    bufSize     size of destination buffer
//
*F*/
IPPFUN(IppStatus, ippsHMACPack_rmf,(const IppsHMACState_rmf* pCtx, Ipp8u* pBuffer, int bufSize))
{
   /* test pointers */
   IPP_BAD_PTR2_RET(pCtx, pBuffer);
   /* test the context */
   IPP_BADARG_RET(!HMAC_VALID_ID(pCtx), ippStsContextMatchErr);
   /* test buffer length */
   IPP_BADARG_RET((int)(sizeof(IppsHMACState_rmf)+HASH_ALIGNMENT-1)>bufSize, ippStsNoMemErr);

   CopyBlock(pCtx, pBuffer, sizeof(IppsHMACState_rmf));
   return ippStsNoErr;
}

/*F*
//    Name: ippsHMACUnpack_rmf
//
// Purpose: Unpack buffer content into the initialized context.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pBuffer == NULL
//                            pCtx == NULL
//    ippStsNoErr             no errors
//
// Parameters:
//    pBuffer     pointer to the source buffer
//    pSize       pointer to the packed spec size
//
*F*/
IPPFUN(IppStatus, ippsHMACUnpack_rmf,(const Ipp8u* pBuffer, IppsHMACState_rmf* pCtx))
{
   /* test pointers */
   IPP_BAD_PTR2_RET(pCtx, pBuffer);

   CopyBlock(pBuffer, pCtx, sizeof(IppsHMACState_rmf));
   return ippStsNoErr;
}

/*F*
//    Name: ippsHMACDuplicate_rmf
//
// Purpose: Clone HMAC state.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pSrcState == NULL
//                            pDstState == NULL
//    ippStsContextMatchErr   pSrcState->idCtx != idCtxHMAC
//                            pDstState->idCtx != idCtxHMAC
//    ippStsNoErr             no errors
//
// Parameters:
//    pSrcState   pointer to the source HMAC state
//    pDstState   pointer to the target HMAC state
//
// Note:
//    pDstState may not to be initialized by ippsHMACInit_rmf()
//
*F*/
IPPFUN(IppStatus, ippsHMACDuplicate_rmf,(const IppsHMACState_rmf* pSrcCtx, IppsHMACState_rmf* pDstCtx))
{
   /* test state pointers */
   IPP_BAD_PTR2_RET(pSrcCtx, pDstCtx);
   /* test states ID */
   IPP_BADARG_RET(!HMAC_VALID_ID(pSrcCtx), ippStsContextMatchErr);

   /* copy HMAC state */
   CopyBlock(pSrcCtx, pDstCtx, sizeof(IppsHMACState_rmf));
   return ippStsNoErr;
}

/*F*
//    Name: ippsHMACUpdate_rmf
//
// Purpose: Updates intermadiate MAC based on input stream.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pSrc == NULL
//                            pState == NULL
//    ippStsContextMatchErr   pState->idCtx != idCtxHMAC
//    ippStsLengthErr         len <0
//    ippStsNoErr             no errors
//
// Parameters:
//    pSrc        pointer to the input stream
//    len         input stream length
//    pState      pointer to the HMAC state
//
*F*/
IPPFUN(IppStatus, ippsHMACUpdate_rmf,(const Ipp8u* pSrc, int len, IppsHMACState_rmf* pCtx))
{
   /* test state pointers */
   IPP_BAD_PTR1_RET(pCtx);

   /* test state ID */
   IPP_BADARG_RET(!HMAC_VALID_ID(pCtx), ippStsContextMatchErr);
   /* test input length */
   IPP_BADARG_RET((len<0), ippStsLengthErr);
   /* test source pointer */
   IPP_BADARG_RET((len && !pSrc), ippStsNullPtrErr);

   if(len)
      return ippsHashUpdate_rmf(pSrc, len, &HASH_CTX(pCtx));
   else
      return ippStsNoErr;
}

/*F*
//    Name: ippsHMACFinal_rmf
//
// Purpose: Stop message digesting and return digest.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pMD == NULL
//                            pState == NULL
//    ippStsContextMatchErr   pState->idCtx != idCtxHMAC
//    ippStsLengthErr         sizeof(DigestMD5) < mdLen <1
//    ippStsNoErr             no errors
//
// Parameters:
//    pMD         address of the output digest
//    pState      pointer to the HMAC state
//
*F*/
IPPFUN(IppStatus, ippsHMACFinal_rmf,(Ipp8u* pMD, int mdLen, IppsHMACState_rmf* pCtx))
{
   /* test state pointer and ID */
   IPP_BAD_PTR1_RET(pCtx);
   IPP_BADARG_RET(!HMAC_VALID_ID(pCtx), ippStsContextMatchErr);

   /* test MD pointer and length */
   IPP_BAD_PTR1_RET(pMD);
   IPP_BADARG_RET(mdLen<=0, ippStsLengthErr);

   {
      /* hash specific */
      IppsHashState_rmf* pHashCtx = &HASH_CTX(pCtx);
      const IppsHashMethod* method = HASH_METHOD(pHashCtx);
      int mbs = method->msgBlkSize;
      int hashSize = method->hashLen;
      if(mdLen>hashSize)
         IPP_ERROR_RET(ippStsLengthErr);

      /*
      // finalize hmac
      */
      {
         /* finalize 1-st step */
         Ipp8u md[IPP_SHA512_DIGEST_BITSIZE/8];
         IppStatus sts = ippsHashFinal_rmf(md, pHashCtx);

         if(ippStsNoErr==sts) {
            /* perform outer hash */
            ippsHashUpdate_rmf(pCtx->opadKey, mbs, pHashCtx);
            ippsHashUpdate_rmf(md, hashSize, pHashCtx);

            /* complete HMAC */
            ippsHashFinal_rmf(md, pHashCtx);
            CopyBlock(md, pMD, IPP_MIN(hashSize, mdLen));

            /* ready to the next HMAC computation */
            ippsHashUpdate_rmf(pCtx->ipadKey, mbs, pHashCtx);
         }

         return sts;
      }
   }
}

/*F*
//    Name: ippsHMACGetTag_rmf
//
// Purpose: Compute digest with further digesting ability.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pMD == NULL
//                            pState == NULL
//    ippStsContextMatchErr   pState->idCtx != idCtxHMAC
//    ippStsLengthErr         size_of_digest < mdLen <1
//    ippStsNoErr             no errors
//
// Parameters:
//    pMD         address of the output digest
//    mdLen       length of the digest
//    pState      pointer to the HMAC state
//
*F*/
IPPFUN(IppStatus, ippsHMACGetTag_rmf,(Ipp8u* pMD, int mdLen, const IppsHMACState_rmf* pCtx))
{
   /* test state pointer and ID */
   IPP_BAD_PTR1_RET(pCtx);
   IPP_BADARG_RET(!HMAC_VALID_ID(pCtx), ippStsContextMatchErr);

   /* test MD pointer */
   IPP_BAD_PTR1_RET(pMD);

   { /* TBD: consider implementation without copy of context */
      IppStatus sts;
      IppsHMACState_rmf tmpCtx;
      ippsHMACDuplicate_rmf(pCtx, &tmpCtx);

      sts = ippsHMACFinal_rmf(pMD, mdLen, &tmpCtx);

      PurgeBlock(&tmpCtx, sizeof(IppsHMACState_rmf));
      return sts;
   }
}

/*F*
//    Name: ippsHMACMessage_rmf
//
// Purpose: MAC (MD5) of the whole message.
//
// Returns:                Reason:
//    ippStsNullPtrErr           pMsg == NULL
//                               pKey == NULL
//                               pMD == NULL
//    ippStsLengthErr            msgLen <0
//                               keyLen <0
//                               size_of_digest < mdLen <1
//    ippStsNotSupportedModeErr  if algID is not match to supported hash alg
//    ippStsNoErr                no errors
//
// Parameters:
//    pMsg        pointer to the input message
//    msgLen      input message length
//    pKey        pointer to the secret key
//    keyLen      secret key length
//    pMD         pointer to message digest
//    mdLen       MD length
//    pMethod     hash method
//
*F*/
IPPFUN(IppStatus, ippsHMACMessage_rmf,(const Ipp8u* pMsg, int msgLen,
                                       const Ipp8u* pKey, int keyLen,
                                       Ipp8u* pMD, int mdLen,
                                       const IppsHashMethod* pMethod))
{
   /* test method pointer */
   IPP_BAD_PTR1_RET(pMethod);
   /* test secret key pointer and length */
   IPP_BAD_PTR1_RET(pKey);
   IPP_BADARG_RET((keyLen<0), ippStsLengthErr);
   /* test input message pointer and length */
   IPP_BADARG_RET((msgLen<0), ippStsLengthErr);
   IPP_BADARG_RET((msgLen && !pMsg), ippStsNullPtrErr);

   /* test MD pointer and length */
   IPP_BAD_PTR1_RET(pMD);
   IPP_BADARG_RET(0>=mdLen || mdLen>pMethod->hashLen, ippStsLengthErr);

   {
      IppsHMACState_rmf ctx;
      IppStatus sts = ippsHMACInit_rmf(pKey, keyLen, &ctx, pMethod);
      if(ippStsNoErr!=sts) goto exit;

      sts = ippsHashUpdate_rmf(pMsg,msgLen, &HASH_CTX(&ctx));
      if(ippStsNoErr!=sts) goto exit;

      sts = ippsHMACFinal_rmf(pMD, mdLen, &ctx);

      exit:
      PurgeBlock(&ctx, sizeof(IppsHMACState_rmf));
      return sts;
   }
}
