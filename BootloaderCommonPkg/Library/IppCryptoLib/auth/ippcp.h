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

/*
//              Intel(R) Integrated Performance Primitives (Intel(R) IPP)
//              Cryptographic Primitives (ippCP)
//
//
*/

#if !defined( IPPCP_H__ ) || defined( _OWN_BLDPCS )
#define IPPCP_H__


#if defined (_WIN32_WCE) && defined (_M_IX86) && defined (__stdcall)
  #define _IPP_STDCALL_CDECL
  #undef __stdcall
#endif


#ifndef __IPPCPDEFS_H__
  #include "ippcpdefs.h"
#endif


#ifdef  __cplusplus
extern "C" {
#endif

#if !defined( _IPP_NO_DEFAULT_LIB )
  #if defined( _IPP_SEQUENTIAL_DYNAMIC )
    #pragma comment( lib, __FILE__ "/../../lib/" _INTEL_PLATFORM "ippcp" )
  #elif defined( _IPP_SEQUENTIAL_STATIC )
    #pragma comment( lib, __FILE__ "/../../lib/" _INTEL_PLATFORM "ippcpmt" )
  #elif defined( _IPP_PARALLEL_DYNAMIC )
    #pragma comment( lib, __FILE__ "/../../lib/" _INTEL_PLATFORM "threaded/ippcp" )
  #elif defined( _IPP_PARALLEL_STATIC )
    #pragma comment( lib, __FILE__ "/../../lib/" _INTEL_PLATFORM "threaded/ippcpmt" )
  #endif
#endif


/* /////////////////////////////////////////////////////////////////////////////
//  Name:       ippcpGetLibVersion
//  Purpose:    getting of the library version
//  Returns:    the structure of information about version of ippCP library
//  Parameters:
//
//  Notes:      not necessary to release the returned structure
*/
IPPAPI( const IppLibraryVersion*, ippcpGetLibVersion, (void) )

/*
// =========================================================
// One-Way Hash Functions
// =========================================================
*/

/* method based generalized (reduced memory footprint) Hash Primitives */
IPPAPI( const IppsHashMethod*, ippsHashMethod_MD5, (void) )
IPPAPI( const IppsHashMethod*, ippsHashMethod_SM3, (void) )
IPPAPI( const IppsHashMethod*, ippsHashMethod_SHA1, (void) )
IPPAPI( const IppsHashMethod*, ippsHashMethod_SHA1_NI, (void) )
IPPAPI( const IppsHashMethod*, ippsHashMethod_SHA1_TT, (void) )
IPPAPI( const IppsHashMethod*, ippsHashMethod_SHA256, (void) )
IPPAPI( const IppsHashMethod*, ippsHashMethod_SHA256_NI, (void) )
IPPAPI( const IppsHashMethod*, ippsHashMethod_SHA256_TT, (void) )

IPPAPI( const IppsHashMethod*, ippsHashMethod_SHA224, (void) )
IPPAPI( const IppsHashMethod*, ippsHashMethod_SHA224_NI, (void) )
IPPAPI( const IppsHashMethod*, ippsHashMethod_SHA224_TT, (void) )

IPPAPI( const IppsHashMethod*, ippsHashMethod_SHA512, (void) )
IPPAPI( const IppsHashMethod*, ippsHashMethod_SHA384, (void) )
IPPAPI( const IppsHashMethod*, ippsHashMethod_SHA512_256, (void) )
IPPAPI( const IppsHashMethod*, ippsHashMethod_SHA512_224, (void) )

IPPAPI(IppStatus, ippsHashGetSize_rmf,(int* pSize))
IPPAPI(IppStatus, ippsHashInit_rmf,(IppsHashState_rmf* pCtx, const IppsHashMethod* pMethod))

IPPAPI(IppStatus, ippsHashPack_rmf,(const IppsHashState_rmf* pCtx, Ipp8u* pBuffer, int bufSize))
IPPAPI(IppStatus, ippsHashUnpack_rmf,(const Ipp8u* pBuffer, IppsHashState_rmf* pCtx))
IPPAPI(IppStatus, ippsHashDuplicate_rmf,(const IppsHashState_rmf* pSrcCtx, IppsHashState_rmf* pDstCtx))

IPPAPI(IppStatus, ippsHashUpdate_rmf,(const Ipp8u* pSrc, int len, IppsHashState_rmf* pCtx))
IPPAPI(IppStatus, ippsHashGetTag_rmf,(Ipp8u* pMD, int tagLen, const IppsHashState_rmf* pCtx))
IPPAPI(IppStatus, ippsHashFinal_rmf,(Ipp8u* pMD, IppsHashState_rmf* pCtx))
IPPAPI(IppStatus, ippsHashMessage_rmf,(const Ipp8u* pMsg, int len, Ipp8u* pMD, const IppsHashMethod* pMethod))


/* general MGF Primitives*/
IPPAPI(IppStatus, ippsMGF,(const Ipp8u* pSeed, int seedLen, Ipp8u* pMask, int maskLen, IppHashAlgId hashAlg))
IPPAPI(IppStatus, ippsMGF1_rmf,(const Ipp8u* pSeed, int seedLen, Ipp8u* pMask, int maskLen, const IppsHashMethod* pMethod))
IPPAPI(IppStatus, ippsMGF2_rmf,(const Ipp8u* pSeed, int seedLen, Ipp8u* pMask, int maskLen, const IppsHashMethod* pMethod))
/*
// =========================================================
// Keyed-Hash Message Authentication Codes
// =========================================================
*/

/* method based generalized (reduced memory footprint) Keyed HMAC primitives */
IPPAPI(IppStatus, ippsHMACGetSize_rmf,(int* pSize))
IPPAPI(IppStatus, ippsHMACInit_rmf,(const Ipp8u* pKey, int keyLen,
                                 IppsHMACState_rmf* pCtx,
                                 const IppsHashMethod* pMethod))

IPPAPI(IppStatus, ippsHMACPack_rmf,(const IppsHMACState_rmf* pCtx, Ipp8u* pBuffer, int bufSize))
IPPAPI(IppStatus, ippsHMACUnpack_rmf,(const Ipp8u* pBuffer, IppsHMACState_rmf* pCtx))
IPPAPI(IppStatus, ippsHMACDuplicate_rmf,(const IppsHMACState_rmf* pSrcCtx, IppsHMACState_rmf* pDstCtx))

IPPAPI(IppStatus, ippsHMACUpdate_rmf,(const Ipp8u* pSrc, int len, IppsHMACState_rmf* pCtx))
IPPAPI(IppStatus, ippsHMACFinal_rmf,(Ipp8u* pMD, int mdLen, IppsHMACState_rmf* pCtx))
IPPAPI(IppStatus, ippsHMACGetTag_rmf,(Ipp8u* pMD, int mdLen, const IppsHMACState_rmf* pCtx))
IPPAPI(IppStatus, ippsHMACMessage_rmf,(const Ipp8u* pMsg, int msgLen,
                                       const Ipp8u* pKey, int keyLen,
                                       Ipp8u* pMD, int mdLen,
                                       const IppsHashMethod* pMethod))


/*
// =========================================================
// Big Number Integer Arithmetic
// =========================================================
*/

/* Signed BigNum Operations */
IPPAPI(IppStatus, ippsBigNumGetSize,(int length, int* pSize))
IPPAPI(IppStatus, ippsBigNumInit,(int length, IppsBigNumState* pBN))

IPPAPI(IppStatus, ippsCmpZero_BN,(const IppsBigNumState* pBN, Ipp32u* pResult))
IPPAPI(IppStatus, ippsCmp_BN,(const IppsBigNumState* pA, const IppsBigNumState* pB, Ipp32u* pResult))

IPPAPI(IppStatus, ippsGetSize_BN,(const IppsBigNumState* pBN, int* pSize))
IPPAPI(IppStatus, ippsSet_BN,(IppsBigNumSGN sgn,
                              int length, const Ipp32u* pData,
                              IppsBigNumState* pBN))
IPPAPI(IppStatus, ippsGet_BN,(IppsBigNumSGN* pSgn,
                              int* pLength, Ipp32u* pData,
                              const IppsBigNumState* pBN))
IPPAPI(IppStatus, ippsRef_BN,(IppsBigNumSGN* pSgn, int* bitSize, Ipp32u** const ppData,
                              const IppsBigNumState* pBN))
IPPAPI(IppStatus, ippsExtGet_BN,(IppsBigNumSGN* pSgn,
                              int* pBitSize, Ipp32u* pData,
                              const IppsBigNumState* pBN))

IPPAPI(IppStatus, ippsAdd_BN,   (IppsBigNumState* pA, IppsBigNumState* pB, IppsBigNumState* pR))
IPPAPI(IppStatus, ippsSub_BN,   (IppsBigNumState* pA, IppsBigNumState* pB, IppsBigNumState* pR))
IPPAPI(IppStatus, ippsMul_BN,   (IppsBigNumState* pA, IppsBigNumState* pB, IppsBigNumState* pR))
IPPAPI(IppStatus, ippsMAC_BN_I, (IppsBigNumState* pA, IppsBigNumState* pB, IppsBigNumState* pR))
IPPAPI(IppStatus, ippsDiv_BN,   (IppsBigNumState* pA, IppsBigNumState* pB, IppsBigNumState* pQ, IppsBigNumState* pR))
IPPAPI(IppStatus, ippsMod_BN,   (IppsBigNumState* pA, IppsBigNumState* pM, IppsBigNumState* pR))
IPPAPI(IppStatus, ippsGcd_BN,   (IppsBigNumState* pA, IppsBigNumState* pB, IppsBigNumState* pGCD))
IPPAPI(IppStatus, ippsModInv_BN,(IppsBigNumState* pA, IppsBigNumState* pM, IppsBigNumState* pInv))

IPPAPI(IppStatus, ippsSetOctString_BN,(const Ipp8u* pStr, int strLen, IppsBigNumState* pBN))
IPPAPI(IppStatus, ippsGetOctString_BN,(Ipp8u* pStr, int strLen, const IppsBigNumState* pBN))

/*
// =========================================================
// RSA Cryptography
// =========================================================
*/
IPPAPI(IppStatus, ippsRSA_GetSizePublicKey,(int rsaModulusBitSize, int pubicExpBitSize, int* pKeySize))
IPPAPI(IppStatus, ippsRSA_InitPublicKey,(int rsaModulusBitSize, int publicExpBitSize,
                                         IppsRSAPublicKeyState* pKey, int keyCtxSize))
IPPAPI(IppStatus, ippsRSA_SetPublicKey,(const IppsBigNumState* pModulus,
                                        const IppsBigNumState* pPublicExp,
                                        IppsRSAPublicKeyState* pKey))
IPPAPI(IppStatus, ippsRSA_GetPublicKey,(IppsBigNumState* pModulus,
                                        IppsBigNumState* pPublicExp,
                                  const IppsRSAPublicKeyState* pKey))

IPPAPI(IppStatus, ippsRSA_GetSizePrivateKeyType1,(int rsaModulusBitSize, int privateExpBitSize, int* pKeySize))
IPPAPI(IppStatus, ippsRSA_InitPrivateKeyType1,(int rsaModulusBitSize, int privateExpBitSize,
                                               IppsRSAPrivateKeyState* pKey, int keyCtxSize))
IPPAPI(IppStatus, ippsRSA_SetPrivateKeyType1,(const IppsBigNumState* pModulus,
                                              const IppsBigNumState* pPrivateExp,
                                              IppsRSAPrivateKeyState* pKey))
IPPAPI(IppStatus, ippsRSA_GetPrivateKeyType1,(IppsBigNumState* pModulus,
                                              IppsBigNumState* pPrivateExp,
                                        const IppsRSAPrivateKeyState* pKey))

IPPAPI(IppStatus, ippsRSA_GetSizePrivateKeyType2,(int factorPbitSize, int factorQbitSize, int* pKeySize))
IPPAPI(IppStatus, ippsRSA_InitPrivateKeyType2,(int factorPbitSize, int factorQbitSize,
                                               IppsRSAPrivateKeyState* pKey, int keyCtxSize))
IPPAPI(IppStatus, ippsRSA_SetPrivateKeyType2,(const IppsBigNumState* pFactorP,
                                              const IppsBigNumState* pFactorQ,
                                              const IppsBigNumState* pCrtExpP,
                                              const IppsBigNumState* pCrtExpQ,
                                              const IppsBigNumState* pInverseQ,
                                              IppsRSAPrivateKeyState* pKey))
IPPAPI(IppStatus, ippsRSA_GetPrivateKeyType2,(IppsBigNumState* pFactorP,
                                              IppsBigNumState* pFactorQ,
                                              IppsBigNumState* pCrtExpP,
                                              IppsBigNumState* pCrtExpQ,
                                              IppsBigNumState* pInverseQ,
                                              const IppsRSAPrivateKeyState* pKey))

IPPAPI(IppStatus, ippsRSA_GetBufferSizePublicKey,(int* pBufferSize, const IppsRSAPublicKeyState* pKey))
IPPAPI(IppStatus, ippsRSA_GetBufferSizePrivateKey,(int* pBufferSize, const IppsRSAPrivateKeyState* pKey))

/* signature scheme : RSA-SSA-PKCS1-v1_5 */
IPPAPI(IppStatus, ippsRSASign_PKCS1v15_rmf,(const Ipp8u* pMsg, int msgLen,
                                                  Ipp8u* pSign,
                                            const IppsRSAPrivateKeyState* pPrvKey,
                                            const IppsRSAPublicKeyState*  pPubKey,
                                            const IppsHashMethod* pMethod,
                                                  Ipp8u* pBuffer))

IPPAPI(IppStatus, ippsRSAVerify_PKCS1v15_rmf,(const Ipp8u* pMsg, int msgLen,
                                              const Ipp8u* pSign, int* pIsValid,
                                              const IppsRSAPublicKeyState* pKey,
                                              const IppsHashMethod* pMethod,
                                                    Ipp8u* pBuffer))

IPPAPI(IppStatus, ippsRSAVerifyHash_PKCS1v15_rmf,(const Ipp8u* md,
                                                  const Ipp8u* pSign, int* pIsValid,
                                                  const IppsRSAPublicKeyState* pKey,
                                                  const IppsHashMethod* pMethod,
                                                        Ipp8u* pBuffer))

IPPAPI(IppStatus, ippsRSAVerify_PSS_rmf,(const Ipp8u* pMsg,  int msgLen,
                                         const Ipp8u* pSign,
                                          int* pIsValid,
                                         const IppsRSAPublicKeyState*  pKey,
                                         const IppsHashMethod* pMethod,
                                               Ipp8u* pBuffer))
#ifdef  __cplusplus
}
#endif


#if defined (_IPP_STDCALL_CDECL)
  #undef  _IPP_STDCALL_CDECL
  #define __stdcall __cdecl
#endif

#endif /* IPPCP_H__ */
