/** @file

  Copyright (c) 2018-2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "owndefs.h"
#include "owncp.h"

#include "pcpngrsa.h"
#include "pcphash.h"
#include "pcptool.h"

#include <Library/CryptoLib.h>
#include <Library/BlMemoryAllocationLib.h>

/* Wrapper function for RSA PKCS_1.5 Verify to make the inferface consistent.
 * Returns non-zero on failure, 0 on success.
 */
int VerifyRsaPkcs1Signature (CONST PUB_KEY_HDR *PubKeyHdr, CONST SIGNATURE_HDR *SignatureHdr,  CONST UINT8  *Hash)
{
  int    sz_n;
  int    sz_e;
  int    sz_rsa;
  int    sz_scratch;
  int    signature_verified;

  Ipp8u  *rsa_n;
  Ipp8u  *rsa_e;
  Ipp16u  mod_len;
  Ipp8u  *bn_buf;
  IppsBigNumState *bn_rsa_n;
  IppsBigNumState *bn_rsa_e;
  Ipp8u *scratch_buf;
  IppStatus err;
  IppsRSAPublicKeyState *rsa_key_s;
  Ipp8u *bn_buf_ptr;
  const IppsHashMethod  *pHashMethod = NULL;

  signature_verified = 0;

  rsa_n = (Ipp8u *) PubKeyHdr->KeyData;
  rsa_e = (Ipp8u *) PubKeyHdr->KeyData + PubKeyHdr->KeySize - RSA_E_SIZE;
  mod_len = PubKeyHdr->KeySize - RSA_E_SIZE;

  err = ippsRSA_GetSizePublicKey(mod_len * 8, RSA_E_SIZE * 8, &sz_rsa);
  if (err != ippStsNoErr) {
    return err;
  }

  err = ippsBigNumGetSize(mod_len / sizeof(Ipp32u), &sz_n);
  if (err != ippStsNoErr) {
    return err;
  }
  err = ippsBigNumGetSize(RSA_E_SIZE / sizeof(Ipp32u), &sz_e);
  if (err != ippStsNoErr) {
    return err;
  }

  // Allign sz
  sz_rsa = IPP_ALIGNED_SIZE (sz_rsa, sizeof(Ipp32u));
  sz_n   = IPP_ALIGNED_SIZE (sz_n, sizeof(Ipp32u));
  sz_e   = IPP_ALIGNED_SIZE (sz_e, sizeof(Ipp32u));

  // Allocate BN Buf
  bn_buf = AllocateTemporaryMemory (sz_rsa + sz_n + sz_e);
  if (bn_buf ==  NULL) {
    return ippStsNoMemErr;
  }

  scratch_buf  = NULL;
  bn_buf_ptr   = bn_buf;
  rsa_key_s    = (IppsRSAPublicKeyState*) bn_buf_ptr;
  bn_buf_ptr   = bn_buf_ptr + sz_rsa;
  bn_rsa_n     = (IppsBigNumState *) bn_buf_ptr;
  bn_buf_ptr   = bn_buf_ptr + sz_n;
  bn_rsa_e     = (IppsBigNumState *) bn_buf_ptr;

  err = ippsBigNumInit(mod_len / sizeof(Ipp32u), bn_rsa_n);
  if (err != ippStsNoErr) {
    goto Done;
  }

  err = ippsSetOctString_BN(rsa_n, mod_len, bn_rsa_n);
  if (err != ippStsNoErr) {
    goto Done;
  }

  err = ippsBigNumInit(RSA_E_SIZE / sizeof(Ipp32u), bn_rsa_e);
  if (err != ippStsNoErr) {
    goto Done;
  }

  err = ippsSetOctString_BN(rsa_e, RSA_E_SIZE, bn_rsa_e);
  if (err != ippStsNoErr) {
    goto Done;
  }

  err = ippsRSA_InitPublicKey(mod_len * 8, RSA_E_SIZE * 8, rsa_key_s, sz_rsa);
  if (err != ippStsNoErr) {
    goto Done;
  }

  err = ippsRSA_SetPublicKey(bn_rsa_n, bn_rsa_e, rsa_key_s);
  if (err != ippStsNoErr) {
    goto Done;
  }

  err =ippsRSA_GetBufferSizePublicKey (&sz_scratch, rsa_key_s);
  if (err != ippStsNoErr) {
    goto Done;
  }

  scratch_buf = AllocateTemporaryMemory (sz_scratch);
  if (scratch_buf ==  NULL) {
    goto Done;
  }

  if ((SignatureHdr->HashAlg == HASH_TYPE_SHA256) &&
      ((FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_256) != 0)) {
    pHashMethod = ippsHashMethod_SHA256();
  } else if ((SignatureHdr->HashAlg == HASH_TYPE_SHA384) &&
             ((FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_384) != 0)) {
    pHashMethod = ippsHashMethod_SHA384();
  }

  if (pHashMethod != NULL) {
    err = ippsRSAVerifyHash_PKCS1v15_rmf((const Ipp8u *)Hash, (Ipp8u *)SignatureHdr->Signature, &signature_verified, rsa_key_s, pHashMethod, scratch_buf);
  } else {
    err = ippStsNoOperation;
  }

  Done:
    if (scratch_buf) {
      FreeTemporaryMemory (scratch_buf);
    }
    if (bn_buf) {
      FreeTemporaryMemory (bn_buf);
    }
    if (err != ippStsNoErr) {
      return err;
    }

  return !signature_verified;
}


