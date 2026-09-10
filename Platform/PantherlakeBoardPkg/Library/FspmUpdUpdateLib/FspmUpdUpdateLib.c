/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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
#include <Library/PchInfoLib.h>
#include <Library/WatchDogTimerLib.h>
#include <Include/CpuRegs.h>
#include <Register/Cpuid.h>
#include <Library/ConfigDataLib.h>
#include <ConfigDataCommonStruct.h>
#include <Library/ContainerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FusaConfigLib.h>
#include <Library/BoardInitLib.h>
#include <Library/HobLib.h>
#include <CpuRegs.h>
#include <PlatformData.h>
#include <Register/IgdRegs.h>
#include "BoardSaConfigPreMem.h"
#include <Library/BootGuardLib.h>
#include <Library/TxtLib.h>

//
// TXT Memory Size Definitions
//
#define TXT_SINIT_MEMORY_SIZE    0x50000
#define TXT_HEAP_MEMORY_SIZE     0xF0000

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
  Return the SPD pointer selected by a CFGDATA-controlled index, or 0 if the
  index (SpdDataSelNNN, an unbounded UINT8 from CFGDATA) is out of range.

  @param[in]  SpdData       Array of candidate SPD pointers.
  @param[in]  SpdDataCount  Number of entries in SpdData.
  @param[in]  Index         CFGDATA-supplied selector index.

  @retval                   Selected SPD pointer value, or 0 if Index is out of range.
**/
STATIC
UINT32
GetSpdDataBySel (
  IN CONST UINT32  *SpdData,
  IN UINT32         SpdDataCount,
  IN UINT8          Index
  )
{
  if (Index >= SpdDataCount) {
    return 0;
  }
  return SpdData[Index];
}

