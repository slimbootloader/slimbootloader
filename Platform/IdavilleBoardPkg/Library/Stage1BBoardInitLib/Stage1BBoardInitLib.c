/** @file

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage1BBoardInitLib.h"

CONST PLT_DEVICE mPlatformDevices[] = {
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = PCI_FUNCTION_NUMBER_PCH_SPI,
        .PciDeviceNumber    = PCI_DEVICE_NUMBER_PCH_SPI,
        .PciBusNumber       = DEFAULT_PCI_BUS_NUMBER_PCH,
        .IsMmioDevice       = 0
      }
    },
    OsBootDeviceSpi,
    0
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = PCI_FUNCTION_NUMBER_PCH_XHCI,
        .PciDeviceNumber    = PCI_DEVICE_NUMBER_PCH_XHCI,
        .PciBusNumber       = DEFAULT_PCI_BUS_NUMBER_PCH,
        .IsMmioDevice       = 0
      }
    },
    OsBootDeviceUsb,
    0
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = PCI_FUNCTION_NUMBER_PCH_SATA_1,
        .PciDeviceNumber    = PCI_DEVICE_NUMBER_PCH_SATA_1,
        .PciBusNumber       = DEFAULT_PCI_BUS_NUMBER_PCH,
        .IsMmioDevice       = 0
      }
    },
    OsBootDeviceSata,
    0
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = PCI_FUNCTION_NUMBER_PCH_SATA_2,
        .PciDeviceNumber    = PCI_DEVICE_NUMBER_PCH_SATA_2,
        .PciBusNumber       = DEFAULT_PCI_BUS_NUMBER_PCH,
        .IsMmioDevice       = 0
      }
    },
    OsBootDeviceSata,
    1
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = PCI_FUNCTION_NUMBER_PCH_SATA_3,
        .PciDeviceNumber    = PCI_DEVICE_NUMBER_PCH_SATA_3,
        .PciBusNumber       = DEFAULT_PCI_BUS_NUMBER_PCH,
        .IsMmioDevice       = 0
      }
    },
    OsBootDeviceSata,
    2
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = PCI_FUNCTION_NUMBER_PCH_CDF_SCS_EMMC,
        .PciDeviceNumber    = PCI_DEVICE_NUMBER_PCH_CDF_SCS_EMMC,
        .PciBusNumber       = DEFAULT_PCI_BUS_NUMBER_PCH,
        .IsMmioDevice       = 0
      }
    },
    OsBootDeviceEmmc,
    0
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = 0,
        .PciDeviceNumber    = 0,
        .PciBusNumber       = 0x3,
        .IsMmioDevice       = 0
      }
    },
    OsBootDeviceNvme,
    0
  }
};

/**
  Update FSP-M UPD config data for TCC mode and tuning

  @param  FspmUpd         The pointer to the FSP-M UPD to be updated.

  @retval None
**/
VOID
TccModePreMemConfig (
  FSPM_UPD  *FspmUpd
)
{
  UINT32                     *TccStreamBase;
  UINT32                      TccStreamSize;
  BIOS_SETTINGS              *PolicyConfig;
  TCC_STREAM_CONFIGURATION   *StreamConfig;
  EFI_STATUS                  Status;
  TCC_CFG_DATA               *TccCfgData;

  TccCfgData = (TCC_CFG_DATA *) FindConfigDataByTag(CDATA_TCC_TAG);
  if ((TccCfgData == NULL) || (TccCfgData->TccEnable == 0)) {
    return;
  }

  DEBUG ((DEBUG_INFO, "TccModePreMemConfig () - Start\n"));

  // TCC related memory settings
  DEBUG ((DEBUG_INFO, "Tcc is enabled, Setting memory config.\n"));

  FspmUpd->FspmConfig.PcdPchDmiAspm                 = 0;
  FspmUpd->FspmConfig.PcdPchLegacyIoLowLatency      = 0;

  FspmUpd->FspmConfig.PcdProcessorEistEnable        = 0;
  FspmUpd->FspmConfig.PcdProcessorHWPMEnable        = 0;
  FspmUpd->FspmConfig.PcdPackageCState              = 0;
  FspmUpd->FspmConfig.PcdMonitorMWait               = 0;

  FspmUpd->FspmConfig.PcdPowerLimit1Enable          = 0;
  FspmUpd->FspmConfig.PcdPowerLimit2Enable          = 0;
  FspmUpd->FspmConfig.PcdTurboMode                  = 0;
  FspmUpd->FspmConfig.PcdDramRaplEnable             = 0;
  FspmUpd->FspmConfig.PcdCkeProgramming             = 1;
  FspmUpd->FspmConfig.PcdApdEnable                  = 0;
  FspmUpd->FspmConfig.PcdPpdEnable                  = 0;

  FspmUpd->FspmConfig.PcdPcieGlobalAspm             = 0;
  FspmUpd->FspmConfig.PcdHyperThreading             = 0;


  FspmUpd->FspmConfig.PcdTccEnable                  = TccCfgData->TccEnable;
  FspmUpd->FspmConfig.PcdTccSoftwareSramEn          = TccCfgData->TccSoftSram;
  FspmUpd->FspmConfig.PcdTccDsoTuningEn             = TccCfgData->TccTuning;
  FspmUpd->FspmConfig.PcdTccErrorLogEn              = TccCfgData->TccErrorLog;

  if (IsMarkedBadDso ()) {
    DEBUG ((DEBUG_INFO, "From previous TCC tuning issues, platform rebooted with default values.\n"));
  } else if (IsWdtFlagsSet (WDT_FLAG_TCC_DSO_IN_PROGRESS) && IsWdtTimeout ()) {
    DEBUG ((DEBUG_INFO, "From previous TCC tuning issues, platform rebooted with default values.\n"));
    WdtClearScratchpad (WDT_FLAG_TCC_DSO_IN_PROGRESS);
    InvalidateBadDso ();
  } else if (TccCfgData->TccTuning != 0) {
    // Setup Watch dog timer
    WdtReloadAndStart (WDT_TIMEOUT_TCC_DSO, WDT_FLAG_TCC_DSO_IN_PROGRESS);

    // Load TCC stream config from container
    TccStreamBase = NULL;
    TccStreamSize = 0;
    Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'T'), (VOID **)&TccStreamBase, &TccStreamSize);

    if (EFI_ERROR (Status) || (TccStreamSize < sizeof(TCC_STREAM_CONFIGURATION))) {
      DEBUG ((DEBUG_INFO, "Load TCC Stream %r, size = 0x%x\n", Status, TccStreamSize));
    } else {
      FspmUpd->FspmConfig.PcdTccStreamCfgBasePreMem   = (UINT32)(UINTN)TccStreamBase;
      FspmUpd->FspmConfig.PcdTccStreamCfgSizePreMem   = TccStreamSize;
      DEBUG ((DEBUG_INFO, "Load TCC stream @0x%p, size = 0x%x\n", TccStreamBase, TccStreamSize));

      if (FspmUpd->FspmConfig.PcdTccDsoTuningEn != 0) {
        StreamConfig = (TCC_STREAM_CONFIGURATION *) TccStreamBase;
        PolicyConfig = (BIOS_SETTINGS *) &StreamConfig->BiosSettings;
        FspmUpd->FspmConfig.PcdPchDmiAspm             = PolicyConfig->DmiAspm;
        FspmUpd->FspmConfig.PcdPchLegacyIoLowLatency  = PolicyConfig->PchPwrClkGate;

        FspmUpd->FspmConfig.PcdProcessorEistEnable    = PolicyConfig->Pstates;
        FspmUpd->FspmConfig.PcdProcessorHWPMEnable    = PolicyConfig->HwpEn;
        if (!PolicyConfig->Cstates) {
          FspmUpd->FspmConfig.PcdPackageCState       = 0;
          FspmUpd->FspmConfig.PcdMonitorMWait        = 0;
        }

        FspmUpd->FspmConfig.PcdPowerLimit1Enable      = PolicyConfig->CoreRapl;
        FspmUpd->FspmConfig.PcdPowerLimit2Enable      = PolicyConfig->CoreRapl;
        FspmUpd->FspmConfig.PcdTurboMode              = PolicyConfig->Turbo;
        FspmUpd->FspmConfig.PcdDramRaplEnable         = PolicyConfig->MemoryRapl;
        if (PolicyConfig->MemPowerDown) {
          FspmUpd->FspmConfig.PcdCkeProgramming       = 1; // Manual CKE
          FspmUpd->FspmConfig.PcdApdEnable            = 0; // Disable APD
          FspmUpd->FspmConfig.PcdPpdEnable            = 0; // Disable PPD
        }

        FspmUpd->FspmConfig.PcdPcieGlobalAspm         = PolicyConfig->CpuPcieAspm;
        FspmUpd->FspmConfig.PcdHyperThreading         = PolicyConfig->HyperThreading;

        DEBUG ((DEBUG_INFO, "Dump TCC DSO BIOS settings:\n"));
        DumpHex (2, 0, sizeof(BIOS_SETTINGS), PolicyConfig);
      }
    }
  }

  DEBUG ((DEBUG_INFO, "TccModePreMemConfig () - End\n"));
}

