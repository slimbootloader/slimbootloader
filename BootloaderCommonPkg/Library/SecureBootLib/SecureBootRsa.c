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
  @param[in]  Usage           Hash usage.
  @param[in]  Signature       Signature header for singanture data.
  @param[in]  PubKeyHdr       Public key header for key data
  @param[in]  PubKeyHashAlg   Hash Alg for PubKeyHash.
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
  IN       HASH_COMP_USAGE  Usage,
  IN CONST SIGNATURE_HDR   *SignatureHdr,
  IN       PUB_KEY_HDR     *PubKeyHdr,
  IN       UINT8            PubKeyHashAlg,
  IN       UINT8           *PubKeyHash      OPTIONAL,
  OUT      UINT8           *OutHash         OPTIONAL
  )
{
  RETURN_STATUS    Status;
  PUB_KEY_HDR     *PublicKey;
  UINT8            Digest[HASH_DIGEST_MAX];
  UINT8            DigestSize;


  PublicKey = PubKeyHdr;
  if ((PublicKey->Identifier != PUBKEY_IDENTIFIER) || (SignatureHdr->Identifier != SIGNATURE_IDENTIFIER)){
    return RETURN_INVALID_PARAMETER;
  }

  // Verify public key first
  Status = DoHashVerify (PublicKey->KeyData, PublicKey->KeySize, Usage, PubKeyHashAlg, PubKeyHash);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  // Verify payload data
  if (SignatureHdr->HashAlg == HASH_TYPE_SHA256) {
    DigestSize = SHA256_DIGEST_SIZE;
  } else if (SignatureHdr->HashAlg == HASH_TYPE_SHA384) {
    DigestSize = SHA384_DIGEST_SIZE;
  } else {
    return RETURN_INVALID_PARAMETER;
  }

  CalculateHash  (Data, Length, SignatureHdr->HashAlg, Digest);

  if (OutHash != NULL) {
    CopyMem (OutHash, Digest, DigestSize);
  }

  if(SignatureHdr->SigType == SIGNING_TYPE_RSA_PKCS_1_5) {
    Status = RsaVerify_Pkcs_1_5 (PublicKey, SignatureHdr, Digest);
  } else {
    Status = RETURN_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "RSA  verification for usage (0x%08X): %r\n", Usage, Status));
  if (RETURN_ERROR (Status)) {
    DEBUG_CODE_BEGIN();

    DEBUG ((DEBUG_INFO, "First %d Bytes Input Data\n", DigestSize));
    DumpHex (2, 0, DigestSize, (VOID *)Data);

    DEBUG ((DEBUG_INFO, "Last %d Bytes Input Data\n", DigestSize));
    DumpHex (2, 0, DigestSize, (VOID *) (Data + Length - DigestSize));

    DEBUG ((DEBUG_INFO, "Image Digest\n"));
    DumpHex (2, 0, DigestSize, (VOID *)Digest);

    DEBUG ((DEBUG_INFO, "Signature\n"));
    DumpHex (2, 0, SignatureHdr->SigSize, (VOID *)(SignatureHdr->Signature));

    DEBUG ((DEBUG_INFO, "Public Key\n"));
    DumpHex (2, 0, PubKeyHdr->KeySize , PubKeyHdr->KeyData);

    DEBUG_CODE_END();
  }

  return Status;
}
