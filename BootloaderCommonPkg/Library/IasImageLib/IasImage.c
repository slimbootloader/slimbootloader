/** @file
  This file defines IAS File structures.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/IasImageLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/Crc32Lib.h>
#include <Library/SecureBootLib.h>

/**
Check the Addr parameter for a valid IAS image; ensure the format is correct,
confirm the IAS header CRC is correct, and (optional) confirm the hash of the
data region of the IAS image matches the expected value. Also, return the
buffer and hash info of the IAS image to the caller.

@param  Addr              Address of the IAS image to be verified for validity.
@param  Size              Size of the IAS image to be verified for validity.
@param  IasImageInfo      IasImage buffer and hash info data structure

@retval NULL  The IAS image is compromised.
@retval Hdr   The IAS image is valid.

**/
IAS_HEADER *
IsIasImageValid (
  IN  VOID               *ImageAddr,
  IN  UINT32              Size,
  OUT IAS_IMAGE_INFO     *IasImageInfo
  )
{
  EFI_STATUS                 Status;
  IAS_HEADER                 *Hdr;
  SIGNATURE_HDR              *SignHdr;
  PUB_KEY_HDR                *PubKeyHdr;
  IAS_PUB_KEY                *Key;
  UINT32                     CrcOut;
  UINT32                     Index;
  UINT32                     KeyIdx;
  UINT8                      PubKeyBuf[sizeof(PUB_KEY_HDR) + RSA2048_NUMBYTES + RSA_E_SIZE];
  UINT8                      SignBuf[sizeof(SIGNATURE_HDR) + RSA2048_NUMBYTES];

  Hdr = (IAS_HEADER *) ImageAddr;

  if (Hdr->MagicPattern != IAS_MAGIC_PATTERN) {
    return NULL;
  }

  if (Hdr->DataOffset > Size) {
    return NULL;
  }

  if (((UINT8 *)IAS_IMAGE_END(Hdr) < (UINT8 *)Hdr) ||  ((UINT8 *)IAS_IMAGE_END(Hdr) > ((UINT8 *)Hdr + Size))) {
    return NULL;
  }

  Status = CalculateCrc32WithType ((UINT8 *)Hdr, sizeof (IAS_HEADER) - sizeof (UINT32), Crc32TypeCastagnoli, &CrcOut);
  if ((Status != EFI_SUCCESS) || (CrcOut != Hdr->HeaderCrc)) {
    DEBUG ((DEBUG_INFO, "CalculateCrc32 Error!\n"));
    return NULL;
  }

  if (!FeaturePcdGet (PcdVerifiedBootEnabled)) {
    DEBUG ((DEBUG_INFO, "IAS image verification is skipped!\n"));
    return Hdr;
  }

  if (!IAS_IMAGE_IS_SIGNED (Hdr->ImageType)) {
    DEBUG ((DEBUG_ERROR, "IAS image is not signed!\n"));
    return NULL;
  }

  SignHdr     = (SIGNATURE_HDR *) SignBuf;
  //Update SIGNATURE_HDR Info
  SignHdr->Identifier = SIGNATURE_IDENTIFIER;
  SignHdr->SigSize = RSA2048_NUMBYTES;
  SignHdr->SigType = SIGNING_TYPE_RSA_PKCS_1_5;
  SignHdr->HashAlg = HASH_TYPE_SHA256;

  CopyMem (SignBuf + sizeof(SIGNATURE_HDR), (UINT8 *) IAS_SIGNATURE (Hdr), RSA2048_NUMBYTES);

  PubKeyHdr     = (PUB_KEY_HDR *) PubKeyBuf;

  //Update  PUB_KEY_HDR Info
  PubKeyHdr->Identifier = PUBKEY_IDENTIFIER;
  PubKeyHdr->KeySize = RSA_E_SIZE + RSA2048_NUMBYTES;
  PubKeyHdr->KeyType = KEY_TYPE_RSA;

  Key = (IAS_PUB_KEY *) (PubKeyBuf + sizeof(PUB_KEY_HDR));
  CopyMem (Key->Modulus, ((UINT8 *) IAS_PUBLIC_KEY (Hdr)), RSA2048_NUMBYTES);

  //
  // The byte order of RSA public key exponent is opposite between what iastool.py
  // generates and what RsaVerify API expects. Thus reverse it here.
  //
  for (Index = 0, KeyIdx = (RSA_E_SIZE + RSA2048_MOD_SIZE - 1); Index < RSA_E_SIZE; Index++, KeyIdx--) {
    Key->PubExp[Index]  = ((UINT8 *) IAS_PUBLIC_KEY (Hdr))[KeyIdx];
  }

  Status = DoRsaVerify ((CONST UINT8 *)Hdr, ((UINT32)IAS_PAYLOAD_END (Hdr)) - ((UINT32)(UINTN)Hdr),
                         HASH_USAGE_PUBKEY_OS, SignHdr, PubKeyHdr, PcdGet8(PcdCompSignHashAlg), NULL, IasImageInfo->HashData);
  if (EFI_ERROR (Status) != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "IAS image verification failed!\n"));
    return NULL;
  }
  DEBUG ((DEBUG_INFO, "IAS image is properly signed/verified\n"));

  // populate IAS_IMAGE_INFO
  IasImageInfo->CompBuf  = (UINT8 *)Hdr;
  IasImageInfo->CompLen  = ((UINT32)IAS_PAYLOAD_END (Hdr)) - ((UINT32)(UINTN)Hdr);
  IasImageInfo->HashAlg  = PcdGet8(PcdCompSignHashAlg);

  return Hdr;
}

/**
Check the Addr parameter for a valid IAS image; ensure the format is correct,
confirm the IAS header CRC is correct, and (optional) confirm the hash of the
data region of the IAS image matches the expected value.

@param  IasImage  Pointer to the IAS image to retrieve files from.
@param  NumImg    The number of files expected to be extracted from the IAS image.
@param  Img       Buffer to be filled with each retrieved file's address and size from the IAS image.

@retval MAX (NumFile, 1)  The number of files retrieved from the IAS image.

**/
UINT32
IasGetFiles (
  IN   IAS_HEADER  *IasImage,
  IN   UINT32     NumImg,
  OUT  IMAGE_DATA  *Img
  )
{
  UINT32 NumFile;
  UINT32 *ImgSize;
  UINT32 *Addr;
  UINT32 Index;
  UINT32 Size;

  ImgSize = IAS_EXT_HDR (IasImage);
  NumFile = IAS_EXT_HDR_SIZE (IasImage) / sizeof (UINT32);

  if (NumImg != 0) {
    ZeroMem (Img, NumImg * sizeof (Img[0]));

    // Return Addr single entry (namely the entire payload) for plain images.
    Img[0].Addr = Addr = (UINT32 *)(UINTN)IAS_PAYLOAD (IasImage);
    Img[0].Size = Size = IasImage->DataLength;
    Img[0].AllocType = ImageAllocateTypePointer;

    // If there are sub-images (Index.e NumFile > 0) return their addresses and sizes.
    for (Index = 0 ; Index < NumImg && Index < NumFile ; Index += 1) {
      Img[Index].Addr = Addr;
      Img[Index].Size = Size = ImgSize[Index];
      Img[Index].AllocType = ImageAllocateTypePointer;
      Addr = (UINT32 *) ((UINT8 *)Addr + ROUNDED_UP (Size, 4));
    }
  }

  return MAX (NumFile, 1);
}
