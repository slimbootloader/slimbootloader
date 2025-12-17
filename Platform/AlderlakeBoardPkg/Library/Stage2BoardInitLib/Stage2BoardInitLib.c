/** @file

  Copyright (c) 2020 - 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/TpmLib.h>
#include <PlatformData.h>
#include <Library/MeExtMeasurementLib.h>
#include "Stage2BoardInitLib.h"
#include "GpioTableAdlNPostMem.h"
#include "GpioTableAdlTsn.h"
#include "GpioTableTestSPostMem.h"
#include <Library/PciePm.h>
#include <Library/PlatformInfo.h>
#include <Library/LoaderPerformanceLib.h>
#include <Library/PciEnumerationLib.h>
#include <Library/FusaConfigLib.h>
#include <Library/MediaAccessLib.h>
#include "SioChip.h"

GLOBAL_REMOVE_IF_UNREFERENCED UINT8    mBigCoreCount;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8    mSmallCoreCount;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8    mModuleId;

STATIC SMMBASE_INFO mSmmBaseInfo = {
  { BL_PLD_COMM_SIG, SMMBASE_INFO_COMM_ID, 0, 0 }
};

STATIC S3_SAVE_REG mS3SaveReg = {
  { BL_PLD_COMM_SIG, S3_SAVE_REG_COMM_ID, 1, 0 },
  { { REG_TYPE_IO, WIDE32, { 0, 0}, (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN), 0x00000000 } }
};

/**
  Platform specific initialization for PCI Enum.
  Logs Device addresses of NVME controllers to boot options.

  @param[in] Bus      Device bus number
  @param[in] Dev      Device number
  @param[in] Fun      Device function number
  @param[in] Phase    Current PCI enumeration phase
**/
VOID
EFIAPI
PlatformPciEnumHookProc (
  UINT8         Bus,
  UINT8         Dev,
  UINT8         Fun,
  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE Phase
  )
{
  STATIC UINT8                            Instance = 0;
  volatile PCI_DEVICE_INDEPENDENT_REGION  *PciDev;

  // Exit on any other PCI enumeration phase
  if (Phase != EfiPciBeforeResourceCollection) {
    return;
  }

  // When an NVME controller is detected, update its boot option entry.
  PciDev = (volatile PCI_DEVICE_INDEPENDENT_REGION *) MM_PCI_ADDRESS (Bus, Dev, Fun, 0);
  if ((PciDev->ClassCode[0] == PCI_IF_MASS_STORAGE_SOLID_STATE_ENTERPRISE_NVMHCI) &&
      (PciDev->ClassCode[1] == PCI_CLASS_MASS_STORAGE_SOLID_STATE) &&
      (PciDev->ClassCode[2] == PCI_CLASS_MASS_STORAGE)) {
    DEBUG((DEBUG_INFO, "Found NVME controller at B%02X|D%02X|F%X\n", Bus, Dev, Fun));
    SetDeviceAddr (OsBootDeviceNvme, Instance, (UINT32)((Bus << 16) | (Dev << 8)));
    Instance += 1;
  }
}

