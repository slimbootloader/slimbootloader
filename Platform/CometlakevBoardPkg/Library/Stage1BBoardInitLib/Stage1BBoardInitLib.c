/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/BoardInitLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/SpiFlashLibCml.h>
#include <Library/ConfigDataLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <ConfigDataDefs.h>
#include <FspmUpd.h>
#include <Library/DebugLib.h>
#include <Library/MmcAccessLib.h>
#include <IndustryStandard/Pci30.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/BoardSupportLib.h>
#include <RegAccess.h>
#include <Library/CryptoLib.h>
#include <Library/PchInfoLib.h>
#include <Library/SocInitLib.h>
#include <Library/TpmLib.h>
#include <Library/BootGuardLib.h>
#include <Library/SgxLib.h>
#include <PciePreMemConfig.h>
#include <PlatformData.h>
#include "PreMemGpioTables.h"
#include <Library/ResetSystemLib.h>
#include <Library/FirmwareUpdateLib.h>

CONST PLT_DEVICE  mPlatformDevices[]= {
  {{0x00001400}, OsBootDeviceUsb   , 0 },
  {{0x00001700}, OsBootDeviceSata  , 0 },
  {{0x00001A00}, OsBootDeviceEmmc  , 0 },
  {{0x00010000}, OsBootDeviceNvme  , 0 },
  {{0x00001F05}, OsBootDeviceSpi   , 0 }
};

CONST UINT16 mRcompResistor[3] = { 121, 75, 100 };
CONST UINT16 mRcompTarget[5] = { 60, 26, 20, 20, 26 };

VOID
GetBoardId (
  OUT UINT8 *BoardId
);

/**
  Set the debug print error level fron CFG data.

**/
VOID *
EFIAPI
SetDebugLevelFromCfgData (
  VOID
  )
{
  GEN_CFG_DATA   *GenCfgData;

  GenCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
  if (GenCfgData != NULL) {
    DEBUG ((DEBUG_INFO, "Load general parameter from CfgData.\n"));
    SetDebugPrintErrorLevel (GenCfgData->DebugPrintLevel);
  }
  return 0;
}