/**
  Checks if Me FW is SPS Ignition type

  @retval FALSE           No SPS Ignition on board
  @retval TRUE            SPS Ignition detected
**/
BOOLEAN
MeTypeIsSpsIgnition (
  VOID
  )
{
  HECI_FWS_REGISTER     MeFirmwareStatus;
  UINT8                 MeOpMode;

  MeFirmwareStatus.ul = HeciPciRead32 (R_ME_HFS);
  if (MeFirmwareStatus.ul == 0xFFFFFFFF) {
    DEBUG ((DEBUG_ERROR, "ERROR: HECI interface is disabled\n"));
    return TRUE;
  }

  MeOpMode = (UINT8) MeFirmwareStatus.r.MeOperationMode;
  DEBUG ((DEBUG_INFO, "[SPS] OpMode=0x%x, ErrCode=0x%x\n", MeFirmwareStatus.r.MeOperationMode, MeFirmwareStatus.r.ErrorCode));

  if (MeOpMode == ME_OPERATION_MODE_SPS_IGNITION) {
    return TRUE;
  }

  return FALSE;
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
  FSPM_UPD                   *FspmUpd;
  MEMORY_CFG_DATA            *MemCfgData;
  BOOT_LOADER_VERSION        *VerInfo;
  BOOLEAN                     FspDebug;
  PLATFORM_DATA              *PlatformData;
  HSIO_CFG_DATA              *HsioCfgData;
  VMD_CFG_DATA               *VmdCfgData;
  FEATURES_CFG_DATA          *FeaturesCfgData;
  UINT8                      *SpdPtr;
  UINT8                      *SpdBytes;
  EFI_STATUS                  Status;
  UINT8                       Channel;
  UINT8                       Dimm;
  UINT8                       SpdIndex;
  BL_SYSTEM_PCI_BASE_LIMITS  *PciIioRes;
  UINT8                       SocId;
  UINT8                       StackId;
  MISC_CFG_DATA              *MiscCfgData;
  PCIE_CFG_DATA              *PcieCfgData;
  PWR_CFG_DATA               *PwrMgmtCfgData;
  UINT32                      Index;
  UINT32                      CarBase;
  UINT32                      CarSize;
  TYPE_CHOPTYPE_ENUM          ChopType;
  UINT32                      SpdData[2];
  UINT8                       SelIdx;
  MEM_SPD_CFG_DATA           *SpdCfgData;
  UINT8                      *SpdSelPtr;
  CLOCK_CFG_DATA             *ClockCfgData;
  UINT8                       PortIndex;

  FspmUpd = (FSPM_UPD *)FspmUpdPtr;

  //
  // CAR   : 0xFE800000 - 0xFE880000 (0x80000 from FSP-T)
  // RcHeap: 0xFE800000 - 0xFE840000 (0x40000 STAGE1_STACK_BASE_OFFSET)
  // SBL   : 0xFE840000 - 0xFE864000 (0x24000 SBL Data, SBL Stack shared with FSP Stack)
  // FSP   : 0xFE864000 - 0xFE880000 (0x1C000 FSP Heap)
  //
  Status = GetTempRamInfo (&CarBase, &CarSize);
  ChopType = GetChopType (0);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "CAR Base 0x%X (0x%X)\n", CarBase, CarSize));
  FspmUpd->FspmArchUpd.StackBase = CarBase \
                                 + FixedPcdGet32 (PcdStage1StackBaseOffset) \
                                 + FixedPcdGet32 (PcdStage1StackSize) \
                                 + FixedPcdGet32 (PcdStage1DataSize);
  FspmUpd->FspmArchUpd.StackSize = CarBase + CarSize - FspmUpd->FspmArchUpd.StackBase;

  // Memory Settings
  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
  if (MemCfgData == NULL) {
    DEBUG ((DEBUG_WARN, "No memory CFGDATA. Continue to boot with default values!\n"));
  } else {
    if (!PcdGetBool (PcdFastBootEnabled)) {
      FspmUpd->FspmConfig.PcdEccSupport             = (UINT8)MemCfgData->GeneralConfig.EccSupport;
      FspmUpd->FspmConfig.PcdMemTest                = (UINT8)MemCfgData->GeneralConfig.MemTest;
      FspmUpd->FspmConfig.PcdEnableBiosSsaRMT       = (UINT8)MemCfgData->PcdEnableBiosSsaRMT;
      FspmUpd->FspmConfig.PcdEnableBiosSsaRMTonFCB  = (UINT8)MemCfgData->PcdEnableBiosSsaRMTonFCB;
    } else {
      FspmUpd->FspmConfig.PcdEccSupport             = 0;
      FspmUpd->FspmConfig.PcdMemTest                = 0;
      FspmUpd->FspmConfig.PcdEnableBiosSsaRMT       = 0;
      FspmUpd->FspmConfig.PcdEnableBiosSsaRMTonFCB  = 0;
    }
    FspmUpd->FspmConfig.PcdFastBoot                 = (UINT8)MemCfgData->GeneralConfig.FastBoot;
    FspmUpd->FspmConfig.PcdMemoryThermalThrottling  = (UINT8)MemCfgData->GeneralConfig.MemoryThermalThrottling;


    FspmUpd->FspmConfig.PcdEnableIMR3               = (UINT8)MemCfgData->GeneralConfig.EnableIMR3;
    FspmUpd->FspmConfig.PcdProcessorX2Apic          = 0; // Apic for single socket

    // Initialize TME (Total Memory Encryption)
    FspmUpd->FspmConfig.PcdTmeEnable                = (UINT8)MemCfgData->GeneralConfig.TmeEnable;

    // Initialize MKTME (Multi-Key Total Memory Encryption)
    // only if TME is enabled
    if (FspmUpd->FspmConfig.PcdTmeEnable == 1) {
      FspmUpd->FspmConfig.PcdMkTmeEnable            = (UINT8)MemCfgData->GeneralConfig.MkTmeEnable;
    } else {
      FspmUpd->FspmConfig.PcdMkTmeEnable            = 0;
    }

    FspmUpd->FspmConfig.PcdDfxWarmResetEliminationEn =  (UINT8)MemCfgData->GeneralConfig.EliminateWarmReset;

    FspmUpd->FspmConfig.PcdDdrFreq                  = (UINT8)MemCfgData->ParameterConfig.DdrFreq;
    FspmUpd->FspmConfig.PcdCommandTiming            = (UINT8)MemCfgData->ParameterConfig.CommandTiming;
    FspmUpd->FspmConfig.PcdCustomRefreshRate        = (UINT8)MemCfgData->ParameterConfig.CustomRefreshRate;

    // Memory SPD Data
    if (MemCfgData->GeneralConfig.MemoryDown) {
      SpdPtr = (UINT8 *) AllocateZeroPool(MAX_IMC * MAX_CHANNEL * MAX_DIMM * MAX_SPD_BYTES);
      if (MemCfgData->GeneralConfig.HwSpdRead == 0) {
        //
        // Read SPD from Config Data
        //
        SpdCfgData = (MEM_SPD_CFG_DATA *) FindConfigDataByTag (CDATA_MEM_SPD_TAG);
        if (SpdCfgData != NULL) {
          SpdData[0] = 0; // NULL
          SpdData[1] = (UINT32)(UINTN) (SpdCfgData->SpdDataPtr1); // SpdData_Lcc_Rdimm.bin
          SpdSelPtr  = (UINT8 *) (UINTN) (&(SpdCfgData->SpdDataSel000));
          for (Dimm  = 0; Dimm < MAX_IMC * MAX_CHANNEL * MAX_DIMM; Dimm++) {
            SelIdx   = SpdSelPtr[0];
            if (SelIdx != 0 ) { // non-NULL SPD data
              CopyMem (SpdPtr + (Dimm * MAX_SPD_BYTES), (VOID *) (UINTN) (SpdData[SelIdx]), MAX_SPD_BYTES);
            }
            SpdSelPtr++;
          }
        }
        FspmUpd->FspmConfig.PcdMemSpdPtr = (UINT32)(UINTN) SpdPtr;
      } else {
        //
        // Read SPD from HW
        //
        if (SpdPtr != NULL) {
          for (Channel = 0; Channel < (MAX_IMC * MAX_CHANNEL); Channel++) {
            for (Dimm  = 0; Dimm < MAX_DIMM; Dimm++) {
              SpdIndex = (Channel * MAX_DIMM) + Dimm;
              SpdBytes = SpdPtr + (SpdIndex * MAX_SPD_BYTES);
              DEBUG ((DEBUG_INFO, "MC[%d]CH[%d]DI[%d] SPD\n", Channel/MAX_CHANNEL, Channel%MAX_CHANNEL, Dimm));
              Status = SpdGetFromSmb (Channel, Dimm, MAX_SPD_BYTES, SpdBytes);
              if (EFI_ERROR(Status)) {
                DEBUG ((DEBUG_WARN, "SPD read from HW error = %r\n", Status));
              } else {
                DumpHex (2, 0, MAX_SPD_BYTES, (VOID *)SpdBytes);
              }
            }
          }
          FspmUpd->FspmConfig.PcdMemSpdPtr = (UINT32)(UINTN) SpdPtr;
        }
      }
    }

  }

  PlatformData    = (PLATFORM_DATA *) GetPlatformDataPtr ();
  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
  if ((PlatformData != NULL) && (FeaturesCfgData != NULL)) {
    PlatformData->PlatformFeatures.VtdEnable        = (UINT8) FeaturesCfgData->Features.VtdSupport & FeaturePcdGet (PcdVtdEnabled);
    FspmUpd->FspmConfig.PcdVtdSupport               = (UINT8) PlatformData->PlatformFeatures.VtdEnable;
  } else {
    DEBUG ((DEBUG_INFO, "No PlatformData/FeaturesCfgData, continue with default platform Vtd feature\n"));
    FspmUpd->FspmConfig.PcdVtdSupport               = 0;
  }

  // HSIO settings
  HsioCfgData = (HSIO_CFG_DATA *)FindConfigDataByTag (CDATA_HSIO_TAG);
  if (HsioCfgData == NULL) {
    DEBUG ((DEBUG_WARN, "No HSIO CFGDATA. Continue to boot with default values!\n"));
  } else {
    FspmUpd->FspmConfig.PcdFiaMuxOverride           = HsioCfgData->PcdFiaMuxOverride;
    FspmUpd->FspmConfig.FiaMuxCfgInvalidate         = HsioCfgData->FiaMuxCfgInvalidate;
    FspmUpd->FspmConfig.PcdFiaLaneConfigPtr         = (UINT32)(UINTN)&HsioCfgData->FiaLaneConfig[0];
  }

  // Misc Settings
  MiscCfgData = (MISC_CFG_DATA *) FindConfigDataByTag (CDATA_MISC_TAG);
  if (MiscCfgData == NULL) {
    DEBUG ((DEBUG_WARN, "No MISC CFGDATA. Continue to boot with default values!\n"));
  } else {
    FspmUpd->FspmConfig.PcdHsuartDevice             = (UINT8)MiscCfgData->MiscCfg.HsuartDevice;
    FspmUpd->FspmConfig.PcdHeciCommunication        = (UINT8)(MeTypeIsSpsIgnition() ? 0 : MiscCfgData->MiscCfg.HeciCommunication);
    FspmUpd->FspmConfig.PcdHyperThreading               = (UINT8)MiscCfgData->MiscCfg.HyperThreading;
    FspmUpd->FspmConfig.PcdAcExceptionOnSplitLockEnable = (UINT8)MiscCfgData->MiscCfg.AcExceptionOnSplitLockEnable;

    FspDebug = FALSE;
    if (!PcdGetBool (PcdFastBootEnabled)) {
      VerInfo = (BOOT_LOADER_VERSION *)GetVerInfoPtr ();
      if ((VerInfo != NULL) && (VerInfo->ImageVersion.FspDebug != 0) && DebugCodeEnabled ()) {
        FspDebug = TRUE;
      }
    }
    if (FspDebug) {
      FspmUpd->FspmConfig.PcdFspDebugPrintErrorLevel  = (UINT8)MiscCfgData->MiscCfg.FspDebugPrintErrorLevel;
    } else {
      FspmUpd->FspmConfig.PcdFspDebugPrintErrorLevel  = 0;
    }
  }

  // Pcie Settings
  PcieCfgData = (PCIE_CFG_DATA *) FindConfigDataByTag (CDATA_PCIE_TAG);
  if (PcieCfgData == NULL) {

  } else {
    // IIO PCIE
    FspmUpd->FspmConfig.PcdIIOPciePortBifurcation    = PcieCfgData->IIOPciePortBifurcation;
    if (ChopType == TypeHcc) {
      FspmUpd->FspmConfig.PcdIIOPciePort2Bifurcation = PcieCfgData->IIOPciePort2Bifurcation;
    } else {
      FspmUpd->FspmConfig.PcdIIOPciePort2Bifurcation = 0;
    }
    FspmUpd->FspmConfig.PcdPcieHotPlugEnable        = PcieCfgData->IIoPcieHotPlugEnable;
    if (FspmUpd->FspmConfig.PcdPcieHotPlugEnable != 0) {
      FspmUpd->FspmConfig.PcdIIoPciePort1AHPCapable  = PcieCfgData->IIoPcieHotPlugOnPort[0];
      FspmUpd->FspmConfig.PcdIIoPciePort1BHPCapable  = PcieCfgData->IIoPcieHotPlugOnPort[1];
      FspmUpd->FspmConfig.PcdIIoPciePort1CHPCapable  = PcieCfgData->IIoPcieHotPlugOnPort[2];
      FspmUpd->FspmConfig.PcdIIoPciePort1DHPCapable  = PcieCfgData->IIoPcieHotPlugOnPort[3];
      FspmUpd->FspmConfig.PcdIIoPciePort1AHPSurprise = PcieCfgData->IIoPcieSurpriseHotPlugOnPort[0];
      FspmUpd->FspmConfig.PcdIIoPciePort1BHPSurprise = PcieCfgData->IIoPcieSurpriseHotPlugOnPort[1];
      FspmUpd->FspmConfig.PcdIIoPciePort1CHPSurprise = PcieCfgData->IIoPcieSurpriseHotPlugOnPort[2];
      FspmUpd->FspmConfig.PcdIIoPciePort1DHPSurprise = PcieCfgData->IIoPcieSurpriseHotPlugOnPort[3];
      if (ChopType == TypeHcc) {
        FspmUpd->FspmConfig.PcdIIoPciePort2AHPCapable  = PcieCfgData->IIoPciePort2HotPlugOnPort[0];
        FspmUpd->FspmConfig.PcdIIoPciePort2BHPCapable  = PcieCfgData->IIoPciePort2HotPlugOnPort[1];
        FspmUpd->FspmConfig.PcdIIoPciePort2CHPCapable  = PcieCfgData->IIoPciePort2HotPlugOnPort[2];
        FspmUpd->FspmConfig.PcdIIoPciePort2DHPCapable  = PcieCfgData->IIoPciePort2HotPlugOnPort[3];
        FspmUpd->FspmConfig.PcdIIoPciePort2AHPSurprise = PcieCfgData->IIoPciePort2SurpriseHotPlugOnPort[0];
        FspmUpd->FspmConfig.PcdIIoPciePort2BHPSurprise = PcieCfgData->IIoPciePort2SurpriseHotPlugOnPort[1];
        FspmUpd->FspmConfig.PcdIIoPciePort2CHPSurprise = PcieCfgData->IIoPciePort2SurpriseHotPlugOnPort[2];
        FspmUpd->FspmConfig.PcdIIoPciePort2DHPSurprise = PcieCfgData->IIoPciePort2SurpriseHotPlugOnPort[3];
      } else {
        FspmUpd->FspmConfig.PcdIIoPciePort2AHPCapable  = 0;
        FspmUpd->FspmConfig.PcdIIoPciePort2BHPCapable  = 0;
        FspmUpd->FspmConfig.PcdIIoPciePort2CHPCapable  = 0;
        FspmUpd->FspmConfig.PcdIIoPciePort2DHPCapable  = 0;
        FspmUpd->FspmConfig.PcdIIoPciePort2AHPSurprise = 0;
        FspmUpd->FspmConfig.PcdIIoPciePort2BHPSurprise = 0;
        FspmUpd->FspmConfig.PcdIIoPciePort2CHPSurprise = 0;
        FspmUpd->FspmConfig.PcdIIoPciePort2DHPSurprise = 0;
      }
    } else {
      FspmUpd->FspmConfig.PcdIIoPciePort1AHPCapable  = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort1BHPCapable  = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort1CHPCapable  = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort1DHPCapable  = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort1AHPSurprise = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort1BHPSurprise = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort1CHPSurprise = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort1DHPSurprise = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2AHPCapable  = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2BHPCapable  = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2CHPCapable  = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2DHPCapable  = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2AHPSurprise = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2BHPSurprise = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2CHPSurprise = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2DHPSurprise = 0;
    }
    FspmUpd->FspmConfig.PcdIIoPcieRLinkDeEmphasis  = PcieCfgData->IIoPcieDeEmphasis[0];
    FspmUpd->FspmConfig.PcdIIoPciePort1ADeEmphasis = PcieCfgData->IIoPcieDeEmphasis[1];
    FspmUpd->FspmConfig.PcdIIoPciePort1BDeEmphasis = PcieCfgData->IIoPcieDeEmphasis[2];
    FspmUpd->FspmConfig.PcdIIoPciePort1CDeEmphasis = PcieCfgData->IIoPcieDeEmphasis[3];
    FspmUpd->FspmConfig.PcdIIoPciePort1DDeEmphasis = PcieCfgData->IIoPcieDeEmphasis[4];
    FspmUpd->FspmConfig.PcdIIoPcieLinkSpeedRLink   = PcieCfgData->IIoPcieLinkSpeed[0];
    FspmUpd->FspmConfig.PcdIIoPciePort1ALinkSpeed  = PcieCfgData->IIoPcieLinkSpeed[1];
    FspmUpd->FspmConfig.PcdIIoPciePort1BLinkSpeed  = PcieCfgData->IIoPcieLinkSpeed[2];
    FspmUpd->FspmConfig.PcdIIoPciePort1CLinkSpeed  = PcieCfgData->IIoPcieLinkSpeed[3];
    FspmUpd->FspmConfig.PcdIIoPciePort1DLinkSpeed  = PcieCfgData->IIoPcieLinkSpeed[4];
    FspmUpd->FspmConfig.PcdIIoPcieRLinkAspm        = PcieCfgData->IIoPcieAspm[0];
    FspmUpd->FspmConfig.PcdIIoPciePort1AAspm       = PcieCfgData->IIoPcieAspm[1];
    FspmUpd->FspmConfig.PcdIIoPciePort1BAspm       = PcieCfgData->IIoPcieAspm[2];
    FspmUpd->FspmConfig.PcdIIoPciePort1CAspm       = PcieCfgData->IIoPcieAspm[3];
    FspmUpd->FspmConfig.PcdIIoPciePort1DAspm       = PcieCfgData->IIoPcieAspm[4];
    if (ChopType == TypeHcc) {
      FspmUpd->FspmConfig.PcdIIoPciePort2ADeEmphasis = PcieCfgData->IIoPciePort2DeEmphasis[0];
      FspmUpd->FspmConfig.PcdIIoPciePort2BDeEmphasis = PcieCfgData->IIoPciePort2DeEmphasis[1];
      FspmUpd->FspmConfig.PcdIIoPciePort2CDeEmphasis = PcieCfgData->IIoPciePort2DeEmphasis[2];
      FspmUpd->FspmConfig.PcdIIoPciePort2DDeEmphasis = PcieCfgData->IIoPciePort2DeEmphasis[3];
      FspmUpd->FspmConfig.PcdIIoPciePort2ALinkSpeed  = PcieCfgData->IIoPciePort2LinkSpeed[0];
      FspmUpd->FspmConfig.PcdIIoPciePort2BLinkSpeed  = PcieCfgData->IIoPciePort2LinkSpeed[1];
      FspmUpd->FspmConfig.PcdIIoPciePort2CLinkSpeed  = PcieCfgData->IIoPciePort2LinkSpeed[2];
      FspmUpd->FspmConfig.PcdIIoPciePort2DLinkSpeed  = PcieCfgData->IIoPciePort2LinkSpeed[3];
      FspmUpd->FspmConfig.PcdIIoPciePort2AAspm       = PcieCfgData->IIoPciePort2Aspm[0];
      FspmUpd->FspmConfig.PcdIIoPciePort2BAspm       = PcieCfgData->IIoPciePort2Aspm[1];
      FspmUpd->FspmConfig.PcdIIoPciePort2CAspm       = PcieCfgData->IIoPciePort2Aspm[2];
      FspmUpd->FspmConfig.PcdIIoPciePort2DAspm       = PcieCfgData->IIoPciePort2Aspm[3];
    } else {
      FspmUpd->FspmConfig.PcdIIoPciePort2ADeEmphasis = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2BDeEmphasis = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2CDeEmphasis = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2DDeEmphasis = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2ALinkSpeed  = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2BLinkSpeed  = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2CLinkSpeed  = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2DLinkSpeed  = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2AAspm       = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2BAspm       = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2CAspm       = 0;
      FspmUpd->FspmConfig.PcdIIoPciePort2DAspm       = 0;
    }
    // PCH PCIE Bifurcation
    FspmUpd->FspmConfig.PcdBifurcationPcie0         = PcieCfgData->BifurcationPcie0;
    FspmUpd->FspmConfig.PcdBifurcationPcie1         = PcieCfgData->BifurcationPcie1;
    FspmUpd->FspmConfig.PcdBifurcationPcie2         = PcieCfgData->BifurcationPcie2;

    for (PortIndex = 0; PortIndex < BL_NUMBER_PORTS_PER_SOCKET; PortIndex++) {
      FspmUpd->FspmConfig.PcdPciePortClkGateEnable[PortIndex] = PcieCfgData->PciePortClkGateEnable[PortIndex];
    }

    FspmUpd->FspmConfig.PcdPchDmiAspm               = PcieCfgData->PchDmiAspm;

    //
    // PCIE Root Port Enable
    //
    FspmUpd->FspmConfig.PcdPcieRootPortEn = PcieCfgData->PchRpConfig.PcieRootPortEn;

    // Pci IIO Resource allocation
    if (PcieCfgData->IioRes.Enable) {
      PciIioRes = (BL_SYSTEM_PCI_BASE_LIMITS *) AllocateZeroPool (sizeof(BL_SYSTEM_PCI_BASE_LIMITS));
      if (PciIioRes != NULL) {
        for (SocId = 0; SocId < BL_MAX_SOCKET; SocId++) {
          for (StackId = 0; StackId < BL_MAX_LOGIC_IIO_STACK; StackId++) {
            PciIioRes->Socket[SocId].StackLimits[StackId].IoBase        = PcieCfgData->IioRes.IoBase[StackId];
            PciIioRes->Socket[SocId].StackLimits[StackId].IoLimit       = PcieCfgData->IioRes.IoLimit[StackId];
            PciIioRes->Socket[SocId].StackLimits[StackId].LowMmioBase   = PcieCfgData->IioRes.LowMmioBase[StackId];
            PciIioRes->Socket[SocId].StackLimits[StackId].LowMmioLimit  = PcieCfgData->IioRes.LowMmioLimit[StackId];
            PciIioRes->Socket[SocId].StackLimits[StackId].HighMmioBase  = PcieCfgData->IioRes.HighMmioBase[StackId];
            PciIioRes->Socket[SocId].StackLimits[StackId].HighMmioLimit = PcieCfgData->IioRes.HighMmioLimit[StackId];
          }
        }
      }
      FspmUpd->FspmConfig.PcdIioResConfigPtr = (UINT32) (UINTN) PciIioRes;
    } else {
      FspmUpd->FspmConfig.PcdIioResConfigPtr = 0;
    }
  }

  // Pwr mgmt Settings
  PwrMgmtCfgData = (PWR_CFG_DATA *) FindConfigDataByTag (CDATA_PWR_TAG);
  if (PwrMgmtCfgData == NULL) {

  } else {
    // P-State
    FspmUpd->FspmConfig.PcdProcessorEistEnable          = (UINT8)PwrMgmtCfgData->PState.EistEnable;
    FspmUpd->FspmConfig.PcdBootPState                   = (UINT8)PwrMgmtCfgData->PState.BootPState;
    FspmUpd->FspmConfig.PcdProcessorHWPMEnable          = (UINT8)PwrMgmtCfgData->PState.HWPMEnable;
    FspmUpd->FspmConfig.PcdProcessorHWPMInterrupt       = (UINT8)PwrMgmtCfgData->PState.HWPMInterrupt;
    FspmUpd->FspmConfig.PcdProcessorEPPEnable           = (UINT8)PwrMgmtCfgData->PState.EPPEnable;
    FspmUpd->FspmConfig.PcdProcessorEppProfile          = (UINT8)PwrMgmtCfgData->PState.EppProfile;
    FspmUpd->FspmConfig.PcdConfigTdpLock                = (UINT8)PwrMgmtCfgData->PState.ConfigTdpLock;
    FspmUpd->FspmConfig.PcdConfigTdpLevel               = (UINT8)PwrMgmtCfgData->PState.ConfigTdpLevel;
    FspmUpd->FspmConfig.PcdAvxSupport                   = (UINT8)PwrMgmtCfgData->PState.AvxSupport;
    FspmUpd->FspmConfig.PcdAvxLicensePreGrant           = (UINT8)PwrMgmtCfgData->PState.AvxLicensePreGrant;
    FspmUpd->FspmConfig.PcdAvxIccpLevel                 = (UINT8)PwrMgmtCfgData->PState.AvxIccpLevel;
    FspmUpd->FspmConfig.PcdGpssTimer                    = (UINT8)PwrMgmtCfgData->PState.GpssTimer;
    FspmUpd->FspmConfig.PcdTurboMode                    = (UINT8)PwrMgmtCfgData->PState.TurboMode;
    FspmUpd->FspmConfig.PcdPwrPerfTuning                = (UINT8)PwrMgmtCfgData->PState.PwrPerfTuning;
    FspmUpd->FspmConfig.PcdAltEngPerfBIAS               = (UINT8)PwrMgmtCfgData->PState.AltEngPerfBIAS;
    // WOL
    if (PwrMgmtCfgData->Miscellaneous.WakeOnLanSupport) {
      IoAndThenOr32 (ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_EN_127_96, (UINT32)~B_ACPI_IO_GPE0_EN_127_96_PME, B_ACPI_IO_GPE0_EN_127_96_PME);
      IoAndThenOr32 (ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_EN_127_96, (UINT32)~B_ACPI_IO_GPE0_EN_127_96_PCI_EXP, B_ACPI_IO_GPE0_EN_127_96_PCI_EXP);
      IoAndThenOr32 (ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_EN_127_96, (UINT32)~B_ACPI_IO_GPE0_EN_127_96_PME_B0, B_ACPI_IO_GPE0_EN_127_96_PME_B0);
    }
    // C-State
    FspmUpd->FspmConfig.PcdPackageCState                = (UINT8)PwrMgmtCfgData->CState.PackageCState;
    FspmUpd->FspmConfig.PcdProcessorC1eEnable           = (UINT8)PwrMgmtCfgData->CState.C1eEnable;
    FspmUpd->FspmConfig.PcdC2C3TT                       = (UINT8)PwrMgmtCfgData->CState.C2C3TT;
    FspmUpd->FspmConfig.PcdC3Enable                     = (UINT8)PwrMgmtCfgData->CState.C3Enable;
    FspmUpd->FspmConfig.PcdC6Enable                     = (UINT8)PwrMgmtCfgData->CState.C6Enable;
    FspmUpd->FspmConfig.PcdMonitorMWait                 = (UINT8)PwrMgmtCfgData->CState.MonitorMWait;
    FspmUpd->FspmConfig.PcdCStateLatencyCtrlValid0      = (UINT8)PwrMgmtCfgData->CState.CStateLatencyCtrlValid0;
    FspmUpd->FspmConfig.PcdCStateLatencyCtrlMultiplier0 = (UINT8)PwrMgmtCfgData->CState.CStateLatencyCtrlMultiplier0;
    FspmUpd->FspmConfig.PcdCStateLatencyCtrlValue0      = (UINT16)PwrMgmtCfgData->CState.CStateLatencyCtrlValue0;
    FspmUpd->FspmConfig.PcdCStateLatencyCtrlValid1      = (UINT8)PwrMgmtCfgData->CState.CStateLatencyCtrlValid1;
    FspmUpd->FspmConfig.PcdCStateLatencyCtrlMultiplier1 = (UINT8)PwrMgmtCfgData->CState.CStateLatencyCtrlMultiplier1;
    FspmUpd->FspmConfig.PcdCStateLatencyCtrlValue1      = (UINT16)PwrMgmtCfgData->CState.CStateLatencyCtrlValue1;
    FspmUpd->FspmConfig.PcdCStateLatencyCtrlValid2      = (UINT8)PwrMgmtCfgData->CState.CStateLatencyCtrlValid2;
    FspmUpd->FspmConfig.PcdCStateLatencyCtrlMultiplier2 = (UINT8)PwrMgmtCfgData->CState.CStateLatencyCtrlMultiplier2;
    FspmUpd->FspmConfig.PcdCStateLatencyCtrlValue2      = (UINT16)PwrMgmtCfgData->CState.CStateLatencyCtrlValue2;
    // T-State
    FspmUpd->FspmConfig.PcdTStateEnable                 = (UINT8)PwrMgmtCfgData->TState.TStateEnable;
    FspmUpd->FspmConfig.PcdEnableProcHot                = (UINT8)PwrMgmtCfgData->TState.EnableProcHot;
    FspmUpd->FspmConfig.PcdEnableThermalMonitor         = (UINT8)PwrMgmtCfgData->TState.EnableThermalMonitor;
  }

  // Tcc enabling
  if ((FeaturePcdGet (PcdTccEnabled)) && (GetBootMode() != BOOT_ON_FLASH_UPDATE)) {
    TccModePreMemConfig (FspmUpd);
  }

  // DCI enabling
  if (FeaturesCfgData != NULL) {
    FspmUpd->FspmConfig.PcdDciEn                    = (UINT8) FeaturesCfgData->Features.DCI;
  }

  // VMD
  VmdCfgData = (VMD_CFG_DATA *)FindConfigDataByTag (CDATA_VMD_TAG);
  if (VmdCfgData == NULL) {
    DEBUG ((DEBUG_WARN, "No VMD CFGDATA. Continue to boot with default values!\n"));
  } else {
    for (Index = 0; Index < 12; Index++) {
      FspmUpd->FspmConfig.PcdVMDPchPortEnable[Index]     = VmdCfgData->VMDPchPortEnable[Index];
    }

    for (Index = 0; Index < 2; Index++) {
      FspmUpd->FspmConfig.PcdVMDEnabled[Index]           = VmdCfgData->VMDEnabled[Index];
      FspmUpd->FspmConfig.PcdVMDHotPlugEnable[Index]     = VmdCfgData->VMDHotPlugEnable[Index];
      FspmUpd->FspmConfig.PcdVMDCfgBarSz[Index]          = VmdCfgData->VMDCfgBarSz[Index];
      FspmUpd->FspmConfig.PcdVMDCfgBarAttr[Index]        = VmdCfgData->VMDCfgBarAttr[Index];
      FspmUpd->FspmConfig.PcdVMDMemBarSz1[Index]         = VmdCfgData->VMDMemBarSz1[Index];
      FspmUpd->FspmConfig.PcdVMDMemBar1Attr[Index]       = VmdCfgData->VMDMemBar1Attr[Index];
      FspmUpd->FspmConfig.PcdVMDMemBarSz2[Index]         = VmdCfgData->VMDMemBarSz2[Index];
      FspmUpd->FspmConfig.PcdVMDMemBar2Attr[Index]       = VmdCfgData->VMDMemBar2Attr[Index];
      FspmUpd->FspmConfig.PcdVMDDirectAssign[Index]      = VmdCfgData->VMDDirectAssign[Index];
    }
    FspmUpd->FspmConfig.PcdVMDPortEnableA                = VmdCfgData->VMDPortEnableA;
    FspmUpd->FspmConfig.PcdVMDPortEnableB                = VmdCfgData->VMDPortEnableB;
    FspmUpd->FspmConfig.PcdVMDPortEnableC                = VmdCfgData->VMDPortEnableC;
    FspmUpd->FspmConfig.PcdVMDPortEnableD                = VmdCfgData->VMDPortEnableD;
  }

  if (!UpdateFspmSgxConfig (FspmUpd)) {
    DEBUG ((DEBUG_INFO, "FSP-M variables for Intel(R) SGX were NOT updated.\n"));
  }

  // CLOCK settings
  ClockCfgData = (CLOCK_CFG_DATA *)FindConfigDataByTag (CDATA_CLOCK_TAG);
  if (ClockCfgData == NULL) {
    DEBUG ((DEBUG_WARN, "No CLOCK CFGDATA. Continue to boot with default values!\n"));
  } else {
    FspmUpd->FspmConfig.PcdSkuClockGeneratorAddress      = ClockCfgData->ClockGeneratorAddress;
    FspmUpd->FspmConfig.PcdSkuSSCSecondarySmbusUsed      = ClockCfgData->SSCSecondarySmbusUsed;
    FspmUpd->FspmConfig.PcdSkipClockGenerator            = ClockCfgData->SkipClockGenerator;
    FspmUpd->FspmConfig.PcdEnableClockSpreadSpec         = ClockCfgData->EnableClockSpreadSpec;
  }

  // To avoid unnecessary reboots caused by overriding HyperThreading
  if (GetBootMode() == BOOT_ON_FLASH_UPDATE) {
    FspmUpd->FspmConfig.PcdHyperThreading                = (UINT8)GetDesiredSmtEnable ();
  }

  // To make consistence between Stage1 and Stage2
  if (PlatformData != NULL) {
    PlatformData->PlatformFeatures.HyperThreading         = FspmUpd->FspmConfig.PcdHyperThreading;
  }
  DEBUG ((DEBUG_INFO, "Update FspmUpd with config data...Done\n"));
}