/**
  Create OS config data support HOB.

  @retval EFI_SUCCESS           OS config data HOB built
  @retval EFI_NOT_FOUND         Loader Global data not found
  @retval EFI_OUT_OF_RESOURCES  Could not build HOB
**/
EFI_STATUS
BuildOsConfigDataHob (
  VOID
  )
{
  OS_CONFIG_DATA_HOB    *OsConfigData;
  GEN_CFG_DATA          *GenCfgData;
  UINT32                MemorySize;
  UINT32                PldRsvdMemSize;
  EFI_STATUS            Status;

  MemorySize   = 0;
  OsConfigData = BuildGuidHob (&gOsConfigDataGuid, sizeof (OS_CONFIG_DATA_HOB));
  if (OsConfigData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  OsConfigData->Revision = 1;

  GenCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
  if (GenCfgData != NULL) {
    if (GenCfgData->OsCrashMemorySize != 0) {
      switch (GenCfgData->OsCrashMemorySize) {
        case 0:
          MemorySize = 0;
          break;
        case 1:
          MemorySize = 0x400000;
          break;
        case 2:
          MemorySize = 0x800000;
          break;
        case 3:
          MemorySize = 0x1000000;
          break;
        case 4:
          MemorySize = 0x2000000;
          break;
        case 5:
          MemorySize = 0x4000000;
          break;
        default:
          MemorySize = 0x400000;
          break;
      }
    }
    OsConfigData->EnableCrashMode   = GenCfgData->EnableCrashMode;
  }

  OsConfigData->OsCrashMemorySize = MemorySize;
  if (MemorySize > 0) {
    PldRsvdMemSize = PcdGet32 (PcdPayloadReservedMemSize);
    Status = PcdSet32S (PcdPayloadReservedMemSize, PldRsvdMemSize + MemorySize);
    ASSERT_EFI_ERROR (Status);
  }
  return EFI_SUCCESS;
}

/**
  Create Csme Boot time HOB.

  @param[out]  CsmeBootTimeData       pointer to CSME_PERFORMANCE_INFO structure

  @retval EFI_SUCCESS           OS config data HOB built
  @retval EFI_NOT_FOUND         Loader Global data not found
  @retval EFI_OUT_OF_RESOURCES  Could not build HOB
**/
EFI_STATUS
UpdateCsmeBootPerfHob (
  OUT CSME_PERFORMANCE_INFO  *CsmeBootTimeData
  )
{
  UINT32      *EarlyBootData;
  UINT32      EarlyBootDataLength;
  UINT32      EarlyBootDataVersion;
  UINT32      AllocatedDataLength;
  EFI_STATUS  Status;

  EarlyBootData = NULL;
  EarlyBootDataLength = 0;

  if (CsmeBootTimeData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Memory allocated for boot data is passed through BootDataLength
  //
  AllocatedDataLength = CsmeBootTimeData->BootDataLength;

  Status = HeciGetEarlyBootPerfData (&EarlyBootData, &EarlyBootDataLength, &EarlyBootDataVersion);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Obtaining CSME Boot performance data failed with status: %r\n", Status));
    return Status;
  } else {
    DEBUG ((DEBUG_INFO, "Found CSME Boot performance data!\n"));
  }

  if (EarlyBootDataLength > AllocatedDataLength) {
    return EFI_OUT_OF_RESOURCES;
  }

  CsmeBootTimeData->Revision = 1;
  CsmeBootTimeData->BootDataVersion = EarlyBootDataVersion;
  CsmeBootTimeData->BootDataLength = EarlyBootDataLength;
  CopyMem (CsmeBootTimeData->BootPerformanceData, EarlyBootData, EarlyBootDataLength * sizeof (UINT32));

  return EFI_SUCCESS;
}

/**
  Set SPI flash EISS and LE
**/
VOID
ProgramSecuritySetting (
  VOID
  )
{
  UINTN            SpiBaseAddress;

  SpiBaseAddress = GetDeviceAddr (OsBootDeviceSpi, 0);
  SpiBaseAddress = TO_MM_PCI_ADDRESS (SpiBaseAddress);

  // Set the BIOS Lock Enable and EISS bits
  MmioOr8 (SpiBaseAddress + R_SPI_CFG_BC, (UINT8) (B_SPI_CFG_BC_LE | B_SPI_CFG_BC_EISS));
}

/**
  Clear SMI sources

**/
VOID
ClearSmi (
  VOID
)
{
  UINT32                SmiEn;
  UINT32                SmiSts;
  UINT32                Pm1Sts;
  UINT16                Pm1Cnt;

  SmiEn = IoRead32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
  if (((SmiEn & B_ACPI_IO_SMI_EN_GBL_SMI) !=0) && ((SmiEn & B_ACPI_IO_SMI_EN_EOS) !=0)) {
    return;
  }

  //
  // Clear the status before setting smi enable
  //
  SmiSts = IoRead32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN + 4));
  Pm1Sts = IoRead32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS));
  Pm1Cnt = IoRead16 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT));

  // Clear RTC alarm and corresponding Pm1Sts only if wake-up source is RTC SMI#
  if (((Pm1Sts & B_ACPI_IO_PM1_STS_RTC_EN) != 0) &&
      ((Pm1Sts & B_ACPI_IO_PM1_STS_RTC) != 0) &&
      ((Pm1Cnt & B_ACPI_IO_PM1_CNT_SCI_EN) == 0)) {
    IoWrite8 (R_RTC_IO_INDEX, R_RTC_IO_REGC);
    IoRead8 (R_RTC_IO_TARGET); /* RTC alarm is cleared upon read */
    Pm1Sts |= B_ACPI_IO_PM1_STS_RTC;
  }

  SmiSts |=
    (
      B_ACPI_IO_SMI_STS_SMBUS |
      B_ACPI_IO_SMI_STS_PERIODIC |
      B_ACPI_IO_SMI_STS_TCO |
      B_ACPI_IO_SMI_STS_MCSMI |
      B_ACPI_IO_SMI_STS_SWSMI_TMR |
      B_ACPI_IO_SMI_STS_APM |
      B_ACPI_IO_SMI_STS_ON_SLP_EN |
      B_ACPI_IO_SMI_STS_BIOS
    );

  Pm1Sts |=
    (
      B_ACPI_IO_PM1_STS_WAK |
      B_ACPI_IO_PM1_STS_PRBTNOR |
      B_ACPI_IO_PM1_STS_RTC |
      B_ACPI_IO_PM1_STS_PWRBTN |
      B_ACPI_IO_PM1_STS_GBL |
      B_ACPI_IO_PM1_STS_TMROF
      );

  IoWrite32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN + 4), SmiSts);
  IoWrite16 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS), (UINT16) Pm1Sts);

  // Clear GPE0 STS in case some bits are set
  IoOr32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_STS_127_96), 0);
}


