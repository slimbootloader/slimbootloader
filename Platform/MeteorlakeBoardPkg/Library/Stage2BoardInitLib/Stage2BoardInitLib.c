/** @file

  Copyright (c) 2020 - 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/TpmLib.h>
#include <PlatformData.h>
#include <Library/MeExtMeasurementLib.h>
#include "Stage2BoardInitLib.h"
#include "PsdLib.h"
#include <Library/TimerLib.h>
#include <GpioV2PinsMtlSoc.h>
#include <Library/GpioV2Lib.h>
#include <Library/PchInfoLib.h>
#include <Register/PchRegsLpc.h>

STATIC CONST UINT32 NhltSignaturesTable[] = {
  SIGNATURE_32 ('N', 'H', 'L', 'T')
  };

CONST PCH_PCIE_CONTROLLER_INFO mPchPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_1, PID_SPA,  0 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_5, PID_SPB,  4 },
  { 29, PID_SPC,  8 },
  { 29, PID_SPD, 12 },
  { 27, PID_SPE, 16 },
  { 27, PID_SPF, 20 },
};

CONST PCH_SERIAL_IO_CONFIG_INFO mPchMtlSerialIoSpiPciCfgCtrAddr[] = {
  {0,R_SERIAL_IO_PCR_PCICFGCTR11},
  {1,R_SERIAL_IO_PCR_PCICFGCTR12},
  {0,R_SERIAL_IO_PCR_PCICFGCTR13},
  {0,R_SERIAL_IO_PCR_PCICFGCTR14}
};

CONST PCH_SERIAL_IO_UART_CONFIG_INFO mPchSSerialIoUartMode[PCH_MAX_SERIALIO_UART_CONTROLLERS] = {
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART0, 1, 2, 16 },
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART1, 1, 2, 17 },
  {2,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART2, 1, 2, 33 }
};

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

  SmiEn = IoRead32 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
  if (((SmiEn & B_ACPI_IO_SMI_EN_GBL_SMI) !=0) && ((SmiEn & B_ACPI_IO_SMI_EN_EOS) !=0)) {
    return;
  }

  //
  // Clear the status before setting smi enable
  //
  SmiSts = IoRead32 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_STS));
  Pm1Sts = IoRead32 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS));
  Pm1Cnt = IoRead16 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT));

  DEBUG ((DEBUG_INFO, "ClearSmi: SmiEn: 0x%x SmiSts: 0x%x Pm1Sts: 0x%x Pm1Cnt: 0x%x\n", SmiEn, SmiSts, Pm1Sts, Pm1Cnt));

  // Clear RTC alarm and corresponding Pm1Sts only if wake-up source is RTC SMI#
  if (((Pm1Sts & B_ACPI_IO_PM1_STS_RTC_EN) != 0) &&
      ((Pm1Sts & B_ACPI_IO_PM1_STS_RTC) != 0) &&
      ((Pm1Cnt & B_ACPI_IO_PM1_CNT_SCI_EN) == 0)) {
    IoWrite8 (R_RTC_IO_INDEX, R_RTC_IO_REGC);
    IoRead8 (R_RTC_IO_TARGET); // RTC alarm is cleared upon read
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

  // Clear SMI Status
  IoWrite32 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_STS), SmiSts);

  // Clear PM1 Status (the lower 16-bit of PM1)
  IoWrite16 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS), (UINT16)Pm1Sts);

  // Clear GPE0 STS in case some bits are set
  IoOr32 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_STS_127_96), 0);
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

//Initialize Platform Igd OpRegion
VOID
EFIAPI
IgdOpRegionPlatformInit (
  VOID
  )
{
  GLOBAL_NVS_AREA           *Gnvs;
  EFI_STATUS                Status;

  Gnvs = (GLOBAL_NVS_AREA *)(UINTN)PcdGet32 (PcdAcpiGnvsAddress);

  Status = IgdOpRegionInit (NULL);
  Gnvs->SaNvs.IgdOpRegionAddress = (UINT32)(UINTN)PcdGet32 (PcdIgdOpRegionAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "VBT not found %r\n", Status));
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
  BL_SW_SMI_INFO            *BlSwSmiInfo;
  FEATURES_DATA             *FeaturesCfgData;

  switch (InitPhase) {
  case PreSiliconInit:
    EnableLegacyRegions ();
    ConfigureGpioV2 (CDATA_GPIO_TAG, NULL, 0);

    if (GetBootMode() != BOOT_ON_FLASH_UPDATE) {
      UpdatePayloadId ();
    }
    SpiConstructor ();
    Status = GetComponentInfo (FLASH_MAP_SIG_VARIABLE, &RgnBase, &RgnSize);
    if (!EFI_ERROR(Status)) {
      VariableConstructor (RgnBase, RgnSize);
    }
    Status = PcdSet32S (PcdFuncCpuInitHook, (UINT32)(UINTN) PlatformCpuInit);

    break;
  case PostSiliconInit:
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
        DEBUG ((DEBUG_INFO, "FspHobList not equal to NULL\n"));
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

    // FIPS is disabled by default. Enable it only when specified by config data.
    FeaturesCfgData = (FEATURES_DATA *)FindConfigDataByTag (CDATA_FEATURES_TAG);
    if (FeaturesCfgData != NULL && (FeaturesCfgData->MeFipsMode != 0)){
      DEBUG ((DEBUG_INFO, "Set HeciSetFipsMode to 0x%x\n", FeaturesCfgData->MeFipsMode));
      Status = HeciSetFipsMode (FeaturesCfgData->MeFipsMode);
      if (!EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "Enabled FIPS mode.\n"));
      }
    }

    break;
  case PostPciEnumeration:
    Status = SetFrameBufferWriteCombining (0, MAX_UINT32);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "Failed to set GFX framebuffer as WC\n"));
    }

    if ((GetPayloadId () == UEFI_PAYLOAD_ID_SIGNATURE) &&
        (GetBootMode() == BOOT_ON_S3_RESUME)) {
      ClearSmi ();
      RestoreS3RegInfo (FindS3Info (S3_SAVE_REG_COMM_ID));

      //
      // If payload registered a software SMI handler for bootloader to restore
      // SMRR base and mask in S3 resume path, trigger sw smi
      //
      BlSwSmiInfo = FindS3Info (BL_SW_SMI_COMM_ID);
      if (BlSwSmiInfo != NULL) {
        UINT32 Data;

        Data = IoRead32 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
        IoWrite32 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN), Data | B_ACPI_IO_SMI_EN_APMC | B_ACPI_IO_SMI_EN_GBL_SMI);
        TriggerPayloadSwSmi (BlSwSmiInfo->BlSwSmiHandlerInput);
      }
    }

    break;
  case PrePayloadLoading:
    ///
    /// Initialize the IGD OpRegion
    ///
    IgdOpRegionPlatformInit ();
    ///
    /// Initialize the HECI device (for test HeciInitLib only)
    ///
    Status = HeciInitialize (HECI1_DEVICE);
    DEBUG ((DEBUG_INFO, "HeciInitLib init Status: %r\n", Status));

    //Initialize and setup MeMeasureboot
    MeMeasuredBootInit();
    break;
  case EndOfStages:
    // Register Heci Service
    HeciRegisterHeciService ();

    ClearSmi ();
    if ((GetPayloadId () == UEFI_PAYLOAD_ID_SIGNATURE) &&
        (GetBootMode() != BOOT_ON_S3_RESUME)) {
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

    if ((GetBootMode() != BOOT_ON_FLASH_UPDATE) && (GetPayloadId() != 0)) {
      ProgramSecuritySetting ();
    }

    //
    // Enable decoding of I/O locations 62h and 66h to LPC
    //
    LpcBase = MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_LPC, 0, 0);
    MmioOr16 (LpcBase + R_LPC_CFG_IOE, B_LPC_CFG_IOE_ME1);

    //
    // Enable EC's ACPI mode to control power to motherboard during Sleep (S3)
    //
    IoWrite16 (EC_C_PORT, EC_C_ACPI_ENABLE);
    break;
  case ReadyToBoot:
    if ((GetBootMode() != BOOT_ON_FLASH_UPDATE) && (GetPayloadId() == 0)) {
      ProgramSecuritySetting ();

      // Set SMI Lock
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
  volatile PCI_DEVICE_INDEPENDENT_REGION *PciDev;
  UINT16                                 Bus;
  UINT8                                  Dev;
  UINT8                                  Instance;

  FillBootOptionListFromCfgData (OsBootOptionList);

  //
  // Depends on the PCI root bridge, connected external PCI devices, the bus number for
  // NVMe device might be different, so update the NVMe bus number in the device table.
  //
  Instance = 0;
  for (Bus = 1; Bus <= PCI_MAX_BUS; Bus++) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; Dev++) {
      PciDev = (volatile PCI_DEVICE_INDEPENDENT_REGION *) MM_PCI_ADDRESS (Bus, Dev, 0, 0);
      if (PciDev->DeviceId != 0xFFFF) {
        if ((PciDev->ClassCode[0] == 2) && (PciDev->ClassCode[1] == 8) && (PciDev->ClassCode[2] == 1)) {
          SetDeviceAddr (OsBootDeviceNvme, Instance, (UINT32)((Bus << 16) | (Dev << 8)));
          Instance += 1;
        }
      }
    }
  }

  return;
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
  DEBUG ((DEBUG_ERROR, "Stage2: SmmRamBase = 0x%x, SmmRamSize = 0x%x\n", LdrSmmInfo->SmmBase, LdrSmmInfo->SmmSize));

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
  }
}
/**
  This function updates CpuSsdt table PNVS dynamically.

  @param[in]  Table         Pointer to ACPI table
  @param[in]  Gnvs          Pointer to platform global NVS data

  @retval                   none

**/
VOID
PatchCpuSsdtTable (
  IN EFI_ACPI_DESCRIPTION_HEADER   *Table,
  IN GLOBAL_NVS_AREA               *GlobalNvs
  )
{
  CPU_NVS_AREA            *CpuNvs;
  UINT8                   *CurrPtr;
  UINT32                  *Signature;

  CpuNvs      = (CPU_NVS_AREA *) &GlobalNvs->CpuNvs;
  for (CurrPtr = (UINT8 *) Table; CurrPtr <= ((UINT8 *) Table + Table->Length); CurrPtr++) {
    Signature = (UINT32 *) (CurrPtr + 1);
    ///
    /// Update the CPU GlobalNvs area
    ///
    if ((*CurrPtr == AML_EXT_REGION_OP) && *Signature == SIGNATURE_32 ('P', 'N', 'V', 'S')) {
      ASSERT (*(UINT32 *) (CurrPtr + 1 + sizeof (*Signature) + 2) == 0xFFFF0000);
      ASSERT (*(UINT16 *) (CurrPtr + 1 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) == 0xAA55);
      ///
      /// Cpu Nvs Area address
      ///
      DEBUG ((DEBUG_INFO, "CPU PNVS Base Old=0x%08X New=0x%08X\n", *(UINT32 *)(CurrPtr + 1 + sizeof (*Signature) + 2), CpuNvs));
      *(UINT32 *) (CurrPtr + 1 + sizeof (*Signature) + 2) = (UINT32) (UINTN) CpuNvs;

      ///
      /// Cpu Nvs Area size
      ///
      DEBUG ((DEBUG_INFO, "CPU PNVS Size Old=0x%04X New=0x%04X\n", *(UINT16 *)(CurrPtr + 1 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1), sizeof (CPU_NVS_AREA)));
      *(UINT16 *) (CurrPtr + 1 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) = sizeof (CPU_NVS_AREA);

      break;
    }
  }
}

