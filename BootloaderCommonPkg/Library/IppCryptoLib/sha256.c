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
#include "pcphash.h"
#include "pcptool.h"
#include "pcphash_rmf.h"

#include <Library/CryptoLib.h>

Ipp8u* Sha256 (const Ipp8u* pMsg, Ipp32u msgLen, Ipp8u* pMD)
{
  ippsHashMessage_rmf(pMsg, msgLen, pMD, ippsHashMethod_SHA256 ());
  return pMD;
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
Sha256Init (
  IN      HASH_CTX   *HashCtx,
  IN      Ipp32u      HashCtxSize
  )
{
  if (HashCtxSize < sizeof(IppsHashState_rmf)) {
    return RETURN_BUFFER_TOO_SMALL;
  }

  if (ippsHashInit_rmf((IppsHashState_rmf*)HashCtx, ippsHashMethod_SHA256 ()) == ippStsNoErr) {
    return RETURN_SUCCESS;
  }

  return RETURN_SECURITY_VIOLATION;
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
Sha256Update (
  IN        HASH_CTX   *HashCtx,
  IN CONST  Ipp8u      *Msg,
  IN        Ipp32u      MsgLen
  )
{
  if (ippsHashUpdate_rmf(Msg, MsgLen, (IppsHashState_rmf*)HashCtx) == ippStsNoErr) {
    return RETURN_SUCCESS;
  }

  return RETURN_SECURITY_VIOLATION;
}


/**
  Finalizes the SHA256 hashing and returns the hash.

  @param[in]   HashCtx     Pointer to the hash context buffer.
  @param[out]  Hash        Sha256 hash of the data.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
Sha256Final (
  IN       HASH_CTX   *HashCtx,
  OUT      Ipp8u      *Hash
  )
{
  if (ippsHashFinal_rmf(Hash, (IppsHashState_rmf*)HashCtx) == ippStsNoErr) {
    return RETURN_SUCCESS;
  }

  return RETURN_SECURITY_VIOLATION;
}
