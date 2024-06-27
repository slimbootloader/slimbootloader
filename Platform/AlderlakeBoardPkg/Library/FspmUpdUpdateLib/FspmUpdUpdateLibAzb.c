/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
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
#include <GpioPinsVer2Lp.h>

#include "BoardSaConfigPreMem.h"

typedef enum {
  DciDbcDisabled  = 0x0,
  DciDbcUsb2      = 0x1,
  DciDbcUsb3      = 0x2,
  DciDbcBoth      = 0x3,
  DciDbcNoChange  = 0x4,
  DciDbcMax
} DCI_DBC_MODE;


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
  FSPM_UPD             *FspmUpd;
  FSP_M_CONFIG         *Fspmcfg;
  MEMORY_CFG_DATA      *MemCfgData;
  PLATFORM_DATA        *PlatformData;
  FEATURES_CFG_DATA    *FeaturesCfgData;
  SILICON_CFG_DATA     *SiCfgData;
  UINT16               PlatformId;
  UINT8                SaDisplayConfigTable[16] = { 0 };
  UINT8                Index;
  UINT8                DebugPort;
  UINT32               SpdData[9];
  UINT32               CarBase;
  UINT32               CarSize;
  EFI_STATUS           Status;

  FspmUpd = (FSPM_UPD *)FspmUpdPtr;
  Fspmcfg = &FspmUpd->FspmConfig;

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
    Fspmcfg->PcdDebugInterfaceFlags = BIT4 | BIT5;
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

  Fspmcfg->MemorySpdPtr000 = SpdData[MemCfgData->SpdDataSel000];
  Fspmcfg->MemorySpdPtr001 = SpdData[MemCfgData->SpdDataSel001];
  Fspmcfg->MemorySpdPtr010 = SpdData[MemCfgData->SpdDataSel010];
  Fspmcfg->MemorySpdPtr011 = SpdData[MemCfgData->SpdDataSel011];
  Fspmcfg->MemorySpdPtr020 = SpdData[MemCfgData->SpdDataSel020];
  Fspmcfg->MemorySpdPtr021 = SpdData[MemCfgData->SpdDataSel021];
  Fspmcfg->MemorySpdPtr030 = SpdData[MemCfgData->SpdDataSel030];
  Fspmcfg->MemorySpdPtr031 = SpdData[MemCfgData->SpdDataSel031];
  Fspmcfg->MemorySpdPtr100 = SpdData[MemCfgData->SpdDataSel100];
  Fspmcfg->MemorySpdPtr101 = SpdData[MemCfgData->SpdDataSel101];
  Fspmcfg->MemorySpdPtr110 = SpdData[MemCfgData->SpdDataSel110];
  Fspmcfg->MemorySpdPtr111 = SpdData[MemCfgData->SpdDataSel111];
  Fspmcfg->MemorySpdPtr120 = SpdData[MemCfgData->SpdDataSel120];
  Fspmcfg->MemorySpdPtr121 = SpdData[MemCfgData->SpdDataSel121];
  Fspmcfg->MemorySpdPtr130 = SpdData[MemCfgData->SpdDataSel130];
  Fspmcfg->MemorySpdPtr131 = SpdData[MemCfgData->SpdDataSel131];

  //Dq/Dqs Mapping arrays
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc0Ch0, MemCfgData->DqsMapCpu2DramMc0Ch0, sizeof(MemCfgData->DqsMapCpu2DramMc0Ch0));
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc0Ch1, MemCfgData->DqsMapCpu2DramMc0Ch1, sizeof(MemCfgData->DqsMapCpu2DramMc0Ch1));
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc0Ch2, MemCfgData->DqsMapCpu2DramMc0Ch2, sizeof(MemCfgData->DqsMapCpu2DramMc0Ch2));
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc0Ch3, MemCfgData->DqsMapCpu2DramMc0Ch3, sizeof(MemCfgData->DqsMapCpu2DramMc0Ch3));
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc1Ch0, MemCfgData->DqsMapCpu2DramMc1Ch0, sizeof(MemCfgData->DqsMapCpu2DramMc1Ch0));
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc1Ch1, MemCfgData->DqsMapCpu2DramMc1Ch1, sizeof(MemCfgData->DqsMapCpu2DramMc1Ch1));
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc1Ch2, MemCfgData->DqsMapCpu2DramMc1Ch2, sizeof(MemCfgData->DqsMapCpu2DramMc1Ch2));
  CopyMem (&Fspmcfg->DqsMapCpu2DramMc1Ch3, MemCfgData->DqsMapCpu2DramMc1Ch3, sizeof(MemCfgData->DqsMapCpu2DramMc1Ch3));

  CopyMem (&Fspmcfg->DqMapCpu2DramMc0Ch0,  MemCfgData->DqMapCpu2DramMc0Ch0,  sizeof(MemCfgData->DqMapCpu2DramMc0Ch0));
  CopyMem (&Fspmcfg->DqMapCpu2DramMc0Ch1,  MemCfgData->DqMapCpu2DramMc0Ch1,  sizeof(MemCfgData->DqMapCpu2DramMc0Ch1));
  CopyMem (&Fspmcfg->DqMapCpu2DramMc0Ch2,  MemCfgData->DqMapCpu2DramMc0Ch2,  sizeof(MemCfgData->DqMapCpu2DramMc0Ch2));
  CopyMem (&Fspmcfg->DqMapCpu2DramMc0Ch3,  MemCfgData->DqMapCpu2DramMc0Ch3,  sizeof(MemCfgData->DqMapCpu2DramMc0Ch3));
  CopyMem (&Fspmcfg->DqMapCpu2DramMc1Ch0,  MemCfgData->DqMapCpu2DramMc1Ch0,  sizeof(MemCfgData->DqMapCpu2DramMc1Ch0));
  CopyMem (&Fspmcfg->DqMapCpu2DramMc1Ch1,  MemCfgData->DqMapCpu2DramMc1Ch1,  sizeof(MemCfgData->DqMapCpu2DramMc1Ch1));
  CopyMem (&Fspmcfg->DqMapCpu2DramMc1Ch2,  MemCfgData->DqMapCpu2DramMc1Ch2,  sizeof(MemCfgData->DqMapCpu2DramMc1Ch2));
  CopyMem (&Fspmcfg->DqMapCpu2DramMc1Ch3,  MemCfgData->DqMapCpu2DramMc1Ch3,  sizeof(MemCfgData->DqMapCpu2DramMc1Ch3));

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

  // MRC Training algorithms
  Fspmcfg->RMT                  = MemCfgData->RMT;
  Fspmcfg->BdatEnable           = MemCfgData->BdatEnable;
  Fspmcfg->BdatTestType         = MemCfgData->BdatTestType;
  Fspmcfg->RMC                  = MemCfgData->RMC;
  Fspmcfg->MEMTST               = MemCfgData->MEMTST;
  Fspmcfg->ECT                  = MemCfgData->ECT;

  // Pcie
  Fspmcfg->PcieRpEnableMask     = MemCfgData->PcieRpEnableMask;
  CopyMem (Fspmcfg->PcieClkSrcUsage,       MemCfgData->PcieClkSrcUsage,       sizeof(MemCfgData->PcieClkSrcUsage));
  CopyMem (Fspmcfg->PcieClkSrcClkReq,      MemCfgData->PcieClkSrcClkReq,      sizeof(MemCfgData->PcieClkSrcClkReq));

  // Cpu Pcie
  Fspmcfg->CpuPcieRpEnableMask           = MemCfgData->CpuPcieRpEnableMask;
  CopyMem (Fspmcfg->CpuPcieRpPcieSpeed, MemCfgData->CpuPcieRpPcieSpeed, sizeof(MemCfgData->CpuPcieRpPcieSpeed));
  Fspmcfg->CpuPcieRpClockReqMsgEnable[0] = MemCfgData->CpuPcieRpClockReqMsgEnable[0];
  Fspmcfg->CpuPcieRpClockReqMsgEnable[1] = MemCfgData->CpuPcieRpClockReqMsgEnable[1];
  Fspmcfg->CpuPcieRpClockReqMsgEnable[2] = MemCfgData->CpuPcieRpClockReqMsgEnable[2];

  Fspmcfg->SkipMbpHob = 0;
  DEBUG ((DEBUG_INFO, "SkipMbpHob = 0x%x\n", Fspmcfg->SkipMbpHob));

  // IBECC - removed in the latest FSP. Add them later when needed
  Fspmcfg->Ibecc                        = MemCfgData->Ibecc;
  Fspmcfg->IbeccOperationMode           = MemCfgData->IbeccOperationMode;
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
    Fspmcfg->VtdDisable   = (UINT8)(!PlatformData->PlatformFeatures.VtdEnable);
    if (PlatformData->PlatformFeatures.VtdEnable == 1) {
      Fspmcfg->VtdIgdEnable           = MemCfgData->VtdIgdEnable;
      Fspmcfg->VtdIpuEnable           = 0x1;
      Fspmcfg->VtdIopEnable           = 0x1;

      Fspmcfg->VtdBaseAddress[0]      = VTD_BASEADDRESS_0;
      Fspmcfg->VtdBaseAddress[1]      = 0x0;
      Fspmcfg->VtdBaseAddress[2]      = VTD_BASEADDRESS_2;

      Fspmcfg->X2ApicOptOut           = MemCfgData->X2ApicOptOut;
      Fspmcfg->DmaBufferSize          = MemCfgData->DmaBufferSize;
      Fspmcfg->DmaControlGuarantee    = MemCfgData->DmaControlGuarantee;
      Fspmcfg->PreBootDmaMask         = 0x1;
    }
  }

  // Host Bridge
  Fspmcfg->MmioSize                   = MemCfgData->MmioSize;
  Fspmcfg->MmioSizeAdjustment         = MemCfgData->MmioSizeAdjustment;

  // Smbus
  Fspmcfg->SmbusEnable                = MemCfgData->SmbusEnable;
  Fspmcfg->PchNumRsvdSmbusAddresses   = sizeof (MemCfgData->RsvdSmbusAddressTablePtr) / sizeof (MemCfgData->RsvdSmbusAddressTablePtr[0]);
  Fspmcfg->RsvdSmbusAddressTablePtr   = (UINT32)(UINTN)MemCfgData->RsvdSmbusAddressTablePtr;

  // Overclocking Cfg
  Fspmcfg->RealtimeMemoryTiming       = MemCfgData->RealtimeMemoryTiming;

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
  Fspmcfg->DmiHweq         = 0x2;
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
  Fspmcfg->TcssItbtPcie0En      = MemCfgData->TcssItbtPcie0En;
  Fspmcfg->TcssItbtPcie1En      = MemCfgData->TcssItbtPcie1En;
  Fspmcfg->TcssItbtPcie2En      = MemCfgData->TcssItbtPcie2En;
  Fspmcfg->TcssItbtPcie3En      = MemCfgData->TcssItbtPcie3En;
  Fspmcfg->TcssXhciEn           = MemCfgData->TcssXhciEn;
  Fspmcfg->TcssDma0En           = MemCfgData->TcssDma0En;
  Fspmcfg->TcssDma1En           = MemCfgData->TcssDma1En;
  Fspmcfg->UsbTcPortEnPreMem    = MemCfgData->UsbTcPortEnPreMem;

  // Misc
  Fspmcfg->UserBd               = MemCfgData->UserBd;
  Fspmcfg->EnableC6Dram         = MemCfgData->EnableC6Dram;
  Fspmcfg->TsegSize             = MemCfgData->TsegSize;
  Fspmcfg->SkipExtGfxScan       = MemCfgData->SkipExtGfxScan;
  Fspmcfg->LockPTMregs          = MemCfgData->LockPTMregs;
  Fspmcfg->CridEnable           = 0x0;
  Fspmcfg->Lfsr0Mask            = 0xb;
  Fspmcfg->Lfsr1Mask            = 0xb;
  Fspmcfg->RefreshPanicWm       = 0x8;
  Fspmcfg->RefreshHpWm          = 0x7;

  // Hda Audio
  Fspmcfg->PchHdaAudioLinkDmicEnable[0]   = 0;
  Fspmcfg->PchHdaAudioLinkDmicEnable[1]   = 0;

  Fspmcfg->PlatformDebugConsent           = MemCfgData->PlatformDebugConsent;
  if (Fspmcfg->PlatformDebugConsent != 0) {
    Fspmcfg->DciModphyPg                  = 0;
    Fspmcfg->DciUsb3TypecUfpDbg           = 2;
    Fspmcfg->DebugInterfaceLockEnable     = TRUE;
    Fspmcfg->DciDbcMode                   = DciDbcNoChange;
    Fspmcfg->DciEn                        = MemCfgData->DciEn;
    Fspmcfg->CpuTraceHubMode              = MemCfgData->CpuTraceHubMode;
    Fspmcfg->PchTraceHubMode              = MemCfgData->PchTraceHubMode;
    Fspmcfg->CpuTraceHubMemReg0Size       = MemCfgData->CpuTraceHubMemReg0Size;
    Fspmcfg->CpuTraceHubMemReg1Size       = MemCfgData->CpuTraceHubMemReg1Size;
    Fspmcfg->PchTraceHubMemReg0Size       = MemCfgData->PchTraceHubMemReg0Size;
    Fspmcfg->PchTraceHubMemReg1Size       = MemCfgData->PchTraceHubMemReg1Size;
  }

  PlatformId = GetPlatformId();
  DEBUG((DEBUG_INFO, "PLATFORM_ID_AZB_LP5_CRB2A board Id %x .....\n", PlatformId));

  Fspmcfg->DdiPortAConfig = SaDisplayConfigTable[0];
  Fspmcfg->DdiPortBConfig = SaDisplayConfigTable[1];
  Fspmcfg->DdiPortAHpd    = SaDisplayConfigTable[2];
  Fspmcfg->DdiPortBHpd    = SaDisplayConfigTable[3];
  Fspmcfg->DdiPortCHpd    = SaDisplayConfigTable[4];
  Fspmcfg->DdiPort1Hpd    = SaDisplayConfigTable[5];
  Fspmcfg->DdiPort2Hpd    = SaDisplayConfigTable[6];
  Fspmcfg->DdiPort3Hpd    = SaDisplayConfigTable[7];
  Fspmcfg->DdiPort4Hpd    = SaDisplayConfigTable[8];
  Fspmcfg->DdiPortADdc    = SaDisplayConfigTable[9];
  Fspmcfg->DdiPortBDdc    = SaDisplayConfigTable[10];
  Fspmcfg->DdiPortCDdc    = SaDisplayConfigTable[11];
  Fspmcfg->DdiPort1Ddc    = SaDisplayConfigTable[12];
  Fspmcfg->DdiPort2Ddc    = SaDisplayConfigTable[13];
  Fspmcfg->DdiPort3Ddc    = SaDisplayConfigTable[14];
  Fspmcfg->DdiPort4Ddc    = SaDisplayConfigTable[15];

  if (IsPchLp ()) {
    switch (GetPlatformId ()) {
      case PLATFORM_ID_AZB_LP5_CRB2A:
        Fspmcfg->DdiPortBHpd             = 0x1;
        Fspmcfg->DmiHweq                 = 0x2;
        Fspmcfg->PcieClkReqGpioMux[9]    = 0x796e9000;
        Fspmcfg->SkipCpuReplacementCheck = 0x0;
        Fspmcfg->FirstDimmBitMask        = 0;
        Fspmcfg->FirstDimmBitMaskEcc     = 0;
        Fspmcfg->Lp5CccConfig            = 0xFF;
        Fspmcfg->GearRatio               = 4;
        Fspmcfg->DdrFreqLimit            = 5200;
        Fspmcfg->SaGv                    = MemCfgData->SaGv;
        Fspmcfg->DisableMc0Ch0           = 1;
        Fspmcfg->DisableMc0Ch1           = 1;
        Fspmcfg->DisableMc0Ch2           = 1;
        Fspmcfg->DisableMc0Ch3           = 1;
        break;
      default:
        DEBUG ((DEBUG_INFO, "Unknown board for FSP-M UPD settings.\n"));
        break;
    }
  }
  if (PcdGetBool (PcdFastBootEnabled) && !(IsPchP ())) {
    Fspmcfg->CpuPcieRpEnableMask = 0;
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
      Fspmcfg->TcssXdciEn         = 0;
      Fspmcfg->PchIshEnable       = 1;
      ZeroMem (Fspmcfg->PchHdaAudioLinkDmicEnable, sizeof (Fspmcfg->PchHdaAudioLinkDmicEnable));
      DEBUG ((DEBUG_INFO, "Stage 1B S0ix config applied.\n"));
    }
  }
}
