/** @file
  An instance for extension shell library

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>

extern CONST SHELL_COMMAND mShellCommandFwUpdate;
extern CONST SHELL_COMMAND mShellCommandPappend;

CONST SHELL_COMMAND *mShellExtensionCommands[] = {
  &mShellCommandFwUpdate,
  &mShellCommandPappend,
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
