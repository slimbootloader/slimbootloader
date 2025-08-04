/** @file
  Shell command `setup` to launch setup payload.

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Guid/OsBootOptionGuid.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootOptionLib.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCommonLib.h>

/**
  Launch setup payload.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandSetupFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandSetup = {
  L"setup",
  L"Launch Setup payload",
  &ShellCommandSetupFunc
};

/**
  Launch setup payload.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandSetupFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  OS_BOOT_OPTION_LIST       *BootOptionList;
  UINT8                     BootOptionIndex;
  EFI_HOB_GUID_TYPE         *BootOptionHob;
  UINT32                    MaxBootOptions;

  BootOptionHob = GetNextGuidHob (&gOsBootOptionGuid, GetHobListPtr());
  if (BootOptionHob == NULL) {
    ShellPrint (L"Boot Option List not found.\n");
    return EFI_UNSUPPORTED;
  }
  BootOptionList = (OS_BOOT_OPTION_LIST *) GET_GUID_HOB_DATA (BootOptionHob);
  MaxBootOptions = (BootOptionHob->Header.HobLength - sizeof(EFI_HOB_GUID_TYPE)
    - sizeof(OS_BOOT_OPTION_LIST)) / sizeof (OS_BOOT_OPTION);

  for (BootOptionIndex = 0; BootOptionIndex < BootOptionList->OsBootOptionCount; BootOptionIndex++) {
    if (AsciiStrCmp ((CHAR8*)BootOptionList->OsBootOption[BootOptionIndex].Image[0].FileImage.FileName, "!SETP/MPYM:STPY") == 0) {
      // Found setup boot option. Set this as current boot and exit.
      BootOptionList->CurrentBoot = BootOptionIndex;
      Shell->ShouldExit = TRUE;
      break;
    }
  }
  if (BootOptionIndex == BootOptionList->OsBootOptionCount) {
    // Didn't find an existing setup boot option, create a new one.
    if (BootOptionIndex < MaxBootOptions) {
      // Available space in boot option list
      ZeroMem (&BootOptionList->OsBootOption[BootOptionIndex], sizeof (OS_BOOT_OPTION));
      BootOptionList->OsBootOption[BootOptionIndex].DevType = OsBootDeviceMemory;
      BootOptionList->OsBootOption[BootOptionIndex].FsType = EnumFileSystemTypeAuto;
      AsciiStrCpyS ((CHAR8*)BootOptionList->OsBootOption[BootOptionIndex].Image[0].FileImage.FileName, MAX_FILE_PATH_LEN, "!SETP/MPYM:STPY");
      BootOptionList->CurrentBoot = BootOptionIndex;
      BootOptionList->OsBootOptionCount++;
      Shell->ShouldExit = TRUE;
    } else {
      ShellPrint (L"No space available in boot option list to create a new setup boot option.\n");
    }
  }

  return EFI_SUCCESS;
}
