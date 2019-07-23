/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "ShellCmds.h"

EFI_STATUS
LoadShellCommands (
  VOID
  )
{
  ShellCommandRegister (&ShellCommandExit);
  ShellCommandRegister (&ShellCommandHelp);
  ShellCommandRegister (&ShellCommandHob);
  ShellCommandRegister (&ShellCommandMem);
  ShellCommandRegister (&ShellCommandMmap);
  ShellCommandRegister (&ShellCommandPerf);
  ShellCommandRegister (&ShellCommandBoot);
  ShellCommandRegister (&ShellCommandMmcDll);
  ShellCommandRegister (&ShellCommandCdata);
  ShellCommandRegister (&ShellCommandDmesg);
  ShellCommandRegister (&ShellCommandCpuid);
  ShellCommandRegister (&ShellCommandIo);
  ShellCommandRegister (&ShellCommandMsr);
  ShellCommandRegister (&ShellCommandMtrr);
  ShellCommandRegister (&ShellCommandPci);
  ShellCommandRegister (&ShellCommandReset);
  ShellCommandRegister (&ShellCommandUcode);

  return EFI_SUCCESS;
}
