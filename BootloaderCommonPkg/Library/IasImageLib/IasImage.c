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
data region of the IAS image matches the expected value.Also, return the
hash of the IAS image to the caller.

@param           Addr              Address of the IAS image to be verified for validity.
@param           Size              Size of the IAS image to be verified for validity.
@param           ImageHash         Hash of the IAS image is returned to the caller.

@retval NULL  The IAS image is compromised.
@retval Hdr   The IAS image is valid.

**/
IAS_HEADER *
IsIasImageValid (
  IN  VOID     *ImageAddr,
  IN  UINT32    Size,
  OUT UINT8    *ImageHash
  )
{
  EFI_STATUS                 Status;
  IAS_HEADER                 *Hdr;
  IAS_PUB_KEY                Key;
  UINT8                      *Signature;
  UINT32                     CrcOut;
  UINT32                     Index;
  UINT32                     KeyIdx;
  UINT8 RsaSignType, SignHashType;

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

  Key.Signature = RSA_KEY_IPP_SIGNATURE;
  CopyMem (&Key.PubKeyMod, ((UINT8 *) IAS_PUBLIC_KEY (Hdr)), sizeof (Key.PubKeyMod));
  //
  // The byte order of RSA public key exponent is opposite between what iastool.py
  // generates and what RsaVerify API expects. Thus reverse it here.
  //
  for (Index = 0, KeyIdx = (RSA_E_SIZE + RSA_MOD_SIZE - 1); Index < RSA_E_SIZE; Index++, KeyIdx--) {
    Key.PubKeyExp[Index] = ((UINT8 *) IAS_PUBLIC_KEY (Hdr))[KeyIdx];
  }

  Signature = (UINT8 *) IAS_SIGNATURE (Hdr);

  //TO-DO: RsaSignType & SignHashType hash info to be retrivied from IasImage header pr 
  // any other process
  RsaSignType = FixedPcdGet8(PcdRsaSignType);
  SignHashType = FixedPcdGet8(PcdSignHashType);

  Status = DoRsaVerify ((CONST UINT8 *)Hdr, ((UINT32)IAS_PAYLOAD_END (Hdr)) - ((UINT32)Hdr), COMP_TYPE_PUBKEY_OS,
                        Signature, (UINT8 *)&Key, NULL, RsaSignType, 
                        SignHashType, ImageHash);
  if (EFI_ERROR (Status) != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "IAS image verification failed!\n"));
    return NULL;
  }
  DEBUG ((DEBUG_INFO, "IAS image is properly signed/verified\n"));

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
    Img[0].Addr = Addr = (UINT32 *) IAS_PAYLOAD (IasImage);
    Img[0].Size = Size = IasImage->DataLength;

    // If there are sub-images (Index.e NumFile > 0) return their addresses and sizes.
    for (Index = 0 ; Index < NumImg && Index < NumFile ; Index += 1) {
      Img[Index].Addr = Addr;
      Img[Index].Size = Size = ImgSize[Index];
      Addr = (UINT32 *) ((UINT8 *)Addr + ROUNDED_UP (Size, 4));
    }
  }

  return MAX (NumFile, 1);
}
