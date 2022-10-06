/** @file
  Reset System Library

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/ResetSystemLib.h>

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
    break ;

  default:
    return ;
  }

  //
  // Coming here, either we are waiting while system reset is in progress
  // or reset failed and we are in dead loop
  //
  CpuDeadLoop();
}
