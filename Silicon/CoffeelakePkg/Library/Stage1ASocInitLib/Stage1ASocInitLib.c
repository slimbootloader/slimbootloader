/** @file
  The platform hook library.

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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


UINT32
EFIAPI
GetCpuTscFreqency (
  VOID
)
{
  return ((AsmReadMsr64(0xCE) >> 8) & 0xFF) * 100000;
}
