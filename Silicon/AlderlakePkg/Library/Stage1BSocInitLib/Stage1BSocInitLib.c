/** @file

  Copyright (c) 2019 - 2025, Intel Corporation. All rights reserved.<BR>
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
#include <Library/TcoTimerLib.h>

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

  RstCause = ResetCold;

  if (GetBootMode() == BOOT_ON_S3_RESUME) {
    RstCause = ResetWakeS3;
    goto done;
  } else if (GetBootMode() == BOOT_ON_S4_RESUME) {
    RstCause = ResetWakeS4;
    goto done;
  }

  Bar = MmioRead32 (MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, 0x10)) & ~0x0F;
  if (Bar == 0) {
    RstCause |= ResetUnknown;
    goto done;
  }

  PmConfa = MmioRead32 (Bar + R_PMC_PWRM_GEN_PMCON_A);

  // Reset reason
  if ((PmConfa & B_PMC_PWRM_GEN_PMCON_A_GBL_RST_STS) != 0) {
    RstCause = ResetGlobal;
  } else if (PmConfa & B_PMC_PWRM_GEN_PMCON_A_HOST_RST_STS) {
    if (PmConfa & B_PMC_PWRM_GEN_PMCON_A_MEM_SR) {
      RstCause = ResetWarm;
    } else {
      RstCause = ResetCold;
    }
  }

  if ((PmConfa & B_PMC_PWRM_GEN_PMCON_A_PWR_FLR) != 0) {
    RstCause |= ResetUnknown;
  }

  if (WasBootCausedByTcoTimeout ()) {
    RstCause |= ResetTcoWdt;
  }

done:
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
