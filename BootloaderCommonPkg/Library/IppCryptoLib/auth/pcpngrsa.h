/*******************************************************************************
* Copyright 2013-2020 Intel Corporation
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
//     Internal Definitions and
//     Internal ng RSA Function Prototypes
//
//
*/

#if !defined(_CP_NG_RSA_H)
#define _CP_NG_RSA_H

#include "pcpbn.h"
#include "pcpmontgomery.h"

struct _cpRSA_public_key {
   IppCtxId       id;            /* key ID */
   int         maxbitSizeN;
   int         maxbitSizeE;
   int            bitSizeN;      /* RSA modulus bitsize */
   int            bitSizeE;      /* RSA public exp bitsize */

   BNU_CHUNK_T*   pDataE;        /* public exp */
   gsModEngine*   pMontN;        /* montgomery engine (N) */
};

/* access */
#define RSA_PUB_KEY_MAXSIZE_N(x) ((x)->maxbitSizeN)
#define RSA_PUB_KEY_MAXSIZE_E(x) ((x)->maxbitSizeE)
#define RSA_PUB_KEY_ID(x)        ((x)->id)
#define RSA_PUB_KEY_BITSIZE_N(x) ((x)->bitSizeN)
#define RSA_PUB_KEY_BITSIZE_E(x) ((x)->bitSizeE)
#define RSA_PUB_KEY_E(x)         ((x)->pDataE)
#define RSA_PUB_KEY_NMONT(x)     ((x)->pMontN)
#define RSA_PUB_KEY_VALID_ID(x)  (RSA_PUB_KEY_ID((x))==idCtxRSA_PubKey)
#define RSA_PUB_KEY_IS_SET(x)    (RSA_PUB_KEY_BITSIZE_N((x))>0)

/* alignment */
#define RSA_PUBLIC_KEY_ALIGNMENT ((int)(sizeof(void*)))

struct _cpRSA_private_key {
   IppCtxId       id;            /* key ID */
   int         maxbitSizeN;
   int         maxbitSizeD;
   int            bitSizeN;      /* RSA modulus bitsize */
   int            bitSizeD;      /* RSA private exp bitsize */
   int            bitSizeP;      /* RSA p-factor bitsize */
   int            bitSizeQ;      /* RSA q-factor bitsize */

   BNU_CHUNK_T*   pDataD;        /* private exp */
   BNU_CHUNK_T*   pDataDp;       /* dp private exp */
   BNU_CHUNK_T*   pDataDq;       /* dq private exp */
   BNU_CHUNK_T*   pDataQinv;     /* qinv coeff in residue domain */

   gsModEngine* pMontP;          /* montgomery engine (P) */
   gsModEngine* pMontQ;          /* montgomery engine (Q) */
   gsModEngine* pMontN;          /* montgomery engine (N) */
};

/* access */
#define RSA_PRV_KEY_MAXSIZE_N(x) ((x)->maxbitSizeN)
#define RSA_PRV_KEY_MAXSIZE_D(x) ((x)->maxbitSizeD)
#define RSA_PRV_KEY_ID(x)        ((x)->id)
#define RSA_PRV_KEY_BITSIZE_N(x) ((x)->bitSizeN)
#define RSA_PRV_KEY_BITSIZE_D(x) ((x)->bitSizeD)
#define RSA_PRV_KEY_BITSIZE_P(x) ((x)->bitSizeP)
#define RSA_PRV_KEY_BITSIZE_Q(x) ((x)->bitSizeQ)
#define RSA_PRV_KEY_D(x)         ((x)->pDataD)
#define RSA_PRV_KEY_DP(x)        ((x)->pDataDp)
#define RSA_PRV_KEY_DQ(x)        ((x)->pDataDq)
#define RSA_PRV_KEY_INVQ(x)      ((x)->pDataQinv)
#define RSA_PRV_KEY_PMONT(x)     ((x)->pMontP)
#define RSA_PRV_KEY_QMONT(x)     ((x)->pMontQ)
#define RSA_PRV_KEY_NMONT(x)     ((x)->pMontN)
#define RSA_PRV_KEY1_VALID_ID(x) (RSA_PRV_KEY_ID((x))==idCtxRSA_PrvKey1)
#define RSA_PRV_KEY2_VALID_ID(x) (RSA_PRV_KEY_ID((x))==idCtxRSA_PrvKey2)
#define RSA_PRV_KEY_VALID_ID(x)  (RSA_PRV_KEY1_VALID_ID((x)) || RSA_PRV_KEY2_VALID_ID((x)))
#define RSA_PRV_KEY_IS_SET(x)    (RSA_PRV_KEY_BITSIZE_N((x))>0)

/* alignment */
#define RSA_PRIVATE_KEY_ALIGNMENT ((int)(sizeof(void*)))

#define MOD_ENGINE_RSA_POOL_SIZE    (2)

/*
// Montgomery engine preparation (GetSize/init/Set)
*/
#define rsaMontExpGetSize OWNAPI(rsaMontExpGetSize)
void rsaMontExpGetSize(int length, int* pSize);

/*
// pubic and private key operations
*/
#define gsRSApub_cipher OWNAPI(gsRSApub_cipher)
void    gsRSApub_cipher(IppsBigNumState* pY, const IppsBigNumState* pX, const IppsRSAPublicKeyState* pKey, BNU_CHUNK_T* pScratchBuffer);

#define gsRSAprv_cipher OWNAPI(gsRSAprv_cipher)
void    gsRSAprv_cipher(IppsBigNumState* pY, const IppsBigNumState* pX, const IppsRSAPrivateKeyState* pKey, BNU_CHUNK_T* pScratchBuffer);

#define gsRSAprv_cipher_crt OWNAPI(gsRSAprv_cipher_crt)
void    gsRSAprv_cipher_crt(IppsBigNumState* pY, const IppsBigNumState* pX, const IppsRSAPrivateKeyState* pKey, BNU_CHUNK_T* pScratchBuffer);


/* declaration of RSA exponentiation */
typedef cpSize (*ngPublicBufNum)(int modulusBits);
typedef cpSize (*ngPrivateBufNum)(int modulusBits);

typedef cpSize (*ngMontExp)(BNU_CHUNK_T* dataY,
                      const BNU_CHUNK_T* dataX, cpSize nsX,
                      const BNU_CHUNK_T* dataE, cpSize nsE,
                            gsModEngine* pMont,
                            BNU_CHUNK_T* pBuffer);

typedef struct _gsMethod_RSA {
   int loModulusBisize;          // application area (lowew
   int hiModulusBisize;          // and upper)

   ngPublicBufNum publicBufNum;  // pub operation buffer in BNU_CHUNK_T
   ngPrivateBufNum privateBufNum;// prv operation buffer in BNU_CHUNK_T

   ngMontExp publicExpFun;       // public key exponentiation
   ngMontExp privateExpFun;      // private key exponentiation
} gsMethod_RSA;

#endif /* _CP_NG_RSA_H */
