/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage1BBoardInitLib.h"
#include <Library/SgxLib22.h>
#include "IioBoardSettingHob.h"

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
        .PciDeviceNumber    = PCI_DEFAULT_DEVICE_NUMBER,
        .PciBusNumber       = PCI_BUS_NUMBER_XHCI_1,
        .IsMmioDevice       = 0
      }
    },
    OsBootDeviceUsb,
    0
  },
    {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = PCI_FUNCTION_NUMBER_PCH_XHCI,
        .PciDeviceNumber    = PCI_DEFAULT_DEVICE_NUMBER,
        .PciBusNumber       = PCI_BUS_NUMBER_XHCI_2,
        .IsMmioDevice       = 0
      }
    },
    OsBootDeviceUsb,
    1
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = PCI_FUNCTION_NUMBER_NVME,
        .PciDeviceNumber    = PCI_DEFAULT_DEVICE_NUMBER,
        .PciBusNumber       = PCI_BUS_NUMBER_NVME,
        .IsMmioDevice       = 0
      }
    },
    OsBootDeviceNvme,
    0
  }
};

// DIMM chunk size in MB
#define DIMM_CHUNK_SIZE 64

// GPIO WARNING: The number of pads must be obtained from board schematics
#define  BOARD_ID_GPIO_PADS_NUMBER     6
#define  BOARD_REV_ID_GPIO_PADS_NUMBER 3

//
// These pads shall not be board specific as these are used for Board ID detection
//
GPIOV2_PAD  mBoardId [BOARD_ID_GPIO_PADS_NUMBER] = {
  // GPIO WARNING: The pad number must be obtained from board schematics
  GPIOV2_IBL_VIRTUAL_69,
  GPIOV2_IBL_VIRTUAL_70,
  GPIOV2_IBL_VIRTUAL_71,
  GPIOV2_IBL_VIRTUAL_72,
  GPIOV2_IBL_VIRTUAL_73,
  GPIOV2_IBL_VIRTUAL_74
};

//
// These pads shall not be board specific as these are used for Rev ID detection
//
GPIOV2_PAD  mBoardRevId [BOARD_REV_ID_GPIO_PADS_NUMBER] = {
  // GPIO WARNING: This should be obtained from board schematics
  GPIOV2_IBL_VIRTUAL_75,
  GPIOV2_IBL_VIRTUAL_76,
  GPIOV2_IBL_VIRTUAL_77
};

/**
  Dump Iio Board Settings Hob.

  @param[out] HobPtr          Hob Pointer for Board Setting

 */