/**
  Update FSP-M UPD config data

  @param  FspmUpdPtr            The pointer to the FSP-M UPD to be updated.

**/
VOID
EFIAPI
UpdateFspConfig (
  VOID     *FspmUpdPtr
)
{
  FSPM_UPD                        *FspmUpd;
  FSP_M_CONFIG                    *Fspmcfg;
  FSP_M_TEST_CONFIG               *FspmcfgTest;
  MEMORY_CFG_DATA                 *MemCfgData;
  GPU_CFG_DATA                    *GpuCfgData;
  PEG_GPIO_DATA                   *PegGpioData;

  FspmUpd       = (FSPM_UPD *)FspmUpdPtr;
  Fspmcfg       = &FspmUpd->FspmConfig;
  FspmcfgTest   = &FspmUpd->FspmTestConfig;

  Fspmcfg->MemorySpdDataLen = 512;
  CopyMem (&Fspmcfg->RcompTarget, mRcompTarget, sizeof(mRcompTarget));

  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
  if (MemCfgData == NULL) {
    CpuHalt ("Failed to find memory CFGDATA!");
    return;
  }

  DEBUG ((DEBUG_INFO, "Load memory parameters from CfgData.\n"));

  CopyMem (&Fspmcfg->SpdAddressTable, MemCfgData->SpdAddressTable, sizeof(MemCfgData->SpdAddressTable));
  //Dq/Dqs Mapping arrays
  CopyMem (&Fspmcfg->DqByteMapCh0, MemCfgData->DqByteMapCh0, sizeof(MemCfgData->DqByteMapCh0));
  CopyMem (&Fspmcfg->DqByteMapCh1, MemCfgData->DqByteMapCh1, sizeof(MemCfgData->DqByteMapCh1));
  CopyMem (&Fspmcfg->DqsMapCpu2DramCh0, MemCfgData->DqsMapCpu2DramCh0, sizeof(MemCfgData->DqsMapCpu2DramCh0));
  CopyMem (&Fspmcfg->DqsMapCpu2DramCh1, MemCfgData->DqsMapCpu2DramCh1, sizeof(MemCfgData->DqsMapCpu2DramCh1));
  Fspmcfg->DqPinsInterleaved      = MemCfgData->DqPinsInterleaved;
  //
  // Tseg 4MB is enough for both debug/release build with SBL
  //
  Fspmcfg->TsegSize               = 0x00400000;
  Fspmcfg->MmioSize               = MemCfgData->MmioSize;
  Fspmcfg->RMT                    = MemCfgData->RMT;
  FspmcfgTest->BdatEnable         = MemCfgData->BdatEnable;
  FspmcfgTest->BdatTestType       = MemCfgData->BdatTestType;
  CopyMem (&Fspmcfg->RcompResistor, mRcompResistor, sizeof(mRcompResistor));

  SetDebugLevelFromCfgData ();

  GpuCfgData = (GPU_CFG_DATA *)FindConfigDataByTag (CDATA_GPU_TAG);
  if (GpuCfgData == NULL) {
    DEBUG ((DEBUG_INFO, "Failed to find GFX CFG!\n"));
    return;
  }

  DEBUG ((DEBUG_INFO, "Load Graphics Cfg Data\n"));
  Fspmcfg->IgdDvmt50PreAlloc          = GpuCfgData->IgdDvmt50PreAlloc;
  Fspmcfg->ApertureSize               = GpuCfgData->ApertureSize;
  Fspmcfg->GttSize                    = GpuCfgData->GttSize;
  Fspmcfg->InternalGfx                = GpuCfgData->InternalGfx;
  Fspmcfg->PrimaryDisplay             = GpuCfgData->PrimaryDisplay;
  Fspmcfg->RMT                        = MemCfgData->RMT;
  FspmcfgTest->BdatEnable             = MemCfgData->BdatEnable;
  FspmcfgTest->BdatTestType           = MemCfgData->BdatTestType;
  Fspmcfg->FClkFrequency              = MemCfgData->FClkFrequency;
  Fspmcfg->UserBd                     = MemCfgData->UserBd;
  Fspmcfg->EnableC6Dram               = 1;
  Fspmcfg->TxtDprMemorySize           = 0x400000;
  Fspmcfg->CpuRatio                   = 0;
  Fspmcfg->CaVrefConfig               = 0x2;
  Fspmcfg->PlatformMemorySize         = 0x169EB000;
  Fspmcfg->PcdSerialDebugLevel        = 3;
  Fspmcfg->SafeMode                   = 0;
  Fspmcfg->PeciC10Reset               = 0;

    ASSERT (sizeof (PEG_GPIO_DATA) == sizeof (Fspmcfg->PegGpioData));
    ZeroMem (Fspmcfg->PegGpioData, sizeof (Fspmcfg->PegGpioData));
    PegGpioData                           = (PEG_GPIO_DATA *) Fspmcfg->PegGpioData;
    PegGpioData->GpioSupport              = 1;
    PegGpioData->SaPeg0ResetGpio.Active   = 0;  // Active Low
    PegGpioData->SaPeg0ResetGpio.GpioPad  = GPIO_CNL_H_GPP_F22;

    ZeroMem (Fspmcfg->PegGen3RxCtlePeaking, sizeof (Fspmcfg->PegGen3RxCtlePeaking));

    FspmcfgTest->Peg3Gen3EqPh3Method      = 0;  // Auto

  if (!(UpdateFspmSgxConfig (FspmUpd))) {
    DEBUG ((DEBUG_INFO, "FSP-M variables for Intel(R) SGX were NOT updated.\n"));
  }

  Fspmcfg->PlatformDebugConsent = MemCfgData->PlatformDebugConsent;
  Fspmcfg->PchTraceHubMode      = MemCfgData->PchTraceHubMode;

  //
  // FSPM UPDs from BIOS
  //

  // FspmConfig->StackSize = 0x10000;
  // FspmConfig->BootMode = 0x4;
  // FspmConfig->PlatformMemorySize = 0x17979000;
  // FspmConfig->BiosGuard = 0x1;
  // FspmConfig->BiosGuardToolsInterface = 0x1;
  // FspmConfig->BiosAcmBase = 0xffd51000;
  // FspmConfig->PchNumRsvdSmbusAddresses = 0x4;
  // FspmConfig->RsvdSmbusAddressTablePtr = 0xfef072bc;
  // FspmConfig->BiosSize = 0x3000;
  // FspmConfig->SaSystemAgentClockGatingEnable = 0x0;
  // FspmConfig->SaPciePllShutdownEnable = 0x0;
  // FspmConfig->SaSvDMI_nFTS = 0x0;
  // FspmConfig->SanFTS_auto = 0x0;
  // FspmConfig->SaSvPEG_nFTS[0] = 0x0;
  // FspmConfig->SaSvPEG_nFTS[1] = 0x0;
  // FspmConfig->SaSvPEG_nFTS[2] = 0x0;
  // FspmConfig->SaSvPEG_nFTS[3] = 0x0;
  // FspmConfig->SaSvPEG_gen3_ccFTS[0] = 0x0;
  // FspmConfig->SaSvPEG_gen3_ccFTS[1] = 0x0;
  // FspmConfig->SaSvPEG_gen3_ccFTS[2] = 0x0;
  // FspmConfig->SaSvPEG_gen3_ccFTS[3] = 0x0;
  // FspmConfig->SaSvPEG_gen3_nccFTS[0] = 0x0;
  // FspmConfig->SaSvPEG_gen3_nccFTS[1] = 0x0;
  // FspmConfig->SaSvPEG_gen3_nccFTS[2] = 0x0;
  // FspmConfig->SaSvPEG_gen3_nccFTS[3] = 0x0;
  // FspmConfig->SanFTS_gen3_auto = 0x0;
  // FspmConfig->SaSvScramblerDmi = 0x0;
  // FspmConfig->SaSvScramblerPeg[0] = 0x0;
  // FspmConfig->SaSvScramblerPeg[1] = 0x0;
  // FspmConfig->SaSvScramblerPeg[2] = 0x0;
  // FspmConfig->SaSvScramblerPeg[3] = 0x0;
  // FspmConfig->SaSvDmiSerr = 0x0;
  // FspmConfig->SaSvScramblerPegGen3[0] = 0x0;
  // FspmConfig->SaSvScramblerPegGen3[1] = 0x0;
  // FspmConfig->SaSvScramblerPegGen3[2] = 0x0;
  // FspmConfig->SaSvScramblerPegGen3[3] = 0x0;
  // FspmConfig->SaSvPegSerr[0] = 0x0;
  // FspmConfig->SaSvPegSerr[1] = 0x0;
  // FspmConfig->SaSvPegSerr[2] = 0x0;
  // FspmConfig->SaSvPegSerr[3] = 0x0;
  // FspmConfig->SaTestTxClkGating = 0x0;
  // FspmConfig->SaTestRxClkGating = 0x0;
  // FspmConfig->SaTestLowPwrMode = 0x0;
  // FspmConfig->SaSrMode = 0x0;
  // FspmConfig->SaPcieVCmCmpLim = 0x0;
  // FspmConfig->SaPcieVCmPLim = 0x0;
  // FspmConfig->SaPcieVCmNpLim = 0x0;
  // FspmConfig->PrefetchNonPrefetchRatio = 0x0;
  // FspmConfig->DmaPassThrough = 0x0;
  // FspmConfig->CCHit2pend = 0x0;
  // FspmConfig->ContextCacheDis = 0x0;
  // FspmConfig->L1Disable = 0x0;
  // FspmConfig->L2Disable = 0x0;
  // FspmConfig->L3Disable = 0x0;
  // FspmConfig->L1Hit2PendDis = 0x0;
  // FspmConfig->L3Hit2PendDis = 0x0;
  // FspmConfig->InvQueueCohDis = 0x0;
  // FspmConfig->SuperPageCap = 0x0;
  // FspmConfig->QueueInvCapDis = 0x0;
  // FspmConfig->TestIntrRemapCapDis = 0x0;
  // FspmConfig->SnoopControl = 0x0;
  // FspmConfig->RemapReverseCtrl = 0x0;
  // FspmConfig->PchTestDmiUpPortTranPreset[0] = 0x7;
  // FspmConfig->PchTestDmiUpPortTranPreset[1] = 0x7;
  // FspmConfig->PchTestDmiUpPortTranPreset[2] = 0x7;
  // FspmConfig->PchTestDmiUpPortTranPreset[3] = 0x7;
  // FspmConfig->HeciCommunication = 0x1;

  FspmUpd->FspmArchUpd.StackBase = 0xfef2ff00;
  FspmUpd->FspmArchUpd.StackSize = 0x40000;
  // Fspmcfg->GmAdr = 0xc0000000;
  // Fspmcfg->GttMmAdr = 0xdf000000;
  Fspmcfg->DqByteMapCh0[2] = 0xf;
  Fspmcfg->DqByteMapCh0[4] = 0xff;
  Fspmcfg->DqByteMapCh0[5] = 0x0;
  Fspmcfg->DqByteMapCh0[6] = 0x0;
  Fspmcfg->DqByteMapCh0[8] = 0x0;
  Fspmcfg->DqByteMapCh0[10] = 0x0;
  Fspmcfg->DqByteMapCh1[0] = 0xf;
  Fspmcfg->DqByteMapCh1[1] = 0xf0;
  Fspmcfg->DqByteMapCh1[2] = 0xf;
  Fspmcfg->DqByteMapCh1[3] = 0xf0;
  Fspmcfg->DqByteMapCh1[4] = 0xff;
  Fspmcfg->DqByteMapCh1[5] = 0x0;
  Fspmcfg->DqByteMapCh1[6] = 0x0;
  Fspmcfg->DqByteMapCh1[8] = 0x0;
  Fspmcfg->DqByteMapCh1[10] = 0x0;
  Fspmcfg->DqsMapCpu2DramCh0[0] = 0x2;
  Fspmcfg->DqsMapCpu2DramCh0[1] = 0x0;
  Fspmcfg->DqsMapCpu2DramCh0[2] = 0x1;
  Fspmcfg->DqsMapCpu2DramCh0[3] = 0x3;
  Fspmcfg->DqsMapCpu2DramCh0[4] = 0x6;
  Fspmcfg->DqsMapCpu2DramCh0[5] = 0x4;
  Fspmcfg->DqsMapCpu2DramCh0[6] = 0x7;
  Fspmcfg->DqsMapCpu2DramCh0[7] = 0x5;
  Fspmcfg->DqsMapCpu2DramCh1[1] = 0x3;
  Fspmcfg->DqsMapCpu2DramCh1[2] = 0x2;
  Fspmcfg->DqsMapCpu2DramCh1[3] = 0x0;
  Fspmcfg->DqsMapCpu2DramCh1[4] = 0x5;
  Fspmcfg->DqsMapCpu2DramCh1[5] = 0x7;
  Fspmcfg->DqsMapCpu2DramCh1[7] = 0x4;
  Fspmcfg->TsegSize = 0x1000000;
  Fspmcfg->InternalGfx = 0x2;
  Fspmcfg->DmiGen3RxCtlePeaking[0] = 0x3;
  Fspmcfg->DmiGen3RxCtlePeaking[1] = 0x3;
  Fspmcfg->TvbVoltageOptimization = 0x0;
  Fspmcfg->PegGen3RxCtlePeaking[8] = 0xc;
  Fspmcfg->PegGen3RxCtlePeaking[9] = 0xc;
  Fspmcfg->TxtImplemented = 0x1;
  Fspmcfg->RingDownBin = 0x0;
  Fspmcfg->TjMaxOffset = 0x0;
  Fspmcfg->SinitMemorySize = 0x50000;
  Fspmcfg->TxtHeapMemorySize = 0xf0000;
  Fspmcfg->IsTPMPresence = 0x4;
  Fspmcfg->PcdDebugInterfaceFlags = 0x32;
  Fspmcfg->RaplLim2WindY = 0xa;
  Fspmcfg->TsodTcritMax = 0x0;
  Fspmcfg->TsodThigMax = 0x0;
  Fspmcfg->CmdRanksTerminated = 0x0;
  FspmcfgTest->BdatEnable = 0x0;
  FspmcfgTest->VtdDisable = 0x0;

}

