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
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
 #include <Uefi/UefiBaseType.h>

#define  DEBUG_IPP    0

EFI_STATUS
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
  return ippsHMACMessage_rmf(ikm, ikmLen, salt, saltLen, prk, prkLen, pMethod);
}

IppStatus HKDFExpand( Ipp8u* okm, int okmLen,
                      const Ipp8u* prk, int prkLen,
                      const Ipp8u* info,int infoLen,
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
  
  Sts = ippsHMACGetSize_rmf(&ctxSize);
  if(Sts !=ippStsNoErr) {
    return Sts;
  }

  pHMAC = AllocatePool (ctxSize);

  Sts = ippsHMACInit_rmf(prk, prkLen, pHMAC, pMethod);
  if(Sts !=ippStsNoErr) {
    return Sts;
  }
  
  tmpLen = 0;
  hashLen = prkLen;
  cnt = 1;

  for(outLen=0; outLen<okmLen; outLen+=hashLen) {
    Sts = ippsHMACUpdate_rmf(tmp,  tmpLen, pHMAC);
    if(Sts !=ippStsNoErr) {
      return Sts;
    }
    Sts = ippsHMACUpdate_rmf(info, infoLen, pHMAC);
    if(Sts !=ippStsNoErr) {
      return Sts;
    }
    Sts = ippsHMACUpdate_rmf(&cnt, sizeof(cnt), pHMAC);
    if(Sts !=ippStsNoErr) {
      return Sts;
    }
    cnt++;
    Sts = ippsHMACFinal_rmf(tmp,  hashLen, pHMAC);
    if(Sts !=ippStsNoErr) {
      return Sts;
    }
    tmpLen = hashLen;

    copyLen = ((outLen+hashLen)<=okmLen)? hashLen : okmLen-outLen;

    CopyMem(okm+outLen, tmp, copyLen);
  }

  FreePool(pHMAC);
  return Sts;
}

EFI_STATUS
HKDF ( const IppsHashMethod* pMethod,
       const Ipp8u* salt, int saltLen,
       const Ipp8u* ikm, int ikmLen,
       const Ipp8u* info,int infoLen,
       Ipp8u* okm, int okmLen
     )
{
  Ipp8u tprk[IPP_SHA256_DIGEST_BITSIZE/8];
  IppStatus Sts;

  // TODO: Check to see if Expand returns correct OKM  or not if ikmLen > 64
  if ((saltLen > 64) ||  (infoLen > 64) || (ikmLen > 64)){
    DEBUG ((DEBUG_ERROR, "HKDF: Invalid Parameter\n"));
    return RETURN_INVALID_PARAMETER;
  }

  // step 1 (extract)
  Sts = HKDFExtract(tprk,sizeof(tprk), salt,saltLen, ikm,ikmLen, pMethod);
  if(Sts !=ippStsNoErr) {
    DEBUG ((DEBUG_ERROR, "HKDF Extract failed:%d\n", Sts));
    return EFI_PROTOCOL_ERROR;
  }
#if DEBUG_IPP
  DumpStr("prk:\n", tprk, sizeof(tprk), 32);
#endif

  // step 2 (expand)
  Sts = HKDFExpand(okm,okmLen, tprk,sizeof(tprk), info,infoLen, pMethod);
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
