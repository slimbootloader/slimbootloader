/** @file

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <FspmUpd.h>
#include <PlatformBoardId.h>
#include <Include/PlatformData.h>
#include <PlatformBase.h>
#include <ConfigDataDefs.h>
#include <Include/TccConfigSubRegions.h>
#include <Library/PchInfoLib.h>
#include <Library/WatchDogTimerLib.h>
#include <Include/CpuRegs.h>
#include <Register/Cpuid.h>
#include <Library/ConfigDataLib.h>
#include <ConfigDataCommonStruct.h>
#include <Library/ContainerLib.h>
#include <Library/BaseMemoryLib.h>
#include <GpioPinsVer2Lp.h>
#include <Library/TccLib.h>

#include "BoardSaConfigPreMem.h"

typedef enum {
  DciDbcDisabled       = 0x0,
  DciDbcUsb2           = 0x1,
  DciDbcUsb3           = 0x2,
  DciDbcBoth           = 0x3,
  DciDbcNoChange       = 0x4,
  DciDbcMax
} DCI_DBC_MODE;

/**
Return Cpu stepping type

@retval UINT8                   Cpu stepping type
**/
CPU_STEPPING
EFIAPI
GetCpuStepping(
  VOID
  )
{
  CPUID_VERSION_INFO_EAX  Eax;
  ///
  /// Read the CPUID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Eax.Uint32, NULL, NULL, NULL);
  return ((CPU_STEPPING) (Eax.Uint32 & CPUID_FULL_STEPPING));
}

/**
  Update FSP-M UPD config data for TCC mode and tuning

  @param  FspmUpd            The pointer to the FSP-M UPD to be updated.

  @retval EFI_NOT_FOUND      TCC Features Data not found or disabled.
  @retval EFI_LOAD_ERROR     Tcc Buffer sub-region not found
  @retval EFI_SUCCESS        Successfully loaded buffer sub-region
**/
EFI_STATUS
TccModePreMemConfig (
  FSPM_UPD  *FspmUpd
)
{
  UINT32                                *TccCacheBase;
  UINT32                                 TccCacheSize;
  UINT32                                *TccStreamBase;
  UINT32                                 TccStreamSize;
  TCC_CFG_DATA                          *TccCfgData;
  BIOS_SETTINGS                         *PolicyConfig;
  TCC_STREAM_CONFIGURATION              *StreamConfig;
  FEATURES_CFG_DATA                     *FeaturesCfgData;
  EFI_STATUS                             Status;

  Status           = EFI_SUCCESS;

  TccCfgData = (TCC_CFG_DATA *) FindConfigDataByTag(CDATA_TCC_TAG);
  if ((TccCfgData == NULL) || (TccCfgData->TccEnable == 0)) {
    return EFI_NOT_FOUND;
  }

    if (GetBootMode() == BOOT_ON_FLASH_UPDATE) {
      DEBUG ((DEBUG_INIT, "In FW update flow. Donot apply DSO settings\n"));
      TccCfgData->TccTuning = 0;
    }

  // TCC related memory settings
  DEBUG ((DEBUG_INFO, "Tcc is enabled, setting Tcc memory config.\n"));
  FspmUpd->FspmConfig.DisPgCloseIdleTimeout      = 1;    // controls Page Close Idle Timeout
  // ADL doesnt support WRC due to HW bug. So disabling this code
  //FspmUpd->FspmConfig.WrcFeatureEnable         = 1;
  FspmUpd->FspmConfig.GtClosEnable               = 1;    // ***This causes hang in FSP and may end up frying up the board if this line is enabled in non-IOTG SKU ***
  FspmUpd->FspmConfig.PowerDownMode              = 0;    // controls command bus tristating during idle periods
  FspmUpd->FspmConfig.HyperThreading         = 0;
  FspmUpd->FspmConfig.DisableStarv2medPrioOnNewReq = 1;

  FspmUpd->FspmConfig.SoftwareSramEnPreMem   = TccCfgData->TccSoftSram;
  FspmUpd->FspmConfig.DsoTuningEnPreMem      = TccCfgData->TccTuning;
  FspmUpd->FspmConfig.TccErrorLogEnPreMem    = TccCfgData->TccErrorLog;

  // S0ix is disabled if TCC is enabled.
  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
  if (FeaturesCfgData != NULL) {
      if (FeaturesCfgData->Features.S0ix == 1) {
      FeaturesCfgData->Features.S0ix = 0;
      DEBUG ((DEBUG_INFO, "S0ix is turned off when TCC is enabled\n"));
    }
  }

  if (IsMarkedBadDso ()) {
    DEBUG ((DEBUG_ERROR, "Incorrect TCC tuning parameters. Platform rebooted with default values.\n"));
    FspmUpd->FspmConfig.TccStreamCfgStatusPreMem = 1;
  } else if (IsWdtFlagsSet(WDT_FLAG_TCC_DSO_IN_PROGRESS) && IsWdtTimeout()) {
    DEBUG ((DEBUG_ERROR, "Incorrect TCC tuning parameters. Platform rebooted with default values.\n"));
    WdtClearScratchpad (WDT_FLAG_TCC_DSO_IN_PROGRESS);

    // Let FSP know that an error has occured in DSO loading
    FspmUpd->FspmConfig.TccStreamCfgStatusPreMem = 1;
    InvalidateBadDso ();
  } else if (TccCfgData->TccTuning != 0) {
    // Setup Watch dog timer
    WdtReloadAndStart (WDT_TIMEOUT_TCC_DSO, WDT_FLAG_TCC_DSO_IN_PROGRESS);

    // Load TCC stream config from container
    TccStreamBase = NULL;
    TccStreamSize = 0;

    Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'T'),
                            (VOID **)&TccStreamBase, &TccStreamSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "TCC Streams not found! %r\n", Status));
    } else {
      if ((TccStreamSize >= sizeof (TCC_STREAM_CONFIGURATION)) && (TccStreamBase != NULL)) {
        DEBUG ((DEBUG_INFO, "Load TCC Stream @0x%p, size = 0x%x\n", TccStreamBase, TccStreamSize));
        DEBUG ((DEBUG_INFO, "PreMem TCC Dso 0x%x\n", FspmUpd->FspmConfig.DsoTuningEnPreMem));
        DEBUG ((DEBUG_INFO, "Dump Tcc Stream with MIN(0x40, TccStreamSize):\n"));
        DumpHex (2, 0, MIN(0x40, TccStreamSize), TccStreamBase);

        FspmUpd->FspmConfig.TccStreamCfgBasePreMem = (UINT32) (UINTN)TccStreamBase;
        FspmUpd->FspmConfig.TccStreamCfgSizePreMem = TccStreamSize;

        // Override Tcc settings from Streams
        StreamConfig   = (TCC_STREAM_CONFIGURATION *) TccStreamBase;
        PolicyConfig   = (BIOS_SETTINGS *) &StreamConfig->BiosSettings;

        if (StreamConfig->Version == TCC_STREAM_CONFIGURATION_VERSION) {
          FspmUpd->FspmConfig.HyperThreading        = PolicyConfig->HyperThreading;
          FspmUpd->FspmConfig.SaGv                  = PolicyConfig->SaGv;
          FspmUpd->FspmConfig.DisPgCloseIdleTimeout = PolicyConfig->DisPgCloseIdle;
          FspmUpd->FspmConfig.PowerDownMode         = PolicyConfig->MemPowerDown;

          DEBUG ((DEBUG_INFO, "Dump TCC DSO BIOS settings:\n"));
          DumpHex (2, 0, sizeof(BIOS_SETTINGS), PolicyConfig);
        }
      }
    }
  }

  // Load Tcc Cache config from container
  TccCacheBase = NULL;
  TccCacheSize = 0;
  Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'C'),
                                (VOID **)&TccCacheBase, &TccCacheSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "TCC Cache config not found! %r\n", Status));
  } else {
    FspmUpd->FspmConfig.TccCacheCfgBasePreMem = (UINT32)(UINTN)TccCacheBase;
    FspmUpd->FspmConfig.TccCacheCfgSizePreMem = TccCacheSize;
    DEBUG ((DEBUG_INFO, "Load TCC cache @0x%p, size = 0x%x\n", TccCacheBase, TccCacheSize));
    DEBUG ((DEBUG_INFO, "Dump Tcc Cache with MIN(0x40, TccCacheSize):\n"));
    DumpHex (2, 0, MIN(0x40, TccCacheSize), TccCacheBase);
  }

  return Status;
}

