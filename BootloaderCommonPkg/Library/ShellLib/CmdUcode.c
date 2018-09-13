/** @file
  Shell command `ucode` to display the loaded microcode patch version.

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
  Display microcode version.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandUcodeFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandUcode = {
  L"ucode",
  L"Display microcode version",
  &ShellCommandUcodeFunc
};

/**
  Display microcode version.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandUcodeFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  UINT64 Value;

  AsmWriteMsr64 (0x8b, 0LL);
  AsmCpuid (0, NULL, NULL, NULL, NULL);
  Value = AsmReadMsr64 (0x8b);

  ShellPrint (L"IA32_BIOS_SIGN_ID: 0x%llx\n", Value);

  return EFI_SUCCESS;
}
