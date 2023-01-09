/** @file

  Copyright (c) 2019 - 2023, Intel Corporation. All rights reserved.<BR>
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
#include <Library/SpiFlashLib.h>
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
#include <Library/BootGuardTpmEventLogLib.h>
#include <Library/SgxLib.h>
#include <PciePreMemConfig.h>
#include <PlatformData.h>
#include "PreMemGpioTables.h"
#include <Register/RegsSpi.h>

CONST PLT_DEVICE  mPlatformDevices[]= {
  {{0x00001400}, OsBootDeviceUsb   , 0 },
  {{0x00001700}, OsBootDeviceSata  , 0 },
  {{0x00001A00}, OsBootDeviceEmmc  , 0 },
  {{0x00001F05}, OsBootDeviceSpi   , 0 }
};

CONST UINT16 mRcompResistor[3] = { 121, 75, 100 };
CONST UINT16 mRcompTarget[5]   = { 60, 26, 20, 20, 26 };

CONST UINT32 mUpxGpioBomPad[]  = {
  GPIO_CNL_LP_GPP_C10,  // BRD_ID2
  GPIO_CNL_LP_GPP_C9,   // BRD_ID1
  GPIO_CNL_LP_GPP_C8,   // BRD_ID0
  GPIO_CNL_LP_GPP_A12,  // DDR_ID2
  GPIO_CNL_LP_GPP_A18,  // DDR_ID1
  GPIO_CNL_LP_GPP_C11   // DDR_ID0
};

CONST GPIO_INIT_CONFIG mUpxBomGpioTemplate = {
  GPIO_CNL_LP_GPP_C10, {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn, GpioOutDefault, GpioIntDis, GpioHostDeepReset, GpioTermNone}
};

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
  PLATFORM_DATA                   *PlatformData;
  UINT16                           PlatformId;
  UINT16                           BomId;
  PEG_GPIO_DATA                   *PegGpioData;
  UINT32                           SpdData[3];
  UINT32                           SpdPtr;

  FspmUpd       = (FSPM_UPD *)FspmUpdPtr;
  Fspmcfg       = &FspmUpd->FspmConfig;
  FspmcfgTest   = &FspmUpd->FspmTestConfig;
  PlatformId    = GetPlatformId ();

  Fspmcfg->MemorySpdDataLen = 512;
  CopyMem (&Fspmcfg->RcompTarget, mRcompTarget, sizeof(mRcompTarget));

  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
  if (MemCfgData == NULL) {
    CpuHalt ("Failed to find memory CFGDATA!");
    return;
  }

  DEBUG ((DEBUG_INFO, "Load memory parameters from CfgData.\n"));
  if (PlatformId == PLATFORM_ID_CFL_H ||
      PlatformId == PLATFORM_ID_WHL ||
      PlatformId == PLATFORM_ID_CFL_S) {
    CopyMem (&Fspmcfg->SpdAddressTable, MemCfgData->SpdAddressTable, sizeof(MemCfgData->SpdAddressTable));
  } else {
    SpdData[0] = 0;
    SpdData[1] = (UINT32)(UINTN) (((MEM_SPD0_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD0_TAG))->MemorySpdPtr0);
    SpdData[2] = (UINT32)(UINTN) (((MEM_SPD1_CFG_DATA *)FindConfigDataByTag (CDATA_MEM_SPD1_TAG))->MemorySpdPtr1);

    if (PlatformId == PLATFORM_ID_UPXTREME) {
      // For UPX, using BomID to decide SPD data instead.
      BomId = GetPlatformBomId ();
      // BOMID [1:0]
      //   0: 16G A & B CH
      //   1:  8G A CH
      //   2:  8G A & B CH
      //   3:  4G A CH
      if ((BomId & BIT1) == BIT1) {
        SpdPtr = SpdData[2];
      } else {
        SpdPtr = SpdData[1];
      }
      if ((BomId & BIT0) == BIT0) {
        // Single Channel
        Fspmcfg->MemorySpdPtr00 = SpdPtr;
        Fspmcfg->MemorySpdPtr01 = 0;
        Fspmcfg->MemorySpdPtr11 = 0;
        Fspmcfg->MemorySpdPtr10 = 0;
      } else {
        // Dual Channel
        Fspmcfg->MemorySpdPtr00 = SpdPtr;
        Fspmcfg->MemorySpdPtr01 = 0;
        Fspmcfg->MemorySpdPtr10 = SpdPtr;
        Fspmcfg->MemorySpdPtr11 = 0;
      }
    } else {
      Fspmcfg->MemorySpdPtr00       = SpdData[MemCfgData->SpdDataSel00];
      Fspmcfg->MemorySpdPtr01       = SpdData[MemCfgData->SpdDataSel01];
      Fspmcfg->MemorySpdPtr10       = SpdData[MemCfgData->SpdDataSel10];
      Fspmcfg->MemorySpdPtr11       = SpdData[MemCfgData->SpdDataSel11];
    }
  }
  //Dq/Dqs Mapping arrays
  CopyMem (&Fspmcfg->DqByteMapCh0, MemCfgData->DqByteMapCh0, sizeof(MemCfgData->DqByteMapCh0));
  CopyMem (&Fspmcfg->DqByteMapCh1, MemCfgData->DqByteMapCh1, sizeof(MemCfgData->DqByteMapCh1));
  CopyMem (&Fspmcfg->DqsMapCpu2DramCh0, MemCfgData->DqsMapCpu2DramCh0, sizeof(MemCfgData->DqsMapCpu2DramCh0));
  CopyMem (&Fspmcfg->DqsMapCpu2DramCh1, MemCfgData->DqsMapCpu2DramCh1, sizeof(MemCfgData->DqsMapCpu2DramCh1));
  Fspmcfg->DqPinsInterleaved      = MemCfgData->DqPinsInterleaved;
  Fspmcfg->TsegSize               = MemCfgData->TsegSize;
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

  if (PlatformId == PLATFORM_ID_CFL_S) {
    ASSERT (sizeof (PEG_GPIO_DATA) == sizeof (Fspmcfg->PegGpioData));
    ZeroMem (Fspmcfg->PegGpioData, sizeof (Fspmcfg->PegGpioData));
    PegGpioData                           = (PEG_GPIO_DATA *) Fspmcfg->PegGpioData;
    PegGpioData->GpioSupport              = 1;
    PegGpioData->SaPeg0ResetGpio.Active   = 0;  // Active Low
    PegGpioData->SaPeg0ResetGpio.GpioPad  = GPIO_CNL_H_GPP_F22;

    ZeroMem (Fspmcfg->PegGen3RxCtlePeaking, sizeof (Fspmcfg->PegGen3RxCtlePeaking));

    FspmcfgTest->Peg3Gen3EqPh3Method      = 0;  // Auto
  }

  if (!(UpdateFspmSgxConfig (FspmUpd))) {
    DEBUG ((DEBUG_INFO, "FSP-M variables for Intel(R) SGX were NOT updated.\n"));
  }

  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if (PlatformData != NULL) {
    PlatformData->PlatformFeatures.VtdEnable = FeaturePcdGet (PcdVtdEnabled) && (!MemCfgData->VtdDisable);
  }

  // Enable VT-d
  if (PlatformData->PlatformFeatures.VtdEnable == 1) {
    FspmcfgTest->VtdDisable = 0;
    Fspmcfg->X2ApicOptOut = MemCfgData->X2ApicOptOut;
    Fspmcfg->VtdBaseAddress[0] = 0xFED90000;
    Fspmcfg->VtdBaseAddress[1] = 0xFED92000;
    Fspmcfg->VtdBaseAddress[2] = 0xFED91000;
  } else {
    FspmcfgTest->VtdDisable = 1;
  }

  Fspmcfg->PlatformDebugConsent = MemCfgData->PlatformDebugConsent;
  Fspmcfg->PchTraceHubMode      = MemCfgData->PchTraceHubMode;
}

/**
  Initialize the SPI controller.

**/
VOID
SpiControllerInitialize (
  VOID
)
{
  UINTN   SpiBaseAddress;

  SpiBaseAddress = GetDeviceAddr (OsBootDeviceSpi, 0);
  SpiBaseAddress = TO_MM_PCI_ADDRESS (SpiBaseAddress);

  MmioWrite32 (SpiBaseAddress + R_SPI_BASE, SPI_TEMP_MEM_BASE_ADDRESS);
  MmioAnd8    (SpiBaseAddress + R_SPI_BCR, (UINT8)~B_SPI_BCR_SMM_BWP);

  SpiConstructor ();
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
/**
  Detect board and configure PlatformID.

  @retval  none
**/
VOID
PlatformIdInitialize (
  IN  VOID
  )
{
  UINT16              PlatformId;
  UINT16              CpuDid;
  UINT32              CpuFamilyModel;
  UINT8               CpuStepping;
  EFI_CPUID_REGISTER  Cpuid;
  EFI_STATUS          Status;
  UINT8               Idx;
  UINT16              BomId;
  UINT32              GpioData;
  GPIO_INIT_CONFIG    UpxBomGpioTemplate;

  ///
  /// Read the CPUID & DID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  CpuFamilyModel = Cpuid.RegEax & CPUID_FULL_FAMILY_MODEL;
  CpuStepping = (Cpuid.RegEax & 0xF);
  CpuDid = PciRead16 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_MC_DEVICE_ID));

  DEBUG ((DEBUG_ERROR, "CpuFamilyModel 0x%X, CpuStepping 0x%X, CpuDid 0x%X\n", CpuFamilyModel, CpuStepping, CpuDid));

  PlatformId = GetPlatformId();
  if (PlatformId == PLATFORM_ID_UPXTREME) {
    // For UPX, further detect the BOM ID
    BomId = 0;
    CopyMem (&UpxBomGpioTemplate, &mUpxBomGpioTemplate, sizeof(UpxBomGpioTemplate));

    // Initialize all GPIO pins to input
    for (Idx = 0; Idx < ARRAY_SIZE(mUpxGpioBomPad); Idx++) {
      UpxBomGpioTemplate.GpioPad = mUpxGpioBomPad[Idx];
      GpioConfigurePads (1, &UpxBomGpioTemplate);
    }

    // Sample the GPIO pin level
    for (Idx = 0; Idx < ARRAY_SIZE(mUpxGpioBomPad); Idx++) {
      Status = GpioGetInputValue (mUpxGpioBomPad[Idx], &GpioData);
      if (EFI_ERROR(Status)) {
        break;
      }
      BomId = (BomId << 1) + (GpioData & BIT0);
    }

    if (Idx == ARRAY_SIZE(mUpxGpioBomPad)) {
      DEBUG ((DEBUG_INFO, "UPX BomID: 0x%02X\n", BomId));
      SetPlatformBomId (BomId);
    }
  }

  if (PlatformId != 0) {
    // PlatformID is assigned already, don't detect again.
    return;
  }

  if (IsPchH()) {         // Coffee lake
    if ((CpuFamilyModel == CPUID_FULL_FAMILY_MODEL_COFFEELAKE_DT_HALO) &&
      ((CpuDid == V_SA_DEVICE_ID_CFL_DT_1) ||
       (CpuDid == V_SA_DEVICE_ID_CFL_DT_2) ||
       (CpuDid == V_SA_DEVICE_ID_CFL_DT_3) ||
       (CpuDid == V_SA_DEVICE_ID_CFL_DT_4))) {
      SetPlatformId (PLATFORM_ID_CFL_S);  // CFL-S
    } else if (CpuFamilyModel == CPUID_FULL_FAMILY_MODEL_COFFEELAKE_DT_HALO &&
      ((CpuDid == V_SA_DEVICE_ID_CFL_HALO_1) ||
       (CpuDid == V_SA_DEVICE_ID_CFL_HALO_2) ||
       (CpuDid == V_SA_DEVICE_ID_CFL_HALO_3) ||
       (CpuDid == V_SA_DEVICE_ID_CFL_HALO_IOT_1))) {
      SetPlatformId (PLATFORM_ID_CFL_H);  // CFL-H
    } else {
      DEBUG ((DEBUG_ERROR, "Unsupported CFL CPU!\n"));
    }
  } else if (IsPchLp()) { // Whiskey lake
    if (CpuFamilyModel == CPUID_FULL_FAMILY_MODEL_COFFEELAKE_ULT_ULX) {
      SetPlatformId (PLATFORM_ID_WHL);  // WHL
    } else {
      DEBUG ((DEBUG_ERROR, "Unsupported WHL CPU!\n"));
    }
  }

}


