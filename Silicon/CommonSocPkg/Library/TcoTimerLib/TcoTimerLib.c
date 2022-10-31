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
InitTcoBaseAddress (
  VOID
  )
{
  // Program TCO base address
  PciWrite32 (PCI_LIB_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SMBUS, PCI_FUNCTION_NUMBER_PCH_SMBUS, R_SMBUS_CFG_TCOBASE), TCO_BASE_ADDRESS);

  // Enable TCO base address
  PciWrite32 (PCI_LIB_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SMBUS, PCI_FUNCTION_NUMBER_PCH_SMBUS, R_SMBUS_CFG_TCOCTL), B_SMBUS_CFG_TCOCTL_TCO_BASE_EN);
}

/**
  Stop the TCO timer
**/
VOID
EFIAPI
StopTcoTimer (
  VOID
  )
{
  IoOr16 (TCO_BASE_ADDRESS + R_TCO_IO_TCO1_CNT, B_TCO_IO_TCO1_CNT_TMR_HLT);
}

/**
  Initialize TCO base address and stop the TCO timer

  @param[in] Timeout    Number of 0.6s ticks to wait
**/
VOID
EFIAPI
InitTcoTimer (
  VOID
  )
{
  InitTcoBaseAddress ();
  StopTcoTimer ();
}

/**
  Start a countdown from a value on the TCO timer

  @param[in] Timeout    Number of 0.6s ticks to wait
**/
VOID
EFIAPI
StartTcoTimer (
  IN UINT16 Timeout
  )
{
  // Set the TCO timeout
  IoOr16 (TCO_BASE_ADDRESS + R_TCO_IO_TMR, Timeout);

  // Reload the TCO timer with the timeout
  IoOr16 (TCO_BASE_ADDRESS + R_TCO_IO_RLD, B_TCO_IO_RLD);

  // Un-halt the TCO timer
  IoAnd16 (TCO_BASE_ADDRESS + R_TCO_IO_TCO1_CNT, (UINT16)~B_TCO_IO_TCO1_CNT_TMR_HLT);
}

/**
  Check if current boot was caused by TCO timeout

  @return TRUE if current boot was caused by TCO timeout
  @return FALSE if current boot was not caused by TCO timeout
**/
BOOLEAN
EFIAPI
WasBootCausedByTcoTimeout (
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