/**
  Detect board and configure PlatformID.

**/
VOID
PlatformIdInitialize (
  VOID
  )
{
  TYPE_CHOPTYPE_ENUM  ChopType;

  if (GetPlatformId () != 0) {
    // PlatformID is assigned already, don't detect again.
    return;
  }

  ChopType = GetChopType (0);
  if (ChopType == TypeLcc) {
    SetPlatformId (PLATFORM_ID_LCC_RP);
  } else if (ChopType == TypeHcc) {
    SetPlatformId (PLATFORM_ID_HCC_RP);
  } else {
    DEBUG ((DEBUG_ERROR, "Unsupported Platform!\n"));
  }
}

/**
  Initialize Platform Device Table

**/
VOID
PlatformDeviceTableInitialize (
  VOID
  )
{
  PLT_DEVICE_TABLE     *PltDeviceTable;
  UINT32                Size;

  Size = sizeof (mPlatformDevices);
  PltDeviceTable = (PLT_DEVICE_TABLE *)AllocatePool (sizeof (PLT_DEVICE_TABLE) + Size);
  if (PltDeviceTable != NULL) {
    PltDeviceTable->DeviceNumber = ARRAY_SIZE (mPlatformDevices);
    CopyMem (PltDeviceTable->Device, mPlatformDevices, Size);

    SetDeviceTable (PltDeviceTable);
  }
}