#if FixedPcdGet8 (PcdTccEnabled)
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
  FEATURES_CFG_DATA                     *FeaturesCfgData;
  TCC_CFG_DATA                          *TccCfgData;

  TccCfgData = (TCC_CFG_DATA *) FindConfigDataByTag(CDATA_TCC_TAG);
  if ((TccCfgData == NULL) || (TccCfgData->TccEnable == 0)) {
    return EFI_UNSUPPORTED;
  }

  // S0ix is disabled if TCC is enabled.
  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
  if (FeaturesCfgData != NULL) {
    if (FeaturesCfgData->Features.S0ix == 1) {
      FeaturesCfgData->Features.S0ix = 0;
      DEBUG ((DEBUG_INFO, "S0ix is turned off when TCC is enabled\n"));
    }
  }

  return EFI_SUCCESS;
}
#endif

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
  POWER_CFG_DATA              *PowerCfgData;
  UINT16                      PlatformId;
  UINT8                       SaDisplayConfigTable[16] = { 0 };
  UINT8                       DebugPort;
  UINT32                      SpdData[7];
  UINT32                      CarBase;
  UINT32                      CarSize;
  EFI_STATUS                  Status;
  FSPM_UPD_COMMON_FSP24       *FspmUpdCommon;


  FspmUpd = (FSPM_UPD *)FspmUpdPtr;
  Fspmcfg = &FspmUpd->FspmConfig;

  FspmUpdCommon = (FSPM_UPD_COMMON_FSP24 *)FspmUpd;
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
    Fspmcfg->PcdDebugInterfaceFlags       = BIT4;
    Fspmcfg->SerialIoUartDebugControllerNumber = DebugPort;
    Fspmcfg->SerialIoUartDebugMode = 4;
  } else {
    Fspmcfg->PcdDebugInterfaceFlags       = BIT1;
    if (DebugPort == 0xFF) {
      Fspmcfg->PcdIsaSerialUartBase = 0;
    } else {
      Fspmcfg->PcdIsaSerialUartBase = 1;
    }
  }

  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
  if (MemCfgData == NULL) {
    CpuHalt ("Failed to find memory CFGDATA!");
    return;
  }

  // Vtd
  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if (PlatformData != NULL) {
    PlatformData->PlatformFeatures.VtdEnable = (!MemCfgData->VtdDisable) & FeaturePcdGet (PcdVtdEnabled);
  }

  ZeroMem (SpdData, sizeof(SpdData));
  SpdData[1] = (UINT32)(UINTN) (((MEM_SPD0_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD0_TAG))->MemorySpdPtr0);
  SpdData[2] = (UINT32)(UINTN) (((MEM_SPD1_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD1_TAG))->MemorySpdPtr1);
  SpdData[3] = (UINT32)(UINTN) (((MEM_SPD2_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD2_TAG))->MemorySpdPtr2);
  SpdData[4] = (UINT32)(UINTN) (((MEM_SPD3_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD3_TAG))->MemorySpdPtr3);
  Fspmcfg->MemorySpdPtr000  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel000);
  Fspmcfg->MemorySpdPtr001  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel001);
  Fspmcfg->MemorySpdPtr010  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel010);
  Fspmcfg->MemorySpdPtr011  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel011);
  Fspmcfg->MemorySpdPtr020  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel020);
  Fspmcfg->MemorySpdPtr021  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel021);
  Fspmcfg->MemorySpdPtr030  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel030);
  Fspmcfg->MemorySpdPtr031  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel031);
  Fspmcfg->MemorySpdPtr100  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel100);
  Fspmcfg->MemorySpdPtr101  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel101);
  Fspmcfg->MemorySpdPtr110  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel110);
  Fspmcfg->MemorySpdPtr111  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel111);
  Fspmcfg->MemorySpdPtr120  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel120);
  Fspmcfg->MemorySpdPtr121  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel121);
  Fspmcfg->MemorySpdPtr130  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel130);
  Fspmcfg->MemorySpdPtr131  = GetSpdDataBySel (SpdData, ARRAY_SIZE (SpdData), MemCfgData->SpdDataSel131);

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
  Fspmcfg->RcompResistor = MemCfgData->RcompResistor;
  CopyMem (&Fspmcfg->RcompTarget, MemCfgData->RcompTarget, sizeof(MemCfgData->RcompTarget));

  Fspmcfg->RmtPerTask               = MemCfgData->RmtPerTask;
  Fspmcfg->Disable2CycleBypass      = MemCfgData->Disable2CycleBypass;

  Fspmcfg->ScramblerSupport         = MemCfgData->ScramblerSupport;
  Fspmcfg->EnhancedInterleave       = MemCfgData->EnhancedInterleave;
  Fspmcfg->RankInterleave           = MemCfgData->RankInterleave;
  Fspmcfg->RemapEnable              = MemCfgData->RemapEnable;
  Fspmcfg->ExitOnFailure            = MemCfgData->ExitOnFailure;
  Fspmcfg->ChHashEnable             = MemCfgData->ChHashEnable;

  Fspmcfg->ChHashInterleaveBit      = MemCfgData->ChHashInterleaveBit;
  Fspmcfg->ChHashMask               = MemCfgData->ChHashMask;
  Fspmcfg->MrcFastBoot              = MemCfgData->MrcFastBoot;
  Fspmcfg->BoardStackUp             = 0;
  Fspmcfg->McSafeMode               = 0;
  Fspmcfg->SubChHashMask            = 0x838;
  Fspmcfg->SafeModeOverride         = 0xFF;
  Fspmcfg->DdrSafeMode              = 4;
  Fspmcfg->PlatformMemorySize       = MemCfgData->PlatformMemorySize;
  Fspmcfg->Lp5CccConfig             = MemCfgData->Lp5CccConfig;
  Fspmcfg->Lp5SplitACTEnable        = MemCfgData->Lp5SplitACTEnable;
  Fspmcfg->DramRfmMode              = 0;
  Fspmcfg->PrmrrSize                = MemCfgData->PrmrrSize;

  // Cpu Cfg
  Fspmcfg->CpuRatio                 = MemCfgData->CpuRatio;
  Fspmcfg->TmeEnable                = MemCfgData->TmeEnable;
  Fspmcfg->ActiveCoreCount          = MemCfgData->ActiveCoreCount;
  Fspmcfg->ActiveSmallCoreCount     = MemCfgData->ActiveSmallCoreCount;
  Fspmcfg->FClkFrequency            = MemCfgData->FClkFrequency;
  Fspmcfg->Txt = 0;
  Fspmcfg->TxtDprMemoryBase         = MemCfgData->TxtDprMemoryBase;
  Fspmcfg->TxtDprMemorySize         = MemCfgData->TxtDprMemorySize;
  Fspmcfg->SinitMemorySize          = MemCfgData->SinitMemorySize;
  Fspmcfg->TxtHeapMemorySize        = MemCfgData->TxtHeapMemorySize;
  Fspmcfg->BiosSize                 = MemCfgData->BiosSize;
  Fspmcfg->BiosGuard                = 0x0; // Need to disable, else it will fails in FSPS
  Fspmcfg->BiosGuardToolsInterface  = 1;
  Fspmcfg->SmmRelocationEnable      = 0;

  Fspmcfg->RMT                      = MemCfgData->RMT;
  Fspmcfg->BdatEnable               = MemCfgData->BdatEnable;
  Fspmcfg->BdatTestType             = MemCfgData->BdatTestType;
  Fspmcfg->RMC                      = MemCfgData->RMC;
  Fspmcfg->ECT                      = MemCfgData->ECT;
  Fspmcfg->SaGv                     = MemCfgData->SaGv;  //0x00 // This only affects ULX/ULT; otherwise SA GV is disabled.
  Fspmcfg->SaGvWpMask               = 0xF;
  Fspmcfg->OCSafeMode               = 1;
  Fspmcfg->DQDQSSWZ                 = 0x1;
  Fspmcfg->TLINECLKCAL              = 0x1;
  Fspmcfg->CMDVC                    = 0x1;
  Fspmcfg->DLLDCC                   = 0x1;
  Fspmcfg->RMTBIT                   = 0x0;
  Fspmcfg->DATAPILIN                = 0x1;

  //Pcie
  Fspmcfg->PcieRpEnableMask     = MemCfgData->PcieRpEnableMask;
  CopyMem (Fspmcfg->PcieClkSrcUsage,       MemCfgData->PcieClkSrcUsage,       sizeof(MemCfgData->PcieClkSrcUsage));
  CopyMem (Fspmcfg->PcieClkSrcClkReq,      MemCfgData->PcieClkSrcClkReq,      sizeof(MemCfgData->PcieClkSrcClkReq));


  // Hd Audio
  if (MemCfgData->PchHdaEnable) {
    Fspmcfg->PchHdaEnable                      = MemCfgData->PchHdaEnable;
    Fspmcfg->PchHdaDspEnable                   = MemCfgData->PchHdaDspEnable;
    Fspmcfg->PchHdaAudioLinkHdaEnable          = MemCfgData->PchHdaAudioLinkHdaEnable;
    Fspmcfg->PchHdaIDispLinkTmode              = MemCfgData->PchHdaIDispLinkTmode;
    Fspmcfg->PchHdaIDispLinkFrequency          = MemCfgData->PchHdaIDispLinkFrequency;
    Fspmcfg->PchHdaIDispCodecDisconnect        = MemCfgData->PchHdaIDispCodecDisconnect;
    CopyMem (Fspmcfg->PchHdaAudioLinkDmicEnable, MemCfgData->PchHdaAudioLinkDmicEnable, sizeof(MemCfgData->PchHdaAudioLinkDmicEnable));
    CopyMem (Fspmcfg->PchHdaAudioLinkSndwEnable, MemCfgData->PchHdaAudioLinkSndwEnable, sizeof(MemCfgData->PchHdaAudioLinkSndwEnable));
    CopyMem (Fspmcfg->PchHdaAudioLinkDmicClockSelect, MemCfgData->PchHdaAudioLinkDmicClockSelect, sizeof(MemCfgData->PchHdaAudioLinkDmicClockSelect));
    Fspmcfg->PchHdaAudioLinkMultilaneData1PinMux[1]  = 0;
    Fspmcfg->PchHdAudioSndwMultilaneSndwInterface[0] = 2;
    Fspmcfg->PchHdAudioSndwMultilaneSndwInterface[1] = 3;
    Fspmcfg->PchHdaSubSystemIds                = MemCfgData->PchHdaSubSystemIds;
  }

  Fspmcfg->SkipMbpHob = 0;

  GfxCfgData = (GRAPHICS_CFG_DATA *)FindConfigDataByTag (CDATA_GRAPHICS_TAG);
  if (GfxCfgData != NULL) {
    Fspmcfg->IgdDvmt50PreAlloc         = GfxCfgData->IgdDvmt50PreAlloc;
    Fspmcfg->InternalGraphics          = GfxCfgData->InternalGfx;
    Fspmcfg->PrimaryDisplay            = GfxCfgData->PrimaryDisplay;
  } else {
    DEBUG ((DEBUG_INFO, "Failed to find GFX CFG!\n"));
  }

  Fspmcfg->Ibecc                    = MemCfgData->Ibecc;
  Fspmcfg->IbeccOperationMode       = MemCfgData->IbeccOperationMode;
  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if (PlatformData != NULL) {
    PlatformData->PlatformFeatures.VtdEnable = (!MemCfgData->VtdDisable) & FeaturePcdGet (PcdVtdEnabled);
    if (PlatformData->PlatformFeatures.VtdEnable == 1) {
      Fspmcfg->VtdDisable          = (UINT8)(!PlatformData->PlatformFeatures.VtdEnable);
      Fspmcfg->VtdBaseAddress[0]   = VTD_BASEADDRESS_0;
      Fspmcfg->VtdBaseAddress[1]   = VTD_BASEADDRESS_1;
      Fspmcfg->VtdBaseAddress[2]   = VTD_BASEADDRESS_2;
      Fspmcfg->DmaBufferSize       = MemCfgData->DmaBufferSize;
      Fspmcfg->DmaControlGuarantee = MemCfgData->DmaControlGuarantee;
      Fspmcfg->PreBootDmaMask      = 0x1;
    }
  }

  // Host Bridge
  Fspmcfg->MmioSize                   = MemCfgData->MmioSize;
  Fspmcfg->MmioSizeAdjustment         = MemCfgData->MmioSizeAdjustment;
