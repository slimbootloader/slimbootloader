/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "owndefs.h"
#include "owncp.h"

#include "pcpngrsa.h"
#include "pcphash.h"
#include "pcptool.h"

#include <Library/CryptoLib.h>

/* Wrapper function for RSAVerify to make the inferface consistent.
 * Returns non-zero on failure, 0 on success.
 */
int VerifyRsaSignature (CONST PUB_KEY_HDR *PubKeyHdr, CONST SIGNATURE_HDR *SignatureHdr,  CONST UINT8  *Hash)
{
  int    sz_n;
  int    sz_e;
  int    sz_rsa;
  int    sz_adj;
  int    signature_verified;

  Ipp8u  *rsa_n;
  Ipp8u  *rsa_e;
  Ipp16u  mod_len;
  Ipp8u  bn_buf[3400];
  IppsBigNumState *bn_rsa_n;
  IppsBigNumState *bn_rsa_e;
  Ipp8u *scratch_buf;
  IppStatus err;
  IppsRSAPublicKeyState *rsa_key_s;
  const IppsHashMethod  *pHashMethod = NULL;

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

  rsa_key_s = (IppsRSAPublicKeyState*)bn_buf;
  sz_adj = ((sz_rsa + sizeof(Ipp32u)) & ~(sizeof(Ipp32u)-1));
  scratch_buf = bn_buf + sz_adj;
  sz_adj = sz_adj + sz_adj;
  bn_rsa_n = (IppsBigNumState *)(bn_buf + sz_adj);
  sz_adj = sz_adj + ((sz_n + sizeof(Ipp32u)) & ~(sizeof(Ipp32u)-1));
  bn_rsa_e = (IppsBigNumState *)(bn_buf + sz_adj);
  sz_adj = sz_adj + ((sz_e + sizeof(Ipp32u)) & ~(sizeof(Ipp32u)-1));

  if (sz_adj > sizeof(bn_buf)) {
    return ippStsNoMemErr;
  }

  err = ippsBigNumInit(mod_len / sizeof(Ipp32u), bn_rsa_n);
  if (err != ippStsNoErr) {
    return err;
  }

  err = ippsSetOctString_BN(rsa_n, mod_len, bn_rsa_n);
  if (err != ippStsNoErr) {
    return err;
  }

  err = ippsBigNumInit(RSA_E_SIZE / sizeof(Ipp32u), bn_rsa_e);
  if (err != ippStsNoErr) {
    return err;
  }

  err = ippsSetOctString_BN(rsa_e, RSA_E_SIZE, bn_rsa_e);
  if (err != ippStsNoErr) {
    return err;
  }

  err = ippsRSA_InitPublicKey(mod_len * 8, RSA_E_SIZE * 8, rsa_key_s, sz_rsa);
  if (err != ippStsNoErr) {
    return err;
  }

  err = ippsRSA_SetPublicKey(bn_rsa_n, bn_rsa_e, rsa_key_s);
  if (err != ippStsNoErr) {
    return err;
  }

  signature_verified = 0;
  if ((SignatureHdr->HashAlg == HASH_TYPE_SHA256)
          && (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_256)){
    pHashMethod = ippsHashMethod_SHA256();
  } else if ((SignatureHdr->HashAlg == HASH_TYPE_SHA384)
          && (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_384)){
     pHashMethod = ippsHashMethod_SHA384();
  }

  if (pHashMethod != NULL) {
    err = ippsRSAVerifyHash_PKCS1v15_rmf((const Ipp8u *)Hash, (Ipp8u *)SignatureHdr->Signature, &signature_verified, rsa_key_s, pHashMethod, scratch_buf);
  } else {
    err = ippStsNoOperation;
  }

  if (err != ippStsNoErr) {
    return err;
  }
  return !signature_verified;
}


/* Wrapper function for RSAVerify to make the inferface consistent.
 * Returns RETURN_SUCCESS on success, others on failure.
 */
RETURN_STATUS
RsaVerify_Pkcs_1_5 (CONST PUB_KEY_HDR *PubKeyHdr, CONST SIGNATURE_HDR *SignatureHdr,  CONST UINT8  *Hash)
{

  if ((SignatureHdr->SigType != SIGNING_TYPE_RSA_PKCS_1_5) ||
                  ((SignatureHdr->SigSize != RSA2048_MOD_SIZE) && (SignatureHdr->SigSize != RSA3072_MOD_SIZE))) {
    return RETURN_INVALID_PARAMETER;
  } else {
    return VerifyRsaSignature (PubKeyHdr, SignatureHdr, Hash) ? RETURN_SECURITY_VIOLATION : RETURN_SUCCESS ;
  }
}
