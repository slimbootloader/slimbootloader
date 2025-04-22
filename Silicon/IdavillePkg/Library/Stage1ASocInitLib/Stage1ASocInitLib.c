/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/IoLib.h>
#include <RegAccess.h>
#include <PlatformBase.h>

#define R_PCH_TCO1_CNT            0x8
#define B_PCH_TCO_CNT_TMR_HLT     BIT11
#define R_PCH_TCO2_STS            0x6
#define B_PCH_TCO2_STS_SECOND_TO  BIT1

/**
  Disable watch dog timer

**/
VOID
EFIAPI
DisableWatchDogTimer (
  VOID
)
{
  //
  // Halt the TCO timer
  //
  IoOr16 (TCO_BASE_ADDRESS + R_PCH_TCO1_CNT, B_PCH_TCO_CNT_TMR_HLT);

  //
  // Clear the Second TO status bit
  //
  IoOr8 (TCO_BASE_ADDRESS + R_PCH_TCO2_STS, B_PCH_TCO2_STS_SECOND_TO);
}

/**
  Enables the execution

**/
VOID
EFIAPI
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