#if FixedPcdGetBool (PcdWclSupport) == 1
  Fspmcfg->FabricGvEnable             = 0;
#endif

  // Smbus
  Fspmcfg->SmbusEnable                = MemCfgData->SmbusEnable;
  Fspmcfg->PchNumRsvdSmbusAddresses   = MemCfgData->PchNumRsvdSmbusAddresses;
  Fspmcfg->RsvdSmbusAddressTablePtr   = (UINT32)(UINTN)MemCfgData->RsvdSmbusAddressTablePtr;

  // Overclocking Cfg
  Fspmcfg->RealtimeMemoryTiming       = MemCfgData->RealtimeMemoryTiming;

  // Tcc related settings
  Fspmcfg->DisPgCloseIdleTimeout      = MemCfgData->DisPgCloseIdleTimeout;  // controls Page Close Idle Timeout
  Fspmcfg->PowerDownMode              = MemCfgData->PowerDownMode;          // controls command bus tristating during idle periods
  Fspmcfg->VmxEnable                  = MemCfgData->VmxEnable;

  // Tcss Dev enable bits

  // Misc
  Fspmcfg->TcssXhciEn             = MemCfgData->TcssXhciEn;
  Fspmcfg->UserBd                 = MemCfgData->UserBd;
  Fspmcfg->TsegSize               = MemCfgData->TsegSize;
  Fspmcfg->SkipExtGfxScan         = MemCfgData->SkipExtGfxScan;

  if (Fspmcfg->PlatformDebugOption != 0) {
    Fspmcfg->DebugInterfaceLockEnable     = TRUE;
    Fspmcfg->DciDbcMode                   = 0;
    Fspmcfg->DciEn                        = MemCfgData->DciEn;
    Fspmcfg->SocTraceHubMode              = MemCfgData->PchTraceHubMode;
    Fspmcfg->SocTraceHubMemReg0Size       = MemCfgData->PchTraceHubMemReg0Size;
    Fspmcfg->SocTraceHubMemReg1Size       = MemCfgData->PchTraceHubMemReg1Size;
  }

  // TXT Configuration
  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag(CDATA_FEATURES_TAG);
  if ((FeaturesCfgData != NULL) && (FeaturesCfgData->Features.TxtEnabled == 1) &&
      (FeaturePcdGet (PcdTxtEnabled))) {
    DEBUG((DEBUG_INFO, "Enabling TXT in FSP-M UPD's\n"));
    Fspmcfg->Txt                  = 0x1;
    Fspmcfg->TxtImplemented       = 0x1;
    Fspmcfg->SinitMemorySize      = TXT_SINIT_MEMORY_SIZE;
    Fspmcfg->TxtHeapMemorySize    = TXT_HEAP_MEMORY_SIZE;
    Fspmcfg->BiosAcmBase          = (UINT32)(UINTN)FindAcm();
    Fspmcfg->VmxEnable            = 1;    // Txt need enable VMX

    IoWrite8 (R_IOPORT_CMOS_STANDARD_INDEX, TXT_CMOS_STATUS_REG);
    UINT8 CmosData = IoRead8 (R_IOPORT_CMOS_STANDARD_DATA);
    if (!(CmosData & BIT4)) {
      CmosData |= BIT4;
      IoWrite8 (R_IOPORT_CMOS_STANDARD_INDEX, TXT_CMOS_STATUS_REG);
      IoWrite8 (R_IOPORT_CMOS_STANDARD_DATA, CmosData);
    }
  } else {
    DEBUG((DEBUG_INFO, "TXT is disabled\n"));
    IoWrite8 (R_IOPORT_CMOS_STANDARD_INDEX, TXT_CMOS_STATUS_REG);
    UINT8 CmosData = IoRead8 (R_IOPORT_CMOS_STANDARD_DATA);
    CmosData &= ~BIT4;
    IoWrite8 (R_IOPORT_CMOS_STANDARD_INDEX, TXT_CMOS_STATUS_REG);
    IoWrite8 (R_IOPORT_CMOS_STANDARD_DATA, CmosData);
  }

  PlatformId = GetPlatformId();
  //
  // Display DDI Initialization ( default Native GPIO as per board during AUTO case)
  //
  switch (PlatformId) {
  case PLATFORM_ID_PTL_UH_LP5X_T4_RVP:
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mPtlLpDdr5RvpRowDisplayDdiConfig, sizeof(mPtlLpDdr5RvpRowDisplayDdiConfig));
    break;
  case PLATFORM_ID_PTL_UH_LP5X_Robinson:
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mPtlRobinsonDisplayDdiConfig, sizeof(mPtlRobinsonDisplayDdiConfig));
    break;
  case PLATFORM_ID_PTL_UH_LP5X_T3_RVP:
  case PLATFORM_ID_PTL_UH_DDR5_T3_RVP:
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mPtlDdr5RvpRowDisplayDdiConfig, sizeof(mPtlDdr5RvpRowDisplayDdiConfig));
    break;
  case PLATFORM_ID_PTL_UH_DDR5_EMB_CRB:
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mPtlDdr5EmbCrbDisplayDdiConfig, sizeof(mPtlDdr5EmbCrbDisplayDdiConfig));
    break;
  case PLATFORM_ID_WCL_DDR5_CRB:
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mWclDdr5CrbDisplayDdiConfig, sizeof(mWclDdr5CrbDisplayDdiConfig));
    break;
  case PLATFORM_ID_WCL_LP5X_RVP:
  case PLATFORM_ID_WCL_DDR5_RVP:
    CopyMem(SaDisplayConfigTable, (VOID *)(UINTN)mWclLpDdr5RvpDisplayDdiConfig, sizeof(mWclLpDdr5RvpDisplayDdiConfig));
    break;
  default:
    DEBUG((DEBUG_INFO, "DDI Init: Unsupported board Id %x .....\n", PlatformId));
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
  Fspmcfg->LidStatus        = 0x1;

  Fspmcfg->Lfsr0Mask      = 0xb;
  Fspmcfg->Lfsr1Mask      = 0xb;

  // Tcc enabling
