/** @file
  Library for TCO timer.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/TcoTimerLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Register/TcoRegs.h>
#include <Register/RegsSmbus.h>

#define DEFAULT_PCI_BUS_NUMBER_PCH      0
#define PCI_DEVICE_NUMBER_PCH_SMBUS     31
#define PCI_FUNCTION_NUMBER_PCH_SMBUS   4

/**
  Program/enable TCO base address
**/
VOID
EFIAPI
InitTcoTimer (
  VOID
  )
{
  // Program TCO base address
  PciWrite32 (PCI_LIB_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SMBUS, PCI_FUNCTION_NUMBER_PCH_SMBUS, R_SMBUS_CFG_TCOBASE), TCO_BASE_ADDRESS);

  // Enable TCO base address
  PciWrite32 (PCI_LIB_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SMBUS, PCI_FUNCTION_NUMBER_PCH_SMBUS, R_SMBUS_CFG_TCOCTL), B_SMBUS_CFG_TCOCTL_TCO_BASE_EN);
}

/**
  Set the TCO timeout

  @param[in] Timeout    Number of 0.6s ticks to wait
**/
VOID
EFIAPI
SetTcoTimeout (
  IN UINT16 Timeout
  )
{
  IoOr16 (TCO_BASE_ADDRESS + R_TCO_IO_TMR, Timeout);
}

/**
  Resume (un-halt) the TCO timer
**/
VOID
EFIAPI
ResumeTcoTimer (
  VOID
  )
{
  IoAnd16 (TCO_BASE_ADDRESS + R_TCO_IO_TCO1_CNT, (UINT16)~B_TCO_IO_TCO1_CNT_TMR_HLT);
}

/**
  Reload the TCO timer with its timeout
**/
VOID
EFIAPI
RestartTcoTimer (
  VOID
  )
{
  IoOr16 (TCO_BASE_ADDRESS + R_TCO_IO_RLD, B_TCO_IO_RLD);
}

/**
  Check if TCO status indicates failure on previous boot

  @return TRUE if timeout exceeded on previous boot
  @return FALSE if timeout not exceeded on previous boot
**/
BOOLEAN
EFIAPI
WasPreviousTcoTimeout (
  VOID
  )
{
  return IoRead16 (TCO_BASE_ADDRESS + R_TCO_IO_TCO2_STS) & B_TCO_IO_TCO2_STS_SECOND_TO;
}

/**
  Clear TCO status
**/
VOID
EFIAPI
ClearTcoStatus (
  VOID
  )
{
  IoOr16 (TCO_BASE_ADDRESS + R_TCO_IO_TCO2_STS, B_TCO_IO_TCO2_STS_SECOND_TO);
}

/**
  Halt the TCO timer
**/
VOID
EFIAPI
HaltTcoTimer (
  VOID
  )
{
  IoOr16 (TCO_BASE_ADDRESS + R_TCO_IO_TCO1_CNT, B_TCO_IO_TCO1_CNT_TMR_HLT);
}

/**
  Initialize TCO timer, set TCO timeout, and restart TCO timer

  @param[in] Timeout    Number of 0.6s ticks to wait
**/
VOID
EFIAPI
EnableTcoTimer (
  IN UINT16 Timeout
  )
{
  InitTcoTimer ();
  SetTcoTimeout (Timeout);
  RestartTcoTimer ();
}

/**
  Initialize and halt TCO timer
**/
VOID
EFIAPI
DisableTcoTimer (
  VOID
  )
{
  InitTcoTimer ();
  HaltTcoTimer ();
}
