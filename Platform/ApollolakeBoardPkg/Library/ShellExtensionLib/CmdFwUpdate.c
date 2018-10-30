/** @file
  Shell command `fwupdate` to reset the system.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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

  AsciiStrCpy((CHAR8 *)FwUpdUserCfgData->FileName, "FwuImage.bin");

  //
  // Send HECI user command to save IBB signal data
  //
  Status = HeciService->HeciUserCommand((UINT8 *)Data, CDATA_CAPSULE_INFO_LENGTH, 1 );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " HeciSendUserCommand: Status : %r\n", Status));
    return Status;
  }

  //
  // Reset the platform
  //
  PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
  if (PlatformService != NULL && PlatformService->ResetSystem != NULL) {
    PlatformService->ResetSystem(EfiResetWarm);
  }

  return EFI_SUCCESS;
}
