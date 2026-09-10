/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/PchPciBdfLib.h>
#include <PchAccess.h>
#include <LowPowerSupport.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Pci30.h>
#include <Library/BaseMemoryLib.h>

/**
  Update Pep constraints.

  @param  Void
  @retval Updated Pep constraints Value
 */
UINT64
EFIAPI
GetLowPowerS0IdleConstraint(
  VOID
  )
{
  return 0;
}
