/** @file
  An instance for extension shell library

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/ShellLib.h>

extern CONST SHELL_COMMAND mShellCommandPappend;
extern CONST SHELL_COMMAND mShellCommandFwUpdate;

CONST SHELL_COMMAND *mShellExtensionCommands[] = {
  &mShellCommandPappend,
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
