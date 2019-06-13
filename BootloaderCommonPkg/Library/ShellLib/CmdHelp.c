/** @file
  Shell command `help` to display the list of supported shell commands.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>

extern CONST SHELL_COMMAND *mShellDefaultCommands[];

/**
  Display list of supported shell commands.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandHelpFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandHelp = {
  L"help",
  L"List supported commands",
  &ShellCommandHelpFunc
};

/**
  Display list of supported shell commands.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandHelpFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  CONST SHELL_COMMAND **Iter;

  for (Iter = mShellDefaultCommands; *Iter != NULL; Iter++) {
    ShellPrint (L"%-8s - %s\n", (*Iter)->Name,
                (*Iter)->Desc);
  }

  for (Iter = Shell->Commands; *Iter != NULL; Iter++) {
    ShellPrint (L"%-8s - %s\n", (*Iter)->Name,
                (*Iter)->Desc);
  }

  return EFI_SUCCESS;
}
