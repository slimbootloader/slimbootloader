/** @file
  TccPlatformLib implementation for EHL platform

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <FspmUpd.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <FspsUpd.h>
#include <Library/TccPlatformLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/TimerLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/TccLib.h>
#include <Register/SaRegs.h>
#include <Register/Cpuid.h>
#include <Library/PchInfoLib.h>


#define GT_MAILBOX_WAITIME                10
#define GT_MAILBOX_TIMEOUT                100      ///< ~100 us
#define GT_COS_TIMEOUT                    3000     ///< ~3 ms

/**
  This procedure will retrieve Pre-Mem TCC BIOS settings table and configure FSP-M settings
**/
EFI_STATUS
EFIAPI
TccSetFspmConfig (
  IN FSPM_UPD          *FspmUpd,
  IN BIOS_SETTINGS     *PreMemBiosSettings,
  IN TCC_BINARIES_BASE *PreMemTccBinariesBase
  )
{

  if (PreMemBiosSettings == NULL) {
    // Set default FSP-M values for TCC mode
    DEBUG ((DEBUG_INFO, "Setting default FSP-M TCC tuning parameters\n"));
    FspmUpd->FspmConfig.SaGv                  = 0;
    FspmUpd->FspmConfig.DisPgCloseIdleTimeout = 1;
    FspmUpd->FspmConfig.RaplLim1Ena           = 0;
    FspmUpd->FspmConfig.RaplLim2Ena           = 0;
    FspmUpd->FspmConfig.PowerDownMode         = 0;
  } else {
    //Set the FSP UPDs based on tuning data
    DEBUG ((DEBUG_INFO, "Setting FSP-M TCC tuning parameters based on user config\n"));
    FspmUpd->FspmConfig.TccTuningEnablePreMem = 1;
    FspmUpd->FspmConfig.SaGv        = PreMemBiosSettings->SaGv;
    FspmUpd->FspmConfig.RaplLim1Ena = PreMemBiosSettings->MemoryRapl;
    FspmUpd->FspmConfig.RaplLim2Ena = PreMemBiosSettings->MemoryRapl;
    if (PreMemBiosSettings->MemPm == 0) {
      FspmUpd->FspmConfig.PowerDownMode         = 0;
      FspmUpd->FspmConfig.DisPgCloseIdleTimeout = 1;
    } else {
      FspmUpd->FspmConfig.PowerDownMode         = 1;
      FspmUpd->FspmConfig.DisPgCloseIdleTimeout = 0;
    }
    // Assign the Tcc binaries to FSP-M UPDs
    FspmUpd->FspmConfig.TccStreamCfgBasePreMem = (UINT32)(UINTN)PreMemTccBinariesBase->TuningBase;
    FspmUpd->FspmConfig.TccStreamCfgSizePreMem = PreMemTccBinariesBase->TuningSize;
    FspmUpd->FspmConfig.TccBufferCfgBase       = (UINT32)(UINTN)PreMemTccBinariesBase->BufferBase;
    FspmUpd->FspmConfig.TccBufferCfgSize       = PreMemTccBinariesBase->BufferSize;
  }
  return EFI_SUCCESS;
}

