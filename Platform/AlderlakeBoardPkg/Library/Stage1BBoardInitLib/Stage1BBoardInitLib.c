/** @file

  Copyright (c) 2020 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage1BBoardInitLib.h"
#include <Library/PciSegmentLib.h>

// X710 X550 Link Degrade WA
#define SA_SEG_NUM                   0
#define SA_MC_BUS                    0
#define  B_PCIE_ACGR3S2_DRXTERMDQ    BIT4
#define  R_PCIE_ACGR3S2              0xC50

#define  MTRR_CACHE_UNCACHEABLE      0

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
        .PciFunctionNumber  = PCI_FUNCTION_NUMBER_PCH_XHCI_1,
        .PciDeviceNumber    = PCI_DEVICE_NUMBER_PCH_XHCI_1,
        .PciBusNumber       = DEFAULT_PCI_BUS_NUMBER_PCH,
        .IsMmioDevice       = 0
      }
    },
    .Type = OsBootDeviceUsb,
    .Instance = 1
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
        .PciFunctionNumber  = PCI_FUNCTION_NUMBER_PCH_SCS_EMMC,
        .PciDeviceNumber    = PCI_DEVICE_NUMBER_PCH_SCS_EMMC,
        .PciBusNumber       = DEFAULT_PCI_BUS_NUMBER_PCH,
        .IsMmioDevice       = 0
      }
    },
    .Type = OsBootDeviceEmmc,
    .Instance = 0
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
        .PciFunctionNumber  = PCI_FUNCTION_NUMBER_PCH_ISH,
        .PciDeviceNumber    = 0,
        .PciBusNumber       = DEFAULT_PCI_BUS_NUMBER_PCH,
        .IsMmioDevice       = 0
      }
    },
    .Type = PltDeviceIsh,
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
  Determines if plaform is an MBL board e.g. CRB or RVP
  @retval  true if board is either CRB or RVP; false otherwise
**/
BOOLEAN
EFIAPI
BoardIsMblAutomotive (
  IN  VOID
  )
{
  UINT8     BoardId;
  BoardId = (UINT8) GetPlatformId ();
  return ((BoardId == PLATFORM_ID_RPLP_LP5_AUTO_RVP) || (BoardId == PLATFORM_ID_RPLP_LP5_AUTO_CRB));
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
      // Add CRTM version event after BtG events
      TpmLogCrtmVersionEvent (BlVersion);
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
  } else if (!BoardIsMblAutomotive () && ((PmcIsRtcBatteryGood () == FALSE) || IsCMOSBad ())) {
    // Skip check if we have a coinless platform.
    // Not First boot, but RTC/CMOS is bad
    // For non coin battery design, this can be skipped.
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
    if (!BoardIsMblAutomotive () && (PmcIsRtcBatteryGood () == FALSE)) {
      // For non coin battery design, this can be skipped.
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

#if FixedPcdGetBool(PcdAzbX710X550WA)
/**
  Get CPU Pcie Root Port Device and Function Number by Root Port physical Number

  @param[in]  RpNumber              Root port physical number. (0-based)
  @param[out] RpDev                 Return corresponding root port device number.
  @param[out] RpFun                 Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port device and function is retrieved
  @retval     EFI_INVALID_PARAMETER RpNumber is invalid
**/
EFI_STATUS
EFIAPI
GetCpuPcieRpDeviceFunction (
  IN  UINTN   RpNumber,
  OUT UINTN   *RpDev,
  OUT UINTN   *RpFun
  )
{
  switch (RpNumber) {
    case 0:
      *RpDev = 6;
      *RpFun = 0;
      break;
    case 1:
      *RpDev = 1;
      *RpFun = 0;
      break;
    case 2:
      *RpDev = 6;
      *RpFun = 2;
      break;
    default:
      *RpDev = 6;
      *RpFun = 0;
      break;
  }
  return EFI_SUCCESS;
}

/**
  Gets pci segment base address of PCIe root port.

  @param[in] RpIndex    Root Port Index

  @return    PCIe       port base address.
**/
UINT64
CpuPcieBase (
  IN  UINT32   RpIndex
  )
{
  UINTN   RpDevice;
  UINTN   RpFunction;

  RpDevice   = 0;
  RpFunction = 0;

  GetCpuPcieRpDeviceFunction (RpIndex, &RpDevice, &RpFunction);
  return PCI_SEGMENT_LIB_ADDRESS (SA_SEG_NUM, SA_MC_BUS, (UINT32) RpDevice, (UINT32) RpFunction, 0);
}

/**
  Actual WA for X710 and x550 NIC Card Link Degrade Issue. Clear B_PCIE_ACGR3S2_DRXTERMDQ bit.

  @param[in] RpIndex    Root Port Index
**/
VOID
ExecuteWAForX710X550 (
  UINT32 RpIndex
  )
{
  UINT32    Data;
  UINT64    RpBase;

  RpBase = CpuPcieBase (RpIndex);
  DEBUG ((DEBUG_INFO, "Clear B_PCIE_ACGR3S2_DRXTERMDQ\n"));
  PciSegmentAnd32 (RpBase + R_PCIE_ACGR3S2, (UINT32)~(B_PCIE_ACGR3S2_DRXTERMDQ));
  Data = PciSegmentRead32 (RpBase + R_PCIE_ACGR3S2);
  DEBUG ((DEBUG_INFO, "R_PCIE_ACGR3S2 Data = %x\n", Data));
  MicroSecondDelay (25000);
}
#endif

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
  UINT8  DeviceId;
  UINT32             MsrIdx;
  UINT32             MsrMax;
  MSR_IA32_MTRRCAP_REGISTER          MsrCap;
  MSR_IA32_MTRR_PHYSMASK_REGISTER    MsrMask;
  MSR_IA32_MTRR_PHYSBASE_REGISTER    MsrBase;
  CPUID_VIR_PHY_ADDRESS_SIZE_EAX     VirPhyAddressSize;

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
    UpdateResetReason ();
    DEBUG ((DEBUG_INFO, "ResetReason = 0x%x\n",GetResetReason()));
    DEBUG ((DEBUG_INFO, "Boot Mode .... %d\n",GetBootMode()));
    // Set ISH Device Address
    DeviceId = (IsPchS () ? PCI_DEVICE_NUMBER_PCH_ISH : PCI_DEVICE_NUMBER_PCH_LP_ISH);
    SetDeviceAddr (PltDeviceIsh, 0, (UINT32)((DEFAULT_PCI_BUS_NUMBER_PCH << 16) | (DeviceId << 8) | PCI_FUNCTION_NUMBER_PCH_ISH));
    break;
  case PreMemoryInit:

#if FixedPcdGetBool(PcdAzbX710X550WA)
    // WA for X710 and x550 NIC Card Link Degrade
    DEBUG((DEBUG_INFO, "WA for x710 and x550 NIC Card in Stage 1B\n"));
    ExecuteWAForX710X550 (0);
#endif
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
    case PLATFORM_ID_RPLP_LP5_AUTO_RVP:
    case PLATFORM_ID_RPLP_LP5_AUTO_CRB:
      ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTablePreMemRplpLp5AutoRvp) / sizeof (mGpioTablePreMemRplpLp5AutoRvp[0]), (UINT8*)mGpioTablePreMemRplpLp5AutoRvp);