VOID
DumpBoardSettingHob (
  IN  IIO_BOARD_SETTINGS_HOB   *IioBoardSettingsHob
  )
{

DEBUG_CODE_BEGIN();

  UINT8                     Socket;
  UINT8                     Pe;
  CHAR8                     PeAscii[2] = {0};
  UINT8                     Port;
  UINT16                    TotalPortIndex;

  DEBUG((DEBUG_VERBOSE, "\n================  BoardSetting Dump Start=============\n"));
  DEBUG((DEBUG_VERBOSE,"[S.Pe.P] |Bif |Cxl |Rsvd|VAddr|VPort|VMuxA|VMuxC|NPAddr|NPB |NPMA|"));
  DEBUG((DEBUG_VERBOSE,"NPMC|SltEIP|SltHPSS|SltPIP|SltAIP|SltPCP|SltABP|SltRsvd|"));
  DEBUG((DEBUG_VERBOSE,"VppEn|VppEx|NpmSpt|SltImp|Rt1|Rt2|ComClk|SRIS|HotP|MrlSP|SltPLS|SltPLV|PhyS\n"));

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    for (Pe = 0; Pe < MAX_IIO_PCIE_PER_SOCKET; Pe++) {
      PeAscii[0] = Pe + '0';

      for (Port = 0; Port < MAX_IIO_PORTS_PER_STACK; Port++) {
        TotalPortIndex = (Socket * MAX_IIO_PCIE_PER_SOCKET * MAX_IIO_PORTS_PER_STACK) + (Pe * MAX_IIO_PORTS_PER_STACK) + Port;

        DEBUG((DEBUG_VERBOSE,"[%d.%a.%3d]|0x%-2X|0x%-2X|0x%-2X", Socket, PeAscii, TotalPortIndex,IioBoardSettingsHob->Socket[Socket].Pe[Pe].Bifurcation,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].CxlSupportInUba,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Reserved));

        DEBUG((DEBUG_VERBOSE,"|0x%-3X|0x%-3X|0x%-3X|0x%-3X",
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Vpp.Address,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Vpp.Port,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Vpp.MuxAddress,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Vpp.MuxChannel));

        DEBUG((DEBUG_VERBOSE,"|0x%-4X|0x%-2X|0x%-2X|0x%-2X",
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Npem.Address,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Npem.Bank,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Npem.MuxAddress,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Npem.MuxChannel));

        DEBUG((DEBUG_VERBOSE,"|0x%-4X|0x%-5X|0x%-4X|0x%-4X|0x%-4X|0x%-4X|0x%-5X",
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Slot.Eip,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Slot.HotPlugSurprise,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Slot.PowerInd,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Slot.AttentionInd,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Slot.PowerCtrl,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Slot.AttentionBtn,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Slot.Reserved
          ));

        DEBUG((DEBUG_VERBOSE,"|0x%-3X|0x%-3X|0x%-4X",
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].VppEnabled,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].VppExpType,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].NpemSupported));

        DEBUG((DEBUG_VERBOSE,"|0x%-4X|0x%-2X|0x%-2X|0x%-2X|0x%-2X|0x%-2X|0x%-3X|0x%-4X|0x%-4X|0x%-2X",
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].SlotImplemented,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Retimer1Present,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].Retimer2Present,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].CommonClock,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].SRIS,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].HotPlug,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].MrlSensorPresent,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].SlotPowerLimitScale,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].SlotPowerLimitValue,
          IioBoardSettingsHob->Socket[Socket].Pe[Pe].Port[Port].PhysicalSlotNumber));
        DEBUG((DEBUG_VERBOSE, "\n"));
      }
    }
  }
  DEBUG((DEBUG_VERBOSE, "\n================ BoardSetting Dump End=============\n"));

DEBUG_CODE_END();
}

/**
  Create and update Iio Board Settings Hob.
  Hob is used to store board specific settings related to IIO before IioSiPolicy creation.

  @param[out] BoardSettingsHob          Pointer to created hob address

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
 */
