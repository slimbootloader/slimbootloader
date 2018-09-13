/** @file
  Shell command `mtrr` to display current MTRR configuration.

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
  Display current MTRR configuration.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandMtrrFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandMtrr = {
  L"mtrr",
  L"Display current MTRR configuration",
  &ShellCommandMtrrFunc
};

/**
  Get string description of MTRR type.

  @param[in]  Type         Numeric MTRR type

  @retval Type string

**/
STATIC
CONST CHAR16 *
EFIAPI
MtrrTypeToStr (
  IN UINTN Type
  )
{
  switch (Type) {
  case 0x00:
    return L"Uncacheable";
  case 0x01:
    return L"Write Combining";
  case 0x04:
    return L"Write-Through";
  case 0x05:
    return L"Write-Protected";
  case 0x06:
    return L"Write-Back";
  default:
    return L"Unknown";
  }
}

/**
  Display current MTRR configuration.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandMtrrFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  UINTN   Index, BaseMsr, MaskMsr, Type;
  BOOLEAN MtrrEnabled, FixedMtrrEnabled, Valid, Verbose;
  UINT64  DefType, BaseVal, Base, MaskVal, Mask, Range, Limit;

  Verbose = FALSE;

  // Check flags
  for (Index = 0; Index < Argc; Index++) {
    if (StrCmp (Argv[Index], L"-h") == 0) {
      goto usage;
    }
    if (StrCmp (Argv[Index], L"-v") == 0) {
      Verbose = TRUE;
    }
  }

  // Dump IA32_MTRR_DEF_TYPE
  DefType          = AsmReadMsr64 (0x2ff);
  MtrrEnabled      = ((DefType & (1 << 11)) != 0);
  FixedMtrrEnabled = ((DefType & (1 << 10)) != 0);
  ShellPrint (L"MTRRs..............: %s\n", MtrrEnabled      ? L"Enabled" : L"Disabled");
  ShellPrint (L"Fixed MTRRs........: %s\n", FixedMtrrEnabled ? L"Enabled" : L"Disabled");
  ShellPrint (L"Default............: %s\n", MtrrTypeToStr (DefType & 0xff));
  ShellPrint (L"\n");

  //
  // TODO: Dump Fixed Range MTRRs
  //

  // Dump Variable MTRR registers
  ShellPrint (L"Variable MTRRs\n");
  for (Index = 0; Index < 10; Index++) {
    BaseMsr = 0x200 + (Index * 2);
    MaskMsr = BaseMsr + 1;

    BaseVal = AsmReadMsr64 (BaseMsr);
    MaskVal = AsmReadMsr64 (MaskMsr);

    Valid = ((MaskVal & (1 << 11)) != 0);
    if (!Valid) {
      continue;
    }

    Type  = BaseVal & 0xff;
    Base  = BaseVal & 0xffffff000ull;
    Mask  = MaskVal & 0xffffff000ull;
    Range = (~Mask & 0xfffffffffull);
    Limit = Base + Range;

    ShellPrint (L"[0x%09llx-0x%09llx] %s\n", Base, Limit, MtrrTypeToStr (Type));
    if (Verbose) {
      ShellPrint (L"   IA32_MTRR_PHYSBASE_%d (0x%x): 0x%016llx\n", Index, BaseMsr, BaseVal);
      ShellPrint (L"   IA32_MTRR_PHYSMASK_%d (0x%x): 0x%016llx\n", Index, MaskMsr, MaskVal);
    }
  }

  return EFI_SUCCESS;

usage:
  ShellPrint (L"Usage: %s [-v]\n", Argv[0]);
  ShellPrint (L"\n"
              L"Flags:\n"
              L"  -v     Verbose\n");
  return EFI_ABORTED;
}