/**
  Initialize SPI controller

**/
VOID
SpiControllerInitialize (
  VOID
)
{
  UINTN   SpiBaseAddress;

  SpiBaseAddress = GetDeviceAddr (OsBootDeviceSpi, 0);
  SpiBaseAddress = TO_MM_PCI_ADDRESS (SpiBaseAddress);

  MmioWrite32 (SpiBaseAddress + R_SPI_BASE, PCH_SPI_BASE_ADDRESS);
  MmioAnd8    (SpiBaseAddress + R_SPI_BCR, (UINT8)~B_SPI_BCR_SMM_BWP);

  SpiConstructor ();
}

/**
    Read the Platform Features from the config data
**/
VOID
PlatformFeaturesInit (
  VOID
  )
{
  FEATURES_CFG_DATA           *FeaturesCfgData;
  PLATFORM_DATA               *PlatformData;
  UINTN                        HeciBaseAddress;
  UINT32                       LdrFeatures;

  // Set common features
  LdrFeatures  = GetFeatureCfg ();
  LdrFeatures |= FeaturePcdGet (PcdAcpiEnabled) ? FEATURE_ACPI : 0;
  LdrFeatures |= FeaturePcdGet (PcdVerifiedBootEnabled) ? FEATURE_VERIFIED_BOOT : 0;
  LdrFeatures |= FeaturePcdGet (PcdMeasuredBootEnabled) ? FEATURE_MEASURED_BOOT : 0;

  // Disable feature by configuration data.
  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
  if (FeaturesCfgData != NULL) {
    if (FeaturesCfgData->Features.Acpi == 0) {
      LdrFeatures &= ~FEATURE_ACPI;
    }

    if (FeaturesCfgData->Features.MeasuredBoot == 0) {
      LdrFeatures &= ~FEATURE_MEASURED_BOOT;
    }

    if (FeaturesCfgData->Features.eMMCTuning != 0) {
      LdrFeatures |= FEATURE_MMC_TUNING;
    }
  } else {
    DEBUG ((DEBUG_INFO, "FEATURES CFG DATA NOT FOUND!\n"));
  }

  // Disable features by boot guard profile
  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if (PlatformData != NULL) {
    HeciBaseAddress = MM_PCI_ADDRESS (
                        ME_BUS,
                        ME_DEVICE_NUMBER,
                        HECI_FUNCTION_NUMBER,
                        0x0
                        );
    GetBootGuardInfo (HeciBaseAddress, &PlatformData->BtGuardInfo);
    DEBUG ((DEBUG_INFO, "GetPlatformDataPtr is copied 0x%08X \n", PlatformData));
    if (!PlatformData->BtGuardInfo.MeasuredBoot) {
      LdrFeatures &= ~FEATURE_MEASURED_BOOT;
    }
    if (!PlatformData->BtGuardInfo.VerifiedBoot) {
      LdrFeatures &= ~FEATURE_VERIFIED_BOOT;
    }
  }

  SetFeatureCfg (LdrFeatures);
}

