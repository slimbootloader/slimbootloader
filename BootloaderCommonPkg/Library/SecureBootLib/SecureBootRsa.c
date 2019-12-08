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
  @param[in]  Signature       Signature header for singanture data.
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
  )
{
  RETURN_STATUS    Status;
  PUB_KEY_HDR     *PublicKey;
  UINT8            Digest[SHA256_DIGEST_SIZE];

  PublicKey = PubKeyHdr;
  if ((PublicKey->Identifier != PUBKEY_IDENTIFIER) || (SignatureHdr->Identifier != SIGNATURE_IDENTIFIER)){
    return RETURN_INVALID_PARAMETER;
  }

  // Verify public key first
  Status = DoHashVerify (PublicKey->KeyData, RSA_MOD_SIZE + RSA_E_SIZE, HASH_TYPE_SHA256, ComponentType, PubKeyHash);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  // Verify payload data
  Sha256 (Data, Length, Digest);

  if (OutHash != NULL) {
    CopyMem (OutHash, Digest, sizeof (Digest));
  }

  if(SignatureHdr->SigType == SIGNING_TYPE_RSA_PKCS_1_5) {

    Status = RsaVerify_Pkcs_1_5 (PublicKey, SignatureHdr, Digest);
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
      DumpHex (2, 0, RSA2048_NUMBYTES, (VOID *)(SignatureHdr->Signature));

      DEBUG ((DEBUG_INFO, "Public Key\n"));
      DumpHex (2, 0, RSA_MOD_SIZE + RSA_E_SIZE + sizeof (UINT32), PubKeyHdr->KeyData);

      DEBUG_CODE_END();

    } else {
      DEBUG ((DEBUG_INFO, "RSA Verification Success!\n"));
    }
  } else {
    return RETURN_UNSUPPORTED;
  }

  return Status;
}