#if FixedPcdGet8 (PcdTccEnabled)
  TccModePreMemConfig (FspmUpd);
#endif

  Fspmcfg->GttMmAdr = GTT_MM_ADDR;
  Fspmcfg->LMemBar = LMEM_BAR;

  PowerCfgData = (POWER_CFG_DATA *) FindConfigDataByTag (CDATA_POWER_TAG);
  if (PowerCfgData != NULL) {
    DEBUG ((DEBUG_INFO, "PowerCfgData found!\n"));
    Fspmcfg->PsysPmax                                             = 0x0;
    Fspmcfg->PowerLimit1Time               = PowerCfgData->PowerLimit1Time;
    Fspmcfg->PowerLimit3Time               = PowerCfgData->PowerLimit3Time;
    Fspmcfg->PowerLimit1                   = PowerCfgData->PowerLimit1;
    Fspmcfg->PowerLimit2                   = PowerCfgData->PowerLimit2;
    Fspmcfg->PowerLimit3                   = PowerCfgData->PowerLimit3;
    Fspmcfg->PowerLimit4                   = PowerCfgData->PowerLimit4;
    Fspmcfg->PowerLimit2Power              = PowerCfgData->PowerLimit2Power;
    Fspmcfg->PowerLimit3DutyCycle          = PowerCfgData->PowerLimit3DutyCycle;
    Fspmcfg->PowerLimit3Lock               = PowerCfgData->PowerLimit3Lock;
    Fspmcfg->PowerLimit4Lock               = PowerCfgData->PowerLimit4Lock;
    Fspmcfg->TccActivationOffset           = PowerCfgData->TccActivationOffset;
    Fspmcfg->TccOffsetLock                 = PowerCfgData->TccOffsetLock;
    Fspmcfg->ConfigTdpLock                 = PowerCfgData->ConfigTdpLock;
    Fspmcfg->ConfigTdpBios                 = PowerCfgData->ConfigTdpBios;
    Fspmcfg->PsysPowerLimit1               = PowerCfgData->PsysPowerLimit1;
    Fspmcfg->PsysPowerLimit1Time           = PowerCfgData->PsysPowerLimit1Time;
    Fspmcfg->PsysPowerLimit2               = PowerCfgData->PsysPowerLimit2;
    Fspmcfg->ConfigTdpLevel                = PowerCfgData->ConfigTdpLevel;
    Fspmcfg->Custom1PowerLimit1            = PowerCfgData->Custom1PowerLimit1;
    Fspmcfg->Custom1PowerLimit2            = PowerCfgData->Custom1PowerLimit2;
    Fspmcfg->PsysPowerLimit1Power          = PowerCfgData->PsysPowerLimit1Power;
    Fspmcfg->PsysPowerLimit2Power          = PowerCfgData->PsysPowerLimit2Power;
    Fspmcfg->DisableVrThermalAlert         = PowerCfgData->DisableVrThermalAlert;
    Fspmcfg->ThermalMonitor                = PowerCfgData->ThermalMonitor;
    Fspmcfg->PsysSlope                     = PowerCfgData->PsysSlope;
    Fspmcfg->PsysOffset                    = PowerCfgData->PsysOffset;
    Fspmcfg->AcousticNoiseMitigation       = PowerCfgData->AcousticNoiseMitigation;
    Fspmcfg->BootMaxFrequency              = PowerCfgData->BootMaxFrequency;

#if FixedPcdGetBool (PcdWclSupport) == 1
    Fspmcfg->FastPkgCRampDisable[1]        = 1;
#endif

    //CPU VR Power Management Custom Config
    CopyMem (Fspmcfg->VrConfigEnable, PowerCfgData->VrConfigEnable, sizeof(PowerCfgData->VrConfigEnable));
    CopyMem (Fspmcfg->ImonSlope, PowerCfgData->ImonSlope, sizeof(PowerCfgData->ImonSlope));
    CopyMem (Fspmcfg->TdcEnable, PowerCfgData->TdcEnable, sizeof(PowerCfgData->TdcEnable));
    CopyMem (Fspmcfg->TdcLock, PowerCfgData->TdcLock, sizeof(PowerCfgData->TdcLock));
    CopyMem (Fspmcfg->TdcTimeWindow, PowerCfgData->TdcTimeWindow, sizeof(PowerCfgData->TdcTimeWindow));
    CopyMem (Fspmcfg->SlowSlewRate, PowerCfgData->SlowSlewRate, sizeof(PowerCfgData->SlowSlewRate));
    CopyMem (Fspmcfg->TdcCurrentLimit, PowerCfgData->TdcCurrentLimit, sizeof(PowerCfgData->TdcCurrentLimit));
    CopyMem (Fspmcfg->AcLoadline, PowerCfgData->AcLoadline, sizeof(PowerCfgData->AcLoadline));
    CopyMem (Fspmcfg->DcLoadline, PowerCfgData->DcLoadline, sizeof(PowerCfgData->DcLoadline));
    CopyMem (Fspmcfg->IccMax, PowerCfgData->IccMax, sizeof(PowerCfgData->IccMax));
    CopyMem (Fspmcfg->VrVoltageLimit, PowerCfgData->VrVoltageLimit, sizeof(PowerCfgData->VrVoltageLimit));
    CopyMem (Fspmcfg->Ps1Threshold, PowerCfgData->Ps1Threshold, sizeof(PowerCfgData->Ps1Threshold));
    CopyMem (Fspmcfg->Ps2Threshold, PowerCfgData->Ps2Threshold, sizeof(PowerCfgData->Ps2Threshold));
    CopyMem (Fspmcfg->Ps3Threshold, PowerCfgData->Ps3Threshold, sizeof(PowerCfgData->Ps3Threshold));
  }

  Fspmcfg->DlvrSpreadSpectrumPercentage    = 2;
  Fspmcfg->ActiveLpAtomCoreCount           = MemCfgData->ActiveLpAtomCoreCount;
  Fspmcfg->I2cPostCodeEnable               = MemCfgData->I2cPostCodeEnable;

  CopyMem(Fspmcfg->DqMapCpu2DramMc0Ch0, MemCfgData->DqMapCpu2DramMc0Ch0, sizeof(MemCfgData->DqMapCpu2DramMc0Ch0));
  CopyMem(Fspmcfg->DqMapCpu2DramMc0Ch1, MemCfgData->DqMapCpu2DramMc0Ch1, sizeof(MemCfgData->DqMapCpu2DramMc0Ch1));
  CopyMem(Fspmcfg->DqMapCpu2DramMc0Ch2, MemCfgData->DqMapCpu2DramMc0Ch2, sizeof(MemCfgData->DqMapCpu2DramMc0Ch2));
  CopyMem(Fspmcfg->DqMapCpu2DramMc0Ch3, MemCfgData->DqMapCpu2DramMc0Ch3, sizeof(MemCfgData->DqMapCpu2DramMc0Ch3));
  CopyMem(Fspmcfg->DqMapCpu2DramMc1Ch0, MemCfgData->DqMapCpu2DramMc1Ch0, sizeof(MemCfgData->DqMapCpu2DramMc1Ch0));
  CopyMem(Fspmcfg->DqMapCpu2DramMc1Ch1, MemCfgData->DqMapCpu2DramMc1Ch1, sizeof(MemCfgData->DqMapCpu2DramMc1Ch1));
  CopyMem(Fspmcfg->DqMapCpu2DramMc1Ch2, MemCfgData->DqMapCpu2DramMc1Ch2, sizeof(MemCfgData->DqMapCpu2DramMc1Ch2));
  CopyMem(Fspmcfg->DqMapCpu2DramMc1Ch3, MemCfgData->DqMapCpu2DramMc1Ch3, sizeof(MemCfgData->DqMapCpu2DramMc1Ch3));

  CopyMem(Fspmcfg->ChannelToCkdQckMapping, MemCfgData->ChannelToCkdQckMapping, sizeof(MemCfgData->ChannelToCkdQckMapping));
  CopyMem(Fspmcfg->PhyClockToCkdDimm, MemCfgData->PhyClockToCkdDimm, sizeof(MemCfgData->PhyClockToCkdDimm));

  Fspmcfg->IgdDvmt50PreAlloc         = 0x2;
  Fspmcfg->CsVrefLow                 = 0x45;
  Fspmcfg->CsVrefHigh                = 0x1d;
  Fspmcfg->CaVrefLow                 = 0x45;
  Fspmcfg->CaVrefHigh                = 0x1d;
  Fspmcfg->MarginLimitCheck          = 0x0;
  Fspmcfg->HgSubSystemId             = 0x2212;
  Fspmcfg->SkipCpuReplacementCheck   = 0x0;
  Fspmcfg->CridEnable                = 0x0;
  Fspmcfg->DrfmBrc                   = 0x2;
  Fspmcfg->LpMode                    = 0x7;
  Fspmcfg->LpMode4                   = 0x1;
  Fspmcfg->TargetedRowRefreshMode    = 0x1;
  Fspmcfg->PprTestDisabled           = 0x1;
  Fspmcfg->VgaInitControl            = 0x0;
  Fspmcfg->TcssPort0                 = MemCfgData->TcssPort0;
  Fspmcfg->TcssPort1                 = MemCfgData->TcssPort1;
  Fspmcfg->TcssPort2                 = MemCfgData->TcssPort2;
  Fspmcfg->TcssPort3                 = MemCfgData->TcssPort3;
  CopyMem(Fspmcfg->IomUsbCDpConfig, MemCfgData->IomUsbCDpConfig, sizeof(MemCfgData->IomUsbCDpConfig));
  Fspmcfg->TcssPlatConf              = MemCfgData->TcssPlatConf;
  Fspmcfg->PchIshEnable              = MemCfgData->PchIshEnable;

  Fspmcfg->DisableMc0Ch0  = 0;
  Fspmcfg->DisableMc0Ch1  = 0;
  Fspmcfg->DisableMc0Ch2  = 0;
  Fspmcfg->DisableMc0Ch3  = 0;
  Fspmcfg->DisableMc1Ch0  = MemCfgData->DisableMc1Ch0;
  Fspmcfg->DisableMc1Ch1  = MemCfgData->DisableMc1Ch1;
  Fspmcfg->DisableMc1Ch2  = MemCfgData->DisableMc1Ch2;
  Fspmcfg->DisableMc1Ch3  = MemCfgData->DisableMc1Ch3;

 /* Telemetry PEI Config */
  Fspmcfg->CpuCrashLogDevice = 1;
  Fspmcfg->StreamTracerSize = 0;
  Fspmcfg->StreamTracerMode = 0;

  Status = FusaConfigPreMem(FspmUpdPtr);
  DEBUG((DEBUG_INFO, "FusaConfigPreMem Status %r\n", Status));

  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
  if (FeaturesCfgData != NULL) {
    if (FeaturesCfgData->Features.S0ix == 1) {
      Fspmcfg->PchIshEnable       = 0;
      Fspmcfg->SaIpuEnable        = 0;
      Fspmcfg->TcssXhciEn         = 0;
      Fspmcfg->TcssPort0          = 0x0;
      Fspmcfg->TcssPort1          = 0x0;
      Fspmcfg->TcssPort2          = 0x0;
      Fspmcfg->TcssPort3          = 0x0;
      Fspmcfg->PchHdaEnable      = 0;
      Fspmcfg->PchHdaDspEnable    = 0;
      Fspmcfg->PchHdaAudioLinkHdaEnable  = 0;
      ZeroMem (Fspmcfg->PchHdaAudioLinkDmicEnable, sizeof (Fspmcfg->PchHdaAudioLinkDmicEnable));
      DEBUG ((DEBUG_INFO, "Stage 1B S0ix config applied.\n"));
    }
  }

  if (GetBootMode() == BOOT_ON_FLASH_UPDATE) {
    Fspmcfg->SiSkipOverrideBootModeWhenFwUpdate = TRUE;
  }

  DEBUG((DEBUG_INFO, "UpdateFspConfig exit\n"));
}