/**
  Initialize TPM.

**/
VOID
TpmInitialize (
  VOID
  )
{
  EFI_STATUS                   Status;
  UINT8                        BootMode;
  PLATFORM_DATA               *PlatformData;
  UINT32                       Features;

  if (MEASURED_BOOT_ENABLED ()) {
    BootMode     = GetBootMode ();
    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();

    if ((PlatformData != NULL) && PlatformData->BtGuardInfo.MeasuredBoot &&
        (!PlatformData->BtGuardInfo.DisconnectAllTpms) &&
        ((PlatformData->BtGuardInfo.TpmType == dTpm20) || (PlatformData->BtGuardInfo.TpmType == Ptt))) {

      //  As per PC Client spec, SRTM should perform a host platform reset
      if (PlatformData->BtGuardInfo.TpmStartupFailureOnS3) {
        ResetSystem(EfiResetCold);
        CpuDeadLoop ();
      }

      // Initialize TPM if it has not already been initialized by BootGuard component (i.e. ACM)
      Status = TpmInit (PlatformData->BtGuardInfo.BypassTpmInit, BootMode);
      if (EFI_ERROR (Status)) {
        CpuHalt ("Tpm Initialization failed !!\n");
      } else {
        if (BootMode != BOOT_ON_S3_RESUME) {
          // Create and add BootGuard Event logs in TCG Event log
          CreateTpmEventLog (PlatformData->BtGuardInfo.TpmType);
        }
      }
    } else {
      DisableTpm ();

      Features  = GetFeatureCfg ();
      Features &= (UINT32)(~FEATURE_MEASURED_BOOT);
      SetFeatureCfg (Features);
    }
  }
}