/**
  Update FSP-M UPD config data.

  @param  FspmUpdPtr            The pointer to the FSP-M UPD to be updated.

**/
VOID
EFIAPI
UpdateFspConfig (
  IN  VOID  *FspmUpdPtr
  )
{
  FSPM_UPD        *FspmUpd;
  FSP_M_CONFIG    *Fspmcfg;
  MEMORY_CFG_DATA             *MemCfgData;
  GRAPHICS_CFG_DATA           *GfxCfgData;
  PLATFORM_DATA               *PlatformData;
  FEATURES_CFG_DATA           *FeaturesCfgData;
  SILICON_CFG_DATA            *SiCfgData;
  UINT16           PlatformId;
  UINT8            SaDisplayConfigTable[16] = { 0 };
  UINT8            Index;
  UINT8            DebugPort;
  UINT32           SpdData[9];

  FspmUpd = (FSPM_UPD *)FspmUpdPtr;
  Fspmcfg = &FspmUpd->FspmConfig;

  DEBUG ((DEBUG_INFO, "FSPM CfgData assignment\n"));
  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
  if (MemCfgData == NULL) {
    CpuHalt ("Failed to find memory CFGDATA!");
    return;
  }
  CopyMem (&Fspmcfg->SpdAddressTable, MemCfgData->SpdAddressTable, sizeof(MemCfgData->SpdAddressTable));

  DebugPort = GetDebugPort ();
  if (DebugPort < GetPchMaxSerialIoUartControllersNum ()) {
    Fspmcfg->PcdDebugInterfaceFlags = BIT4;
    Fspmcfg->SerialIoUartDebugControllerNumber = DebugPort;
    Fspmcfg->SerialIoUartDebugMode = 4;
  } else {
    Fspmcfg->PcdDebugInterfaceFlags = BIT1;
    if (DebugPort == 0xFF) {
      Fspmcfg->PcdIsaSerialUartBase = 0;
    } else {
      Fspmcfg->PcdIsaSerialUartBase = 1;
    }
  }
  Fspmcfg->PcdSerialDebugLevel          = 0x5;

  // Memory SPD Data
  SpdData[0] = 0;
  SpdData[1] = (UINT32)(UINTN) (((MEM_SPD0_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD0_TAG))->MemorySpdPtr0);
  SpdData[2] = (UINT32)(UINTN) (((MEM_SPD1_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD1_TAG))->MemorySpdPtr1);
  SpdData[3] = (UINT32)(UINTN) (((MEM_SPD2_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD2_TAG))->MemorySpdPtr2);
  SpdData[4] = (UINT32)(UINTN) (((MEM_SPD3_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD3_TAG))->MemorySpdPtr3);
  SpdData[5] = (UINT32)(UINTN) (((MEM_SPD4_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD4_TAG))->MemorySpdPtr4);
  SpdData[6] = (UINT32)(UINTN) (((MEM_SPD5_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD5_TAG))->MemorySpdPtr5);
  SpdData[7] = (UINT32)(UINTN) (((MEM_SPD6_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD6_TAG))->MemorySpdPtr6);
  SpdData[8] = (UINT32)(UINTN) (((MEM_SPD7_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD7_TAG))->MemorySpdPtr7);
  Fspmcfg->MemorySpdPtr000  = SpdData[MemCfgData->SpdDataSel000];
  Fspmcfg->MemorySpdPtr001  = SpdData[MemCfgData->SpdDataSel001];
  Fspmcfg->MemorySpdPtr010  = SpdData[MemCfgData->SpdDataSel010];
  Fspmcfg->MemorySpdPtr011  = SpdData[MemCfgData->SpdDataSel011];
  Fspmcfg->MemorySpdPtr020  = SpdData[MemCfgData->SpdDataSel020];
  Fspmcfg->MemorySpdPtr021  = SpdData[MemCfgData->SpdDataSel021];
  Fspmcfg->MemorySpdPtr030  = SpdData[MemCfgData->SpdDataSel030];
  Fspmcfg->MemorySpdPtr031  = SpdData[MemCfgData->SpdDataSel031];
  Fspmcfg->MemorySpdPtr100  = SpdData[MemCfgData->SpdDataSel100];
  Fspmcfg->MemorySpdPtr101  = SpdData[MemCfgData->SpdDataSel101];
  Fspmcfg->MemorySpdPtr110  = SpdData[MemCfgData->SpdDataSel110];
  Fspmcfg->MemorySpdPtr111  = SpdData[MemCfgData->SpdDataSel111];
  Fspmcfg->MemorySpdPtr120  = SpdData[MemCfgData->SpdDataSel120];
  Fspmcfg->MemorySpdPtr121  = SpdData[MemCfgData->SpdDataSel121];
  Fspmcfg->MemorySpdPtr130  = SpdData[MemCfgData->SpdDataSel130];
  Fspmcfg->MemorySpdPtr131  = SpdData[MemCfgData->SpdDataSel131];

  //Dq/Dqs Mapping arrays
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc0Ch0, MemCfgData->DqsMapCpu2DramMc0Ch0, sizeof(MemCfgData->DqsMapCpu2DramMc0Ch0));
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc0Ch1, MemCfgData->DqsMapCpu2DramMc0Ch1, sizeof(MemCfgData->DqsMapCpu2DramMc0Ch1));
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc0Ch2, MemCfgData->DqsMapCpu2DramMc0Ch2, sizeof(MemCfgData->DqsMapCpu2DramMc0Ch2));
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc0Ch3, MemCfgData->DqsMapCpu2DramMc0Ch3, sizeof(MemCfgData->DqsMapCpu2DramMc0Ch3));
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc1Ch0, MemCfgData->DqsMapCpu2DramMc1Ch0, sizeof(MemCfgData->DqsMapCpu2DramMc1Ch0));
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc1Ch1, MemCfgData->DqsMapCpu2DramMc1Ch1, sizeof(MemCfgData->DqsMapCpu2DramMc1Ch1));
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc1Ch2, MemCfgData->DqsMapCpu2DramMc1Ch2, sizeof(MemCfgData->DqsMapCpu2DramMc1Ch2));
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc1Ch3, MemCfgData->DqsMapCpu2DramMc1Ch3, sizeof(MemCfgData->DqsMapCpu2DramMc1Ch3));

  CopyMem (&Fspmcfg->DqMapCpu2DramMc0Ch0, MemCfgData->DqMapCpu2DramMc0Ch0, sizeof(MemCfgData->DqMapCpu2DramMc0Ch0));
  CopyMem (&Fspmcfg->DqMapCpu2DramMc0Ch1, MemCfgData->DqMapCpu2DramMc0Ch1, sizeof(MemCfgData->DqMapCpu2DramMc0Ch1));
  CopyMem (&Fspmcfg->DqMapCpu2DramMc0Ch2, MemCfgData->DqMapCpu2DramMc0Ch2, sizeof(MemCfgData->DqMapCpu2DramMc0Ch2));
  CopyMem (&Fspmcfg->DqMapCpu2DramMc0Ch3, MemCfgData->DqMapCpu2DramMc0Ch3, sizeof(MemCfgData->DqMapCpu2DramMc0Ch3));
  CopyMem (&Fspmcfg->DqMapCpu2DramMc1Ch0, MemCfgData->DqMapCpu2DramMc1Ch0, sizeof(MemCfgData->DqMapCpu2DramMc1Ch0));
  CopyMem (&Fspmcfg->DqMapCpu2DramMc1Ch1, MemCfgData->DqMapCpu2DramMc1Ch1, sizeof(MemCfgData->DqMapCpu2DramMc1Ch1));
  CopyMem (&Fspmcfg->DqMapCpu2DramMc1Ch2, MemCfgData->DqMapCpu2DramMc1Ch2, sizeof(MemCfgData->DqMapCpu2DramMc1Ch2));
  CopyMem (&Fspmcfg->DqMapCpu2DramMc1Ch3, MemCfgData->DqMapCpu2DramMc1Ch3, sizeof(MemCfgData->DqMapCpu2DramMc1Ch3));

  Fspmcfg->DqPinsInterleaved      = MemCfgData->DqPinsInterleaved;

  //RComp
  Fspmcfg->RcompResistor = MemCfgData->RcompResistor;
  CopyMem (&Fspmcfg->RcompTarget, MemCfgData->RcompTarget, sizeof(MemCfgData->RcompTarget));

  // Mem cfg
  Fspmcfg->RmtPerTask           = MemCfgData->RmtPerTask;
  Fspmcfg->MrcSafeConfig        = MemCfgData->MrcSafeConfig;
  Fspmcfg->ScramblerSupport     = MemCfgData->ScramblerSupport;
  Fspmcfg->EnhancedInterleave   = MemCfgData->EnhancedInterleave;
  Fspmcfg->RankInterleave       = MemCfgData->RankInterleave;
  Fspmcfg->RemapEnable          = MemCfgData->RemapEnable;
  Fspmcfg->ExitOnFailure        = MemCfgData->ExitOnFailure;
  Fspmcfg->ChHashEnable         = MemCfgData->ChHashEnable;
  Fspmcfg->ChHashInterleaveBit  = MemCfgData->ChHashInterleaveBit;
  Fspmcfg->ChHashMask           = MemCfgData->ChHashMask;
  Fspmcfg->MrcFastBoot          = MemCfgData->MrcFastBoot;
  // Maximum amt of memory needed
  Fspmcfg->PlatformMemorySize   = MemCfgData->PlatformMemorySize;
  Fspmcfg->SafeMode = 0x0;

  // Cpu Cfg
  Fspmcfg->CpuRatio             = MemCfgData->CpuRatio;
  Fspmcfg->TmeEnable            = MemCfgData->TmeEnable;
  Fspmcfg->BootFrequency        = MemCfgData->BootFrequency;
  Fspmcfg->ActiveCoreCount      = MemCfgData->ActiveCoreCount;
  Fspmcfg->ActiveSmallCoreCount = MemCfgData->ActiveSmallCoreCount;
  Fspmcfg->FClkFrequency        = MemCfgData->FClkFrequency;
  Fspmcfg->TxtDprMemoryBase     = MemCfgData->TxtDprMemoryBase;
  Fspmcfg->TxtDprMemorySize     = MemCfgData->TxtDprMemorySize;
  Fspmcfg->SinitMemorySize      = MemCfgData->SinitMemorySize;
  Fspmcfg->TxtHeapMemorySize    = MemCfgData->TxtHeapMemorySize;
  Fspmcfg->BiosSize             = MemCfgData->BiosSize;
  Fspmcfg->BiosAcmBase          = MemCfgData->BiosAcmBase;
  Fspmcfg->BiosGuard            = 0x0; // Need to disable, else it will fails in FSPS
  Fspmcfg->BiosGuardToolsInterface = 1;

  // Skip CPU replacement check for embedded design to always enable fast boot
  Fspmcfg->SkipCpuReplacementCheck = 1;

  //MRC Training algorithms
  Fspmcfg->RMT                  = MemCfgData->RMT;
  Fspmcfg->BdatEnable           = MemCfgData->BdatEnable;
  Fspmcfg->BdatTestType         = MemCfgData->BdatTestType;
  Fspmcfg->RMC                  = MemCfgData->RMC;
  Fspmcfg->MEMTST               = MemCfgData->MEMTST;
  Fspmcfg->ECT                  = MemCfgData->ECT;

  //.Pcie
  Fspmcfg->PcieRpEnableMask = MemCfgData->PcieRpEnableMask;
  CopyMem (Fspmcfg->PcieClkSrcUsage,       MemCfgData->PcieClkSrcUsage,       sizeof(MemCfgData->PcieClkSrcUsage));
  CopyMem (Fspmcfg->PcieClkSrcClkReq,      MemCfgData->PcieClkSrcClkReq,      sizeof(MemCfgData->PcieClkSrcClkReq));

  // Cpu Pcie
  Fspmcfg->CpuPcieRpEnableMask           = MemCfgData->CpuPcieRpEnableMask;
  CopyMem (Fspmcfg->CpuPcieRpPcieSpeed, MemCfgData->CpuPcieRpPcieSpeed, sizeof(MemCfgData->CpuPcieRpPcieSpeed));
  Fspmcfg->CpuPcieRpClockReqMsgEnable[0] = MemCfgData->CpuPcieRpClockReqMsgEnable[0];
  Fspmcfg->CpuPcieRpClockReqMsgEnable[1] = MemCfgData->CpuPcieRpClockReqMsgEnable[1];
  Fspmcfg->CpuPcieRpClockReqMsgEnable[2] = MemCfgData->CpuPcieRpClockReqMsgEnable[2];

  // Hd Audio
  Fspmcfg->PchHdaEnable               = MemCfgData->PchHdaEnable;
  Fspmcfg->PchHdaDspEnable            = MemCfgData->PchHdaDspEnable;
  Fspmcfg->PchHdaAudioLinkHdaEnable   = MemCfgData->PchHdaAudioLinkHdaEnable;
  CopyMem (Fspmcfg->PchHdaAudioLinkDmicEnable, MemCfgData->PchHdaAudioLinkDmicEnable, sizeof(MemCfgData->PchHdaAudioLinkDmicEnable));
  Fspmcfg->PchHdaAudioLinkDmicClockSelect[0] = MemCfgData->PchHdaAudioLinkDmicClockSelect[0];
  Fspmcfg->PchHdaAudioLinkDmicClockSelect[1] = MemCfgData->PchHdaAudioLinkDmicClockSelect[1];
  CopyMem (Fspmcfg->PchHdaAudioLinkSndwEnable, MemCfgData->PchHdaAudioLinkSndwEnable, sizeof(MemCfgData->PchHdaAudioLinkSndwEnable));
  Fspmcfg->PchHdaIDispLinkTmode              = MemCfgData->PchHdaIDispLinkTmode;
  Fspmcfg->PchHdaIDispLinkFrequency   = MemCfgData->PchHdaIDispLinkFrequency;
  Fspmcfg->PchHdaIDispCodecDisconnect = MemCfgData->PchHdaIDispCodecDisconnect;

  if (IsAdlPch () && IsPchLp ()) {
    Fspmcfg->PchHdaAudioLinkDmicClkAPinMux[0] = GPIO_VER2_LP_MUXING_DMIC0_CLKA_GPP_S2;
    Fspmcfg->PchHdaAudioLinkDmicClkBPinMux[0] = GPIO_VER2_LP_MUXING_DMIC1_CLKA_GPP_S6;
    Fspmcfg->PchHdaAudioLinkDmicDataPinMux[0] = GPIO_VER2_LP_MUXING_DMIC0_DATA_GPP_S3;
    Fspmcfg->PchHdaAudioLinkDmicClkAPinMux[1] = GPIO_VER2_LP_MUXING_DMIC1_CLKA_GPP_S6;
    Fspmcfg->PchHdaAudioLinkDmicClkBPinMux[1] = GPIO_VER2_LP_MUXING_DMIC1_CLKB_GPP_S5;
    Fspmcfg->PchHdaAudioLinkDmicDataPinMux[1] = GPIO_VER2_LP_MUXING_DMIC1_DATA_GPP_S7;
  }

  Fspmcfg->SkipMbpHob = 0;
  DEBUG ((DEBUG_INFO, "SkipMbpHob = 0x%x\n", Fspmcfg->SkipMbpHob));

  // Gfx
  GfxCfgData = (GRAPHICS_CFG_DATA *)FindConfigDataByTag (CDATA_GRAPHICS_TAG);
  if (GfxCfgData != NULL) {
    Fspmcfg->IgdDvmt50PreAlloc         = GfxCfgData->IgdDvmt50PreAlloc;
    Fspmcfg->ApertureSize              = GfxCfgData->ApertureSize;
    Fspmcfg->GttSize                   = GfxCfgData->GttSize;
    Fspmcfg->InternalGfx               = GfxCfgData->InternalGfx;
    Fspmcfg->PrimaryDisplay            = GfxCfgData->PrimaryDisplay;
  } else {
    DEBUG ((DEBUG_INFO, "Failed to find GFX CFG!\n"));
  }

  // IBECC - removed in the latest FSP. Add them later when needed
  Fspmcfg->Ibecc                    = MemCfgData->Ibecc;
  Fspmcfg->IbeccOperationMode       = MemCfgData->IbeccOperationMode;
  //Fspmcfg->IbeccParity              = MemCfgData->IbeccParity;
  //Fspmcfg->IbeccErrorInj            = MemCfgData->IbeccErrorInj;
  for (Index = 0; Index < 8; Index++) {
    Fspmcfg->IbeccProtectedRangeEnable[Index] = MemCfgData->IbeccProtectedRegionEnable[Index];
    Fspmcfg->IbeccProtectedRangeBase[Index]   = MemCfgData->IbeccProtectedRegionBase[Index];
    if (MemCfgData->IbeccProtectedRegionMask[Index] > 0x03FFFFFF) {
      MemCfgData->IbeccProtectedRegionMask[Index] = 0x03FFFFFF;
    }
    Fspmcfg->IbeccProtectedRangeMask[Index]   = MemCfgData->IbeccProtectedRegionMask[Index];
  }

  // Vtd
  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if (PlatformData != NULL) {
    PlatformData->PlatformFeatures.VtdEnable = (!MemCfgData->VtdDisable) & FeaturePcdGet (PcdVtdEnabled);
    if (PlatformData->PlatformFeatures.VtdEnable == 1) {
      Fspmcfg->VtdDisable   = (UINT8)(!PlatformData->PlatformFeatures.VtdEnable);
      Fspmcfg->VtdIgdEnable = MemCfgData->VtdIgdEnable;
      Fspmcfg->VtdIpuEnable = 0x1;
      Fspmcfg->VtdIopEnable = 0x1;

      Fspmcfg->VtdBaseAddress[0] = VTD_BASEADDRESS_0;
      Fspmcfg->VtdBaseAddress[1] = VTD_BASEADDRESS_1;
      Fspmcfg->VtdBaseAddress[2] = VTD_BASEADDRESS_2;

      Fspmcfg->X2ApicOptOut        = MemCfgData->X2ApicOptOut;
      Fspmcfg->DmaBufferSize       = MemCfgData->DmaBufferSize;
      Fspmcfg->DmaControlGuarantee = MemCfgData->DmaControlGuarantee;
      Fspmcfg->PreBootDmaMask      = 0x1;
    }
  }

  // Host Bridge
  Fspmcfg->MmioSize             = MemCfgData->MmioSize;
  Fspmcfg->MmioSizeAdjustment   = MemCfgData->MmioSizeAdjustment;

  // Smbus
  Fspmcfg->SmbusEnable               = MemCfgData->SmbusEnable;
  Fspmcfg->PchNumRsvdSmbusAddresses  = MemCfgData->PchNumRsvdSmbusAddresses;
  Fspmcfg->RsvdSmbusAddressTablePtr  = MemCfgData->RsvdSmbusAddressTablePtr;

  // Overclocking Cfg
  Fspmcfg->RealtimeMemoryTiming = MemCfgData->RealtimeMemoryTiming;

  // Tcc related settings
  Fspmcfg->SaGv                       = MemCfgData->SaGv;                   // System Agent Geyserville - SAGV dynamically adjusts the system agent
                                                                            // voltage and clock frequencies depending on power and performance requirements
  Fspmcfg->DisPgCloseIdleTimeout      = MemCfgData->DisPgCloseIdleTimeout;  // controls Page Close Idle Timeout
  Fspmcfg->PowerDownMode              = MemCfgData->PowerDownMode;          // controls command bus tristating during idle periods
  Fspmcfg->WrcFeatureEnable           = MemCfgData->WrcFeatureEnable;
  Fspmcfg->HyperThreading             = MemCfgData->HyperThreading;
  Fspmcfg->GtClosEnable               = MemCfgData->GtClosEnable;
  Fspmcfg->VmxEnable                  = MemCfgData->VmxEnable;
  Fspmcfg->Lp5BankMode                = MemCfgData->Lp5BankMode;

  // CSI port
  CopyMem (Fspmcfg->IpuLaneUsed, MemCfgData->IpuLaneUsed, sizeof(MemCfgData->IpuLaneUsed));
  CopyMem (Fspmcfg->CsiSpeed, MemCfgData->CsiSpeed, sizeof(MemCfgData->CsiSpeed));

  // DMI cfg
  Fspmcfg->DmiMaxLinkSpeed = MemCfgData->DmiMaxLinkSpeed;
  Fspmcfg->DmiAspm         = MemCfgData->DmiAspm;
  Fspmcfg->DmiAspmCtrl     = MemCfgData->DmiAspmCtrl;
  Fspmcfg->DmiHweq         = 0x1;
  CopyMem (Fspmcfg->DmiGen3RootPortPreset, MemCfgData->DmiGen3RootPortPreset, sizeof(MemCfgData->DmiGen3RootPortPreset));
  CopyMem (Fspmcfg->DmiGen3EndPointPreset, MemCfgData->DmiGen3EndPointPreset, sizeof(MemCfgData->DmiGen3EndPointPreset));
  CopyMem (Fspmcfg->DmiGen3EndPointHint,   MemCfgData->DmiGen3EndPointHint,   sizeof(MemCfgData->DmiGen3EndPointHint));
  Fspmcfg->DmiGen3Ltcpo[7] = 0x2;
  for (Index = 0; Index < 8; Index++) {
    Fspmcfg->DmiGen3Ltcpre[Index]         = MemCfgData->DmiGen3Ltcpre[Index];
    Fspmcfg->DmiGen3DsPortRxPreset[Index] = MemCfgData->DmiGen3DsPortRxPreset[Index];
    Fspmcfg->DmiGen3DsPortTxPreset[Index] = MemCfgData->DmiGen3DsPortTxPreset[Index];
  }
  for (Index = 0; Index < 8; Index++) {
    Fspmcfg->DmiGen4Ltcpo[Index] = 0x7;
  }
  CopyMem (Fspmcfg->CpuDmiHwEqGen3CoeffListCm, MemCfgData->CpuDmiHwEqGen3CoeffListCm, sizeof(MemCfgData->CpuDmiHwEqGen3CoeffListCm));
  CopyMem (Fspmcfg->CpuDmiHwEqGen3CoeffListCp, MemCfgData->CpuDmiHwEqGen3CoeffListCp, sizeof(MemCfgData->CpuDmiHwEqGen3CoeffListCp));
  CopyMem (Fspmcfg->CpuDmiHwEqGen4CoeffListCm, MemCfgData->CpuDmiHwEqGen4CoeffListCm, sizeof(MemCfgData->CpuDmiHwEqGen4CoeffListCm));
  CopyMem (Fspmcfg->CpuDmiHwEqGen4CoeffListCp, MemCfgData->CpuDmiHwEqGen4CoeffListCp, sizeof(MemCfgData->CpuDmiHwEqGen4CoeffListCp));
  CopyMem (Fspmcfg->PchDmiHwEqGen3CoeffListCm, MemCfgData->PchDmiHwEqGen3CoeffListCm, sizeof(MemCfgData->PchDmiHwEqGen3CoeffListCm));
  CopyMem (Fspmcfg->PchDmiHwEqGen3CoeffListCp, MemCfgData->PchDmiHwEqGen3CoeffListCp, sizeof(MemCfgData->PchDmiHwEqGen3CoeffListCp));
  Fspmcfg->Gen3EqPhase23Bypass = MemCfgData->Gen3EqPhase23Bypass;
  Fspmcfg->Gen3EqPhase3Bypass  = MemCfgData->Gen3EqPhase3Bypass;
  Fspmcfg->Gen3LtcoEnable      = MemCfgData->Gen3LtcoEnable;

  // Tcss Dev enable bits
  Fspmcfg->TcssItbtPcie0En = MemCfgData->TcssItbtPcie0En;
  Fspmcfg->TcssItbtPcie1En = MemCfgData->TcssItbtPcie1En;
  Fspmcfg->TcssItbtPcie2En = MemCfgData->TcssItbtPcie2En;
  Fspmcfg->TcssItbtPcie3En = MemCfgData->TcssItbtPcie3En;
  Fspmcfg->TcssXhciEn      = MemCfgData->TcssXhciEn;
  Fspmcfg->TcssDma0En      = MemCfgData->TcssDma0En;
  Fspmcfg->TcssDma1En      = MemCfgData->TcssDma1En;
  Fspmcfg->UsbTcPortEnPreMem = MemCfgData->UsbTcPortEnPreMem;

  // Misc
  Fspmcfg->UserBd               = MemCfgData->UserBd;
  Fspmcfg->EnableC6Dram         = MemCfgData->EnableC6Dram;
  Fspmcfg->TsegSize             = MemCfgData->TsegSize;
  Fspmcfg->SkipExtGfxScan       = MemCfgData->SkipExtGfxScan;
  Fspmcfg->LockPTMregs          = MemCfgData->LockPTMregs;
  Fspmcfg->CridEnable           = 0x0;
  Fspmcfg->Ddr4OneDpc           = 0x3;

  Fspmcfg->PlatformDebugConsent         = MemCfgData->PlatformDebugConsent;
  if (Fspmcfg->PlatformDebugConsent != 0) {
    Fspmcfg->DciModphyPg                  = 0;
    Fspmcfg->DciUsb3TypecUfpDbg           = 2;
    Fspmcfg->DebugInterfaceLockEnable     = TRUE;
    Fspmcfg->DciDbcMode                   = DciDbcNoChange; // 4
    Fspmcfg->DciEn                        = MemCfgData->DciEn;
    Fspmcfg->CpuTraceHubMode              = MemCfgData->CpuTraceHubMode; // 2
    Fspmcfg->PchTraceHubMode              = MemCfgData->PchTraceHubMode; // 2
    Fspmcfg->CpuTraceHubMemReg0Size       = MemCfgData->CpuTraceHubMemReg0Size;
    Fspmcfg->CpuTraceHubMemReg1Size       = MemCfgData->CpuTraceHubMemReg1Size;
    Fspmcfg->PchTraceHubMemReg0Size       = MemCfgData->PchTraceHubMemReg0Size;
    Fspmcfg->PchTraceHubMemReg1Size       = MemCfgData->PchTraceHubMemReg1Size;
  }

  PlatformId = GetPlatformId();
  //
  // Display DDI Initialization ( default Native GPIO as per board during AUTO case)
  //
  switch (PlatformId) {
  case PLATFORM_ID_ADL_S_ADP_S_CRB:
  case PLATFORM_ID_ADL_S_ADP_S_DDR5_UDIMM_1DC_CRB:
  case PLATFORM_ID_ADL_S_ADP_S_DDR4_SODIMM_CRB:
  case PLATFORM_ID_ADL_S_ADP_S_DDR5_SODIMM_CRB:
    DEBUG((DEBUG_INFO, "ADL_S_ADP_S_CRB board Id %x .....\n", PlatformId));
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mAdlUDdr4RowDisplayDdiConfig, sizeof(mAdlUDdr4RowDisplayDdiConfig));
    break;
  case PLATFORM_ID_ADL_PS_DDR5_RVP:
    DEBUG((DEBUG_INFO, "PLATFORM_ID_ADL_PS_DDR5_RVP board Id %x .....\n", PlatformId));
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mAdlPSEdpHdmiDisplayDdiConfig, sizeof(mAdlPSEdpHdmiDisplayDdiConfig));
    break;
  case PLATFORM_ID_TEST_S_DDR5_UDIMM_RVP:
  case PLATFORM_ID_TEST_S_DDR5_SODIMM_RVP:
    // DP + DP
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mTestSDdr5RowDisplayDdiConfig3, sizeof(mTestSDdr5RowDisplayDdiConfig3));
    break;
  case PLATFORM_ID_ADL_N_DDR5_CRB:
    // DP + DP
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mAdlNddr5CrbRowDisplayDdiConfig, sizeof(mAdlNddr5CrbRowDisplayDdiConfig));
    break;
  case PLATFORM_ID_ADL_N_LPDDR5_RVP:
    // DP + DP
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mAdlNLpddr5RowDisplayDdiConfig, sizeof(mAdlNLpddr5RowDisplayDdiConfig));
    break;
  default:
    DEBUG((DEBUG_INFO, "DDI Init: Unsupported board Id %x .....\n", PlatformId));
    break;
  }
  Fspmcfg->DdiPortAConfig = SaDisplayConfigTable[0];
  Fspmcfg->DdiPortBConfig = SaDisplayConfigTable[1];
  Fspmcfg->DdiPortAHpd = SaDisplayConfigTable[2];
  Fspmcfg->DdiPortBHpd = SaDisplayConfigTable[3];
  Fspmcfg->DdiPortCHpd = SaDisplayConfigTable[4];
  Fspmcfg->DdiPort1Hpd = SaDisplayConfigTable[5];
  Fspmcfg->DdiPort2Hpd = SaDisplayConfigTable[6];
  Fspmcfg->DdiPort3Hpd = SaDisplayConfigTable[7];
  Fspmcfg->DdiPort4Hpd = SaDisplayConfigTable[8];
  Fspmcfg->DdiPortADdc = SaDisplayConfigTable[9];
  Fspmcfg->DdiPortBDdc = SaDisplayConfigTable[10];
  Fspmcfg->DdiPortCDdc = SaDisplayConfigTable[11];
  Fspmcfg->DdiPort1Ddc = SaDisplayConfigTable[12];
  Fspmcfg->DdiPort2Ddc = SaDisplayConfigTable[13];
  Fspmcfg->DdiPort3Ddc = SaDisplayConfigTable[14];
  Fspmcfg->DdiPort4Ddc = SaDisplayConfigTable[15];

  if (GetBootMode() == BOOT_ON_FLASH_UPDATE) {
    //
    // WA - Microcode issue is causing assert in FSP during
    // firmware update mode. Avoiding this UPD set until
    // microcode issue is resolved.
    //
#if 0
    Fspmcfg->SiSkipOverrideBootModeWhenFwUpdate = TRUE;
#endif
  }
