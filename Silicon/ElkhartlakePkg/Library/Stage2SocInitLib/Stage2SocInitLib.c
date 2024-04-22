/** @file
  The platform hook library.

  Copyright (c) 2013 - 2020, Intel Corporation. All rights reserved.<BR>
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
#include <Library/IoLib.h>
#include <GlobalNvsAreaDef.h>
#include <Register/SaRegs.h>
#include <PchAccess.h>
#include <Register/PchRegsSmbus.h>
#include <Register/PmcRegs.h>
#include <Library/VTdLib.h>

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
  Clear PME status
**/
VOID
EFIAPI
ClearPmeStatus (
  UINTN Base,
  UINT32 Reg,
  UINT16 Pos
)
{
  UINT16 Val;

  if (PciRead32 (Base) == 0xFFFFFFFF) {
    return;
  }

  Val = PciRead16 (Base + Reg);
  if (Val & Pos) {
    PciWrite16(Base + Reg, Val | Pos);
  }

  Val = PciRead16 (Base + Reg);
  DEBUG ((DEBUG_INFO, "ClearPmStatus: Base: 0x%x Reg(0x%x): 0x%x\n",
          Base, Reg, Val));
}


/**
  Clear SMBUS status and SMB_WAK_STS of GPE0
**/
VOID
EFIAPI
ClearSmbusStatus (
  VOID
)
{
  UINTN                       SmbusRegBase;
  UINT16                      SmbusIoBase;

  SmbusRegBase = PCI_LIB_ADDRESS (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_SMBUS,
                   PCI_FUNCTION_NUMBER_PCH_SMBUS,
                   0
                   );

  if (PciRead32 (SmbusRegBase) == 0xFFFFFFFF) {
    return;
  }

  SmbusIoBase = PciRead16 (SmbusRegBase + R_SMBUS_CFG_BASE) & B_SMBUS_CFG_BASE_BAR;
  if (SmbusIoBase == 0) {
    return;
  }

  PciOr8 (SmbusRegBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_IO_SPACE);
  //
  // Clear SMBUS status and SMB_WAK_STS of GPE0
  //
  IoWrite8 (SmbusIoBase + R_SMBUS_IO_HSTS, B_SMBUS_IO_SMBALERT_STS);
  IoWrite32 (ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_STS_127_96, B_ACPI_IO_GPE0_STS_127_96_SMB_WAK);
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
  GLOBAL_NVS_AREA                 *Gnvs;
  UINT8                           Index;

  if (FeaturePcdGet (PcdVtdEnabled)) {
    Gnvs = (GLOBAL_NVS_AREA *)GnvsIn;
    DEBUG ((DEBUG_INFO, "Initialize VT-d\n"));
    McD0BaseAddress  = PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, 0);
    McD2BaseAddress  = PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, 0);

    Gnvs->SaNvs.VtdDisable = ~(FeaturePcdGet (PcdVtdEnabled));
    for (Index = 0; Index < VTD_ENGINE_NUMBER; Index++) {
      Gnvs->SaNvs.VtdBaseAddress[Index] = ReadVtdBaseAddress (Index);
    }
    Gnvs->SaNvs.VtdEngine1Vid = PciRead16(McD2BaseAddress + PCI_VENDOR_ID_OFFSET);

    ///
    /// Check if SA supports VTD
    ///
    if ((PciRead32 (McD0BaseAddress + R_SA_MC_CAPID0_A_OFFSET) & BIT23)) {
      DEBUG ((DEBUG_WARN, "VTd disabled or no capability!\n"));
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