/**
  Determine if this is firmware update mode.

  This function will determine if we have to put system in firmware update mode.

  @retval  TRUE if firmware update mode is in progress, otherwise FALSE
**/
BOOLEAN
IsFirmwareUpdate (
  VOID
  )
{
  UINTN     Address;

  //
  // Check if state machine is set to capsule processing mode.
  //
  if (CheckStateMachine (NULL) == EFI_SUCCESS) {
    return TRUE;
  }

  //
  // Check if platform firmware update trigger is set.
  //

  //
  // BIOSSCRATCHPAD3_CFG (Bus UBOX_DECS_BUS_NUM, Dev 0, Func 2 PCI_Config 0xAC)
  // If BIT0 is set, indicates FWUpdate is triggered.
  //
  Address = MM_PCI_ADDRESS (UBOX_DECS_BUS_NUM, UBOX_DECS_DEV_NUM, UBOX_DECS_FUNC_NUM, 0);
  if (MmioRead32 (Address + R_BIOSSCRATCHPAD3_UBOX_DECS) & BIT0) {
    return TRUE;
  }

  return FALSE;
}


UINT8
GetPlatformPowerState (
  IN  VOID
  )
{
  UINT8         BootMode;
  UINT32       *NvsData;

  NvsData = (UINT32 *) FindNvsData ();
  if (NvsData == NULL) {
    // FBAF (First Boot After Flash)
    BootMode = BOOT_WITH_FULL_CONFIGURATION;
  } else if ((PmcIsRtcBatteryGood () == FALSE) || IsCMOSBad ()) {
    // Not First boot, but RTC/CMOS is bad
    BootMode = BOOT_WITH_DEFAULT_SETTINGS;
    DEBUG ((DEBUG_INFO, "RTC battery or CMOS Diag bad. Boot with Default Settings.\n"));
  } else {
    // Normal Boot
    BootMode = BOOT_WITH_MINIMAL_CONFIGURATION;
    if (IoRead16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS) & B_ACPI_IO_PM1_STS_WAK) {
      switch (IoRead16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT) & B_ACPI_IO_PM1_CNT_SLP_TYP) {
        case V_ACPI_IO_PM1_CNT_S3:
          BootMode = BOOT_ON_S3_RESUME;
          break;
        case V_ACPI_IO_PM1_CNT_S4:
          BootMode = BOOT_ON_S4_RESUME;
          break;
        case V_ACPI_IO_PM1_CNT_S5:
          //
          // Overwrite BOOT_ON_S5_RESUME with MIN_CONFIG
          //
          BootMode = BOOT_WITH_MINIMAL_CONFIGURATION;
          break;
        default:
          BootMode = BOOT_WITH_MINIMAL_CONFIGURATION;
          break;
      }
    }
    //
    // Report RTC battery failure, Clear Sleep Type
    //
    if (PmcIsRtcBatteryGood () == FALSE) {
      BootMode = BOOT_WITH_DEFAULT_SETTINGS;
      DEBUG ((DEBUG_INFO, "RTC Battery bad. Boot with Default Settings.\n"));
      IoAndThenOr16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT, (UINT16) ~B_ACPI_IO_PM1_CNT_SLP_TYP, V_ACPI_IO_PM1_CNT_S0);
    }
    //
    // Clear WAK STS. Clear the PWRBTN_EN, it causes SMI# otherwise (SCI_EN is 0).
    //
    IoAndThenOr32 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS, (UINT32)~B_ACPI_IO_PM1_EN_PWRBTN_EN, B_ACPI_IO_PM1_STS_WAK);
  }

  IoAnd32 (ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_STS_127_96, (UINT32)~B_ACPI_IO_GPE0_STS_127_96_PME_B0);
  IoAnd32 (ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_STS_127_96, (UINT32)~B_ACPI_IO_GPE0_STS_127_96_PCI_EXP);
  IoAnd32 (ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_STS_127_96, (UINT32)~B_ACPI_IO_GPE0_STS_127_96_PME);

  return BootMode;
}

