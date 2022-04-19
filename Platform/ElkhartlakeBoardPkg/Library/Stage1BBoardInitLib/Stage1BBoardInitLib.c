/** @file

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BoardInitLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/SpiFlashLib.h>
#include <FspmUpd.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Pci30.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BoardSupportLib.h>
#include <Library/SocInitLib.h>
#include <PchAccess.h>
#include <RegAccess.h>
#include <Library/CryptoLib.h>
#include <Library/SmbusLib.h>
#include <ConfigDataDefs.h>
#include "GpioTableEhlPreMem.h"
#include <Register/PmcRegs.h>
#include <Library/TpmLib.h>
#include <Library/BootGuardLib.h>
#include <PlatformData.h>
#include <PlatformBoardId.h>
#include <Library/PchSciLib.h>
#include <Library/ContainerLib.h>
#include <ConfigBlock.h>
#include <TccConfigSubRegions.h>
#include <Register/RtcRegs.h>
#include <Register/PmcRegs.h>
#include <GpioConfig.h>
#include <Library/GpioLib.h>

CONST PLT_DEVICE  mPlatformDevices[]= {
  {{0x00001700}, OsBootDeviceSata  , 0 },
  {{0x00001205}, OsBootDeviceUfs   , 0 },
  {{0x00001207}, OsBootDeviceUfs   , 1 },
  {{0x00001A00}, OsBootDeviceEmmc  , 0 },
  {{0x00010000}, OsBootDeviceNvme  , 0 },
  {{0x00001F05}, OsBootDeviceSpi   , 0 },
  {{0x00001400}, OsBootDeviceUsb   , 0 },
  {{0x01000000}, OsBootDeviceMemory, 0 },
  {{0x00001F04}, PltDeviceSmbus    , 0 }
};

#define HDAUDIO_LINK_MODE_HDA               0
#define HDAUDIO_LINK_MODE_I2S_SSP           1
#define HDAUDIO_LINK_MODE_SNDW              2
#define HDAUDIO_LINK_MODE_ADVANCED          3

typedef union {
  UINT8    InputPort0Raw;
  struct {
    UINT8  BomId          :1;   // [0]    - BOM_ID[4]
    UINT8  FabId          :2;   // [2:1]  - Fab_IDx
    UINT8  SpdPresent     :1;   // [3]    - SPD_PRSNT
    UINT8  VirtualBattery :1;   // [4]    - VIRTUAL_BATTERY
    UINT8  VirtualDock    :1;   // [5]    - VIRTUAL_DOCK_DET_N
    UINT8  MemId          :2;   // [7:6]  - MEM_ID
  } InputPort0Fields;
} SMBUS_INPUT_PORT0;

typedef union {
  UINT8    InputPort1Raw;
  struct {
    UINT8  BoardId        :4;   // [3:0]   - BOARD_IDx
    UINT8  BomId          :4;   // [7:4]   - BOM_ID[3:0]
  } InputPort1Fields;
} SMBUS_INPUT_PORT1;

//
// PCA9555 device slave address
//
#define SMBUS_IO_EXPANDER_SLAVE_ADDRESS    0x22

//
// PCA9555 command byte
// -------   ------------
// Command     Register
// -------   ------------
//   0x0     Input Port 0
//   0x1     Input Port 1
#define SMBUS_IO_EXPANDER_INPUT_PORT0_CMD   0x0
#define SMBUS_IO_EXPANDER_INPUT_PORT1_CMD   0x1

/**
  Returns the BoardId ID of the platform from UP2 6000 GPIO pins.

  @param[in]  BoardId           BoardId ID as determined through the GPIO Pins.
**/
VOID
EFIAPI

/**
  Returns the BoardId ID of the platform from Smbus I/O port expander PCA9555PW.

  @param[in]  BoardId           BoardId ID as determined through the Smbus.
**/
GetBoardIdFromSmbus (
  OUT UINT16          *BoardId
  )
{
  UINT8                    BomId;
  SMBUS_INPUT_PORT0        SmbusInputPort0Info;
  SMBUS_INPUT_PORT1        SmbusInputPort1Info;
  EFI_STATUS               Status0;
  EFI_STATUS               Status1;

  Status0 = EFI_DEVICE_ERROR;
  Status1 = EFI_DEVICE_ERROR;

  *BoardId = BoardIdEhlLp4xType4Rvp1;
  DEBUG ((DEBUG_INFO, "Initially set to default BoardId 0x%X\n", *BoardId));

  SmbusInputPort0Info.InputPort0Raw = SmBusReadDataByte (SMBUS_LIB_ADDRESS(SMBUS_IO_EXPANDER_SLAVE_ADDRESS, SMBUS_IO_EXPANDER_INPUT_PORT0_CMD, 0, 0), &Status0);
  SmbusInputPort1Info.InputPort1Raw = SmBusReadDataByte (SMBUS_LIB_ADDRESS(SMBUS_IO_EXPANDER_SLAVE_ADDRESS, SMBUS_IO_EXPANDER_INPUT_PORT1_CMD, 0, 0), &Status1);

  if ((Status0 == EFI_SUCCESS) && (Status1 == EFI_SUCCESS)) {
    DEBUG ((DEBUG_INFO, "Raw data from Input Port 0 is 0x%x\n", SmbusInputPort0Info.InputPort0Raw));
    DEBUG ((DEBUG_INFO, "Raw data from Input Port 1 is 0x%x\n", SmbusInputPort1Info.InputPort1Raw));

    *BoardId = (UINT16) SmbusInputPort1Info.InputPort1Fields.BoardId;
    *BoardId &= 0xF;
    if((*BoardId) < 3 || (*BoardId) > 5) {
      DEBUG ((DEBUG_ERROR, "Fail to read BoardId\n"));
      *BoardId = BoardIdEhlLp4xType4Rvp1;
    }
    BomId = (UINT8) ((SmbusInputPort0Info.InputPort0Fields.BomId << 4) | SmbusInputPort1Info.InputPort1Fields.BomId);
    BomId &= 0x1F;

    DEBUG ((DEBUG_INFO, "Fields.BoardId from Smbus Io expander is 0x%x\n", *BoardId));
    DEBUG ((DEBUG_INFO, "Fields.BomId from Smbus Io expander is 0x%x\n", BomId));
    DEBUG ((DEBUG_INFO, "Fields.FabId from Smbus Io expander is 0x%x\n", (UINT16) (SmbusInputPort0Info.InputPort0Fields.FabId)));
    DEBUG ((DEBUG_INFO, "Fields.SpdPresent from Smbus Io expander is %x\n", (BOOLEAN) (SmbusInputPort0Info.InputPort0Fields.SpdPresent)));
  } else {
    DEBUG ((DEBUG_ERROR, "Failed to get Board ID from Smbus Io expander\n"));
    *BoardId = BoardIdEhlLp4xType3Crb;
  }
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
  UINT32                    *TccCacheBase;
  UINT32                    TccCacheSize;
  UINT32                    *TccStreamBase;
  UINT32                    TccStreamSize;
  EFI_STATUS                Status;
  TCC_CFG_DATA              *TccCfgData;
  BIOS_SETTINGS             *PolicyConfig;
  TCC_STREAM_CONFIGURATION  *StreamConfig;

  TccCfgData = (TCC_CFG_DATA *) FindConfigDataByTag(CDATA_TCC_TAG);
  if ((TccCfgData == NULL) || ((TccCfgData->TccEnable == 0) && (TccCfgData->TccTuning == 0))) {
    return EFI_NOT_FOUND;
  }

  // TCC related memory settings
  DEBUG ((DEBUG_INFO, "Tcc is enabled, Setting memory config.\n"));
  FspmUpd->FspmConfig.TccModeEnablePreMem    = 1;

  FspmUpd->FspmConfig.SaGv                   = 0;    // System Agent Geyserville - SAGV dynamically adjusts the system agent
  FspmUpd->FspmConfig.DisPgCloseIdleTimeout  = 1;    // controls Page Close Idle Timeout
  FspmUpd->FspmConfig.PowerDownMode          = 0;    // controls command bus tristating during idle periods
  FspmUpd->FspmConfig.HyperThreading         = 0;
  FspmUpd->FspmConfig.VmxEnable              = 1;    // RTCM need enable VMX
  FspmUpd->FspmConfig.RaplLim1Ena            = 0;
  FspmUpd->FspmConfig.RaplLim2Ena            = 0;

  FspmUpd->FspmConfig.SoftwareSramEnPreMem   = TccCfgData->TccSoftSram;
  FspmUpd->FspmConfig.DsoTuningEnPreMem      = TccCfgData->TccTuning;
  FspmUpd->FspmConfig.TccErrorLogEnPreMem    = TccCfgData->TccErrorLog;

  // Load TCC stream config from container
  TccStreamBase = NULL;
  TccStreamSize = 0;
  Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'T'),
                          (VOID **)&TccStreamBase, &TccStreamSize);
  if (EFI_ERROR (Status) || (TccStreamSize < sizeof(TCC_STREAM_CONFIGURATION))) {
    DEBUG ((DEBUG_INFO, "Load TCC Stream %r, size = 0x%x\n", Status, TccStreamSize));
  } else {
    FspmUpd->FspmConfig.TccStreamCfgBasePreMem = (UINT32)(UINTN)TccStreamBase;
    FspmUpd->FspmConfig.TccStreamCfgSizePreMem = TccStreamSize;
    DEBUG ((DEBUG_INFO, "Load TCC stream @0x%p, size = 0x%x\n", TccStreamBase, TccStreamSize));

    if (TccCfgData->TccTuning != 0) {
      StreamConfig = (TCC_STREAM_CONFIGURATION *) TccStreamBase;
      PolicyConfig = (BIOS_SETTINGS *) &StreamConfig->BiosSettings;

      FspmUpd->FspmConfig.SaGv                   = PolicyConfig->SaGv;
      FspmUpd->FspmConfig.RaplLim1Ena            = PolicyConfig->MemoryRapl;
      FspmUpd->FspmConfig.RaplLim2Ena            = PolicyConfig->MemoryRapl;
      FspmUpd->FspmConfig.PowerDownMode          = PolicyConfig->MemPowerDown;
      FspmUpd->FspmConfig.DisPgCloseIdleTimeout  = PolicyConfig->DisPgCloseIdle;
      PLAT_FEAT.S0ixEnable                       = PolicyConfig->Sstates;
      DEBUG ((DEBUG_INFO, "Dump TCC DSO BIOS settings:\n"));
      DumpHex (2, 0, sizeof(BIOS_SETTINGS), PolicyConfig);
    }
  }

  // Print configured TCC stream settings
  DEBUG ((DEBUG_INFO, "TCC Stage 1B parameters configuration details:\n"));
  DEBUG ((DEBUG_INFO, "SaGv                  = %x\n", FspmUpd->FspmConfig.SaGv                   ));
  DEBUG ((DEBUG_INFO, "RaplLim1Ena           = %x\n", FspmUpd->FspmConfig.RaplLim1Ena            ));
  DEBUG ((DEBUG_INFO, "RaplLim2Ena           = %x\n", FspmUpd->FspmConfig.RaplLim2Ena            ));
  DEBUG ((DEBUG_INFO, "PowerDownMode         = %x\n", FspmUpd->FspmConfig.PowerDownMode          ));
  DEBUG ((DEBUG_INFO, "DisPgCloseIdleTimeout = %x\n", FspmUpd->FspmConfig.DisPgCloseIdleTimeout  ));
  DEBUG ((DEBUG_INFO, "SoftwareSramEnPreMem  = %x\n", FspmUpd->FspmConfig.SoftwareSramEnPreMem   ));
  DEBUG ((DEBUG_INFO, "DsoTuningEnPreMem     = %x\n", FspmUpd->FspmConfig.DsoTuningEnPreMem      ));
  DEBUG ((DEBUG_INFO, "TccErrorLogEnPreMem   = %x\n", FspmUpd->FspmConfig.TccErrorLogEnPreMem    ));
  DEBUG ((DEBUG_INFO, "Tcc s0ix support      = %x\n", S0IX_STATUS()                              ));

  // Load Tcc Cache config from container
  TccCacheBase = NULL;
  TccCacheSize = 0;
  Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'C'),
                                (VOID **)&TccCacheBase, &TccCacheSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "TCC Cache config not found! %r\n", Status));
  } else {
    FspmUpd->FspmConfig.TccCacheCfgBasePreMem = (UINT32)(UINTN)TccCacheBase;
    FspmUpd->FspmConfig.TccCacheCfgSizePreMem = TccCacheSize;
    DEBUG ((DEBUG_INFO, "Load TCC cache @0x%p, size = 0x%x\n", TccCacheBase, TccCacheSize));
  }

  return Status;
}

