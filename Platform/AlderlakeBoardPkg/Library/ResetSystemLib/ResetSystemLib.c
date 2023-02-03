/** @file
  Reset System Library

  Copyright (c) 2020 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/ResetSystemLib.h>
#include <Library/PcdLib.h>
#include <PchAccess.h>
#include <FspEas/FspApi.h>

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
  Calling this function causes a PCH Global reset in addition to system-wide
  initialization.

  System reset should not return, if it returns, it means the system does
  not support warm reset.
**/
VOID
ResetPchGlobal (
  VOID
  )
{
  // Enable PMC Global reset on FSP_STATUS_RESET_REQUIRED_3
  MmioOr32 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_ETR3, (UINT32) (B_PMC_PWRM_ETR3_CF9GR));
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
  EFI_STATUS    FspResetRequest = 0;

  switch (ResetType) {
  case EfiResetWarm:
    ResetWarm ();
    break;

  case EfiResetCold:
    ResetCold ();
    break;

  case EfiResetShutdown:
    ResetShutdown ();
    break ;

  case EfiResetPlatformSpecific:
    FspResetRequest = (EFI_STATUS)PcdGet32(PcdFspResetStatus);
    if (FspResetRequest == FSP_STATUS_RESET_REQUIRED_3) {
      ResetPchGlobal ();
    }


  default:
    return ;
  }

  //
  // Coming here, either we are waiting while system reset is in progress
  // or reset failed and we are in dead loop
  //
  CpuDeadLoop();
}