/**
  Platform specific initialization for BSP and APs.

  NOTE: If there is code to access common resource in this function,
        need use lock to avoid resource access conflict.

  @param[in] CpuIndex       CPU index to initialize.
**/
VOID
EFIAPI
PlatformCpuInit (
  UINT32         CpuIndex
  )
{
  UINT8   ModuleId = 0;

  if (IsSecondaryThread()) {
    return;
  }

  if (DetectCoreType() == V_BIG_CORE_TYPE) {
    mBigCoreCount++;
  }
  else if (DetectCoreType() == V_ATOM_CORE_TYPE) {
    mSmallCoreCount++;

    ModuleId = (UINT8) (((UINT8) MpGetInfo()->CpuInfo[CpuIndex].ApicId) >> 3);
    //
    // Default value for all BITS is 0, which means small Core
    //
    if (ModuleId == ATOM_MODULE_ID_1) {
      mModuleId |= BIT0;
    } else if (ModuleId == ATOM_MODULE_ID_2) {
      mModuleId |= BIT1;
    }
  }
}

/**
  Initialize Platform Igd OpRegion

  @return VOID
 */
VOID
EFIAPI
IgdOpRegionPlatformInit (
  VOID
  )
{
  GLOBAL_NVS_AREA           *Gnvs;
  IGD_OP_PLATFORM_INFO      IgdPlatformInfo;
  EFI_STATUS                Status;

  Gnvs = (GLOBAL_NVS_AREA *)(UINTN)PcdGet32 (PcdAcpiGnvsAddress);

  IgdPlatformInfo.TurboIMON = Gnvs->SaNvs.GfxTurboIMON;

  Status = IgdOpRegionInit (&IgdPlatformInfo);
  Gnvs->SaNvs.IgdOpRegionAddress = (UINT32)(UINTN)PcdGet32 (PcdIgdOpRegionAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "VBT not found %r\n", Status));
  }
}

/**
  Fix up the memory map entry for the flash map if it exceeds 16MB

  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
FixUpFlashMapEntry (VOID)
{
  EFI_HOB_GUID_TYPE *RawHob;
  MEMORY_MAP_INFO   *MemoryMapInfo;
  UINT32            MapIndex;

  DEBUG ((DEBUG_VERBOSE, "FixUpFlashMapEntry Start\n"));
  RawHob = GetFirstGuidHob(&gLoaderMemoryMapInfoGuid);
  if (RawHob == NULL) {
    DEBUG ((DEBUG_ERROR, "Couldn't find Memory Map HOB\n"));
    return EFI_NOT_FOUND;
  }
  MemoryMapInfo = (MEMORY_MAP_INFO*)(RawHob + 1);

  DEBUG ((DEBUG_VERBOSE, "Map Entry Count %d\n", MemoryMapInfo->Count));
  for (MapIndex = 0 ; MapIndex < MemoryMapInfo->Count ; MapIndex++) {
    DEBUG ((DEBUG_VERBOSE, "Map index %d base 0x%X\n", MapIndex, MemoryMapInfo->Entry[MapIndex].Base));
    if (MemoryMapInfo->Entry[MapIndex].Base == PcdGet32(PcdFlashBaseAddress)
      && MemoryMapInfo->Entry[MapIndex].Size == PcdGet32(PcdFlashSize)
      && PcdGet32(PcdFlashSize) > SIZE_16MB)
    {
      // only first 16MB of flash is memory mapped.
      // Reserving more than that will cause memory map overlaps
      DEBUG ((DEBUG_VERBOSE, "Found Flash Map Entry\n"));
      MemoryMapInfo->Entry[MapIndex].Base = MAX_UINT32 - SIZE_16MB + 1;
      MemoryMapInfo->Entry[MapIndex].Size = SIZE_16MB;
      DEBUG ((DEBUG_VERBOSE, "Map index %d base 0x%X size 0x%X\n", MapIndex, MemoryMapInfo->Entry[MapIndex].Base, MemoryMapInfo->Entry[MapIndex].Size));
      return EFI_SUCCESS;
    }
  }
  return EFI_SUCCESS;
}

#if FixedPcdGetBool(PcdAzbSupport)
/**
 * A known hardware design issue in the CRB board where USB instance 0
 * is mapped to the eMMC device, and the actual USB interface is mapped
 * to the next instance. Consequently, in order to boot from USB flash
 * drive, instance 0 must be initialized and de-initialized before the
 * flash drive can be detected successfully.
 *
 * This workaround is necessary to work with this specific design, ensuring
 * that USB flash drives are constantly recognized.
 *
 * @return EFI_STATUS
 */

