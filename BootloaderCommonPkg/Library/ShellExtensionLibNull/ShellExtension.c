/** @file
  A NULL instance for extension shell library

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>

CONST SHELL_COMMAND *mShellExtensionCommands[] = {
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