#if FixedPcdGetBool(PcdPcieWwanEnable)
      ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTablePreMemRplpAutoWwan) / sizeof (mGpioTablePreMemRplpAutoWwan[0]), (UINT8*)mGpioTablePreMemRplpAutoWwan);
#endif
      MaxLedInit ();
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
    case PLATFORM_ID_ADL_N_UP2PTWL:
    case PLATFORM_ID_ADL_N_UP7EN50:
      ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTablePreMemAdlNLpddr5Rvp) / sizeof (mGpioTablePreMemAdlNLpddr5Rvp[0]), (UINT8*)mGpioTablePreMemAdlNLpddr5Rvp);
      break;
    case PLATFORM_ID_AZB_LP5_CRB2A:
#if FixedPcdGetBool(PcdAzbWwanSupport)
      ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTablePreMemAzbLp5) / sizeof (mGpioTablePreMemAzbLp5[0]), (UINT8*)mGpioTablePreMemAzbLp5);

      GpioSetOutputValue ( GPIO_VER2_LP_GPP_C5, 0);
      MicroSecondDelay (20 * 1000);  // Delay by 20ms

      GpioSetOutputValue ( GPIO_VER2_LP_GPP_E5, 0);
      MicroSecondDelay (10 * 1000);  // Delay by 10ms

      GpioSetOutputValue ( GPIO_VER2_LP_GPP_D9, 0);
      MicroSecondDelay (500 * 1000);  // Delay by 500ms

      ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTableAzbM80WwanEarlyPreMem) / sizeof (mGpioTableAzbM80WwanEarlyPreMem[0]), (UINT8*)mGpioTableAzbM80WwanEarlyPreMem);

      MicroSecondDelay (20 * 1000);  // Delay by 20ms

      GpioSetOutputValue ( GPIO_VER2_LP_GPP_E5, 1);
      MicroSecondDelay (80 * 1000);  // Delay by 80ms

      GpioSetOutputValue ( GPIO_VER2_LP_GPP_C5, 1);
#endif

      break;
    default:
      DEBUG ((DEBUG_ERROR, "Could not find pre-mem GPIO for PlatformId 0x%X!\n", GetPlatformId ()));
      break;
    }
    break;
  case PostMemoryInit:
    UpdateMemoryInfo ();
    break;
  case PreTempRamExit:
    break;
  case PostTempRamExit:
    TpmInitialize();

    // Uncache(UC) memory address above Top of upper usable memory(Touum)
    // Find a free MTRR pair
    MsrCap.Uint64 = AsmReadMsr64 (MSR_IA32_MTRRCAP);
    MsrMax = MSR_IA32_MTRR_PHYSBASE0 + 2 * MsrCap.Bits.VCNT;
    for (MsrIdx = MSR_IA32_MTRR_PHYSBASE0; MsrIdx < MsrMax; MsrIdx += 2) {
      MsrMask.Uint64 = AsmReadMsr64 (MsrIdx + 1);
      if (MsrMask.Bits.V == 0) {
        break;
      }
    }
    if (MsrIdx == MsrMax) {
      DEBUG ((DEBUG_VERBOSE, "Failed to find free MTRR registers"));
    } else {

      // Program range above max memory 1MB uncachable
      AsmCpuid (CPUID_VIR_PHY_ADDRESS_SIZE, &VirPhyAddressSize.Uint32, NULL, NULL, NULL);
      DEBUG ((DEBUG_INFO, "VirPhyAddressSize.Uint32 = %llx\n", VirPhyAddressSize.Uint32));
      MsrBase.Uint64 = GetMemoryInfo (EnumMemInfoTouum);
      MsrBase.Bits.Type = MTRR_CACHE_UNCACHEABLE;
      AsmWriteMsr64 (MsrIdx,  MsrBase.Uint64);
      MsrMask.Uint64  = (~(SIZE_1MB - 1)) & (LShiftU64 (1, VirPhyAddressSize.Bits.PhysicalAddressBits) - 1);
      MsrMask.Bits.V  = 1;
      AsmWriteMsr64 (MsrIdx + 1, MsrMask.Uint64);
    }
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


