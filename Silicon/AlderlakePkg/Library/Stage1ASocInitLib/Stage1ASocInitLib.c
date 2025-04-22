/** @file

  Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/IoLib.h>
#include <RegAccess.h>
#include <PlatformBase.h>

/**
  Disable watch dog timer

**/
VOID
DisableWatchDogTimer (
  VOID
)
{
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