/**
  Update S0ix flag
**/
VOID
UpdateS0ixStatus (
  VOID
)
{
  FEATURES_CFG_DATA             *FeaturesCfgData;
  BOOLEAN                       PchSciSupported;

  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
  if (FeaturesCfgData == NULL) {
    DEBUG ((DEBUG_INFO, "Failed to find Cfg Config Data! S0ix setting failed.\n"));
    return;
  }
  PchSciSupported               = PchIsSciSupported ();

  // EHL S0ix condition depends on:
  // - user turn on via config data
  // - not FUSA sku
  // - TCC config turns it on
  if (PchSciSupported == 1 || FeaturesCfgData->Features.S0ix == 0 || S0IX_STATUS() == 0) {
    PLAT_FEAT.S0ixEnable = 0;
  } else {
    PLAT_FEAT.S0ixEnable = 1;
  }
  DEBUG ((DEBUG_INFO, "S0ix Status = %x\n", S0IX_STATUS()));
}

/**
  Update FSP-M UPD config data

  @param  FspmUpdPtr            The pointer to the FSP-M UPD to be updated.

  @retval None
**/
VOID
UpdateFspConfig (
  VOID     *FspmUpdPtr
)
{
  FSPM_UPD                      *FspmUpd;
  FSPM_ARCH_UPD                 *FspmArchUpd;
  FSP_M_CONFIG                  *Fspmcfg;
  MEMORY_CFG_DATA               *MemCfgData;
  GRAPHICS_CFG_DATA             *GfxCfgData;
  SECURITY_CFG_DATA             *SecCfgData;
  UINT32                        Index;
  UINT8                         DebugPort;
  BOOLEAN                       PchSciSupported;
  TCC_CFG_DATA                  *TccCfgData;

  FspmUpd                       = (FSPM_UPD *)FspmUpdPtr;
  FspmArchUpd                   = &FspmUpd->FspmArchUpd;
  Fspmcfg                       = &FspmUpd->FspmConfig;
  PchSciSupported               = PchIsSciSupported ();

  DebugPort = GetDebugPort ();
  if (DebugPort < PCH_MAX_SERIALIO_UART_CONTROLLERS) {
    Fspmcfg->PcdDebugInterfaceFlags = BIT4;
    Fspmcfg->SerialIoUartDebugControllerNumber = DebugPort;
  } else {
    Fspmcfg->PcdDebugInterfaceFlags = BIT1;
    if (DebugPort == 0xFF) {
      Fspmcfg->PcdIsaSerialUartBase = 0;
    } else {
      Fspmcfg->PcdIsaSerialUartBase = 1;
    }
  }
  Fspmcfg->PcdSerialDebugLevel          = 0x3;

  SecCfgData = (SECURITY_CFG_DATA *)FindConfigDataByTag (CDATA_SECURITY_TAG);
  if (SecCfgData != NULL) {
    DEBUG ((DEBUG_INFO, "Load Security Cfg Data\n"));
    // Configure Sgx SPD Data
    Fspmcfg->EnableSgx = SecCfgData->EnableSgx;
  } else {
    DEBUG ((DEBUG_INFO, "Failed to find security CFG!\n"));
  }

  //FSPM has a default value initially, during boot time, Slimboot can modify it which is coming from CfgData blob.
  //then call FSPMemoryInit() who consumes all the overridden values from CfgData blob.

  // Value from EDKII BIOS.
  FspmArchUpd->StackBase        = 0xFEF3FF00;
  FspmArchUpd->StackSize        = 0x40000;

  DEBUG ((DEBUG_INFO, "FSPM CfgData assignment\n"));
  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
  if (MemCfgData != NULL) {
    DEBUG ((DEBUG_INFO, "Load Memory Cfg Data\n"));

    // Board Config
    Fspmcfg->UserBd             = MemCfgData->UserBd;
    Fspmcfg->CridEnable         = MemCfgData->CridEnable;

    // SA_MISC_PEI_PREMEM_CONFIG Start
    CopyMem (&Fspmcfg->SpdAddressTable, MemCfgData->SpdAddressTable, sizeof(MemCfgData->SpdAddressTable));
    Fspmcfg->MemorySpdPtr00             = (UINT32)(UINTN)MemCfgData->MemorySpdPtr00;
    Fspmcfg->MemorySpdPtr10             = (UINT32)(UINTN)MemCfgData->MemorySpdPtr10;
    Fspmcfg->MmioSizeAdjustment         = MemCfgData->MmioSizeAdjustment;
    Fspmcfg->MmioSize                   = MemCfgData->MmioSize;
    Fspmcfg->TsegSize                   = MemCfgData->TsegSize;

    Fspmcfg->PchNumRsvdSmbusAddresses   = sizeof (MemCfgData->SmbusAddressTable) / sizeof (MemCfgData->SmbusAddressTable[0]);
    Fspmcfg->RsvdSmbusAddressTablePtr   = (UINT32)(UINTN)MemCfgData->SmbusAddressTable;

    // SA:PCIE_PEI_PREMEM_CONFIG
    Fspmcfg->TxtImplemented             = MemCfgData->TxtImplemented;
    Fspmcfg->DmiGen3ProgramStaticEq     = MemCfgData->DmiGen3ProgramStaticEq;
    Fspmcfg->InitPcieAspmAfterOprom     = MemCfgData->InitPcieAspmAfterOprom;
    Fspmcfg->DmiDeEmphasis              = MemCfgData->DmiDeEmphasis;
    for (Index=0; Index < 8; Index++) {
      Fspmcfg->DmiGen3RootPortPreset[Index]             = MemCfgData->DmiGen3RootPortPreset[Index];
      Fspmcfg->DmiGen3EndPointPreset[Index]             = MemCfgData->DmiGen3EndPointPreset[Index];
      Fspmcfg->DmiGen3EndPointHint[Index]               = MemCfgData->DmiGen3EndPointHint[Index];
    }
    CopyMem (&Fspmcfg->DmiGen3RxCtlePeaking, MemCfgData->DmiGen3RxCtlePeaking, sizeof(MemCfgData->DmiGen3RxCtlePeaking));

    // SA:TCSS_PEI_PREMEM_CONFIG
    Fspmcfg->UsbTcPortEnPreMem          = MemCfgData->UsbTcPortEnPreMem;
    Fspmcfg->PcieMultipleSegmentEnabled = MemCfgData->PcieMultipleSegmentEnabled;
    Fspmcfg->TcssItbtPcie0En            = MemCfgData->TcssItbtPcie0En;
    Fspmcfg->TcssItbtPcie1En            = MemCfgData->TcssItbtPcie1En;
    Fspmcfg->TcssItbtPcie2En            = MemCfgData->TcssItbtPcie2En;
    Fspmcfg->TcssItbtPcie3En            = MemCfgData->TcssItbtPcie3En;
    Fspmcfg->TcssXhciEn                 = MemCfgData->TcssXhciEn;
    Fspmcfg->TcssXdciEn                 = MemCfgData->TcssXdciEn;
    Fspmcfg->TcssDma0En                 = MemCfgData->TcssDma0En;
    Fspmcfg->TcssDma1En                 = MemCfgData->TcssDma1En;

    // SA:OVERCLOCKING_CONFIG
    Fspmcfg->SaOcSupport                = MemCfgData->SaOcSupport;
    Fspmcfg->GtVoltageMode              = MemCfgData->GtVoltageMode;
    Fspmcfg->GtMaxOcRatio               = MemCfgData->GtMaxOcRatio;
    Fspmcfg->GtVoltageOffset            = MemCfgData->GtVoltageOffset;
    Fspmcfg->GtVoltageOverride          = MemCfgData->GtVoltageOverride;
    Fspmcfg->GtExtraTurboVoltage        = MemCfgData->GtExtraTurboVoltage;
    Fspmcfg->SaVoltageOffset            = MemCfgData->SaVoltageOffset;
    Fspmcfg->RealtimeMemoryTiming       = MemCfgData->RealtimeMemoryTiming;
    for (Index = 0; Index < 0xC; Index++) {
      Fspmcfg->DqByteMapCh0[Index] = MemCfgData->DqByteMapCh0[Index];
      Fspmcfg->DqByteMapCh1[Index] = MemCfgData->DqByteMapCh1[Index];
    }
    CopyMem (&Fspmcfg->DqsMapCpu2DramCh0, MemCfgData->DqsMapCpu2DramCh0, sizeof(MemCfgData->DqsMapCpu2DramCh0));
    CopyMem (&Fspmcfg->DqsMapCpu2DramCh1, MemCfgData->DqsMapCpu2DramCh1, sizeof(MemCfgData->DqsMapCpu2DramCh1));

    // SA_MISC_PEI_PREMEM_CONFIG End

    //Dq/Dqs interleaving
    Fspmcfg->DqPinsInterleaved          = MemCfgData->DqPinsInterleaved;

    Fspmcfg->CaVrefConfig               = MemCfgData->CaVrefConfig;

    //RComp
    CopyMem (&Fspmcfg->RcompResistor, MemCfgData->RcompResistor, sizeof(MemCfgData->RcompResistor));
    CopyMem (&Fspmcfg->RcompTarget, MemCfgData->RcompTarget, sizeof(MemCfgData->RcompTarget));

    Fspmcfg->MrcFastBoot                = MemCfgData->MrcFastBoot;
    Fspmcfg->RmtPerTask                 = MemCfgData->RmtPerTask;
    Fspmcfg->IedSize                    = MemCfgData->IedSize;
    Fspmcfg->SmbusEnable                = MemCfgData->SmbusEnable;
    Fspmcfg->CpuRatio                   = MemCfgData->CpuRatio;
    Fspmcfg->HyperThreading             = MemCfgData->HyperThreading;
    Fspmcfg->FClkFrequency              = MemCfgData->FClkFrequency;
    Fspmcfg->SaGv                       = MemCfgData->SaGv;
    //if(MemCfgData->SaGv == 4) {
    //  Fspmcfg->FreqSaGvLow              = MemCfgData->FreqSaGvLow;
    //  Fspmcfg->FreqSaGvMid              = MemCfgData->FreqSaGvMid;
    //}
    Fspmcfg->DdrSpeedControl            = MemCfgData->DdrSpeedControl;
    Fspmcfg->FreqSaGvLow                = MemCfgData->FreqSaGvLow;
    Fspmcfg->FreqSaGvMid                = MemCfgData->FreqSaGvMid;
    Fspmcfg->DdrFreqLimit               = MemCfgData->DdrFreqLimit;
    Fspmcfg->SaGvLowGear2               = MemCfgData->SaGvLowGear2;
    Fspmcfg->SaGvMidGear2               = MemCfgData->SaGvMidGear2;
    Fspmcfg->SaGvHighGear2              = MemCfgData->SaGvHighGear2;
    Fspmcfg->PchSmbAlertEnable          = MemCfgData->PchSmbAlertEnable;

    Fspmcfg->EnhancedInterleave         = MemCfgData->EnhancedInterleave;
    Fspmcfg->RankInterleave             = MemCfgData->RankInterleave;
    Fspmcfg->RhPrevention               = MemCfgData->RhPrevention;
    if(MemCfgData->RhPrevention == 1) {
      Fspmcfg->RhSolution               = MemCfgData->RhSolution;
    }
    if((MemCfgData->RhPrevention == 1) || (MemCfgData->RhSolution == 0)) {
      Fspmcfg->RhActProbability         = MemCfgData->RhActProbability;
    }
    Fspmcfg->ExitOnFailure              = MemCfgData->ExitOnFailure;
    Fspmcfg->ChHashEnable               = MemCfgData->ChHashEnable;
    Fspmcfg->ChHashInterleaveBit        = MemCfgData->ChHashInterleaveBit;
    Fspmcfg->ChHashMask                 = MemCfgData->ChHashMask;
    Fspmcfg->CkeRankMapping             = MemCfgData->CkeRankMapping;
    Fspmcfg->RemapEnable                = MemCfgData->RemapEnable;

    Fspmcfg->DisableDimmChannel0        = MemCfgData->DisableDimmChannel0;
    Fspmcfg->DisableDimmChannel1        = MemCfgData->DisableDimmChannel1;
    Fspmcfg->ScramblerSupport           = MemCfgData->ScramblerSupport;
    Fspmcfg->PchPseEnable               = MemCfgData->PchPseEnable;

    FspmUpd->FspmConfig.tCL             = MemCfgData->tCL;
    FspmUpd->FspmConfig.tCWL            = MemCfgData->tCWL;
    FspmUpd->FspmConfig.tFAW            = MemCfgData->tFAW;
    FspmUpd->FspmConfig.tRAS            = MemCfgData->tRAS;
    FspmUpd->FspmConfig.tRCDtRP         = MemCfgData->tRCDtRP;
    FspmUpd->FspmConfig.tREFI           = MemCfgData->tREFI;
    FspmUpd->FspmConfig.tRFC            = MemCfgData->tRFC;
    FspmUpd->FspmConfig.tRRD            = MemCfgData->tRRD;
    FspmUpd->FspmConfig.tRTP            = MemCfgData->tRTP;
    FspmUpd->FspmConfig.tWR             = MemCfgData->tWR;
    FspmUpd->FspmConfig.tWTR            = MemCfgData->tWTR;
    FspmUpd->FspmConfig.NModeSupport    = MemCfgData->NModeSupport;
    FspmUpd->FspmConfig.DllBwEn0        = MemCfgData->DllBwEn0;
    FspmUpd->FspmConfig.DllBwEn1        = MemCfgData->DllBwEn1;
    FspmUpd->FspmConfig.DllBwEn2        = MemCfgData->DllBwEn2;
    FspmUpd->FspmConfig.DllBwEn3        = MemCfgData->DllBwEn3;

    //MRC Training algorithms
    Fspmcfg->RMT                        = MemCfgData->RMT;
    Fspmcfg->BdatEnable                 = MemCfgData->BdatEnable;
    Fspmcfg->BdatTestType               = MemCfgData->BdatTestType;
    Fspmcfg->RMC                        = MemCfgData->RMC;
    Fspmcfg->MEMTST                     = MemCfgData->MEMTST;
    //FspmUpd->FspmRestrictedConfig.PerBankRefresh = 0x1;

    Fspmcfg->ECT                        = MemCfgData->ECT;
    Fspmcfg->MrcSafeConfig              = MemCfgData->MrcSafeConfig;
    Fspmcfg->RaplLim1WindX              = MemCfgData->RaplLim1WindX;
    Fspmcfg->RaplLim1WindY              = MemCfgData->RaplLim1WindY;
    Fspmcfg->RaplLim1Pwr                = MemCfgData->RaplLim1Pwr;

    // CPU_SEC_PREMEM_CONFIG
    Fspmcfg->PrmrrSize                  = MemCfgData->PrmrrSize;
    Fspmcfg->Txt                        = MemCfgData->Txt;
    Fspmcfg->SkipStopPbet               = MemCfgData->SkipStopPbet;

    // CPU_LIB_PREMEM_CONFIG
    Fspmcfg->BootFrequency              = MemCfgData->BootFrequency;
    Fspmcfg->ActiveCoreCount            = MemCfgData->ActiveCoreCount;
    Fspmcfg->JtagC10PowerGateDisable    = MemCfgData->JtagC10PowerGateDisable;
    Fspmcfg->BistOnReset                = MemCfgData->BistOnReset;
    Fspmcfg->VmxEnable                  = MemCfgData->VmxEnable;
    Fspmcfg->CpuRatio                   = MemCfgData->CpuRatio;

    //CPU_TXT_PREMEM_CONFIG
    Fspmcfg->SinitMemorySize            = MemCfgData->SinitMemorySize;
    Fspmcfg->TxtHeapMemorySize          = MemCfgData->TxtHeapMemorySize;
    Fspmcfg->TgaSize                    = MemCfgData->TgaSize;
    Fspmcfg->TxtLcpPdBase               = MemCfgData->TxtLcpPdBase;
    Fspmcfg->TxtLcpPdSize               = MemCfgData->TxtLcpPdSize;
    Fspmcfg->TxtDprMemoryBase           = MemCfgData->TxtDprMemoryBase;
    Fspmcfg->TxtDprMemorySize           = MemCfgData->TxtDprMemorySize;
    Fspmcfg->BiosAcmSize                = MemCfgData->BiosAcmSize;
    Fspmcfg->BiosAcmBase                = MemCfgData->BiosAcmBase;
    Fspmcfg->ApStartupBase              = MemCfgData->ApStartupBase;

    // PCIE Clock Source
    CopyMem (Fspmcfg->PcieClkSrcUsage, MemCfgData->PcieClkSrcUsage, sizeof(MemCfgData->PcieClkSrcUsage));
    CopyMem (Fspmcfg->PcieClkSrcClkReq, MemCfgData->PcieClkSrcClkReq, sizeof(MemCfgData->PcieClkSrcClkReq));

    // PCI General
    Fspmcfg->PchMasterClockGating       = MemCfgData->PchMasterClockGating;
    Fspmcfg->PchMasterPowerGating       = MemCfgData->PchMasterPowerGating;

    // HSIO PCIE signal compliance settings
    CopyMem (Fspmcfg->PchPcieHsioRxSetCtleEnable,           MemCfgData->PchPcieHsioRxSetCtleEnable,           sizeof(MemCfgData->PchPcieHsioRxSetCtleEnable));
    CopyMem (Fspmcfg->PchPcieHsioRxSetCtle,                 MemCfgData->PchPcieHsioRxSetCtle,                 sizeof(MemCfgData->PchPcieHsioRxSetCtle));
    CopyMem (Fspmcfg->PchPcieHsioTxGen1DownscaleAmpEnable,  MemCfgData->PchPcieHsioTxGen1DownscaleAmpEnable,  sizeof(MemCfgData->PchPcieHsioTxGen1DownscaleAmpEnable));
    CopyMem (Fspmcfg->PchPcieHsioTxGen1DownscaleAmp,        MemCfgData->PchPcieHsioTxGen1DownscaleAmp,        sizeof(MemCfgData->PchPcieHsioTxGen1DownscaleAmp));
    CopyMem (Fspmcfg->PchPcieHsioTxGen2DownscaleAmpEnable,  MemCfgData->PchPcieHsioTxGen2DownscaleAmpEnable,  sizeof(MemCfgData->PchPcieHsioTxGen2DownscaleAmpEnable));
    CopyMem (Fspmcfg->PchPcieHsioTxGen2DownscaleAmp,        MemCfgData->PchPcieHsioTxGen2DownscaleAmp,        sizeof(MemCfgData->PchPcieHsioTxGen2DownscaleAmp));
    CopyMem (Fspmcfg->PchPcieHsioTxGen3DownscaleAmpEnable,  MemCfgData->PchPcieHsioTxGen3DownscaleAmpEnable,  sizeof(MemCfgData->PchPcieHsioTxGen3DownscaleAmpEnable));
    CopyMem (Fspmcfg->PchPcieHsioTxGen3DownscaleAmp,        MemCfgData->PchPcieHsioTxGen3DownscaleAmp,        sizeof(MemCfgData->PchPcieHsioTxGen3DownscaleAmp));
    CopyMem (Fspmcfg->PchPcieHsioTxGen1DeEmphEnable,        MemCfgData->PchPcieHsioTxGen1DeEmphEnable,        sizeof(MemCfgData->PchPcieHsioTxGen1DeEmphEnable));
    CopyMem (Fspmcfg->PchPcieHsioTxGen1DeEmph,              MemCfgData->PchPcieHsioTxGen1DeEmph,              sizeof(MemCfgData->PchPcieHsioTxGen1DeEmph));
    CopyMem (Fspmcfg->PchPcieHsioTxGen2DeEmph3p5Enable,     MemCfgData->PchPcieHsioTxGen2DeEmph3p5Enable,     sizeof(MemCfgData->PchPcieHsioTxGen2DeEmph3p5Enable));
    CopyMem (Fspmcfg->PchPcieHsioTxGen2DeEmph3p5,           MemCfgData->PchPcieHsioTxGen2DeEmph3p5,           sizeof(MemCfgData->PchPcieHsioTxGen2DeEmph3p5));
    CopyMem (Fspmcfg->PchPcieHsioTxGen2DeEmph6p0Enable,     MemCfgData->PchPcieHsioTxGen2DeEmph6p0Enable,     sizeof(MemCfgData->PchPcieHsioTxGen2DeEmph6p0Enable));
    CopyMem (Fspmcfg->PchPcieHsioTxGen2DeEmph6p0,           MemCfgData->PchPcieHsioTxGen2DeEmph6p0,           sizeof(MemCfgData->PchPcieHsioTxGen2DeEmph6p0));

    // HSIO SATA signal compliance settings
    CopyMem (Fspmcfg->PchSataHsioRxGen1EqBoostMagEnable,    MemCfgData->PchSataHsioRxGen1EqBoostMagEnable,    sizeof(MemCfgData->PchSataHsioRxGen1EqBoostMagEnable));
    CopyMem (Fspmcfg->PchSataHsioRxGen1EqBoostMag,          MemCfgData->PchSataHsioRxGen1EqBoostMag,          sizeof(MemCfgData->PchSataHsioRxGen1EqBoostMag));
    CopyMem (Fspmcfg->PchSataHsioRxGen2EqBoostMagEnable,    MemCfgData->PchSataHsioRxGen2EqBoostMagEnable,    sizeof(MemCfgData->PchSataHsioRxGen2EqBoostMagEnable));
    CopyMem (Fspmcfg->PchSataHsioRxGen2EqBoostMag,          MemCfgData->PchSataHsioRxGen2EqBoostMag,          sizeof(MemCfgData->PchSataHsioRxGen2EqBoostMag));
    CopyMem (Fspmcfg->PchSataHsioRxGen3EqBoostMagEnable,    MemCfgData->PchSataHsioRxGen3EqBoostMagEnable,    sizeof(MemCfgData->PchSataHsioRxGen3EqBoostMagEnable));
    CopyMem (Fspmcfg->PchSataHsioRxGen3EqBoostMag,          MemCfgData->PchSataHsioRxGen3EqBoostMag,          sizeof(MemCfgData->PchSataHsioRxGen3EqBoostMag));
    CopyMem (Fspmcfg->PchSataHsioTxGen1DownscaleAmpEnable,  MemCfgData->PchSataHsioTxGen1DownscaleAmpEnable,  sizeof(MemCfgData->PchSataHsioTxGen1DownscaleAmpEnable));
    CopyMem (Fspmcfg->PchSataHsioTxGen1DownscaleAmp,        MemCfgData->PchSataHsioTxGen1DownscaleAmp,        sizeof(MemCfgData->PchSataHsioTxGen1DownscaleAmp));
    CopyMem (Fspmcfg->PchSataHsioTxGen2DownscaleAmpEnable,  MemCfgData->PchSataHsioTxGen2DownscaleAmpEnable,  sizeof(MemCfgData->PchSataHsioTxGen2DownscaleAmpEnable));
    CopyMem (Fspmcfg->PchSataHsioTxGen2DownscaleAmp,        MemCfgData->PchSataHsioTxGen2DownscaleAmp,        sizeof(MemCfgData->PchSataHsioTxGen2DownscaleAmp));
    CopyMem (Fspmcfg->PchSataHsioTxGen3DownscaleAmpEnable,  MemCfgData->PchSataHsioTxGen3DownscaleAmpEnable,  sizeof(MemCfgData->PchSataHsioTxGen3DownscaleAmpEnable));
    CopyMem (Fspmcfg->PchSataHsioTxGen3DownscaleAmp,        MemCfgData->PchSataHsioTxGen3DownscaleAmp,        sizeof(MemCfgData->PchSataHsioTxGen3DownscaleAmp));
    CopyMem (Fspmcfg->PchSataHsioTxGen1DeEmphEnable,        MemCfgData->PchSataHsioTxGen1DeEmphEnable,        sizeof(MemCfgData->PchSataHsioTxGen1DeEmphEnable));
    CopyMem (Fspmcfg->PchSataHsioTxGen1DeEmph,              MemCfgData->PchSataHsioTxGen1DeEmph,              sizeof(MemCfgData->PchSataHsioTxGen1DeEmph));
    CopyMem (Fspmcfg->PchSataHsioTxGen2DeEmphEnable,        MemCfgData->PchSataHsioTxGen2DeEmphEnable,        sizeof(MemCfgData->PchSataHsioTxGen2DeEmphEnable));
    CopyMem (Fspmcfg->PchSataHsioTxGen2DeEmph,              MemCfgData->PchSataHsioTxGen2DeEmph,              sizeof(MemCfgData->PchSataHsioTxGen2DeEmph));
    CopyMem (Fspmcfg->PchSataHsioTxGen3DeEmphEnable,        MemCfgData->PchSataHsioTxGen3DeEmphEnable,        sizeof(MemCfgData->PchSataHsioTxGen3DeEmphEnable));
    CopyMem (Fspmcfg->PchSataHsioTxGen3DeEmph,              MemCfgData->PchSataHsioTxGen3DeEmph,              sizeof(MemCfgData->PchSataHsioTxGen3DeEmph));

    //Vtd Config
    Fspmcfg->VtdDisable                 = !FeaturePcdGet (PcdVtdEnabled);
    if (FeaturePcdGet (PcdTccEnabled)) {
    TccCfgData = (TCC_CFG_DATA *)FindConfigDataByTag (CDATA_TCC_TAG);
    if ((TccCfgData != NULL) && (TccCfgData->TccEnable == 1)) {
      DEBUG ((DEBUG_INFO, "Enable VTd since TCC is enabled\n"));
      Fspmcfg->VtdDisable = 0;
      }
    }
    if (Fspmcfg->VtdDisable == 0) {
      Fspmcfg->VtdIgdEnable             = 1;
      Fspmcfg->VtdIopEnable             = 1;
      Fspmcfg->DmaControlGuarantee      = MemCfgData->DmaControlGuarantee;
      Fspmcfg->X2ApicOptOut             = MemCfgData->X2ApicOptOut;
      CopyMem(Fspmcfg->VtdBaseAddress, MemCfgData->VtdBaseAddress, sizeof(MemCfgData->VtdBaseAddress));
    }

    // Block Config
    Fspmcfg->SkipExtGfxScan             = MemCfgData->SkipExtGfxScan;
    Fspmcfg->WdtDisableAndLock          = MemCfgData->WdtDisableAndLock;

    //IBECC Config
    Fspmcfg->Ibecc                      = MemCfgData->Ibecc;
    Fspmcfg->IbeccParity                = MemCfgData->IbeccParity;
    Fspmcfg->IbeccOperationMode         = MemCfgData->IbeccOperationMode;
    Fspmcfg->IbeccErrorInj              = MemCfgData->IbeccErrorInj;
    for (Index = 0; Index < 0x8; Index++) {
      Fspmcfg->IbeccProtectedRegionEnable[Index] = MemCfgData->IbeccProtectedRegionEnable[Index];
      Fspmcfg->IbeccProtectedRegionBase[Index]   = MemCfgData->IbeccProtectedRegionBase[Index];
      Fspmcfg->IbeccProtectedRegionMask[Index]   = MemCfgData->IbeccProtectedRegionMask[Index];
    }

    // Debug Config
    Fspmcfg->DciEn                        = 1;
    Fspmcfg->DciModphyPg                  = 0;
    Fspmcfg->DciDbcMode                   = DciDbcNoChange; // 4
    Fspmcfg->CpuTraceHubMode              = TraceHubModeHostDebugger; // 2
    Fspmcfg->PchTraceHubMode              = TraceHubModeHostDebugger; // 2
    Fspmcfg->DciUsb3TypecUfpDbg           = 2;
    Fspmcfg->DebugInterfaceLockEnable     = TRUE;
    Fspmcfg->PlatformDebugConsent         = MemCfgData->PlatformDebugConsent;
    Fspmcfg->CpuTraceHubMemReg0Size       = MemCfgData->CpuTraceHubMemReg0Size;
    Fspmcfg->CpuTraceHubMemReg1Size       = MemCfgData->CpuTraceHubMemReg1Size;
    Fspmcfg->PchTraceHubMemReg0Size       = MemCfgData->PchTraceHubMemReg0Size;
    Fspmcfg->PchTraceHubMemReg1Size       = MemCfgData->PchTraceHubMemReg1Size;

    switch (MemCfgData->PlatformDebugConsent) {
      case ProbeTypeDisabled:
        Fspmcfg->DciEn                    = FALSE;
        Fspmcfg->CpuTraceHubMode          = TraceHubModeDisabled;
        Fspmcfg->PchTraceHubMode          = TraceHubModeDisabled;
        break;

      case ProbeTypeUsb2Dbc:
         Fspmcfg->DciDbcMode               = DciDbcBoth;
        break;

      case ProbeTypeUsb3Dbc:
         Fspmcfg->DciDbcMode               = DciDbcUsb3;
        break;

      case ProbeType2WireDciOob:
         Fspmcfg->CpuTraceHubMode          = TraceHubModeDisabled;
         Fspmcfg->PchTraceHubMode          = TraceHubModeDisabled;
        break;

      case ProbeTypeDciOob:
        Fspmcfg->DciModphyPg              = FALSE;
        break;

      case ProbeTypeManual:
        Fspmcfg->DciEn                    = MemCfgData->DciEn;
        Fspmcfg->DciModphyPg              = MemCfgData->DciModphyPg;
        Fspmcfg->DciUsb3TypecUfpDbg       = MemCfgData->DciUsb3TypecUfpDbg;
        Fspmcfg->DciDbcMode               = MemCfgData->DciDbcMode;
        Fspmcfg->CpuTraceHubMode          = MemCfgData->CpuTraceHubMode;
        Fspmcfg->PchTraceHubMode          = MemCfgData->PchTraceHubMode;
        Fspmcfg->DebugInterfaceEnable     = 0x0;
        break;

      default:
        break;
    }

      DEBUG ((DEBUG_INFO, "PlatformDebugConsent = %x\n",   Fspmcfg->PlatformDebugConsent));
      DEBUG ((DEBUG_INFO, "DciEn = %x\n",                  Fspmcfg->DciEn));
      DEBUG ((DEBUG_INFO, "DciModphyPg = %x\n",            Fspmcfg->DciModphyPg));
      DEBUG ((DEBUG_INFO, "DciUsb3TypecUfpDbg = %x\n",     Fspmcfg->DciUsb3TypecUfpDbg));
      DEBUG ((DEBUG_INFO, "DciDbcMode = %x\n",             Fspmcfg->DciDbcMode));
      DEBUG ((DEBUG_INFO, "DebugInterfaceEnable = %x\n",   Fspmcfg->DebugInterfaceEnable));
      DEBUG ((DEBUG_INFO, "CpuTraceHubMode = %x\n",        Fspmcfg->CpuTraceHubMode));
      DEBUG ((DEBUG_INFO, "CpuTraceHubMemReg0Size = %x\n", Fspmcfg->CpuTraceHubMemReg0Size));
      DEBUG ((DEBUG_INFO, "CpuTraceHubMemReg1Size = %x\n", Fspmcfg->CpuTraceHubMemReg1Size));
      DEBUG ((DEBUG_INFO, "PchTraceHubMode = %x\n",        Fspmcfg->PchTraceHubMode));
      DEBUG ((DEBUG_INFO, "PchTraceHubMemReg0Size = %x\n", Fspmcfg->PchTraceHubMemReg0Size));
      DEBUG ((DEBUG_INFO, "PchTraceHubMemReg1Size = %x\n", Fspmcfg->PchTraceHubMemReg1Size));
  }

  // IGD config data
  GfxCfgData = (GRAPHICS_CFG_DATA *)FindConfigDataByTag (CDATA_GRAPHICS_TAG);
  if (GfxCfgData != NULL) {
    DEBUG ((DEBUG_INFO, "Load Graphics Cfg Data\n"));
    Fspmcfg->IgdDvmt50PreAlloc          = GfxCfgData->IgdDvmt50PreAlloc;
    Fspmcfg->ApertureSize               = GfxCfgData->ApertureSize;
    Fspmcfg->GttSize                    = GfxCfgData->GttSize;
    Fspmcfg->InternalGfx                = GfxCfgData->InternalGfx;
    Fspmcfg->PrimaryDisplay             = GfxCfgData->PrimaryDisplay;
    Fspmcfg->GttMmAdr                   = GfxCfgData->GttMmAdr;
    Fspmcfg->GmAdr                      = GfxCfgData->GmAdr;
    Fspmcfg->PsmiRegionSize             = GfxCfgData->PsmiRegionSize;
    Fspmcfg->DismSize                   = GfxCfgData->DismSize;
    Fspmcfg->GtPsmiSupport              = GfxCfgData->GtPsmiSupport;

  // PCH HDA config data
    Fspmcfg->PchHdaEnable               = GfxCfgData->PchHdaEnable;
    Fspmcfg->PchHdaDspEnable            = GfxCfgData->PchHdaDspEnable;
    Fspmcfg->PchHdaIDispLinkFrequency   = GfxCfgData->PchHdaIDispLinkFrequency;
    Fspmcfg->PchHdaIDispLinkTmode       = GfxCfgData->PchHdaIDispLinkTmode;
    Fspmcfg->PchHdaIDispCodecDisconnect = GfxCfgData->PchHdaIDispCodecDisconnect;
    Fspmcfg->PchHdaDspUaaCompliance     = GfxCfgData->PchHdaDspUaaCompliance;

    switch (GfxCfgData->PchHdAudioLinkMode) {
      case HDAUDIO_LINK_MODE_HDA:
        DEBUG ((DEBUG_INFO, "HDA Policy: HD Audio Link Mode\n"));
        Fspmcfg->PchHdaAudioLinkHdaEnable               = 1;
        for (Index = 0; Index < PCH_MAX_HDA_DMIC_LINK_NUM; Index++) {
          Fspmcfg->PchHdaAudioLinkDmicEnable[Index]     = 1;
        }
        // Override default policy setting (PeiPchPolicyLib) due to conflict between Hda and Sndw0
        Fspmcfg->PchHdaAudioLinkSndwEnable[0]           = 0;
      break;
      case HDAUDIO_LINK_MODE_I2S_SSP:
        DEBUG ((DEBUG_INFO, "HDA Policy: SSP/I2S Link Mode\n"));
        for (Index = 0; Index < PCH_MAX_HDA_SSP_LINK_NUM; Index++) {
          Fspmcfg->PchHdaAudioLinkSspEnable[Index]      = 0;
        }
        if (PchSciSupported != 1) {
          Fspmcfg->PchHdaAudioLinkSspEnable[0]          = 1;
        }
        for (Index = 0; Index < PCH_MAX_HDA_DMIC_LINK_NUM; Index++) {
          if (PchSciSupported) {
            Fspmcfg->PchHdaAudioLinkDmicEnable[Index]   = 0;
          }
        }
        // Override default policy setting (PeiPchPolicyLib) due to conflict between Hda and Ssp0 links
        Fspmcfg->PchHdaAudioLinkHdaEnable               = 0;
        break;
      case HDAUDIO_LINK_MODE_SNDW:
        DEBUG ((DEBUG_INFO, "HDA Policy: SoundWire Link Mode\n"));
        // Override default policy setting (PeiPchPolicyLib) due to conflict between Hda and Sndw0
        Fspmcfg->PchHdaAudioLinkHdaEnable               = 0;
        for (Index = 0; Index < PCH_MAX_HDA_SNDW_LINK_NUM; Index++) {
          Fspmcfg->PchHdaAudioLinkSndwEnable[Index]     = 1;
        }
        Fspmcfg->PchHdaAudioLinkHdaEnable               = 0;
        // Override default policy setting (PeiPchPolicyLib) due to conflict between Hda and Dmic0/1 with Sndw4/3
        Fspmcfg->PchHdaAudioLinkHdaEnable               = 0;
        for (Index = 0; Index < PCH_MAX_HDA_DMIC_LINK_NUM; Index++) {
          Fspmcfg->PchHdaAudioLinkDmicEnable[Index]     = 0;
        }
        break;
      case HDAUDIO_LINK_MODE_ADVANCED:
        DEBUG ((DEBUG_INFO, "HDA Policy: Advanced Link Mode Configuration\n"));
        Fspmcfg->PchHdaAudioLinkHdaEnable               = GfxCfgData->PchHdaAudioLinkHdaEnable;
        for (Index = 0; Index < PCH_MAX_HDA_DMIC_LINK_NUM; Index++) {
          Fspmcfg->PchHdaAudioLinkDmicEnable[Index]     = GfxCfgData->PchHdaAudioLinkDmicEnable[Index];
        }
        for (Index = 0; Index < PCH_MAX_HDA_SSP_LINK_NUM; Index++) {
          Fspmcfg->PchHdaAudioLinkSspEnable[Index]      = GfxCfgData->PchHdaAudioLinkSspEnable[Index];
        }
        for (Index = 0; Index < PCH_MAX_HDA_SNDW_LINK_NUM; Index++) {
          Fspmcfg->PchHdaAudioLinkSndwEnable[Index]     =  GfxCfgData->PchHdaAudioLinkSndwEnable[Index];
        }
        break;
      default:
        DEBUG ((DEBUG_INFO, "HDA Policy Default: HD Audio Link Mode\n"));
        //UPDATE_POLICY (((FSPM_UPD *) FspmUpd)->FspmConfig.PchHdaAudioLinkHdaEnable,   HdaPreMemConfig->AudioLinkHda.Enable,     TRUE);
          Fspmcfg->PchHdaAudioLinkHdaEnable         = 1;
        for (Index = 0; Index < PCH_MAX_HDA_DMIC_LINK_NUM; Index++) {
          Fspmcfg->PchHdaAudioLinkDmicEnable[Index] = 1;
        }
        break;
    }

    Fspmcfg->PchHdaAudioLinkDmicClkAPinMux[0] = GPIO_VER3_MUXING_DMIC0_CLKA_GPP_R6;
    Fspmcfg->PchHdaAudioLinkDmicClkBPinMux[0] = GPIO_VER3_MUXING_DMIC0_CLKB_GPP_G13;
    Fspmcfg->PchHdaAudioLinkDmicDataPinMux[0] = GPIO_VER3_MUXING_DMIC0_DATA_GPP_R7;
    Fspmcfg->PchHdaAudioLinkDmicClkAPinMux[1] = GPIO_VER3_MUXING_DMIC1_CLKA_GPP_R4;
    Fspmcfg->PchHdaAudioLinkDmicClkBPinMux[1] = GPIO_VER3_MUXING_DMIC1_CLKB_GPP_G14;
    Fspmcfg->PchHdaAudioLinkDmicDataPinMux[1] = GPIO_VER3_MUXING_DMIC1_DATA_GPP_R5;

  // PCH HDA Config data end

    Fspmcfg->DisableTeIgd         = 0x01;
    Fspmcfg->HeciCommunication2   = 0x01;

  } else {
    DEBUG ((DEBUG_INFO, "Failed to find GFX CFG!\n"));
  }

  // Enable s0ix by default
  PLAT_FEAT.S0ixEnable = 1;

  if (FeaturePcdGet (PcdTccEnabled)) {
    TccModePreMemConfig (FspmUpd);
  }

  UpdateS0ixStatus ();

  if (S0IX_STATUS() == 1) {
    // configure FSP-M upd for s0ix
    Fspmcfg->TcssXdciEn = 0;
  }
}

