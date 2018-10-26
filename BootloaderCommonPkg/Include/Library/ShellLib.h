/** @file
  A minimal command-line shell.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
  CONST SHELL_COMMAND **Commands;
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

// Basic Commands
extern CONST SHELL_COMMAND ShellCommandExit;
extern CONST SHELL_COMMAND ShellCommandHelp;
#define SHELL_COMMANDS_BASIC \
  &ShellCommandExit, \
  &ShellCommandHelp, \

// Standard Commands
extern CONST SHELL_COMMAND ShellCommandHob;
extern CONST SHELL_COMMAND ShellCommandMem;
extern CONST SHELL_COMMAND ShellCommandMmap;
extern CONST SHELL_COMMAND ShellCommandPerf;
extern CONST SHELL_COMMAND ShellCommandBoot;
extern CONST SHELL_COMMAND ShellCommandMmcDll;
extern CONST SHELL_COMMAND ShellCommandCdata;
extern CONST SHELL_COMMAND ShellCommandDmesg;

#define SHELL_COMMANDS_STANDARD \
  &ShellCommandHob, \
  &ShellCommandMem, \
  &ShellCommandMmap, \
  &ShellCommandPerf, \
  &ShellCommandBoot, \
  &ShellCommandMmcDll, \
  &ShellCommandCdata, \
  &ShellCommandDmesg, \

// x86 Architectural Commands
extern CONST SHELL_COMMAND ShellCommandCpuid;
extern CONST SHELL_COMMAND ShellCommandIo;
extern CONST SHELL_COMMAND ShellCommandMsr;
extern CONST SHELL_COMMAND ShellCommandMtrr;
extern CONST SHELL_COMMAND ShellCommandPci;
extern CONST SHELL_COMMAND ShellCommandReset;
extern CONST SHELL_COMMAND ShellCommandUcode;
#define SHELL_COMMANDS_X86 \
  &ShellCommandCpuid, \
  &ShellCommandIo, \
  &ShellCommandMsr, \
  &ShellCommandMtrr, \
  &ShellCommandPci, \
  &ShellCommandReset, \
  &ShellCommandUcode, \

#define SHELL_COMMANDS_DEFAULT \
  SHELL_COMMANDS_BASIC \
  SHELL_COMMANDS_STANDARD \
  SHELL_COMMANDS_X86 \

#endif