/**
  Calculate relative power

  @param[in]  BaseRatio     Maximum bus ratio
  @param[in]  CurrRatio     Current bus ratio to get relative power
  @param[in]  TdpMilliWatt  Maximum power in mW

  @retval                   Calculated power value in mW

**/
STATIC
UINT32
CalculateRelativePower (
  IN  UINT16  BaseRatio,
  IN  UINT16  CurrRatio,
  IN  UINT32  TdpMilliWatt
  )
{
  UINT64  Power1;
  UINT64  Power2;
  UINT32  BasePower;

  ASSERT (CurrRatio <= BaseRatio);

  BasePower = (TdpMilliWatt / 1000);

  Power1 = (110000 - ((BaseRatio - CurrRatio) * 625));
  Power1 = DivU64x32 (Power1, 11);
  Power1 = MultU64x64 (Power1, Power1);

  Power2 = ((CurrRatio * 100) / BaseRatio);
  Power2 = DivU64x32 (MultU64x32 (MultU64x64 (Power2, Power1), BasePower), 10000000);

  return (UINT32)Power2;
}

/**
  Update ACPI CPU P-state PSS table

  @param[in]  PssTbl        Pointer to Cpu0Ist table
  @param[in]  GlobalNvs     Pointer to platform global NVS data

  @retval EFI_SUCCESS       Updated PSS successfully
  @retval Ohteres           Errors during updating PSS table

**/
EFI_STATUS
AcpiPatchPss (
  IN  EFI_ACPI_DESCRIPTION_HEADER   *Table,
  IN  GLOBAL_NVS_AREA               *GlobalNvs
  )
{
  UINT8               *Ptr;
  UINT8               *End;
  UINT8               *Lpss;
  UINT8               *Tpss;
  UINT16              PackageTdp;
  UINT32              PackageTdpWatt;
  UINT8               ProcessorPowerUnit;
  MSR_REGISTER        MsrValue;
  PSS_PARAMS          PssParams;

  Ptr = (UINT8 *)Table;
  End = (UINT8 *)Table+ Table->Length;
  for (Lpss = NULL, Tpss = NULL; Ptr < End; Ptr++) {
    if ((Lpss == NULL) && (*(UINT32 *)Ptr == SIGNATURE_32 ('L', 'P', 'S', 'S')) && (*(Ptr - 1) == AML_NAME_OP)) {
      Lpss = Ptr;
    }
    if ((Tpss == NULL) && (*(UINT32 *)Ptr == SIGNATURE_32 ('T', 'P', 'S', 'S')) && (*(Ptr - 1) == AML_NAME_OP)) {
      Tpss = Ptr;
    }

    if ((Lpss != NULL) && (Tpss != NULL)) {
      break;
    }
  }

  if ((Lpss == NULL) && (Tpss == NULL)) {
    DEBUG ((DEBUG_INFO, "Failed to find LPSS/TPSS in Cpu0Ist\n"));
    return EFI_NOT_FOUND;
  }

  MsrValue.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  PssParams.MaxBusRatio = MsrValue.Bytes.SecondByte;
  PssParams.MinBusRatio = MsrValue.Bytes.SixthByte;

  if ((GlobalNvs->CpuNvs.PpmFlags & PPM_TURBO) != 0) {
    MsrValue.Qword = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);
    PssParams.TurboBusRatio = (UINT8)(MsrValue.Dwords.Low & 0xFF);
  } else {
    PssParams.TurboBusRatio = 0;
  }

  MsrValue.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU_UNIT);
  ProcessorPowerUnit = (UINT8)(MsrValue.Bytes.FirstByte & 0xF);
  if (ProcessorPowerUnit == 0) {
    ProcessorPowerUnit = 1;
  } else {
    ProcessorPowerUnit = (UINT8) LShiftU64 (2, (ProcessorPowerUnit - 1));
  }

  MsrValue.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
  PackageTdp = (UINT16)(MsrValue.Dwords.Low & 0x7FFF);
  PackageTdpWatt = (UINT32)DivU64x32 (PackageTdp, ProcessorPowerUnit);

  PssParams.PackageMaxPower = (PackageTdpWatt * 1000);
  PssParams.PackageMinPower = CalculateRelativePower (PssParams.MaxBusRatio,
                                                      PssParams.MinBusRatio,
                                                      PssParams.PackageMaxPower);
  PssParams.GetRelativePower = CalculateRelativePower;
  PssParams.DoListAll = FALSE;

  if (Lpss != NULL) {
    DEBUG ((DEBUG_INFO, "Patching LPSS in Cpu0Ist\n"));
    AcpiPatchPssTable (Lpss, &PssParams);
  }

  if (Tpss != NULL) {
    DEBUG ((DEBUG_INFO, "Patching TPSS in Cpu0Ist\n"));
    AcpiPatchPssTable (Tpss, &PssParams);
  }

  return EFI_SUCCESS;
}

