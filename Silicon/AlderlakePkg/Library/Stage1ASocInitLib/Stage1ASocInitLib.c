/** @file

  Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/IoLib.h>
#include <RegAccess.h>
#include <PlatformBase.h>
#include <Register/TcoRegs.h>

/**
  Disable watch dog timer

**/
VOID
DisableWatchDogTimer (
  VOID
)
{
  //
  // Halt the TCO timer
  //
  IoOr16 (TCO_BASE_ADDRESS + R_TCO_IO_TCO1_CNT, B_TCO_IO_TCO1_CNT_TMR_HLT);

  //
  // Clear the Second TO status bit
  //
  IoOr8 (TCO_BASE_ADDRESS + R_TCO_IO_TCO2_STS, B_TCO_IO_TCO2_STS_SECOND_TO);
}

/**
  Enables the execution

**/
VOID
EnableCodeExecution (
  VOID
)
{
}

/**
  Get cpu tsc frequency.

  @retval   cpu TSC frequency

**/
UINT32
EFIAPI
GetCpuTscFreqency (
  VOID
  )
{
  return ((AsmReadMsr64 (0xCE) >> 8) & 0xFF) * 100000;
}