/**
  Initialize the SPI controller.

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
      PCI_COMMAND_OFFSET
    ),
    EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE
  );
  SpiConstructor ();
}

/**
  Switch between the boot partitions.

  This function will use platform specific method of switching
  between primary and backup partitions.

  @param[in] Partition        Partition to select

  @retval  EFI_SUCCESS        Switched to desired partition successfully.
  @retval  others             Error happening.
**/
EFI_STATUS
SetBootPartition (
  IN BOOT_PARTITION  Partition
  )
{
  UINTN     P2sbBase;
  UINT32    P2sbBar;
  UINT32    TopSwapReg;
  UINT32    Data32;

  //
  // Get Top swap register Bit0 in PCH Private Configuration Space.
  //
  P2sbBase   = MmPciBase (0, PCI_DEVICE_NUMBER_PCH_LPC, 1); // P2SB device base
  DEBUG ((DEBUG_ERROR, "p2sbbase: %x\n", P2sbBase));

  P2sbBar=0xFD000000;
  TopSwapReg = P2sbBar | ((PID_RTC_HOST) << 16) | (UINT16)(R_RTC_PCR_BUC);
  Data32    = MmioRead32 (TopSwapReg);
  DEBUG ((DEBUG_INFO, "P2sbBar=0x%x, Data32=0x%x\n", P2sbBar, Data32));

  if (Partition == BackupPartition) {
    //
    // Switch to back up partition - Set Top Swap
    //
    Data32 |= BIT0;
  } else if (Partition == PrimaryPartition) {
    //
    // Switch to primary partition - Clear Top Swap
    //
    Data32 &= ~BIT0;
  }

  MmioWrite32 (TopSwapReg, Data32);
  DEBUG ((DEBUG_INFO, "write Data32=0x%x\n", Data32));
  Data32 = MmioRead32 (TopSwapReg);

  DEBUG ((DEBUG_INFO, "Read it to ensure data is written. Data32=0x%x\n", Data32));

  return EFI_SUCCESS;
}

