/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2BoardInitLib.h"
#include <Library/MediaAccessLib.h>

extern EFI_ACPI_DMAR_HEADER mAcpiDmarTableTemplate;
extern EFI_ACPI_6_4_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER mAcpiMadtTableTemplate;
extern EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER mAcpiMcfgTableTemplate;

STATIC
CONST EFI_ACPI_COMMON_HEADER *mPlatformAcpiTables[] = {
  (EFI_ACPI_COMMON_HEADER *)&mAcpiDmarTableTemplate,
  (EFI_ACPI_COMMON_HEADER *)&mAcpiMadtTableTemplate,
  (EFI_ACPI_COMMON_HEADER *)&mAcpiMcfgTableTemplate,
  NULL
};

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
  UINT16                Pm1Sts;

  SmiEn = IoRead32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
  if (((SmiEn & B_ACPI_IO_SMI_EN_GBL_SMI) !=0) && ((SmiEn & B_ACPI_IO_SMI_EN_EOS) !=0)) {
    return;
  }

  //
  // Clear the status before setting smi enable
  //
  SmiSts = IoRead32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN + 4));
  Pm1Sts = IoRead16 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS));

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
}

/**
  Set SPI flash EISS and LE
**/
VOID
ProgramSecuritySetting (
  VOID
  )
{
  UINTN               SpiBaseAddress;

  SpiBaseAddress = GetDeviceAddr (OsBootDeviceSpi, 0);
  SpiBaseAddress = TO_MM_PCI_ADDRESS (SpiBaseAddress);

  // Set the BIOS Lock Enable and EISS bits
  MmioOr8 (SpiBaseAddress + R_SPI_BCR, (UINT8) (B_SPI_BCR_BLE | B_SPI_BCR_EISS));
}

