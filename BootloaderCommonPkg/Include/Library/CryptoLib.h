/** @file
  Provides sha256 and RSA2048 verify functions.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef __CRYPTO_LIB_H__
#define __CRYPTO_LIB_H__

#include <Uefi/UefiBaseType.h>

#define RSA_KEY_VB_SIGNATURE     SIGNATURE_32 ('$', '_', 'V', 'B')
#define RSA_KEY_IPP_SIGNATURE    SIGNATURE_32 ('$', 'I', 'P', 'P')
#define PUBKEY_IDENTIFIER        SIGNATURE_32 ('P', 'U', 'B', 'K')
#define SIGNATURE_IDENTIFIER     SIGNATURE_32 ('S', 'I', 'G', 'N')


#define SIG_TYPE_RSA2048SHA256   0
#define SIG_TYPE_RSA3072SHA384   1

typedef UINT8 SIGN_TYPE;
#define SIGNING_TYPE_RSA_PKCS_1_5     1
#define SIGNING_TYPE_RSA_PSS          2

typedef UINT8 KEY_TYPE;
#define KEY_TYPE_RSA                  1

#define RSA2048_NUMBYTES              256
#define RSA3072_NUMBYTES              384

#define RSA_MOD_SIZE 256 //hardcode n size to be 256
#define RSA_E_SIZE   4   //hardcode e size to be 4

typedef UINT8 HASH_ALG_TYPE;
#define  HASH_TYPE_SHA256              1
#define  HASH_TYPE_SHA384              2
#define  HASH_TYPE_SHA512              3
#define  HASH_TYPE_SM3                 4

#define SHA256_DIGEST_SIZE       32
#define SHA384_DIGEST_SIZE       48
#define SM3_DIGEST_SIZE          32
#define SHA512_DIGEST_SIZE       64
#define HASH_DIGEST_MAX          SHA512_DIGEST_SIZE

#define RSA2048NUMBYTES          256
#define RSA3072NUMBYTES          384

#define IPP_HASH_CTX_SIZE  256   //IPP Hash context size

#define IPP_HASHLIB_SHA1         0x0001
#define IPP_HASHLIB_SHA2_256     0x0002
#define IPP_HASHLIB_SHA2_384     0x0004
#define IPP_HASHLIB_SHA2_512     0x0008
#define IPP_HASHLIB_SM3_256      0x0010


typedef UINT8 HASH_CTX[IPP_HASH_CTX_SIZE];   //IPP Hash context buffer


typedef struct {
  //signature ('P', 'U', 'B', 'K')
  UINT32                   Identifier;

  //Length of Public Key
  UINT16                   KeySize;

  //KeyType RSA or ECC
  KEY_TYPE                 KeyType;

  UINT8                    Rsvd;

  //Pubic key data with KeySize bytes
  //Contains Modulus(256/384 sized) and PubExp[4]
  UINT8                    KeyData[0];
} PUB_KEY_HDR;

typedef struct {
  //signature Identifier('S', 'I', 'G', 'N')
  UINT32                   Identifier;

  //Length of signature 2K and 3K in bytes
  UINT16                   SigSize;

 //PKCSv1.5 or RSA-PSS or ECC
  SIGN_TYPE                SigType;

  //Hash Alg for signingh SHA256, SHA384
  HASH_ALG_TYPE            HashAlg;

  //Signature length defined by SigSize bytes
  UINT8                    Signature[0];
} SIGNATURE_HDR;


#define RSA_SIGNATURE_SIZE         (RSA2048_NUMBYTES + sizeof(SIGNATURE_HDR))
#define RSA_KEYSIZE_SIZE           (RSA_MOD_SIZE + RSA_E_SIZE + sizeof(PUB_KEY_HDR))
#define RSA_SIGNATURE_AND_KEY_SIZE  (RSA_SIGNATURE_SIZE + RSA_KEYSIZE_SIZE)

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
  Computes the SHA-384 message digest of a input data buffer.

  This function performs the SHA-384 message digest of a given data buffer, and places
  the digest value into the specified memory.

  @param[in]   Data        Pointer to the buffer containing the data to be hashed.
  @param[in]   Length      Length of Data buffer in bytes.
  @param[out]  Digest      Pointer to a buffer that receives the SHA-384 digest
                           value (48 bytes).

  @retval                  A pointer to SHA-384 digest value
**/
UINT8 *
Sha384 (
  IN  CONST UINT8          *Data,
  IN        UINT32          Length,
  OUT       UINT8          *Digest
  );

/**
  Computes the SM3 message digest of a input data buffer.

  This function performs the SM3 message digest of a given data buffer, and places
  the digest value into the specified memory.

  @param[in]   Data        Pointer to the buffer containing the data to be hashed.
  @param[in]   Length      Length of Data buffer in bytes.
  @param[out]  Digest      Pointer to a buffer that receives the SM3 digest
                           value (32 bytes).

  @retval                  A pointer to SM3 digest value
**/
UINT8 *
Sm3 (
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
RsaVerify_Pkcs_1_5 (
  CONST PUB_KEY_HDR        *PubKeyHdr,
  CONST SIGNATURE_HDR      *SignatureHdr,
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


/**
  Initializes the hash context for SHA384 hashing.

  @param[in]   HashCtx       Pointer to the hash context buffer.
  @param[in]   HashCtxSize   Length of the hash context.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_BUFFER_TOO_SMALL    Hash context buffer size is not large enough.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
Sha384Init (
  IN      HASH_CTX   *HashCtx,
  IN      UINT32      HashCtxSize
  );

/**
  Consumes the data for SHA384 hashing.
  This method can be called multiple times to hash separate pieces of data.

  @param[in]   HashCtx     Pointer to the hash context buffer.
  @param[in]   Msg         Data to be hashed.
  @param[in]   MsgLen      Length of data to be hashed.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
Sha384Update (
  IN        HASH_CTX   *HashCtx,
  IN CONST  UINT8      *Msg,
  IN        UINT32      MsgLen
  );

/**
  Finalizes the SHA384 hashing and returns the hash.

  @param[in]   HashCtx     Pointer to the hash context buffer.
  @param[out]  Hash        Sha384 hash of the data.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
Sha384Final (
  IN       HASH_CTX   *HashCtx,
  OUT      UINT8      *Hash
  );

/**
  Initializes the hash context for SM3 hashing.

  @param[in]   HashCtx       Pointer to the hash context buffer.
  @param[in]   HashCtxSize   Length of the hash context.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_BUFFER_TOO_SMALL    Hash context buffer size is not large enough.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
Sm3Init (
  IN      HASH_CTX   *HashCtx,
  IN      UINT32      HashCtxSize
  );

/**
  Consumes the data for SM3 hashing.
  This method can be called multiple times to hash separate pieces of data.

  @param[in]   HashCtx     Pointer to the hash context buffer.
  @param[in]   Msg         Data to be hashed.
  @param[in]   MsgLen      Length of data to be hashed.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
Sm3Update (
  IN        HASH_CTX   *HashCtx,
  IN CONST  UINT8      *Msg,
  IN        UINT32      MsgLen
  );

/**
  Finalizes the SM3 hashing and returns the hash.

  @param[in]   HashCtx     Pointer to the hash context buffer.
  @param[out]  Hash        Sm3 hash of the data.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
Sm3Final (
  IN       HASH_CTX   *HashCtx,
  OUT      UINT8      *Hash
  );

#endif

