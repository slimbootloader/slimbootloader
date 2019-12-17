/** @file
  A minimal command-line shell.

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _SHELLLIB_H_
#define _SHELLLIB_H_

#include <PiPei.h>
#include <Library/BaseLib.h>

typedef struct {
  LIST_ENTRY            CommandEntryList;
  CHAR16               *CommandLineHist;
  INTN                  CommandLineIdx;
  UINTN                 CommandLineMaxLen;
  BOOLEAN               ShouldExit;
} SHELL;

typedef EFI_STATUS (EFIAPI *SHELL_COMMAND_ENTRY_FUNC) (SHELL *Shell, UINTN Argc, CHAR16 *Argv[]);

typedef struct {
  CONST CHAR16             *Name;
  CONST CHAR16             *Desc;
  SHELL_COMMAND_ENTRY_FUNC  Entry;
} SHELL_COMMAND;

/**
  Begin a run-time interactive shell.

  @param[in]  Timeout       seconds to wait for input before returning (0 for no timeout)

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
Shell (
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

  @param[in]  Shell        Shell Context
  @param[in]  ShellCommand A Shell Command to be registered

  @retval EFI_SUCCESS
  @retval EFI_OUT_OF_RESOURCES
**/
EFI_STATUS
EFIAPI
ShellCommandRegister (
  IN  SHELL                 *Shell,
  IN  CONST SHELL_COMMAND   *ShellCommand
  );

#endif
