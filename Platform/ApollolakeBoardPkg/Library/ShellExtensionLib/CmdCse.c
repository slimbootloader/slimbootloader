/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/ShellExtensionLib.h>
#include <Service/HeciService.h>
#include <Library/BootloaderCommonLib.h>

/**
  CSE related command interface.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandCseFunc (
  IN SHELL                  *Shell,
  IN UINTN                  Argc,
  IN CHAR16                 *Argv[]
  )
{
  EFI_STATUS                Status;
  UINT32                    Index;
  HECI_SERVICE              *HeciService;

  HeciService = (HECI_SERVICE *) GetServiceBySignature (HECI_SERVICE_SIGNATURE);
  if ((HeciService == NULL) || (HeciService->SimpleHeciCommand == NULL)) {
    ShellPrint (L"CSE service is not supported.\n");
    return EFI_UNSUPPORTED;
  }

  for (Index = 0; Index < Argc; Index++) {
    if (StrCmp (Argv[Index], L"-h") == 0) {
      goto usage;
    }

    if (StrCmp (Argv[Index], L"-D") == 0) {
      ShellPrint (L"Entering DNX mode\n");
      Status = HeciService->SimpleHeciCommand (SUBCMD_ENTER_DNX_MODE);
      if (EFI_ERROR (Status)) {
        ShellPrint (L"Failed, Status = %r\n", Status);
      }
      return Status;
    }

    if (StrCmp (Argv[Index], L"-S") == 0) {
      Status = HeciService->SimpleHeciCommand (SUBCMD_DUMP_CSE_STATUS);
      if (EFI_ERROR (Status)) {
        ShellPrint (L"Failed, Status = %r\n", Status);
      }
      return Status;
    }
  }

  return EFI_SUCCESS;

usage:
  ShellPrint (L"Usage: %s [-D|-S]\n", Argv[0]);
  ShellPrint (L"\n");
  ShellPrint (L"  -D     enter DNX mode - reboot will be issued\n");
  ShellPrint (L"  -S     dump status of CSE FW and HECI interface\n");

  return EFI_ABORTED;
}

CONST SHELL_COMMAND mShellCommandCse = {
  L"cse",
  L"CSE command interface",
  &ShellCommandCseFunc
};