/**
  Enables flash protection
**/
VOID
EnableFlashProtection (
  VOID
  )
{
  FLASH_PROTECTION_CONFIG FlashProtectionConfig;
  UINT32                  DlockValue;
  UINT32                  Index;
  UINT32                  Data32;
  UINT32                  Timer;
  UINT16                  Data16And;
  UINT16                  Data16Or;
  UINT32                  UEFIVarAddress;
  UINT32                  UEFIVarSize;
  EFI_STATUS              Status;
  UINT32                  BaseAddress;
  UINT32                  RegionSize;

  ZeroMem (&FlashProtectionConfig, sizeof (FlashProtectionConfig));

  //Check if Firmware update is triggered, if yes then skipping Flash protection
  if (GetBootMode() == BOOT_ON_FLASH_UPDATE) {
    DEBUG((DEBUG_INFO, "Skipping Flash Proctection..!!! Since Firware Update is triggered\n"));
    return;
  } else {
    //
    // Initialize flash protection
    //
    Status = SpiGetRegionAddress (FlashRegionBios, &BaseAddress, &RegionSize);
    DEBUG ((DEBUG_INFO, "SpiGetRegionAddress Status: %r, BaseAddress: %08x, RegionSize: %08x\n", Status, BaseAddress, RegionSize));

    if (!EFI_ERROR (Status)) {
      FlashProtectionConfig.ProtectRange[0].WriteProtectionEnable = TRUE;
      FlashProtectionConfig.ProtectRange[0].ReadProtectionEnable  = FALSE;
      FlashProtectionConfig.ProtectRange[2].WriteProtectionEnable = FALSE;
      FlashProtectionConfig.ProtectRange[2].ReadProtectionEnable  = FALSE;
      FlashProtectionConfig.ProtectRange[3].WriteProtectionEnable = FALSE;
      FlashProtectionConfig.ProtectRange[3].ReadProtectionEnable  = FALSE;
      FlashProtectionConfig.ProtectRange[4].WriteProtectionEnable = FALSE;
      FlashProtectionConfig.ProtectRange[4].ReadProtectionEnable  = FALSE;
      if (GetPayloadId() != 0) {
        FlashProtectionConfig.ProtectRange[1].WriteProtectionEnable = TRUE;
        FlashProtectionConfig.ProtectRange[1].ReadProtectionEnable  = FALSE;
        //If payloadID is UEFI, then making UEFI variable region as Un protected
        Status = GetComponentInfo (FLASH_MAP_SIG_UEFIVARIABLE, &UEFIVarAddress, &UEFIVarSize);
        DEBUG ((DEBUG_INFO, "GetComponentInfo Status: %r, UEFIVarAddress: %08x, UEFIVarSize: %08x\n", Status, UEFIVarAddress, UEFIVarSize));
        if (!EFI_ERROR (Status)) {
          UEFIVarAddress -= ((UINT32)(~RegionSize) + 1);
          FlashProtectionConfig.ProtectRange[0].ProtectedRangeBase  = (UINT16)(BaseAddress >> 12);
          FlashProtectionConfig.ProtectRange[0].ProtectedRangeLimit = (UINT16)((BaseAddress + UEFIVarAddress - 1) >> 12);
          FlashProtectionConfig.ProtectRange[1].ProtectedRangeBase  = (UINT16)((BaseAddress + UEFIVarAddress + UEFIVarSize) >> 12);
          FlashProtectionConfig.ProtectRange[1].ProtectedRangeLimit = (UINT16)(((BaseAddress + RegionSize) - 1) >> 12);
        }
      } else {
        FlashProtectionConfig.ProtectRange[1].WriteProtectionEnable = FALSE;
        FlashProtectionConfig.ProtectRange[1].ReadProtectionEnable  = FALSE;
        //making whole BIOS region as protected
        FlashProtectionConfig.ProtectRange[0].ProtectedRangeBase  = (UINT16)(BaseAddress >> 12);
        FlashProtectionConfig.ProtectRange[0].ProtectedRangeLimit = (UINT16)(((BaseAddress + RegionSize) - 1) >> 12);
      }

      DlockValue = MmioRead32 (PCH_SPI_BASE_ADDRESS + R_SPI_MEM_BIOS_DLOCK);
      for (Index = 0; Index < FLASH_PROTECTED_RANGES; ++Index) {
        if ((FlashProtectionConfig.ProtectRange[Index].WriteProtectionEnable ||
            FlashProtectionConfig.ProtectRange[Index].ReadProtectionEnable) != TRUE) {
          continue;
        }

        //
        // Proceed to program the register after ensure it is enabled
        //
        Data32 = 0;
        Data32 |= (FlashProtectionConfig.ProtectRange[Index].WriteProtectionEnable == TRUE) ? B_SPI_MEM_BIOS_FPR0_WPE : 0;
        Data32 |= (FlashProtectionConfig.ProtectRange[Index].ReadProtectionEnable == TRUE) ? B_SPI_MEM_BIOS_FPR0_RPE : 0;
        Data32 |= ((UINT32) FlashProtectionConfig.ProtectRange[Index].ProtectedRangeLimit << N_SPI_MEM_BIOS_FPR0_PRL) & B_SPI_MEM_BIOS_FPR0_PRL;
        Data32 |= FlashProtectionConfig.ProtectRange[Index].ProtectedRangeBase & B_SPI_MEM_BIOS_FPR0_PRB;


        DlockValue |= (UINT32) (B_SPI_MEM_BIOS_DLOCK_PR0LOCKDN << Index);
        MmioWrite32 ((UINTN) (PCH_SPI_BASE_ADDRESS + (R_SPI_MEM_BIOS_FPR0 + (Index * S_SPI_MEM_BIOS_FPRX))), Data32);

      }

      //
      // Program DLOCK register
      //
      MmioWrite32 ((UINTN) PCH_SPI_BASE_ADDRESS + R_SPI_MEM_BIOS_DLOCK, DlockValue);

      ///
      /// (SPI_BAR0 + 04[15]) at end of post.  When set to 1, those Flash Program Registers
      /// that are locked down by this FLOCKDN bit cannot be written.
      /// Please refer to the EDS for which program registers are impacted.
      /// Additionally SPI_BAR0 + 0x04 BIT11 (WRSDIS) must be programed to disable Write Status in HW sequencing
      ///

      //
      // Ensure there is no pending SPI trasaction before setting lock bits
      //
      Timer = 0;
      while (MmioRead16 (PCH_SPI_BASE_ADDRESS + R_SPI_MEM_BIOS_HSFSTS_CTL) & B_SPI_MEM_BIOS_HSFSTS_CTL_H_SCIP) {
        if (Timer > SPI_WAIT_TIME) {
          //
          // SPI transaction is pending too long at this point, return.
          //
          DEBUG ((DEBUG_ERROR, "SPI Cycle timeout\n"));
          return;
        }
        MicroSecondDelay (SPI_WAIT_PERIOD);
        Timer += SPI_WAIT_PERIOD;
      }

      //
      // Clear any outstanding status
      //
      Data16Or  = B_SPI_MEM_BIOS_HSFSTS_CTL_H_SAF_DLE
                | B_SPI_MEM_BIOS_HSFSTS_CTL_H_SAF_ERROR
                | B_SPI_MEM_BIOS_HSFSTS_CTL_H_AEL
                | B_SPI_MEM_BIOS_HSFSTS_CTL_FCERR
                | B_SPI_MEM_BIOS_HSFSTS_CTL_FDONE;
      Data16And = 0xFFFF;
      MmioAndThenOr16 (PCH_SPI_BASE_ADDRESS + R_SPI_MEM_BIOS_HSFSTS_CTL, Data16And, Data16Or);

      //
      // Set WRSDIS
      //
      Data16Or  = B_SPI_MEM_BIOS_HSFSTS_CTL_WRSDIS;
      Data16And = 0xFFFF;
      MmioAndThenOr16 (PCH_SPI_BASE_ADDRESS + R_SPI_MEM_BIOS_HSFSTS_CTL, Data16And, Data16Or);

      //
      // Set FLOCKDN
      //
      Data16Or  = B_SPI_MEM_BIOS_HSFSTS_CTL_FLOCKDN;
      Data16And = 0xFFFF;
      MmioAndThenOr16 (PCH_SPI_BASE_ADDRESS + R_SPI_MEM_BIOS_HSFSTS_CTL, Data16And, Data16Or);

      ///
      /// It is strongly recommended that to sets the Vendor Component Lock (VCL) bits. VCL applies
      /// the lock to both VSCC0 and VSCC1 even if VSCC0 is not used. Without the VCL bits set, it is
      /// possible to make Host/GbE VSCC register(s) changes in that can cause undesired host and
      /// integrated GbE Serial Flash functionality.
      ///
      MmioOr32 ((UINTN) PCH_SPI_BASE_ADDRESS + R_SPI_MEM_BIOS_SFDP0_VSCC0, B_SPI_MEM_BIOS_SFDP0_VSCC0_VCL);

      //
      // End of Flash Protection
      //
    }
  }
}

