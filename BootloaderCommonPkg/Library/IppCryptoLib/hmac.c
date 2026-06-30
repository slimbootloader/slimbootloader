/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "owndefs.h"
#include "owncp.h"
#include "pcphash.h"
#include "pcptool.h"
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
 #include <Uefi/UefiBaseType.h>

#define  DEBUG_IPP    0

EFI_STATUS
EFIAPI
HmacSha256 (const Ipp8u* pMsg, Ipp32u msgLen, const Ipp8u* pKey, Ipp32u keyLen, Ipp8u* pMD, Ipp32u mdLen)
{
  IppStatus Result;
  Result = ippsHMACMessage_rmf(pMsg, msgLen, pKey, keyLen, pMD, mdLen, ippsHashMethod_SHA256 ());
  if (Result != ippStsNoErr) {
    return EFI_PROTOCOL_ERROR;
  }
  return EFI_SUCCESS;
}

#if DEBUG_IPP
static void DumpStr(const char* note, const Ipp8u* inp, int inpLen, int lineLen)
{
  int n;
  int m;
  int printNum;

  if(note) {
    DEBUG ((DEBUG_INFO, "%s\n", note));
  }
  for(n=0; n<inpLen; n+=lineLen) {
    printNum = ((n+lineLen)<=inpLen)? lineLen : inpLen-n;
    for(m=0; m<printNum; m++) {
      DEBUG ((DEBUG_INFO, "%02x", inp[n+m]));
    }
    DEBUG ((DEBUG_INFO, "\n"));
  }
}
#endif

IppStatus HKDFExtract( Ipp8u* prk, int prkLen,
                       const Ipp8u* salt, int saltLen,
                       const Ipp8u* ikm, int ikmLen,
                       const IppsHashMethod* pMethod
                     )
{
  if ((prk == NULL) || (ikm == NULL) || (pMethod == NULL)) {
    DEBUG ((DEBUG_ERROR, "HKDFExtract: NULL pointer parameter\n"));
    return ippStsNullPtrErr;
  }
  if ((prkLen <= 0) || (ikmLen <= 0)) {
    DEBUG ((DEBUG_ERROR, "HKDFExtract: Invalid length parameter\n"));
    return ippStsBadArgErr;
  }
  if ((salt == NULL) && (saltLen != 0)) {
    DEBUG ((DEBUG_ERROR, "HKDFExtract: NULL salt with non-zero saltLen\n"));
    return ippStsNullPtrErr;
  }

  //
  // RFC 5869 Section 2.2: If salt is not present, it defaults to a string of HashLen zeros.
  //
  if ((salt == NULL) && (saltLen == 0)) {
    Ipp8u zero_salt[64];
    ZeroMem (zero_salt, prkLen);
    return ippsHMACMessage_rmf(ikm, ikmLen, zero_salt, prkLen, prk, prkLen, pMethod);
  }

  return ippsHMACMessage_rmf(ikm, ikmLen, salt, saltLen, prk, prkLen, pMethod);
}

