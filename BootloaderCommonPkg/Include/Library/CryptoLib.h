/** @file
  Provides sha256 and RSA2048 verify functions.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#ifndef __CRYPTO_LIB_H__
#define __CRYPTO_LIB_H__

#include <Uefi/UefiBaseType.h>

#define RSA_KEY_VB_SIGNATURE     SIGNATURE_32 ('$', '_', 'V', 'B')
#define RSA_KEY_IPP_SIGNATURE    SIGNATURE_32 ('$', 'I', 'P', 'P')

#define SIG_TYPE_RSA2048SHA256   0

#define SHA256_DIGEST_SIZE       32
#define RSA2048NUMBYTES          256

#define RSA_MOD_SIZE 256 //hardcode n size to be 256
#define RSA_E_SIZE   4   //hardcode e size to be 4

#define RSA_SIGNATURE_AND_KEY_SIZE  (RSA2048NUMBYTES + RSA_MOD_SIZE + RSA_E_SIZE + sizeof(UINT32))

#define IPP_HASH_CTX_SIZE  256   //IPP Hash context size

typedef UINT8 HASH_CTX[IPP_HASH_CTX_SIZE];   //IPP Hash context buffer

//
// RSA verify required key parameter format
//
typedef struct {
  UINT32                   Signature;
  UINT8                    PubKeyData[0];
} RSA_PUB_KEY;

/**
  Computes the SHA-256 message digest of a input data buffer.

  This function performs the SHA-256 message digest of a given data buffer, and places
  the digest value into the specified memory.

  @param[in]   Data        Pointer to the buffer containing the data to be hashed.
  @param[in]   Length      Length of Data buffer in bytes.
  @param[out]  Digest      Pointer to a buffer that receives the SHA-256 digest
                           value (32 bytes).

  @retval                  A pointer to SHA-256 digest value
**/
UINT8 *
Sha256 (
  IN  CONST UINT8          *Data,
  IN        UINT32          Length,
  OUT       UINT8          *Digest
  );

/**
  Verifies the RSA signature with PKCS1-v1_5 encoding scheme defined in RSA PKCS#1.

  @param[in]  PubKey         Pointer to a pre-processed RSA key data.
  @param[in]  Signature      Pointer to RSA PKCS1-v1_5 signature to be verified.
  @param[in]  SignatureLen   Length of the signature in bytes.
  @param[in]  SignatureType  Now only support signature type SIG_TYPE_RSA2048SHA256.
  @param[in]  Hash           Pointer to octet message hash to be checked.

  @retval  RETURN_SUCCESS             Valid signature.
  @retval  RETURN_INVALID_PARAMETER   Key or signature format is incorrect.
  @retval  RETURN_SECURITY_VIOLATION  Invalid signature.

**/
RETURN_STATUS
RsaVerify (
  CONST RSA_PUB_KEY        *PubKey,
  CONST UINT8              *Signature,
  CONST UINT32              SignatureLen,
  CONST UINT8               SignatureType,
  CONST UINT8              *Hash
  );

/**
  Computes the HMAC SHA-256 message digest of a input data buffer.

  This function performs the HMAC SHA-256 message digest of a given data buffer, and places
  the digest value into the specified memory.

  @param[in]   Msg         Pointer to the buffer containing the data to be hashed.
  @param[in]   MsgLen      Length of Data buffer in bytes.
  @param[in]   Key         Pointer to the secret key.
  @param[in]   KeyLen      Length of the secret key.
  @param[out]  Hmac        Pointer to a buffer that receives the HMAC SHA-256 digest
                           value (32 bytes).
  @param[in]   HmacLen     Length of the HMAC SHA-256 buffer in bytes.

  @retval   EFI_SUCCESS                 Successfully calculated HMAC SHA256
  @retval   EFI_PROTOCOL_ERROR          Error caused while calculating HMAC SHA256
**/
EFI_STATUS
HmacSha256 (
  IN  CONST UINT8          *Msg,
  IN        UINT32          MsgLen,
  IN  CONST UINT8          *Key,
  IN        UINT32          KeyLen,
  OUT       UINT8          *Hmac,
  IN        UINT32          HmacLen
  );

/**
  Wrapper function for HMAC HKDF logic

  HKDF-Extract(salt, IKM) -> PRK as Ouput
  HKDF-Expand(PRK, info, L) -> OKM as Output
  Method used: SHA-256

  @param[in]   Salt         Pointer to the Salt/Nonce data.
  @param[in]   SaltLen      Length of the Salt/Nonce.
  @param[in]   Ikm          Input Keying Material. This is the pointer to the secret Key.
  @param[in]   IkmLen       Length of the secret key.
  @param[in]   Info         Pointer to the msg/info to be used.
  @param[in]   InfoLen      Length of the msg.
  @param[out]  Okm          Output Keying Material. Pointer to the final key that is derived using HKDF algo.
  @param[in]   OkmLen       Length of the OKM buffer in bytes.

  Returns non-zero on failure, 0 on success.
**/
EFI_STATUS
HkdfExtractExpand (
  IN CONST UINT8 *Salt,
  IN INT32 SaltLen,
  IN CONST UINT8 *Ikm,
  IN INT32 IkmLen,
  IN CONST UINT8* Info,
  IN INT32 InfoLen,
  OUT UINT8 *Okm,
  IN INT32 OkmLen
  );

/**
  Initializes the hash context for SHA256 hashing.

  @param[in]   HashCtx       Pointer to the hash context buffer.
  @param[in]   HashCtxSize   Length of the hash context.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_BUFFER_TOO_SMALL    Hash context buffer size is not large enough.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
Sha256Init (
  IN      HASH_CTX   *HashCtx,
  IN      UINT32      HashCtxSize
  );

/**
  Consumes the data for SHA256 hashing.
  This method can be called multiple times to hash separate pieces of data.

  @param[in]   HashCtx     Pointer to the hash context buffer.
  @param[in]   Msg         Data to be hashed.
  @param[in]   MsgLen      Length of data to be hashed.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
Sha256Update (
  IN        HASH_CTX   *HashCtx,
  IN CONST  UINT8      *Msg,
  IN        UINT32      MsgLen
  );

/**
  Finalizes the SHA256 hashing and returns the hash.

  @param[in]   HashCtx     Pointer to the hash context buffer.
  @param[out]  Hash        Sha256 hash of the data.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
Sha256Final (
  IN       HASH_CTX   *HashCtx,
  OUT      UINT8      *Hash
  );

#endif