/**
 * Initialize and de-initialize USB device instance.
 */

VOID
UsbDevInstanceInitDeinit (VOID)
{
  EFI_STATUS  Status;
  UINT8       DevInstance;
  UINTN       UsbPciBase;

  DevInstance = 0;

  UsbPciBase = GetDeviceAddr (OsBootDeviceUsb, DevInstance);
  UsbPciBase = TO_MM_PCI_ADDRESS (UsbPciBase);

  // Init Boot device functions
  Status = MediaSetInterfaceType (OsBootDeviceUsb);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MediaSetInterfaceType (USB) failed: %r\n", Status));
    return;
  }
  Status = MediaInitialize (UsbPciBase, DevInitAll);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MediaInitialize (USB init) failed: %r\n", Status));
    return;
  }
  Status = MediaInitialize (0, DevDeinit);
   if (EFI_ERROR (Status)) {
     DEBUG ((DEBUG_WARN, "MediaInitialize (USB deinit) failed: %r\n", Status));
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
  VOID                           *NvsData;
  UINT32                          MrcDataLen;
  VOID                           *FspHobList;
  EFI_STATUS                      SubStatus;

  switch (InitPhase) {
  case PreSiliconInit:
    ConfigureGpioV2 (CDATA_GPIO_TAG, NULL, 0);
    UpdatePayloadId ();
    SpiConstructor ();
    break;
  case PostSiliconInit:
    PlatformPostSiliconInit ();
    // Override the Smbios default Info using SMBIOS binary blob
    if (FeaturePcdGet (PcdSmbiosEnabled)) {
      LoadSmbiosStringsFromComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('S', 'M', 'B', 'S'));
    }

    // Saving of NVS data is part of common SBL code which attempts to save
    // this data on SPI flash (after retrieving FSP NVS data HOB) right after
    // Temp ram tear down for all other platforms. But, for GNRD, as NVS data
    // HOB is published only after FSP silicon initialization, this has to be
    // done in PostSiliconInit stage.
    FspHobList = GetFspHobListPtr ();
    NvsData = GetFspNvsDataBuffer (FspHobList, &MrcDataLen);
    if ((NvsData != NULL) && (MrcDataLen > 0)) {
      SubStatus = SaveNvsData (NvsData, MrcDataLen);
      DEBUG ((DEBUG_INFO, "Save MRC Training Data (0x%p 0x%06X) ... %r\n", NvsData, MrcDataLen, SubStatus));
    }

    // Enable flash protection
    EnableFlashProtection();
    break;
  case PrePciEnumeration:
    PlatformPrePciEnumeration ();
    (VOID) PcdSet32S (PcdAcpiTableTemplatePtr, (UINT32)(UINTN)mPlatformAcpiTables);
    break;
  case PostPciEnumeration:
    PlatformPostPciEnumeration ();
    InitializeBmcVideo ();
    break;
  case PrePayloadLoading:
    DumpDebugInfo ();
    UsbDevInstanceInitDeinit ();
    break;
  case EndOfStages:
    PlatformServiceInit ();
    ClearSmi ();
    if (GetPayloadId() != 0) {
      ProgramSecuritySetting ();
    }
    BuildSgxSblDataInfoHob ();
    break;
  case ReadyToBoot:
    if (GetPayloadId() == 0) {
      ProgramSecuritySetting ();
      //
      // set SMI LOCK (SMI_LOCK)
      // PWRM Offset 1024h Bit 4
      //
      DEBUG ((DEBUG_INFO, "Set SMI Lock\n"));
      MmioOr8 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_B, (UINT8)B_PMC_PWRM_GEN_PMCON_B_SMI_LOCK);
    }
    break;
  case EndOfFirmware:
    ClearFspHob ();
    FreeSgxMem ();
    break;
  default:
    break;
  }
}

