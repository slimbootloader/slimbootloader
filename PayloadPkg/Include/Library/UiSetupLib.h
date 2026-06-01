/** @file
  Public API for the UiSetup library.

  Consumers (OsLoader, ShellLib/CmdUi) use only this header.
  The implementation is in PayloadPkg/Library/UiSetupLib/UiSetupLib.inf.

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef __UI_SETUP_LIB_H__
#define __UI_SETUP_LIB_H__

#include <Uefi.h>

/**
  Run the interactive UiSetup menu.

  @retval EFI_SUCCESS      UiSetup exited normally.
  @retval EFI_UNSUPPORTED  Feature is unavailable.
  @retval Others           Error from UiSetup internals.
**/
EFI_STATUS
EFIAPI
RunUiSetup (
  VOID
  );

/**
  Shell command handler: cfgset <field.path> <value>

  @param[in]  Argc   Argument count (including command name).
  @param[in]  Argv   Argument vector.

  @retval EFI_SUCCESS            Field written and saved.
  @retval EFI_INVALID_PARAMETER  Wrong number of arguments.
  @retval EFI_UNSUPPORTED        Feature is disabled.
  @retval Others                 Parse or save error.
**/
EFI_STATUS
EFIAPI
UiCfgShellCfgSetCommand (
  IN UINTN    Argc,
  IN CHAR16  *Argv[]
  );

/**
  Shell command handler: cfgget <field.path>

  @param[in]  Argc   Argument count (including command name).
  @param[in]  Argv   Argument vector.

  @retval EFI_SUCCESS            Field value printed.
  @retval EFI_INVALID_PARAMETER  Wrong number of arguments.
  @retval EFI_UNSUPPORTED        Feature is disabled.
  @retval Others                 Lookup error.
**/
EFI_STATUS
EFIAPI
UiCfgShellCfgGetCommand (
  IN UINTN    Argc,
  IN CHAR16  *Argv[]
  );

/**
  Shell command handler: cfgfind [pattern] [--verbose]

  @param[in]  Argc   Argument count (including command name).
  @param[in]  Argv   Argument vector.

  @retval EFI_SUCCESS            Fields listed.
  @retval EFI_UNSUPPORTED        Feature is disabled.
  @retval Others                 Lookup error.
**/
EFI_STATUS
EFIAPI
UiCfgShellCfgFindCommand (
  IN UINTN    Argc,
  IN CHAR16  *Argv[]
  );

#endif // __UI_SETUP_LIB_H__