#ifndef PLATFORM_ADLN
    Fspmcfg->WRDS = 0x1;
#endif
  if (IsPchLp ()) {
    Fspmcfg->DdiPortAConfig = 0x1;
    Fspmcfg->WdtDisableAndLock = 0x1;
    Fspmcfg->FirstDimmBitMask = 0x0;
    switch (GetPlatformId ()) {
      case PLATFORM_ID_ADL_P_LP4_RVP:
        Fspmcfg->DdiPortBHpd = 0x1;
        Fspmcfg->DdiPortBDdc = 0x1;
        break;
      case PLATFORM_ID_ADL_P_LP5_RVP:
        Fspmcfg->DdiPortBConfig = 0x1;
        Fspmcfg->PrmrrSize = 0x200000;
        Fspmcfg->PcieClkReqGpioMux[9] = 0x796e9000;
        Fspmcfg->TcssXdciEn = 0x1;
        Fspmcfg->Ddr4OneDpc = 0x3;
        Fspmcfg->Lp5CccConfig = 0xff;
        break;
      case PLATFORM_ID_ADL_P_DDR5_RVP:
        Fspmcfg->DdiPortBHpd = 0x1;
        Fspmcfg->PrmrrSize = 0x200000;
        Fspmcfg->PcieClkReqGpioMux[9] = 0x796e9000;
        Fspmcfg->TcssXdciEn = 0x1;
        Fspmcfg->Ddr4OneDpc = 0x3;
        break;
      case PLATFORM_ID_ADL_PS_DDR5_RVP:
        Fspmcfg->DdiPortBHpd = 0x1;
        Fspmcfg->DmiHweq = 0x2;
        Fspmcfg->PcieClkReqGpioMux[9] = 0x796e9000;
        break;
      case PLATFORM_ID_ADL_N_DDR5_CRB:
        Fspmcfg->CpuPcieRpEnableMask = 0x0;
        Fspmcfg->DmiHweq = 0x2;
        Fspmcfg->Lp5CccConfig = 0xff;
        Fspmcfg->SkipCpuReplacementCheck = 0x0;
        Fspmcfg->FirstDimmBitMaskEcc = 0x0;
        Fspmcfg->Lp5BankMode = 0x0;
        break;
      case PLATFORM_ID_ADL_N_LPDDR5_RVP:
        Fspmcfg->DmiHweq = 0x2;
        Fspmcfg->Lp5CccConfig = 0xff;
        Fspmcfg->FirstDimmBitMask = 0x0;
        Fspmcfg->SkipCpuReplacementCheck = 0x0;
        break;
      default:
        DEBUG ((DEBUG_INFO, "Unknown board for FSP-M UPD settings.\n"));
        break;
    }
  }
  if (PcdGetBool (PcdFastBootEnabled) && !(IsPchP ())) {
    Fspmcfg->CpuPcieRpEnableMask = 0;
  }

  Fspmcfg->Lfsr0Mask      = 0xb;
  Fspmcfg->Lfsr1Mask      = 0xb;
  Fspmcfg->RefreshPanicWm = 0x8;
  Fspmcfg->RefreshHpWm    = 0x7;
  // Tcc enabling
  if (IsPchS () && FeaturePcdGet (PcdTccEnabled)) {
    TccModePreMemConfig (FspmUpd);
  }
  // S0ix is disabled if TSN is enabled.
  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
  SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if ((SiCfgData != NULL) && (SiCfgData->PchTsnEnable == 1)) {
    if (FeaturesCfgData != NULL) {
      FeaturesCfgData->Features.S0ix = 0;
      DEBUG ((DEBUG_INFO, "TSN is enabled, turn off S0ix\n"));
    }
  }

  if (FeaturesCfgData != NULL) {
    if (FeaturesCfgData->Features.S0ix == 1) {
      Fspmcfg->PchIshEnable       = 0;
      Fspmcfg->TcssXdciEn         = 0;
      Fspmcfg->TcssDma0En         = 0;
      Fspmcfg->TcssDma1En         = 0;
      Fspmcfg->TcssItbtPcie0En    = 0;
      Fspmcfg->TcssItbtPcie1En    = 0;
      Fspmcfg->TcssItbtPcie2En    = 0;
      Fspmcfg->TcssItbtPcie3En    = 0;
      Fspmcfg->PchHdaDspEnable    = 0;
      Fspmcfg->PchHdaAudioLinkHdaEnable  = 0;
      ZeroMem (Fspmcfg->PchHdaAudioLinkDmicEnable, sizeof (Fspmcfg->PchHdaAudioLinkDmicEnable));
      DEBUG ((DEBUG_INFO, "Stage 1B S0ix config applied.\n"));
    }
  }
}