/**
  Determine if we this is firmware update mode.

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


VOID
FwuTopSwapSetting (
  IN FW_UPDATE_STATUS    *pFwUpdStatus
  )
{
  UINT32    Data32;
  UINT32    TopSwapReg;
  UINT32    P2sbBar;
  EFI_STATUS          Status;
  UINT32              RsvdBase;
  UINT32              RsvdSize;

    if (pFwUpdStatus == NULL) {
    Status = GetComponentInfoByPartition (FLASH_MAP_SIG_BLRESERVED, FALSE, &RsvdBase, &RsvdSize);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "Could not get component information for bootloader reserved region\n"));
    }
    pFwUpdStatus = (FW_UPDATE_STATUS *)(UINTN)RsvdBase;
  }

    P2sbBar=0xFD000000;
    TopSwapReg = P2sbBar | ((PID_RTC_HOST) << 16) | (UINT16)(R_RTC_PCR_BUC);
    Data32    = MmioRead32 (TopSwapReg);
    DEBUG ((DEBUG_INFO, "TopSwapReg=0x%x\n",  Data32));
    if (pFwUpdStatus->StateMachine == 0x7E) {
      if (GetCurrentBootPartition() == 0) {
        SetBootPartition(1);
        ResetSystem(EfiResetCold);
      }
    } else if (pFwUpdStatus->StateMachine == 0x7D) {
      if (GetCurrentBootPartition() == 1) {
        SetBootPartition(0);
        ResetSystem(EfiResetCold);
      }
    }
    else{
     DEBUG ((DEBUG_INFO, "Not in Firmware Update mode.\n"));
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
  UINT32                       Features;
  PLATFORM_DATA               *PlatformData;
  UINTN                        HeciBaseAddress;

  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();

  if (PlatformData != NULL) {
    HeciBaseAddress = MM_PCI_ADDRESS (
                        0x0,
                        22,
                        0x0,
                        0x0
                        );
    GetBootGuardInfo (HeciBaseAddress, &PlatformData->BtGuardInfo);
    DEBUG ((DEBUG_INFO, "[GetPlatformDataPtr] GetPlatformDataPtr is copied 0x%08X \n",  PlatformData ));
  }

  Features        = 0;
  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag(CDATA_FEATURES_TAG);
  if (FeaturesCfgData != NULL) {

    if (FeaturesCfgData->Features.Acpi != 0) {
      Features |= FEATURE_ACPI;
    } else {
      Features &= ~FEATURE_ACPI;
    }

    if((PlatformData != NULL && PlatformData->BtGuardInfo.VerifiedBoot != 0) && FeaturePcdGet(PcdVerifiedBootEnabled)){
      Features |= FEATURE_VERIFIED_BOOT;
    }else {
      Features &= ~FEATURE_VERIFIED_BOOT;
    }

    if ((FeaturesCfgData->Features.MeasuredBoot != 0) && (PlatformData !=NULL && PlatformData->BtGuardInfo.MeasuredBoot != 0) && MEASURED_BOOT_ENABLED()) {
      Features |= FEATURE_MEASURED_BOOT;
    } else {
      Features &= ~FEATURE_MEASURED_BOOT;
    }

    if (FeaturesCfgData->Features.eMMCTuning != 0) {
      Features |= FEATURE_MMC_TUNING;
    }
  } else {
    DEBUG ((DEBUG_INFO, "FEATURES CFG DATA NOT FOUND!\n"));
  }

  SetFeatureCfg (Features);
  DEBUG ((DEBUG_INFO, "PlatformFeaturesInit: Features 0x%x\n", GetFeatureCfg ()));
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

  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();

  BootMode = GetBootMode();

  if ((PlatformData != NULL) && (PlatformData->BtGuardInfo.MeasuredBoot == 1) &&
      (PlatformData->BtGuardInfo.DisconnectAllTpms == 0) &&
      ((PlatformData->BtGuardInfo.TpmType == dTpm20) || (PlatformData->BtGuardInfo.TpmType == Ptt))) {

    //  As per PC Client spec, SRTM should perform a host platform reset
    if (PlatformData->BtGuardInfo.TpmStartupFailureOnS3 == TRUE) {
      ResetSystem(EfiResetCold);
      CpuDeadLoop ();
    }
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

    Features  = GetFeatureCfg ();
    Features &= (UINT32)(~FEATURE_MEASURED_BOOT);
    SetFeatureCfg (Features);
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
  UINT32        PmconA, PmconB;
  UINTN         PmcBaseAddr;

  PmcBaseAddr = PCI_LIB_ADDRESS (
    DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_PMC,
    PCI_FUNCTION_NUMBER_PCH_PMC,
    0);
  PmconA = PciRead32 (PmcBaseAddr + R_PMC_PWRM_GEN_PMCON_A);
  PmconB = PciRead32 (PmcBaseAddr + R_PMC_PWRM_GEN_PMCON_B);

  //
  // Clear PWRBTNOR_STS
  //
  if (IoRead16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS) & B_ACPI_IO_PM1_STS_PRBTNOR) {
    IoWrite16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS, B_ACPI_IO_PM1_STS_PRBTNOR);
  }

  //Enable TCO BIT for CHIPSEC BIOS SMI
   IoOr32(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN, B_ACPI_IO_SMI_EN_TCO);

  //
  // If Global Reset Status, Power Failure. Host Reset Status bits are set, return S5 State
  //
  if ((PmconB & ( B_PMC_PWRM_GEN_PMCON_B_PWR_FLR | B_PMC_PWRM_GEN_PMCON_B_HOST_RST_STS)) != 0) {
    return BOOT_WITH_FULL_CONFIGURATION;
  }

  if ((PmconA & (B_PMC_PWRM_GEN_PMCON_A_GBL_RST_STS)) != 0) {
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
  /// Clear Wake Status
  /// Also clear the PWRBTN_EN, it causes SMI# otherwise (SCI_EN is 0)
  ///
  IoAndThenOr32 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS, (UINT32)~B_ACPI_IO_PM1_EN_PWRBTN_EN, B_ACPI_IO_PM1_STS_WAK );
  PmconB = PciRead32 (PmcBaseAddr + R_PMC_PWRM_GEN_PMCON_B);
  if((PmconB & B_PMC_PWRM_GEN_PMCON_B_RTC_PWR_STS) != 0) {
    BootMode = BOOT_WITH_FULL_CONFIGURATION;

    ///
    /// Clear Sleep Type
    ///
    IoAndThenOr16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT, (UINT16) ~B_ACPI_IO_PM1_CNT_SLP_TYP, V_ACPI_IO_PM1_CNT_S0);
  }

  //Disabling PME_0_EN to avoid infinite SMI generated during S3 wake up.
  //SMI is generated because of GPE0_STS is set in SMI_STS.
  IoAnd32 (ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_EN_127_96, (UINT32)~B_ACPI_IO_GPE0_STS_127_96_PME_B0);

  return BootMode;
}

/**
  Initialize EMMC in early stage before MemoryInit to achieve better performance number

  Assume EMMC PCI Memory Config Space is not configured yet.
  Use the same temporary base address(0xFE600000) with BIOS's one for EMMC Config Space
**/
VOID
EarlyBootDeviceInit (
  VOID
)
{
  EFI_STATUS  Status        = EFI_SUCCESS;
  UINTN       EmmcHcPciBase = MmPciBase(0, 0x1A, 0);
  UINT32      Base          = 0xFE600000;

  /* Configure EMMC GPIO Pad */
  GpioConfigurePads (sizeof (mGpioTableEMMC) / sizeof (GPIO_INIT_CONFIG), mGpioTableEMMC);
  DEBUG ((DEBUG_INFO, "Early GpioInit for EMMC\n"));

  MmioWrite32 (EmmcHcPciBase + PCI_BASE_ADDRESSREG_OFFSET, Base);
  MmioWrite32 (EmmcHcPciBase + PCI_BASE_ADDRESSREG_OFFSET + 4, 0x00000000);
  MmioOr32    (EmmcHcPciBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);

  /* Configure initial DLLs for early emmc init before SiliconInit */
  MmioWrite32(Base + R_SCS_MEM_TX_CMD_DLL_CNTL,       0x505);
  MmioWrite32(Base + R_SCS_MEM_TX_DATA_DLL_CNTL1,     0x0B0B);
  MmioWrite32(Base + R_SCS_MEM_TX_DATA_DLL_CNTL2,     0x1C292828);
  MmioWrite32(Base + R_SCS_MEM_RX_CMD_DATA_DLL_CNTL1, 0x1C0B5F32);
  MmioWrite32(Base + R_SCS_MEM_RX_CMD_DATA_DLL_CNTL2, 0x21808);
  MmioWrite32(Base + R_SCS_MEM_RX_STROBE_DLL_CNTL,    0x1818);

  Status = MmcInitialize(EmmcHcPciBase, DevInitOnlyPhase1);

  MmioAnd32   (EmmcHcPciBase + PCI_COMMAND_OFFSET, (UINT32)~EFI_PCI_COMMAND_MEMORY_SPACE);
  MmioWrite32 (EmmcHcPciBase + PCI_BASE_ADDRESSREG_OFFSET, 0x00000000);

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Early MMC Init Fails @ 0x%08X - Status(%r)\n", Base, Status));
  } else {
    DEBUG ((DEBUG_INFO, "Early MMC Init Done @ 0x%08X\n", Base));
  }
}