EFI_STATUS
EFIAPI
UsbDevInstanceInitDeinit()
{
  EFI_STATUS  Status;
  UINT8       DevInstance;
  UINTN       UsbPciBase;

  // USB device instance 0 need to be initialized and deinited before
  // use any other USB device instance.
  DevInstance = 0;

  DEBUG((DEBUG_INFO, "UsbDevInstanceInitDeinit\n"));

  UsbPciBase = GetDeviceAddr (OsBootDeviceUsb, DevInstance);
  UsbPciBase = TO_MM_PCI_ADDRESS (UsbPciBase);

  DEBUG ((DEBUG_INFO, "UsbPciBase(0x%x)\n", UsbPciBase));

  //
  // Init Boot device functions
  //
  Status = MediaSetInterfaceType (OsBootDeviceUsb);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Invalid Boot device configured\n"));
    return RETURN_UNSUPPORTED;
  }

  Status = MediaInitialize (UsbPciBase, DevInitAll);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  MediaInitialize(0, DevDeinit);

  return RETURN_SUCCESS;
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
  IN  BOARD_INIT_PHASE    InitPhase
  )
{
  EFI_STATUS      Status;
  UINT32          RgnBase;
  UINT32          RgnSize;
  UINT32          Length;
  UINT32          TsegBase;
  UINT32          TsegSize;
  UINTN           LpcBase;
  EFI_PEI_GRAPHICS_INFO_HOB *FspGfxHob;
  VOID                      *FspHobList;
  SILICON_CFG_DATA          *SiCfgData;
  BL_SW_SMI_INFO            *BlSwSmiInfo;
  FEATURES_DATA             *FeatureCfgData;
  UINT32                    Data;

  SiCfgData = NULL;

  switch (InitPhase) {
  case PreSiliconInit:
    EnableLegacyRegions ();
    ConfigureGpio (CDATA_GPIO_TAG, 0, NULL);
    switch (GetPlatformId ()) {
      case PLATFORM_ID_ADL_N_LPDDR5_RVP:
      case PLATFORM_ID_ADL_N_UP7EN50:
        ConfigureGpio (CDATA_NO_TAG, sizeof (mGpioTablePostMemAdlNLpddr5Rvp) / sizeof (mGpioTablePostMemAdlNLpddr5Rvp[0]), (UINT8*)mGpioTablePostMemAdlNLpddr5Rvp);
        break;
      case PLATFORM_ID_ADL_N_UP2PTWL:
      default:
        break;
    }
    if (GetBootMode() != BOOT_ON_FLASH_UPDATE) {
      UpdatePayloadId ();
    }
    SpiConstructor ();
    Status = GetComponentInfo (FLASH_MAP_SIG_VARIABLE, &RgnBase, &RgnSize);
    if (!EFI_ERROR(Status)) {
      VariableConstructor (RgnBase, RgnSize);
    }
    SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
    if (SiCfgData != NULL) {
      // Configure TSN GPIO table if TSN is enabled.
      if (SiCfgData->PchTsnEnable == 1) {
        switch (GetPlatformId ()) {
          case PLATFORM_ID_ADL_P_LP5_RVP:
          case PLATFORM_ID_ADL_P_DDR5_RVP:
          case PLATFORM_ID_RPLP_LP5_AUTO_RVP:
          case PLATFORM_ID_RPLP_LP5_AUTO_CRB:
            DEBUG ((DEBUG_WARN, "TSN GPIO: No ADL-P GPIO table available currently...\n"));
            break;
          case PLATFORM_ID_ADL_S_ADP_S_CRB:
          case PLATFORM_ID_ADL_S_ADP_S_DDR5_UDIMM_1DC_CRB:
          case PLATFORM_ID_ADL_S_ADP_S_DDR4_SODIMM_CRB:
          case PLATFORM_ID_ADL_S_ADP_S_DDR5_SODIMM_CRB:
          case PLATFORM_ID_TEST_S_DDR5_UDIMM_RVP:
          case PLATFORM_ID_TEST_S_DDR5_SODIMM_RVP:
            ConfigureGpio (CDATA_NO_TAG, sizeof (mAdlSTsnDeviceGpioTable) / sizeof (mAdlSTsnDeviceGpioTable[0]), (UINT8*)mAdlSTsnDeviceGpioTable);
            break;
          case PLATFORM_ID_ADL_N_DDR5_CRB:
          case PLATFORM_ID_ADL_N_LPDDR5_RVP:
          case PLATFORM_ID_ADL_N_UP7EN50:
            ConfigureGpio (CDATA_NO_TAG, sizeof (mAdlNTsnDeviceGpioTable) / sizeof (mAdlNTsnDeviceGpioTable[0]), (UINT8*)mAdlNTsnDeviceGpioTable);
            break;
          case PLATFORM_ID_ADL_N_UP2PTWL:
          default:
            DEBUG ((DEBUG_WARN, "TSN GPIO: Unrecognized BoardId 0x%X\n", GetPlatformId ()));
            break;
        }
      }
    }
    Status = PcdSet32S (PcdFuncCpuInitHook, (UINT32)(UINTN) PlatformCpuInit);
#if !FixedPcdGetBool(PcdAzbSupport)
    if (PcdGetBool (PcdFastBootEnabled) == FALSE) {
      if (IsPchS () || IsPchP()) {
        Status = HsPhyLoadAndInit ();
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_WARN, "HsPhyInit failure, %r\n", Status));
        }
      }
    }
