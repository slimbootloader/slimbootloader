/** @file
  PCH SCI device library.

  Copyright (c) 2019 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <PchAccess.h>
#include <Register/PchRegsSciEhl.h>

/**
  Check whether Sycamore Island (SCI) is supported by PCH Series.

  @retval TRUE                    SCI is supported in current PCH
  @retval FALSE                   SCI is not supported on current PCH
**/
BOOLEAN
EFIAPI
PchIsSciSupported (
  VOID
  )
{
  UINTN  PciBaseAddress;
  PciBaseAddress = PCI_LIB_ADDRESS (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_SCI,
                     PCI_FUNCTION_NUMBER_PCH_SCI,
                     R_SCI_CFG_DEVICE_ID
                     );

  if (PciRead16 (PciBaseAddress) != 0xFFFF) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Query SCI boot status. This routine is only valid after
  PCI enumeration has completed since it relies on BAR0 to
  have been programmed already.

  @retval    BOOLEAN   TRUE if SCI boot successful, else FALSE
**/
BOOLEAN
EFIAPI
SciBootSuccess (
  VOID
  )
{
  UINTN                           PciBaseAddress;
  UINT32                          MmioBase;
  BOOLEAN                         SciBootSts;

  SciBootSts = FALSE;

  PciBaseAddress = PCI_LIB_ADDRESS (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_SCI,
                     PCI_FUNCTION_NUMBER_PCH_SCI,
                     R_SCI_CFG_DEVICE_ID
                     );

  MmioBase = PciRead32 (PciBaseAddress + PCI_BASE_ADDRESSREG_OFFSET) & ~(0xF);

  if (V_SCI_MEM_BOOT_STATUS == MmioRead32 (MmioBase + R_SCI_MEM_SCRATCHPAD) ) {
    SciBootSts = TRUE;
  }

  return SciBootSts;
}