/**
  Initialize the SPI controller.

  @retval    None
**/
VOID
SpiControllerInitialize (
  VOID
)
{
  /* Enable PCI config space for SPI device */
  PciWrite32(
             PCI_LIB_ADDRESS (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_SPI,
                    PCI_FUNCTION_NUMBER_PCH_SPI,
                    R_SPI_CFG_BAR0
                    ),
             SPI_TEMP_MEM_BASE_ADDRESS
            );
  PciWrite8(
             PCI_LIB_ADDRESS (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_SPI,
                    PCI_FUNCTION_NUMBER_PCH_SPI,
                    4
                    ),
              6
            );
  SpiConstructor ();
}

/**
  Determine if this is firmware update mode.

  This function will determine if we have to put system in firmware update mode.

  @retval  EFI_SUCCESS           The operation completed successfully.
  @retval  others                There is error happening.
**/
BOOLEAN
IsFirmwareUpdate ()
{
  //
  // Check if state machine is set to capsule processing mode.
  //
  if (CheckStateMachine (NULL) == EFI_SUCCESS) {
    return TRUE;
  }

  //
  // Check if platform firmware update trigger is set.
  //
  if (IoRead32 (ACPI_BASE_ADDRESS + R_ACPI_IO_OC_WDT_CTL) & BIT16) {
    return TRUE;
  }

  return FALSE;
}

