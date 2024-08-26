/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
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
#include <Library/BoardInitLib.h>

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
  Update FSP-M UPD config data.

  @param  FspmUpdPtr            The pointer to the FSP-M UPD to be updated.

**/
VOID
EFIAPI
UpdateFspConfig (
  IN  VOID  *FspmUpdPtr
  )
{
  FSPM_UPD                    *FspmUpd;
  FSP_M_CONFIG                *Fspmcfg;
  MEMORY_CFG_DATA             *MemCfgData;
  GRAPHICS_CFG_DATA           *GfxCfgData;
  PLATFORM_DATA               *PlatformData;
  FEATURES_CFG_DATA           *FeaturesCfgData;
  SILICON_CFG_DATA            *SiCfgData;
  POWER_CFG_DATA              *PowerCfgData;
  UINT16                      BoardId;
  UINT8                       SaDisplayConfigTable[16] = { 0 };
  UINT8                       Index;
  UINT8                       DebugPort;
  UINT32                      SpdData[7];
  UINT32                      CarBase;
  UINT32                      CarSize;
  EFI_STATUS                  Status;
  FSPM_UPD_COMMON             *FspmUpdCommon;

  FspmUpd = (FSPM_UPD *)FspmUpdPtr;
  Fspmcfg = &FspmUpd->FspmConfig;

  FspmUpdCommon = (FSPM_UPD_COMMON *)FspmUpd;
  FspmUpdCommon->FspmArchUpd.NvsBufferPtr         = (UINT32)(UINTN)FindNvsData();

  Status = GetTempRamInfo (&CarBase, &CarSize);
  ASSERT_EFI_ERROR (Status);
  FspmUpd->FspmArchUpd.StackBase = CarBase \
                                 + FixedPcdGet32 (PcdStage1StackBaseOffset) \
                                 + FixedPcdGet32 (PcdStage1StackSize) \
                                 + FixedPcdGet32 (PcdStage1DataSize);
  FspmUpd->FspmArchUpd.StackSize = CarBase + CarSize - FspmUpd->FspmArchUpd.StackBase;
  DEBUG ((DEBUG_INFO, "CAR Base 0x%X (0x%X)\n", CarBase, CarSize));
  DEBUG ((DEBUG_INFO, "FSPM Stack Base=0x%X, Size=0x%X\n", FspmUpd->FspmArchUpd.StackBase, FspmUpd->FspmArchUpd.StackSize));

  DEBUG ((DEBUG_INFO, "FSPM CfgData assignment\n"));
  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
  if (MemCfgData == NULL) {
    CpuHalt ("Failed to find memory CFGDATA!");
    return;
  }
  CopyMem (&Fspmcfg->SpdAddressTable, MemCfgData->SpdAddressTable, sizeof(MemCfgData->SpdAddressTable));

  DebugPort = GetDebugPort ();
  if (DebugPort < GetPchMaxSerialIoUartControllersNum ()) {
    Fspmcfg->SerialIoUartDebugControllerNumber = DebugPort;
    Fspmcfg->SerialIoUartDebugMode = 4;
  } else {
    if (DebugPort == 0xFF) {
      Fspmcfg->PcdIsaSerialUartBase = 0;
    } else {
      Fspmcfg->PcdIsaSerialUartBase = 1;
    }
  }

  Fspmcfg->PcdDebugInterfaceFlags       = BIT1|BIT4;

  // Memory SPD Data
  //Update if embedded memory needed
  ZeroMem (SpdData, sizeof(SpdData));
  SpdData[1] = (UINT32)(UINTN) (((MEM_SPD0_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD0_TAG))->MemorySpdPtr0);
  SpdData[2] = (UINT32)(UINTN) (((MEM_SPD1_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD1_TAG))->MemorySpdPtr1);
  SpdData[3] = (UINT32)(UINTN) (((MEM_SPD2_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD2_TAG))->MemorySpdPtr2);
  SpdData[4] = (UINT32)(UINTN) (((MEM_SPD3_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD3_TAG))->MemorySpdPtr3);
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
  Fspmcfg->RmtPerTask               = MemCfgData->RmtPerTask;
  Fspmcfg->ScramblerSupport         = MemCfgData->ScramblerSupport;
  Fspmcfg->EnhancedInterleave       = MemCfgData->EnhancedInterleave;
  Fspmcfg->RankInterleave           = MemCfgData->RankInterleave;
  Fspmcfg->RemapEnable              = MemCfgData->RemapEnable;
  Fspmcfg->ExitOnFailure            = MemCfgData->ExitOnFailure;
  Fspmcfg->ChHashEnable             = MemCfgData->ChHashEnable;
  Fspmcfg->ChHashInterleaveBit      = MemCfgData->ChHashInterleaveBit;
  Fspmcfg->ChHashMask               = MemCfgData->ChHashMask;
  Fspmcfg->MrcFastBoot              = MemCfgData->MrcFastBoot;
  Fspmcfg->Write0                   = TRUE;
  Fspmcfg->BoardStackUp             = 0;
  Fspmcfg->MrcSafeMode              = 0;
  Fspmcfg->McSafeMode               = 0;
  Fspmcfg->DdrSafeMode              = 0;
  // Maximum amt of memory needed
  Fspmcfg->PlatformMemorySize       = MemCfgData->PlatformMemorySize;

  // Cpu Cfg
  Fspmcfg->CpuRatio                 = MemCfgData->CpuRatio;
  Fspmcfg->TmeEnable                = MemCfgData->TmeEnable;
  Fspmcfg->ActiveCoreCount          = MemCfgData->ActiveCoreCount;
  Fspmcfg->ActiveSmallCoreCount     = MemCfgData->ActiveSmallCoreCount;
  Fspmcfg->FClkFrequency            = MemCfgData->FClkFrequency;
  Fspmcfg->TxtDprMemoryBase         = MemCfgData->TxtDprMemoryBase;
  Fspmcfg->TxtDprMemorySize         = MemCfgData->TxtDprMemorySize;
  Fspmcfg->SinitMemorySize          = MemCfgData->SinitMemorySize;
  Fspmcfg->TxtHeapMemorySize        = MemCfgData->TxtHeapMemorySize;
  Fspmcfg->BiosSize                 = MemCfgData->BiosSize;
  Fspmcfg->BiosAcmBase              = MemCfgData->BiosAcmBase;
  Fspmcfg->BiosGuard                = 0x0; // Need to disable, else it will fails in FSPS
  Fspmcfg->BiosGuardToolsInterface  = 1;

  //MRC Training algorithms
  Fspmcfg->RMT                      = MemCfgData->RMT;
  Fspmcfg->BdatEnable               = MemCfgData->BdatEnable;
  Fspmcfg->BdatTestType             = MemCfgData->BdatTestType;
  Fspmcfg->RMC                      = MemCfgData->RMC;
  Fspmcfg->ECT                      = MemCfgData->ECT;
  Fspmcfg->SaGv                     = MemCfgData->SaGv;  //0x00 // This only affects ULX/ULT; otherwise SA GV is disabled.
  Fspmcfg->SaGvWpMask               = 0xF;

  //.Pcie
  Fspmcfg->PcieRpEnableMask     = MemCfgData->PcieRpEnableMask;
  CopyMem (Fspmcfg->PcieClkSrcUsage,       MemCfgData->PcieClkSrcUsage,       sizeof(MemCfgData->PcieClkSrcUsage));
  CopyMem (Fspmcfg->PcieClkSrcClkReq,      MemCfgData->PcieClkSrcClkReq,      sizeof(MemCfgData->PcieClkSrcClkReq));

  // Hd Audio
  Fspmcfg->PchHdaEnable                      = MemCfgData->PchHdaEnable;
  Fspmcfg->PchHdaDspEnable                   = MemCfgData->PchHdaDspEnable;
  Fspmcfg->PchHdaAudioLinkHdaEnable          = MemCfgData->PchHdaAudioLinkHdaEnable;
  Fspmcfg->PchHdaIDispLinkTmode              = MemCfgData->PchHdaIDispLinkTmode;
  Fspmcfg->PchHdaIDispLinkFrequency          = MemCfgData->PchHdaIDispLinkFrequency;
  Fspmcfg->PchHdaIDispCodecDisconnect        = MemCfgData->PchHdaIDispCodecDisconnect;
  CopyMem (Fspmcfg->PchHdaAudioLinkDmicEnable, MemCfgData->PchHdaAudioLinkDmicEnable, sizeof(MemCfgData->PchHdaAudioLinkDmicEnable));
  CopyMem (Fspmcfg->PchHdaAudioLinkSndwEnable, MemCfgData->PchHdaAudioLinkSndwEnable, sizeof(MemCfgData->PchHdaAudioLinkSndwEnable));
  CopyMem (Fspmcfg->PchHdaAudioLinkDmicClockSelect, MemCfgData->PchHdaAudioLinkDmicClockSelect, sizeof(MemCfgData->PchHdaAudioLinkDmicClockSelect));

  Fspmcfg->SkipMbpHob = 0;

  // Gfx
  GfxCfgData = (GRAPHICS_CFG_DATA *)FindConfigDataByTag (CDATA_GRAPHICS_TAG);
  if (GfxCfgData != NULL) {
    Fspmcfg->IgdDvmt50PreAlloc         = GfxCfgData->IgdDvmt50PreAlloc;
    Fspmcfg->InternalGfx               = GfxCfgData->InternalGfx;
    Fspmcfg->PrimaryDisplay            = GfxCfgData->PrimaryDisplay;
  } else {
    DEBUG ((DEBUG_INFO, "Failed to find GFX CFG!\n"));
  }

  // IBECC - removed in the latest FSP. Add them later when needed
  Fspmcfg->Ibecc                    = MemCfgData->Ibecc;
  Fspmcfg->IbeccOperationMode       = MemCfgData->IbeccOperationMode;

  // Vtd
  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if (PlatformData != NULL) {
    PlatformData->PlatformFeatures.VtdEnable = (!MemCfgData->VtdDisable) & FeaturePcdGet (PcdVtdEnabled);
    if (PlatformData->PlatformFeatures.VtdEnable == 1) {
      Fspmcfg->VtdDisable          = 0x0;
      Fspmcfg->VtdIgdEnable        = MemCfgData->VtdIgdEnable;
      Fspmcfg->VtdIopEnable        = 0x1;
      Fspmcfg->VtdBaseAddress[0]   = VTD_BASEADDRESS_0;
      Fspmcfg->VtdBaseAddress[1]   = VTD_BASEADDRESS_1;
      Fspmcfg->X2ApicOptOut        = MemCfgData->X2ApicOptOut;
      Fspmcfg->DmaBufferSize       = MemCfgData->DmaBufferSize;
      Fspmcfg->DmaControlGuarantee = MemCfgData->DmaControlGuarantee;
      Fspmcfg->PreBootDmaMask      = 0x1;
    } else {
      Fspmcfg->VtdDisable          = 0x1;
    }
  }

  // Host Bridge
  Fspmcfg->MmioSize                   = MemCfgData->MmioSize;
  Fspmcfg->MmioSizeAdjustment         = MemCfgData->MmioSizeAdjustment;

  // Smbus
  Fspmcfg->SmbusEnable                = MemCfgData->SmbusEnable;
  Fspmcfg->PchNumRsvdSmbusAddresses   = MemCfgData->PchNumRsvdSmbusAddresses;
  Fspmcfg->RsvdSmbusAddressTablePtr   = MemCfgData->RsvdSmbusAddressTablePtr;

  // Overclocking Cfg
  Fspmcfg->RealtimeMemoryTiming       = MemCfgData->RealtimeMemoryTiming;

  // Tcc related settings
  Fspmcfg->DisPgCloseIdleTimeout      = MemCfgData->DisPgCloseIdleTimeout;  // controls Page Close Idle Timeout
  Fspmcfg->PowerDownMode              = MemCfgData->PowerDownMode;          // controls command bus tristating during idle periods
  Fspmcfg->HyperThreading             = MemCfgData->HyperThreading;
  Fspmcfg->VmxEnable                  = MemCfgData->VmxEnable;

  // DMI cfg
  Fspmcfg->DmiMaxLinkSpeed = MemCfgData->DmiMaxLinkSpeed;
  Fspmcfg->DmiAspm         = MemCfgData->DmiAspm;
  Fspmcfg->DmiHweq         = 0x1;
  CopyMem (Fspmcfg->DmiGen3DsPortRxPreset, MemCfgData->DmiGen3DsPortRxPreset, sizeof(MemCfgData->DmiGen3DsPortRxPreset));
  for (Index = 0; Index < 8; Index++) {
    Fspmcfg->DmiGen3Ltcpre[Index]         = MemCfgData->DmiGen3Ltcpre[Index];
    Fspmcfg->DmiGen3DsPortTxPreset[Index] = MemCfgData->DmiGen3DsPortTxPreset[Index];
    Fspmcfg->DmiGen4Ltcpo[Index] = 0;
    Fspmcfg->PchDmiTranCoOverEn[Index] = 0;
    Fspmcfg->PchDmiTranCoOverPostCur[Index] = 0;
    Fspmcfg->PchDmiTranCoOverPreCur[Index] = 0;
    Fspmcfg->PchDmiUpPortTranPreset[Index] = 0;
    if (MemCfgData->IbeccProtectedRegionMask[Index] > 0x03FFFFFF) {
      MemCfgData->IbeccProtectedRegionMask[Index] = 0x03FFFFFF;
    }
  }
  Fspmcfg->Gen3EqPhase23Bypass    = MemCfgData->Gen3EqPhase23Bypass;
  Fspmcfg->Gen3EqPhase3Bypass     = MemCfgData->Gen3EqPhase3Bypass;
  Fspmcfg->Gen3LtcoEnable         = MemCfgData->Gen3LtcoEnable;


  // Tcss Dev enable bits
  Fspmcfg->TcssItbtPcie0En        = MemCfgData->TcssItbtPcie0En;
  Fspmcfg->TcssItbtPcie1En        = MemCfgData->TcssItbtPcie1En;
  Fspmcfg->TcssItbtPcie2En        = MemCfgData->TcssItbtPcie2En;
  Fspmcfg->TcssItbtPcie3En        = MemCfgData->TcssItbtPcie3En;
  Fspmcfg->TcssXhciEn             = MemCfgData->TcssXhciEn;
  Fspmcfg->TcssDma0En             = MemCfgData->TcssDma0En;
  Fspmcfg->TcssDma1En             = MemCfgData->TcssDma1En;
  Fspmcfg->UsbTcPortEnPreMem      = MemCfgData->UsbTcPortEnPreMem;

  // Misc
  Fspmcfg->UserBd                 = MemCfgData->UserBd;
  Fspmcfg->TsegSize               = MemCfgData->TsegSize;
  Fspmcfg->SkipExtGfxScan         = MemCfgData->SkipExtGfxScan;
  Fspmcfg->LockPTMregs            = MemCfgData->LockPTMregs;

  //Debug consent
  Fspmcfg->PlatformDebugOption            = MemCfgData->PlatformDebugOption;
  if (Fspmcfg->PlatformDebugOption != 0) {
    Fspmcfg->DciUsb3TypecUfpDbg           = 2;
    Fspmcfg->DebugInterfaceLockEnable     = TRUE;
    Fspmcfg->DciDbcMode                   = DciDbcNoChange; // 4
    Fspmcfg->DciEn                        = MemCfgData->DciEn;
    Fspmcfg->PchTraceHubMode              = MemCfgData->PchTraceHubMode; // 2
    Fspmcfg->PchTraceHubMemReg0Size       = MemCfgData->PchTraceHubMemReg0Size;
    Fspmcfg->PchTraceHubMemReg1Size       = MemCfgData->PchTraceHubMemReg1Size;
  }

  BoardId = GetPlatformId();
  DEBUG((DEBUG_INFO, "Display DDI init: board Id %x .....\n", BoardId));
  //
  // Display DDI Initialization ( default Native GPIO as per board during AUTO case)
  //
  switch (BoardId) {
   case PLATFORM_ID_ARL_S_UDIMM_1DPC_RVP:
    DEBUG((DEBUG_INFO, "BoardIdMtlSUDimm1DEvcrb board Id %x .....\n", BoardId));
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mMtlSUDimm1DEvcrbRowDisplayDdiConfig, sizeof(mMtlSUDimm1DEvcrbRowDisplayDdiConfig));
    break;
   case PLATFORM_ID_ARL_H_DDR5_CRB:
    DEBUG((DEBUG_INFO, "BoardIdArlHSODimm1DCRB board Id %x .....\n", BoardId));
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mArlhDdr5SODimmSbsCrbRowDisplayDdiConfig, sizeof(mArlhDdr5SODimmSbsCrbRowDisplayDdiConfig));
    break;
   case PLATFORM_ID_ARL_H_DDR5_RVP:
    DEBUG((DEBUG_INFO, "BoardIdArlHSODimm1DRvp board Id %x .....\n", BoardId));
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mArlhDdr5SODimmSbsRvpRowDisplayDdiConfig, sizeof(mArlhDdr5SODimmSbsRvpRowDisplayDdiConfig));
    break;
   case  PLATFORM_ID_ARL_S_SODIMM_RVP:
    DEBUG((DEBUG_INFO, "BoardIdArlHSODimm1DRvp board Id %x .....\n", BoardId));
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mArlsDdr5SODimmSbsRvpRowDisplayDdiConfig, sizeof(mArlsDdr5SODimmSbsRvpRowDisplayDdiConfig));
    break;
   case  PLATFORM_ID_ARL_H_LP5x_Rvp:
    DEBUG((DEBUG_INFO, "BoardIdArlHlp5xDRvp board Id %x .....\n", BoardId));
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mArlhLpddr5SODimmSbsRvpRowDisplayDdiConfig, sizeof(mArlhLpddr5SODimmSbsRvpRowDisplayDdiConfig));
    break;
  default:
    DEBUG((DEBUG_INFO, "DDI Init: Unsupported board Id %x .....\n", BoardId));
    break;
  }
  Fspmcfg->DdiPortAConfig   = SaDisplayConfigTable[0];
  Fspmcfg->DdiPortBConfig   = SaDisplayConfigTable[1];
  Fspmcfg->DdiPortAHpd      = SaDisplayConfigTable[2];
  Fspmcfg->DdiPortBHpd      = SaDisplayConfigTable[3];
  Fspmcfg->DdiPortCHpd      = SaDisplayConfigTable[4];
  Fspmcfg->DdiPort1Hpd      = SaDisplayConfigTable[5];
  Fspmcfg->DdiPort2Hpd      = SaDisplayConfigTable[6];
  Fspmcfg->DdiPort3Hpd      = SaDisplayConfigTable[7];
  Fspmcfg->DdiPort4Hpd      = SaDisplayConfigTable[8];
  Fspmcfg->DdiPortADdc      = SaDisplayConfigTable[9];
  Fspmcfg->DdiPortBDdc      = SaDisplayConfigTable[10];
  Fspmcfg->DdiPortCDdc      = SaDisplayConfigTable[11];
  Fspmcfg->DdiPort1Ddc      = SaDisplayConfigTable[12];
  Fspmcfg->DdiPort2Ddc      = SaDisplayConfigTable[13];
  Fspmcfg->DdiPort3Ddc      = SaDisplayConfigTable[14];
  Fspmcfg->DdiPort4Ddc      = SaDisplayConfigTable[15];

  Fspmcfg->Lfsr0Mask      = 0xb;
  Fspmcfg->Lfsr1Mask      = 0xb;

  // S0ix is disabled if TSN is enabled.
  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
  SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if ((SiCfgData != NULL) && (SiCfgData->PchTsnEnable == 1)) {
    if (FeaturesCfgData != NULL) {
      FeaturesCfgData->Features.S0ix = 0;
      DEBUG ((DEBUG_INFO, "TSN is enabled, turn off S0ix\n"));
    }
  }

  PowerCfgData = (POWER_CFG_DATA *) FindConfigDataByTag (CDATA_POWER_TAG);
  if (PowerCfgData != NULL) {
    DEBUG ((DEBUG_INFO, "PowerCfgData found!\n"));
    Fspmcfg->PsysPmax                                             = 0x0;
    Fspmcfg->TdcEnable[0]                                         = 0x1;
    Fspmcfg->TdcEnable[1]                                         = 0x1;
    Fspmcfg->TdcEnable[2]                                         = 0x1;
    Fspmcfg->TdcTimeWindow[0]                                     = 0x3e8;
    Fspmcfg->TdcTimeWindow[1]                                     = 0x3e8;
    Fspmcfg->TdcTimeWindow[2]                                     = 0x3e8;
  }

  Fspmcfg->TcssItbtPcie0En                                      = MemCfgData->TcssItbtPcie0En;
  Fspmcfg->TcssItbtPcie1En                                      = MemCfgData->TcssItbtPcie1En;
  Fspmcfg->TcssItbtPcie2En                                      = MemCfgData->TcssItbtPcie2En;
  Fspmcfg->TcssItbtPcie3En                                      = MemCfgData->TcssItbtPcie3En;
  Fspmcfg->TcssXhciEn                                           = MemCfgData->TcssXhciEn;
  Fspmcfg->TcssDma0En                                           = MemCfgData->TcssDma0En;
  Fspmcfg->TcssDma1En                                           = MemCfgData->TcssDma1En;
  Fspmcfg->UsbTcPortEnPreMem                                    = MemCfgData->UsbTcPortEnPreMem;
  Fspmcfg->I2cPostCodeEnable                                    = MemCfgData->I2cPostCodeEnable;

  CopyMem(Fspmcfg->DqMapCpu2DramMc0Ch0, MemCfgData->DqMapCpu2DramMc0Ch0, sizeof(MemCfgData->DqMapCpu2DramMc0Ch0));
  CopyMem(Fspmcfg->DqMapCpu2DramMc0Ch1, MemCfgData->DqMapCpu2DramMc0Ch1, sizeof(MemCfgData->DqMapCpu2DramMc0Ch1));
  CopyMem(Fspmcfg->DqMapCpu2DramMc0Ch2, MemCfgData->DqMapCpu2DramMc0Ch2, sizeof(MemCfgData->DqMapCpu2DramMc0Ch2));
  CopyMem(Fspmcfg->DqMapCpu2DramMc0Ch3, MemCfgData->DqMapCpu2DramMc0Ch3, sizeof(MemCfgData->DqMapCpu2DramMc0Ch3));
  CopyMem(Fspmcfg->DqMapCpu2DramMc1Ch0, MemCfgData->DqMapCpu2DramMc1Ch0, sizeof(MemCfgData->DqMapCpu2DramMc1Ch0));
  CopyMem(Fspmcfg->DqMapCpu2DramMc1Ch1, MemCfgData->DqMapCpu2DramMc1Ch1, sizeof(MemCfgData->DqMapCpu2DramMc1Ch1));
  CopyMem(Fspmcfg->DqMapCpu2DramMc1Ch2, MemCfgData->DqMapCpu2DramMc1Ch2, sizeof(MemCfgData->DqMapCpu2DramMc1Ch2));
  CopyMem(Fspmcfg->DqMapCpu2DramMc1Ch3, MemCfgData->DqMapCpu2DramMc1Ch3, sizeof(MemCfgData->DqMapCpu2DramMc1Ch3));

  Fspmcfg->ActiveSocNorthAtomCoreCount                          = 0xFF;
  Fspmcfg->VccInDemotionEnable[1]                               = 0x1;
  Fspmcfg->VccInDemotionEnable[2]                               = 0x1;
  Fspmcfg->TcssXdciEn                                           = 0x0;
  Fspmcfg->IgdDvmt50PreAlloc                                    = 0x2;
  Fspmcfg->SaGv                                                 = 0x1;
  Fspmcfg->PchHdaEnable                                         = 0x1;
  Fspmcfg->DmiMaxLinkSpeed                                      = 0x1;
  Fspmcfg->DmiAspm                                              = 0x4;
  Fspmcfg->CsVrefLow                                            = 0x45;
  Fspmcfg->CsVrefHigh                                           = 0x1d;
  Fspmcfg->CaVrefLow                                            = 0x45;
  Fspmcfg->CaVrefHigh                                           = 0x1d;
  Fspmcfg->MarginLimitCheck                                     = 0x1;
  Fspmcfg->HgSubSystemId                                        = 0x2212;
  Fspmcfg->PchDmiUpPortTranPresetEn                             = 0x0;
  Fspmcfg->PchDmiRtlepceb                                       = 0x0;
  Fspmcfg->SkipCpuReplacementCheck                              = 0x1;
  Fspmcfg->CridEnable                                           = 0x0;
  Fspmcfg->DmiGen3DsPortRxPreset[0] = 0x1;
  Fspmcfg->DmiGen3DsPortRxPreset[1] = 0x1;
  Fspmcfg->DmiGen3DsPortRxPreset[2] = 0x1;
  Fspmcfg->DmiGen3DsPortRxPreset[3] = 0x1;
  Fspmcfg->DmiGen3DsPortRxPreset[4] = 0x1;
  Fspmcfg->DmiGen3DsPortRxPreset[5] = 0x1;
  Fspmcfg->DmiGen3DsPortRxPreset[6] = 0x1;
  Fspmcfg->DmiGen3DsPortRxPreset[7] = 0x2;

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
  switch (BoardId) {
    case PLATFORM_ID_ARL_S_UDIMM_1DPC_RVP:
      Fspmcfg->SaIpuEnable = 0x0;
      Fspmcfg->DmiMaxLinkSpeed = 0x0;
      Fspmcfg->PchDmiGen3RootPortPreset[1] = 0x5;
      Fspmcfg->PchDmiGen3RootPortPreset[2] = 0x6;
      Fspmcfg->DmiHweq = 0x2;
      ZeroMem (Fspmcfg->PchDmiGen3Ltcpre, sizeof(Fspmcfg->PchDmiGen3Ltcpre));
      ZeroMem (Fspmcfg->PchDmiGen3Ltcpo, sizeof(Fspmcfg->PchDmiGen3Ltcpo));
      for (Index = 0; Index < 8; Index++) {
        Fspmcfg->DmiGen3DsPortRxPreset[Index] = 0x7;
        Fspmcfg->DmiGen4UsPortTxPreset[Index] = 0x7;
        Fspmcfg->PchDmiUpPortTranPreset[Index] = 0x7;
      }
      Fspmcfg->DmiAspmL1ExitLatency = 0x6;
      Fspmcfg->PchDmiAspmL1ExitLatency = 0x6;
      Fspmcfg->DmiL1ssEnable = 0x1;
      Fspmcfg->PrmrrSize = 0x200000;
      Fspmcfg->PchPcieClkSrcUsage[4] = 0x0;
      Fspmcfg->PchPcieClkSrcUsage[5] = 0x4;
      Fspmcfg->PchPcieClkSrcUsage[6] = 0x70;
      Fspmcfg->PchPcieClkSrcUsage[7] = 0x7;
      Fspmcfg->PchPcieClkSrcUsage[8] = 0x8;
      Fspmcfg->PchPcieClkSrcUsage[9] = 0xc;
      Fspmcfg->PchPcieClkSrcUsage[10] = 0x14;
      Fspmcfg->PcieClkReqGpioMux[2] = 0x146400;
      Fspmcfg->PcieClkReqGpioMux[3] = 0x146401;
      Fspmcfg->PchHdaSdiEnable[1] = 0x1;
      Fspmcfg->PchHdaAudioLinkSndwEnable[0] = 0x0;
      Fspmcfg->Write0 = 0x0;
      Fspmcfg->AllowOppRefBelowWriteThrehold = 0x1;
      Fspmcfg->PchDmiTranCoOverPostCur[0] = 0x5;
      Fspmcfg->PchDmiTranCoOverPostCur[1] = 0x7;
      Fspmcfg->PchDmiTranCoOverPreCur[0] = 0x4;
      Fspmcfg->PchDmiTranCoOverPreCur[1] = 0x6;
      Fspmcfg->PchDmiTranCoOverPreCur[2] = 0x8;
      Fspmcfg->PchDmiUpPortTranPresetEn = 0x1;
      Fspmcfg->PchDmiRtlepceb = 0x1;
      Fspmcfg->Psi2Threshold[2] = 0x28;
      Fspmcfg->DlvrRfiFrequency = 0x4b0;
      Fspmcfg->EnableFastVmode[0] = 0x0;
      Fspmcfg->EnableFastVmode[1] = 0x0;
      Fspmcfg->CepEnable[0] = 0x0;
      Fspmcfg->CepEnable[1] = 0x0;
      Fspmcfg->PchPcieClkSrcClkReq[2] = 0x2;
      Fspmcfg->PchPcieClkSrcClkReq[3] = 0x3;
      Fspmcfg->PchPcieClkSrcClkReq[4] = 0x4;
      Fspmcfg->PchPcieClkSrcClkReq[5] = 0x5;
      Fspmcfg->PchPcieClkSrcClkReq[6] = 0x6;
      Fspmcfg->PchPcieClkSrcClkReq[7] = 0x7;
      Fspmcfg->PchPcieClkSrcClkReq[8] = 0x8;
      Fspmcfg->PchPcieClkSrcClkReq[9] = 0x9;
      Fspmcfg->PchPcieClkSrcClkReq[10] = 0xa;
      Fspmcfg->PchPcieClkSrcClkReq[11] = 0xb;
      Fspmcfg->PchPcieClkSrcClkReq[12] = 0xc;
      Fspmcfg->PchPcieClkSrcClkReq[13] = 0xd;
      Fspmcfg->PchPcieClkSrcClkReq[14] = 0xe;
      Fspmcfg->PchPcieClkSrcClkReq[15] = 0xf;
      Fspmcfg->EnablePwrDn = 0x0;
      Fspmcfg->EnablePwrDnLpddr = 0x0;
      Fspmcfg->SrefCfgEna = 0x0;
      Fspmcfg->WdtDisableAndLock = 0x0; // Remove this later if any issues
      break;
  }
}

