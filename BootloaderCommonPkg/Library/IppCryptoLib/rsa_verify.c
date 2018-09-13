/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
int VerifyRsaSignature (const void *hash, const void *rsa_n,  const void *rsa_e, const void *sig)
{
	int    sz_n;
	int    sz_e;
	int    sz_rsa;
	int    sz_adj;
	int    signature_verified;

	Ipp8u  bn_buf[3400];
	IppsBigNumState *bn_rsa_n;
	IppsBigNumState *bn_rsa_e;
	Ipp8u *scratch_buf;
	IppStatus err;
	IppsRSAPublicKeyState *rsa_key_s;


	err = ippsRSA_GetSizePublicKey(RSA_MOD_SIZE * 8, RSA_E_SIZE * 8, &sz_rsa);
	if (err != ippStsNoErr) {
		return err;
	}

	err = ippsBigNumGetSize(RSA_MOD_SIZE / sizeof(Ipp32u), &sz_n);
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

	err = ippsBigNumInit(RSA_MOD_SIZE / sizeof(Ipp32u), bn_rsa_n);
	if (err != ippStsNoErr) {
		return err;
	}

	err = ippsSetOctString_BN(rsa_n, RSA_MOD_SIZE, bn_rsa_n);
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

	err = ippsRSA_InitPublicKey(RSA_MOD_SIZE * 8, RSA_E_SIZE * 8, rsa_key_s, sz_rsa);
	if (err != ippStsNoErr) {
		return err;
	}

	err = ippsRSA_SetPublicKey(bn_rsa_n, bn_rsa_e, rsa_key_s);
	if (err != ippStsNoErr) {
		return err;
	}

	signature_verified = 0;
	err = ippsRSAVerifyHash_PKCS1v15_rmf((const Ipp8u *)hash, (Ipp8u *)sig, &signature_verified, rsa_key_s, ippsHashMethod_SHA256 (), scratch_buf);
	if (err != ippStsNoErr) {
		return err;
	}
	return !signature_verified;
}


/* Wrapper function for RSAVerify to make the inferface consistent.
 * Returns RETURN_SUCCESS on success, others on failure.
 */
RETURN_STATUS
RsaVerify(const RSA_PUB_KEY *key, const Ipp8u *sig,	const Ipp32u sig_len, const Ipp8u  sig_type, const Ipp8u *hash)
{
	Ipp8u  *rsa_n;
	Ipp8u  *rsa_e;

	if ((key->Signature != RSA_KEY_IPP_SIGNATURE) || (sig_type != SIG_TYPE_RSA2048SHA256) || (sig_len != RSA_MOD_SIZE)) {
		return RETURN_INVALID_PARAMETER;
	} else {
		rsa_n = (Ipp8u *)key->PubKeyData;
		rsa_e = rsa_n + RSA_MOD_SIZE;
		return VerifyRsaSignature (hash, rsa_n, rsa_e, sig) ? RETURN_SECURITY_VIOLATION : RETURN_SUCCESS ;
	}
}
