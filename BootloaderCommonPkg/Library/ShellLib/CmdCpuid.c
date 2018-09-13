/** @file
  Shell command `Cpuid` to read CPU specific information.

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
  Read CPU specific information using CPUID instruction.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandCpuidFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandCpuid = {
  L"cpuid",
  L"Read CPU specific information",
  &ShellCommandCpuidFunc
};

/**
  Read CPU specific information using CPUID instruction.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandCpuidFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  UINT32   Index;
  UINT32   SubIndex;
  UINT32   Eax;
  UINT32   Ebx;
  UINT32   Ecx;
  UINT32   Edx;

  if ((Argc < 2) || (Argc > 3)) {
    ShellPrint (L"Usage: %s <index> [<subindex>]\n", Argv[0]);
    return EFI_ABORTED;
  }

  Index = StrHexToUintn (Argv[1]);
  if (Argc == 3) {
    SubIndex = StrHexToUintn (Argv[2]);
  } else {
    SubIndex = 0;
  }

  AsmCpuidEx (Index, SubIndex, &Eax, &Ebx, &Ecx, &Edx);

  ShellPrint (L"cpuid (eax=0x%x, ecx=0x%x):\n", Index, SubIndex);
  ShellPrint (L"  eax=0x%08X, ebx=0x%08X, ecx=0x%08X, edx=0x%08X\n", Eax, Ebx, Ecx, Edx);

  return EFI_SUCCESS;
}
