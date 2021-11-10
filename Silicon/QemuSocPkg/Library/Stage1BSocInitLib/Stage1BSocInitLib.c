/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <RegAccess.h>
#include <Library/BootloaderCoreLib.h>

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
 Update memory map related info using SOC registers.

 **/
VOID
EFIAPI
UpdateMemoryInfo (
  VOID
)
{
}
