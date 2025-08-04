/** @file

  Copyright (c) 2019 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SHELL_CMDS_H__
#define __SHELL_CMDS_H__

#include <Library/ShellLib.h>

extern CONST SHELL_COMMAND ShellCommandExit;
extern CONST SHELL_COMMAND ShellCommandHelp;
extern CONST SHELL_COMMAND ShellCommandHob;
extern CONST SHELL_COMMAND ShellCommandMm;
extern CONST SHELL_COMMAND ShellCommandMmap;
extern CONST SHELL_COMMAND ShellCommandPerf;
extern CONST SHELL_COMMAND ShellCommandBoot;
extern CONST SHELL_COMMAND ShellCommandMmcDll;
extern CONST SHELL_COMMAND ShellCommandCdata;
extern CONST SHELL_COMMAND ShellCommandDmesg;
extern CONST SHELL_COMMAND ShellCommandCpuid;
extern CONST SHELL_COMMAND ShellCommandMsr;
extern CONST SHELL_COMMAND ShellCommandMtrr;
extern CONST SHELL_COMMAND ShellCommandPci;
extern CONST SHELL_COMMAND ShellCommandReset;
extern CONST SHELL_COMMAND ShellCommandUcode;
extern CONST SHELL_COMMAND ShellCommandCls;
extern CONST SHELL_COMMAND ShellCommandFs;
extern CONST SHELL_COMMAND ShellCommandUsbDev;
extern CONST SHELL_COMMAND ShellCommandCorruptComp;
extern CONST SHELL_COMMAND ShellCommandAcpi;
extern CONST SHELL_COMMAND ShellCommandSetup;

/**
  Load shell commands.

  @param[in]  Shell        shell instance

  @retval EFI_SUCCESS      Shell command loaded successfully

**/
EFI_STATUS
LoadShellCommands (
  IN SHELL   *Shell
  );

#endif /* __SHELL_CMDS_H__ */