/**
  This function will update several fields of LPI state entry in LPIT
  @param[in] Table        The table to update
  @param[in] Index        The entry index
  @param[in] Flags         The ACPI_LPI_STATE_FLAGS
  @param[in] Address     Residency counter address
  @param[in] Frequency  Residency counter freqency
**/

VOID UpdLpiStat (
    IN ACPI_LOW_POWER_IDLE_TABLE *Table,
    IN UINT8 Index,
    IN ACPI_LPI_STATE_FLAGS Flags,
    IN UINT64 Address,
    IN UINT64 Frequency
    )
{
  if (!Table)
    return;

  Table->LpiStates[Index].Flags = Flags;
  Table->LpiStates[Index].ResidencyCounter.Address = Address;
  Table->LpiStates[Index].ResidencyCounterFrequency = Frequency;
}

/**
  Update PCH NVS and SA NVS area address and size in ACPI table.

  @param[in] Current    Pointer to ACPI description header

  @retval    EFI_SUCCESS   Operation completed successfully.
  @retval    EFI_INVALID_PARAMETER Invalid paremeter.

**/
EFI_STATUS
EFIAPI
PlatformUpdateAcpiTable (
  IN UINT8                   *Current
  )
{
  EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE *MmCfg;
  EFI_ACPI_DESCRIPTION_HEADER *Table;
  UINT8                       *Ptr;
  UINT8                       *End;
  GLOBAL_NVS_AREA             *GlobalNvs;
  UINT32                       Base;
  UINT16                       Size;
  VOID                        *FspHobList;
  PLATFORM_DATA               *PlatformData;
  FEATURES_CFG_DATA           *FeaturesCfgData;
  EFI_STATUS                   Status;
  EFI_ACPI_6_3_FIXED_ACPI_DESCRIPTION_TABLE *FadtPointer;

  GlobalNvs  = (GLOBAL_NVS_AREA *)(UINTN) PcdGet32 (PcdAcpiGnvsAddress);

  Table = (EFI_ACPI_DESCRIPTION_HEADER *) Current;
  Ptr  = (UINT8 *)Table;
  End  = (UINT8 *)Table + Table->Length;

  if (Table->Signature == EFI_ACPI_5_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
    for (; Ptr < End; Ptr++) {
      if (*(Ptr-1) != AML_NAME_OP)
        continue;
      if (*(UINT32 *)Ptr == SIGNATURE_32 ('P','N','V','B')) {
        Base = (UINT32) (UINTN) &GlobalNvs->PchNvs;
        DEBUG ((DEBUG_INFO, "PNVS Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 5), Base));
        *(UINT32 *)(Ptr + 5) = Base;
      } else if (*(UINT32 *)Ptr == SIGNATURE_32 ('P','N','V','L')) {
        Size = sizeof (PCH_NVS_AREA);
        DEBUG ((DEBUG_INFO, "PNVL Old=0x%08X New=0x%08X\n", *(UINT16 *)(Ptr + 5), Size));
        *(UINT16 *)(Ptr + 5) = Size;
        // PNVL is after PNVB
        break;
      }
    }
  } else if (Table->Signature == \
    EFI_ACPI_5_0_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE) {
    MmCfg = (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE *)
            ((EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER *)Ptr + 1);
    Base  = 0;
    while ((UINT8 *)MmCfg < End) {
      MmCfg->BaseAddress = PcdGet64 (PcdPciExpressBaseAddress) + Base;
      Base += 0x10000000;
      MmCfg++;
    }
  } else if (Table->OemTableId == SIGNATURE_64 ('S', 'a', 'S', 's', 'd', 't', ' ', 0)) {
    for (; Ptr < End; Ptr++) {
      if (*(UINT32 *)Ptr == SIGNATURE_32 ('S','A','N','V')) {
        Base = (UINT32) (UINTN) &GlobalNvs->SaNvs;
        DEBUG ((DEBUG_INFO, "SANV Base Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 6), Base));
        *(UINT32 *)(Ptr + 6) = Base;

        Size = sizeof (SYSTEM_AGENT_NVS_AREA);
        DEBUG ((DEBUG_INFO, "SANV Size Old=0x%04X New=0x%04X\n", *(UINT16 *)(Ptr + 11), Size));
        *(UINT16 *)(Ptr + 11) = Size;
        break;
      }
    }
  } else if (Table->Signature == NhltSignaturesTable[0]) {
    GlobalNvs->PchNvs.NHLA[0]  = (UINT64)(UINTN) Table;
    GlobalNvs->PchNvs.NHLL[0]  = Table->Length;
    DEBUG ((DEBUG_INFO, "NHLT Base 0x%08X, Size 0x%08X\n", (UINT32)(UINTN)GlobalNvs->PchNvs.NHLA[0], GlobalNvs->PchNvs.NHLL[0]));
  }else if (Table->OemTableId == SIGNATURE_64 ('C', 'p', 'u', 'S', 's', 'd', 't', 0)) {
      DEBUG ((DEBUG_INFO, "Updated CPUSSDT Table in AcpiTable Entries\n"));
      PatchCpuSsdtTable (Table, GlobalNvs);
  } else if (Table->OemTableId == SIGNATURE_64 ('C', 'p', 'u', '0', 'I', 's', 't', 0)) {
      DEBUG ((DEBUG_INFO, "Updated CPU0IST Table in AcpiTable Entries\n"));
    //
    // Temporarily disable Pss table patch during firmware update
    //
    if (GetBootMode() != BOOT_ON_FLASH_UPDATE) {
      AcpiPatchPss (Table, GlobalNvs);
    }
  } else if (Table->Signature == SIGNATURE_32 ('R', 'T', 'C', 'T')) {
    DEBUG ((DEBUG_INFO, "Find RTCT table\n"));

    return EFI_UNSUPPORTED;
  } else if (Table->Signature == EFI_BDAT_TABLE_SIGNATURE) {
    FspHobList = GetFspHobListPtr ();
    if (FspHobList != NULL) {
      UpdateBdatAcpiTable (Table, FspHobList);
      DEBUG ((DEBUG_INFO, "Updated BDAT Table in AcpiTable Entries\n"));
    }
  } else if (Table->Signature == EFI_ACPI_6_1_LOW_POWER_IDLE_TABLE_STRUCTURE_SIGNATURE) {
      UINT8  LpitStateEntries = 0;
      UINT64 Address = 0;
      UINT64 ResidencyCounterFrequency = 0;
      ACPI_LPI_STATE_FLAGS Flags = {{ 0x0, 0x0, 0x0 }}; // Clear LPI State- Disabled bit
      LpitStateEntries = (UINT8)(((EFI_ACPI_DESCRIPTION_HEADER *)Table)->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(ACPI_LPI_NATIVE_CSTATE_DESCRIPTOR);
      if (LpitStateEntries != 0) {
        LPI_COUNTER_VALID_STS Lpi_Counter;
        //
        // mSystemConfiguration.LpitResidencyCounter : 1 - C10 Counter, 3 - SLP_S0 Based, 7 - ATX Shutdown/PS_ON
        // Expectation is
        // if "C10" is set in BIOS setup , only C10 table should be valid
        // if "SLP S0" is set in BIOS setup, both C10 and SLP S0 should be valid
        // if "ATX Shutdown/PS_ON" is set in BIOS setup, both C10 ,SLP S0, PSON should be valid
        //
        Lpi_Counter.Data8 = 0x3;
        Address = MSR_PC10_RCNTR;
        ResidencyCounterFrequency = 0; //Counter runs at TSC frequency
        UpdLpiStat((ACPI_LOW_POWER_IDLE_TABLE *)Table, 0, Flags, Address, ResidencyCounterFrequency);
          //
          // Read PWRM Base Address to fill in Residency counter Address Space
          //
        Address = PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_SLP_S0_RESIDENCY;
        ResidencyCounterFrequency = 8197;  //Counter runs at 122us granularity which implies 10KHz frequency (8197Hz)
        UpdLpiStat((ACPI_LOW_POWER_IDLE_TABLE *)Table, 1, Flags, Address, ResidencyCounterFrequency);
        if (Lpi_Counter.Bits.Ps_On) {
          //
          // Read PWRM Base Address to fill in Residency counter Address Space
          //
          Address = PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_PS_ON_COUNTER;
          ResidencyCounterFrequency = 10000;  //Counter runs at 122us granularity which implies 10KHz frequency (8197Hz)
          UpdLpiStat((ACPI_LOW_POWER_IDLE_TABLE *)Table, 2, Flags, Address, ResidencyCounterFrequency);
        }
      }
  } else if (Table->Signature == EFI_ACPI_6_3_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
    DEBUG ((DEBUG_INFO, "Updated FADT Table entries in AcpiTable\n"));
    FadtPointer = (EFI_ACPI_6_3_FIXED_ACPI_DESCRIPTION_TABLE *) Table;
    FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
    if (FeaturesCfgData != NULL) {
      if (FeaturesCfgData->Features.S0ix == 1) {
        DEBUG ((DEBUG_INFO, "Enable S0ix / Low Power S0 Idle Capable ACPI flag.\n"));
        FadtPointer->Flags = (EFI_ACPI_6_3_LOW_POWER_S0_IDLE_CAPABLE | FadtPointer->Flags);
        // set indicates the power button is handled as a control method device.
        FadtPointer->Flags |= EFI_ACPI_6_3_PWR_BUTTON;
      } else {
        FadtPointer->Flags &= ~(EFI_ACPI_6_3_PWR_BUTTON); // clear indicates the power button is handled as a fixed feature programming model
      }
    }
  }

  if (MEASURED_BOOT_ENABLED()) {
    if ((Table->Signature  == EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE) ||
        (Table->OemTableId == ACPI_SSDT_TPM2_DEVICE_OEM_TABLE_ID)) {
      Status = UpdateTpm2AcpiTable(Table);
      ASSERT_EFI_ERROR (Status);
    }
  }

  if (FeaturePcdGet (PcdVtdEnabled)) {
    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
    if (PlatformData != NULL) {
      if (PlatformData->PlatformFeatures.VtdEnable == 1) {
        if (Table->Signature == EFI_ACPI_VTD_DMAR_TABLE_SIGNATURE) {
          UpdateDmarAcpi(Table);
        }
      }
    }
  }

  //
  // Updating the ACPI table for PSD.
  //
  if (FeaturePcdGet (PcdPsdBiosEnabled)) {
    if (Table->Signature == EFI_ACPI_PSD_SIGNATURE) {
      PSD_CFG_DATA *PsdCfgData = NULL;
      PsdCfgData = (PSD_CFG_DATA *)FindConfigDataByTag (CDATA_PSD_TAG);
      if (PsdCfgData != NULL) {
        if (PsdCfgData->EnablePsd == 1) {
          Status = UpdateAcpiPsdTable ( (VOID* )Current);
          DEBUG ( (DEBUG_INFO, "Updated Psd Table in AcpiTable Entries\n") );
        }
      }
    }
  }

  //
  // EC SSDT
  //
  if (Table->Signature == EFI_ACPI_6_4_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE &&
      Table->OemTableId == SIGNATURE_64 ('E', 'c', 'S', 's', 'd', 't', ' ', 0)) {
    DEBUG((DEBUG_INFO, "Found EcSsdt\n"));
    for (; Ptr < End; Ptr++) {
      if (*(UINT32 *)Ptr == SIGNATURE_32 ('E','N','V','S')) {
        Base = (UINT32) (UINTN) &GlobalNvs->EcNvs;
        DEBUG ((DEBUG_INFO, "ENVS Base Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 6), Base));
        *(UINT32 *)(Ptr + 6) = Base;

        Size = sizeof (EC_NVS_AREA);
        DEBUG ((DEBUG_INFO, "ENVS Size Old=0x%04X New=0x%04X\n", *(UINT16 *)(Ptr + 11), Size));
        *(UINT16 *)(Ptr + 11) = Size;
        break;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
 Update CPU NVS from CpuInitDataHob

 @param[in, out]  CpuNvs           Pointer to CPU NVS region

**/
VOID
UpdateCpuNvs (
  IN OUT CPU_NVS_AREA                   *CpuNvs
  )
{
  EFI_HOB_GUID_TYPE          *GuidHob;
  CPU_INIT_DATA_HOB          *CpuInitDataHob;
  VOID                       *FspHobList;
  MSR_FLEX_RATIO_REGISTER     FlexRatio;
  MSR_TEMPERATURE_TARGET_REGISTER TempTarget;
  UINT8                       MaxRefTemp;
  CPU_SKU                     CpuSku;
  UINT16                      C6Latency = 0;
  UINT16                      C7Latency = 0;
  UINT16                      C8Latency = 0;
  UINT16                      C9Latency = 0;
  UINT16                      C10Latency = 0;
  UINT8                       Index;
  CPUID_PROCESSOR_FREQUENCY_ECX  ProcessorFreq;
  MSR_PLATFORM_INFO_REGISTER     PlatformInfoMsr;
  CPU_POWER_MANAGEMENT_DATA_HOB  *CpuPowerMgmtDataHob;

  if (CpuNvs == NULL) {
    DEBUG ((DEBUG_ERROR, "Invalid Cpu Nvs pointer!!!\n"));
    return;
  }

  ///
  /// Get CPU Init Data Hob
  ///
  GuidHob = NULL;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    GuidHob = GetNextGuidHob (&gCpuInitDataHobGuid, FspHobList);
  }
  if (GuidHob == NULL) {
    DEBUG ((DEBUG_ERROR, "CPU Data HOB not available\n"));
    return;
  }

  CpuInitDataHob = GET_GUID_HOB_DATA (GuidHob);

  ///
  /// Get CPU Power Management Data Hob
  ///
  CpuPowerMgmtDataHob = GetNextGuidHob (&gCpuPowerManagementDataHobGuid, FspHobList);
  if (CpuPowerMgmtDataHob == NULL) {
    DEBUG ((DEBUG_ERROR, "CPU Power Management Data Hob not available\n"));
    return;
  }
  ///
  /// Update NVS ASL items.
  ///
  CpuNvs->Cpuid = GetCpuFamily() | GetCpuStepping();
  CpuNvs->Revision = 1;

  ///
  /// Calculate the number of Oc bins supported. Read in MSR 194h FLEX_RATIO bits (19:17)
  ///
  FlexRatio.Uint64 = AsmReadMsr64(MSR_FLEX_RATIO);
  CpuNvs->OcBins = (UINT8) FlexRatio.Bits.OcBins;

  ///
  /// Automatic Thermal Reporting for Thermal Management
  ///
  if (CpuInitDataHob->EnableAllThermalFunctions) {
    ///
    /// Thermal Reporting for Critical trip
    /// MSR 1A2 bits 23:16 define the temperature that this specific processor can
    /// function upto. It is recommended that this value + 5  be used as default Critical trip point
    /// _CRT.
    ///
    TempTarget.Uint64 = AsmReadMsr64 (MSR_TEMPERATURE_TARGET);
    MaxRefTemp = (UINT8) TempTarget.Bits.RefTemp;
    CpuNvs->AutoCriticalTripPoint = MaxRefTemp + 5;

  ///
    /// Thermal Reporting for Active Thermal Management
    /// It is recommended that the processor specific value in MSR 1A2 bits 15:8
    /// be used as the highest Active trip point i.e. _AC0.
    ///
    CpuNvs->AutoActiveTripPoint = MaxRefTemp - (UINT8) TempTarget.Bits.FanTempTargetOfst;

    ///
    /// Thermal Reporting for Passive Thermal Management
    /// On all turbo enabled systems, it is recommended that the ACPI _PSV point be
    /// set to a temperature above the Active cooling temperature and Tcc activation
    /// temperature.
    /// If platform embedded controller will issue PECI commands to reduce power as a
    /// passive thermal action, then it is recommended to use the package's max temperature
    /// for passive thermal control.
    ///
    CpuNvs->AutoPassiveTripPoint = CpuNvs->AutoCriticalTripPoint + 3;
  }

  ///
  ///  Update the PPM NVS area PPM flags
  ///
  CpuNvs->PpmFlags = CpuInitDataHob->PpmFlags;
  DEBUG ((DEBUG_INFO, "PpmFlags CFGD = 0x%08X\n", CpuNvs->PpmFlags ));
  ///
  /// For U/Y, hard coded latency values are used instead of calculations. Set values here.
  ///
  CpuSku = GetCpuSku();
  switch (CpuSku) {
    case EnumCpuUlt:
    case EnumCpuUlx:
    case EnumCpuHalo:
    case EnumCpuTrad:
      C6Latency  = C6_LATENCY;
      C7Latency  = C7_LATENCY;
      C8Latency  = C8_LATENCY;
      C9Latency  = C9_LATENCY;
      C10Latency = C10_LATENCY;
    break;
    default:
    break;
  }

  ///
  /// Update PPM NVRAM Values for C6
  ///
  if (CpuNvs->PpmFlags & C6_LONG_LATENCY_ENABLE) {
    CpuNvs->C6MwaitValue = MWAIT_C6_1;
  }
  else {
    CpuNvs->C6MwaitValue = MWAIT_C6;
  }

  ///
  /// Update PPM NVRAM Values for C7 - select the C-state supported among- C7 / C7S
  ///
  if (CpuNvs->PpmFlags & PPM_C7) { // Is C7 supported ?
    if (CpuNvs->PpmFlags & C7_LONG_LATENCY_ENABLE) {
      CpuNvs->C7MwaitValue = MWAIT_C7_1;
    }
    else {
      CpuNvs->C7MwaitValue = MWAIT_C7;
    }
  }
  if (CpuNvs->PpmFlags & PPM_C7S) { // Is C7S supported ?
    if (CpuNvs->PpmFlags & C7s_LONG_LATENCY_ENABLE) {
      CpuNvs->C7MwaitValue = MWAIT_C7_3;
    }
    else {
      CpuNvs->C7MwaitValue = MWAIT_C7_2;
    }
  }

  ///
  /// Hard coded values are used instead of calculations. Reinitialize here to simplify the code for C6 and C7.
  ///

  if (C6Latency != 0) {
    CpuNvs->C6Latency = C6Latency;
  }

  if (C7Latency != 0) {
    CpuNvs->C7Latency = C7Latency;
  }

  ///
  /// Update PPM NVRAM Values for CD - select the deepest C-state supported among- C8 / C9 / C10
  ///
  if (CpuNvs->PpmFlags & PPM_C10) { // C10 supported
    CpuNvs->CDIOLevel = PCH_ACPI_LV7;
    CpuNvs->CDPowerValue = C10_POWER;
    CpuNvs->CDMwaitValue = MWAIT_CD_2;
    if (C10Latency != 0) {
      CpuNvs->CDLatency = C10Latency;
    }
  }
  else if(CpuNvs->PpmFlags & PPM_C9) { // C9 supported
    CpuNvs->CDIOLevel = PCH_ACPI_LV6;
    CpuNvs->CDPowerValue = C9_POWER;
    CpuNvs->CDMwaitValue = MWAIT_CD_1;
    if (C9Latency != 0) {
      CpuNvs->CDLatency = C9Latency;
    }
  }
  else if (CpuNvs->PpmFlags & PPM_C8) { // C8 supported
    CpuNvs->CDIOLevel = PCH_ACPI_LV5;
    CpuNvs->CDPowerValue = C8_POWER;
    CpuNvs->CDMwaitValue = MWAIT_CD;
    if (C8Latency != 0) {
      CpuNvs->CDLatency = C8Latency;
    }
  }

  if (CpuNvs->PpmFlags & PPM_HWP) {
    CpuNvs->HwpEnable = 1;
  }

  CpuNvs->CtdpLevelsSupported = CpuInitDataHob->CtdpLevelsSupported;                        /// ConfigTdp Number Of Levels.
  CpuNvs->ConfigTdpBootModeIndex = CpuInitDataHob->ConfigTdpBootModeIndex;                 /// CTDP Boot Mode Index
  for (Index = 0; Index < 3; Index++) {
    CpuNvs->CtdpTar[Index] = CpuInitDataHob->CtdpTar[Index];                             /// CTDP Levels TAR
    CpuNvs->CtdpPowerLimit1[Index] = CpuInitDataHob->CtdpPowerLimit1[Index];             /// CTDP Levels Power Limit1
    CpuNvs->CtdpPowerLimit2[Index] = CpuInitDataHob->CtdpPowerLimit2[Index];             /// CTDP Levels Power Limit2
    CpuNvs->CtdpPowerLimitWindow[Index] = CpuInitDataHob->CtdpPowerLimitWindow[Index];    /// CTDP Levels Power Limit1 Time Window
    CpuNvs->CtdpCtc[Index] = CpuInitDataHob->CtdpCtc[Index];                             /// CTDP Levels CTC
    CpuNvs->CtdpPpc[Index] = CpuInitDataHob->CtdpPpc[Index];                             /// CTDP Levels PPC
  }

  CpuNvs->ConfigurablePpc = CpuInitDataHob->ConfigurablePpc;                               /// Max ratio of the slowest core
  CpuNvs->EnableRp        = CpuPowerMgmtDataHob->EnableRp;                                           /// Enable/Disable Resource Priority Feature
  CpuNvs->CoreType       = 0xFF00;
  CpuNvs->HeteroStatus   = IsHeteroCoreSupported();

  AsmCpuid (CPUID_PROCESSOR_FREQUENCY, NULL, NULL, &ProcessorFreq.Uint32, NULL);
  CpuNvs->BusFrequency = (UINT16) ProcessorFreq.Bits.BusFrequency;

  PlatformInfoMsr.Uint64 = AsmReadMsr64 (MSR_PLATFORM_INFO);
  CpuNvs->MaxP1Ratio     = (UINT8) PlatformInfoMsr.Bits.MaxNonTurboLimRatio;
  DEBUG ((DEBUG_INFO, "Update Cpu Nvs Done\n"));
}

/**
 Update APCI values in the Gnvs.

 @param[in] GnvsIn  pointer to platform Gnvs

**/
VOID
EFIAPI
PlatformUpdateAcpiGnvs (
  IN VOID  *GnvsIn
  )
{
  GLOBAL_NVS_AREA         *GlobalNvs;
  PLATFORM_NVS_AREA       *PlatformNvs;
  PCH_NVS_AREA            *PchNvs;
  CPU_NVS_AREA            *CpuNvs;
  SYSTEM_AGENT_NVS_AREA   *SaNvs;
  SYS_CPU_INFO            *SysCpuInfo;
  DPTF_NVS_AREA           *DptfNvs;
  FSPS_UPD                *FspsUpd;
  FSP_S_CONFIG            *FspsConfig;
  SILICON_CFG_DATA        *SiCfgData;
  FEATURES_CFG_DATA       *FeaturesCfgData;
  UINT8                    Index;
  UINT8                    Length;
  UINT8                    RpDev;
  UINT8                    RpFun;
  UINT32                   Data32;
  UINT8                    GroupIndex[3];

  GlobalNvs = (GLOBAL_NVS_AREA *)GnvsIn;
  ZeroMem (GlobalNvs, sizeof (GLOBAL_NVS_AREA));
  PlatformNvs = (PLATFORM_NVS_AREA *) &GlobalNvs->PlatformNvs;
  PchNvs      = (PCH_NVS_AREA *) &GlobalNvs->PchNvs;
  CpuNvs      = (CPU_NVS_AREA *) &GlobalNvs->CpuNvs;
  DptfNvs     = (DPTF_NVS_AREA*) &GlobalNvs->DptfNvs;
  SaNvs       = (SYSTEM_AGENT_NVS_AREA *) &GlobalNvs->SaNvs;
  FspsUpd     = (FSPS_UPD *)(UINTN)PcdGet32 (PcdFspsUpdPtr);
  FspsConfig  = &FspsUpd->FspsConfig;

  //PCH NVS Init
  PchNvs->PchSeries     = PchSeries();
  PchNvs->PchGeneration = PchGeneration();
  PchNvs->PchStepping   = PchStepping();
  PchNvs->PchAttached   = MtlSocIsPchAttached();
  DEBUG ((DEBUG_INFO, "PchSeries = 0x%x\n", PchNvs->PchSeries));
  DEBUG ((DEBUG_INFO, "PchGeneration = 0x%x\n", PchNvs->PchGeneration));
  DEBUG ((DEBUG_INFO, "PchStepping = 0x%x\n", PchNvs->PchStepping));
  DEBUG ((DEBUG_INFO, "PchAttached = 0x%x\n", PchNvs->PchAttached));


  //
  // Update ASL PCIE port address according to root port device and function
  //
  for (Index = 0; Index < GetPchMaxPciePortNum (); Index++) {
    RpDev = PchPcieRpDevNumber (Index);
    RpFun = PchPcieRpFuncNumber (Index);
    DEBUG ((DEBUG_INFO, "RpDev: 0x%x\n", RpDev));
    DEBUG ((DEBUG_INFO, "RpFun: 0x%x\n", RpFun));
    Data32 = ((UINT8) RpDev << 16) | (UINT8) RpFun;
    PchNvs->RpAddress[Index] = Data32;
    DEBUG ((DEBUG_INFO, "RpAddress[%d] = 0x%08X\n", Index, PchNvs->RpAddress[Index]));

    // Need to match with FSP-S UPD
    PchNvs->LtrEnable[Index]                = FspsConfig->PcieRpLtrEnable[Index];
    PchNvs->PcieLtrMaxSnoopLatency[Index]   = FspsConfig->PcieRpLtrMaxSnoopLatency[Index];
    PchNvs->PcieLtrMaxNoSnoopLatency[Index] = FspsConfig->PcieRpLtrMaxNoSnoopLatency[Index];
  }

  // TSN
  SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if ((SiCfgData != NULL) && (SiCfgData->PchTsnEnable)) {
    PlatformNvs->TsnPcsEnabled  = 1;
  }

  // COM port enabling
  if (IsPchS ()) {
    PlatformNvs->PcdH8S2113SIO              = 0;
  }

  //
  // Update HPET base address.
  //
  PchNvs->HPTE          = TRUE;
  PchNvs->HPTB          = HPET_BASE_ADDRESS;

  //
  // Update SBREG_BAR.
  //
  PchNvs->SBRG          = PCH_PCR_BASE_ADDRESS;

  //
  // Update PMC ACPIBASE and PWRMBASE
  //
  PchNvs->PMBS          = ACPI_BASE_ADDRESS;

  PchNvs->PWRM          = PCH_PWRM_BASE_ADDRESS;

  //
  // GPIO device
  //
  PchNvs->SGIR  = 0xE;
  PchNvs->GPHD  = 0;

    GpioGetGpeMapping (GPIOV2_MTL_SOC_M_CHIPSET_ID, &GroupIndex[0], &GroupIndex[1], &GroupIndex[2]);

  PchNvs->GEI0 = GroupIndex[0];
  PchNvs->GEI1 = GroupIndex[1];
  PchNvs->GEI2 = GroupIndex[2];
  PchNvs->GED0 = (UINT8) 0x0;
  PchNvs->GED1 = (UINT8) 0x0;
  PchNvs->GED2 = (UINT8) 0x0;
  DEBUG ((DEBUG_INFO, "GEI [0x%X 0x%X 0x%X], GED [0x%X 0x%X 0x%X]\n",
    PchNvs->GEI0, PchNvs->GEI1, PchNvs->GEI2,
    PchNvs->GED0, PchNvs->GED1, PchNvs->GED2));

  //Serial IO Devices
  //SPI
  Length = GetPchMaxSerialIoSpiControllersNum ();
  for (Index = 0; Index < Length ; Index++) {
    PchNvs->SC0[Index] = MtlPchSerialIoSpiPciCfgBase (Index);
    PchNvs->SM0[Index] = FspsConfig->SerialIoSpiMode[Index];
  }

  //I2C
  Length = GetPchMaxSerialIoI2cControllersNum ();
  for (Index = 0; Index < Length ; Index++) {
    PchNvs->IC0[Index] = MtlPchSerialIoI2cPciCfgBase (Index);
    PchNvs->IM0[Index] = FspsConfig->SerialIoI2cMode[Index];
  }

  PchNvs->ClockToRootPortMap[0] = 0x80;
  PchNvs->ClockToRootPortMap[1] = 0x9;
  PchNvs->ClockToRootPortMap[2] = 0x70;
  PchNvs->ClockToRootPortMap[3] = 0xFF;
  PchNvs->ClockToRootPortMap[4] = 0x8;
  PchNvs->ClockToRootPortMap[5] = 0x7;
  PchNvs->ClockToRootPortMap[6] = 0xB;
  PchNvs->ClockToRootPortMap[7] = 0xA;
  PchNvs->ClockToRootPortMap[8] = 0x9;
  PchNvs->ClockToRootPortMap[9] = 0x0;
  PchNvs->ClockToRootPortMap[10] = 0x0;
  PchNvs->ClockToRootPortMap[11] = 0x0;
  PchNvs->ClockToRootPortMap[12] = 0x0;
  PchNvs->ClockToRootPortMap[13] = 0x0;
  PchNvs->ClockToRootPortMap[14] = 0x0;
  PchNvs->ClockToRootPortMap[15] = 0x0;
  PchNvs->ClockToRootPortMap[16] = 0x0;
  PchNvs->ClockToRootPortMap[17] = 0x0;
  PchNvs->ClockReq[0] = 0;
  PchNvs->ClockReq[1] = 1;
  PchNvs->ClockReq[2] = 2;
  PchNvs->ClockReq[3] = 3;
  PchNvs->ClockReq[4] = 4;
  PchNvs->ClockReq[5] = 5;
  PchNvs->ClockReq[6] = 6;
  PchNvs->ClockReq[7] = 7;
  PchNvs->ClockReq[8] = 8;
  PchNvs->ClockReq[9] = 0;
  PchNvs->ClockReq[10] = 0;
  PchNvs->ClockReq[11] = 0;
  PchNvs->ClockReq[12] = 0;
  PchNvs->ClockReq[13] = 0;
  PchNvs->ClockReq[14] = 0;
  PchNvs->ClockReq[15] = 0;

  //UART
  Length = GetPchMaxSerialIoUartControllersNum ();
  for (Index = 0; Index < Length ; Index++) {
    PchNvs->UM0[Index] = FspsConfig->SerialIoUartMode[Index];
    if (FspsConfig->SerialIoUartMode[Index] == 1) {
      PchNvs->UC0[Index] = SerialIoUartPciCfgBase(Index);
    } else if (FspsConfig->SerialIoUartMode[Index] == 3) {
      PchNvs->UC0[Index] = PCH_SERIAL_IO_BASE_ADDRESS + 0xD000 + 0x2000 * Index; /* COM */
    }
    PchNvs->UD0[Index] = FspsConfig->SerialIoUartDmaEnable[Index];
    PchNvs->UP0[Index] = FspsConfig->SerialIoUartPowerGating[Index];
    if (Index < PCH_MAX_SERIALIO_UART_CONTROLLERS) {
      PchNvs->UI0[Index] = mPchSSerialIoUartMode[Index].SerialIoUARTIrq;
    }
  }

  PchNvs->UI0[2] = 31;
  PchNvs->CnviBtCore         = FspsConfig->CnviBtCore;
  PlatformNvs->CnviBtAudioOffload = FspsConfig->CnviBtAudioOffload;
  PchNvs->PsOnEnable         = FspsConfig->PsOnEnable;
  PchNvs->CnviPortId         = MTL_SOC_PID_CNVI;
  PchNvs->IclkPid            = MTL_SOC_PID_ICLK;
  PchNvs->GBES               = 1;
  PchNvs->CpuSku             = GetCpuSku();

  // Update HDA ACPI
  PchNvs->XTAL                                  = V_EPOC_XTAL_38_4_MHZ;
  PchNvs->ADFM                                  = 0x101;  //DspFeatureMask
  PchNvs->SWQ0                                  = 2;
  PchNvs->SWQ1                                  = 2;
  PchNvs->SWQ2                                  = 2;
  PchNvs->SWQ3                                  = 2;
  PchNvs->ACS0                                  = 0;
  PchNvs->ACS1                                  = 0;
  PchNvs->ACS2                                  = 0;
  PchNvs->ACS3                                  = 0;
  PchNvs->DAI0                                  = 1;
  PchNvs->DAI1                                  = 1;
  PchNvs->DAI2                                  = 1;
  PchNvs->DAI3                                  = 1;
  PchNvs->DOD0                                  = 1;
  PchNvs->DOD1                                  = 1;
  PchNvs->DOD2                                  = 1;
  PchNvs->DOD3                                  = 1;
  PchNvs->SWMC                                  = 4;
  PchNvs->AFDF                                  = 0;

  //
  // Update PCH USB ports count
  //
  PchNvs->PchUsb2PortCount = (UINT8) GetPchUsb2MaxPhysicalPortNum ();
  PchNvs->PchUsb3PortCount = (UINT8) GetPchXhciMaxUsb3PortNum ();

  //Platform NVS Init
  PlatformNvs->ApicEnable                   = 1;
  PlatformNvs->PlatformId = (UINT8) GetPlatformId ();
  PlatformNvs->PowerState                   = 1;
  if ((SiCfgData != NULL) && (SiCfgData->EcAvailable)) {
    PlatformNvs->EcAvailable            = SiCfgData->EcAvailable;
    DEBUG ((DEBUG_INFO, "SiCfgData->EcAvailable  = 0x%X\n",SiCfgData->EcAvailable));
  }

  SysCpuInfo = MpGetInfo ();
  DEBUG ((DEBUG_INFO, "Num CPUs  = 0x%X\n",(UINT8)SysCpuInfo->CpuCount));
  if ( SysCpuInfo != NULL ) {
    PlatformNvs->ThreadCount      = (UINT8)SysCpuInfo->CpuCount;
  }

  PlatformNvs->Ps2MouseEnable    = 0x0;
  PlatformNvs->NativePCIESupport = 1;
  PlatformNvs->PciDelayOptimizationEcr = 0;
  PlatformNvs->UsbTypeCSupport = 1;

  PlatformNvs->GenerationId = 0;
  PlatformNvs->PlatformFlavor = FlavorDesktop;
  PlatformNvs->BoardRev = 1;
  PlatformNvs->BoardType = 0;
  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
  if (FeaturesCfgData != NULL) {
    if (FeaturesCfgData->Features.S0ix == 1) {
      PlatformNvs->LowPowerS0Idle                   = 1;
      PlatformNvs->LowPowerS0IdleConstraint = GetLowPowerS0IdleConstraint();
    }
    else{
      PlatformNvs->LowPowerS0Idle = 0;
    }
  }

  PlatformNvs->I2SC     = 0x0;
  PlatformNvs->I2SI     = GPIOV2_MTL_SOC_M_GPP_F9;
  PlatformNvs->I2SB     = 0x0;
  PlatformNvs->HdaDspPpModuleMask = 0;

  PlatformNvs->Ac1TripPoint                 = 55;
  PlatformNvs->Ac0TripPoint                 = 71;
  PlatformNvs->Ac1FanSpeed                  = 75;
  PlatformNvs->Ac0FanSpeed                  = 100;
  PlatformNvs->PassiveThermalTripPoint      = 95;
  PlatformNvs->PassiveTc1Value              = 1;
  PlatformNvs->PassiveTc2Value              = 5;
  PlatformNvs->PassiveTspValue              = 10;
  PlatformNvs->CriticalThermalTripPoint     = 119;

  //
  // Intel(R) Dynamic Tuning Technology Devices and trip points
  //
   DptfNvs->EnableDptf                   = 0;

  //
  // Wireless
  //
  PlatformNvs->PowerSharingManagerEnable    = 0;

  PlatformNvs->WifiEnable                   = 0;


  // Miscellaneous
  //
  switch (GetPlatformId())
  {
    case BoardIdMtlPDdr5SODimmSbsRvp:
      PlatformNvs->PcieSlot1WakeGpio = 0;
      PlatformNvs->PcieSlot1RpNumber = 0;
      PlatformNvs->PcieSlot1PowerEnableGpio = 0;
      PlatformNvs->PcieSlot1RstGpio = 0;

      PlatformNvs->PcieSlot2RpNumber = 0;

      PlatformNvs->PcieSlot3WakeGpio = GPIOV2_MTL_SOC_M_GPP_C2;
      PlatformNvs->PcieSlot3RpNumber = 6;
      PlatformNvs->PcieSlot3PowerEnableGpio = GPIOV2_MTL_SOC_M_GPP_A18;
      PlatformNvs->PcieSlot3PowerEnableGpioPolarity = 1;
      PlatformNvs->PcieSlot3RstGpio = GPIOV2_MTL_SOC_M_GPP_A19;

      PlatformNvs->PcieSlot4WakeGpio = GPIOV2_MTL_SOC_M_GPP_F20;
      PlatformNvs->PcieSlot4RpNumber = 15;
      PlatformNvs->PcieSlot4PowerEnableGpio = GPIOV2_MTL_SOC_M_GPP_D3;
      PlatformNvs->PcieSlot4RstGpio = GPIOV2_MTL_SOC_M_GPP_B9;

      PlatformNvs->PcieSlot5WakeGpio = GPIOV2_MTL_SOC_M_GPP_F19;
      PlatformNvs->PcieSlot5PowerEnableGpio = GPIOV2_MTL_SOC_M_GPP_B15;
      PlatformNvs->PcieSlot5PowerEnableGpioPolarity = 1;
      PlatformNvs->PcieSlot5RstGpio = GPIOV2_MTL_SOC_M_GPP_A20;

      PlatformNvs->M2Ssd1PowerEnableGpio = GPIOV2_MTL_SOC_M_GPP_D5;
      PlatformNvs->M2Ssd1RstGpio = GPIOV2_MTL_SOC_M_GPP_D2;

      PlatformNvs->M2Ssd2PowerEnableGpio = GPIOV2_MTL_SOC_M_GPP_D1;
      PlatformNvs->M2Ssd2RstGpio = GPIOV2_MTL_SOC_M_GPP_A13;

      PlatformNvs->M2Ssd3PowerEnableGpio = 0;
      PlatformNvs->M2Ssd3PowerEnableGpioPolarity = 0;
      PlatformNvs->M2Ssd3RstGpio = 0;
    break;

    default:
      PlatformNvs->PcieSlot1WakeGpio = GPIOV2_MTL_SOC_M_GPP_F19;
      PlatformNvs->PcieSlot1RpNumber = 9;
      PlatformNvs->PcieSlot1PowerEnableGpio = GPIOV2_MTL_SOC_M_GPP_D3;
      PlatformNvs->PcieSlot1RstGpio = GPIOV2_MTL_SOC_M_GPP_F19;

      PlatformNvs->PcieSlot2RpNumber = 11;

      PlatformNvs->PcieSlot3WakeGpio = 0;
      PlatformNvs->PcieSlot3RpNumber = 0;
      PlatformNvs->PcieSlot3PowerEnableGpio = 0;
      PlatformNvs->PcieSlot3PowerEnableGpioPolarity = 0;
      PlatformNvs->PcieSlot3RstGpio = 0;

      PlatformNvs->PcieSlot4WakeGpio = 0;
      PlatformNvs->PcieSlot4RpNumber = 0;
      PlatformNvs->PcieSlot4PowerEnableGpio = 0;
      PlatformNvs->PcieSlot4RstGpio = 0;

      PlatformNvs->PcieSlot5WakeGpio = 0;
      PlatformNvs->PcieSlot5PowerEnableGpio = 0;
      PlatformNvs->PcieSlot5PowerEnableGpioPolarity = 0;
      PlatformNvs->PcieSlot5RstGpio = 0;

      PlatformNvs->M2Ssd1PowerEnableGpio = GPIOV2_MTL_SOC_M_GPP_H11;
      PlatformNvs->M2Ssd1RstGpio = GPIOV2_MTL_SOC_M_GPP_A11;

      PlatformNvs->M2Ssd2PowerEnableGpio = GPIOV2_MTL_SOC_M_GPP_F5;
      PlatformNvs->M2Ssd2RstGpio = GPIOV2_MTL_SOC_M_GPP_F20;

      PlatformNvs->M2Ssd3PowerEnableGpio = 0;
      PlatformNvs->M2Ssd3PowerEnableGpioPolarity = 1;
      PlatformNvs->M2Ssd3RstGpio = 0;
    break;
  }

  PlatformNvs->ConfigTdpBios = 0;
  PlatformNvs->PL1LimitCS = 0;
  PlatformNvs->PL1LimitCSValue = 4500;

  PlatformNvs->PcieSlot1PowerEnableGpioPolarity = 0;
  PlatformNvs->PcieSlot1RstGpioPolarity = 0;

  PlatformNvs->PcieSlot2WakeGpio = 0;
  PlatformNvs->PcieSlot2PowerEnableGpio = 0;
  PlatformNvs->PcieSlot2PowerEnableGpioPolarity = 0;
  PlatformNvs->PcieSlot2RstGpio = 0;
  PlatformNvs->PcieSlot2RstGpioPolarity = 0;

  PlatformNvs->PcieSlot3RstGpioPolarity = 0;

  PlatformNvs->PcieSlot4PowerEnableGpioPolarity = 0;
  PlatformNvs->PcieSlot4RstGpioPolarity = 0;

  PlatformNvs->PcieSlot5RpNumber = 0;
  PlatformNvs->PcieSlot5RstGpioPolarity = 0;

  PlatformNvs->PcieSlot6WakeGpio = 0;
  PlatformNvs->PcieSlot6RpNumber = 0;
  PlatformNvs->PcieSlot6PowerEnableGpio = 0;
  PlatformNvs->PcieSlot6PowerEnableGpioPolarity = 0;
  PlatformNvs->PcieSlot6RstGpio = 0;
  PlatformNvs->PcieSlot6RstGpioPolarity = 0;

  PlatformNvs->PcieSlot7WakeGpio = 0;
  PlatformNvs->PcieSlot7RpNumber = 0;
  PlatformNvs->PcieSlot7PowerEnableGpio = 0;
  PlatformNvs->PcieSlot7PowerEnableGpioPolarity = 0;
  PlatformNvs->PcieSlot7RstGpio = 0;
  PlatformNvs->PcieSlot7RstGpioPolarity = 0;

  PlatformNvs->WlanWakeGpio = GPIOV2_MTL_SOC_M_GPP_A12;
  PlatformNvs->WlanRstGpio = GPIOV2_MTL_SOC_M_GPP_H2;
  PlatformNvs->WlanRootPortNumber = 8;

  PlatformNvs->M2Ssd1PowerEnableGpioPolarity = 1;
  PlatformNvs->M2Ssd1RstGpioPolarity = 0;

  PlatformNvs->M2Ssd2PowerEnableGpioPolarity = 1;
  PlatformNvs->M2Ssd2RstGpioPolarity = 0;

  PlatformNvs->M2Ssd3RstGpioPolarity = 0;

  PlatformNvs->M2Ssd4PowerEnableGpio = GPIOV2_MTL_SOC_M_GPP_D6;
  PlatformNvs->M2Ssd4PowerEnableGpioPolarity = 1;
  PlatformNvs->M2Ssd4RstGpio = GPIOV2_MTL_SOC_M_GPP_A14;
  PlatformNvs->M2Ssd4RstGpioPolarity = 0;

  PlatformNvs->SataPortPowerEnableGpio = 0;
  PlatformNvs->SataPortPowerEnableGpioPolarity = 0;

  PlatformNvs->WwanWakeGpio = GPIOV2_MTL_SOC_M_GPP_F21;
  PlatformNvs->WwanRootPortNumber = 0x7;
  PlatformNvs->WwanFwFlashDevice = 0;
  PlatformNvs->XdciFnEnable = 1;

  //EcNvs->LidSwitchWakeGpio = GPIOV2_MTL_SOC_M_GPP_V2;
  PlatformNvs->HidEventFilterEnable         = 0x01;
  PlatformNvs->Rtd3Support = 0x1;
  PlatformNvs->Rtd3P0dl = 0x64;
  PlatformNvs->Rtd3AudioDelay = 0xC8;
  PlatformNvs->Rtd3SensorHub = 0x44;
  PlatformNvs->Rtd3TouchPanelDelay = 0x44;
  PlatformNvs->Rtd3TouchPadDelay = 0x44;
  PlatformNvs->VRRampUpDelay = 0x10;
  PlatformNvs->PstateCapping = 0x0;
  PlatformNvs->UsbPowerResourceTest = 0x0;
  PlatformNvs->SensorStandby = 0x0;
  PlatformNvs->Rtd3Config0 = 0x0;
  PlatformNvs->Rtd3Config1 = 0x0;
  PlatformNvs->StorageRtd3Support = 0x1;
  PlatformNvs->DeepestUSBSleepWakeCapability = 0x4;
  //CPU NVS Init
  UpdateCpuNvs (CpuNvs);

  //System Agent NVS Init
  SaNvs->XPcieCfgBaseAddress      = (UINT32)(PcdGet64(PcdPciExpressBaseAddress));
  SaNvs->Mmio64Base               = 0;
  SaNvs->Mmio64Length             = 0;
  SaNvs->Mmio32Base               = PcdGet32(PcdPciResourceMem32Base);
  if (SaNvs->Mmio32Base < SaNvs->XPcieCfgBaseAddress) {
    SaNvs->Mmio32Length = SaNvs->XPcieCfgBaseAddress - SaNvs->Mmio32Base;
  } else if (SaNvs->Mmio32Base < PCH_PCR_BASE_ADDRESS) {
    SaNvs->Mmio32Length = PCH_PCR_BASE_ADDRESS - SaNvs->Mmio32Base;
  } else {
    DEBUG((DEBUG_INFO, "acpi: Unable to configure M32L with M32B=0x%08X\n", SaNvs->Mmio32Base));
  }

  SaNvs->SimicsEnvironment = 0;
  SaNvs->AlsEnable = 0;
  SaNvs->IgdState = 1;
  SaNvs->BrightnessPercentage = 100;
  SaNvs->EdpValid = 0;
  SaNvs->LtrEnable[0] = 1;
  SaNvs->LtrEnable[1] = 1;
  SaNvs->LtrEnable[2] = 1;
  SaNvs->LtrEnable[3] = 1;

  SaNvs->PcieLtrMaxSnoopLatency[0] = 0x100F;
  SaNvs->PcieLtrMaxSnoopLatency[1] = 0x100F;
  SaNvs->PcieLtrMaxSnoopLatency[2] = 0x100F;
  SaNvs->PcieLtrMaxSnoopLatency[3] = 0x100F;
  SaNvs->PcieLtrMaxNoSnoopLatency[0] = 0x100F;
  SaNvs->PcieLtrMaxNoSnoopLatency[1] = 0x100F;
  SaNvs->PcieLtrMaxNoSnoopLatency[2] = 0x100F;
  SaNvs->PcieLtrMaxNoSnoopLatency[3] = 0x100F;

  PlatformNvs->PpmFlags           = CpuNvs->PpmFlags;
  SocUpdateAcpiGnvs ((VOID *)GnvsIn);
}
