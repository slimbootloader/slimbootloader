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
#include <Library/SpiFlashLibCfl.h>
#include <Library/ConfigDataLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <ConfigDataDefs.h>
#include <FspmUpd.h>
#include <Library/DebugLib.h>
#include <Library/MmcAccessLib.h>
#include <IndustryStandard/Pci30.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <RegAccess.h>
#include <Library/CryptoLib.h>
#include <ConfigDataBlob.h>
#include "GpioTables.h"
#include <Library/PchInfoLib.h>
#include <Library/SocInitLib.h>
#include <Library/TpmLib.h>
#include <Library/BootGuardLib.h>
#include <Library/BootGuardTpmEventLogLib.h>
#include <Library/SgxLib.h>
#include <PciePreMemConfig.h>
#include <PlatformData.h>

CONST PLT_DEVICE  mPlatformDevices[]= {
  {{0x00001400}, OsBootDeviceUsb   , 0 },
  {{0x00001700}, OsBootDeviceSata  , 0 },
  {{0x00001A00}, OsBootDeviceEmmc  , 0 },
  {{0x00001F05}, OsBootDeviceSpi   , 0 }
};

CONST UINT8  mRsvdSmbusAddressTable[4] = { 0xA2, 0xA0, 0xA2, 0xA0 };
CONST UINT16 mRcompResistor[3] = { 121, 75, 100 };
CONST UINT16 mRcompTarget[5] = { 60, 26, 20, 20, 26 };

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
UpdateFspConfig (
  VOID     *FspmUpdPtr
)
{
  FSPM_UPD                        *FspmUpd;
  FSP_M_CONFIG                    *Fspmcfg;
  FSP_M_TEST_CONFIG               *FspmcfgTest;
  MEMORY_CFG_DATA                 *MemCfgData;
  GPU_CFG_DATA                    *GpuCfgData;
  UINT16                           PlatformId;
  PEG_GPIO_DATA                   *PegGpioData;

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
    Fspmcfg->MemorySpdPtr00       = (UINT32) MemCfgData->MemorySpdPtr00;
    Fspmcfg->MemorySpdPtr01       = (UINT32) MemCfgData->MemorySpdPtr01;
    Fspmcfg->MemorySpdPtr10       = (UINT32) MemCfgData->MemorySpdPtr10;
    Fspmcfg->MemorySpdPtr11       = (UINT32) MemCfgData->MemorySpdPtr11;
  }
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
  Determine if we this is firmware update mode.

  This function will determine if we have to put system in firmware update mode.

  @retval  EFI_SUCCESS           The operation completed successfully.
  @retval  others                There is error happening.
