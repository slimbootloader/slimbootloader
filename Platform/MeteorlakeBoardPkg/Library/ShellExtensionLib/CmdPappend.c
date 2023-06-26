/** @file
  Shell command `pappend` to append additional boot parameters.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/ShellLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Service/PlatformService.h>
#include <Library/DebugLib.h>
#include <Library/ShellExtensionLib.h>
#include <Library/BaseMemoryLib.h>

extern CONST CHAR8 *mAppendCmdLineParams;

CHAR8 mCustomParams[128];

CHAR8 *mExtraCmdParams[] = {
  " root=PARTLABEL=primary noapic i915.force_probe=9a49",
  " root=PARTLABEL=primary noapic poll=idle",
};

/**
  Append some parameters to the boot option given.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandPappendFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND mShellCommandPappend = {
  L"pappend",
  L"Append extra parameters to the boot option's command line",
  &ShellCommandPappendFunc
};

/**
  Append some parameters to the boot option given.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandPappendFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  UINTN   Index;
  UINT32  NumOfParams;
  BOOLEAN IsSetN;

  if (Argc <= 1) {
    ShellPrint (L"Usage: pappend {get | setn n | set \"<new params>\" | clear}\n\n");
    ShellPrint (L" 'pappend get' will display the default 'n' options\n");
    ShellPrint (L"               and show the currently selected params.\n");
    ShellPrint (L" 'pappend setn n' select the default option 'n' as the\n");
    ShellPrint (L"               params to use for appending.\n");
    ShellPrint (L" 'pappend set \"<new params>\" ' will take in a \"<new params>\"\n");
    ShellPrint (L"               string to be used as the params to be appended.\n");
    ShellPrint (L" 'pappend clear' will clear the currently selected params.\n\n");
    ShellPrint (L"Examples:\n");
    ShellPrint (L"  pappend get\n");
    ShellPrint (L"  pappend setn 0\n");
    ShellPrint (L"  pappend set \" noapic i915.force_probe=9a49\"\n");
    ShellPrint (L"  pappend clear\n");

    return EFI_SUCCESS;
  }

  NumOfParams = sizeof (mExtraCmdParams) / sizeof (mExtraCmdParams[0]);

  if (StrCmp (Argv[1], L"get") == 0) {
    ShellPrint (L"Default Params List:\n");
    for (Index = 0; Index < NumOfParams; Index++) {
      ShellPrint (L"\tIndex %d: \"%a\"\n", Index, mExtraCmdParams[Index]);
    }
    ShellPrint (L"\nCurrently selected: ");
    if (mAppendCmdLineParams != NULL) {
      ShellPrint (L"\"%a\"\n", mAppendCmdLineParams);
    } else {
      ShellPrint (L"NULL\n");
    }
  } else if (StrCmp (Argv[1], L"clear") == 0) {
    mAppendCmdLineParams = NULL;
    ShellPrint (L"Cleared the currently selected params.\n");
  } else if ((StrCmp (Argv[1], L"setn") == 0) || (StrCmp (Argv[1], L"set") == 0)) {
    IsSetN = StrCmp (Argv[1], L"setn") == 0;
    if (Argc > 3) {
      ShellPrint (L"Too many arguments.\n");
      if (IsSetN == FALSE) {
        ShellPrint (L"Please use \" \" to contain the params string.\n");
      }
      goto ExitPappendCmd;
    } else if (Argc < 3) {
      ShellPrint (L"Too few arguments for set%s. Please specify ", IsSetN ? L"n" : L"");
      if (IsSetN) {
        ShellPrint (L"a default option 'n'.\n");
      } else {
        ShellPrint (L"a \"<new params>\" string.\n");
      }
      goto ExitPappendCmd;
    }
    ShellPrint (L"Setting the selected params...\n");

    if (IsSetN) {
      Index = StrDecimalToUintn (Argv[2]);
      if (Index < NumOfParams) {
        mAppendCmdLineParams = mExtraCmdParams[Index];
      } else {
        ShellPrint (L"Invalid value n=%d.\n", Index);
        goto ExitPappendCmd;
      }
    } else {
      if (StrLen (Argv[2]) < sizeof (mCustomParams)) {
        SetMem (&(mCustomParams[0]), sizeof (mCustomParams), 0);
        UnicodeStrToAsciiStrS (Argv[2], (CHAR8 *)(&mCustomParams), sizeof (mCustomParams));
        mAppendCmdLineParams = &(mCustomParams[0]);
      } else {
        ShellPrint (L"Custom params too long, current len %d, max len %d.\n", StrLen (Argv[2]), sizeof (mCustomParams) - 1);
      }
    }
  } else {
    ShellPrint (L"Invalid option '%s'\n", Argv[1]);
  }

ExitPappendCmd:
  return EFI_SUCCESS;
}