/**
  Update FSP-S UPD config data.

  @param  FspsUpdPtr    The pointer to the FSP-S UPD to be updated.

**/
VOID
EFIAPI
UpdateFspConfig (
  VOID     *FspsUpdPtr
  )
{

  FSPS_UPD                *FspsUpd;
  FSPS_CONFIG             *FspsConfig;
  FEATURES_CFG_DATA       *FeaturesCfgData;
  PCIE_CFG_DATA           *PcieCfgData;
  PWR_CFG_DATA            *PwrMgmtCfgData;
  EFI_STATUS              Status;

  FspsUpd              = (FSPS_UPD *) FspsUpdPtr;
  FspsConfig           = &FspsUpd->FspsConfig;

  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
  if (FeaturesCfgData == NULL) {
    DEBUG ((DEBUG_WARN, "No FeaturesCfgData CFGDATA. Continue to boot with default values!\n"));
  } else {
      FspsConfig->BiosInterface = FeaturesCfgData->Features.BiosInterface;
  }

  // Pcie Settings
  PcieCfgData = (PCIE_CFG_DATA *) FindConfigDataByTag (CDATA_PCIE_TAG);
  if (PcieCfgData == NULL) {
    DEBUG ((DEBUG_WARN, "No PcieCfgData CFGDATA. Continue to boot with default values!\n"));
  } else {
    FspsConfig->PcieRootPort0DeEmphasis = (UINT8)PcieCfgData->PchRpConfig.PcieRootPortDeEmphasis[0];
    FspsConfig->PcieRootPort1DeEmphasis = (UINT8)PcieCfgData->PchRpConfig.PcieRootPortDeEmphasis[1];
    FspsConfig->PcieRootPort2DeEmphasis = (UINT8)PcieCfgData->PchRpConfig.PcieRootPortDeEmphasis[2];
    FspsConfig->PcieRootPort3DeEmphasis = (UINT8)PcieCfgData->PchRpConfig.PcieRootPortDeEmphasis[3];
    FspsConfig->PcieRootPort4DeEmphasis = (UINT8)PcieCfgData->PchRpConfig.PcieRootPortDeEmphasis[4];
    FspsConfig->PcieRootPort5DeEmphasis = (UINT8)PcieCfgData->PchRpConfig.PcieRootPortDeEmphasis[5];
    FspsConfig->PcieRootPort6DeEmphasis = (UINT8)PcieCfgData->PchRpConfig.PcieRootPortDeEmphasis[6];
    FspsConfig->PcieRootPort7DeEmphasis = (UINT8)PcieCfgData->PchRpConfig.PcieRootPortDeEmphasis[7];
  }

  // PCH ADR Settings
  FspsConfig->PchAdrEn = 2;

  // SMM Settings
  FspsConfig->FspSmmInitEn = 0;

  // Pwr mgmt Settings
  PwrMgmtCfgData = (PWR_CFG_DATA *) FindConfigDataByTag (CDATA_PWR_TAG);
  if (PwrMgmtCfgData == NULL) {
    DEBUG ((DEBUG_WARN, "No PwrMgmtCfgData CFGDATA. Continue to boot with default values!\n"));
  } else {
    FspsConfig->CpuPmC2c3tt                 = (UINT8)PwrMgmtCfgData->CState.C2C3TT;;
    FspsConfig->CpuPmMonitorMWait           = (UINT8)PwrMgmtCfgData->CState.MonitorMWait;;
    FspsConfig->CpuPmTurboMode              = (UINT8)PwrMgmtCfgData->PState.TurboMode;
    FspsConfig->CpuPmPwrPerfTuning          = (UINT8)PwrMgmtCfgData->PState.PwrPerfTuning;
    FspsConfig->CpuPmProcessorHWPMEnable    = (UINT8)PwrMgmtCfgData->PState.HWPMEnable;
    FspsConfig->CpuPmProcessorHWPMInterrupt = (UINT8)PwrMgmtCfgData->PState.HWPMInterrupt;
    FspsConfig->CpuPmProcessorEPPProfile    = (UINT8)PwrMgmtCfgData->PState.EppProfile;
    FspsConfig->CpuPmBootPstate             = (UINT8)PwrMgmtCfgData->PState.BootPState;
  }

  // Sgx Settings
  Status = UpdateFspsSgxConfig(FspsUpd);
  DEBUG((DEBUG_INFO, "UpdateFspsSgxConfig return status = 0x%x\n", Status));

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
RETURN_STATUS
EFIAPI
SaveNvsData (
  IN  VOID    *Buffer,
  IN  UINT32   Length
  )
{
  RETURN_STATUS   Status;
  UINT32          Address;
  UINT32          BaseAddress;
  UINT32          RegionSize;
  UINT32          MrcDataRegSize;

  Address        = 0;
  MrcDataRegSize = 0;
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
  }

  DEBUG ((DEBUG_INFO, "SaveNvsData Done - %r\n", Status));
  return Status;
}