/* Wrapper function for RSA PSS verify to make the inferface consistent.
 * Returns non-zero on failure, 0 on success.
 */
int VerifyRsaPssSignature (CONST PUB_KEY_HDR *PubKeyHdr, CONST SIGNATURE_HDR *SignatureHdr,  CONST UINT8  *Src, CONST UINT32  Size)
{
  int    sz_n;
  int    sz_e;
  int    sz_rsa;
  int    sz_scratch;
  int    signature_verified;

  Ipp8u  *rsa_n;
  Ipp8u  *rsa_e;
  Ipp16u  mod_len;
  Ipp8u  *bn_buf;
  IppsBigNumState *bn_rsa_n;
  IppsBigNumState *bn_rsa_e;
  Ipp8u *scratch_buf;
  IppStatus err;
  IppsRSAPublicKeyState *rsa_key_s;
  Ipp8u *bn_buf_ptr;
  const IppsHashMethod  *pHashMethod = NULL;

  scratch_buf = NULL;

  signature_verified = 0;

  rsa_n = (Ipp8u *) PubKeyHdr->KeyData;
  rsa_e = (Ipp8u *) PubKeyHdr->KeyData + PubKeyHdr->KeySize - RSA_E_SIZE;
  mod_len = PubKeyHdr->KeySize - RSA_E_SIZE;

  err = ippsRSA_GetSizePublicKey(mod_len * 8, RSA_E_SIZE * 8, &sz_rsa);
  if (err != ippStsNoErr) {
    return err;
  }

  err = ippsBigNumGetSize(mod_len / sizeof(Ipp32u), &sz_n);
  if (err != ippStsNoErr) {
    return err;
  }
  err = ippsBigNumGetSize(RSA_E_SIZE / sizeof(Ipp32u), &sz_e);
  if (err != ippStsNoErr) {
    return err;
  }

  // Allign sz
  sz_rsa = IPP_ALIGNED_SIZE (sz_rsa, sizeof(Ipp32u));
  sz_n   = IPP_ALIGNED_SIZE (sz_n, sizeof(Ipp32u));
  sz_e   = IPP_ALIGNED_SIZE (sz_e, sizeof(Ipp32u));

  // Allocate BN Buf
  bn_buf = AllocateTemporaryMemory (sz_rsa + sz_n + sz_e);
  if (bn_buf ==  NULL) {
    return ippStsNoMemErr;
  }

  bn_buf_ptr   = bn_buf;
  rsa_key_s    = (IppsRSAPublicKeyState*) bn_buf_ptr;
  bn_buf_ptr   = bn_buf_ptr + sz_rsa;
  bn_rsa_n     = (IppsBigNumState *) bn_buf_ptr;
  bn_buf_ptr   = bn_buf_ptr + sz_n;
  bn_rsa_e     = (IppsBigNumState *) bn_buf_ptr;

  err = ippsBigNumInit(mod_len / sizeof(Ipp32u), bn_rsa_n);
  if (err != ippStsNoErr) {
    goto Done;
  }

  err = ippsSetOctString_BN(rsa_n, mod_len, bn_rsa_n);
  if (err != ippStsNoErr) {
    goto Done;
  }

  err = ippsBigNumInit(RSA_E_SIZE / sizeof(Ipp32u), bn_rsa_e);
  if (err != ippStsNoErr) {
    goto Done;
  }

  err = ippsSetOctString_BN(rsa_e, RSA_E_SIZE, bn_rsa_e);
  if (err != ippStsNoErr) {
    goto Done;
  }

  err = ippsRSA_InitPublicKey(mod_len * 8, RSA_E_SIZE * 8, rsa_key_s, sz_rsa);
  if (err != ippStsNoErr) {
    goto Done;
  }

  err = ippsRSA_SetPublicKey(bn_rsa_n, bn_rsa_e, rsa_key_s);
  if (err != ippStsNoErr) {
    goto Done;
  }

  err =ippsRSA_GetBufferSizePublicKey (&sz_scratch, rsa_key_s);
  if (err != ippStsNoErr) {
    goto Done;
  }

  scratch_buf = AllocateTemporaryMemory (sz_scratch);
  if (scratch_buf ==  NULL) {
    goto Done;
  }

  if ((SignatureHdr->HashAlg == HASH_TYPE_SHA256)
          && ((FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_256) != 0)){
    pHashMethod = ippsHashMethod_SHA256();
  } else if ((SignatureHdr->HashAlg == HASH_TYPE_SHA384)
          && ((FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_384) != 0)){
     pHashMethod = ippsHashMethod_SHA384();
  }

  if (pHashMethod != NULL) {
    err = ippsRSAVerify_PSS_rmf((const Ipp8u *)Src, Size, (Ipp8u *)SignatureHdr->Signature, &signature_verified, rsa_key_s, pHashMethod, scratch_buf);
  } else {
    err = ippStsNoOperation;
  }

  Done:
    if (scratch_buf != NULL) {
      FreeTemporaryMemory (scratch_buf);
    }
    if (bn_buf) {
      FreeTemporaryMemory (bn_buf);
    }
    if (err != ippStsNoErr) {
      return err;
    }

  return !signature_verified;
}