#endif
    break;
  case PostSiliconInit:
    FusaConfigPostSi();
    FeatureCfgData = (FEATURES_DATA *)FindConfigDataByTag (CDATA_FEATURES_TAG);
    SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
    if (IsWdtFlagsSet(WDT_FLAG_TCC_DSO_IN_PROGRESS)) {
      WdtDisable (WDT_FLAG_TCC_DSO_IN_PROGRESS);
    }

    // Set TSEG base/size PCD
    TsegBase = MmioRead32 (TO_MM_PCI_ADDRESS (0x00000000) + R_SA_TSEGMB) & ~0xF;
    TsegSize = MmioRead32 (TO_MM_PCI_ADDRESS (0x00000000) + R_SA_BGSM) & ~0xF;
    TsegSize -= TsegBase;
    (VOID) PcdSet32S (PcdSmramTsegBase, TsegBase);
    (VOID) PcdSet32S (PcdSmramTsegSize, (UINT32)TsegSize);

    if (PcdGetBool (PcdFramebufferInitEnabled)) {
      FspGfxHob = NULL;
      FspHobList = GetFspHobListPtr ();
      if (FspHobList != NULL) {
        FspGfxHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GetGuidHobData (FspHobList, &Length, &gEfiGraphicsInfoHobGuid);
      }
      if (FspGfxHob != NULL) {
        DEBUG ((DEBUG_INFO, "FspGfxHob->FrameBufferBase = 0x%lx\n", FspGfxHob->FrameBufferBase));
        PciWrite8 (PCI_LIB_ADDRESS(IGD_BUS_NUM, IGD_DEV_NUM, IGD_FUN_NUM, PCI_COMMAND_OFFSET), \
                   EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
        PciWrite32 (PCI_LIB_ADDRESS(IGD_BUS_NUM, IGD_DEV_NUM, IGD_FUN_NUM, 0x18), \
                   (UINT32)FspGfxHob->FrameBufferBase);
      } else {
        DEBUG ((DEBUG_ERROR, "FspGfxHob is not available\n"));
      }
    }
    BuildOsConfigDataHob ();
    // Override the Smbios default Info using SMBIOS binary blob
    if (FeaturePcdGet (PcdSmbiosEnabled)) {
      LoadSmbiosStringsFromComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('S', 'M', 'B', 'S'));
    }

    if ((SiCfgData != NULL) && (SiCfgData->EcAvailable == 0)) {
      if ((FeatureCfgData != NULL) && (FeatureCfgData->S0ix == 1)) {
        DEBUG ((DEBUG_INFO, "S0ix enabled. Skipping SioInit\n"));
      } else {
        //Init SIO if EC is not available and S0ix is disabled.
        DEBUG ((DEBUG_INFO, "SioInit\n"));
        SioInit();
      }
    }

    // FIPS is disabled by default. enable it only when it is required.
    if (FeatureCfgData != NULL && (FeatureCfgData->MeFipsMode != 0)){
      DEBUG ((DEBUG_INFO, "Set HeciSetFipsMode to 0x%x\n", FeatureCfgData->MeFipsMode));
      Status = HeciSetFipsMode(FeatureCfgData->MeFipsMode);
      if (!EFI_ERROR(Status)) {
        DEBUG ((DEBUG_INFO, "Enabled FIPS mode.\n"));
      }
    }

    break;
  case PrePciEnumeration:
    (VOID) PcdSet32S (PcdPciEnumHookProc, (UINT32)(UINTN) PlatformPciEnumHookProc);
    break;
  case PostPciEnumeration:
    if (FeaturePcdGet (PcdEnablePciePm)) {
      PciePmConfig ();
    }
    // UEFI Payload will change cache type to UC based on PCI root bridge
    // info HOB MMIO range. In some cases this causes a CPU exception.
    if (GetPayloadId () != UEFI_PAYLOAD_ID_SIGNATURE) {
      Status = SetFrameBufferWriteCombining (0, MAX_UINT32);
      if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_INFO, "Failed to set GFX framebuffer as WC\n"));
      }
    }
    if ((GetPayloadId () == UEFI_PAYLOAD_ID_SIGNATURE) && (GetBootMode() == BOOT_ON_S3_RESUME)) {
      ClearSmi ();
      RestoreS3RegInfo (FindS3Info (S3_SAVE_REG_COMM_ID));
      //
      // If payload registered a software SMI handler for bootloader to restore
      // SMRR base and mask in S3 resume path, trigger sw smi
      //
      BlSwSmiInfo = FindS3Info (BL_SW_SMI_COMM_ID);
      if (BlSwSmiInfo != NULL) {
        Data = IoRead32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
        IoWrite32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN), Data | B_ACPI_IO_SMI_EN_APMC | B_ACPI_IO_SMI_EN_GBL_SMI);
        TriggerPayloadSwSmi (BlSwSmiInfo->BlSwSmiHandlerInput);
      }
    }
    break;
  case PrePayloadLoading:
    Status = FixUpFlashMapEntry();
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Error fixing up Flash Map Memory Map entry %r\n", Status));
    ///
    /// Initialize the IGD OpRegion
    ///
    }
    IgdOpRegionPlatformInit ();

    ///
    /// Initialize the HECI device (for test HeciInitLib only)
    ///
    Status = HeciInitialize (HECI1_DEVICE);
    DEBUG ((DEBUG_INFO, "HeciInitLib init Status: %r\n", Status));

    //Initialize and setup MeMeasureboot
    MeMeasuredBootInit();
