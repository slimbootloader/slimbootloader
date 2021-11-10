/** @file
  The platform hook library.

  Copyright (c) 2013 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/DebugLib.h>
#include <Library/BoardInitLib.h>
#include <MemInfoHob.h>
#include <RegAccess.h>

#define  TCO_STS_2ND_TIMEOUT      BIT1

#define  SMBUS_PORT_ID            0xC6
#define  GENERAL_CONTROL_REG      0x0C
#define  TCO_STS_NO_REBOOT        BIT2

/**
  Enables the execution by writing to the MSR.
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
  SbRegBase  = PciRead32 (PCI_LIB_ADDRESS(0, 31, 1, 0x10)) & 0xFF000000;
  GeneralControlReg = MmioRead32  (SbRegBase + ((SMBUS_PORT_ID) << 16) + GENERAL_CONTROL_REG);

  // Get Tco Status2 from from SmBus config space
  TcoBase     = PciRead32 (PCI_LIB_ADDRESS(0, 31, 4, 0x50)) & 0x0000FFE0;
  TcoSts2Reg  = IoRead16 (TcoBase + 0x6);

  DEBUG ((DEBUG_INFO, "TcoSts2Reg= 0x%x, GeneralControlReg = 0x%x\n", TcoSts2Reg, GeneralControlReg));
  if (((GeneralControlReg & TCO_STS_NO_REBOOT) == 0) && !(TcoSts2Reg & TCO_STS_2ND_TIMEOUT)) {
    RebootByWdt = TRUE;
  }

  return RebootByWdt;
}

/**
  Check if this boot is caused by warm reset.

  @RETVAL   TRUE     It is caused by warm reset.
  @RETVAL   FALSE    It is not caused by warm reset.
**/
BOOLEAN
IsWarmReset (
  VOID
  )
{
  VOID                  *FspHobList;
  MEMORY_PLATFORM_DATA  *MemPlatformData;
  UINT8                 ResetType;
  BOOLEAN               IsWarmReset;

  IsWarmReset     = FALSE;
  MemPlatformData = NULL;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    MemPlatformData = GetGuidHobData (
                        FspHobList,
                        NULL,
                        &gSiMemoryPlatformDataGuid
                        );
  }
  if (MemPlatformData != NULL) {
    if (MemPlatformData->BootMode == 1) {
      IsWarmReset = TRUE;
    }
  } else {
    //
    // Read reset type from reset control register 0xCF9, writing 0x04/0x06
    // to 0xCF9 also cause warm reset.
    //
    ResetType = IoRead8 (0xCF9);
    if ((ResetType == 0x4) || (ResetType == 0x6)) {
      IsWarmReset = TRUE;
    }
  }

  return IsWarmReset;
}

/**
  Update reset reason.
**/
VOID
EFIAPI
UpdateResetReason (
  VOID
)
{
  UINT8    ResetReason;

  ResetReason  = 0;
  if (IsRebootByWdt ()) {
    ResetReason |= ResetTcoWdt;
  }
  if (IsWarmReset ()) {
    ResetReason |= ResetWarm;
  }
  SetResetReason (ResetReason);
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