/**
  This procedure will retrieve Post-Mem TCC BIOS settings table and configure FSP-S settings
**/
EFI_STATUS
EFIAPI
TccSetFspsConfig (
  IN BIOS_SETTINGS*     BiosSettings,
  IN TCC_BINARIES_BASE* TccBinariesBase
  )
{
  FSPS_UPD  *FspsUpd;
  UINT8     MaxPchPcieRootPorts;
  UINT16    Index;

  MaxPchPcieRootPorts = GetPchMaxPciePortNum ();
  if (BiosSettings == NULL) {
    // Set default FSP-S values for TCC mode
    DEBUG ((DEBUG_INFO, "Setting default FSP-S TCC tuning parameters\n"));
    FspsUpd->FspsConfig.Eist = 0; // Intel Speed Step->EnableGv
    FspsUpd->FspsConfig.Cx   = 0;
    FspsUpd->FspsConfig.Hwp  = 0;
    FspsUpd->FspsConfig.AcSplitLock = 0;
    FspsUpd->FspsConfig.PsfTccEnable = 1; // Enable will decrease psf transaction latency by disabling some psf power management features
    FspsUpd->FspsConfig.PchDmiAspmCtrl = 0; // ASPM configuration on the PCH side of the DMI/OPI Link
    FspsUpd->FspsConfig.PchLegacyIoLowLatency = 1;
    FspsUpd->FspsConfig.PchTsnGbeMultiVcEnable = 1;
    FspsUpd->FspsConfig.PseTsnGbeMultiVcEnable[0] = 1;
    FspsUpd->FspsConfig.PseTsnGbeMultiVcEnable[1] = 1;
    FspsUpd->FspsConfig.PchPmPwrBtnOverridePeriod = 0;
    for (Index = 0; Index < MaxPchPcieRootPorts; Index++) {
      FspsUpd->FspsConfig.PcieRpAspm[Index] = 0;
      FspsUpd->FspsConfig.PcieRpL1Substates[Index] = 0;
      FspsUpd->FspsConfig.PciePtm[Index] = 1;
      if (IsRpMultiVC (Index) == 1) {
        FspsUpd->FspsConfig.PcieRpMultiVcEnabled[Index] = 1;
        FspsUpd->FspsConfig.PcieRpVc1TcMap [Index] = 0x7F;
      }
    }
    FspsUpd->FspsConfig.RenderStandby = 0;
    FspsUpd->FspsConfig.GtClosEnable = 1;
  } else {
    //Set the FSP UPDs based on tuning data
    DEBUG ((DEBUG_INFO, "Setting FSP-S TCC tuning parameters based on user config\n"));
    FspsUpd->FspsConfig.TccTuningEnable  = 1;
    FspsUpd->FspsConfig.Eist                  = BiosSettings->Pstates;
    FspsUpd->FspsConfig.Hwp                   = BiosSettings->HwpEn;
    FspsUpd->FspsConfig.Cx                    = BiosSettings->Cstates;
    FspsUpd->FspsConfig.PsfTccEnable          = BiosSettings->FabricPm;
    FspsUpd->FspsConfig.PchDmiAspmCtrl        = BiosSettings->DmiAspm;
    FspsUpd->FspsConfig.PchLegacyIoLowLatency = BiosSettings->PchPwrClkGate;
    FspsUpd->FspsConfig.PchPmPwrBtnOverridePeriod = (BiosSettings->Sstates==1? 0x3:0);
    for (Index = 0; Index < MaxPchPcieRootPorts; Index++) {
      FspsUpd->FspsConfig.PcieRpAspm[Index]        = BiosSettings->PcieAspm;
      FspsUpd->FspsConfig.PcieRpL1Substates[Index] = BiosSettings->PcieRpL1;
    }
    FspsUpd->FspsConfig.RenderStandby    = BiosSettings->GtRstRc6;
  }
  // Assign the Tcc binaries to FSP-S UPDs
  FspsUpd->FspsConfig.TccBiosCfgBase   = (UINT32)(UINTN)TccBinariesBase->BufferBase;
  FspsUpd->FspsConfig.TccBiosCfgSize   = TccBinariesBase->BufferSize;
  FspsUpd->FspsConfig.TccStreamCfgBase = (UINT32)(UINTN)TccBinariesBase->TuningBase;
  FspsUpd->FspsConfig.TccStreamCfgSize = TccBinariesBase->TuningSize;
  FspsUpd->FspsConfig.TccCacheCfgBase  = (UINT32)(UINTN)TccBinariesBase->CacheconfigBase;
  FspsUpd->FspsConfig.TccCacheCfgSize  = TccBinariesBase->CacheconfigSize;
  FspsUpd->FspsConfig.TccPtcmBinBase   = (UINT32)(UINTN)TccBinariesBase->PtcmBase;
  FspsUpd->FspsConfig.TccPtcmBinSize   = TccBinariesBase->PtcmSize;

  return EFI_SUCCESS;
}

/*
  Get Max IA L2 Cache COS

  @retval UINT32    Max IA L2 Cache COS
*/
UINT32
EFIAPI
GetMaxIaL2Cos (
  VOID
  )
{
  CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF_EDX L2CacheRdtAlloc;

  AsmCpuidEx (
    CPUID_INTEL_RDT_ALLOCATION,
    CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF,
    NULL,
    NULL,
    NULL,
    &L2CacheRdtAlloc.Uint32
    );

  // Add 1 for max cox
  return L2CacheRdtAlloc.Bits.HighestCosNumber + 1;
}

/**
  Poll Run busy clear

  @param[in] Base    - Base address of MMIO
  @param[in] Timeout - Timeout value in microsecond

  @retval TRUE       - Run Busy bit is clear
  @retval FALSE      - Run Busy bit is still set
**/
BOOLEAN
PollRunBusyClear (
  IN    UINT64           Base,
  IN    UINT32           Timeout
  )
{
  UINT32  Value;
  BOOLEAN Status = FALSE;

  //
  // Make timeout an exact multiple of 10 to avoid infinite loop
  //
  if ((Timeout) % 10 != 0) {
    Timeout = (Timeout) + 10 - ((Timeout) % 10);
  }

  while (Timeout != 0) {
    Value = MmioRead32 ((UINTN) Base + 0x138124);
    if (Value & BIT31) {
      //
      // Wait for 10us and try again.
      //
      DEBUG ((DEBUG_INFO, "Interface register run busy bit is still set. Trying again \n"));
      MicroSecondDelay (10);
      Timeout = Timeout - 10;
    } else {
      Status = TRUE;
      break;
    }
  }
  // ASSERT ((Timeout != 0));

  return Status;
}