#if FixedPcdGetBool(PcdAzbSupport)
    // Workaround for specific hardware design of CRB platform:
    // USB instance 0 is mapped to eMMC, so it must be initialized
    // and deinitialized to enable USB flash drive detection.
    UsbDevInstanceInitDeinit();
#endif
    break;
  case EndOfStages:
    // Register Heci Service
    HeciRegisterHeciService ();
    ClearSmi ();
    if (GetPayloadId () == UEFI_PAYLOAD_ID_SIGNATURE) {
      if (GetBootMode() != BOOT_ON_S3_RESUME) {
        ClearS3SaveRegion ();
        //
        // Set SMMBASE_INFO dummy strucutre in TSEG before others
        //
        mSmmBaseInfo.SmmBaseHdr.Count     = (UINT8) MpGetInfo()->CpuCount;
        mSmmBaseInfo.SmmBaseHdr.TotalSize = sizeof(BL_PLD_COMM_HDR) + mSmmBaseInfo.SmmBaseHdr.Count * sizeof(CPU_SMMBASE);
        AppendS3Info ((VOID *)&mSmmBaseInfo, TRUE);
        //
        // Set REG_INFO struct in TSEG region except 'Val' for regs
        //
        mS3SaveReg.S3SaveHdr.TotalSize = sizeof(BL_PLD_COMM_HDR) + mS3SaveReg.S3SaveHdr.Count * sizeof(REG_INFO);
        AppendS3Info ((VOID *)&mS3SaveReg, FALSE);
      }
    }
    if ((GetBootMode() != BOOT_ON_FLASH_UPDATE) && (GetPayloadId() != 0)) {
      ProgramSecuritySetting ();
    }

    SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
    if ( (SiCfgData != NULL) && (SiCfgData->EcAvailable == 1)) {
      //
      // Enable decoding of I/O locations 62h and 66h to LPC
      //
      LpcBase = MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_LPC, 0, 0);
      MmioOr16 (LpcBase + R_LPC_CFG_IOE, B_LPC_CFG_IOE_ME1);

      //
      // Enable EC's ACPI mode to control power to motherboard during Sleep (S3)
      //
      IoWrite16 (EC_C_PORT, EC_C_ACPI_ENABLE);
    }
    break;
  case ReadyToBoot:
    if ((GetBootMode() != BOOT_ON_FLASH_UPDATE) && (GetPayloadId() == 0)) {
      ProgramSecuritySetting ();
      //
      // set SMI LOCK (SMI_LOCK)
      // PWRM Offset 1024h Bit 4
      //
      MmioOr8 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_B, (UINT8)B_PMC_PWRM_GEN_PMCON_B_SMI_LOCK);
    }
    break;
  case EndOfFirmware:
    ClearFspHob ();
    break;
  default:
    break;
  }
}


/**
  This function will clear the DISB - DRAM Initialization Scratchpad Bit.
**/
STATIC
VOID
ClearDramInitScratchpad (
  IN  VOID
  )
{
  //
  // Clear B_PMC_PWRM_GEN_PMCON_A_DISB.
  // NOTE: Byte access to not clear BIT18 and BIT16
  //
  MmioAnd8 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_A + 2, (UINT8) ~((B_PMC_PWRM_GEN_PMCON_A_DISB | B_PMC_PWRM_GEN_PMCON_A_MS4V | B_PMC_PWRM_GEN_PMCON_A_SUS_PWR_FLR) >> 16));
}

/**
  Save MRC data onto the Flash region from NVS buffer.

  @param[in] Buffer pointer to an NVS data
  @param[in] Length Length of the NVS data(MRC)

  @retval EFI_SUCCESS
  @retval RETURN_ALREADY_STARTED
  @retval EFI_DEVICE_ERROR
  @retval EFI_INVALID_PARAMETER
 **/
