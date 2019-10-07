/** @file
  The platform hook library.

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <PlatformBase.h>
#include <RegAccess.h>
#include <Library/IoLib.h>

VOID
EnableCodeExecution (
  VOID
)
{

}

VOID
DisableWatchDogTimer (
  VOID
)
{
  ///
  /// Halt the TCO timer as early as possible
  ///
  IoWrite16 (TCO_BASE_ADDRESS + R_TCO_IO_TCO1_CNT, B_TCO_IO_TCO1_CNT_TMR_HLT);
}
