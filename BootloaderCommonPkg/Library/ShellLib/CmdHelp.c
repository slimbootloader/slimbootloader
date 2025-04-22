/** @file
  Shell command `help` to display the list of supported shell commands.

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/DebugLib.h>
#include "Shell.h"

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
  LIST_ENTRY                *EntryList;
  LIST_ENTRY                *Link;
  SHELL_COMMAND_LIST_ENTRY  *Entry;

  EntryList = &Shell->CommandEntryList;
  for (Link = EntryList->ForwardLink; Link != EntryList; Link = Link->ForwardLink) {
    Entry = CR (Link, SHELL_COMMAND_LIST_ENTRY, Link, SHELL_COMMAND_LIST_ENTRY_SIGNATURE);

    ShellPrint (L"%-8s - %s\n", Entry->ShellCommand->Name, Entry->ShellCommand->Desc);
  }

  return EFI_SUCCESS;
}
