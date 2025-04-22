/** @file
  Shell command `fwupdate` to reset the system.

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/IoLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Service/PlatformService.h>
#include <Library/DebugLib.h>
#include <Library/ShellExtensionLib.h>
#include <Library/FirmwareUpdateLib.h>
#include <Register/UBOX_MISC.h>
#include <IndustryStandard/Pci22.h>
#include <Library/PcdLib.h>

#define FWU_BOOT_MODE_OFFSET   0x40
#define FWU_BOOT_MODE_VALUE    0x5A

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
  PLATFORM_SERVICE     *PlatformService;
  UINTN                 Address;

  //
  // BIOSSCRATCHPAD3_CFG (Bus PCI_MAX_BUS-1, Dev 0, Func 2 PCI_Config 0xAC)
  // If BIT0 is set, indicates FWUpdate is triggered.
  //
  Address = MM_PCI_ADDRESS (PCI_MAX_BUS-1, UBOX_DECS_DEV_NUM, UBOX_DECS_FUNC_NUM, 0);
  MmioOr32 (Address + R_BIOSSCRATCHPAD3_UBOX_DECS, BIT0);

  //
  // Reset the platform
  //
  PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
  if (PlatformService != NULL && PlatformService->ResetSystem != NULL) {
    PlatformService->ResetSystem(EfiResetWarm);
  }

  return EFI_SUCCESS;
}
