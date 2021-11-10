/** @file
  Shell command `mtrr` to display current MTRR configuration.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/MtrrLib.h>

/**
  Display current MTRR configuration.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandMtrrFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandMtrr = {
  L"mtrr",
  L"Display current MTRR configuration",
  &ShellCommandMtrrFunc
};

/**
  Display current MTRR configuration.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandMtrrFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  UINT32      Index;

  // Check flags
  for (Index = 0; Index < Argc; Index++) {
    if (StrCmp (Argv[Index], L"-h") == 0) {
      goto usage;
    }
  }

  PrintMtrr (NULL, TRUE);
  return EFI_SUCCESS;

usage:
  ShellPrint (L"Usage: %s\n", Argv[0]);
  return EFI_ABORTED;
}
