/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Register/SaRegsHostBridge.h>

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