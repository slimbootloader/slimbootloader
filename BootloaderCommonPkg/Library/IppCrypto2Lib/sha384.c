/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include "owndefs.h"
#include "owncp.h"
#include "pcphash.h"
#include "pcptool.h"
#include "pcphash_rmf.h"
#include "pcpsha512stuff.h"

#include <Library/CryptoLib.h>
#include <Library/DebugLib.h>
#include <Library/BlMemoryAllocationLib.h>
#ifdef IPPCP_FIPS_MODE_SELFTEST
#include "ippcp/fips_cert.h"
#endif

/**
  Initializes user context for hash computation. Compaitable with XIP.

  @retval                  Pointer to SHA384 hash-method.
**/
IPPFUN( const IppsHashMethod*, ippsHashMethod_SHA384Sbl, (void) )
{
   static IppsHashMethod method = {
      ippHashAlg_SHA384,
      IPP_SHA384_DIGEST_BITSIZE/8,
      MBS_SHA512,
      MLR_SHA512,
      sha512_384_hashInit,
      sha512_hashUpdate,
      sha512_384_hashOctString,
      sha512_msgRep
   };
   return &method;
}
#if IPPCP_FIPS_MODE_SELFTEST
static BOOLEAN FirstUse = TRUE;
#endif
/**
  Computes the SHA-384 message digest of a input data buffer.

  This function performs the SHA-384 message digest of a given data buffer, and places
  the digest value into the specified memory.

  @param[in]   pMsg        Pointer to the buffer containing the data to be hashed.
  @param[in]   msgLen      Length of Data buffer in bytes.
  @param[out]  pMD         Pointer to a buffer that receives the SHA-384 digest
                           value (48 bytes).

  @retval                  A pointer to SHA-384 digest value
**/
Ipp8u*
EFIAPI
Sha384 (const Ipp8u* pMsg, Ipp32u msgLen, Ipp8u* pMD)
{
#ifdef IPPCP_FIPS_MODE_SELFTEST
  //------ FIPS-required part
  if (FirstUse) {
    FirstUse = FALSE;
    // 1. check that the function is FIPS-approved:
    if(!ippcp_is_fips_approved_func(HashMessage_rmf)) {
      return NULL; // cannot use this function in FIPS mode.
    }
    // 2. Run the Selftest
    fips_test_status selftest_status = IPPCP_ALGO_SELFTEST_OK;

    // Query buffer size for the test and allocate it (it can be done on Intel® Cryptography Primitives Library side with IPPCP_SELFTEST_USE_MALLOC=on)
    selftest_status += fips_selftest_ippsHashMessage_rmf(ippHashAlg_SHA384);

    // Check selftest status
    if (IPPCP_ALGO_SELFTEST_OK != selftest_status) {
      return NULL; // selftest is not successful -> cannot use this function in FIPS mode.
    }
  }
  //------ FIPS-required part ends (only needed before the first use of algorithm)
#endif

  if (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_384) {
    ippsHashMessage_rmf(pMsg, msgLen, pMD, ippsHashMethod_SHA384Sbl ());
    return pMD;

  } else {
    pMD = NULL;
    return pMD;
  }
}

/**
  Initializes the hash context for SHA384 hashing.

  @param[in]   HashCtx       Pointer to the hash context buffer.
  @param[in]   HashCtxSize   Length of the hash context.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_BUFFER_TOO_SMALL    Hash context buffer size is not large enough.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
EFIAPI
Sha384Init (
  IN      HASH_CTX   *HashCtx,
  IN      Ipp32u      HashCtxSize
  )
{
  if (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_384) {
    if (HashCtxSize < sizeof(IppsHashState_rmf)) {
      return RETURN_BUFFER_TOO_SMALL;
    }

    if (ippsHashInit_rmf((IppsHashState_rmf*)HashCtx, ippsHashMethod_SHA384Sbl ()) == ippStsNoErr) {
      return RETURN_SUCCESS;
    }

    return RETURN_SECURITY_VIOLATION;

  } else {
    return RETURN_UNSUPPORTED;
  }
}

/**
  Consumes the data for SHA384 hashing.
  This method can be called multiple times to hash separate pieces of data.

  @param[in]   HashCtx     Pointer to the hash context buffer.
  @param[in]   Msg         Data to be hashed.
  @param[in]   MsgLen      Length of data to be hashed.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
EFIAPI
Sha384Update (
  IN        HASH_CTX   *HashCtx,
  IN CONST  Ipp8u      *Msg,
  IN        Ipp32u      MsgLen
  )
{
#ifdef IPPCP_FIPS_MODE_SELFTEST
  //------ FIPS-required part

  // 1. check that the function is FIPS-approved:
  if(!ippcp_is_fips_approved_func(HashUpdate_rmf)) {
      return RETURN_UNSUPPORTED; // cannot use this function in FIPS mode.
  }

  // 2. Run the Selftest
  fips_test_status selftest_status = IPPCP_ALGO_SELFTEST_OK;

  // Query buffer size for the test and allocate it (it can be done on Intel® Cryptography Primitives Library side with IPPCP_SELFTEST_USE_MALLOC=on)
  int BufferSize = 0;
  selftest_status += fips_selftest_ippsHash_rmf_get_size(&BufferSize);

  Ipp8u *pBuffer = AllocateTemporaryMemory(BufferSize);

  // Run the test
  selftest_status += fips_selftest_ippsHashUpdate_rmf(ippHashAlg_SHA384, pBuffer);

  // Check selftest status
  if (IPPCP_ALGO_SELFTEST_OK != selftest_status) {
      return RETURN_UNSUPPORTED; // selftest is not successful -> cannot use this function in FIPS mode.
  }
  //------ FIPS-required part ends (only needed before the first use of algorithm)
#endif

  if (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_384) {
    if (ippsHashUpdate_rmf(Msg, MsgLen, (IppsHashState_rmf*)HashCtx) == ippStsNoErr) {
      return RETURN_SUCCESS;
    }

    return RETURN_SECURITY_VIOLATION;

  } else {
      return RETURN_UNSUPPORTED;
  }
}


/**
  Finalizes the SHA384 hashing and returns the hash.

  @param[in]   HashCtx     Pointer to the hash context buffer.
  @param[out]  Hash        Sha384 hash of the data.

  @retval  RETURN_SUCCESS             Success.
  @retval  RETURN_SECURITY_VIOLATION  All other errors.
**/
RETURN_STATUS
EFIAPI
Sha384Final (
  IN       HASH_CTX   *HashCtx,
  OUT      Ipp8u      *Hash
  )
{
#ifdef IPPCP_FIPS_MODE_SELFTEST
  //------ FIPS-required part

  // 1. check that the function is FIPS-approved:
  if(!ippcp_is_fips_approved_func(HashFinal_rmf)) {
      return RETURN_UNSUPPORTED; // cannot use this function in FIPS mode.
  }
#endif

  if (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_384) {
    if (ippsHashFinal_rmf(Hash, (IppsHashState_rmf*)HashCtx) == ippStsNoErr) {
      return RETURN_SUCCESS;
    }

    return RETURN_SECURITY_VIOLATION;

  } else {
    return RETURN_UNSUPPORTED;
  }
}
