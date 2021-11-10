/** @file

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OsLoader.h"
/**
  Print Pre-OS or/and extra images.

  @param[in] BootOption     the boot options
  @param[in] Flags          the boot flags
  @param[in] ImageType      the image types

**/
VOID
PrintExtraImages (
OS_BOOT_OPTION *BootOption,
UINT8 Flags,
LOAD_IMAGE_TYPE ImageType
)
{
  if ((BootOption->BootFlags & Flags) != 0){
    if (BootOption->Image[ImageType].LbaImage.Valid == 1) {
      DEBUG ((DEBUG_INFO, " [%x|0x%x]", ImageType, BootOption->Image[ImageType].LbaImage.SwPart,
      BootOption->Image[ImageType].LbaImage.LbaAddr));
    } else if (BootOption->Image[ImageType].FileName[0] != '\0') {
      DEBUG ((DEBUG_INFO, " [%a]", BootOption->Image[ImageType].FileName));
    }
  }
}

/**
  Print the OS boot option list.

  @param[in]  OsBootOptionList    the OS boot option list

**/
VOID
PrintBootOptions (
  OS_BOOT_OPTION_LIST        *OsBootOptionList
  )
{
  UINT32                     Index;
  OS_BOOT_OPTION             *BootOption;

  DEBUG ((DEBUG_INFO, "Boot options (in HEX):\n\n"));

  DEBUG ((DEBUG_INFO, "Idx|ImgType|DevType|DevNum|Flags|HwPart|FsType|SwPart|File/Lbaoffset\n"));
  for (Index = 0; Index < OsBootOptionList->OsBootOptionCount; Index++) {
    BootOption = &OsBootOptionList->OsBootOption[Index];
    if (BootOption->FsType < EnumFileSystemMax) {
      DEBUG ((DEBUG_INFO, "%3x|%7x| %5a | %4x | %3x | %4x | %4a | %4x | %a", \
                 Index, \
                 BootOption->ImageType, \
                 GetBootDeviceNameString(BootOption->DevType), \
                 BootOption->DevInstance, \
                 BootOption->BootFlags, \
                 BootOption->HwPart,  \
                 GetFsTypeString (BootOption->FsType), \
                 BootOption->SwPart,  \
                 BootOption->Image[0].FileName \
                 ));
    } else {
      DEBUG ((DEBUG_INFO, "%3x|%7x| %5a | %4x | %3x | %4x | %4a | %4x | 0x%x", \
                 Index, \
                 BootOption->ImageType, \
                 GetBootDeviceNameString(BootOption->DevType), \
                 BootOption->DevInstance, \
                 BootOption->BootFlags, \
                 BootOption->HwPart,  \
                 GetFsTypeString (BootOption->FsType), \
                 BootOption->Image[0].LbaImage.SwPart, \
                 BootOption->Image[0].LbaImage.LbaAddr \
                 ));
    }
    //Print Pre-OS image filename
    PrintExtraImages (BootOption,BOOT_FLAGS_PREOS,LoadImageTypePreOs);
    //Print extra image filename
    PrintExtraImages (BootOption,BOOT_FLAGS_EXTRA,LoadImageTypeExtra0);

    if (Index == OsBootOptionList->CurrentBoot) {
      DEBUG ((DEBUG_INFO," *Current"));
    }
    DEBUG ((DEBUG_INFO, "\n"));
  }
}


/**
  Get the boot option index to start to boot with.

  @param[in]  OsBootOptionList    the OS boot option list
  @param[in]  BootOptionIndex     Previous boot option index.

  @retval     New boot option index
**/
UINT8
GetCurrentBootOption (
  IN OS_BOOT_OPTION_LIST     *OsBootOptionList,
  IN UINT8                   BootOptionIndex
  )
{
  UINT8                      Index;
  OS_BOOT_OPTION             *BootOption;
  OS_CONFIG_DATA_HOB         *OsConfigData;
  UINT8                      Data8;

  //
  // Boot option could be reset to specific boot option.
  //
  if (OsBootOptionList->BootOptionReset != 0) {
    OsBootOptionList->BootOptionReset = 0;
    return OsBootOptionList->CurrentBoot;
  }

  if (BootOptionIndex != 0) {
    return BootOptionIndex;
  }

  // Give another chance like crashmode if ResetReason has non-cold boot reason
  Data8 = (UINT8)~(ResetCold | ResetPowerOn | ResetGlobal | ResetWakeS3);
  if ((OsBootOptionList->ResetReason & Data8) == 0) {
    return OsBootOptionList->CurrentBoot;
  }

  OsConfigData = (OS_CONFIG_DATA_HOB *) GetGuidHobData (NULL, NULL, &gOsConfigDataGuid);
  if ((OsConfigData != NULL) && (OsConfigData->EnableCrashMode != 0)) {
    //
    // Now Crash mode is enabled.
    // And reset reason is not 0, means it is warm reset or watch dog timer caused reset
    // Find CrashOS boot option to boot.
    //
    for (Index = 0; Index < OsBootOptionList->OsBootOptionCount; Index++) {
      BootOption = &OsBootOptionList->OsBootOption[Index];
      if ((BootOption->BootFlags & BOOT_FLAGS_CRASH_OS) != 0) {
        return Index;
      }
    }
  }

  return BootOptionIndex;
}

/**
  Get the next boot option index to boot with.

  @param[in]  OsBootOptionList    the OS boot option list
  @param[in]  BootOptionIndex     Current boot option index.

  @retval     New boot option index
**/
UINT8
GetNextBootOption (
  IN OS_BOOT_OPTION_LIST     *OsBootOptionList,
  IN UINT8                   BootOptionIndex
  )
{
  UINT8                      Index;
  OS_BOOT_OPTION             *BootOption;

  //
  // CrashOS is handled at the start of boot option.
  // Should not boot into CrashOS in normal boot flow.
  //
  Index = BootOptionIndex + 1;
  for ( ; Index < OsBootOptionList->OsBootOptionCount; Index++) {
    BootOption = &OsBootOptionList->OsBootOption[Index];
    if ((BootOption->BootFlags & BOOT_FLAGS_CRASH_OS) != 0) {
      continue;
    }
    break;
  }

  return Index;
}