/**
  Print Bootguard status

**/
VOID
PrintBootGuardStatus (
  VOID
  )
{
DEBUG_CODE_BEGIN();
  UINT32  Data;

  Data = *(UINT32 *) (UINTN)  (TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_ACM_STATUS);
  DEBUG ((DEBUG_ERROR, "[Boot Guard] AcmStatus : 0x%08X\n", Data));

  Data = *(UINT32 *) (UINTN) (TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_BOOTSTATUS);
  DEBUG ((DEBUG_ERROR, "[Boot Guard] BootStatus : 0x%08X\n", Data));

  if ((Data & (BIT31 | BIT30)) != BIT31) {
    DEBUG ((DEBUG_ERROR, "[Boot Guard] Boot Guard Failed or is Disabled!\n", Data));
  } else {
    DEBUG ((DEBUG_INFO, "[Boot Guard] Boot Guard is Enabled Successfully.\n", Data));
  }
DEBUG_CODE_END();
}

/**
  Print Platform Post Memory Info

**/
VOID
PrintPostMemoryInfo (
  VOID
)
{
DEBUG_CODE_BEGIN();
  EFI_HOB_GUID_TYPE         *GuidHob;
  VOID                      *FspHobList;
  FSP_SMBIOS_MEMORY_INFO    *FspSmbiosMemoryInfo;
  CHANNEL_INFO              *ChannelInfo;
  DIMM_INFO                 *DimmInfo;
  UINT8                      ChannelCount;
  UINT8                      DimmCount;
  UINT8                      ModulePartNumLen;
  UINT8                      Index;

  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    GuidHob = GetNextGuidHob (&gFspSmbiosMemoryInfoHobGuid, FspHobList);
    if (GuidHob != NULL) {
      FspSmbiosMemoryInfo = GET_GUID_HOB_DATA (GuidHob);
      if (FspSmbiosMemoryInfo != NULL) {
        DEBUG ((DEBUG_INFO, "====== FSP SMBIOS MEMORY INFO HOB ======\n"));
        DEBUG ((DEBUG_INFO, "Revision            : %d\n", FspSmbiosMemoryInfo->Revision));
        DEBUG ((DEBUG_INFO, "DataWidth           : %d\n", FspSmbiosMemoryInfo->DataWidth));
        DEBUG ((DEBUG_INFO, "MemoryType          : %d\n", FspSmbiosMemoryInfo->MemoryType));
        DEBUG ((DEBUG_INFO, "MemoryFrequencyInMHz: %d\n", FspSmbiosMemoryInfo->MemoryFrequencyInMHz));
        DEBUG ((DEBUG_INFO, "ErrorCorrectionType : %d\n", FspSmbiosMemoryInfo->ErrorCorrectionType));
        DEBUG ((DEBUG_INFO, "ChannelCount        : %d\n", FspSmbiosMemoryInfo->ChannelCount));
        for (ChannelCount = 0; ChannelCount < FspSmbiosMemoryInfo->ChannelCount; ChannelCount++) {
          ChannelInfo = &FspSmbiosMemoryInfo->ChannelInfo[ChannelCount];
          if (ChannelInfo != NULL) {
            DEBUG ((DEBUG_INFO, "  ChannelId: %d\n", ChannelInfo->ChannelId));
            DEBUG ((DEBUG_INFO, "  DimmCount: %d\n", ChannelInfo->DimmCount));
            for (DimmCount = 0; DimmCount < ChannelInfo->DimmCount; DimmCount++) {
              DimmInfo = &ChannelInfo->DimmInfo[DimmCount];
              if (DimmInfo != NULL) {
                DEBUG ((DEBUG_INFO, "    DimmId       : %d\n", DimmInfo->DimmId));
                DEBUG ((DEBUG_INFO, "    SizeInMb     : %d\n", DimmInfo->SizeInMb));
                DEBUG ((DEBUG_INFO, "    MfgId        : %d\n", DimmInfo->MfgId));
                DEBUG ((DEBUG_INFO, "    ModulePartNum: "));
                ModulePartNumLen = sizeof (DimmInfo->ModulePartNum);
                for (Index = 0; Index < ModulePartNumLen; Index++) {
                  DEBUG ((DEBUG_INFO, "%c", DimmInfo->ModulePartNum[Index]));
                }
                DEBUG ((DEBUG_INFO, "\n"));
              }
            }
          }
        }
        DEBUG ((DEBUG_INFO, "========================================\n"));
      }
    }
  }