IppStatus HKDFExpand( Ipp8u* okm, int okmLen,
                      const Ipp8u* prk, int prkLen,
                      const Ipp8u* info, int infoLen,
                      const IppsHashMethod* pMethod)
{
  int ctxSize = 0;
  IppsHMACState_rmf* pHMAC;
  Ipp8u tmp[IPP_SHA512_DIGEST_BITSIZE/8];
  int tmpLen;
  int hashLen;
  Ipp8u cnt;
  int outLen;
  int copyLen;
  IppStatus Sts;

  //
  // Validate required pointer parameters
  //
  if ((okm == NULL) || (prk == NULL) || (pMethod == NULL)) {
    DEBUG ((DEBUG_ERROR, "HKDFExpand: NULL pointer parameter\n"));
    return ippStsNullPtrErr;
  }

  //
  // RFC 5869 Section 2.3: info is optional context; NULL is valid if infoLen == 0
  //
  if ((info == NULL) && (infoLen != 0)) {
    DEBUG ((DEBUG_ERROR, "HKDFExpand: NULL info with non-zero infoLen\n"));
    return ippStsNullPtrErr;
  }

  //
  // Validate length parameters
  //
  if ((okmLen <= 0) || (prkLen <= 0)) {
    DEBUG ((DEBUG_ERROR, "HKDFExpand: Invalid length parameter\n"));
    return ippStsBadArgErr;
  }

  //
  // Enforce RFC 5869 Section 2.3: L <= 255 * HashLen
  //
  hashLen = prkLen;
  if ((hashLen <= 0) || (okmLen > (255 * hashLen))) {
    DEBUG ((DEBUG_ERROR, "HKDFExpand: okmLen exceeds 255*hashLen limit\n"));
    return ippStsBadArgErr;
  }

  Sts = ippsHMACGetSize_rmf(&ctxSize);
  if(Sts !=ippStsNoErr) {
    return Sts;
  }

  pHMAC = AllocatePool (ctxSize);
  if (pHMAC == NULL) {
    Sts = ippStsNoMemErr;
    return Sts;
  }

  Sts = ippsHMACInit_rmf(prk, prkLen, pHMAC, pMethod);
  if(Sts !=ippStsNoErr) {
    goto Exit;
  }

  tmpLen = 0;
  cnt = 1;

  //
  // Use safe pointer for info (RFC 5869 allows empty/NULL info)
  //
  const Ipp8u empty_info = 0;
  const Ipp8u *use_info = info ? info : &empty_info;

  for(outLen = 0; outLen < okmLen; outLen += hashLen) {
    Sts = ippsHMACUpdate_rmf(tmp, tmpLen, pHMAC);
    if(Sts !=ippStsNoErr) {
      goto Exit;
    }
    Sts = ippsHMACUpdate_rmf(use_info, infoLen, pHMAC);
    if(Sts !=ippStsNoErr) {
      goto Exit;
    }
    Sts = ippsHMACUpdate_rmf(&cnt, sizeof(cnt), pHMAC);
    if(Sts !=ippStsNoErr) {
      goto Exit;
    }
    cnt++;
    Sts = ippsHMACFinal_rmf(tmp, hashLen, pHMAC);
    if(Sts !=ippStsNoErr) {
      goto Exit;
    }
    tmpLen = hashLen;

    copyLen = ((outLen + hashLen) <= okmLen) ? hashLen : (okmLen - outLen);
    if (copyLen <= 0) {
      Sts = ippStsBadArgErr;
      goto Exit;
    }

    CopyMem(okm + outLen, tmp, copyLen);
  }

Exit:
  ZeroMem(pHMAC, ctxSize);
  FreePool(pHMAC);
  return Sts;
}

EFI_STATUS
HKDF ( const IppsHashMethod* pMethod,
       const Ipp8u* salt, int saltLen,
       const Ipp8u* ikm, int ikmLen,
       const Ipp8u* info, int infoLen,
       Ipp8u* okm, int okmLen
     )
{
  Ipp8u tprk[IPP_SHA256_DIGEST_BITSIZE/8];
  IppStatus Sts;

  // step 1 (extract)
  Sts = HKDFExtract(tprk, sizeof(tprk), salt, saltLen, ikm, ikmLen, pMethod);
  if(Sts !=ippStsNoErr) {
    ZeroMem(tprk, sizeof(tprk));
    DEBUG ((DEBUG_ERROR, "HKDF Extract failed:%d\n", Sts));
    return EFI_PROTOCOL_ERROR;
  }
#if DEBUG_IPP
  DumpStr("prk:\n", tprk, sizeof(tprk), 32);
#endif

  // step 2 (expand)
  Sts = HKDFExpand(okm, okmLen, tprk, sizeof(tprk), info, infoLen, pMethod);

  //
  // Clear PRK from stack before return
  //
  ZeroMem(tprk, sizeof(tprk));

  if(Sts !=ippStsNoErr) {
    DEBUG ((DEBUG_ERROR, "HKDF Expand failed:%d\n", Sts));
    return EFI_PROTOCOL_ERROR;
  }

  return EFI_SUCCESS;
}

/* Wrapper function for HMAC HKDF logic
 * Returns non-zero on failure, 0 on success.
 */
EFI_STATUS
HkdfExtractExpand( const Ipp8u* salt, int saltLen,
                   const Ipp8u* ikm, int ikmLen,
                   const Ipp8u* info, int infoLen,
                   Ipp8u* okm, int okmLen
                 )
{
  EFI_STATUS Status;

#if DEBUG_IPP
  DEBUG ((DEBUG_INFO, "SaltLen = %x, IKMLen = 0x%x, OKMLen = 0x%x, Info = 0x%x\n", saltLen, ikmLen, okmLen, infoLen));
  DumpStr("salt:", salt, saltLen, 32);
  DumpStr("ikm:", ikm, ikmLen, 32);
  DumpStr("info:", info, 10, 32);
#endif

  Status = HKDF( ippsHashMethod_SHA256(),
                    salt, saltLen,
                    ikm, ikmLen,
                    info, infoLen,
                    okm, okmLen
               );
#if DEBUG_IPP
  DumpStr("okm:\n", okm, okmLen, 32);
#endif

  return Status;
}