/**
  Read RTC content through its registers.

  @param  Address  Address offset of RTC.

  @return The data of UINT8 type read from RTC.
**/
UINT8
RtcRead (
  IN  UINT8 Address
  )
{
  IoWrite8 (R_RTC_IO_INDEX, (UINT8) (Address | (UINT8) (IoRead8 (R_RTC_IO_INDEX) & 0x80)));
  return IoRead8 (R_RTC_IO_TARGET);
}

/**
  Write RTC through its registers.

  @param  Address  Address offset of RTC.
  @param  Data     The content you want to write into RTC.

**/
VOID
RtcWrite (
  IN  UINT8   Address,
  IN  UINT8   Data
  )
{
  IoWrite8 (R_RTC_IO_INDEX, (UINT8) (Address | (UINT8) (IoRead8 (R_RTC_IO_INDEX) & 0x80)));
  IoWrite8 (R_RTC_IO_TARGET, Data);
}

/**
  Initialize RTC.

  @retval    None.

**/
VOID
RtcInit (
  VOID
  )
{
  UINT32  Bar;
  UINT8   PmConf1;
  UINT8   Data8;

  Bar     = MmioRead32 (MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PMC_CFG_BASE)) & ~0x0F;
  PmConf1 = MmioRead8 (Bar + R_PMC_PWRM_GEN_PMCON_B);

  RtcRead (R_RTC_IO_REGA);

  if ((PmConf1 & B_PMC_PWRM_GEN_PMCON_B_RTC_PWR_STS) != 0) {
    DEBUG ((DEBUG_INFO, "Initialize RTC with default values\n"));
    RtcWrite (R_RTC_IO_REGA, 0x66);

    Data8 = RtcRead (R_RTC_IO_REGB);
    Data8 &= ~(BIT2);
    Data8 |= (BIT7 | BIT1);
    RtcWrite (R_RTC_IO_REGB, Data8);

    RtcWrite (R_RTC_IO_REGA, 0x26);

    Data8 = RtcRead (R_RTC_IO_REGB);
    Data8 &= ~(BIT7);
    RtcWrite (R_RTC_IO_REGB, Data8);

    RtcWrite (0x0F, 0x00);

    RtcWrite (R_RTC_IO_REGD, 0x00);
    RtcRead (R_RTC_IO_REGD);

    RtcWrite (R_RTC_HOURSALARM, 0x00);
    RtcWrite (R_RTC_MINUTESALARM, 0x00);
    RtcWrite (R_RTC_SECONDSALARM, 0x00);

    // 11/11/2011 11:11:11
    RtcWrite (RTC_SECONDS, 0x11);
    RtcWrite (RTC_MINUTES, 0x11);
    RtcWrite (RTC_HOURS, 0x11);
    RtcWrite (RTC_DAY_OF_MONTH, 0x11);
    RtcWrite (RTC_MONTH, 0x11);
    RtcWrite (RTC_YEAR, 0x11);
    RtcWrite (B_RTC_CENTURY, 0x20);

    PmConf1 &= ~(B_PMC_PWRM_GEN_PMCON_B_RTC_PWR_STS);
    MmioWrite8 (Bar + R_PMC_PWRM_GEN_PMCON_B, PmConf1);
  }
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
  LdrFeatures |= FeaturePcdGet (PcdAcpiEnabled)?FEATURE_ACPI:0;
  LdrFeatures |= FeaturePcdGet (PcdVerifiedBootEnabled)?FEATURE_VERIFIED_BOOT:0;
  LdrFeatures |= FeaturePcdGet (PcdMeasuredBootEnabled)?FEATURE_MEASURED_BOOT:0;

  // Disable feature by configuration data.
  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag(CDATA_FEATURES_TAG);
  if (FeaturesCfgData != NULL) {
    if (FeaturesCfgData->Features.Acpi == 0) {
      LdrFeatures &= ~FEATURE_ACPI;
    }

    if (FeaturesCfgData->Features.MeasuredBoot == 0) {
      LdrFeatures &= ~FEATURE_MEASURED_BOOT;
    }
  }

  // Disable features by boot guard profile
  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if (PlatformData != NULL) {
    HeciBaseAddress = MM_PCI_ADDRESS (
                        0x0,
                        22,
                        0x0,
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
  Detect board and configure PlatformID.

  @retval  none
**/
VOID
PlatformIdInitialize (
  IN  VOID
  )
{
  UINT16 PlatformId;
  UINT16 BoardId;

  PlatformId = GetPlatformId();
  if (PlatformId == 0){
    GetBoardIdFromSmbus (&BoardId);
    SetPlatformId (BoardId);
  }
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

  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
  BootMode = GetBootMode();

  if((PlatformData != NULL) && ((PlatformData->BtGuardInfo.MeasuredBoot == 1) &&
    (PlatformData->BtGuardInfo.DisconnectAllTpms == 0) &&
    ((PlatformData->BtGuardInfo.TpmType == dTpm20) || (PlatformData->BtGuardInfo.TpmType == Ptt)))){

    // Initialize TPM if it has not already been initialized by BootGuard component (i.e. ACM)
    Status = TpmInit(PlatformData->BtGuardInfo.BypassTpmInit, BootMode);

    if (EFI_SUCCESS == Status) {
      if (BootMode != BOOT_ON_S3_RESUME) {

        // Create and add BootGuard Event logs in TCG Event log
        CreateTpmEventLog (PlatformData->BtGuardInfo.TpmType);
      }
    } else {
      CpuHalt ("Tpm Initialization failed !!\n");
    }
  } else {
    DisableTpm();
  }
}

/**
  Read the current platform state from PM status reg.

  @retval  Bootmode current power state.
**/
UINT8
GetPlatformPowerState (
  VOID
  )
{
  UINT8         BootMode;
  UINT32        PmconA;

  PmconA = MmioRead32 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_A);

  //
  // Clear PWRBTNOR_STS
  //
  if (IoRead16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS) & B_ACPI_IO_PM1_STS_PRBTNOR) {
    IoWrite16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS, B_ACPI_IO_PM1_STS_PRBTNOR);
  }

  //
  // If Global Reset Status, Power Failure. Host Reset Status bits are set, return S5 State
  //
  if ((PmconA & (B_PMC_PWRM_GEN_PMCON_A_GBL_RST_STS | B_PMC_PWRM_GEN_PMCON_A_PWR_FLR | B_PMC_PWRM_GEN_PMCON_A_HOST_RST_STS)) != 0) {
    return BOOT_WITH_FULL_CONFIGURATION;
  }

  BootMode = BOOT_WITH_FULL_CONFIGURATION;
  if (IoRead16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS) & B_ACPI_IO_PM1_STS_WAK) {
    switch (IoRead16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT) & B_ACPI_IO_PM1_CNT_SLP_TYP) {
      case V_ACPI_IO_PM1_CNT_S3:
        BootMode = BOOT_ON_S3_RESUME;
        break;
      case V_ACPI_IO_PM1_CNT_S4:
        BootMode = BOOT_ON_S4_RESUME;
        break;
      case V_ACPI_IO_PM1_CNT_S5:
        BootMode = BOOT_ON_S5_RESUME;
        break;
      default:
        BootMode = BOOT_WITH_FULL_CONFIGURATION;
        break;
    }
  }

  ///
  /// Clear the PWRBTN_EN, it causes SMI# otherwise (SCI_EN is 0)
  ///
  IoAndThenOr32 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS, (UINT32)~B_ACPI_IO_PM1_EN_PWRBTN_EN, B_ACPI_IO_PM1_STS_WAK );

  //Clear RTC_EN bit as it cause SMI storm
  IoAnd32 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS, (UINT32)~B_ACPI_IO_PM1_EN_RTC_EN);

  if ((MmioRead8 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_B) & B_PMC_PWRM_GEN_PMCON_B_RTC_PWR_STS) != 0) {
    BootMode = BOOT_WITH_FULL_CONFIGURATION;

    ///
    /// Clear Wake Status and Sleep Type
    ///
    IoWrite16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS, B_ACPI_IO_PM1_STS_WAK);
    IoAndThenOr16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT, (UINT16) ~B_ACPI_IO_PM1_CNT_SLP_TYP, V_ACPI_IO_PM1_CNT_S0);
  }

  return BootMode;
}

