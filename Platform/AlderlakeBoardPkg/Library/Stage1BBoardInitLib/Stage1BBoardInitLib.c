/** @file

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage1BBoardInitLib.h"

CONST PLT_DEVICE mPlatformDevices[] = {
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = PCI_FUNCTION_NUMBER_PCH_XHCI,
        .PciDeviceNumber    = PCI_DEVICE_NUMBER_PCH_XHCI,
        .PciBusNumber       = DEFAULT_PCI_BUS_NUMBER_PCH,
        .IsMmioDevice       = 0
      }
    },
    .Type = OsBootDeviceUsb,
    .Instance = 0
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
    .Type = OsBootDeviceSata,
    .Instance = 0
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = PCI_FUNCTION_NUMBER_PCH_SPI,
        .PciDeviceNumber    = PCI_DEVICE_NUMBER_PCH_SPI,
        .PciBusNumber       = DEFAULT_PCI_BUS_NUMBER_PCH,
        .IsMmioDevice       = 0
      }
    },
    .Type = OsBootDeviceSpi,
    .Instance = 0
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = 0,
        .PciDeviceNumber    = 0,
        .PciBusNumber       = 0,
        .IsMmioDevice       = 0
      }
    },
    .Type = OsBootDeviceNvme,
    .Instance = 0
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = 0,
        .PciDeviceNumber    = 0,
        .PciBusNumber       = 0,
        .IsMmioDevice       = 0
      }
    },
    .Type = OsBootDeviceNvme,
    .Instance = 1
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = 5,
        .PciDeviceNumber    = 18,
        .PciBusNumber       = 0,
        .IsMmioDevice       = 0
      }
    },
    .Type = OsBootDeviceUfs,
    .Instance = 0
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = 7,
        .PciDeviceNumber    = 18,
        .PciBusNumber       = 0,
        .IsMmioDevice       = 0
      }
    },
    .Type = OsBootDeviceUfs,
    .Instance = 1
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = PCI_FUNCTION_NUMBER_PCH_SMBUS,
        .PciDeviceNumber    = PCI_DEVICE_NUMBER_PCH_SMBUS,
        .PciBusNumber       = DEFAULT_PCI_BUS_NUMBER_PCH,
        .IsMmioDevice       = 0
      }
    },
    .Type = PltDeviceSmbus,
    .Instance = 0
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = 0,
        .PciDeviceNumber    = 2,
        .PciBusNumber       = 0,
        .IsMmioDevice       = 0
      }
    },
    .Type = PlatformDeviceGraphics,
    .Instance = 0
  }
};


VOID
GetBoardId (
  OUT UINT8 *BoardId
);

/**
  Detect board and configure PlatformID.

  @retval  none
**/
VOID
EFIAPI
PlatformIdInitialize (
  IN  VOID
  )
{
  UINT8     BoardId;

  BoardId = (UINT8) GetPlatformId ();

  if (BoardId == 0) {
    GetBoardId (&BoardId);
    SetPlatformId (BoardId);
  }

  DEBUG ((DEBUG_INFO, "BoardId 0x%X \n", BoardId));
}

