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
  UINT8                       UpdateFlag;
  UINTN                       ImageIdx;
  UINTN                       Idx;
  UINT64                      Lba;
  CHAR8                      *StrPtr;

  ImageIdx        = 1;
  BootOption      = &OsBootOptionList->OsBootOption[0];
  OsBootOptionList->CurrentBoot = 0;
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
      }
    } else {
      // CFGDATA has short structure to save size on flash
      // Need to translate the short format to OS_BOOT_OPTION format
      UpdateFlag = 1;
      ImageIdx   = 0;
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
        BootOption++;
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