/**
  Initialize Board specific things in Stage1 Phase

  @param[in]  InitPhase            Indicates a board init phase to be initialized

  @retval None
**/
VOID
BoardInit (
  IN  BOARD_INIT_PHASE  InitPhase
)
{
  UINTN PmcBase;
  PLT_DEVICE_TABLE  *PltDeviceTable;

  PmcBase = MmPciBase (
            DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_PMC,
            PCI_FUNCTION_NUMBER_PCH_PMC
            );

  switch (InitPhase) {
  case PreConfigInit:
DEBUG_CODE_BEGIN();
    UINT32  Data;

    Data = *(UINT32 *)(UINTN)(0xFED30328);
    DEBUG ((DEBUG_ERROR, "[Boot Guard] AcmStatus : 0x%08X\n", Data));

    Data = *(UINT32 *)(UINTN)(0xFED300A4);
    DEBUG ((DEBUG_ERROR, "[Boot Guard] BootStatus: 0x%08X\n", Data));

    if ((Data & (BIT31 | BIT30)) != BIT31) {
      DEBUG ((DEBUG_ERROR, "[Boot Guard] Boot Guard Failed or is Disabled!\n", Data));
    } else {
      DEBUG ((DEBUG_ERROR, "[Boot Guard] Boot Guard is Enabled Successfully.\n", Data));
    }
DEBUG_CODE_END();
    PltDeviceTable = (PLT_DEVICE_TABLE *)AllocatePool (sizeof (PLT_DEVICE_TABLE) + sizeof (mPlatformDevices));
    if (PltDeviceTable != NULL) {
      PltDeviceTable->DeviceNumber = sizeof (mPlatformDevices) /sizeof (PLT_DEVICE);
      CopyMem (PltDeviceTable->Device, mPlatformDevices, sizeof (mPlatformDevices));
      SetDeviceTable (PltDeviceTable);
    } else {
      DEBUG ((DEBUG_ERROR, "Out of resource for PltDeviceTable\n"));
    }
    SpiControllerInitialize ();
    SetBootMode (IsFirmwareUpdate () ? BOOT_ON_FLASH_UPDATE : GetPlatformPowerState ());
    break;
  case PostConfigInit:
    PlatformIdInitialize();
    PlatformNameInit ();
    RtcInit();
    PlatformFeaturesInit ();
    break;
  case PreMemoryInit:
    //
    // Set the DISB bit in PCH (DRAM Initialization Scratchpad Bit)
    // prior to starting DRAM Initialization Sequence.
    //
    MmioOr32 (PmcBase + R_PMC_PWRM_GEN_PMCON_A, B_PMC_PWRM_GEN_PMCON_A_DISB);
    ConfigureGpio (CDATA_NO_TAG, ARRAY_SIZE(mGpioTablePreMemEhl), (UINT8*)mGpioTablePreMemEhl);
    break;
  case PostMemoryInit:
    //
    // Clear the DISB bit after completing DRAM Initialization Sequence
    //
    MmioAnd32 (PmcBase + R_PMC_PWRM_GEN_PMCON_A, 0);
    UpdateMemoryInfo ();
    break;
  case PreTempRamExit:
    break;
  case PostTempRamExit:
    if (MEASURED_BOOT_ENABLED()) {
      TpmInitialize();
    }
    break;
  default:
    break;
  }
}

