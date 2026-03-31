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

/*
 * Provide UpdateSHA512_C as pure-C fallback for L9 XIP builds.
 *
 * pcpsha512l9as.nasm (L9 AVX2) exports UpdateSHA512 and is used when running
 * from RAM (Stage1B+). In XIP (Stage1A), vmovdqa ymm on ROM-mapped .data
 * constants (SHA512_YMM_K) requires 32-byte alignment that the flash layout
 * does not guarantee -> #GP(13). pcphashsha512px.c is ipp-crypto's portable-C
 * fallback (scalar 64-bit loads, no alignment requirement, XIP-safe).
 *
 * Its guard "#if !(...(_IPP32E >= _IPP32E_E9))" excludes it when _IPP32E=512
 * (L9). We temporarily raise _IPP32E_E9 to bypass the guard, then restore it.
 * The function is renamed UpdateSHA512_C (via #define) to avoid a duplicate
 * symbol conflict with UpdateSHA512 from pcpsha512l9as.nasm.
 */
#ifdef _L9
#undef  _IPP32E_E9
#define _IPP32E_E9  99999
#ifndef _ENABLE_ALG_SHA384_
#  define _ENABLE_ALG_SHA384_
#endif
#ifndef _ALG_SHA512_COMPACT_
#  define _ALG_SHA512_COMPACT_
#endif
/*
 * Rename the pure-C UpdateSHA512 to UpdateSHA512_C to avoid a duplicate
 * symbol conflict with UpdateSHA512 exported by pcpsha512l9as.nasm (AVX2).
 */
#undef  UpdateSHA512
#define UpdateSHA512 UpdateSHA512_C
#include "hash/sha512/pcphashsha512px.c"
#undef  UpdateSHA512
#undef  _IPP32E_E9
#define _IPP32E_E9  256

/* Pure-C wrapper: used when executing from XIP flash (Stage1A). */
IPP_OWN_DEFN(static void, sha512_hashUpdate_c, (void* pHash, const Ipp8u* pMsg, int msgLen))
{
   UpdateSHA512_C(pHash, pMsg, msgLen, sha512_cnt);
}
#endif /* _L9 */

/**
  Initializes user context for hash computation. Compatible with XIP.

  On L9: dispatches to AVX2 sha512_hashUpdate when executing from RAM
  (Stage1B+), or to pure-C sha512_hashUpdate_c when executing from flash
  (Stage1A XIP). Both method tables are const static (.rdata), safe to read
  in XIP. Non-L9 variants always use sha512_hashUpdate.

  @retval                  Pointer to SHA384 hash-method.
**/
IPPFUN( const IppsHashMethod*, ippsHashMethod_SHA384Sbl, (void) )
{
#ifdef _L9
   /* AVX2 path: pcpsha512l9as.nasm provides UpdateSHA512. RAM execution only. */
   static const IppsHashMethod method_avx2 = {
      ippHashAlg_SHA384,
      IPP_SHA384_DIGEST_BITSIZE/8,
      MBS_SHA512,
      MLR_SHA512,
      IPP_SHA384_STATE_BYTESIZE,
      sha512_384_hashInit,
      sha512_hashUpdate,
      sha512_384_hashOctString,
      sha512_msgRep
   };
   /* Pure-C path: UpdateSHA512_C from pcphashsha512px.c. XIP-safe. */
   static const IppsHashMethod method_c = {
      ippHashAlg_SHA384,
      IPP_SHA384_DIGEST_BITSIZE/8,
      MBS_SHA512,
      MLR_SHA512,
      IPP_SHA384_STATE_BYTESIZE,
      sha512_384_hashInit,
      sha512_hashUpdate_c,
      sha512_384_hashOctString,
      sha512_msgRep
   };
   /*
    * XIP detection: if this function's code address falls within the flash
    * region, we are still executing in place (Stage1A). Use pure-C path to
    * avoid vmovdqa ymm on ROM-mapped .data constants (-> #GP(13)).
    */
   UINTN FnAddr = (UINTN)(VOID*)ippsHashMethod_SHA384Sbl;
   if (FnAddr >= FixedPcdGet32 (PcdFlashBaseAddress) &&
       FnAddr <  (UINTN)FixedPcdGet32 (PcdFlashBaseAddress) +
                        FixedPcdGet32 (PcdFlashSize)) {
      return &method_c;
   }
   return &method_avx2;
#else
   static const IppsHashMethod method = {
      ippHashAlg_SHA384,
      IPP_SHA384_DIGEST_BITSIZE/8,
      MBS_SHA512,
      MLR_SHA512,
      IPP_SHA384_STATE_BYTESIZE,
      sha512_384_hashInit,
      sha512_hashUpdate,
      sha512_384_hashOctString,
      sha512_msgRep
   };
   return &method;
#endif
}

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
  if (FixedPcdGet8(PcdIppHashLibSupportedMask) & IPP_HASHLIB_SHA2_384) {
    if (ippsHashFinal_rmf(Hash, (IppsHashState_rmf*)HashCtx) == ippStsNoErr) {
      return RETURN_SUCCESS;
    }

    return RETURN_SECURITY_VIOLATION;

  } else {
    return RETURN_UNSUPPORTED;
  }
}