EFI_STATUS
EFIAPI
SaveNvsData (
  IN  VOID    *Buffer,
  IN  UINT32   Length
  )
{
  EFI_STATUS      Status;
  UINT32          Address;
  UINT32          BaseAddress;
  UINT32          RegionSize;
  UINT32          MrcDataRegSize;

  Status = GetComponentInfo (FLASH_MAP_SIG_MRCDATA, &Address, &MrcDataRegSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "No MRC component found!\n"));
    return EFI_NOT_FOUND;
  }

  if (Length > MrcDataRegSize) {
    DEBUG ((DEBUG_INFO, "MRC component size is too small!\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Compare input data against the stored MRC training data
  // if they match, no need to update again.
  //
  if (CompareMem ((VOID *)(UINTN)Address, Buffer, Length) == 0) {
    DEBUG ((DEBUG_INFO, "Same MRC data. No need to update.\n"));
    ClearDramInitScratchpad ();
    return EFI_ALREADY_STARTED;
  }

  Status = SpiGetRegionAddress (FlashRegionBios, &BaseAddress, &RegionSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to get BIOS region!\n"));
    return Status;
  }

  BaseAddress = ((UINT32)(~RegionSize) + 1);
  if (Address < BaseAddress) {
    DEBUG ((DEBUG_INFO, "Invalid BIOS region! BaseAddress 0x%08X, Address 0x%08X\n", BaseAddress, Address));
    return EFI_ACCESS_DENIED;
  }

  Address -= BaseAddress;
  if ((Address + ALIGN_UP (Length, SIZE_4KB)) > RegionSize) {
    DEBUG ((DEBUG_INFO, "Invalid BIOS region size! Address 0x%08X, Length 0x%08X, RegionSize 0x%08X\n", Address, Length, RegionSize));
    return EFI_OUT_OF_RESOURCES;
  }

  Status = SpiFlashErase (FlashRegionBios, Address, ALIGN_UP (Length, SIZE_4KB));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to erase BIOS region!\n"));
    return Status;
  }

  Status = SpiFlashWrite (FlashRegionBios, Address, Length, Buffer);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to write MRC data!\n"));
    return Status;
  } else {
    ClearDramInitScratchpad ();
    DEBUG ((DEBUG_INFO, "Successfully wrote MRC data @ 0x%x!\n",Address));
  }

  DEBUG ((DEBUG_INFO, "SaveNvsData Done - %r\n", Status));
  return Status;
}

/**
 Update the OS boot option

 @param OsBootOptionList pointer to boot option list.
 **/
VOID
EFIAPI
UpdateOsBootMediumInfo (
  OUT  OS_BOOT_OPTION_LIST  *OsBootOptionList
  )
{
  FillBootOptionListFromCfgData (OsBootOptionList);
}

/**
 Update the frame buffer info by reading the PCI address.

 @param[out] GfxInfo pointer to global HOB data structure.
 **/
VOID
EFIAPI
UpdateFrameBufferInfo (
  OUT  EFI_PEI_GRAPHICS_INFO_HOB   *GfxInfo
  )
{
 if (PcdGetBool (PcdIntelGfxEnabled)) {
   DEBUG ((DEBUG_INFO, "Updating FB \n"));
    GfxInfo->FrameBufferBase = PciRead32 (PCI_LIB_ADDRESS (IGD_BUS_NUM, IGD_DEV_NUM, IGD_FUN_NUM, 0x18)) & 0xFFFFFF00;
  }
  DEBUG ((DEBUG_INFO, "GfxDeviceInfo Frame Buffer =0x%x\n", GfxInfo->FrameBufferBase));
}

/**
 Update the frame buffer device info.

 @param[out] GfxDeviceInfo   pointer to EFI_PEI_GRAPHICS_DEVICE_INFO_HOB structure.
**/
VOID
UpdateFrameBufferDeviceInfo (
  OUT  EFI_PEI_GRAPHICS_DEVICE_INFO_HOB   *GfxDeviceInfo
  )
{
  GfxDeviceInfo->BarIndex = 1;
  GfxDeviceInfo->VendorId = PciRead16 (PCI_LIB_ADDRESS (IGD_BUS_NUM, IGD_DEV_NUM, IGD_FUN_NUM, 0));
  GfxDeviceInfo->DeviceId = PciRead16 (PCI_LIB_ADDRESS (IGD_BUS_NUM, IGD_DEV_NUM, IGD_FUN_NUM, 2));
}