RETURN_STATUS
CreateAndUpdateBoardSettingsHob (
  IIO_BOARD_SETTINGS_HOB **BoardSettingsHob
  )
{
  // Allocate memory for IIO board settings HOB data structure
  *BoardSettingsHob = (IIO_BOARD_SETTINGS_HOB *)AllocatePool(sizeof (IIO_BOARD_SETTINGS_HOB));
  if (*BoardSettingsHob == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem(*BoardSettingsHob, sizeof (IIO_BOARD_SETTINGS_HOB));

  // Load the HOB with default configuration
  CopyMem(*BoardSettingsHob, &DefaultConfigIIOBoardSettingsHob, sizeof(IIO_BOARD_SETTINGS_HOB));

  // Update the desired fields of HOB here if required

  // Dump the HOB data
  DumpBoardSettingHob(*BoardSettingsHob);

  return EFI_SUCCESS;
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
  PLATFORM_DATA_BL           *PlatformData;
  FEATURES_CFG_DATA          *FeaturesCfgData;
  SEC_CFG_DATA               *Securitycfgdata;
  EFI_STATUS                  Status;
  PCIE_CFG_DATA              *PcieCfgData;
  UINT32                      CarBase;
  UINT32                      CarSize;
  UINT8                      DebugPort;
  FSPM_UPD_COMMON_FSP24       *FspmUpdCommon24;
  FSP_INFO_HEADER             *FspHeader;
  IIO_BOARD_SETTINGS_HOB     *BoardSettingsHob;

  FspmUpd = (FSPM_UPD *)FspmUpdPtr;

  // Overriding the FSPM_ARCH2_UPD struct as FSP expects this structure to be populated
  // and uses these values
  FspmUpdCommon24 = (FSPM_UPD_COMMON_FSP24 *)FspmUpd;
  FspmUpdCommon24->FspmArchUpd.BootLoaderTolumSize  = 0;
  FspmUpdCommon24->FspmArchUpd.BootMode             = (UINT32)GetBootMode();

  FspHeader = (FSP_INFO_HEADER *)(UINTN)(PCD_GET32_WITH_ADJUST (PcdFSPMBase) + FSP_INFO_HEADER_OFF);
  // This must be set to NULL if ImageAttribute indicates FSP Variable support
  if ((FspHeader->ImageAttribute & IMAGE_ATTRIBUTE_VAR_SERVICES_SUPPORT) == IMAGE_ATTRIBUTE_VAR_SERVICES_SUPPORT) {
    FspmUpdCommon24->FspmArchUpd.NvsBufferPtr       = (UINTN)NULL;
  } else {
    FspmUpdCommon24->FspmArchUpd.NvsBufferPtr       = (UINTN)FindNvsData();
  }

  Status = GetTempRamInfo (&CarBase, &CarSize);
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
      FspmUpd->FspmConfig.DynamicEccModeSel        = MemCfgData->GeneralConfig.DynamicEccModeSel;
      FspmUpd->FspmConfig.HwMemTest                = MemCfgData->GeneralConfig.MemTest;
      FspmUpd->FspmConfig.EnableRMT                = MemCfgData->EnableRMT;
      FspmUpd->FspmConfig.EnableRMTonFCB           = MemCfgData->EnableRMTonFCB;
    } else {
      FspmUpd->FspmConfig.DynamicEccModeSel        = 0;
      FspmUpd->FspmConfig.HwMemTest                = 0;
      FspmUpd->FspmConfig.EnableRMT                = 0;
      FspmUpd->FspmConfig.EnableRMTonFCB           = 0;
    }

    FspmUpd->FspmConfig.TestSignalBitMaskRMT         = MemCfgData->TestSignalBitMaskRMT;
    FspmUpd->FspmConfig.serialDebugMsgLvl            = MemCfgData->ParameterConfig.serialDebugMsgLvl;
    FspmUpd->FspmConfig.AttemptFastBoot              = MemCfgData->GeneralConfig.AttemptFastBoot;
    FspmUpd->FspmConfig.AttemptFastBootCold          = MemCfgData->GeneralConfig.AttemptFastBootCold;
    FspmUpd->FspmConfig.EnforceDdrMemoryFreqPor      = MemCfgData->ParameterConfig.EnforceDdrMemoryFreqPor;
    // Initialize TME (Total Memory Encryption)
    FspmUpd->FspmConfig.SecurityCctEnableTme             = MemCfgData->GeneralConfig.TmeEnable;

    // Initialize MKTME (Multi-Key Total Memory Encryption)
    // only if TME is enabled
    if (FspmUpd->FspmConfig.SecurityCctEnableTme == 1) {
      FspmUpd->FspmConfig.SecurityCctEnableMktme          = MemCfgData->GeneralConfig.MkTmeEnable;
    } else {
      FspmUpd->FspmConfig.SecurityCctEnableMktme          = 0;
    }

    FspmUpd->FspmConfig.KtiLinkSpeedPerPort[0]              = MemCfgData->ParameterConfig.KtiLinkSpeedPerPort[0];
    FspmUpd->FspmConfig.DfxL0pEnable[0]                     = MemCfgData->ParameterConfig.DfxL0pEnable[0];
    FspmUpd->FspmConfig.DfxL1Enable[0]                      = MemCfgData->ParameterConfig.DfxL1Enable[0];
    FspmUpd->FspmConfig.KtiLinkSpeedPerPort[1]              = MemCfgData->ParameterConfig.KtiLinkSpeedPerPort[1];
    FspmUpd->FspmConfig.DfxL0pEnable[1]                     = MemCfgData->ParameterConfig.DfxL0pEnable[1];
    FspmUpd->FspmConfig.DfxL1Enable[1]                      = MemCfgData->ParameterConfig.DfxL1Enable[1];
    FspmUpd->FspmConfig.KtiLinkSpeedPerPort[2]              = MemCfgData->ParameterConfig.KtiLinkSpeedPerPort[2];
    FspmUpd->FspmConfig.DfxL0pEnable[2]                     = MemCfgData->ParameterConfig.DfxL0pEnable[2];
    FspmUpd->FspmConfig.DfxL1Enable[2]                      = MemCfgData->ParameterConfig.DfxL1Enable[2];
    FspmUpd->FspmConfig.KtiLinkSpeedPerPort[3]              = MemCfgData->ParameterConfig.KtiLinkSpeedPerPort[3];
    FspmUpd->FspmConfig.DfxL0pEnable[3]                     = MemCfgData->ParameterConfig.DfxL0pEnable[3];
    FspmUpd->FspmConfig.DfxL1Enable[3]                      = MemCfgData->ParameterConfig.DfxL1Enable[3];
    FspmUpd->FspmConfig.DirectoryModeEn                     = MemCfgData->ParameterConfig.DirectoryModeEn;
    FspmUpd->FspmConfig.UmaClustering                       = MemCfgData->ParameterConfig.UmaClustering;
    FspmUpd->FspmConfig.RefreshMode                         = MemCfgData->ParameterConfig.RefreshMode;
    FspmUpd->FspmConfig.MemHotIn                            = MemCfgData->ParameterConfig.MemHotIn;
    FspmUpd->FspmConfig.KtiCrcMode                          = MemCfgData->ParameterConfig.KtiCrcMode;
    FspmUpd->FspmConfig.ProcessorMsrLockControl             = MemCfgData->ParameterConfig.ProcessorMsrLockControl;
    FspmUpd->FspmConfig.EnforcePopulationPor                = MemCfgData->ParameterConfig.EnforcePopulationPor;
    FspmUpd->FspmConfig.RMTPerDisplayPlots                  = MemCfgData->RMTPerDisplayPlots;
    FspmUpd->FspmConfig.MemIOHealthCheck                    = MemCfgData->ParameterConfig.MemIOHealthCheck;
    FspmUpd->FspmConfig.RMTDebugMessages                    = MemCfgData->RMTDebugMessages;
    FspmUpd->FspmConfig.CriticalRxDqsDelayLeftEdge          = MemCfgData->ParameterConfig.CriticalRxDqsDelayLeftEdge;
    FspmUpd->FspmConfig.CriticalRxDqsDelayRightEdge         = MemCfgData->ParameterConfig.CriticalRxDqsDelayRightEdge;
    FspmUpd->FspmConfig.CriticalRxVrefLeftEdge              = MemCfgData->ParameterConfig.CriticalRxVrefLeftEdge;
    FspmUpd->FspmConfig.CriticalRxVrefRightEdge             = MemCfgData->ParameterConfig.CriticalRxVrefRightEdge;
    FspmUpd->FspmConfig.CriticalTxDqDelayLeftEdge           = MemCfgData->ParameterConfig.CriticalTxDqDelayLeftEdge;
    FspmUpd->FspmConfig.CriticalTxDqDelayRightEdge          = MemCfgData->ParameterConfig.CriticalTxDqDelayRightEdge;
    FspmUpd->FspmConfig.CriticalTxVrefLeftEdge              = MemCfgData->ParameterConfig.CriticalTxVrefLeftEdge;
    FspmUpd->FspmConfig.CriticalTxVrefRightEdge             = MemCfgData->ParameterConfig.CriticalTxVrefRightEdge;
    FspmUpd->FspmConfig.CriticalCaTimingLeftEdge            = MemCfgData->ParameterConfig.CriticalCaTimingLeftEdge;
    FspmUpd->FspmConfig.CriticalCaTimingRightEdge           = MemCfgData->ParameterConfig.CriticalCaTimingRightEdge;
    FspmUpd->FspmConfig.CriticalCtlTimingLeftEdge           = MemCfgData->ParameterConfig.CriticalCtlTimingLeftEdge;
    FspmUpd->FspmConfig.CriticalCtlTimingRightEdge          = MemCfgData->ParameterConfig.CriticalCtlTimingRightEdge;
    FspmUpd->FspmConfig.CriticalCaVrefLeftEdge              = MemCfgData->ParameterConfig.CriticalCaVrefLeftEdge;
    FspmUpd->FspmConfig.CriticalCaVrefRightEdge             = MemCfgData->ParameterConfig.CriticalCaVrefRightEdge;
  }

  PlatformData    = (PLATFORM_DATA_BL *) GetPlatformDataPtr ();
  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
  if ((PlatformData != NULL) && (FeaturesCfgData != NULL)) {
    PlatformData->PlatformFeatures.VtdEnable                = (UINT8)FeaturesCfgData->Features.VtdSupport & FeaturePcdGet (PcdVtdEnabled);
    FspmUpd->FspmConfig.VtdSupport                          = (UINT8)PlatformData->PlatformFeatures.VtdEnable;
    FspmUpd->FspmConfig.mmiohBase                           = FeaturesCfgData->Features.mmiohBase;
    FspmUpd->FspmConfig.PlatformDataAreaBase                = FeaturesCfgData->Features.PlatformDataAreaBase;
    FspmUpd->FspmConfig.PlatformDataAreaSize                = FeaturesCfgData->Features.PlatformDataAreaSize;
    FspmUpd->FspmConfig.BiosRegionBase                      = FeaturesCfgData->Features.BiosRegionBase;
    FspmUpd->FspmConfig.BiosRegionSize                      = FeaturesCfgData->Features.BiosRegionSize;
    FspmUpd->FspmConfig.LegacyVgaStack                      = FeaturesCfgData->Features.LegacyVgaStack;
  } else {
    DEBUG ((DEBUG_INFO, "No PlatformData/FeaturesCfgData, continue with default platform Vtd feature\n"));
    FspmUpd->FspmConfig.VtdSupport               = 0;
  }

  // Security Settings
  Securitycfgdata = (SEC_CFG_DATA *)FindConfigDataByTag (CDATA_SEC_TAG);
  if (Securitycfgdata == NULL) {
    DEBUG ((DEBUG_WARN, "No Securitycfgdata CFGDATA. Continue to boot with default values!\n"));
  } else {
      FspmUpd->FspmConfig.SecurityCctActmPcdFlash                =  PcdGet32 (PcdActmBase);
      FspmUpd->FspmConfig.SecurityCctPrmrrSafSize                 = Securitycfgdata->SecurityCctPrmrrSafSize;
      FspmUpd->FspmConfig.SecurityCctPrmrrSbftSize                = Securitycfgdata->SecurityCctPrmrrSbftSize;
      FspmUpd->FspmConfig.SecurityCctEnableSbft                   = Securitycfgdata->SecurityCctEnableSbft;
  }

  Status = UpdateFspmSgxConfig(FspmUpd);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_WARN, "SGX Config not updated\n"));
  }

  // Pcie Settings
  PcieCfgData = (PCIE_CFG_DATA *) FindConfigDataByTag (CDATA_PCIE_TAG);
  if (PcieCfgData == NULL) {
    DEBUG ((DEBUG_WARN, "No PcieCfgData CFGDATA. Continue to boot with default values!\n"));
  } else {
      FspmUpd->FspmConfig.DeEmphasisNumber                    = PcieCfgData->DeEmphasisNumber;
      FspmUpd->FspmConfig.PcieCommonClockNumber               = PcieCfgData->PcieCommonClockNumber;
      FspmUpd->FspmConfig.DfxCxlDebugModeNumber               = PcieCfgData->DfxCxlDebugModeNumber;
      FspmUpd->FspmConfig.Gen5LinkDegradation                 = PcieCfgData->Gen5LinkDegradation;
      FspmUpd->FspmConfig.MemIOLoopCount                      = PcieCfgData->MemIOLoopCount;
      FspmUpd->FspmConfig.DfxAdvDebugJumper                   = PcieCfgData->DfxAdvDebugJumper;
      FspmUpd->FspmConfig.DelayAfterPCIeLinkTraining          = PcieCfgData->DelayAfterPCIeLinkTraining;
  }

  FspmUpd->FspmConfig.mmiohSize                              = 0xFF;
  FspmUpd->FspmConfig.SecurityCctTmeCryptoAlg                = 0xc;
  FspmUpd->FspmConfig.DfxEvMode                              = 0x2;

  // Uart Setting
  DebugPort = GetDebugPort ();
  if (DebugPort == 0xFF)
    FspmUpd->FspmConfig.SerialIoUartDebugIoBase = 0x3F8;
  else if (DebugPort == 0x02)
    FspmUpd->FspmConfig.SerialIoUartDebugIoBase = 0x3E8;

  if (FeaturePcdGet (PcdCpuX2ApicEnabled)) {
    FspmUpd->FspmConfig.X2apic                            = 1;
  } else {
    FspmUpd->FspmConfig.X2apic                            = 0;
  }

  FspmUpd->FspmConfig.serialDebugMsgLvlTrainResults = 0x0;
  FspmUpd->FspmConfig.DfxDisableBiosDone = 0;
  FspmUpd->FspmConfig.SecurityCctDfxMcheckMsr72 =2;
  FspmUpd->FspmConfig.ProcessorSmxEnable = 0x0;
  FspmUpd->FspmConfig.SecurityCctDfxDisableCctBiosDone = 0;
  FspmUpd->FspmConfig.SmmRelocationEnable = 0;

  // Create and update IIO board settings HOB
  // On successful creation, update the HOB address and length UPDs
  Status = CreateAndUpdateBoardSettingsHob(&BoardSettingsHob);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Could not create IIO board settings HOB!\n"));
    ASSERT_EFI_ERROR(Status);
  } else {
    FspmUpd->FspmConfig.IioBoardSettingsHobPtr = (UINT32)(UINTN)BoardSettingsHob;
    FspmUpd->FspmConfig.IioBoardSettingsHobLength = sizeof(IIO_BOARD_SETTINGS_HOB);
    DEBUG((DEBUG_VERBOSE, "IIO board settings HOB created at 0x%X length %X\n", FspmUpd->FspmConfig.IioBoardSettingsHobPtr, sizeof(IIO_BOARD_SETTINGS_HOB)));
  }

  DEBUG((DEBUG_INFO, "Update FspmUpd with config data...Done\n"));

}


