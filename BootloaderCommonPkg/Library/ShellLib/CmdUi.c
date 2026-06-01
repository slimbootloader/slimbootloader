/** @file
  Shell commands for UiSetup launch and UiSetup-backed config access.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/UiSetupLib.h>

// LocalConsoleInit is implemented in PayloadPkg/OsLoader and linked at payload level.
EFI_STATUS
LocalConsoleInit (
  IN  BOOLEAN   ForceFbConsole
  );
/**
  Launch UiSetup.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandUiFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  EFI_STATUS Status;

  // Reinitialize console input/output so USB keyboard works in UiSetup from shell.
  LocalConsoleInit (TRUE);

  Status = RunUiSetup ();

  // Restore shell console state after exiting UiSetup.
  LocalConsoleInit (FALSE);

  if (EFI_ERROR (Status)) {
    ShellPrint (L"UiSetup failed: %r\n", Status);
    return Status;
  }

  return Status;
}

CONST SHELL_COMMAND ShellCommandUi = {
  L"uisetup",
  L"Launch UI based setup menu",
  &ShellCommandUiFunc
};

STATIC
EFI_STATUS
EFIAPI
ShellCommandCfgSetFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  return UiCfgShellCfgSetCommand (Argc, Argv);
}

STATIC
EFI_STATUS
EFIAPI
ShellCommandCfgGetFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  return UiCfgShellCfgGetCommand (Argc, Argv);
}

STATIC
EFI_STATUS
EFIAPI
ShellCommandCfgFindFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  return UiCfgShellCfgFindCommand (Argc, Argv);
}

CONST SHELL_COMMAND ShellCommandCfgSet = {
  L"cfgset",
  L"Set an SBL config field by canonical path",
  &ShellCommandCfgSetFunc
};

CONST SHELL_COMMAND ShellCommandCfgGet = {
  L"cfgget",
  L"Read an SBL config field by canonical path",
  &ShellCommandCfgGetFunc
};

CONST SHELL_COMMAND ShellCommandCfgFind = {
  L"cfgfind",
  L"List or search SBL config fields",
  &ShellCommandCfgFindFunc
};