/**
 Update loader platform info.

 @param[out] LoaderPlatformInfo pointer to platform info HOB

**/
VOID
UpdateLoaderPlatformInfo (
  OUT LOADER_PLATFORM_INFO        *LoaderPlatformInfo
)
{
  PLATFORM_DATA                *PlatformData;
  UINT64                       PlatformInfoMsr;
  UINT16                       MeManuMode;
  UINT16                       PlatformPreProd;

  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
  if(PlatformData != NULL){
    //
    // Get for Sample part
    //
    PlatformInfoMsr = AsmReadMsr64 (MSR_PLATFORM_INFO);
    // Bit27 represents Sample part detail
    PlatformPreProd = (PlatformInfoMsr & BIT27) ? 1 : 0;

    //
    // Get Manufacturing Mode
    //
    HeciGetManufactureMode(&MeManuMode);

    LoaderPlatformInfo->HwState = (PlatformData->BtGuardInfo.VerifiedBoot << 0)
                                     | (PlatformData->BtGuardInfo.MeasuredBoot << 1)
                                     | (MeManuMode << 2)
                                     | (PlatformPreProd << 3) ;
    LoaderPlatformInfo->Flags   = FLAGS_SPI_DISABLE_SMM_WRITE_PROTECT;

    if (PlatformData->BtGuardInfo.TpmType == dTpm20)
      LoaderPlatformInfo->TpmType = TPM_TYPE_DTPM20;
    else if (PlatformData->BtGuardInfo.TpmType == Ptt)
      LoaderPlatformInfo->TpmType = TPM_TYPE_PTT;
    else if (PlatformData->BtGuardInfo.TpmType == TpmNone)
      LoaderPlatformInfo->TpmType = TPM_TYPE_NONE;

    DEBUG ((DEBUG_INFO, "Stage2: HwState 0x%x TpmType 0x%x\n", LoaderPlatformInfo->HwState, LoaderPlatformInfo->TpmType));
  }
}

/**
 Update loader SMM info.

 @param[out] LdrSmmInfo     pointer to SMM information HOB

**/
VOID
UpdateSmmInfo (
  OUT  LDR_SMM_INFO           *LdrSmmInfo
)
{
  if (LdrSmmInfo == NULL) {
    return;
  }
  LdrSmmInfo->SmmBase = PcdGet32 (PcdSmramTsegBase);
  LdrSmmInfo->SmmSize = PcdGet32 (PcdSmramTsegSize);
  LdrSmmInfo->Flags = SMM_FLAGS_4KB_COMMUNICATION;
  DEBUG ((DEBUG_INFO, "Stage2: SmmRamBase = 0x%x, SmmRamSize = 0x%x\n", LdrSmmInfo->SmmBase, LdrSmmInfo->SmmSize));

  //
  // Update smi ctrl register data
  //
  LdrSmmInfo->SmiCtrlReg.RegType    = (UINT8)REG_TYPE_IO;
  LdrSmmInfo->SmiCtrlReg.RegWidth   = (UINT8)WIDE32;
  LdrSmmInfo->SmiCtrlReg.SmiGblPos  = (UINT8)HighBitSet32 (B_ACPI_IO_SMI_EN_GBL_SMI);
  LdrSmmInfo->SmiCtrlReg.SmiApmPos  = (UINT8)HighBitSet32 (B_ACPI_IO_SMI_EN_APMC);
  LdrSmmInfo->SmiCtrlReg.SmiEosPos  = (UINT8)HighBitSet32 (B_ACPI_IO_SMI_EN_EOS);
  LdrSmmInfo->SmiCtrlReg.Address    = (UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN);

  //
  // Update smi status register data
  //
  LdrSmmInfo->SmiStsReg.RegType    = (UINT8)REG_TYPE_IO;
  LdrSmmInfo->SmiStsReg.RegWidth   = (UINT8)WIDE32;
  LdrSmmInfo->SmiStsReg.SmiApmPos  = (UINT8)HighBitSet32 (B_ACPI_IO_SMI_EN_APMC);
  LdrSmmInfo->SmiStsReg.Address    = (UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_STS);

  //
  // Update smi lock register data
  //
  LdrSmmInfo->SmiLockReg.RegType    = (UINT8)REG_TYPE_MMIO;
  LdrSmmInfo->SmiLockReg.RegWidth   = (UINT8)WIDE32;
  LdrSmmInfo->SmiLockReg.SmiLockPos = (UINT8)HighBitSet32 (B_PMC_PWRM_GEN_PMCON_B_SMI_LOCK);
  LdrSmmInfo->SmiLockReg.Address    = (UINT32)(PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_B);

}

/**
 Update Hob Info with platform specific data

 @param[in]  Guid          The GUID to tag the customized HOB.
 @param[out] HobInfo       The start address of GUID HOB data.
 **/
VOID
EFIAPI
PlatformUpdateHobInfo (
  IN CONST EFI_GUID              *Guid,
  OUT      VOID                  *HobInfo
)
{
  ASSERT (Guid != NULL);
  ASSERT (HobInfo != NULL);

  // Just compare Guid memory addresses which are in Stage2 data section instead of CompareGuid ()
  if (Guid == &gEfiGraphicsInfoHobGuid) {
    UpdateFrameBufferInfo (HobInfo);
  } else if (Guid == &gEfiGraphicsDeviceInfoHobGuid) {
    UpdateFrameBufferDeviceInfo (HobInfo);
  } else if (Guid == &gOsBootOptionGuid) {
    UpdateOsBootMediumInfo (HobInfo);
  } else if (Guid == &gSmmInformationGuid) {
    UpdateSmmInfo (HobInfo);
  } else if (Guid == &gLoaderPlatformInfoGuid) {
    UpdateLoaderPlatformInfo (HobInfo);
  } else if (Guid == &gCsmePerformanceInfoGuid) {
    UpdateCsmeBootPerfHob (HobInfo);
  }
}
