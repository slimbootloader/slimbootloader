/** @file
  Shell command `msr` to read/write model-specific registers.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>

/**
  Read or write model specific registers.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandMsrFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandMsr = {
  L"msr",
  L"Read or write model specific registers",
  &ShellCommandMsrFunc
};

/**
  Read or write model specific registers.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandMsrFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  BOOLEAN Write;
  UINT32  Addr;
  UINT64  Value;

  if (Argc == 2) {
    Write = FALSE;
  } else if (Argc == 3) {
    Write = TRUE;
    Value = StrHexToUint64 (Argv[2]);
  } else {
    goto usage;
  }

  Addr = (UINT32)StrHexToUintn (Argv[1]);

  if (Write) {
    AsmWriteMsr64 (Addr, Value);
  } else {
    Value = AsmReadMsr64 (Addr);
    ShellPrint (L"0x%x: 0x%llx\n", Addr, Value);
  }

  return EFI_SUCCESS;

usage:
  ShellPrint (L"Usage: %s <addr> [<value>]\n", Argv[0]);
  return EFI_ABORTED;
}
