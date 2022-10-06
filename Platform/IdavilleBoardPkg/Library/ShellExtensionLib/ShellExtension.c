/** @file
  Shell command `fwupdate` to reset the system.

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>

extern CONST SHELL_COMMAND mShellCommandFwUpdate;

CONST SHELL_COMMAND *mShellExtensionCommands[] = {
  &mShellCommandFwUpdate,
  NULL,
};

/**
  Get shell command list supported by extension shell lib

  @retval Pointer of SHELL_COMMAND array.

**/
CONST SHELL_COMMAND **
EFIAPI
GetShellExtensionCmds (
  VOID
  )
{
  return mShellExtensionCommands;
}
