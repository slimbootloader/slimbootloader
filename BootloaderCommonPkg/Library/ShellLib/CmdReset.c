/** @file
  Shell command `reset` to reset the system.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/IoLib.h>
#include <Library/ResetSystemLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Service/PlatformService.h>

/**
  Reset the system.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandResetFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandReset = {
  L"reset",
  L"Reset the system",
  &ShellCommandResetFunc
};

/**
  Reset the system.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandResetFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  PLATFORM_SERVICE   *PlatformService;
  EFI_RESET_TYPE      ResetType;
  RESET_SYSTEM        FuncResetSystem;

  // Get ResetType
  ResetType = EfiResetCold;
  if (Argc == 2) {
    if (StrCmp (Argv[1], L"warm") == 0) {
      ResetType = EfiResetWarm;
    } else if (StrCmp (Argv[1], L"cold") == 0) {
      ResetType = EfiResetCold;
    } else if (StrCmp (Argv[1], L"off") == 0) {
      ResetType = EfiResetShutdown;
    } else {
      goto usage;
    }
  } else if (Argc > 2) {
    goto usage;
  }

  // Get ResetSystem API if necessary
  FuncResetSystem = ResetSystem;
  PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
  if (PlatformService != NULL && PlatformService->ResetSystem != NULL) {
    FuncResetSystem = PlatformService->ResetSystem;
  }

  ShellPrint (L"Resetting...\n");

  FuncResetSystem (ResetType);
  return EFI_SUCCESS;

usage:
  ShellPrint (L"Usage: reset [cold | warm | off]\n");
  ShellPrint (L"       cold reset is default\n");
  return EFI_ABORTED;
}