/**
  Detect board and configure PlatformID.

**/
VOID
PlatformIdInitialize (
  VOID
  )
{
  SetPlatformId (PLATFORM_ID_HCC_RP);
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
  PLATFORM_DATA_BL             *PlatformData;
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
  } else {
    DEBUG ((DEBUG_INFO, "FEATURES CFG DATA NOT FOUND!\n"));
  }

  // Disable features by boot guard profile
  PlatformData = (PLATFORM_DATA_BL *)GetPlatformDataPtr ();
  if (PlatformData != NULL) {
    GetBootGuardInfo (0, &PlatformData->BtGuardInfo);
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
  Disable measured boot in SBL
**/
VOID
DisableMeasuredBoot (
  VOID
  )
{
  UINT32    Features;
  Features  = GetFeatureCfg ();
  Features &= (UINT32)(~FEATURE_MEASURED_BOOT);
  SetFeatureCfg (Features);
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
  PLATFORM_DATA_BL             *PlatformData = NULL;
  BOOT_LOADER_VERSION          *BlVersion = NULL;

  BootMode     = GetBootMode ();
  PlatformData = (PLATFORM_DATA_BL *)GetPlatformDataPtr ();
  BlVersion    = GetVerInfoPtr ();

  if (PlatformData == NULL ||
     (PlatformData->BtGuardInfo.BootGuardCapability &&
      PlatformData->BtGuardInfo.DisconnectAllTpms)) {
    DEBUG ((DEBUG_ERROR, "Tpm set to be disabled by ACM !! \n"));
    DisableTpm ();
    DisableMeasuredBoot ();
    return;
  }

  if (PlatformData->BtGuardInfo.BootGuardCapability &&
      PlatformData->BtGuardInfo.MeasuredBoot) {
      //  As per PC Client spec, SRTM should perform a host platform reset
      if (PlatformData->BtGuardInfo.TpmStartupFailureOnS3) {
        ResetSystem(EfiResetCold);
        CpuDeadLoop ();
      }

      // Initialize TPM if it has not already been initialized by BootGuard component (i.e. ACM)
      Status = TpmInit (PlatformData->BtGuardInfo.BypassTpmInit, BootMode);
      if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Tpm Initialization failed  %r !! \n", Status));
      DisableMeasuredBoot ();
    } else if (BootMode != BOOT_ON_S3_RESUME) {
          // Create and add BootGuard Event logs in TCG Event log
          CreateTpmEventLog (PlatformData->BtGuardInfo.TpmType);
        }
  } else if (MEASURED_BOOT_ENABLED ()) {
    // If measured boot enabled in SBL but not in ACM, force TPM initialization here
    Status = TpmInit (FALSE, BootMode);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Tpm Initialization failed  %r !! \n", Status));
      DisableMeasuredBoot ();
    } else if (BootMode != BOOT_ON_S3_RESUME) {
      // Add only CRTM version event to TPM event log at this point
      TpmLogCrtmVersionEvent (BlVersion);
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
    DEBUG ((DEBUG_ERROR, "[Boot Guard] Boot Guard Failed or is Disabled!\n"));
  } else {
    DEBUG ((DEBUG_INFO, "[Boot Guard] Boot Guard is Enabled Successfully.\n"));
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
  EFI_HOB_GUID_TYPE               *GuidHob;
  VOID                            *FspHobList;
  FSP_EXT_SYSTEM_MEMORY_MAP_HOB   *hob;
  FSP_EXT_SYSTEM_MEMORY_MAP_DATA  *FspSmbiosMemoryInfo;
  FSP_EXT_DIMM_DEVICE             *DimmDevice;
  UINT8                           ChIndex;
  UINT8                           DimmIndex;

  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    GuidHob = GetNextGuidHob (&gFspExtSysMemMapInfoHobGuid, FspHobList);
    if (GuidHob != NULL) {
      hob = GET_GUID_HOB_DATA (GuidHob);
      if (hob != NULL) {
        FspSmbiosMemoryInfo = &hob->Data;
        DEBUG ((DEBUG_INFO, "====== FSP MEMORY INFO HOB ======\n"));
        DEBUG ((DEBUG_INFO, "MemoryType           : %d\n", FspSmbiosMemoryInfo->MemoryConfigurationInfo.DramType));
        DEBUG ((DEBUG_INFO, "MemoryFrequencyInMHz : %d\n", FspSmbiosMemoryInfo->MemoryConfigurationInfo.MemFreq));
        DEBUG ((DEBUG_INFO, "ErrorCorrectionType  : %d\n", FspSmbiosMemoryInfo->MemoryConfigurationInfo.ErrorCorrectionType));
        DEBUG ((DEBUG_INFO, "ChannelCount         : %d\n", FspSmbiosMemoryInfo->MemoryMapSocketInfo.Sockets[0].MemoryChannelDeviceInfo.Count));

        for (ChIndex = 0; ChIndex < FspSmbiosMemoryInfo->MemoryMapSocketInfo.Sockets[0].MemoryChannelDeviceInfo.Count; ChIndex++) {
          if (FspSmbiosMemoryInfo->MemoryMapSocketInfo.Sockets[0].MemoryChannelDeviceInfo.Devices[ChIndex].Enabled) {
            DEBUG ((DEBUG_INFO, "  ChannelId : %d\n", ChIndex));
            DEBUG ((DEBUG_INFO, "  DimmCount : %d\n", FspSmbiosMemoryInfo->MemoryMapSocketInfo.Sockets[0].MemoryChannelDeviceInfo.Devices[ChIndex].DimmInfo.Count));
            for (DimmIndex = 0; DimmIndex < FspSmbiosMemoryInfo->MemoryMapSocketInfo.Sockets[0].MemoryChannelDeviceInfo.Devices[ChIndex].DimmInfo.Count; DimmIndex++) {
              if (FspSmbiosMemoryInfo->MemoryMapSocketInfo.Sockets[0].MemoryChannelDeviceInfo.Devices[ChIndex].DimmInfo.Dimms[DimmIndex].Enabled){
                DEBUG ((DEBUG_INFO, "    DimmId       : %d\n", DimmIndex));
                DimmDevice = &FspSmbiosMemoryInfo->MemoryMapSocketInfo.Sockets[0].MemoryChannelDeviceInfo.Devices[ChIndex].DimmInfo.Dimms[DimmIndex];
                DEBUG ((DEBUG_INFO, "    DataWidth    : %d\n", DimmDevice->DataWidth));
                DEBUG ((DEBUG_INFO, "    NumRanks     : %d\n", DimmDevice->NumRanks));
                DEBUG ((DEBUG_INFO, "    DimmSize(MB) : %d\n", DimmDevice->DimmSize * DIMM_CHUNK_SIZE)); // DimmSize's unit is number of chunks. Each chunk is of size 64 MB.
                DEBUG ((DEBUG_INFO, "    DeviceID     : %d\n", DimmDevice->DeviceID));
                DEBUG ((DEBUG_INFO, "    PartNumber   : %s\n", DimmDevice->PartNumber));
              }
            } //Dimms loop
          }
        } //Channels loop
        DEBUG ((DEBUG_INFO, "========================================\n"));
      }
    }
  }
}

