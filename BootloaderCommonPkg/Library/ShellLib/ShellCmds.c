/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/ShellExtensionLib.h>
#include "ShellCmds.h"
#include "Shell.h"

EFI_STATUS
LoadShellCommands (
  VOID
  )
{
  LIST_ENTRY           *EntryList;
  CONST SHELL_COMMAND **ShellExtensionCmds;
  CONST SHELL_COMMAND **Iter;

  EntryList = GetShellCommandEntryList ();
  InitializeListHead (EntryList);

  ShellCommandRegister (&ShellCommandExit);
  ShellCommandRegister (&ShellCommandHelp);
  ShellCommandRegister (&ShellCommandHob);
  ShellCommandRegister (&ShellCommandMm);
  ShellCommandRegister (&ShellCommandMmap);
  ShellCommandRegister (&ShellCommandPerf);
  ShellCommandRegister (&ShellCommandBoot);
  ShellCommandRegister (&ShellCommandMmcDll);
  ShellCommandRegister (&ShellCommandCdata);
  ShellCommandRegister (&ShellCommandDmesg);
  ShellCommandRegister (&ShellCommandCpuid);
  ShellCommandRegister (&ShellCommandMsr);
  ShellCommandRegister (&ShellCommandMtrr);
  ShellCommandRegister (&ShellCommandPci);
  ShellCommandRegister (&ShellCommandReset);
  ShellCommandRegister (&ShellCommandUcode);
  ShellCommandRegister (&ShellCommandCls);
  ShellCommandRegister (&ShellCommandFs);

  //
  // Load Platform specific shell commands
  //
  ShellExtensionCmds = GetShellExtensionCmds ();
  for (Iter = ShellExtensionCmds; *Iter != NULL; Iter++) {
    ShellCommandRegister (*Iter);
  }

  return EFI_SUCCESS;
}