/**
  Initialize Board specific things in Stage1 Phase

  @param[in]  InitPhase            Indicates a board init phase to be initialized

**/
VOID
EFIAPI
BoardInit (
  IN  BOARD_INIT_PHASE  InitPhase
)
{

  PLT_DEVICE_TABLE     *PltDeviceTable;
  UINT8                 BoardId;

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
    if (IsPchLp ()) {
      EarlyBootDeviceInit ();
    }

    PltDeviceTable = (PLT_DEVICE_TABLE *)AllocatePool (sizeof (PLT_DEVICE_TABLE) + sizeof (mPlatformDevices));
    if (PltDeviceTable) {
      PltDeviceTable->DeviceNumber = sizeof (mPlatformDevices) /sizeof (PLT_DEVICE);
      CopyMem (PltDeviceTable->Device, mPlatformDevices, sizeof (mPlatformDevices));
      SetDeviceTable (PltDeviceTable);
    }

    SpiControllerInitialize ();
    SetBootMode (IsFirmwareUpdate() ? BOOT_ON_FLASH_UPDATE : GetPlatformPowerState());
    GetBoardId(&BoardId);
    SetPlatformId(BoardId);
    FwuTopSwapSetting(NULL);
    break;
  case PostConfigInit:
    PlatformNameInit ();
    PlatformFeaturesInit ();
    break;
  case PreMemoryInit:
    GpioConfigurePads (sizeof (mGpioTableCmlS82Ddr4PreMem) / sizeof (GPIO_INIT_CONFIG), mGpioTableCmlS82Ddr4PreMem );
    break;
  case PostMemoryInit:
    DEBUG ((DEBUG_INFO, "PostMemoryInit called\n"));
    UpdateMemoryInfo ();
    break;
  case PreTempRamExit:
    break;
  case PostTempRamExit:
    if (FeaturePcdGet (PcdMeasuredBootEnabled)) {
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
  UINT32    MrcData;
  EFI_STATUS    Status;

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