/**
  Initialize Platform Device Table

  @retval  none
**/
VOID
EFIAPI
PlatformDeviceTableInitialize (
  IN  VOID
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
  Set TS based on FW update status.
  This function will set the TS register based on the FW update status.
  The TS register is set here as opposed to in the FW update payload due to a
  uCode assert issue.

  @retval  EFI_SUCCESS           The operation completed successfully.
  @retval  others                There is error happening.
**/
VOID
FwuTopSwapSetting (
  VOID
  )
{
  EFI_STATUS        Status;
  UINT32            RsvdBase;
  UINT32            RsvdSize;
  FW_UPDATE_STATUS  *FwUpdStatus;

  Status = GetComponentInfoByPartition (FLASH_MAP_SIG_BLRESERVED, FALSE, &RsvdBase, &RsvdSize);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Could not get component information for bootloader reserved region\n"));
  }
  FwUpdStatus = (FW_UPDATE_STATUS *)(UINTN)RsvdBase;


  // If in a recovery path, stay on current partition
  if (PcdGetBool (PcdSblResiliencyEnabled) && IsRecoveryTriggered ()) {
    return;
  }

  switch (FwUpdStatus->StateMachine) {
    case FW_UPDATE_SM_RECOVERY:
      // This indicates that recovery is in progress
      ASSERT (PcdGetBool (PcdSblResiliencyEnabled));
      break;

    case FW_UPDATE_SM_PART_A:
      // This indicates that update of partition B is complete
      if (GetCurrentBootPartition () == PrimaryPartition) {
        if (IsTopSwapTriggered ()) {
          ClearTopSwapTrigger ();
          SetBootPartition (BackupPartition);
          ResetSystem (EfiResetCold);
        }
      } else {
        if (IsTopSwapTriggered ()) {
          DEBUG((DEBUG_INFO, "Already on partition that was meant to be swapped back to\n"));
          ClearTopSwapTrigger ();
          if (PcdGetBool (PcdSblResiliencyEnabled)) {
            SetRecoveryTrigger ();
          }
        }
      }
      break;

    case FW_UPDATE_SM_PART_AB:
      // This indicates update of partition A and B is complete and
      // firmware update structure need finalization
      break;

    default:
      if (GetCurrentBootPartition () == BackupPartition) {
        if (IsTopSwapTriggered ()) {
          ClearTopSwapTrigger ();
          SetBootPartition (PrimaryPartition);
          ResetSystem (EfiResetCold);
        }
      } else {
        if (IsTopSwapTriggered ()) {
          DEBUG((DEBUG_INFO, "Already on partition that was meant to be swapped back to\n"));
          ClearTopSwapTrigger ();
          if (PcdGetBool (PcdSblResiliencyEnabled)) {
            SetRecoveryTrigger ();
          }
        }
      }
      break;
  }
}

/**
  Determine if this is firmware update mode.

  This function will determine if we have to put system in firmware update mode.

  @retval  EFI_SUCCESS           The operation completed successfully.
  @retval  others                There is error happening.
**/
BOOLEAN
IsFirmwareUpdate (
  VOID
  )
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
  if (IsUpdateTriggered ()) {
    return TRUE;
  }

  //
  // Check if we need to recover a failing partition.
  //
  if (PcdGetBool (PcdSblResiliencyEnabled) &&
      IsRecoveryTriggered ()) {
    return TRUE;
  }

  return FALSE;
}

/**
  Initialize SPI controller

  @retval  none
**/
VOID
EFIAPI
SpiControllerInitialize (
  IN  VOID
)
{
  UINTN   SpiBaseAddress;

  SpiBaseAddress = GetDeviceAddr (OsBootDeviceSpi, 0);
  SpiBaseAddress = TO_MM_PCI_ADDRESS (SpiBaseAddress);

  MmioWrite32 (SpiBaseAddress + R_SPI_CFG_BAR0, PCH_SPI_BASE_ADDRESS);
  MmioAnd8    (SpiBaseAddress + R_SPI_CFG_BC, (UINT8)~B_SPI_CFG_BC_EISS);

  SpiConstructor ();
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
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "Variable initialize failed %r!\n", Status));
    }
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
  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
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
    HeciBaseAddress = MeGetHeciMmPciAddress (0, 0);
    GetBootGuardInfo (HeciBaseAddress, &PlatformData->BtGuardInfo);
    DEBUG ((DEBUG_INFO, "GetPlatformDataPtr is copied 0x%08X \n", PlatformData));
    if ((PlatformData->BtGuardInfo.MeasuredBoot) && (!PlatformData->BtGuardInfo.BypassTpmInit)) {
      // Disable the measured boot when TPM init is not succesful by ACM
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

  BootMode     = GetBootMode();
  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();


  if ((PlatformData != NULL) && MEASURED_BOOT_ENABLED() &&
    (!PlatformData->BtGuardInfo.DisconnectAllTpms)) {

    if ((PlatformData->BtGuardInfo.MeasuredBoot) &&
      ((PlatformData->BtGuardInfo.TpmType == dTpm20) || (PlatformData->BtGuardInfo.TpmType == Ptt))) {

      //  As per PC Client spec, SRTM should perform a host platform reset
      if (PlatformData->BtGuardInfo.TpmStartupFailureOnS3) {
        ResetSystem(EfiResetCold);
        CpuDeadLoop ();
      }

      // TPM is initialized in by ACM.
      Status = TpmInit(PlatformData->BtGuardInfo.BypassTpmInit, BootMode);
    } else {
      // Initialize TPM if it has not already been initialized by BootGuard component (i.e. ACM)
      Status = TpmInit(FALSE, BootMode);
    }

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Tpm Initialization failed  %r !! \n", Status));

      Features  = GetFeatureCfg ();
      Features &= (UINT32)(~FEATURE_MEASURED_BOOT);
      SetFeatureCfg (Features);
    } else {
      if (BootMode != BOOT_ON_S3_RESUME) {
        // Create and add BootGuard Event logs in TCG Event log
        CreateTpmEventLog (PlatformData->BtGuardInfo.TpmType);
    }
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
  UINT32       *NvsData;

  //
  // Clear PWRBTNOR_STS
  //
  if (IoRead16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS) & B_ACPI_IO_PM1_STS_PRBTNOR) {
    IoWrite16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS, B_ACPI_IO_PM1_STS_PRBTNOR);
  }

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

  ///
  /// Disable Power Management Event EN
  ///
  IoAnd32 (ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_EN_127_96, (UINT32)~B_ACPI_IO_GPE0_EN_127_96_PME_B0);

  return BootMode;
}

