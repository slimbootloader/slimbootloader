/** @file
  The platform hook library.

  Copyright (c) 2013 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <CpuRegsAccess.h>

/**
  Enables the execution by writing to the MSR.
**/
VOID
EFIAPI
EnableCodeExecution (
  VOID
)
{

}

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
  // Program and enable TCO base address.
  //
  PciWrite32 (PCI_LIB_ADDRESS(0, 31, 4, 0x50), TCO_BASE_ADDRESS);
  PciWrite32 (PCI_LIB_ADDRESS(0, 31, 4, 0x54), BIT8);

  //
  // Halt the TCO timer.
  //
  IoOr16 (TCO_BASE_ADDRESS + 0x8, BIT11);
}