/**
  Get GT COS of specific policy

  @param[in] CosPolicy              GT COS Policy to get ways
  @param[out] CosWaysMask           Cos Ways Mask

  @retval EFI_SUCCESS               Got GT COS successful
  @retval EFI_INVALID_PARAMETER     Invalid COS policy
  @retval EFI_UNSUPPORTED           Unsupported MSR
**/
EFI_STATUS
EFIAPI
GetGtCos (
  IN UINT8     GtCosIndex,
  OUT UINT32   *CosWaysMask
  )
{
  UINT64                GttMmAdr;
  UINT64                GttMmAdrHigh;
  UINT16                WaitTime;
  EFI_STATUS            Status;

  Status = EFI_SUCCESS;
  WaitTime = GT_COS_TIMEOUT;

  GttMmAdr = PciRead32 (PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, R_SA_IGD_GTTMMADR));
  if ((GttMmAdr & BIT2) == BIT2) {
    //64-bit BAR
    GttMmAdrHigh = (UINT64) PciRead32 (PCI_LIB_ADDRESS (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, R_SA_IGD_GTTMMADR + 0x4));
    GttMmAdr = (((GttMmAdrHigh << 32) | GttMmAdr) & ~((UINT64) 0xF));
  } else {
    GttMmAdr &= ~((UINT64) 0xF);
  }

  if (GtCosIndex > MAX_GT_L3_WAY_MASKS) {
    return EFI_INVALID_PARAMETER;
  }

  while (WaitTime != 0) { //3ms loop
    MmioWrite32 ((UINTN) GttMmAdr + R_SA_GTTMMADR_MAILBOX_DATA_LOW_OFFSET, (UINT32) GtCosIndex); // mailbox_low       = GTCOS Index
    MmioWrite32 ((UINTN) GttMmAdr + R_SA_GTTMMADR_MAILBOX_DATA_HIGH_OFFSET, 0x00000000);    // mailbox_high      = 0x00000000
    MmioWrite32 ((UINTN) GttMmAdr + R_SA_GTTMMADR_MAILBOX_INTERFACE_OFFSET, 0x8000001D);    // mailbox Interface = 0x80000007
    PollRunBusyClear ((UINTN) GttMmAdr, GT_MAILBOX_TIMEOUT);                                // Poll Run Busy cleared
    //
    // Check for MailBox Data read status successful
    //
    *CosWaysMask =  MmioRead32 ((UINTN) GttMmAdr + R_SA_GTTMMADR_MAILBOX_DATA_LOW_OFFSET);
    // DEBUG ((DEBUG_INFO, "TccDxe Gt L3 Mask %d, %x\n", Index, MmioRead32(GttMmAdr + R_SA_GTTMMADR_MAILBOX_DATA_LOW_OFFSET)));
    if (MmioRead32 ((UINTN)GttMmAdr + R_SA_GTTMMADR_MAILBOX_DATA_LOW_OFFSET)) {
      DEBUG ((DEBUG_INFO, "Mailbox Data low read Successful \n"));
      DEBUG ((DEBUG_INFO, "Mailbox Data low  %x\n", MmioRead32((UINTN) GttMmAdr + R_SA_GTTMMADR_MAILBOX_DATA_LOW_OFFSET)));
      DEBUG ((DEBUG_INFO, "Mailbox Data high %x\n", MmioRead32((UINTN) GttMmAdr + R_SA_GTTMMADR_MAILBOX_DATA_HIGH_OFFSET)));
      break;
    }
    MicroSecondDelay (GT_MAILBOX_WAITIME);
    WaitTime = WaitTime - GT_MAILBOX_WAITIME;
  }
  if (WaitTime == 0) {
    Status = EFI_NOT_FOUND;
    DEBUG ((DEBUG_INFO, "Failed to read GT Mailbox\n"));
  }

  return Status;
}

/**
  Get WRC COS of specific policy

  @param[out] CosWaysMask  Cos Ways Mask

  @retval EFI_SUCCESS           get WRC COS successful
  @retval EFI_INVALID_PARAMETER invalid COS policy
  @retval EFI_UNSUPPORTED       Unsupported MSR
**/
EFI_STATUS
EFIAPI
MsrGetWrcCos (
  OUT UINT32   *CosWaysMask
  )
{
  //EHL does not support WrcCos
  return EFI_UNSUPPORTED;
}