/**
  Board specific hook points.

  Implement board specific initialization during the boot flow.

  @param[in] InitPhase             Current phase in the boot flow.

**/
VOID
BoardInit (
  IN  BOARD_INIT_PHASE  InitPhase
  )
{
  switch (InitPhase) {
  case PreConfigInit:
DEBUG_CODE_BEGIN();
    UINT32  Data;
    UINT64  AcmInfo;

    Data = *(UINT32 *) (UINTN) (0xFED30328);
    DEBUG ((DEBUG_ERROR, "[Boot Guard] AcmStatus : 0x%08X\n", Data));

    Data = *(UINT32 *) (UINTN) (0xFED300A4);
    DEBUG ((DEBUG_ERROR, "[Boot Guard] BootStatus: 0x%08X\n", Data));

    if ((Data & (BIT31 | BIT30)) != BIT31) {
      DEBUG ((DEBUG_ERROR, "[Boot Guard] Boot Guard Failed or is Disabled!\n", Data));
    } else {
      DEBUG ((DEBUG_INFO, "[Boot Guard] Boot Guard is Enabled Successfully.\n", Data));
    }

    AcmInfo = AsmReadMsr64(0x13A);
    DEBUG ((DEBUG_ERROR, "[Boot Guard] Acm Info: 0x%llx\n", AcmInfo));
    DEBUG ((DEBUG_ERROR, "[Boot Guard] Verified Boot Status: %s\n", (AcmInfo & BIT6) == BIT6 ? L"Enabled" : L"Disabled"));
    DEBUG ((DEBUG_ERROR, "[Boot Guard] Measured Boot Status: %s\n", (AcmInfo & BIT5) == BIT5 ? L"Enabled" : L"Disabled"));

DEBUG_CODE_END();
    PlatformDeviceTableInitialize ();
    SpiControllerInitialize ();
    FwuTopSwapSetting ();
    break;
  case PostConfigInit:
    PlatformIdInitialize ();
    PlatformNameInit ();
    SetBootMode (IsFirmwareUpdate() ? BOOT_ON_FLASH_UPDATE : GetPlatformPowerState());
    PlatformFeaturesInit ();
    VariableInitialize ();
    RtcInit ();
    DEBUG ((DEBUG_INFO, "Boot Mode .... %d\n",GetBootMode()));
    break;
  case PreMemoryInit:
    //
    // Set B_CNL_PCH_PWRM_GEN_PMCON_A_DISB.
    // NOTE: Byte access and not clear BIT18 and BIT16 (W1C bits)
    //
    MmioAndThenOr8 (
      PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_A + 2,
      (UINT8) ~((B_PMC_PWRM_GEN_PMCON_A_MS4V | B_PMC_PWRM_GEN_PMCON_A_SUS_PWR_FLR) >> 16),
      B_PMC_PWRM_GEN_PMCON_A_DISB >> 16
    );
    switch (GetPlatformId ()) {
    case PLATFORM_ID_ADL_S_ADP_S_CRB:
    case 0x18:
      ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTablePreMemAdlSDdr4UDimm2DCrb) / sizeof (mGpioTablePreMemAdlSDdr4UDimm2DCrb[0]), (UINT8*)mGpioTablePreMemAdlSDdr4UDimm2DCrb);
      break;
    case PLATFORM_ID_ADL_S_ADP_S_DDR5_UDIMM_1DC_CRB:
    case PLATFORM_ID_ADL_S_ADP_S_DDR5_SODIMM_CRB:
      ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTablePreMemAdlSDdr5UDimm1DCrb) / sizeof (mGpioTablePreMemAdlSDdr5UDimm1DCrb[0]), (UINT8*)mGpioTablePreMemAdlSDdr5UDimm1DCrb);
      break;
    case PLATFORM_ID_ADL_P_LP4_RVP:
      ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTablePreMemAdlPLp4Rvp) / sizeof (mGpioTablePreMemAdlPLp4Rvp[0]), (UINT8*)mGpioTablePreMemAdlPLp4Rvp);
      break;
    case PLATFORM_ID_ADL_P_LP5_RVP:
      ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTablePreMemAdlPLp5Rvp) / sizeof (mGpioTablePreMemAdlPLp5Rvp[0]), (UINT8*)mGpioTablePreMemAdlPLp5Rvp);
      break;
    case PLATFORM_ID_ADL_P_DDR5_RVP:
      ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTablePreMemAdlPDdr5Rvp) / sizeof (mGpioTablePreMemAdlPDdr5Rvp[0]), (UINT8*)mGpioTablePreMemAdlPDdr5Rvp);
      break;
    case PLATFORM_ID_TEST_S_DDR5_UDIMM_RVP:
    case PLATFORM_ID_TEST_S_DDR5_SODIMM_RVP:
      ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTableEarlyPreMemTestSDdr5UDimm1DRvp) / sizeof (mGpioTableEarlyPreMemTestSDdr5UDimm1DRvp[0]), (UINT8*)mGpioTableEarlyPreMemTestSDdr5UDimm1DRvp);
      break;
    case PLATFORM_ID_ADL_PS_DDR5_RVP:
    case PLATFORM_ID_ADL_PS_DDR5_CRB:
      ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTablePreMemAdlPsDdr5Rvp) / sizeof (mGpioTablePreMemAdlPsDdr5Rvp[0]), (UINT8*)mGpioTablePreMemAdlPsDdr5Rvp);
      break;
    case PLATFORM_ID_ADL_N_DDR5_CRB:
      ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTablePreMemAdlNDdr5Crb) / sizeof (mGpioTablePreMemAdlNDdr5Crb[0]), (UINT8*)mGpioTablePreMemAdlNDdr5Crb);
      break;
    case PLATFORM_ID_ADL_N_LPDDR5_RVP:
      ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTablePreMemAdlNLpddr5Rvp) / sizeof (mGpioTablePreMemAdlNLpddr5Rvp[0]), (UINT8*)mGpioTablePreMemAdlNLpddr5Rvp);
      break;
    default:
      DEBUG ((DEBUG_ERROR, "Could not find pre-mem GPIO for PlatformId 0x%X!\n", GetPlatformId ()));
      break;
    }
    break;
  case PostMemoryInit:
    UpdateMemoryInfo ();
    GetCrashLogData();
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
  UINT32            MrcData;
  DYNAMIC_CFG_DATA *DynCfgData;
  EFI_STATUS        Status;

  MrcData = 0;

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
  if (!EFI_ERROR (Status)) {
    if ((MrcData != 0) && (*(UINT32 *)(UINTN)MrcData != 0xFFFFFFFF)) {
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

