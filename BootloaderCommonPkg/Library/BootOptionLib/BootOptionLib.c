/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Guid/OsBootOptionGuid.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/BootOptionLib.h>

CHAR8  *mOsBootDeviceName[] = { "SATA", "SD", "MMC", "UFS", "SPI", "USB", "NVME", "MEM" };
CHAR8  *mOsFsTypeName[]     = { "FAT", "EXT2", "AUTO", "RAW" };

/**
  Get boot option list

  This function will get OS boot option from HOB.

  @retval HOB data         Return HOB data if it is found.
  @retval NULL             Return NULL if it is NOT found.
**/
OS_BOOT_OPTION_LIST *
EFIAPI
GetBootOptionList (
  VOID
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;

  GuidHob = GetNextGuidHob (&gOsBootOptionGuid, GetHobListPtr());
  if (GuidHob == NULL) {
    return NULL;
  }

  return (OS_BOOT_OPTION_LIST *) GET_GUID_HOB_DATA (GuidHob);
}


/**
  Get file system string by file type

  @param[in]  FsType       File system type

  @retval String           Return a string when file systme is found.
  @retval NULL             Return NULL if file system is not found.
**/
CHAR8 *
EFIAPI
GetFsTypeString (
  IN OS_FILE_SYSTEM_TYPE   FsType
  )
{
  if (FsType > EnumFileSystemMax) {
    return NULL;
  }
  return mOsFsTypeName[FsType];
}

/**
  Get boot device name string by boot device type

  @param[in]  DevType      Boot device type

  @retval String           Return a string when boot device type is found.
  @retval NULL             Return NULL if boot device type is not found.
**/
CHAR8 *
EFIAPI
GetBootDeviceNameString (
  OS_BOOT_MEDIUM_TYPE      DevType
  )
{
  if (DevType >= OsBootDeviceMax) {
    return NULL;
  }
  return mOsBootDeviceName[DevType];
}