/**
  Initialize GPIO pads

  @param[in]  PlatformId            Indicates a specific platform id

  @retval     none
**/
VOID
GpioInit (
  IN  UINT16            PlatformId
  )
{
  EFI_STATUS  Status;

  if (PlatformId == PLATFORM_ID_WHL) {
    Status = GpioConfigurePads (sizeof (mGpioTableCnlUDdr4PreMem) / sizeof (GPIO_INIT_CONFIG), mGpioTableCnlUDdr4PreMem );
    Status |= GpioConfigurePads (sizeof (mGpioTableWhlUDdr4WwanPreMem) / sizeof (GPIO_INIT_CONFIG), mGpioTableWhlUDdr4WwanPreMem );
  } else if (PlatformId == PLATFORM_ID_CFL_H) {
    Status = GpioConfigurePads (sizeof (mGpioTableCoffeelakeHDdr4PreMem) / sizeof (GPIO_INIT_CONFIG), mGpioTableCoffeelakeHDdr4PreMem );
  } else if (PlatformId == PLATFORM_ID_CFL_S) {
    Status = GpioConfigurePads (sizeof (mGpioTableCflS82Ddr4PreMem) / sizeof (GPIO_INIT_CONFIG), mGpioTableCflS82Ddr4PreMem );
  } else {
    Status = EFI_SUCCESS;
  }

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "GpioConfig unsuccessful = %r\n", Status));
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

  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();

  if (PlatformData != NULL) {
    GetBootGuardInfo (&(PlatformData->BtGuardInfo));
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

  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();

  BootMode = GetBootMode();

  if ((PlatformData != NULL) && (PlatformData->BtGuardInfo.MeasuredBoot == 1) &&
      (PlatformData->BtGuardInfo.DisconnectAllTpms == 0) &&
      ((PlatformData->BtGuardInfo.TpmType == dTpm20) || (PlatformData->BtGuardInfo.TpmType == Ptt))) {
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
    IoWrite16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS, B_ACPI_IO_PM1_STS_PRBTNOR | B_ACPI_IO_PM1_STS_PRBTN);
  }

  //
  // If Global Reset Status, Power Failure. Host Reset Status bits are set, return S5 State
  //
  BootMode = BOOT_WITH_FULL_CONFIGURATION;
  if ((PmconA & (B_PMC_PWRM_GEN_PMCON_A_GBL_RST_STS | B_PMC_PWRM_GEN_PMCON_A_PWR_FLR | B_PMC_PWRM_GEN_PMCON_A_HOST_RST_STS)) == 0) {
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
    IoAndThenOr32 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS, (UINT32)~B_ACPI_IO_PM1_EN_PWRBTN_EN, B_ACPI_IO_PM1_STS_WAK);

    IoAnd32 (ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_EN_127_96, (UINT32)~B_ACPI_IO_GPE0_STS_127_96_PME_B0);

    if ((MmioRead8 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_B) & B_PMC_PWRM_GEN_PMCON_B_RTC_PWR_STS) != 0) {
      BootMode = BOOT_WITH_FULL_CONFIGURATION;

      ///
      /// Clear Sleep Type
      ///
      IoAndThenOr16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT, (UINT16) ~B_ACPI_IO_PM1_CNT_SLP_TYP, V_ACPI_IO_PM1_CNT_S0);
    }
  }

  if ((BootMode == BOOT_WITH_FULL_CONFIGURATION) || (BootMode == BOOT_ON_S5_RESUME)) {
    // Clear power button status to prevent false power button event detection later on
    IoWrite16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS,  B_ACPI_IO_PM1_STS_PRBTN);
  }

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
  UINT16                PlatformId;
  PLT_DEVICE_TABLE     *PltDeviceTable;

  PlatformId = GetPlatformId ();

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
    if (PltDeviceTable == NULL) {
      return;
    }
    PltDeviceTable->DeviceNumber = sizeof (mPlatformDevices) /sizeof (PLT_DEVICE);
    CopyMem (PltDeviceTable->Device, mPlatformDevices, sizeof (mPlatformDevices));
    SetDeviceTable (PltDeviceTable);
    SpiControllerInitialize ();
    SetBootMode (IsFirmwareUpdate() ? BOOT_ON_FLASH_UPDATE : GetPlatformPowerState());
    break;
  case PostConfigInit:
    PlatformIdInitialize ();
    PlatformNameInit ();
    PlatformFeaturesInit ();
    break;
  case PreMemoryInit:
    GpioInit (PlatformId);
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
