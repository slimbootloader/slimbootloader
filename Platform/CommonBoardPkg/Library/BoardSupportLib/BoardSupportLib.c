/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/BoardSupportLib.h>
#include <ConfigDataCommonDefs.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/CryptoLib.h>

#define  IMAGE_TYPE_ADDENDUM  0xFE
#define  IMAGE_TYPE_NOT_USED  0xFF

/**
  Fill the boot option list data with CFGDATA info

  @param[in, out]   OsBootOptionList pointer to boot option list.
 **/
VOID
EFIAPI
FillBootOptionListFromCfgData (
  IN OUT   OS_BOOT_OPTION_LIST   *OsBootOptionList
)
{
  OS_BOOT_OPTION             *BootOption;
  OS_BOOT_OPTION             *BootOptionCfgData;
  UINT8                       PrevBootOptionIndex;
  UINT8                       UpdateFlag;
  UINTN                       ImageIdx;
  UINTN                       Idx;
  UINT64                      Lba;
  CHAR8                      *StrPtr;

  ImageIdx        = 1;
  OsBootOptionList->CurrentBoot = 0;
  OsBootOptionList->OsBootOptionCount = 0;
  PrevBootOptionIndex = 0;
  for (Idx = 0; Idx < MAX_BOOT_OPTION_CFGDATA_ENTRY; Idx++) {
    BootOptionCfgData = (OS_BOOT_OPTION *)FindConfigDataByTag (CDATA_BOOT_OPTION_TAG + Idx);
    if (BootOptionCfgData == NULL) {
      break;
    }

    if (BootOptionCfgData->ImageType == IMAGE_TYPE_NOT_USED) {
      continue;
    }

    UpdateFlag = 0;
    if (BootOptionCfgData->ImageType == IMAGE_TYPE_ADDENDUM) {
      // This entry is an addendum for previous boot option entry
      if (ImageIdx < ARRAY_SIZE(BootOption->Image)) {
        UpdateFlag = 2;
        BootOption = &OsBootOptionList->OsBootOption[PrevBootOptionIndex];
      }
    } else {
      // CFGDATA has short structure to save size on flash
      // Need to translate the short format to OS_BOOT_OPTION format
      UpdateFlag = 1;
      ImageIdx   = 0;
      BootOption = &OsBootOptionList->OsBootOption[OsBootOptionList->OsBootOptionCount];
      CopyMem (BootOption, BootOptionCfgData, OFFSET_OF (OS_BOOT_OPTION, Image[0]));
    }

    if (UpdateFlag > 0) {
      StrPtr = (CHAR8 *)BootOptionCfgData->Image[0].FileName;
      // Use either LBA or filename. '#' indicates it is LBA string.
      if ((StrPtr[0] == '#') && (AsciiStrHexToUint64S (StrPtr + 1, NULL, &Lba) == RETURN_SUCCESS)) {
        BootOption->Image[ImageIdx].LbaImage.Valid   = 1;
        BootOption->Image[ImageIdx].LbaImage.SwPart  = BootOptionCfgData->SwPart;
        // LBA should be defined as 64bit.
        // Will remove the typecast when the structure is fixed.
        BootOption->Image[ImageIdx].LbaImage.LbaAddr = (UINT32)Lba;
      } else {
        CopyMem (BootOption->Image[ImageIdx].FileName, BootOptionCfgData->Image[0].FileName,
                 sizeof (BootOption->Image[ImageIdx].FileName));
      }
      if (UpdateFlag == 1) {
        PrevBootOptionIndex = OsBootOptionList->OsBootOptionCount;
        OsBootOptionList->OsBootOptionCount++;
        if (OsBootOptionList->OsBootOptionCount >= PcdGet32 (PcdOsBootOptionNumber)) {
          break;
        }
      }
      ImageIdx += 1;
    }
  }

  DEBUG ((DEBUG_INFO, "Created %d OS boot options\n",  OsBootOptionList->OsBootOptionCount));
}

/**
  Set the platform name with CFGDATA info

 **/
VOID
EFIAPI
PlatformNameInit (
  VOID
)
{
  PLAT_NAME_CFG_DATA     *PlatNameConfigData;

  PlatNameConfigData = (PLAT_NAME_CFG_DATA *) FindConfigDataByTag(CDATA_PLAT_NAME_TAG);
  if (PlatNameConfigData != NULL) {
    SetPlatformName ((VOID *)&PlatNameConfigData->PlatformName);
  } else {
    DEBUG ((DEBUG_INFO, "Platform Name config not found"));
  }
}


/**
  Load the configuration data blob from SPI flash into destination buffer.
  It supports the sources: PDR, BIOS for external Cfgdata.

  @param[in]    Dst        Destination address to load configuration data blob.
  @param[in]    Src        Source address to load configuration data blob.
  @param[in]    Len        The destination address buffer size.

  @retval  EFI_SUCCESS             Configuration data blob was loaded successfully.
  @retval  EFI_NOT_FOUND           Configuration data blob cannot be found.
  @retval  EFI_OUT_OF_RESOURCES    Destination buffer is too small to hold the
                                   configuration data blob.
  @retval  Others                  Failed to load configuration data blob.

**/
EFI_STATUS
EFIAPI
SpiLoadExternalConfigData (
  IN UINT32  Dst,
  IN UINT32  Src,
  IN UINT32  Len
  )
{
  EFI_STATUS   Status;
  UINT32       Address;
  UINT32       BlobSize;
  UINT8       *Buffer;
  CDATA_BLOB  *CfgBlob;
  UINT32       SignedLen;
  UINT32       CfgDataLoadSrc;
  UINT32       Base;
  UINT32       Length;

  Address  = 0;
  BlobSize = sizeof(CDATA_BLOB);
  Buffer   = (UINT8 *)Dst;
  Base     = 0;

  CfgDataLoadSrc = PcdGet32 (PcdCfgDataLoadSource);

  CfgBlob = NULL;
  if (Len < BlobSize) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Get configuration data header
  //
  Status = EFI_UNSUPPORTED;
  if (CfgDataLoadSrc == FlashRegionPlatformData) {
    Status = SpiFlashRead (FlashRegionPlatformData, Address, BlobSize, Buffer);
  } else if (CfgDataLoadSrc == FlashRegionBios) {
    Status = GetComponentInfo (FLASH_MAP_SIG_CFGDATA, &Base, &Length);
    if (!EFI_ERROR(Status)) {
      CopyMem (Buffer, (VOID *)Base, BlobSize);
    }
  }
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Check the configuration signature and size
  //
  CfgBlob = (CDATA_BLOB  *)Buffer;
  if ((CfgBlob == NULL) || (CfgBlob->Signature != CFG_DATA_SIGNATURE)) {
    return EFI_NOT_FOUND;
  }

  SignedLen = CfgBlob->UsedLength;
  if (FeaturePcdGet (PcdVerifiedBootEnabled)) {
    SignedLen += RSA2048_SIGNATURE_AND_KEY_SIZE;
  }

  if ((SignedLen > Len) || (SignedLen <= sizeof(CDATA_BLOB))) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Read the full configuration data
  //
  if (CfgDataLoadSrc == FlashRegionPlatformData) {
    Status = SpiFlashRead (FlashRegionPlatformData, Address + BlobSize, SignedLen - BlobSize, Buffer + BlobSize);
  } else {
    if (Base > 0) {
      CopyMem (Buffer + BlobSize, (VOID *)(Base + BlobSize), SignedLen - BlobSize);
    }
  }

  return Status;
}