/**
  Print Board management controller presence status (BMC / BMC-less)

**/
VOID
PrintBMCPresenceStatus (
  VOID
  )
{
  PLATFORM_DATA_BL  *PlatformData;

  PlatformData = (PLATFORM_DATA_BL *) GetPlatformDataPtr ();
  if ((PlatformData == NULL)) {
    DEBUG ((DEBUG_ERROR, "Platform data is not found\n"));
    return;
  }
  if (PlatformData->PlatformFeatures.BMCPresent == TRUE) {
    DEBUG ((DEBUG_INFO, "BMC is detected\n"));
  } else {
    DEBUG ((DEBUG_INFO, "BMC is not detected\n"));
  }
}

/**
  Initialize Variable.

**/
VOID
VariableInitialize (
  VOID
  )
{
  UINT32                    RgnBase;
  UINT32                    RgnSize;
  EFI_STATUS                Status;

  RgnBase = 0;
  RgnSize = 0;

  Status = GetComponentInfo (FLASH_MAP_SIG_VARIABLE, &RgnBase, &RgnSize);
  if (!EFI_ERROR (Status)) {
    Status = VariableConstructor (RgnBase, RgnSize);

DEBUG_CODE_BEGIN ();
    if (!EFI_ERROR (Status)) {
      UINTN   VariableLen;
      CHAR8   Buffer[VAR_STS_MAX_SIZE_BYTES + 1];

      VariableLen = VAR_STS_MAX_SIZE_BYTES;
      Status = GetVariable (VAR_NAME_VAR_STS, NULL, NULL, &VariableLen, Buffer);
      if (!EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "VariableInitialize: %a %a(%d)\n", VAR_NAME_VAR_STS, Buffer, VariableLen));
      } else if (Status == EFI_NOT_FOUND) {
        Status = AsciiStrCpyS (Buffer, VAR_STS_MAX_SIZE_BYTES, "Initialized!\0");
        if (!EFI_ERROR (Status)) {
          Status = SetVariable (VAR_NAME_VAR_STS, NULL, 0, AsciiStrLen (Buffer), Buffer);
        }
      }
    }
DEBUG_CODE_END ();
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
    PrintBMCPresenceStatus ();
    PrintBootGuardStatus ();
    PlatformDeviceTableInitialize ();
    SpiControllerInitialize ();
    SetBootMode (IsFirmwareUpdate() ? BOOT_ON_FLASH_UPDATE : GetPlatformPowerState());
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
    break;
  case PostMemoryInit:
    PrintPostMemoryInfo ();
    break;
  case PreTempRamExit:
    break;
  case PostTempRamExit:
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
