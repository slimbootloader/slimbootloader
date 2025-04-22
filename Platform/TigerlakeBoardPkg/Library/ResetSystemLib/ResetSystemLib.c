/** @file
  Reset System Library

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/ResetSystemLib.h>
#include <Library/PcdLib.h>
#include <Register/PmcRegs.h>
#include <PchAccess.h>
#include <FspEas/FspApi.h>
#include <PlatformBase.h>

//
// Reset Control Register
//
#define R_RST_CNT                     0xCF9 ///< Reset Control
#define V_RST_CNT_FULLRESET           0x0E
#define V_RST_CNT_HARDRESET           0x06


/**
  Calling this function causes a system-wide reset. This sets
  all circuitry within the system to its initial state. This type of reset
  is asynchronous to system operation and operates without regard to
  cycle boundaries.

  System reset should not return, if it returns, it means the system does
  not support cold reset.
**/
VOID
ResetCold (
  VOID
  )
{
  IoWrite8 ((UINTN) R_RST_CNT, V_RST_CNT_FULLRESET);
}

/**
  Calling this function causes a system-wide initialization. The processors
  are set to their initial state, and pending cycles are not corrupted.

  System reset should not return, if it returns, it means the system does
  not support warm reset.
**/
VOID
ResetWarm (
  VOID
  )
{
  IoWrite8 ((UINTN) R_RST_CNT, V_RST_CNT_HARDRESET);
}

/**
  Calling this function causes the system to enter a power state equivalent
  to the ACPI G2/S5 or G3 states.

  System shutdown should not return, if it returns, it means the system does
  not support shut down reset.
**/
VOID
ResetShutdown (
  VOID
  )
{
  UINT32         Data32;
  ///
  /// Firstly, GPE0_EN should be disabled to avoid any GPI waking up the system from S5
  ///
  IoWrite32 (ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_EN_127_96, 0);

  ///
  /// Secondly, PwrSts register must be cleared
  ///
  /// Write a "1" to bit[8] of power button status register at
  /// (PM_BASE + PM1_STS_OFFSET) to clear this bit
  ///
  IoWrite16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS, B_ACPI_IO_PM1_STS_PWRBTN);

  ///
  /// Finally, transform system into S5 sleep state
  ///
  Data32 = IoRead32 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT);

  Data32 = (UINT32) ((Data32 &~(B_ACPI_IO_PM1_CNT_SLP_TYP + B_ACPI_IO_PM1_CNT_SLP_EN)) | V_ACPI_IO_PM1_CNT_S5);

  IoWrite32 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT, Data32);

  Data32 = Data32 | B_ACPI_IO_PM1_CNT_SLP_EN;

  IoWrite32 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT, Data32);
}

/**
  Calling this function causes the system to enter a power state for platform specific.

**/
VOID
ResetPlatformSpecific (
  VOID
  )
{
  IoWrite8 ((UINTN) R_RST_CNT, V_RST_CNT_FULLRESET);
}

/**
  Resets the entire platform.

  @param[in] ResetType          The type of reset to perform.

**/
VOID
EFIAPI
ResetSystem (
  IN EFI_RESET_TYPE   ResetType
  )
{
  switch (ResetType) {
  case EfiResetWarm:
    ResetWarm ();
    break;

  case EfiResetCold:
    ResetCold ();
    break;

  case EfiResetShutdown:
    ResetShutdown ();
    break;

  case EfiResetPlatformSpecific:
    ResetPlatformSpecific ();

  default:
    break;
  }

  //
  // Coming here, either we are waiting while system reset is in progress
  // or reset failed and we are in dead loop
  //
  CpuDeadLoop();
}
