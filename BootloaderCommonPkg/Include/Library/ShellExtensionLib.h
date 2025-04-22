/** @file
  Header file for shell extension library.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _SHELL_EXTENSION_LIB_H_
#define _SHELL_EXTENSION_LIB_H_

#include <PiPei.h>
#include <Library/ShellLib.h>

/**
  Get shell command list supported by extension shell lib

  @retval Pointer of SHELL_COMMAND array.

**/
CONST SHELL_COMMAND **
EFIAPI
GetShellExtensionCmds (
  VOID
  );

#endif
