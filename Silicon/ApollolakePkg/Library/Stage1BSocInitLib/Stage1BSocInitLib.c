/** @file
  The platform hook library.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/IoLib.h>
#include <RegAccess.h>

/**
  Enables the execution by writing to the MSR.
**/
VOID
EnableCodeExecution (
  VOID
)
{
  AsmWriteMsr32 (0x120, 0x100);
}
