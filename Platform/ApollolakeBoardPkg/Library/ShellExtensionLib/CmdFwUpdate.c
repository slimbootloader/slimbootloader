/** @file
  Shell command `fwupdate` to reset the system.

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/IoLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Service/PlatformService.h>
#include <Library/ConfigDataLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/ShellExtensionLib.h>
#include <Service/HeciService.h>
#include <ConfigDataCommonStruct.h>
#include <Library/FirmwareUpdateLib.h>
#include <ScRegs/RegsPmc.h>

#define CDATA_CAPSULE_INFO_LENGTH 128
#define CDATA_HEADER_LENGTH       8

/**
  Reset the system.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandFwUpdateFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND mShellCommandFwUpdate = {
  L"fwupdate",
  L"Initiate Firmware Update",
  &ShellCommandFwUpdateFunc
};

/**
  Print the the command usage.

  @param[in]  Argv    command line arguments

**/
VOID
FwuPrintUsage (
  IN CHAR16 *Argv[]
  )
{
  ShellPrint (L"Usage: %s [-h|-p|-c]\n", Argv[0]);
  ShellPrint (L"  -h: print the usage\n"
              L"  -p: use platform register to trigger firmware update. It is the default.\n"
              L"  -c: use CSME command to trigger firmware update.\n");
}

/**
  Initiate firmware update.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandFwUpdateFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  CDATA_BLOB            *UserCfgData;
  CDATA_HEADER          *CdataHeader;
  CAPSULE_INFO_CFG_DATA *FwUpdUserCfgData;
  UINT8                 Data[CDATA_CAPSULE_INFO_LENGTH];
  PLATFORM_SERVICE      *PlatformService;
  HECI_SERVICE          *HeciService;
  UINT32                CdataLength;
  EFI_STATUS            Status;
  BOOLEAN               ShowHelp;
  BOOLEAN               UseRegister;

  ShowHelp    = FALSE;
  UseRegister = TRUE;
  if (Argc > 2) {
    ShowHelp = TRUE;
  } else if (Argc == 2) {
    if (StrCmp (Argv[1], L"-c") == 0) {
      UseRegister = FALSE;
    } else if (StrCmp (Argv[1], L"-p") == 0) {
      UseRegister = TRUE;
    } else {
      ShowHelp = TRUE;
    }
  }

  if (ShowHelp) {
    FwuPrintUsage (Argv);
    return EFI_SUCCESS;
  }

  DEBUG((DEBUG_INFO, "Use %a interface for FWU\n", UseRegister ? "register" : "CSME"));

  if (UseRegister) {
    // Check if platform firmware update trigger is set.
    // Only [16:23] are used by FWU
    MmioAndThenOr32 (PMC_BASE_ADDRESS + R_PMC_BIOS_SCRATCHPAD, 0xFF00FFFF, BIT16);
  } else {
    // Use HECI interface
    HeciService = (HECI_SERVICE *) GetServiceBySignature (HECI_SERVICE_SIGNATURE);
    if ((HeciService == NULL) || (HeciService->SimpleHeciCommand == NULL)) {
      ShellPrint (L"Firmware update command is not supported.\n");
      return EFI_UNSUPPORTED;
    }

    ZeroMem (Data, CDATA_CAPSULE_INFO_LENGTH);

    UserCfgData = (CDATA_BLOB *)(&Data[0]);

    //
    // Populate configuration data header
    //
    CdataLength = (CDATA_HEADER_LENGTH + sizeof(CAPSULE_INFO_CFG_DATA));
    UserCfgData->Signature = CFG_DATA_SIGNATURE;
    UserCfgData->HeaderLength = sizeof(CDATA_BLOB);
    UserCfgData->UsedLength  = sizeof(CDATA_BLOB)  + CdataLength;
    UserCfgData->TotalLength = CDATA_CAPSULE_INFO_LENGTH;

    //
    // Populate CDATA header for the item
    //
    CdataHeader = (CDATA_HEADER *)(&Data[sizeof(CDATA_BLOB)]);
    CdataHeader->ConditionNum = 1;
    CdataHeader->Length = CdataLength >> 2;
    CdataHeader->Version = 1;
    CdataHeader->Tag = CDATA_CAPSULE_INFO_TAG;
    CdataHeader->Condition[0].Value = 0xFFFFFFFF;

    //
    // Populate firmware update user configuration data structure
    //
    FwUpdUserCfgData = (CAPSULE_INFO_CFG_DATA *)(&Data[sizeof(CDATA_BLOB) + CDATA_HEADER_LENGTH]);
    FwUpdUserCfgData->DevType     = 5;
    FwUpdUserCfgData->DevInstance = 0;
    FwUpdUserCfgData->HwPart      = 0;
    FwUpdUserCfgData->SwPart      = 0;
    FwUpdUserCfgData->FsType      = 2;
    FwUpdUserCfgData->LbaAddr     = 0;

    AsciiStrCpyS ((CHAR8 *)FwUpdUserCfgData->FileName, sizeof(FwUpdUserCfgData->FileName), "FwuImage.bin");

    //
    // Send HECI user command to save IBB signal data
    //
    Status = HeciService->HeciUserCommand((UINT8 *)Data, CDATA_CAPSULE_INFO_LENGTH, 1 );
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, " HeciSendUserCommand: Status : %r\n", Status));
      return Status;
    }
  }

  //
  // Reset the platform
  //
  PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
  if (PlatformService != NULL && PlatformService->ResetSystem != NULL) {
    AsmWbinvd ();
    PlatformService->ResetSystem(EfiResetWarm);
  }

  return EFI_SUCCESS;
}
