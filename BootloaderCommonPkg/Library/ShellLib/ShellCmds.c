/** @file

  Copyright (c) 2019 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/ShellExtensionLib.h>
#include "ShellCmds.h"
#include "Shell.h"

#define MAX_ADDED_SHELL_COMMANDS  8

STATIC CONST SHELL_COMMAND  *mAddedShellCommands[MAX_ADDED_SHELL_COMMANDS];
STATIC UINTN                mAddedShellCommandCount;

/**
  Add a command to be registered each time the shell starts.

  @param[in]  ShellCommand A Shell Command to be added

  @retval EFI_SUCCESS
  @retval EFI_OUT_OF_RESOURCES
**/
EFI_STATUS
EFIAPI
ShellAddCommand (
  IN  CONST SHELL_COMMAND   *ShellCommand
  )
{
  if (mAddedShellCommandCount >= MAX_ADDED_SHELL_COMMANDS) {
    return EFI_OUT_OF_RESOURCES;
  }

  mAddedShellCommands[mAddedShellCommandCount++] = ShellCommand;
  return EFI_SUCCESS;
}

/**
  Load shell commands.

  @param[in]  Shell        shell instance

  @retval EFI_SUCCESS      Shell command loaded successfully

**/
EFI_STATUS
LoadShellCommands (
  IN SHELL   *Shell
  )
{
  CONST SHELL_COMMAND **ShellExtensionCmds;
  CONST SHELL_COMMAND **Iter;
  UINTN               Index;

  // Basic Shell commands
  ShellCommandRegister (Shell, &ShellCommandExit);
  ShellCommandRegister (Shell, &ShellCommandHelp);
  ShellCommandRegister (Shell, &ShellCommandMm);
  ShellCommandRegister (Shell, &ShellCommandCpuid);
  ShellCommandRegister (Shell, &ShellCommandMsr);
  ShellCommandRegister (Shell, &ShellCommandMtrr);
  ShellCommandRegister (Shell, &ShellCommandUcode);
  ShellCommandRegister (Shell, &ShellCommandCls);

  for (Index = 0; Index < mAddedShellCommandCount; Index++) {
    ShellCommandRegister (Shell, mAddedShellCommands[Index]);
  }

  if (!FeaturePcdGet (PcdMiniShellEnabled)) {
    // More Shell commands
    ShellCommandRegister (Shell, &ShellCommandPci);
    ShellCommandRegister (Shell, &ShellCommandHob);
    ShellCommandRegister (Shell, &ShellCommandMmap);
    ShellCommandRegister (Shell, &ShellCommandPerf);
    ShellCommandRegister (Shell, &ShellCommandBoot);
    ShellCommandRegister (Shell, &ShellCommandMmcDll);
    ShellCommandRegister (Shell, &ShellCommandCdata);
    ShellCommandRegister (Shell, &ShellCommandDmesg);
    ShellCommandRegister (Shell, &ShellCommandReset);
    ShellCommandRegister (Shell, &ShellCommandFs);
    ShellCommandRegister (Shell, &ShellCommandUsbDev);
    ShellCommandRegister (Shell, &ShellCommandAcpi);
    ShellCommandRegister (Shell, &ShellCommandFlashmap);

    // Load Platform specific shell commands
    ShellExtensionCmds = GetShellExtensionCmds ();
    for (Iter = ShellExtensionCmds; *Iter != NULL; Iter++) {
      ShellCommandRegister (Shell, *Iter);
    }

    if (FeaturePcdGet (PcdEnableSetup)) {
      ShellCommandRegister (Shell, &ShellCommandSetup);;
    }

    if (FeaturePcdGet (PcdUiSetupEnabled)) {
      ShellCommandRegister (Shell, &ShellCommandUi);
      ShellCommandRegister (Shell, &ShellCommandCfgSet);
      ShellCommandRegister (Shell, &ShellCommandCfgGet);
      ShellCommandRegister (Shell, &ShellCommandCfgFind);
    }
  }

  return EFI_SUCCESS;
}