/* Wrapper function for RSA PKCS_1.5 Verify to make the inferface consistent.
 * Returns RETURN_SUCCESS on success, others on failure.
 */
RETURN_STATUS
EFIAPI
RsaVerify_Pkcs_1_5 (CONST PUB_KEY_HDR *PubKeyHdr, CONST SIGNATURE_HDR *SignatureHdr,  CONST UINT8  *Hash)
{

  if (FixedPcdGet8(PcdCompSignSchemeSupportedMask) & IPP_RSALIB_PKCS_1_5) {
    if ((SignatureHdr->SigType != SIGNING_TYPE_RSA_PKCS_1_5) ||
                    ((SignatureHdr->SigSize != RSA2048_MOD_SIZE) && (SignatureHdr->SigSize != RSA3072_MOD_SIZE))) {
      return RETURN_INVALID_PARAMETER;
    } else {
      return VerifyRsaPkcs1Signature (PubKeyHdr, SignatureHdr, Hash) ? RETURN_SECURITY_VIOLATION : RETURN_SUCCESS ;
    }
  } else {
      return RETURN_UNSUPPORTED;
  }
}


/* Wrapper function for RSA-PSS verify to make the inferface consistent.
 * Returns RETURN_SUCCESS on success, others on failure.
 */
RETURN_STATUS
EFIAPI
RsaVerify_PSS (CONST PUB_KEY_HDR *PubKeyHdr, CONST SIGNATURE_HDR *SignatureHdr,  CONST UINT8  *Src, CONST UINT32 SrcSize)
{

  if (FixedPcdGet8(PcdCompSignSchemeSupportedMask) & IPP_RSALIB_PSS) {
    if ((SignatureHdr->SigType != SIGNING_TYPE_RSA_PSS) ||
                    ((SignatureHdr->SigSize != RSA2048_MOD_SIZE) && (SignatureHdr->SigSize != RSA3072_MOD_SIZE))) {
      return RETURN_INVALID_PARAMETER;
    } else {
      return VerifyRsaPssSignature (PubKeyHdr, SignatureHdr, Src, SrcSize) ? RETURN_SECURITY_VIOLATION : RETURN_SUCCESS ;
    }
  } else {
      return RETURN_UNSUPPORTED;
  }
}

