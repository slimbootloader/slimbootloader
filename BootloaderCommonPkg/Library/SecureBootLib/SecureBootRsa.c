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
  IN       UINT8           *PubKeyHash      OPTIONAL,
  OUT      UINT8           *OutHash         OPTIONAL
  )
{
  UINTN            Index;
  RETURN_STATUS    Status;
  UINT8           *TmpPubKey;
  RSA_PUB_KEY     *InpPubKey;
  UINT8            Digest[SHA256_DIGEST_SIZE];
  UINT8            PubKeyBuf[RSA_MOD_SIZE + RSA_E_SIZE];

  // Verify public key first
  // Hash caculation for key uses different endian
  InpPubKey = (RSA_PUB_KEY *)PubKey;
  TmpPubKey = PubKeyBuf;
  for (Index = 0; Index < RSA_MOD_SIZE; Index++) {
    TmpPubKey[Index] = InpPubKey->PubKeyData[RSA_MOD_SIZE - 1 - Index];
  }
  for (Index = 0; Index < RSA_E_SIZE; Index++) {
    TmpPubKey[RSA_MOD_SIZE + Index] = InpPubKey->PubKeyData[RSA_MOD_SIZE + RSA_E_SIZE - 1 - Index];
  }

  Status = DoHashVerify ((CONST UINT8 *)PubKeyBuf, RSA_MOD_SIZE + RSA_E_SIZE, HASH_TYPE_SHA256, ComponentType, PubKeyHash);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  // Verify payload data
  Sha256 (Data, Length, Digest);

  if (OutHash != NULL) {
    CopyMem (OutHash, Digest, sizeof (Digest));
  }

  Status = RsaVerify ((RSA_PUB_KEY *)PubKey, Signature, RSA2048NUMBYTES, SIG_TYPE_RSA2048SHA256, Digest);
  if (RETURN_ERROR (Status)) {
    DEBUG_CODE_BEGIN();

    DEBUG ((DEBUG_INFO, "RSA Verification Failed!\n"));

    DEBUG ((DEBUG_INFO, "First 32Bytes Input Data\n"));
    DumpHex (2, 0, SHA256_DIGEST_SIZE, (VOID *)Data);

    DEBUG ((DEBUG_INFO, "Last 32Bytes Input Data\n"));
    DumpHex (2, 0, SHA256_DIGEST_SIZE, (VOID *) (Data + Length - 32));

    DEBUG ((DEBUG_INFO, "Image Digest\n"));
    DumpHex (2, 0, SHA256_DIGEST_SIZE, (VOID *)Digest);

    DEBUG ((DEBUG_INFO, "Signature\n"));
    DumpHex (2, 0, RSA2048NUMBYTES, (VOID *)Signature);

    DEBUG ((DEBUG_INFO, "Public Key\n"));
    DumpHex (2, 0, RSA_MOD_SIZE + RSA_E_SIZE + sizeof (UINT32), PubKey);

    DEBUG_CODE_END();

  } else {
    DEBUG ((DEBUG_INFO, "RSA Verification Success!\n"));
  }

  return Status;
}
