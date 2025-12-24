/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage1BBoardInitLib.h"
#include <Include/GpioV2Config.h>
#include <Library/PcdLib.h>
#include <Register/PchRegsSmbus.h>
#include <Library/MeExtMeasurementLib.h>
#include <IndustryStandard/Pci22.h>
#include <Library/PciSegmentLib.h>


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
        .PciBusNumber       = 0x1,
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
        .PciBusNumber       = 0x1,
        .IsMmioDevice       = 0
      }
    },
    .Type = OsBootDeviceNvme,
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
  },
  {
    .Dev = {
      .PciDev = {
        .PciFunctionNumber  = 0,
        .PciDeviceNumber    = 22,
#if (FixedPcdGet8(PcdMtlSSupport) == 1)
        .PciBusNumber       = 0x80,
#else
        .PciBusNumber       = 0x0,
#endif
        .IsMmioDevice       = 0
      }
    },
    .Type = PlatformDeviceMe,
    .Instance = 0
  },
  {
    // SOC P2sb 0:31:1
    .Dev = {
      .DevAddr = 0xE0000000,
    },
    .Type = PlatformDeviceP2sb,
    .Instance = 0
  },
  {
    // IOE P2sb 0:19:0
    // Use Reserved fields for high address of SMREG_BAR
    .Dev = {
      .DevAddr = 0xF0000000,
    },
    .Type = PlatformDeviceP2sb,
    .Instance = 1,
    .Reserved = 0x3FF
  },
#if (FixedPcdGet8(PcdMtlSSupport) == 1)
  {
    // PCH P2sb  0x80:31:1
    // Use Reserved fields for high address of SMREG_BAR
    .Dev = {
      .DevAddr = 0xF0000000,
    },
    .Type = PlatformDeviceP2sb,
    .Instance = 2,
    .Reserved = 0x5F
  }
#endif
};


VOID
GetBoardId (
  OUT UINT8 *BoardId
);

/**
  Initialize Smbus controller.

  @param[in]  VOID
  @retval     VOID
**/

VOID
EFIAPI
SmbusInit (
  VOID
)
{
  UINT64   SmbusBase;

  SmbusBase = PCI_SEGMENT_LIB_ADDRESS (
                DEFAULT_PCI_SEGMENT_NUMBER_PCH,
                DEFAULT_PCI_BUS_NUMBER_PCH,
                PCI_DEVICE_NUMBER_PCH_SMBUS,
                PCI_FUNCTION_NUMBER_PCH_SMBUS,
                0
                );
  //
  // Set SMBus PCI 0x64 = 0x0A0A0000
  //
  PciSegmentWrite32 (SmbusBase + R_SMBUS_CFG_64, 0x0A0A0000);
  //
  // Initialize SMBUS IO BAR
  //
  PciSegmentWrite16 (SmbusBase + R_SMBUS_CFG_BASE, SMBUS_IO_BASE);
  //
  // Enable the Smbus I/O Enable
  //
  PciSegmentOr8 (SmbusBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_IO_SPACE);
  //
  // Enable the SMBUS Controller
  //
  PciSegmentOr8 (SmbusBase + R_SMBUS_CFG_HOSTC, B_SMBUS_CFG_HOSTC_HST_EN);
}

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
#if (FixedPcdGetBool(PcdMtlSSupport) == 1)
  UINT8                 DevIndex;
#endif

  Size = sizeof (mPlatformDevices);
  PltDeviceTable = (PLT_DEVICE_TABLE *)AllocatePool (sizeof (PLT_DEVICE_TABLE) + Size);
  if (PltDeviceTable != NULL) {
    PltDeviceTable->DeviceNumber = ARRAY_SIZE (mPlatformDevices);
    CopyMem (PltDeviceTable->Device, mPlatformDevices, Size);
#if (FixedPcdGetBool(PcdMtlSSupport) == 1)
    for (DevIndex = 0 ; DevIndex < PltDeviceTable->DeviceNumber; DevIndex++) {
      if (PltDeviceTable->Device[DevIndex].Type == OsBootDeviceUsb ||
          PltDeviceTable->Device[DevIndex].Type == OsBootDeviceSata) {
        PltDeviceTable->Device[DevIndex].Dev.PciDev.PciBusNumber = 0x80;
      }
    }
#endif

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
  FEATURES_CFG_DATA           *FeaturesCfgData = NULL;
  PLATFORM_DATA               *PlatformData = NULL;
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
  } else {
    DEBUG((DEBUG_ERROR, "FEATURES_CFG_DATA is NULL\n"));
  }

  // Disable features by boot guard profile
  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if (PlatformData != NULL) {
    HeciBaseAddress = MeGetHeciMmPciAddress (0, 0);
    GetBootGuardInfo (HeciBaseAddress, &PlatformData->BtGuardInfo);
    DEBUG ((DEBUG_INFO, "GetPlatformDataPtr is copied 0x%08X \n", PlatformData));
    if (!PlatformData->BtGuardInfo.VerifiedBoot) {
      LdrFeatures &= ~FEATURE_VERIFIED_BOOT;
    }
  } else {
    DEBUG((DEBUG_ERROR, "PLATFORM_DATA is NULL\n"));
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
  PLATFORM_DATA               *PlatformData = NULL;
  BOOT_LOADER_VERSION         *BlVersion = NULL;

  BootMode     = GetBootMode();
  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
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
    //  As per PC Client spec, if TPM startup failed in ACM on S3 resume, reset
    if (PlatformData->BtGuardInfo.TpmStartupFailureOnS3) {
      ResetSystem (EfiResetCold);
      CpuDeadLoop ();
    }
    // If measured boot enabled in ACM, let ACM decide on if TPM is initialized here
    Status = TpmInit (PlatformData->BtGuardInfo.BypassTpmInit, BootMode);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Tpm Initialization failed  %r !! \n", Status));
      DisableMeasuredBoot ();
    } else if (BootMode != BOOT_ON_S3_RESUME) {
      // Add BtG events to TPM event log
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
    // Clear the PWRBTN_EN, it causes SMI# otherwise (SCI_EN is 0).
    //
    IoAnd16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_EN, (UINT16)~B_ACPI_IO_PM1_EN_PWRBTN);
  }

  ///
  /// Disable Power Management Event EN
  ///
  IoAnd32 (ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_EN_127_96, (UINT32)~B_ACPI_IO_GPE0_EN_127_96_PME_B0_EN);

  return BootMode;
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
    SmbusInit();
    FwuTopSwapSetting();
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
    break;
  case PostMemoryInit:
    UpdateMemoryInfo ();
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
    DEBUG ((DEBUG_INFO, "Dynamic Config is not NULL!\n"));
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
    DEBUG ((DEBUG_INFO, "Status Success for GetComponentInfo\n"));
    // MrcData = 0x46C51000;
    DEBUG ((DEBUG_INFO, "MRC Data @ 0x%08X!\n", MrcData));
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
  DEBUG ((DEBUG_INFO, "Load External Config Data\n"));
  return SpiLoadExternalConfigData (Dst, Src, Len);
}

