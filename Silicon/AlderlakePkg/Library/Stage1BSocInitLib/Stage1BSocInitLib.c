/** @file

  Copyright (c) 2019 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Register/SaRegsHostBridge.h>
#include <Guid/OsBootOptionGuid.h>
#include <Register/PmcRegs.h>
#include <Register/PchBdfAssignment.h>
#include <Register/RegsSmbus.h>
#include <Register/TcoRegs.h>

#define  SMBUS_PORT_ID            0xC6
#define  GENERAL_CONTROL_REG      0x0C
#define  TCO_STS_NO_REBOOT        BIT2

/**
  Enables the execution
**/
VOID
EnableCodeExecution (
  VOID
)
{
}

/**
  Check if boot is caused by watch dog timer

  @RETVAL   TRUE     It is caused by watch dog timer timeout.
  @RETVAL   FALSE    It is not caused by watch dog timer timeout.
**/
BOOLEAN
IsRebootByWdt (
  VOID
)
{
  UINT32   TcoBase;
  UINT32   SbRegBase;
  UINT32   GeneralControlReg;
  UINT16   TcoSts2Reg;
  BOOLEAN  RebootByWdt;

  RebootByWdt = FALSE;

  // Get No_Reboot from SmBus PCR register via P2SB base address
  SbRegBase  = PciRead32 (PCI_LIB_ADDRESS(0, PCI_DEVICE_NUMBER_PCH_P2SB, PCI_FUNCTION_NUMBER_PCH_P2SB, 0x10)) & 0xFF000000;
  GeneralControlReg = MmioRead32  (SbRegBase + ((SMBUS_PORT_ID) << 16) + GENERAL_CONTROL_REG);

  // Get Tco Status2 from from SmBus config space
  TcoBase     = PciRead32 (PCI_LIB_ADDRESS(0, PCI_DEVICE_NUMBER_PCH_SMBUS, PCI_FUNCTION_NUMBER_PCH_SMBUS, R_SMBUS_CFG_TCOBASE)) & B_SMBUS_CFG_TCOBASE_BAR;
  TcoSts2Reg  = IoRead16 (TcoBase + R_TCO_IO_TCO2_STS);

  if (((GeneralControlReg & TCO_STS_NO_REBOOT) == 0) && ((TcoSts2Reg & B_TCO_IO_TCO2_STS_SECOND_TO) != 0)) {
    RebootByWdt = TRUE;
  }

  return RebootByWdt;
}


/**
  Get the reset reason and save to global variable.
**/
VOID
EFIAPI
UpdateResetReason (
  VOID
  )
{
  RESET_CAUSE              RstCause;
  UINT32                   PmConfa;
  UINT32                   Bar;

  RstCause = ResetUnknown;

  Bar     = MmioRead32 (MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, 0x10)) & ~0x0F;
  PmConfa = MmioRead32 (Bar + R_PMC_PWRM_GEN_PMCON_A);

  // Reset reason
  if ((PmConfa & B_PMC_PWRM_GEN_PMCON_A_GBL_RST_STS) != 0) {
    RstCause = ResetGlobal;
  } else if ((PmConfa & B_PMC_PWRM_GEN_PMCON_A_HOST_RST_STS) != 0 && (PmConfa & B_PMC_PWRM_GEN_PMCON_A_MEM_SR) != 0) {
    RstCause = ResetWarm;
  } else if ((PmConfa & B_PMC_PWRM_GEN_PMCON_A_HOST_RST_STS) != 0) {
    RstCause = ResetCold;
  } else if ((PmConfa & B_PMC_PWRM_GEN_PMCON_A_PWR_FLR) != 0) {
    RstCause = ResetPowerOn;
  }

  if (GetBootMode() == BOOT_ON_S3_RESUME) {
    RstCause = ResetWakeS3;
  } else if (GetBootMode() == BOOT_ON_S4_RESUME) {
    RstCause = ResetWakeS4;
  }

  if (IsRebootByWdt ()) {
    RstCause |= ResetTcoWdt;
  }

  SetResetReason (RstCause);

}


/**
 Update memory map related info using SOC registers.

 **/
VOID
EFIAPI
UpdateMemoryInfo (
  VOID
)
{
  UINT32  Tolum;
  UINT64  Touum;
  UINT64  Tom;

  // Update system memory info using SOC specific registers

  Tom  = PciRead32 (PCI_LIB_ADDRESS(SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_TOM));
  Tom += LShiftU64 (PciRead32 (PCI_LIB_ADDRESS(SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_TOM + 4)), 32);
  Tom &= B_SA_TOM_MASK;
  SetMemoryInfo (EnumMemInfoTom,  Tom);

  Tolum  = PciRead32 (PCI_LIB_ADDRESS(SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_TOLUD));
  Tolum &= B_SA_TOLUD_MASK;
  SetMemoryInfo (EnumMemInfoTolum,  Tolum);

  Touum  = PciRead32 (PCI_LIB_ADDRESS(SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_TOUUD));
  Touum += LShiftU64 (PciRead32 (PCI_LIB_ADDRESS(SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_TOUUD + 4)), 32);
  Touum &= B_SA_TOUUD_MASK;
  if (Touum < SIZE_4GB) {
    Touum = SIZE_4GB;
  }
  SetMemoryInfo (EnumMemInfoTouum,  Touum);
}
