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
  Verify data block hash with the built-in one.

  @param[in]  Data           Data buffer pointer.
  @param[in]  Length         Data buffer size.
  @param[in]  HashAlg        Specify hash algorithm.
  @param[in]  ComponentType  Component type.
  @param[in,out]  Hash       On input,  expected hash value when ComponentType is not used.
                             On output, calculated hash value when verification succeeds.

  @retval RETURN_SUCCESS             Hash verification succeeded.
  @retval RETRUN_INVALID_PARAMETER   Hash parameter is not valid.
  @retval RETURN_NOT_FOUND           Hash data for ComponentType is not found.
  @retval RETURN_UNSUPPORTED         Hash component type is not supported.
  @retval RETURN_SECURITY_VIOLATION  Hash verification failed.

**/
RETURN_STATUS
DoHashVerify (
  IN CONST UINT8           *Data,
  IN       UINT32           Length,
  IN       UINT8            HashAlg,
  IN       UINT8            ComponentType,
  IN OUT   UINT8           *Hash
  );

/**
  Verifies the RSA signature with PKCS1-v1_5 encoding scheme defined in RSA PKCS#1.
  Also(optional), return the hash of the message to the caller.

  @param[in]  Data            Data buffer pointer.
  @param[in]  Length          Data buffer size.
  @param[in]  ComponentType   Component type.
  @param[in]  SignatureHdr    Signature header for singanture data.
  @param[in]  PubKeyHdr       Public key header for key data
  @param[in]  PubKeyHash      Public key hash value when ComponentType is not used.
  @param[out] OutHash         Calculated data hash value.


  @retval RETURN_SUCCESS             RSA verification succeeded.
  @retval RETURN_NOT_FOUND           Hash data for ComponentType is not found.
  @retval RETURN_UNSUPPORTED         Hash component type is not supported.
  @retval RETURN_SECURITY_VIOLATION  PubKey or Signature verification failed.

**/
RETURN_STATUS
DoRsaVerify (
  IN CONST UINT8           *Data,
  IN       UINT32           Length,
  IN       UINT8            ComponentType,
  IN CONST SIGNATURE_HDR   *SignatureHdr,
  IN       PUB_KEY_HDR     *PubKeyHdr,
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