DEBUG_CODE_END();
}

/**
  shadow Fspm code into cache to prevent DMI SPI access during mode activation

**/
VOID
ShadowFspmCodeInCache (
  VOID
  )
{
  FSP_INFO_HEADER *FspInfoHeader;
  UINT32           ImageBase;
  UINT32           ImageMax;
  UINT8           *Temp;

  //
  // BtG already cached Fspm region. So cache Fspm only if non-BtG
  //
  if ((AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_NEM_ENABLED) == 0) {
    FspInfoHeader = (FSP_INFO_HEADER *)(UINTN)(PCD_GET32_WITH_ADJUST (PcdFSPMBase) + FSP_INFO_HEADER_OFF);
    ImageBase     = FspInfoHeader->ImageBase;
    ImageMax      = ImageBase + FspInfoHeader->ImageSize;
    DEBUG ((DEBUG_INFO, "ShadowFspmCodeInCache: ImageBase 0x%x, ImageMax 0x%x\n", ImageBase, ImageMax));

    //
    // This is to avoid gcc compiler warning 'unused-but-set-variable'.
    // Even if 'volatile' keyword is used, gcc complains on a local variable.
    //
    Temp = AllocateTemporaryMemory (0);

    while (ImageBase < ImageMax) {
      *Temp = *(volatile UINT8 *)(UINTN)ImageBase;
      ImageBase += 64;
    }
  }
}

/**
  Handle TopSwap setting

**/
VOID
FwuTopSwapSetting (
  VOID
  )
{
  EFI_STATUS        Status;
  UINT32            Data32;
  UINT32            TopSwapReg;
  UINT32            P2sbBar;
  UINT32            RsvdBase;
  UINT32            RsvdSize;
  UINTN             P2sbBase;
  BOOLEAN           P2sbIsHidden;
  FW_UPDATE_STATUS  *pFwUpdStatus;

  pFwUpdStatus = NULL;
  Status = GetComponentInfoByPartition (FLASH_MAP_SIG_BLRESERVED, FALSE, &RsvdBase, &RsvdSize);
  if (!EFI_ERROR (Status)) {
    pFwUpdStatus = (FW_UPDATE_STATUS *)(UINTN)RsvdBase;
  } else {
    DEBUG ((DEBUG_ERROR, "Failed to get BLRESERVED region\n"));
  }

  //
  // Get Top swap register Bit0 in PCH Private Configuration Space.
  //
  P2sbBase     = MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_P2SB, 0); // P2SB device base
  P2sbIsHidden = FALSE;
  if (MmioRead16 (P2sbBase) == 0xFFFF) {
    //
    // unhide P2SB
    //
    MmioWrite8 (P2sbBase + R_P2SB_CFG_E0 + 1, 0);
    P2sbIsHidden = TRUE;
    DEBUG ((DEBUG_INFO, "P2sb is hidden, unhide it\n"));
  }

  P2sbBar   = MmioRead32 (P2sbBase + R_P2SB_CFG_SBREG_BAR);
  P2sbBar  &= 0xFFFFFFF0;
  if (P2sbBar == 0xFFFFFFF0) {
    DEBUG ((DEBUG_INFO, "Failed to get P2sbBase!\n"));
    if (P2sbIsHidden) {
      MmioWrite8 (P2sbBase + R_P2SB_CFG_E0 + 1, BIT0);
      DEBUG ((DEBUG_INFO, "Hide p2sb again.\n"));
    }
    return;
  }

  TopSwapReg = P2sbBar | ((PID_RTC_HOST) << 16) | (UINT16)(R_RTC_PCR_BUC);
  Data32     = MmioRead32 (TopSwapReg);
  DEBUG ((DEBUG_VERBOSE, "P2sbBar=0x%x, TopSwapReg=0x%x\n", P2sbBar, Data32));
  if ((pFwUpdStatus != NULL) && (pFwUpdStatus->StateMachine == FW_UPDATE_SM_PART_A)) {
    if (GetCurrentBootPartition() == 0) {
      DEBUG ((DEBUG_INFO, "Reboot to BP1...\n"));
      Data32 |= B_RTC_PCR_BUC_TS;
      MmioWrite32 (TopSwapReg, Data32);
      ResetSystem (EfiResetCold);
    }
  } else if ((pFwUpdStatus != NULL) && (pFwUpdStatus->StateMachine == FW_UPDATE_SM_PART_B)) {
    if (GetCurrentBootPartition() == 1) {
      DEBUG ((DEBUG_INFO, "Reboot to BP0...\n"));
      Data32 &= ~B_RTC_PCR_BUC_TS;
      MmioWrite32 (TopSwapReg, Data32);
      ResetSystem (EfiResetCold);
    }
  } else {
    DEBUG ((DEBUG_INFO, "Not in Firmware Update mode.\n"));
    if (GetCurrentBootPartition() == 1) {
      DEBUG ((DEBUG_INFO, "Reboot to BP0...\n"));
      Data32 &= ~B_RTC_PCR_BUC_TS;
      MmioWrite32 (TopSwapReg, Data32);
      ResetSystem (EfiResetCold);
    }
  }

  if (P2sbIsHidden) {
    //
    // Hide P2SB
    //
    MmioWrite8 (P2sbBase + R_P2SB_CFG_E0 + 1, BIT0);
    DEBUG ((DEBUG_INFO, "Hide p2sb again.\n"));
  }
}

/**
  Board specific hook points.

  Implement board specific initialization during the boot flow.

  @param[in] InitPhase             Current phase in the boot flow.

**/
VOID
EFIAPI
BoardInit (
  IN  BOARD_INIT_PHASE  InitPhase
  )
{
  switch (InitPhase) {
  case PreConfigInit:
    PrintBootGuardStatus ();
    PlatformDeviceTableInitialize ();
    SpiControllerInitialize ();
    SetBootMode (IsFirmwareUpdate() ? BOOT_ON_FLASH_UPDATE : GetPlatformPowerState());
    FwuTopSwapSetting ();
    break;
  case PostConfigInit:
    PlatformIdInitialize ();
    PlatformNameInit ();
    PlatformFeaturesInit ();
    VariableInitialize ();
    RtcInit ();
    break;
  case PreMemoryInit:
    PreMemGpioInit ();
    ShadowFspmCodeInCache ();
    break;
  case PostMemoryInit:
    PrintPostMemoryInfo ();
    break;
  case PreTempRamExit:
    break;
  case PostTempRamExit:
    EnlargeCodeRegionCache ();
    TpmInitialize();
    break;
  default:
    break;
  }
}

/**
  Search for the saved MrcParam to initialize Memory for fastboot.

  @retval Found MrcParam or NULL

**/
VOID *
EFIAPI
FindNvsData (
  VOID
  )
{
  UINT32        MrcData;
  EFI_STATUS    Status;

  Status = GetComponentInfo (FLASH_MAP_SIG_MRCDATA, &MrcData, NULL);
  if (!EFI_ERROR (Status)) {
    if (*(UINT32 *)(UINTN)MrcData != 0xFFFFFFFF) {
      DEBUG ((DEBUG_INFO, "Found Saved MRC Data @ 0x%08X!\n", MrcData));
      return (VOID *)(UINTN)MrcData;
    }
  }

  DEBUG ((DEBUG_INFO, "Not Found Saved MRC Data!\n"));
  return NULL;
}

/**
  Load the configuration data blob from media into destination buffer.

  @param[in]    Dst        Destination address to load configuration data blob.
  @param[in]    Src        Source address to load configuration data blob.
  @param[in]    Len        The destination address buffer size.

  @retval  EFI_SUCCESS             Configuration data blob was loaded successfully.
  @retval  EFI_NOT_FOUND           Configuration data blob cannot be found.
  @retval  EFI_OUT_OF_RESOURCES    Destination buffer is too small to hold the
                                   configuration data blob.
  @retval  Others                  Failed to load configuration data blob.

**/
EFI_STATUS
EFIAPI
LoadExternalConfigData (
  IN UINT32  Dst,
  IN UINT32  Src,
  IN UINT32  Len
  )
{
  return SpiLoadExternalConfigData (Dst, Src, Len);
}
