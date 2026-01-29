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
#include <SaRegsPci000Ptl.h>
#include <Register/SaRegsHostBridge.h>
#include <Register/IgdRegs.h>
#include <Register/IpuRegs.h>
#include <Register/VtdRegs.h>
#include <IndustryStandard/Pci30.h>
#include <Include/PlatformData.h>
#include <Include/CpuRegs.h>

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
 Update GPIO address and length to global NVS data.

 @param [in] GnvsIn Pointer to Global NVS data.

 **/
VOID
EFIAPI
SocUpdateAcpiGnvs (
  IN VOID  *GnvsIn
  )
{
  ///
  /// Vtd Initialization
  ///
  UINTN                           McD0BaseAddress;
  UINTN                           McD2BaseAddress;
  UINTN                           McD5BaseAddress;
  PLATFORM_DATA                   *PlatformData;

  if (FeaturePcdGet (PcdVtdEnabled)) {
    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
    if (PlatformData != NULL) {
      if (PlatformData->PlatformFeatures.VtdEnable == 0) {
        return;
      }
    } else {
      return;
    }

    McD0BaseAddress  = PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, 0);
    McD2BaseAddress  = PCI_LIB_ADDRESS (IGD_BUS_NUM, IGD_DEV_NUM, IGD_FUN_NUM, 0);
    McD5BaseAddress  = PCI_LIB_ADDRESS (IPU_BUS_NUM, IPU_DEV_NUM, IPU_FUN_NUM, 0);

    DEBUG ((DEBUG_INFO, "McD0BaseAddress 0x%x McD2BaseAddress 0x%x McD5BaseAddress 0x%x\n",McD0BaseAddress,McD2BaseAddress,McD5BaseAddress));
    ///
    /// Check if SA supports VTD
    ///
    if (((!PlatformData->PlatformFeatures.VtdEnable)) || (PciRead32 (McD0BaseAddress + R_SA_MC_CAPID0_A_OFFSET) & BIT23)) {
      DEBUG ((DEBUG_INFO, "VTd disabled or no capability!\n"));
    } else {
      DEBUG ((DEBUG_INFO, "VTd enabled\n"));
   }
  }
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
