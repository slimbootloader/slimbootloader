/** @file
  Secure boot library routines to provide RSA signature verification.

  Copyright (c) 2017-2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
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
  )
{
  RETURN_STATUS    Status;
  PUB_KEY_HDR     *PublicKey;
  PUB_KEY_HDR     *PublicKeyCopy;
  SIGNATURE_HDR   *SignatureCopy;
  UINT8            Digest[HASH_DIGEST_MAX];
  UINT8            DigestSize;
  UINT32           DumpSize;
  UINT64           PublicKeyBytes;
  UINT64           SignatureBytes;
  UINT8           *PublicKeyBuf;
  UINT8           *SignatureBuf;

  ZeroMem (&Digest, sizeof(Digest));

  PublicKey = PubKeyHdr;
  if ((PublicKey->Identifier != PUBKEY_IDENTIFIER) || (SignatureHdr->Identifier != SIGNATURE_IDENTIFIER)){
    return RETURN_INVALID_PARAMETER;
  }

  SignatureBytes = (UINT64)sizeof (SIGNATURE_HDR) + (UINT64)SignatureHdr->SigSize;
  PublicKeyBytes = (UINT64)sizeof (PUB_KEY_HDR) + (UINT64)PublicKey->KeySize;
  if ((SignatureBytes > MAX_UINT32) || (PublicKeyBytes > MAX_UINT32)) {
    return RETURN_INVALID_PARAMETER;
  }

  SignatureBuf = AllocatePool ((UINTN)SignatureBytes);
  PublicKeyBuf = AllocatePool ((UINTN)PublicKeyBytes);
  if ((SignatureBuf == NULL) || (PublicKeyBuf == NULL)) {
    if (SignatureBuf != NULL) {
      FreePool (SignatureBuf);
    }
    if (PublicKeyBuf != NULL) {
      FreePool (PublicKeyBuf);
    }
    return RETURN_OUT_OF_RESOURCES;
  }

  CopyMem (SignatureBuf, SignatureHdr, (UINTN)SignatureBytes);
  CopyMem (PublicKeyBuf, PublicKey, (UINTN)PublicKeyBytes);

  SignatureCopy = (SIGNATURE_HDR *)SignatureBuf;
  PublicKeyCopy = (PUB_KEY_HDR *)PublicKeyBuf;

  if ((PublicKeyCopy->Identifier != PUBKEY_IDENTIFIER) || (SignatureCopy->Identifier != SIGNATURE_IDENTIFIER)) {
    Status = RETURN_INVALID_PARAMETER;
    goto Exit;
  }

  if (((UINT64)sizeof (SIGNATURE_HDR) + (UINT64)SignatureCopy->SigSize != SignatureBytes) ||
      ((UINT64)sizeof (PUB_KEY_HDR) + (UINT64)PublicKeyCopy->KeySize != PublicKeyBytes)) {
    Status = RETURN_INVALID_PARAMETER;
    goto Exit;
  }

  // Verify public key first
  Status = DoHashVerify (PublicKeyCopy->KeyData, PublicKeyCopy->KeySize, Usage, PubKeyHashAlg, PubKeyHash);
  if (RETURN_ERROR (Status)) {
    goto Exit;
  }

  // Verify payload data
  if (SignatureCopy->HashAlg == HASH_TYPE_SHA256) {
    DigestSize = SHA256_DIGEST_SIZE;
  } else if (SignatureCopy->HashAlg == HASH_TYPE_SHA384) {
    DigestSize = SHA384_DIGEST_SIZE;
  } else {
    Status = RETURN_INVALID_PARAMETER;
    goto Exit;
  }

  DEBUG ((DEBUG_INFO, "SignType (0x%x) SignSize (0x%x)  SignHashAlg (0x%x)\n", \
                  SignatureCopy->SigType, SignatureCopy->SigSize, SignatureCopy->HashAlg));

  if(SignatureCopy->SigType == SIGNING_TYPE_RSA_PKCS_1_5) {
    Status = CalculateHash  (Data, Length, SignatureCopy->HashAlg, Digest);
    if (EFI_ERROR(Status)) {
      Status = RETURN_UNSUPPORTED;
      goto Exit;
    }

    if (OutHash != NULL) {
      CopyMem (OutHash, Digest, DigestSize);
    }

#if FixedPcdGetBool(PcdIppcrypto2Lib)
    // RSA Pkcs 1.5 requires to pass message to be verified
    Status = RsaVerify2_Pkcs_1_5 (PublicKeyCopy, SignatureCopy, Data, Length);
#else
    Status = RsaVerify_Pkcs_1_5 (PublicKeyCopy, SignatureCopy, Digest);
#endif

  } else if(SignatureCopy->SigType == SIGNING_TYPE_RSA_PSS) {

    // Calculate Hash only when OutHash is valid
    // RSA PSS requires to pass message to be verified
    if (OutHash != NULL) {
      Status = CalculateHash  (Data, Length, SignatureCopy->HashAlg, Digest);
      if (EFI_ERROR(Status)) {
        Status = RETURN_UNSUPPORTED;
        goto Exit;
      }
      CopyMem (OutHash, Digest, DigestSize);
    }

    Status = RsaVerify_PSS (PublicKeyCopy, SignatureCopy, Data, Length);

  }  else {
    Status = RETURN_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "RSA verification for usage (0x%08X): %r\n", Usage, Status));
  if (RETURN_ERROR (Status)) {
    DEBUG_CODE_BEGIN();

    DumpSize = MIN (Length, DigestSize);

    DEBUG ((DEBUG_INFO, "First %u Bytes Input Data\n", DumpSize));
    DumpHex (2, 0, DumpSize, (VOID *)Data);

    DEBUG ((DEBUG_INFO, "Last %u Bytes Input Data\n", DumpSize));
    DumpHex (2, 0, DumpSize, (VOID *) (Data + Length - DumpSize));

    DEBUG ((DEBUG_INFO, "Image Digest\n"));
    DumpHex (2, 0, DigestSize, (VOID *)Digest);

    DEBUG ((DEBUG_INFO, "Signature\n"));
    DumpHex (2, 0, SignatureCopy->SigSize, (VOID *)(SignatureCopy->Signature));

    DEBUG ((DEBUG_INFO, "Public Key\n"));
    DumpHex (2, 0, PublicKeyCopy->KeySize , PublicKeyCopy->KeyData);

    DEBUG_CODE_END();
  }

Exit:
  FreePool (SignatureBuf);
  FreePool (PublicKeyBuf);
  return Status;
}
