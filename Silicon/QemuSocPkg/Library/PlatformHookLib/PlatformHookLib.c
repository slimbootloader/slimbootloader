/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Pci.h>
#include <RegAccess.h>

RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{	
  return RETURN_SUCCESS;
}
