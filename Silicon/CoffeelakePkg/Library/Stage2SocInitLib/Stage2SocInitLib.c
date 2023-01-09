/** @file
  The platform hook library.

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
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
#include <Register/SaRegs.h>
#include <Library/VTdLib.h>
#include <PlatformData.h>

UINT32
EFIAPI
GetAcpiGnvsSize (
  VOID
)
{
  return sizeof (GLOBAL_NVS_AREA);
}

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
  GLOBAL_NVS_AREA                 *Gnvs;
  UINT8                           Index;
  PLATFORM_DATA                   *PlatformData;

  if (FeaturePcdGet (PcdVtdEnabled)) {
    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
    if (PlatformData != NULL) {
      if (PlatformData->PlatformFeatures.VtdEnable == 0) {
        return;
      }
    }

    Gnvs = (GLOBAL_NVS_AREA *)GnvsIn;
    DEBUG ((DEBUG_INFO, "Initialize VT-d\n"));
    McD0BaseAddress  = PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, 0);
    McD2BaseAddress  = PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, 0);

    Gnvs->SaNvs.VtdDisable = !FeaturePcdGet (PcdVtdEnabled);
    for (Index = 0; Index < SA_VTD_ENGINE_NUMBER; Index++) {
      Gnvs->SaNvs.VtdBaseAddress[Index] = ReadVtdBaseAddress (Index);
    }
    Gnvs->SaNvs.VtdEngine1Vid = PciRead16(McD2BaseAddress + PCI_VENDOR_ID_OFFSET);

    ///
    /// Check if SA supports VTD
    ///
    if ((Gnvs->SaNvs.VtdDisable) || (PciRead32 (McD0BaseAddress + R_SA_MC_CAPID0_A_OFFSET) & BIT23)) {
      DEBUG ((DEBUG_INFO, "VTd disabled or no capability!\n"));
    } else {
      DEBUG ((DEBUG_INFO, "VTd enabled\n"));
    }
  }
}

VOID
EnableLegacyRegions (
  VOID
)
{
  PciWrite32 (PCI_LIB_ADDRESS(0, 0, 0, 0x84), 0x00003333);
  PciWrite32 (PCI_LIB_ADDRESS(0, 0, 0, 0x80), 0x33333330);
}

