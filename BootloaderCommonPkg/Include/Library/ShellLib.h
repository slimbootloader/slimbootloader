/** @file
  A minimal command-line shell.

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _SHELLLIB_H_
#define _SHELLLIB_H_

#include <PiPei.h>
#include <Library/BaseLib.h>

#define MAX_COMMAND_LINE_LEN 256

typedef struct _SHELL SHELL;

typedef EFI_STATUS (EFIAPI *SHELL_COMMAND_ENTRY_FUNC) (SHELL *Shell, UINTN Argc, CHAR16 *Argv[]);

typedef struct {
  CONST CHAR16             *Name;
  CONST CHAR16             *Desc;
  SHELL_COMMAND_ENTRY_FUNC  Entry;
} SHELL_COMMAND;

struct _SHELL {
  BOOLEAN               ShouldExit;
};

/**
  Begin a run-time interactive shell.

  @param[in]  Commands      command list (may be NULL for default commands)
  @param[in]  Timeout       seconds to wait for input before returning (0 for no timeout)

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
Shell (
  IN CONST SHELL_COMMAND **Commands,
  IN       UINTN           Timeout
  );

/**
  Prints a message to the serial port.

  If Format is NULL, then ASSERT().

  @param  Format      Format string for the message to print.
  @param  ...         Variable argument list whose contents are accessed
                      based on the format string specified by Format.

  @retval Number of characters written

**/
UINTN
EFIAPI
ShellPrint (
  IN  CONST CHAR16 *Format,
  ...
  );

/**
  Register a Shell Command

  @param[in]  ShellCommand A Shell Command to be registered

  @retval EFI_SUCCESS
  @retval EFI_OUT_OF_RESOURCES
**/
EFI_STATUS
EFIAPI
ShellCommandRegister (
  IN  CONST SHELL_COMMAND   *ShellCommand
  );

#endif
