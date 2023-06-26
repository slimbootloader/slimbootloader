/** @file
  Shell command `fwupdate` to reset the system.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/IoLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Service/PlatformService.h>
#include <Library/DebugLib.h>
#include <Library/ShellExtensionLib.h>
#include <Library/FirmwareUpdateLib.h>
#include <Register/PmcRegs.h>
#include <PlatformBase.h>

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
  PLATFORM_SERVICE      *PlatformService;
  UINT32                OcWdtRegData;

  //
  // This is platform specific method.use over-clocking WDT scratchpad bits
  //
  OcWdtRegData = IoRead32 (ACPI_BASE_ADDRESS + R_ACPI_IO_OC_WDT_CTL);
  OcWdtRegData |= BIT16;
  IoWrite32 (ACPI_BASE_ADDRESS + R_ACPI_IO_OC_WDT_CTL, OcWdtRegData);

  //
  // Reset the platform
  //
  PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
  if (PlatformService != NULL && PlatformService->ResetSystem != NULL) {
    PlatformService->ResetSystem(EfiResetWarm);
  }

  return EFI_SUCCESS;
}
