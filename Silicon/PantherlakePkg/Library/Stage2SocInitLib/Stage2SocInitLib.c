/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <GlobalNvsAreaDef.h>
#include <PlatformData.h>
#include <Register/SaRegsHostBridge.h>
#include <IndustryStandard/Pci30.h>
#include <Include/PlatformData.h>

/**
  Get size of global nvs area.

  @param[in] none

  @retval    UINT32     Size of global nvs area.

**/
UINT32
EFIAPI
GetAcpiGnvsSize (
  VOID
  )
{
  return sizeof (GLOBAL_NVS_AREA);
}

/**
 To enable DRAM Memory legacy region through Program attribute map

 **/
VOID
EnableLegacyRegions (
  VOID
)
{
  PciWrite32 (PCI_LIB_ADDRESS(0, 0, 0, PAM0_0_0_0_PCI_REG), 0x33333330);
  PciWrite32 (PCI_LIB_ADDRESS(0, 0, 0, PAM4_0_0_0_PCI_REG), 0x00333333);
}
