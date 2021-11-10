/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __VERIFIED_BOOT_LIB_H__
#define __VERIFIED_BOOT_LIB_H__

#include <Guid/KeyHashGuid.h>
#include <Library/CryptoLib.h>

#define  SIG_TYPE_RSA2048_SHA256       0
#define  SIG_TYPE_RSA3072_SHA384       1

/**
  Get hash to extend a firmware stage component
  Hash calculation to extend would be in either of ways
  Retrieve Hash from Component hash table or
  Calculate Hash using source buf and length provided

  @param[in]  ComponentType             Stage whose measurement need to be extended.
  @param[in]  HashType                  Hash type required
  @param[in]  Src                       Buffer Address
  @param[in]  Length                    Data Len
  @param[out] HashData                  Hash Data buf addr

  @retval RETURN_SUCCESS      Operation completed successfully.
  @retval Others              Unable to calcuate hash.
**/
RETURN_STATUS
EFIAPI
GetHashToExtend (
  IN       UINT8            ComponentType,
  IN       HASH_ALG_TYPE    HashType,
  IN       UINT8           *Src,
  IN       UINT32           Length,
  OUT      UINT8           *HashData
  );

/**
  Calculate hash API.

  @param[in]  Data           Data buffer pointer.
  @param[in]  Length         Data buffer size.
  @param[in]  HashAlg        Specify hash algrothsm.
  @param[in,out]  OutHash    Hash of Data buffer


  @retval RETURN_SUCCESS             Hash Calculation succeeded.
  @retval RETRUN_INVALID_PARAMETER   Hash parameter is not valid.
  @retval RETURN_UNSUPPORTED         Hash Alg type is not supported.

**/
RETURN_STATUS
EFIAPI
CalculateHash  (
  IN CONST UINT8          *Data,
  IN       UINT32          Length,
  IN       UINT8           HashAlg,
  IN OUT   UINT8          *OutHash
  );

/**
  Verify data block hash with the built-in one.

  @param[in]  Data           Data buffer pointer.
  @param[in]  Length         Data buffer size.
  @param[in]  Usage          Hash component usage.
  @param[in]  HashAlg        Specify hash algorithm.
  @param[in,out]  Hash       On input,  expected hash value when hash component usage is 0.
                             On output, calculated hash value when verification succeeds.

  @retval RETURN_SUCCESS             Hash verification succeeded.
  @retval RETURN_INVALID_PARAMETER   Hash parameter is not valid.
  @retval RETURN_NOT_FOUND           Hash data for hash component usage is not found.
  @retval RETURN_UNSUPPORTED         HashAlg not supported.
  @retval RETURN_SECURITY_VIOLATION  Hash verification failed.

**/
RETURN_STATUS
EFIAPI
DoHashVerify (
  IN CONST UINT8           *Data,
  IN       UINT32           Length,
  IN       HASH_COMP_USAGE  Usage,
  IN       UINT8            HashAlg,
  IN OUT   UINT8           *Hash
  );

/**
  Verifies the RSA signature with PKCS1-v1_5 encoding scheme defined in RSA PKCS#1.
  Also(optional), return the hash of the message to the caller.

  @param[in]  Data            Data buffer pointer.
  @param[in]  Length          Data buffer size.
  @param[in]  Usage           Hash usage.
  @param[in]  Signature       Signature header for singanture data.
  @param[in]  PubKeyHdr       Public key header for key data
  @param[in]  PubKeyHashAlg   Hash Alg for PubKeyHash.
  @param[in]  PubKeyHash      Public key hash value when hash component usage is 0.
  @param[out] OutHash         Calculated data hash value.


  @retval RETURN_SUCCESS             RSA verification succeeded.
  @retval RETURN_NOT_FOUND           Hash data for hash component usage is not found.
  @retval RETURN_UNSUPPORTED         Hash alg type is not supported.
  @retval RETURN_SECURITY_VIOLATION  PubKey or Signature verification failed.

**/
RETURN_STATUS
EFIAPI
DoRsaVerify (
  IN CONST UINT8           *Data,
  IN       UINT32           Length,
  IN       HASH_COMP_USAGE  Usage,
  IN CONST SIGNATURE_HDR   *SignatureHdr,
  IN       PUB_KEY_HDR     *PubKeyHdr,
  IN       UINT8            PubKeyHashAlg,
  IN       UINT8           *PubKeyHash      OPTIONAL,
  OUT      UINT8           *OutHash         OPTIONAL
  );

/**
  Generate RandomNumbers.

  @param[out] Data                   Array of random numbers generated
  @param[in] Size                    Size of the array to be generated.

  @retval EFI_SUCCESS                Random number generation successful.
  @retval EFI_UNSUPPORTED            Couldn't generate a random number.
**/
RETURN_STATUS
EFIAPI
GenerateRandomNumbers(
  OUT   CHAR8   *Data,
  IN    UINTN   Size
  );
#endif /* __VERIFIED_BOOT_LIB_H__ */
