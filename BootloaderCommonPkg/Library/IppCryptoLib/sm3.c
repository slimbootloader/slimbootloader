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

Ipp8u* Sm3 (const Ipp8u* pMsg, Ipp32u msgLen, Ipp8u* pMD)
{
  #if (FixedPcdGet8(PcdIppHashLibSupported) & 0x8)
  {
    ippsHashMessage_rmf(pMsg, msgLen, pMD, ippsHashMethod_SM3 ());
    return pMD;
  }
  #else
  {
    pMD = NULL;
    return pMD;
  }
  #endif
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
Sm3Init (
  IN      HASH_CTX   *HashCtx,
  IN      Ipp32u      HashCtxSize
  )
{
  #if (FixedPcdGet8(PcdIppHashLibSupported) & 0x8)
  {
    if (HashCtxSize < sizeof(IppsHashState_rmf)) {
      return RETURN_BUFFER_TOO_SMALL;
    }

    if (ippsHashInit_rmf((IppsHashState_rmf*)HashCtx, ippsHashMethod_SM3 ()) == ippStsNoErr) {
      return RETURN_SUCCESS;
    }

    return RETURN_SECURITY_VIOLATION;
  }
  #else
  {
    return RETURN_UNSUPPORTED;
  }
  #endif
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
Sm3Update (
  IN        HASH_CTX   *HashCtx,
  IN CONST  Ipp8u      *Msg,
  IN        Ipp32u      MsgLen
  )
{

  #if (FixedPcdGet8(PcdIppHashLibSupported) & 0x8)
  {
    if (ippsHashUpdate_rmf(Msg, MsgLen, (IppsHashState_rmf*)HashCtx) == ippStsNoErr) {
      return RETURN_SUCCESS;
    }

    return RETURN_SECURITY_VIOLATION;
  }
  #else
  {
    return RETURN_UNSUPPORTED;
  }
  #endif
}


/**
  Finalizes the Sm3 hashing and returns the hash.

  @param[in]   HashCtx     Pointer to the hash context buffer.
  @param[out]  Hash        Sm3 hash of the data.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
Sm3Final (
  IN       HASH_CTX   *HashCtx,
  OUT      Ipp8u      *Hash
  )
{
  #if (FixedPcdGet8(PcdIppHashLibSupported) & 0x8)
  {
    if (ippsHashFinal_rmf(Hash, (IppsHashState_rmf*)HashCtx) == ippStsNoErr) {
      return RETURN_SUCCESS;
    }

    return RETURN_SECURITY_VIOLATION;
  }
  #else
  {
    return RETURN_UNSUPPORTED;
  }
  #endif
}
