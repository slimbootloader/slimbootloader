/** @file
  Shell command `msr` to read/write model-specific registers.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
  UINTN   Addr;
  UINT64  Value;

  if (Argc == 2) {
    Write = FALSE;
  } else if (Argc == 3) {
    Write = TRUE;
    Value = StrHexToUint64 (Argv[2]);
  } else {
    goto usage;
  }

  Addr = StrHexToUintn (Argv[1]);

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