/**
 Update the OS boot option

 @param[out] OsBootOptionList   pointer to boot option list.
 **/
VOID
EFIAPI
UpdateOsBootMediumInfo (
  OUT  OS_BOOT_OPTION_LIST  *OsBootOptionList
  )
{
  UINT16                                  Bus;
  volatile PCI_DEVICE_INDEPENDENT_REGION *PciDev;
  UINT8                                   Nvme_Instance;
  UINT8                                   Usb_Instance;

  FillBootOptionListFromCfgData (OsBootOptionList);

  //
  // Depends on the PCI root bridge, pluged PCI devices, the bus number for NVMe device
  // might be different. so update the NVMe bus number in the device table.
  //
  Nvme_Instance = 0;
  Usb_Instance = 0;

  for (Bus = 1; Bus <= PCI_MAX_BUS; Bus++) {
    PciDev = (volatile PCI_DEVICE_INDEPENDENT_REGION *) MM_PCI_ADDRESS (Bus, 0, 0, 0);
    if (PciDev->DeviceId != 0xFFFF) {
      if ((PciDev->ClassCode[0] == PCI_IF_MASS_STORAGE_SOLID_STATE_ENTERPRISE_NVMHCI) &&
          (PciDev->ClassCode[1] == PCI_CLASS_MASS_STORAGE_SOLID_STATE) &&
          (PciDev->ClassCode[2] == PCI_CLASS_MASS_STORAGE)) {
        SetDeviceAddr (OsBootDeviceNvme, Nvme_Instance, (UINT32)(Bus << 16));
        Nvme_Instance += 1;
      }
      if ((PciDev->ClassCode[0] == PCI_IF_XHCI) &&
          (PciDev->ClassCode[1] == PCI_CLASS_SERIAL_USB) &&
          (PciDev->ClassCode[2] == PCI_CLASS_SERIAL)) {
        SetDeviceAddr (OsBootDeviceUsb, Usb_Instance, (UINT32)(Bus << 16));
        Usb_Instance += 1;
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
  if (GfxInfo != NULL) {
    GfxInfo->GraphicsMode.HorizontalResolution  = 800;
    GfxInfo->GraphicsMode.VerticalResolution    = 600;
  }
}

/**
 Checks if Aspeed display controller is present.

 @param[out] VendorId.
 @param[out] DeviceId.

 @retval TRUE   Aspeed display controller is present
 @retval FALSE  Aspeed display controller is not present
 **/
BOOLEAN
IsAspeedDispCardPresent (
  OUT  UINT16   *VendorId,
  OUT  UINT16   *DeviceId
)
{
  BOOLEAN           IsAspdDispPresent;
  UINT16            Bus;
  UINTN             PciBase;

  IsAspdDispPresent = FALSE;
  for (Bus = 1; Bus <= PCI_MAX_BUS; Bus++) {
    PciBase = MM_PCI_ADDRESS (Bus, 0, 0, 0);
    if (MmioRead32 (PciBase) == ((ASPEED2600_DEVICE_ID << 16) | ASPEED2600_VENDOR_ID)) {
      DEBUG ((DEBUG_INFO, "Using ASPEED VGA\n"));
      *VendorId = ASPEED2600_VENDOR_ID;
      *DeviceId = ASPEED2600_DEVICE_ID;
      IsAspdDispPresent = TRUE;
      break;
    }
  }
  return IsAspdDispPresent;
}

/**
 Checks if External graphics card is present.

 @param[out] VendorId.
 @param[out] DeviceId.

 @retval TRUE   External graphics card is present
 @retval FALSE  External graphics card is not present
 **/
BOOLEAN
IsExtDispCardPresent (
  OUT  UINT16   *VendorId,
  OUT  UINT16   *DeviceId
)
{
  BOOLEAN           IsExtDispPresent;
  UINT16            Bus;
  UINT8             Dev;
  volatile          PCI_DEVICE_INDEPENDENT_REGION *PciDev;

  IsExtDispPresent = FALSE;
  for (Bus = 1; Bus <= PCI_MAX_BUS; Bus++) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; Dev++) {
      PciDev = (volatile PCI_DEVICE_INDEPENDENT_REGION *) MM_PCI_ADDRESS (Bus, Dev, 0, 0);
      if (PciDev->DeviceId != 0xFFFF) {
        // Check if device class is of type display and skip if ast2600's display is found
        if ((PciDev->ClassCode[2] == PCI_CLASS_DISPLAY) && (PciDev->VendorId != ASPEED2600_VENDOR_ID) && (PciDev->DeviceId != ASPEED2600_DEVICE_ID)) {
          DEBUG ((DEBUG_INFO, "Using External Graphics card : DeviceId:0x%x VendorId:0x%x\n", PciDev->DeviceId, PciDev->VendorId));
          IsExtDispPresent = TRUE;
          *VendorId = PciDev->VendorId;
          *DeviceId = PciDev->DeviceId;
          break;
        }
      }
    }
    if (IsExtDispPresent == TRUE) {
      break;
    }
  }

  return IsExtDispPresent;
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
#if !FixedPcdGetBool(PcdSimicsEnv)
  PLATFORM_MISC     *PlatformMisc;
  PLATFORM_DATA_BL  *PlatformData;
#endif
  UINT16            VendorId;
  UINT16            DeviceId;
  BOOLEAN           IsBMCPresent;

  // Initialize to default values
  VendorId        = 0;
  DeviceId        = 0;
  IsBMCPresent    = FALSE;


  // For Simics environment, select pre-silicon graphics
#if FixedPcdGetBool(PcdSimicsEnv)
    DEBUG ((DEBUG_INFO, "Using Pre-Si VGA\n"));
    VendorId = PRE_SI_GNX_CTRLR_VENDOR_ID;
    DeviceId = PRE_SI_GNX_CTRLR_DEVICE_ID;
#else
  PlatformData = (PLATFORM_DATA_BL *) GetPlatformDataPtr ();
  if ((PlatformData == NULL)) {
    DEBUG ((DEBUG_ERROR, "Platform data is not found\n"));
    return;
  } else {
    IsBMCPresent = (BOOLEAN) PlatformData->PlatformFeatures.BMCPresent;
  }
  // If BMC is not present, external graphics card is expected
  // to be connected for display
  if (IsBMCPresent == FALSE) {
    if (IsExtDispCardPresent (&VendorId, &DeviceId) == FALSE) {
      DEBUG ((DEBUG_ERROR, "BMC-less Board : External graphics card is not detected\n"));
    }
  } else {
    // BMC is present. Check the configuration for choice of Display controller
    PlatformMisc = (PLATFORM_MISC *) FindConfigDataByTag (CDATA_PLTMISC_TAG);
    if ((PlatformMisc == NULL)) {
      DEBUG ((DEBUG_ERROR, "Platform Misc data is not found\n"));
      return;
    }
    if (PlatformMisc->ActiveVideo == 0) {
      // Check if display controller device of ast2600 is present
      if (IsAspeedDispCardPresent (&VendorId, &DeviceId) == FALSE) {
        DEBUG ((DEBUG_ERROR, "Aspeed display card is not detected\n"));
      }
    } else {
      // Check if external graphics card is present
      if (IsExtDispCardPresent (&VendorId, &DeviceId) == FALSE) {
        DEBUG ((DEBUG_ERROR, "External graphics card is not detected. Checking if Aspeed display card is present.\n"));
        // As external graphics card wasn't found, trigger fall back to check presence of ast2600
        if (IsAspeedDispCardPresent (&VendorId, &DeviceId) == FALSE) {
          DEBUG ((DEBUG_ERROR, "Failed to detect both External and aspeed display card\n"));
        }
      }
    }
  }
#endif
  GfxDeviceInfo->VendorId = VendorId;
  GfxDeviceInfo->DeviceId = DeviceId;
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
  if (LdrSmmInfo != NULL) {
    LdrSmmInfo->SmmBase = PcdGet32 (PcdSmramTsegBase);
    LdrSmmInfo->SmmSize = PcdGet32 (PcdSmramTsegSize);
    LdrSmmInfo->Flags   = SMM_FLAGS_4KB_COMMUNICATION;

    DEBUG ((DEBUG_VERBOSE, "SmmBase 0x%08X, SmmSize 0x%08X\n",
      LdrSmmInfo->SmmBase, LdrSmmInfo->SmmSize));

    //
    // Update the HOB with smi ctrl register data
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
    LdrSmmInfo->SmiStsReg.SmiApmPos  = (UINT8)HighBitSet32 (B_ACPI_IO_SMI_STS_APM);
    LdrSmmInfo->SmiStsReg.Address    = (UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_STS);

    //
    // Update smi lock register data
    //
    LdrSmmInfo->SmiLockReg.RegType    = (UINT8)REG_TYPE_MMIO;
    LdrSmmInfo->SmiLockReg.RegWidth   = (UINT8)WIDE32;
    LdrSmmInfo->SmiLockReg.SmiLockPos = (UINT8)HighBitSet32 (B_PMC_PWRM_GEN_PMCON_B_SMI_LOCK);
    LdrSmmInfo->SmiLockReg.Address    = (UINT32)(PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_B);
  }
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
  PLATFORM_DATA_BL             *PlatformData;
  UINT64                       PlatformInfoMsr;
  UINT16                       PlatformPreProd;

  PlatformData = (PLATFORM_DATA_BL *)GetPlatformDataPtr ();
  if(PlatformData != NULL){

    //
    // Get for Sample part
    //
    PlatformInfoMsr = AsmReadMsr64 (MSR_PLATFORM_INFO);
    // Bit27 represents Sample part detail
    PlatformPreProd = (PlatformInfoMsr & BIT27) ? 1 : 0;

    LoaderPlatformInfo->HwState = (PlatformData->BtGuardInfo.VerifiedBoot << 0)
                                     | (PlatformData->BtGuardInfo.MeasuredBoot << 1)
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
  EFI_ACPI_DESCRIPTION_HEADER           *Table;
  EFI_STATUS                            Status;

  Status = EFI_SUCCESS;

  Table = (EFI_ACPI_DESCRIPTION_HEADER *) Current;
  switch (Table->Signature) {
  case EFI_ACPI_5_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    Status = PatchDsdtTable (Table);
    break;
  case EFI_ACPI_VTD_DMAR_TABLE_SIGNATURE:
    Status = PatchDmarTable (Table);
    break;
  case EFI_ACPI_5_0_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE:
    Status = PatchMadtTable (Table);
    break;
  case EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE:
    Status = PatchTpm2AcpiTable (Table);
    break;
  case EFI_ACPI_5_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    if (Table->OemTableId == SIGNATURE_64 ('S', 'S', 'D', 'T', ' ', ' ', 'P', 'M')) {
      Status = PatchCpuPmSsdtTable (Table);
    }
    break;
  case OEM1_SSDT_TABLE_SIGNATURE:
    if (Table->OemTableId == SIGNATURE_64 ('C', 'P', 'U', ' ', 'E', 'I', 'S', 'T')) {
      Status = PatchOem1SsdtTable (Table);
    }
    break;
  case EFI_BDAT_TABLE_SIGNATURE:
    Status = PatchBdatTable (Table);
    break;
  case EFI_ACPI_6_2_DEBUG_PORT_2_TABLE_SIGNATURE:
      Status = PatchDbg2Table (Table);
      break;
  case EFI_ACPI_6_4_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE:
      Status = PatchMcfgTable (Table);
      break;
  }

  return Status;
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
  EFI_CPUID_REGISTER        CpuidRegisters = {0};
  UINT32                    NumOfBitShift;
  GLOBAL_NVS_AREA           *Gnvs;
  PCH_NVS_AREA              *PchNvs;
  BIOS_ACPI_PARAM           *AcpiParameter;
  FSPS_UPD                  *FspsUpd;
  FSPS_CONFIG               *FspsConfig;
  EFI_HOB_GUID_TYPE         *GuidHob;
  IIO_UDS                   *IioUdsHobData;
  VOID                      *FspHobList;
  PWR_CFG_DATA              *PwrMgmtCfgData;

  Gnvs          = (GLOBAL_NVS_AREA *)GnvsIn;
  PchNvs        = &Gnvs->PchNvs;
  AcpiParameter = &Gnvs->BiosAcpiParam;
  ZeroMem (Gnvs, sizeof (GLOBAL_NVS_AREA));

  FspsUpd         = (FSPS_UPD *)(UINTN)PcdGet32 (PcdFspsUpdPtr);
  FspsConfig      = &FspsUpd->FspsConfig;
  PwrMgmtCfgData  = (PWR_CFG_DATA *) FindConfigDataByTag (CDATA_PWR_TAG);

  PchNvs->PchSeries                     = PchSeries ();
  PchNvs->PchGeneration                 = PchGeneration ();
  PchNvs->PchStepping                   = PchStepping ();
  PchNvs->HPTE                          = TRUE;
  PchNvs->HPTB                          = EFI_ACPI_EVENT_TIMER_BLOCK_ADDRESS;
  PchNvs->SBRG                          = PCH_PCR_BASE_ADDRESS;
  PchNvs->PMBS                          = ACPI_BASE_ADDRESS;
  PchNvs->PWRM                          = PCH_PWRM_BASE_ADDRESS;
  PchNvs->TcoBase                       = TCO_BASE_ADDRESS;
  PchNvs->SGIR                          = FspsConfig->PchGpioIrqRoute;

  PchNvs->GEI0                          = 0; //(UINT8) GPIO_IBL_GROUP_GPP_C;
  PchNvs->GEI1                          = 1; //(UINT8) GPIO_IBL_GROUP_GPP_D;
  PchNvs->GEI2                          = 2; //(UINT8) GPIO_IBL_GROUP_GPP_E;
  PchNvs->GED0                          = (UINT8) 0;
  PchNvs->GED1                          = (UINT8) 0;
  PchNvs->GED2                          = (UINT8) 0;

  // Initialize UART0 for console at 0x3E8 when S3M UART is used
  if (GetDebugPort () == S3M_UART_PORT) {
    PchNvs->UM0[0]                      = 3;      // UART_COM mode
    PchNvs->UC0[0]                      = 0x3E8;  // I/O port address
    PchNvs->UI0[0]                      = PchPIRQA; // IRQ 16 (APIC mode)
  }
  PchNvs->PsOnEnable                    = 0;
  AcpiParameter->PlatformId             = GetPlatformId ();
  AcpiParameter->IoApicEnable           = 0x1;
  AcpiParameter->PchIoApic_24_119       = 0;

  AsmCpuid (CPUID_VERSION_INFO, &CpuidRegisters.RegEax, &CpuidRegisters.RegEbx,
            &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx);
  DEBUG ((DEBUG_INFO, "CPUID_VERSION_INFO 0x%08X\n", CpuidRegisters.RegEax));
  AcpiParameter->ProcessorId            = (CpuidRegisters.RegEax & 0xFFFF0);

  NumOfBitShift = 0;
  AsmCpuidEx(CPUID_EXTENDED_TOPOLOGY, 1, &NumOfBitShift, NULL, NULL, NULL);
  NumOfBitShift &= 0x1F;
  AcpiParameter->CpuSkuNumOfBitShift    = (UINT8) NumOfBitShift;

  AcpiParameter->SncAnd2Cluster         = 0;
  AcpiParameter->MmCfg                  = (UINT32)(UINTN)PcdGet64 (PcdPciExpressBaseAddress);

  GuidHob = NULL;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    GuidHob = GetNextGuidHob (&gIioUdsHobGuid, FspHobList);
  }
  if (GuidHob != NULL) {
    IioUdsHobData = GET_GUID_HOB_DATA (GuidHob);
    if ((IioUdsHobData != NULL) && (IioUdsHobData->PlatformData.MemTsegSize != 0)) {
      AcpiParameter->TsegSize           = (UINT32)(IioUdsHobData->PlatformData.MemTsegSize >> 20); // Size in MB
    }
  }
  AcpiParameter->PmBase                 = ACPI_BASE_ADDRESS;

  AcpiParameter->C6Enable               = (UINT8)PPM_C6P_AS_ACPI_C2;
  if (PwrMgmtCfgData != NULL) {
    AcpiParameter->C1eEnable            = (UINT8)PwrMgmtCfgData->CState.C1eEnable;
    AcpiParameter->MonitorMwaitEnable   = (UINT8)PwrMgmtCfgData->CState.MonitorMWait;
    AcpiParameter->CStateEnable         = (UINT8)PwrMgmtCfgData->CState.EnableCstate;
    AcpiParameter->PStateEnable         = (UINT8)PwrMgmtCfgData->PState.EistEnable;
    AcpiParameter->HWAllEnable          = (UINT8)PwrMgmtCfgData->PState.PsdFunction;
    AcpiParameter->HWPMEnable           = (UINT8)PwrMgmtCfgData->PState.HWPMEnable;
  } else {
    DEBUG ((DEBUG_WARN, "No PwrMgmtCfgData found. Using default ACPI power values.\n"));
  }

  AcpiParameter->TpmEnable              = 0;
  AcpiParameter->DebugModeIndicator     = 1;
  AcpiParameter->TxtEnable              = 0x0;
  AcpiParameter->EmcaEn                 = 0x1;
  AcpiParameter->WheaSupportEn          = 0x1;
  AcpiParameter->XhciMode               = 0x0;
  AcpiParameter->ApicIdOverrided        = 0x1;
}
