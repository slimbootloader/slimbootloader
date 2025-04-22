/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include "owndefs.h"
#include "owncp.h"
#include "pcphash.h"
#include "pcptool.h"
#include "pcphash_rmf.h"

#include <Library/CryptoLib.h>

/**
  Computes the SM3 message digest of a input data buffer.

  This function performs the SM3 message digest of a given data buffer, and places
  the digest value into the specified memory.

  @param[in]   pMsg        Pointer to the buffer containing the data to be hashed.
  @param[in]   msgLen      Length of Data buffer in bytes.
  @param[out]  pMD         Pointer to a buffer that receives the SM3 digest
                           value (32 bytes).

  @retval                  A pointer to SM3 digest value
**/
Ipp8u*
EFIAPI
Sm3 (const Ipp8u* pMsg, Ipp32u msgLen, Ipp8u* pMD)
{
  if (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SM3_256) {
    ippsHashMessage_rmf(pMsg, msgLen, pMD, ippsHashMethod_SM3 ());
    return pMD;

  } else {
    pMD = NULL;
    return pMD;
  }

}

/**
  Initializes the hash context for Sm3 hashing.

  @param[in]   HashCtx       Pointer to the hash context buffer.
  @param[in]   HashCtxSize   Length of the hash context.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_BUFFER_TOO_SMALL    Hash context buffer size is not large enough.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
EFIAPI
Sm3Init (
  IN      HASH_CTX   *HashCtx,
  IN      Ipp32u      HashCtxSize
  )
{
  if (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SM3_256) {
    if (HashCtxSize < sizeof(IppsHashState_rmf)) {
      return RETURN_BUFFER_TOO_SMALL;
    }

    if (ippsHashInit_rmf((IppsHashState_rmf*)HashCtx, ippsHashMethod_SM3 ()) == ippStsNoErr) {
      return RETURN_SUCCESS;
    }

    return RETURN_SECURITY_VIOLATION;

  } else {
    return RETURN_UNSUPPORTED;
  }
}

/**
  Consumes the data for Sm3 hashing.
  This method can be called multiple times to hash separate pieces of data.

  @param[in]   HashCtx     Pointer to the hash context buffer.
  @param[in]   Msg         Data to be hashed.
  @param[in]   MsgLen      Length of data to be hashed.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
EFIAPI
Sm3Update (
  IN        HASH_CTX   *HashCtx,
  IN CONST  Ipp8u      *Msg,
  IN        Ipp32u      MsgLen
  )
{

  if (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SM3_256) {
    if (ippsHashUpdate_rmf(Msg, MsgLen, (IppsHashState_rmf*)HashCtx) == ippStsNoErr) {
      return RETURN_SUCCESS;
    }

    return RETURN_SECURITY_VIOLATION;
  } else {
    return RETURN_UNSUPPORTED;
  }
}


/**
  Finalizes the Sm3 hashing and returns the hash.

  @param[in]   HashCtx     Pointer to the hash context buffer.
  @param[out]  Hash        Sm3 hash of the data.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
EFIAPI
Sm3Final (
  IN       HASH_CTX   *HashCtx,
  OUT      Ipp8u      *Hash
  )
{
  if (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SM3_256) {
    if (ippsHashFinal_rmf(Hash, (IppsHashState_rmf*)HashCtx) == ippStsNoErr) {
      return RETURN_SUCCESS;
    }

    return RETURN_SECURITY_VIOLATION;
  } else {
    return RETURN_UNSUPPORTED;
  }
}
