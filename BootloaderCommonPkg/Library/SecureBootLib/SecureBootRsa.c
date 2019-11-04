/** @file
  Secure boot library routines to provide RSA signature verification.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CryptoLib.h>
#include <Library/SecureBootLib.h>
#include <Library/BootloaderCommonLib.h>

/**
  Verifies the RSA signature with PKCS1-v1_5 encoding scheme defined in RSA PKCS#1.
  Also(optional), return the hash of the message to the caller.

  @param[in]  Data            Data buffer pointer.
  @param[in]  Length          Data buffer size.
  @param[in]  ComponentType   Component type.
  @param[in]  Signature       Signature for the data buffer.
  @param[in]  PubKey          Public key data pointer.
  @param[in]  PubKeyHash      Public key hash value when ComponentType is not used.
  @param[in]  PubKeyHashAlg   Pubkey Hash Alg .
  @param[in]  SignKeyType     Type of Public key Sign .
  @param[in]  SignHashAlg     Hash Alg used for signing.
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
  IN CONST UINT8           *Signature,
  IN       UINT8           *PubKey,
  IN       UINT8           *PubKeyHash,
  IN       UINT8            PubKeyHashAlg,
  IN       UINT8            SignKeyType,
  IN       UINT8            SignHashAlg,
  OUT      UINT8           *OutHash         OPTIONAL
  )
{
  UINTN            Index;
  RETURN_STATUS    Status;
  UINT8           *TmpPubKey;
  RSA_PUB_KEY     *InpPubKey;
  UINT8            Digest[HASH_DIGEST_MAX]; //Set max size
  UINT8            PubKeyBuf[RSA_MOD_SIZE_MAX + RSA_E_SIZE];
  UINT8            DigestSize;
  UINT16           RsaModSz=0;

  // Verify public key first
  // Hash caculation for key uses different endian
  if(SignKeyType == SIG_TYPE_RSA2048){
    RsaModSz = RSA_MOD_SIZE_2048;
    DEBUG ((DEBUG_INFO, "RSA_MOD_SIZE_2048!\n"));
  } else if (SignKeyType == SIG_TYPE_RSA3072) {
    RsaModSz = RSA_MOD_SIZE_3072;
    DEBUG ((DEBUG_INFO, "RSA_MOD_SIZE_3072!\n"));
  } else {
    return RETURN_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "SignHashAlg 0x%x!\n", SignHashAlg));

  InpPubKey = (RSA_PUB_KEY *)PubKey;
  TmpPubKey = PubKeyBuf;
  for (Index = 0; Index < RsaModSz; Index++) {
    TmpPubKey[Index] = InpPubKey->PubKeyData[RsaModSz - 1 - Index];
  }
  for (Index = 0; Index < RSA_E_SIZE; Index++) {
    TmpPubKey[RsaModSz + Index] = InpPubKey->PubKeyData[RsaModSz + RSA_E_SIZE - 1 - Index];
  }

  Status = DoHashVerify ((CONST UINT8 *)PubKeyBuf, RsaModSz + RSA_E_SIZE, PubKeyHashAlg, ComponentType, PubKeyHash);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  // Verify payload data
  if (SignHashAlg == HASH_TYPE_SHA256) {
      DEBUG ((DEBUG_INFO, "SHA256 Sign Hash!\n"));
      Sha256 (Data, Length, Digest);
      DigestSize = SHA256_DIGEST_SIZE;
  } else {
    return RETURN_INVALID_PARAMETER;
  }

  if(OutHash != NULL){
    CopyMem (OutHash, Digest, DigestSize);
  }

  Status = RsaVerify ((RSA_PUB_KEY *)PubKey, Signature, RSA2048NUMBYTES, SignKeyType, SignHashAlg, Digest);
  if (RETURN_ERROR (Status)) {
    DEBUG_CODE_BEGIN();

    DEBUG ((DEBUG_INFO, "RSA Verification Failed!\n"));

    DEBUG ((DEBUG_INFO, "First %d Bytes Input Data\n", DigestSize));
    DumpHex (2, 0, DigestSize, (VOID *)Data);

    DEBUG ((DEBUG_INFO, "Last %d Bytes Input Data\n", DigestSize));
    DumpHex (2, 0, DigestSize, (VOID *) (Data + Length - 32));

    DEBUG ((DEBUG_INFO, "Image Digest\n"));
    DumpHex (2, 0, DigestSize, (VOID *)Digest);

    DEBUG ((DEBUG_INFO, "Signature\n"));
    DumpHex (2, 0, RSA2048NUMBYTES, (VOID *)Signature);

    DEBUG ((DEBUG_INFO, "Public Key\n"));
    DumpHex (2, 0, RsaModSz + RSA_E_SIZE + sizeof (UINT32), PubKey);

    DEBUG_CODE_END();

  } else {
    DEBUG ((DEBUG_INFO, "RSA Verification Success!\n"));
  }

  return Status;
}