**/
BOOLEAN
IsFirmwareUpdate ()
{
  UINT16  FirmwareUpdateStatus;

  //
  // This is platform specific method. Here just use COMS.
  //
  IoWrite8(0x70, 0x40);
  FirmwareUpdateStatus = IoRead8(0x71);

  if (FirmwareUpdateStatus == 0x5A) {
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
  UINT16              CpuDid;
  UINT32              CpuFamilyModel;
  UINT8               CpuStepping;
  EFI_CPUID_REGISTER  Cpuid;

  ///
  /// Read the CPUID & DID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  CpuFamilyModel = Cpuid.RegEax & CPUID_FULL_FAMILY_MODEL;
  CpuStepping = (Cpuid.RegEax & 0xF);
  CpuDid = PciRead16 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_MC_DEVICE_ID));

  DEBUG ((DEBUG_ERROR, "CpuFamilyModel 0x%X, CpuStepping 0x%X, CpuDid 0x%X\n", CpuFamilyModel, CpuStepping, CpuDid));
  if (GetPlatformId () != 0) {
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
  Print the output of the GPIO Config table that was read from CfgData.

  @param GpioPinNum           Number of GPIO entries in the table.

  @param GpioConfData         GPIO Config Data that was read from the Configuration region either from internal or external source.

**/
VOID
PrintGpioConfigTable (
  IN UINT32              GpioPinNum,
  IN VOID*               GpioConfData
)
{
  GPIO_INIT_CONFIG  *GpioInitConf;
  UINT32            *PadDataPtr;
  UINT32             Index;

  GpioInitConf = (GPIO_INIT_CONFIG *)GpioConfData;
  for (Index  = 0; Index < GpioPinNum; Index++) {
    PadDataPtr = (UINT32 *)&GpioInitConf->GpioConfig;
    DEBUG ((DEBUG_INFO, "GPIO PAD: 0x%08X   DATA: 0x%08X 0x%08X\n", GpioInitConf->GpioPad, PadDataPtr[0], PadDataPtr[1]));
    GpioInitConf++;
  }
}

/**
  Configure GPIOs

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_INVALID_PARAMETER         Invalid group or pad number
  @retval EFI_NOT_FOUND                 GPIO configuration data not found
  #retval EFI_LOAD_ERROR                GPIO configuration data corrupted
**/
EFI_STATUS
GpioConfig (
  VOID
  )
{
  GPIO_CFG_HDR       *GpioCfgCurrHdr;
  GPIO_CFG_HDR       *GpioCfgBaseHdr;
  GPIO_CFG_HDR       *GpioCfgHdr;
  UINT32              GpioEntries;
  UINT32              Index;
  UINT32              Offset;
  UINT8              *GpioCfgDataBuffer;
  UINT8              *GpioTable;

  //Find the GPIO CFG HDR
  GpioCfgCurrHdr = (GPIO_CFG_HDR *)FindConfigDataByTag (CDATA_GPIO_TAG);
  if (GpioCfgCurrHdr == NULL) {
    return EFI_NOT_FOUND;
  }

  GpioEntries    = 0;
  GpioCfgBaseHdr = NULL;

  //Find the GPIO CFG Data based on Platform ID. GpioTableData is the start of the GPIO entries
  if (GpioCfgCurrHdr->GpioBaseTableId < 16) {
    GpioCfgBaseHdr = (GPIO_CFG_HDR *)FindConfigDataByPidTag (GpioCfgCurrHdr->GpioBaseTableId, CDATA_GPIO_TAG);
    if (GpioCfgBaseHdr == NULL) {
      DEBUG ((DEBUG_ERROR, "Cannot find base GPIO table for platform ID %d\n", GpioCfgCurrHdr->GpioBaseTableId));
      return EFI_NOT_FOUND;
    }
    if (GpioCfgCurrHdr->GpioItemSize != GpioCfgBaseHdr->GpioItemSize) {
      DEBUG ((DEBUG_ERROR, "Inconsistent GPIO item size\n"));
      return EFI_LOAD_ERROR;
    }
    GpioCfgHdr = GpioCfgBaseHdr;
  } else {
    GpioCfgHdr = GpioCfgCurrHdr;
  }

  Offset     = 0;
  GpioTable  = (UINT8 *)AllocateTemporaryMemory (0);  //allocate new buffer
  GpioCfgDataBuffer = GpioTable;

  for (Index = 0; Index  < GpioCfgHdr->GpioItemCount; Index++) {
    if (GpioCfgCurrHdr->GpioBaseTableBitMask[Index >> 3] & (1 << (Index & 7))) {
      CopyMem (GpioTable, GpioCfgHdr->GpioTableData + Offset, GpioCfgHdr->GpioItemSize);
      GpioTable += GpioCfgHdr->GpioItemSize;
      GpioEntries++;
    }
    Offset += GpioCfgHdr->GpioItemSize;
  }

  if (GpioCfgBaseHdr != NULL) {
    CopyMem (GpioTable, GpioCfgCurrHdr->GpioTableData, GpioCfgCurrHdr->GpioItemCount * GpioCfgCurrHdr->GpioItemSize);
    GpioEntries += GpioCfgCurrHdr->GpioItemCount;
  }

  DEBUG_CODE_BEGIN ();
  PrintGpioConfigTable (GpioEntries, GpioCfgDataBuffer);
  DEBUG_CODE_END ();

  return GpioConfigurePads (GpioEntries, (GPIO_INIT_CONFIG *) GpioCfgDataBuffer);
}

/**
  Initialize GPIO pads

  @param[in]  PlatformId            Indicates a specific platform id
  @param[in]  InitPhase             Board init phase. Normally, PreMemoryInit or PostMemoryInit.

  @retval     none
**/
VOID
GpioInit (
  IN  UINT16            PlatformId,
  IN  BOARD_INIT_PHASE  InitPhase
  )
{
  EFI_STATUS            Status = EFI_INVALID_PARAMETER;

  switch (InitPhase) {
  case PreMemoryInit:
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
    break;
  case PostMemoryInit:
    Status = GpioConfig ();
    break;
  default:
    break;
  }

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "GpioConfig unsuccessful = %r\n", Status));
  }
}


