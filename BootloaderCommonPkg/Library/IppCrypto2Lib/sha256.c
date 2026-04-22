/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include "owndefs.h"
#include "owncp.h"
#include "pcphash.h"
#include "pcptool.h"
#include "pcphash_rmf.h"
#include "pcpsha256stuff.h"

#include <Library/CryptoLib.h>

/**
  Initializes user context for hash computation. Compaitable with XIP.

  @retval                  Pointer to SHA256 hash-method.
**/
IPPFUN( const IppsHashMethod*, ippsHashMethod_SHA256Sbl, (void) )
{
   static IppsHashMethod method = {
      ippHashAlg_SHA256,
      IPP_SHA256_DIGEST_BITSIZE/8,
      MBS_SHA256,
      MLR_SHA256,
      IPP_SHA256_STATE_BYTESIZE,
      sha256_hashInit,
/* Y8 (SSE4.2/Goldmont, _IPP32E=128): SHA-NI is present on Goldmont-class,
 * but NOT on Haswell/Broadwell/Skylake which are E9/L9 class.
 * Condition (_IPP32E < _IPP32E_E9) therefore covers only Y8 (Goldmont etc.).
 * L9 (AVX2, _IPP32E=512) does NOT satisfy (_IPP32E < _IPP32E_E9) = (512 < 256),
 * so L9 falls to sha256_hashUpdate by default — which uses vmovdqa ymm (32-byte
 * alignment required) and crashes when running XIP from unaligned flash.
 *
 * When IPPCP_CUSTOM_BUILD is defined (SHA-NI-capable platform only; see INF),
 * also select sha256_ni_hashUpdate for L9 so this static method table matches
 * what ippsHashMethodSet_SHA256_TT would select at runtime. */
#if ((_IPP32E >= _IPP32E_Y8) && (_IPP32E < _IPP32E_E9)) \
 || (defined(IPPCP_CUSTOM_BUILD) && (_IPP32E >= _IPP32E_L9) \
     && (_SHA_NI_ENABLING_ == _FEATURE_TICKTOCK_ || _SHA_NI_ENABLING_ == _FEATURE_ON_))
      sha256_ni_hashUpdate,
#else
      sha256_hashUpdate,
#endif
      sha256_hashOctString,
      sha256_msgRep
   };
   return &method;
}

/**
  Computes the SHA-256 message digest of a input data buffer.

  This function performs the SHA-256 message digest of a given data buffer, and places
  the digest value into the specified memory.

  @param[in]   pMsg        Pointer to the buffer containing the data to be hashed.
  @param[in]   msgLen      Length of Data buffer in bytes.
  @param[out]  pMD         Pointer to a buffer that receives the SHA-256 digest
                           value (32 bytes).

  @retval                  A pointer to SHA-256 digest value
**/
Ipp8u*
EFIAPI
Sha256 (const Ipp8u* pMsg, Ipp32u msgLen, Ipp8u* pMD)
{
  if (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_256) {

    ippsHashMessage_rmf(pMsg, msgLen, pMD, ippsHashMethod_SHA256Sbl ());
    return pMD;

  } else {
    pMD = NULL;
    return pMD;
  }
}

/**
  Initializes the hash context for SHA256 hashing.

  @param[in]   HashCtx       Pointer to the hash context buffer.
  @param[in]   HashCtxSize   Length of the hash context.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_BUFFER_TOO_SMALL    Hash context buffer size is not large enough.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
EFIAPI
Sha256Init (
  IN      HASH_CTX   *HashCtx,
  IN      Ipp32u      HashCtxSize
  )
{
  if (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_256) {
    if (HashCtxSize < sizeof(IppsHashState_rmf)) {
      return RETURN_BUFFER_TOO_SMALL;
    }

    if (ippsHashInit_rmf((IppsHashState_rmf*)HashCtx, ippsHashMethod_SHA256Sbl ()) == ippStsNoErr) {
      return RETURN_SUCCESS;
    }

    return RETURN_SECURITY_VIOLATION;

  } else {
    return RETURN_UNSUPPORTED;
  }
}

/**
  Consumes the data for SHA256 hashing.
  This method can be called multiple times to hash separate pieces of data.

  @param[in]   HashCtx     Pointer to the hash context buffer.
  @param[in]   Msg         Data to be hashed.
  @param[in]   MsgLen      Length of data to be hashed.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
EFIAPI
Sha256Update (
  IN        HASH_CTX   *HashCtx,
  IN CONST  Ipp8u      *Msg,
  IN        Ipp32u      MsgLen
  )
{
  if (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_256) {
    if (ippsHashUpdate_rmf(Msg, MsgLen, (IppsHashState_rmf*)HashCtx) == ippStsNoErr) {
      return RETURN_SUCCESS;
    }

    return RETURN_SECURITY_VIOLATION;

  }  else {
    return RETURN_UNSUPPORTED;
  }
}


/**
  Finalizes the SHA256 hashing and returns the hash.

  @param[in]   HashCtx     Pointer to the hash context buffer.
  @param[out]  Hash        Sha256 hash of the data.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
EFIAPI
Sha256Final (
  IN       HASH_CTX   *HashCtx,
  OUT      Ipp8u      *Hash
  )
{
  if (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_256) {
    if (ippsHashFinal_rmf(Hash, (IppsHashState_rmf*)HashCtx) == ippStsNoErr) {
      return RETURN_SUCCESS;
    }

    return RETURN_SECURITY_VIOLATION;

  } else {
    return RETURN_UNSUPPORTED;
  }
}