/**
  Search for the saved MrcParam to initialize Memory for fastboot

  @retval Found MrcParam or NULL
**/
VOID *
EFIAPI
FindNvsData (
  VOID
)
{
  UINT32            MrcData;
  DYNAMIC_CFG_DATA *DynCfgData;
  EFI_STATUS        Status;

  DynCfgData = (DYNAMIC_CFG_DATA *)FindConfigDataByTag (CDATA_DYNAMIC_TAG);
  if (DynCfgData != NULL) {
    //
    // When enabled, memory training is enforced even if consistent memory parameters are available
    //
    if (DynCfgData->MrcTrainingEnforcement) {
      DEBUG ((DEBUG_INFO, "Training Enforcement enabled, hence providing NULL pointer for NVS Data!\n"));
      return NULL;
    }
  } else {
    DEBUG ((DEBUG_INFO, "Failed to find dynamic CFG!\n"));
  }

  Status = GetComponentInfo (FLASH_MAP_SIG_MRCDATA, &MrcData, NULL);
  if (EFI_ERROR(Status)) {
    return NULL;
  }

  if (*(UINT32 *)(UINTN)MrcData == 0xFFFFFFFF) {
    return NULL;
  } else {
    return (VOID *)(UINTN)MrcData;
  }
}

/**
  Load the configuration data blob from media into destination buffer.
  Currently for ICL, we support the sources: PDR, BIOS for external Cfgdata.

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
