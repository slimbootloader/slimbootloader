/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/ShellExtensionLib.h>
#include "ShellCmds.h"
#include "Shell.h"

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

  ShellCommandRegister (Shell, &ShellCommandExit);
  ShellCommandRegister (Shell, &ShellCommandHelp);
  ShellCommandRegister (Shell, &ShellCommandHob);
  ShellCommandRegister (Shell, &ShellCommandMm);
  ShellCommandRegister (Shell, &ShellCommandMmap);
  ShellCommandRegister (Shell, &ShellCommandPerf);
  ShellCommandRegister (Shell, &ShellCommandBoot);
  ShellCommandRegister (Shell, &ShellCommandMmcDll);
  ShellCommandRegister (Shell, &ShellCommandCdata);
  ShellCommandRegister (Shell, &ShellCommandDmesg);
  ShellCommandRegister (Shell, &ShellCommandCpuid);
  ShellCommandRegister (Shell, &ShellCommandMsr);
  ShellCommandRegister (Shell, &ShellCommandMtrr);
  ShellCommandRegister (Shell, &ShellCommandPci);
  ShellCommandRegister (Shell, &ShellCommandReset);
  ShellCommandRegister (Shell, &ShellCommandUcode);
  ShellCommandRegister (Shell, &ShellCommandCls);
  ShellCommandRegister (Shell, &ShellCommandFs);

  //
  // Load Platform specific shell commands
  //
  ShellExtensionCmds = GetShellExtensionCmds ();
  for (Iter = ShellExtensionCmds; *Iter != NULL; Iter++) {
    ShellCommandRegister (Shell, *Iter);
  }

  return EFI_SUCCESS;
}
