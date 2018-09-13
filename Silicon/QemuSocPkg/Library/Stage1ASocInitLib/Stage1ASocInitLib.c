/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/BaseLib.h>

/**
  Disable watch dog timer (Halt TCO timer).
**/
VOID
DisableWatchDogTimer (
  VOID
)
{
}

/**
  Enables the execution by writing to the MSR.
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
  return 8 * 100000;
}
