/** @file
  The platform hook library.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/IoLib.h>
#include <RegAccess.h>

/**
  Disable watch dog timer (Halt TCO timer).
**/
VOID
EFIAPI
DisableWatchDogTimer (
  VOID
)
{
  //
  // Halt the TCO timer.
  //
  IoOr16 (ACPI_BASE_ADDRESS + R_TCO1_CNT, B_TCO1_CNT_TMR_HLT);
}

/**
  Enables the execution by writing to the MSR.
**/
VOID
EFIAPI
EnableCodeExecution (
  VOID
)
{
  AsmWriteMsr32 (0x120, 0x100);
}
