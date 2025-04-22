/** @file
  Shell command `mmcdll` to display system performance data.

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/MmcAccessLib.h>
#include <Library/MmcTuningLib.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/VariableLib.h>
#include <Guid/OsBootOptionGuid.h>


/**
  Display performance data.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandMmcDllFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandMmcDll = {
  L"mmcdll",
  L"Tune or print MMC DLL data",
  &ShellCommandMmcDllFunc
};

/**
  Print DLL tuning data.

  @param[in]  TuningData    pointer to tuning data

**/
STATIC
VOID
PrintDllData (
  IN EMMC_TUNING_DATA *TuningData
  )
{
  ShellPrint (L"  HS400 |  TX 0x%02x |  RX 0x%02x\n", TuningData->Hs400TxDataDll, TuningData->Hs400RxStrobe1Dll);
}


/**
  Get MMC base address

  This function returns the base address for MMC device

  @retval UINTN       Return MMC base address if device is found
  @retval NULL        Return NULL if it is NOT found.
**/
UINTN
GetMmcBaseAddress (
  VOID
  )
{
  UINTN                         MmcBaseAddress;

  MmcBaseAddress = GetDeviceAddr (OsBootDeviceEmmc, 0);
  MmcBaseAddress = TO_MM_PCI_ADDRESS (MmcBaseAddress);

  return MmcBaseAddress;
}

/**
  Tune or print mmc dll data.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandMmcDllFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  EFI_STATUS                Status;
  UINTN                     EmmcHcPciBase;
  EMMC_TUNING_DATA          EmmcTuningData;
  UINTN                     VariableLen;

  if ((Argc < 2) || (Argc > 3)) {
    goto usage;
  }

  if (StrCmp (Argv[1], L"tune") == 0) {
    EmmcHcPciBase = GetMmcBaseAddress();
    if (EmmcHcPciBase == 0) {
      ShellPrint (L"Invalid base address for Mmc device!\n");
      return EFI_ABORTED;
    }

    MmcInitialize (EmmcHcPciBase, DevInitAll);
    MmcTuning (EmmcHcPciBase);
    VariableLen = sizeof (EmmcTuningData);
    Status = GetVariable (L"MMCDLL", NULL, NULL, &VariableLen, &EmmcTuningData);
    if (EFI_ERROR (Status)) {
      ShellPrint (L"MMC traing fails, not found MMCDLL variable.\n");
      return EFI_DEVICE_ERROR;
    }
    PrintDllData (&EmmcTuningData);
  } else if (StrCmp (Argv[1], L"dump") == 0) {
    ShellPrint (L"Support for dumping dll tuning data will be added shortly!\n");
  } else {
    goto usage;
  }

  return EFI_SUCCESS;

usage:
  ShellPrint (L"Usage: %s <'tune'|'dump'>\n", Argv[0]);
  return EFI_ABORTED;
}