VOID
PlatformNameInit (
  IN VOID
)
{
  PLAT_NAME_CFG_DATA     *PlatNameConfigData;

  PlatNameConfigData = (PLAT_NAME_CFG_DATA *) FindConfigDataByTag(CDATA_PLAT_NAME_TAG);
  if (PlatNameConfigData != NULL) {
    SetPlatformName ((VOID *)&PlatNameConfigData->PlatformName);
  } else {
    DEBUG ((DEBUG_ERROR, "Platform Name config not found"));
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
  LOADER_GLOBAL_DATA          *LdrGlobal;
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

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer ();
  LdrGlobal->LdrFeatures = Features;

  DEBUG ((DEBUG_INFO, "PlatformFeaturesInit: LdrGlobal->LdrFeatures 0x%x\n",LdrGlobal->LdrFeatures));
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
  /// Clear Wake Status
  /// Also clear the PWRBTN_EN, it causes SMI# otherwise (SCI_EN is 0)
  ///
  IoWrite32 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS, ((UINT32)~B_ACPI_IO_PM1_EN_PWRBTN_EN & R_ACPI_IO_PM1_EN_MASK) | B_ACPI_IO_PM1_STS_WAK );

  if ((MmioRead8 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_B) & B_PMC_PWRM_GEN_PMCON_B_RTC_PWR_STS) != 0) {
    BootMode = BOOT_WITH_FULL_CONFIGURATION;

    ///
    /// Clear Sleep Type
    ///
    IoAndThenOr16 (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT, (UINT16) ~B_ACPI_IO_PM1_CNT_SLP_TYP, V_ACPI_IO_PM1_CNT_S0);
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
  UINT32      EmmcHcPciBase = MmPciBase(0, 0x1A, 0);
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

    Data = *(UINT32 *) (0xFED30328);
    DEBUG ((DEBUG_ERROR, "[Boot Guard] AcmStatus : 0x%08X\n", Data));

    Data = *(UINT32 *) (0xFED300A4);
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
  case PostMemoryInit:
    GpioInit (PlatformId, InitPhase);
    DEBUG ((DEBUG_INFO, "PostMemoryInit called\n"));
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

  if (*(UINT32 *)MrcData == 0xFFFFFFFF) {
    return NULL;
  } else {
    return (VOID *)MrcData;
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
  EFI_STATUS   Status;
  UINT32       Address;
  UINT32       BlobSize;
  UINT8       *Buffer;
  CDATA_BLOB  *CfgBlob;
  UINT32       SignedLen;
  UINT32       CfgDataLoadSrc;
  UINT8        Index;

  Address  = 0;
  BlobSize = sizeof(CDATA_BLOB);
  Buffer   = (UINT8 *)Dst;
  CfgBlob  = NULL;
  Status   = EFI_SUCCESS;
  CfgDataLoadSrc = PcdGet32 (PcdCfgDataLoadSource);

  // Check the validity of the Cfg Data Source:
  // Currently supported sources are BIOS, PDR.
  // Source cannot be both at the same time.
    if(CfgDataLoadSrc == FlashRegionBios) {
      Index = 1;
    } else if (CfgDataLoadSrc == FlashRegionPlatformData) {
      Index = 0;
    } else {
      return EFI_NOT_FOUND;
    }
    DEBUG ((DEBUG_INFO, "Load External Cfg data...%a\n", GetCfgDataSource(Index)));

  // Get the CfgBlob Hdr from the source
  if (CfgDataLoadSrc == FlashRegionBios) {
    Buffer = (UINT8 *)Src;
  } else if (CfgDataLoadSrc == FlashRegionPlatformData) {
    if (Len < BlobSize) {
      return EFI_OUT_OF_RESOURCES;
    } else {
      Status = SpiFlashRead (FlashRegionPlatformData, Address, BlobSize, Buffer);
    }
  }

  if (!EFI_ERROR(Status)) {
    CfgBlob = (CDATA_BLOB  *)Buffer;
  }
  // Check for CfgBlob integrity
  if((CfgBlob != NULL) &&(CfgBlob->Signature == CFG_DATA_SIGNATURE)) {
    SignedLen = CfgBlob->UsedLength;

    if (FeaturePcdGet (PcdVerifiedBootEnabled)) {
      SignedLen += RSA_SIGNATURE_AND_KEY_SIZE;
    }
    if ((SignedLen <= Len) && (SignedLen > sizeof(CDATA_BLOB))) {
      // Read the CfgBlob data into final destination
      if (CfgDataLoadSrc == FlashRegionBios) {  //BIOS
        CopyMem ((VOID *)Dst, (VOID *)Src, SignedLen);
        Status = EFI_SUCCESS;
      } else if (CfgDataLoadSrc == FlashRegionPlatformData) {  //PDR
        Status = SpiFlashRead ( FlashRegionPlatformData,
                                Address + BlobSize,
                                SignedLen - BlobSize,
                                Buffer + BlobSize);
      }
    } else {
      Status = EFI_OUT_OF_RESOURCES;
    }
  }
  return Status;
}


/**
  Get the pointer to the Built-In Config Data

  @retval UINT8*    Pointer to the Built-In Config Data
**/
UINT8 *
GetBuiltInConfigData(
  IN  VOID
)
{
  if (PcdGet32 (PcdCfgDatabaseSize) > 0) {
    return (UINT8 *) &mConfigDataBlob[16];
  } else {
    return NULL;
  }
}
