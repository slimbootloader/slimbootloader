/** @file

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2BoardInitLib.h"

extern BOOLEAN MeTypeIsSpsIgnition (VOID);

BL_SCS_SD_DLL mScsSdDll;

PCH_DEVICE_INTERRUPT_CONFIG mDevIntConfig[] = {
//  {31, 0, PchNoInt, 0}, // LPC/eSPI Interface, doesn't use interrupts
//  {31, 1, PchNoInt, 0}, // P2SB, doesn't use interrupts
//  {31, 2, PchNoInt, 0}, // PMC , doesn't use interrupts
    {31, 4, PchIntB, PchPIRQB}, // SMBus Controller
//  {31, 5, PchNoInt, 0}, // SPI , doesn't use interrupts
//  {31, 6, PchNoInt, 0}, // NPK ACPI
    {31, 7, PchIntA, PchPIRQA}, // NPK
    {30, 0, PchIntA, PchPIRQA}, // USB 3.0 xHCI Controller
    {29, 0, PchIntA, PchPIRQA}, // CPU LEGACY
    {28, 0, PchIntA, PchPIRQA}, // eMMC standalone
    {27, 0, PchIntA, PchPIRQA}, // IE: HECI #1
    {27, 1, PchIntB, PchPIRQB}, // IE: HECI #2
    {27, 2, PchIntC, PchPIRQC}, // IE: IDE-Redirection (IDE-R)
    {27, 3, PchIntD, PchPIRQD}, // IE: Keyboard and Text (KT) Redirection
    {27, 4, PchIntA, PchPIRQA}, // IE: HECI #3
    {27, 6, PchIntB, PchPIRQB}, // IE: HECI #4
    {26, 0, PchIntA, PchPIRQA}, // UART0
    {26, 1, PchIntB, PchPIRQB}, // UART1
    {26, 2, PchIntC, PchPIRQC}, // UART2
    {24, 0, PchIntA, PchPIRQA}, // CSME: HECI #1
    {24, 1, PchIntB, PchPIRQB}, // CSME: HECI #2
    {24, 2, PchIntC, PchPIRQC}, // CSME: IDE-Redirection (IDE-R)
    {24, 3, PchIntD, PchPIRQD}, // CSME: Keyboard and Text (KT) Redirection
    {24, 4, PchIntA, PchPIRQA}, // CSME: HECI #3
    {24, 6, PchIntB, PchPIRQB}, // CSME: HECI #4
    {23, 0, PchIntA, PchPIRQA}, // PCIE_RP[11]
    {22, 0, PchIntA, PchPIRQA}, // PCIE_RP[10]
    {21, 0, PchIntA, PchPIRQA}, // PCIE_RP[9]
    {20, 0, PchIntA, PchPIRQA}, // PCIE_RP[8]
    {19, 0, PchIntA, PchPIRQA}, // PCIE_RP[7]
    {18, 0, PchIntA, PchPIRQA}, // PCIE_RP[6]
    {17, 0, PchIntA, PchPIRQA}, // PCIE_RP[5]
    {16, 0, PchIntA, PchPIRQA}, // PCIE_RP[4]
    {15, 0, PchIntA, PchPIRQA}, // SMBus 1
    {14, 0, PchIntA, PchPIRQA}, // SATA Controller 3
    {12, 0, PchIntA, PchPIRQA}, // PCIE_RP[3]
    {11, 0, PchIntA, PchPIRQA}, // PCIE_RP[2]
    {10, 0, PchIntA, PchPIRQA}, // PCIE_RP[1]
    { 9, 0, PchIntA, PchPIRQA}, // PCIE_RP[0]
    { 8, 0, PchIntA, PchPIRQA}, // SATA Controller 1
    { 7, 0, PchIntA, PchPIRQA}, // SATA Controller 0
    { 6, 0, PchIntA, PchPIRQA}, // VRP2
};

CONST EFI_ACPI_DESCRIPTION_HEADER  mAcpiTccRtctTableTemplate = {
  EFI_ACPI_RTCT_SIGNATURE,
  sizeof (EFI_ACPI_DESCRIPTION_HEADER)
  // Other fields will be updated in runtime
};

STATIC
CONST EFI_ACPI_COMMON_HEADER *mPlatformAcpiTables[] = {
  (EFI_ACPI_COMMON_HEADER *)&mAcpiTccRtctTableTemplate,
  NULL
};

BOOLEAN mTccDsoTuning      = FALSE;

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
  switch (InitPhase) {
  case PreSiliconInit:
    ConfigureGpio (CDATA_GPIO_TAG, 0, NULL);
    if (GetBootMode() != BOOT_ON_FLASH_UPDATE) {
      UpdatePayloadId ();
    }
    SpiConstructor ();
    (VOID) PcdSet32S (PcdAcpiTableTemplatePtr, (UINT32)(UINTN)mPlatformAcpiTables);
    break;
  case PostSiliconInit:
    PlatformPostSiliconInit ();
    InitializeSmbiosInfo ();
    SetTmeVar ();
    FreeSgxMem ();
    break;
  case PrePciEnumeration:
    PlatformPrePciEnumeration ();
    break;
  case PostPciEnumeration:
    PlatformPostPciEnumeration ();
    InterruptRoutingInit ();
    InitializeBmcVideo ();
    break;
  case PrePayloadLoading:
    if (MeTypeIsSpsIgnition() == FALSE) {
      HeciInitialize (HECI1_DEVICE);
      DumpDebugInfo ();
    }
    break;
  case EndOfStages:
    PlatformServiceInit ();
    HeciRegisterHeciService ();
    ClearSmi ();
    if ((GetBootMode() != BOOT_ON_FLASH_UPDATE) && (GetPayloadId() != 0)) {
      ProgramSecuritySetting ();
    }
    BuildSgxSblDataInfoHob ();
    FreeSgxMem();
    break;
  case ReadyToBoot:
    if ((GetBootMode() != BOOT_ON_FLASH_UPDATE) && (GetPayloadId() == 0)) {
      ProgramSecuritySetting ();
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
  Update FSP-S UPD config data for TCC mode and tuning

  @param  FspmUpd           The pointer to the FSP-S UPD to be updated.

  @retval None
**/
VOID
TccModePostMemConfig (
  FSPS_UPD  *FspsUpd
)
{
  UINT32                      *TccCacheconfigBase;
  UINT32                       TccCacheconfigSize;
  UINT32                      *TccCrlBase;
  UINT32                       TccCrlSize;
  UINT32                      *TccStreamBase;
  UINT32                       TccStreamSize;
  BIOS_SETTINGS               *PolicyConfig;
  TCC_STREAM_CONFIGURATION    *StreamConfig;
  EFI_STATUS                   Status;
  TCC_CFG_DATA                *TccCfgData;

  TccCfgData = (TCC_CFG_DATA *) FindConfigDataByTag(CDATA_TCC_TAG);
  if ((TccCfgData == NULL) || (TccCfgData->TccEnable == 0)) {
    return;
  }

  DEBUG ((DEBUG_INFO, "TccModePostMemConfig () - Start\n"));
  DEBUG ((DEBUG_INFO, "Set TCC silicon:\n"));

  FspsUpd->FspsConfig.PcdPcieRootPort0Aspm             = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort0L1SubStates      = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort1Aspm             = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort1L1SubStates      = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort2Aspm             = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort2L1SubStates      = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort3Aspm             = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort3L1SubStates      = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort4Aspm             = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort4L1SubStates      = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort5Aspm             = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort5L1SubStates      = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort6Aspm             = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort6L1SubStates      = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort7Aspm             = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort7L1SubStates      = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort8Aspm             = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort8L1SubStates      = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort9Aspm             = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort9L1SubStates      = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort10Aspm            = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort10L1SubStates     = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort11Aspm            = 0;
  FspsUpd->FspsConfig.PcdPcieRootPort11L1SubStates     = 0;

  if (!IsWdtFlagsSet (WDT_FLAG_TCC_DSO_IN_PROGRESS)) {
    //
    // If FSPM doesn't enable TCC DSO timer, FSPS should also skip TCC DSO.
    //
    DEBUG ((DEBUG_INFO, "DSO Tuning skipped.\n"));
  } else if (TccCfgData->TccTuning != 0) {
    // Reload Watch dog timer
    WdtReloadAndStart (WDT_TIMEOUT_TCC_DSO, WDT_FLAG_TCC_DSO_IN_PROGRESS);

    // Load TCC stream config from container
    TccStreamBase = NULL;
    TccStreamSize = 0;
    Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'T'),
                            (VOID **)&TccStreamBase, &TccStreamSize);
    if (EFI_ERROR (Status) || (TccStreamSize < sizeof (TCC_STREAM_CONFIGURATION))) {
      DEBUG ((DEBUG_INFO, "Load TCC Stream %r, size = 0x%x\n", Status, TccStreamSize));
    } else {
      FspsUpd->FspsConfig.PcdTccStreamCfgBase =          (UINT32)(UINTN)TccStreamBase;
      FspsUpd->FspsConfig.PcdTccStreamCfgSize =          TccStreamSize;
      DEBUG ((DEBUG_INFO, "Load tcc stream @0x%p, size = 0x%x\n", TccStreamBase, TccStreamSize));

      // Update UPD from stream
      if (TccCfgData->TccTuning != 0) {
        StreamConfig   = (TCC_STREAM_CONFIGURATION *) TccStreamBase;
        PolicyConfig = (BIOS_SETTINGS *) &StreamConfig->BiosSettings;

        FspsUpd->FspsConfig.PcdPcieRootPort0Aspm         = PolicyConfig->PchPcieAspm;
        FspsUpd->FspsConfig.PcdPcieRootPort0L1SubStates  = PolicyConfig->PchPcieRpL1;
        FspsUpd->FspsConfig.PcdPcieRootPort1Aspm         = PolicyConfig->PchPcieAspm;
        FspsUpd->FspsConfig.PcdPcieRootPort1L1SubStates  = PolicyConfig->PchPcieRpL1;
        FspsUpd->FspsConfig.PcdPcieRootPort2Aspm         = PolicyConfig->PchPcieAspm;
        FspsUpd->FspsConfig.PcdPcieRootPort2L1SubStates  = PolicyConfig->PchPcieRpL1;
        FspsUpd->FspsConfig.PcdPcieRootPort3Aspm         = PolicyConfig->PchPcieAspm;
        FspsUpd->FspsConfig.PcdPcieRootPort3L1SubStates  = PolicyConfig->PchPcieRpL1;
        FspsUpd->FspsConfig.PcdPcieRootPort4Aspm         = PolicyConfig->PchPcieAspm;
        FspsUpd->FspsConfig.PcdPcieRootPort4L1SubStates  = PolicyConfig->PchPcieRpL1;
        FspsUpd->FspsConfig.PcdPcieRootPort5Aspm         = PolicyConfig->PchPcieAspm;
        FspsUpd->FspsConfig.PcdPcieRootPort5L1SubStates  = PolicyConfig->PchPcieRpL1;
        FspsUpd->FspsConfig.PcdPcieRootPort6Aspm         = PolicyConfig->PchPcieAspm;
        FspsUpd->FspsConfig.PcdPcieRootPort6L1SubStates  = PolicyConfig->PchPcieRpL1;
        FspsUpd->FspsConfig.PcdPcieRootPort7Aspm         = PolicyConfig->PchPcieAspm;
        FspsUpd->FspsConfig.PcdPcieRootPort7L1SubStates  = PolicyConfig->PchPcieRpL1;
        FspsUpd->FspsConfig.PcdPcieRootPort8Aspm         = PolicyConfig->PchPcieAspm;
        FspsUpd->FspsConfig.PcdPcieRootPort8L1SubStates  = PolicyConfig->PchPcieRpL1;
        FspsUpd->FspsConfig.PcdPcieRootPort9Aspm         = PolicyConfig->PchPcieAspm;
        FspsUpd->FspsConfig.PcdPcieRootPort9L1SubStates  = PolicyConfig->PchPcieRpL1;
        FspsUpd->FspsConfig.PcdPcieRootPort10Aspm        = PolicyConfig->PchPcieAspm;
        FspsUpd->FspsConfig.PcdPcieRootPort10L1SubStates = PolicyConfig->PchPcieRpL1;
        FspsUpd->FspsConfig.PcdPcieRootPort11Aspm        = PolicyConfig->PchPcieAspm;
        FspsUpd->FspsConfig.PcdPcieRootPort11L1SubStates = PolicyConfig->PchPcieRpL1;
        mTccDsoTuning      = TRUE;
      }
    }
  }

  // Load TCC cache config binary from container
  TccCacheconfigBase = NULL;
  TccCacheconfigSize = 0;
  Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'C'),
                                (VOID **)&TccCacheconfigBase, &TccCacheconfigSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "TCC Cache config not found! %r\n", Status));
  } else {
    FspsUpd->FspsConfig.PcdTccCacheCfgBase             = (UINT32)(UINTN)TccCacheconfigBase;
    FspsUpd->FspsConfig.PcdTccCacheCfgSize             = TccCacheconfigSize;
    DEBUG ((DEBUG_INFO, "Load tcc cache @0x%p, size = 0x%x\n", TccCacheconfigBase, TccCacheconfigSize));
  }

  // Load TCC CRL binary from container
  TccCrlBase = NULL;
  TccCrlSize = 0;
  Status = LoadComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('T', 'C', 'C', 'M'),
                                (VOID **)&TccCrlBase, &TccCrlSize);
  DEBUG ((DEBUG_INFO, "Load tcc CrlBase @0x%p, size = 0x%x\n", TccCrlBase, TccCrlSize));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "TCC CRL not found! %r\n", Status));
  } else {
    FspsUpd->FspsConfig.PcdTccCrlBinBase               = (UINT32)(UINTN)TccCrlBase;
    FspsUpd->FspsConfig.PcdTccCrlBinSize               = TccCrlSize;
    DEBUG ((DEBUG_INFO, "Load tcc crl @0x%p, size = 0x%x\n", TccCrlBase, TccCrlSize));
  }

  DEBUG ((DEBUG_INFO, "TccModePostMemConfig () - End\n"));
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
  FSPS_UPD            *FspsUpd;
  FSP_S_CONFIG        *FspsConfig;
  SILICON_CFG_DATA    *SiCfgData;
  FEATURES_CFG_DATA   *FeaturesCfgData;
  EMMC_CFG_DATA       *EmmcCfgData;
  UINT8                PortIdx;
  UINT32               PortMask;
  UINT32               PortShift;
  UINT8                RangeIdx;
  UINT8                WriteIdx;
  UINT16               RgnMax;
  UINT16               VarBase;
  UINT16               VarLimit;
  UINT32               RgnBase;
  UINT32               RgnSize;
  UINT32               BiosRgnBase;
  UINT32               BiosRgnSize;
  UINT32               RangeData;
  MISC_CFG_DATA       *MiscCfgData;
  PCIE_CFG_DATA       *PcieCfgData;
  MEMORY_CFG_DATA     *MemCfgData;
  EFI_STATUS           Status;

  FspsUpd    = (FSPS_UPD *)FspsUpdPtr;
  FspsConfig = &FspsUpd->FspsConfig;

  SiCfgData  = (SILICON_CFG_DATA *) FindConfigDataByTag (CDATA_SILICON_TAG);
  if (SiCfgData != NULL) {
    FspsConfig->PcdEnableSATA           = (SiCfgData->SataConfig0.EnableSATA0 << 0);
    FspsConfig->PcdEnableSATA          |= (SiCfgData->SataConfig0.EnableSATA1 << 8);
    FspsConfig->PcdEnableSATA          |= (SiCfgData->SataConfig0.EnableSATA2 << 16);

    FspsConfig->PcdSATAmode             = (SiCfgData->SataConfig0.SATA0Mode << 0);
    FspsConfig->PcdSATAmode            |= (SiCfgData->SataConfig0.SATA1Mode << 8);
    FspsConfig->PcdSATAmode            |= (SiCfgData->SataConfig0.SATA2Mode << 16);

    FspsConfig->PcdSATAInterruptMode    = (SiCfgData->SataConfig0.SATA0IntMode << 0);
    FspsConfig->PcdSATAInterruptMode   |= (SiCfgData->SataConfig0.SATA1IntMode << 8);
    FspsConfig->PcdSATAInterruptMode   |= (SiCfgData->SataConfig0.SATA2IntMode << 16);

    FspsConfig->PcdSATA0PortEnable      = 0;
    FspsConfig->PcdSATA0PortHotplug     = 0;
    FspsConfig->PcdSATA1PortEnable      = 0;
    FspsConfig->PcdSATA1PortHotplug     = 0;
    FspsConfig->PcdSATA2PortEnable      = 0;
    FspsConfig->PcdSATA2PortHotplug     = 0;
    for (PortIdx = 0; PortIdx < GetPchMaxSataPortNum(); PortIdx++) {
      PortMask  = (1 << PortIdx);
      PortShift = (4 * PortIdx);
      FspsConfig->PcdSATA0PortEnable   |= ((SiCfgData->SataConfig0.SATA0PortEnable  & PortMask) == PortMask) << PortShift;
      FspsConfig->PcdSATA0PortHotplug  |= ((SiCfgData->SataConfig0.SATA0PortHotplug & PortMask) == PortMask) << PortShift;
      FspsConfig->PcdSATA1PortEnable   |= ((SiCfgData->SataConfig1.SATA1PortEnable  & PortMask) == PortMask) << PortShift;
      FspsConfig->PcdSATA1PortHotplug  |= ((SiCfgData->SataConfig1.SATA1PortHotplug & PortMask) == PortMask) << PortShift;
      FspsConfig->PcdSATA2PortEnable   |= ((SiCfgData->SataConfig1.SATA2PortEnable  & PortMask) == PortMask) << PortShift;
      FspsConfig->PcdSATA2PortHotplug  |= ((SiCfgData->SataConfig1.SATA2PortHotplug & PortMask) == PortMask) << PortShift;
    }

    //
    // FPRR settings
    //
    if (GetBootMode() != BOOT_ON_FLASH_UPDATE) {
      Status = SpiGetRegionAddress (FlashRegionBios, &BiosRgnBase, &BiosRgnSize);
      ASSERT_EFI_ERROR (Status);
      RgnMax = (UINT16)((BiosRgnBase + BiosRgnSize - 1) >> 12);
      DEBUG ((DEBUG_VERBOSE, "Bios Region(0x%X-0x%X), FPRR Region Max 0x%X\n", BiosRgnBase, (BiosRgnBase + BiosRgnSize - 1), RgnMax));

      MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
      ASSERT (MemCfgData != NULL);
      if ((MemCfgData != NULL) && (MemCfgData->GeneralConfig.TmeEnable != 0)) {
        Status = GetComponentInfo (FLASH_MAP_SIG_VARIABLE, &RgnBase, &RgnSize);
        DEBUG ((DEBUG_VERBOSE, "Variable Region(0x%X-0x%X)\n", RgnBase, (RgnBase + RgnSize - 1)));
        ASSERT_EFI_ERROR (Status);

        RgnBase -= ((UINT32)(~BiosRgnSize) + 1);
        RgnBase += BiosRgnBase;

        VarBase  = (UINT16)(RgnBase >> 12);
        VarLimit = (UINT16)((RgnBase + RgnSize - 1) >> 12);
      } else {
        VarBase  = 0;
        VarLimit = 0;
      }
      DEBUG ((DEBUG_VERBOSE, "Variable FPRR Region(0x%X-0x%X)\n", VarBase, VarLimit));

      for (RangeIdx = 0, WriteIdx = 0xFF; RangeIdx < 5; RangeIdx++) {
        FspsConfig->PcdWriteProtectionEnable[RangeIdx] = (UINT8)SiCfgData->FprrConfig.WriteEnable[RangeIdx];
        FspsConfig->PcdReadProtectionEnable[RangeIdx]  = (UINT8)SiCfgData->FprrConfig.ReadEnable[RangeIdx];
        RangeData = SiCfgData->FprrConfig.Range[RangeIdx];
        FspsConfig->PcdProtectedRangeBase[RangeIdx]    = (UINT16)(RangeData & 0xFFFF);
        FspsConfig->PcdProtectedRangeLimit[RangeIdx]   = (UINT16)(RangeData >> 16);

        if ((FspsConfig->PcdProtectedRangeBase[RangeIdx] > FspsConfig->PcdProtectedRangeLimit[RangeIdx]) ||
            (FspsConfig->PcdProtectedRangeBase[RangeIdx] > RgnMax) ||
            (FspsConfig->PcdProtectedRangeLimit[RangeIdx] > RgnMax)) {
          DEBUG ((DEBUG_ERROR, "Invalid FPRR setting! Base:0x%X, Limit:0x%X, MaxRange: 0x%X\n",
            FspsConfig->PcdProtectedRangeBase[RangeIdx],
            FspsConfig->PcdProtectedRangeLimit[RangeIdx],
            RgnMax));
          ASSERT (FALSE);
        }

        if ((VarBase != 0) && (VarLimit != 0)) {
          //
          // Check if Variable Region is already covered
          //
          if ((FspsConfig->PcdWriteProtectionEnable[RangeIdx] != 0) &&
              (FspsConfig->PcdProtectedRangeBase[RangeIdx] != 0) &&
              (FspsConfig->PcdProtectedRangeLimit[RangeIdx] != 0) &&
              (FspsConfig->PcdProtectedRangeBase[RangeIdx] <= VarBase) &&
              (FspsConfig->PcdProtectedRangeLimit[RangeIdx] >= VarLimit)) {
            VarBase  = 0;
            VarLimit = 0;
          }

          // Save the 1st available slot for later use
          if ((FspsConfig->PcdProtectedRangeBase[RangeIdx] == 0) &&
              (FspsConfig->PcdProtectedRangeLimit[RangeIdx] == 0) &&
              (WriteIdx == 0xFF)) {
            WriteIdx = RangeIdx;
          }
        }
      }

      if ((VarBase != 0) && (VarLimit != 0)) {
        if (WriteIdx != 0xFF) {
          FspsConfig->PcdWriteProtectionEnable[WriteIdx] = 1;
          FspsConfig->PcdProtectedRangeBase[WriteIdx]    = VarBase;
          FspsConfig->PcdProtectedRangeLimit[WriteIdx]   = VarLimit;
          DEBUG ((DEBUG_VERBOSE, "ProtectedRange Base:0x%X Limit:0x%X\n",
            FspsConfig->PcdProtectedRangeBase[WriteIdx],
            FspsConfig->PcdProtectedRangeLimit[WriteIdx]));
        } else {
          DEBUG ((DEBUG_ERROR, "No available FPRR config slot found for Variable Region!\n"));
          ASSERT (FALSE);
        }
      }
    }

    FspsConfig->PcdDevIntConfigPtr              = (UINT32) (UINTN) (mDevIntConfig);
    FspsConfig->PcdNumOfDevIntConfig            = sizeof (mDevIntConfig) / sizeof (PCH_DEVICE_INTERRUPT_CONFIG);
    FspsConfig->PcdIntConfigPxRcConfig          = LShiftU64 ((UINT64)SiCfgData->IntConfig.PxRcConfig1, 32) | SiCfgData->IntConfig.PxRcConfig0;
    FspsConfig->PcdIntConfigGpioIrqRoute        = (UINT8)SiCfgData->IntConfig.GpioIrqRoute;
    FspsConfig->PcdIntConfigSciIrqSelect        = (UINT8)SiCfgData->IntConfig.SciIrqSelect;
  }

  // EMMC DLL Settings
  EmmcCfgData = (EMMC_CFG_DATA *) FindConfigDataByTag (CDATA_EMMC_TAG);
  if (EmmcCfgData != NULL) {
    mScsSdDll.TxCmdDelayControl         = EmmcCfgData->TxCmdDelayControl;
    mScsSdDll.TxDataDelayControl1       = EmmcCfgData->TxDataDelayControl1;
    mScsSdDll.TxDataDelayControl2       = EmmcCfgData->TxDataDelayControl2;
    mScsSdDll.RxCmdDataDelayControl1    = EmmcCfgData->RxCmdDataDelayControl1;
    mScsSdDll.RxCmdDataDelayControl2    = EmmcCfgData->RxCmdDataDelayControl2;
    mScsSdDll.RxStrobeDelayControl      = EmmcCfgData->RxStrobeDelayControl;
  }

  // Misc Settings
  MiscCfgData = (MISC_CFG_DATA *) FindConfigDataByTag (CDATA_MISC_TAG);
  if (MiscCfgData == NULL) {
    DEBUG ((DEBUG_WARN, "No MISC CFGDATA. Continue to boot with default values!\n"));
  } else {
    FspsConfig->PcdEmmc                 = (UINT8) MiscCfgData->MiscCfg.Emmc;
    FspsConfig->PcdEmmcHS400Support     = (UINT8) MiscCfgData->MiscCfg.EmmcHS400Support;
  }

  // Pcie Settings
  PcieCfgData = (PCIE_CFG_DATA *) FindConfigDataByTag (CDATA_PCIE_TAG);
  if (PcieCfgData == NULL) {

  } else {
    FspsConfig->PcdPcieRootPortVppOverride    = 0;
    FspsConfig->PcdPcieRootPortVppPort        = 0;
    FspsConfig->PcdPcieRootPortVppAddress     = 0;
    FspsConfig->PcdPcieRootPortPtmEnable      = 0;
    for (PortIdx = 0; PortIdx < GetPchMaxPciePortNum(); PortIdx++) {
      PortMask  = 0x0F;
      PortShift = (4 * PortIdx);
      FspsConfig->PcdPcieRootPortVppOverride |= LShiftU64 ((UINT64)(PcieCfgData->PchRpConfig.VppOverride[PortIdx] & PortMask), PortShift);
      FspsConfig->PcdPcieRootPortVppPort     |= LShiftU64 ((UINT64)(PcieCfgData->PchRpConfig.VppPort[PortIdx]     & PortMask), PortShift);
      FspsConfig->PcdPcieRootPortVppAddress  |= LShiftU64 ((UINT64)(PcieCfgData->PchRpConfig.VppAddress[PortIdx]  & PortMask), PortShift);
      FspsConfig->PcdPcieRootPortPtmEnable   |= LShiftU64 ((UINT64)(PcieCfgData->PchRpConfig.PtmEnable[PortIdx]   & PortMask), PortShift);
    }

    FspsConfig->PcdPcieClockGatingEnabled       = PcieCfgData->PchRpConfig.ClockGatingEnabled;
    FspsConfig->PcdPchRlinkClockGating          = PcieCfgData->PchRpConfig.RlinkClockGating;

    FspsConfig->PcdPcieRootPort0LinkSpeed       = PcieCfgData->PchRpConfig.LinkSpeed[0];
    FspsConfig->PcdPcieRootPort1LinkSpeed       = PcieCfgData->PchRpConfig.LinkSpeed[1];
    FspsConfig->PcdPcieRootPort2LinkSpeed       = PcieCfgData->PchRpConfig.LinkSpeed[2];
    FspsConfig->PcdPcieRootPort3LinkSpeed       = PcieCfgData->PchRpConfig.LinkSpeed[3];
    FspsConfig->PcdPcieRootPort4LinkSpeed       = PcieCfgData->PchRpConfig.LinkSpeed[4];
    FspsConfig->PcdPcieRootPort5LinkSpeed       = PcieCfgData->PchRpConfig.LinkSpeed[5];
    FspsConfig->PcdPcieRootPort6LinkSpeed       = PcieCfgData->PchRpConfig.LinkSpeed[6];
    FspsConfig->PcdPcieRootPort7LinkSpeed       = PcieCfgData->PchRpConfig.LinkSpeed[7];
    FspsConfig->PcdPcieRootPort8LinkSpeed       = PcieCfgData->PchRpConfig.LinkSpeed[8];
    FspsConfig->PcdPcieRootPort9LinkSpeed       = PcieCfgData->PchRpConfig.LinkSpeed[9];
    FspsConfig->PcdPcieRootPort10LinkSpeed      = PcieCfgData->PchRpConfig.LinkSpeed[10];
    FspsConfig->PcdPcieRootPort11LinkSpeed      = PcieCfgData->PchRpConfig.LinkSpeed[11];
    FspsConfig->PcdPcieRootPort0Aspm            = PcieCfgData->PchRpConfig.Aspm[0];
    FspsConfig->PcdPcieRootPort1Aspm            = PcieCfgData->PchRpConfig.Aspm[1];
    FspsConfig->PcdPcieRootPort2Aspm            = PcieCfgData->PchRpConfig.Aspm[2];
    FspsConfig->PcdPcieRootPort3Aspm            = PcieCfgData->PchRpConfig.Aspm[3];
    FspsConfig->PcdPcieRootPort4Aspm            = PcieCfgData->PchRpConfig.Aspm[4];
    FspsConfig->PcdPcieRootPort5Aspm            = PcieCfgData->PchRpConfig.Aspm[5];
    FspsConfig->PcdPcieRootPort6Aspm            = PcieCfgData->PchRpConfig.Aspm[6];
    FspsConfig->PcdPcieRootPort7Aspm            = PcieCfgData->PchRpConfig.Aspm[7];
    FspsConfig->PcdPcieRootPort8Aspm            = PcieCfgData->PchRpConfig.Aspm[8];
    FspsConfig->PcdPcieRootPort9Aspm            = PcieCfgData->PchRpConfig.Aspm[9];
    FspsConfig->PcdPcieRootPort10Aspm           = PcieCfgData->PchRpConfig.Aspm[10];
    FspsConfig->PcdPcieRootPort11Aspm           = PcieCfgData->PchRpConfig.Aspm[11];
    FspsConfig->PcdPcieRootPort0ConnectionType  = PcieCfgData->PchRpConfig.ConnectionType[0];
    FspsConfig->PcdPcieRootPort1ConnectionType  = PcieCfgData->PchRpConfig.ConnectionType[1];
    FspsConfig->PcdPcieRootPort2ConnectionType  = PcieCfgData->PchRpConfig.ConnectionType[2];
    FspsConfig->PcdPcieRootPort3ConnectionType  = PcieCfgData->PchRpConfig.ConnectionType[3];
    FspsConfig->PcdPcieRootPort4ConnectionType  = PcieCfgData->PchRpConfig.ConnectionType[4];
    FspsConfig->PcdPcieRootPort5ConnectionType  = PcieCfgData->PchRpConfig.ConnectionType[5];
    FspsConfig->PcdPcieRootPort6ConnectionType  = PcieCfgData->PchRpConfig.ConnectionType[6];
    FspsConfig->PcdPcieRootPort7ConnectionType  = PcieCfgData->PchRpConfig.ConnectionType[7];
    FspsConfig->PcdPcieRootPort8ConnectionType  = PcieCfgData->PchRpConfig.ConnectionType[8];
    FspsConfig->PcdPcieRootPort9ConnectionType  = PcieCfgData->PchRpConfig.ConnectionType[9];
    FspsConfig->PcdPcieRootPort10ConnectionType = PcieCfgData->PchRpConfig.ConnectionType[10];
    FspsConfig->PcdPcieRootPort11ConnectionType = PcieCfgData->PchRpConfig.ConnectionType[11];
    FspsConfig->PcdPcieRootPort0HotPlug         = PcieCfgData->PchRpConfig.HotPlug[0];
    FspsConfig->PcdPcieRootPort1HotPlug         = PcieCfgData->PchRpConfig.HotPlug[1];
    FspsConfig->PcdPcieRootPort2HotPlug         = PcieCfgData->PchRpConfig.HotPlug[2];
    FspsConfig->PcdPcieRootPort3HotPlug         = PcieCfgData->PchRpConfig.HotPlug[3];
    FspsConfig->PcdPcieRootPort4HotPlug         = PcieCfgData->PchRpConfig.HotPlug[4];
    FspsConfig->PcdPcieRootPort5HotPlug         = PcieCfgData->PchRpConfig.HotPlug[5];
    FspsConfig->PcdPcieRootPort6HotPlug         = PcieCfgData->PchRpConfig.HotPlug[6];
    FspsConfig->PcdPcieRootPort7HotPlug         = PcieCfgData->PchRpConfig.HotPlug[7];
    FspsConfig->PcdPcieRootPort8HotPlug         = PcieCfgData->PchRpConfig.HotPlug[8];
    FspsConfig->PcdPcieRootPort9HotPlug         = PcieCfgData->PchRpConfig.HotPlug[9];
    FspsConfig->PcdPcieRootPort10HotPlug        = PcieCfgData->PchRpConfig.HotPlug[10];
    FspsConfig->PcdPcieRootPort11HotPlug        = PcieCfgData->PchRpConfig.HotPlug[11];
  }

  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
  if ((FeaturesCfgData != NULL) && (FeaturesCfgData->Features.eMMCTuning == 0) && (EmmcCfgData != NULL)) {
    FspsConfig->PcdEMMCDLLConfigPtr = (UINT32)(UINTN) &mScsSdDll;
  }

  if ((FeaturePcdGet (PcdTccEnabled)) && (GetBootMode() != BOOT_ON_FLASH_UPDATE)) {
    TccModePostMemConfig (FspsUpd);
  }

  if (!UpdateFspsSgxConfig (FspsUpd)) {
     DEBUG ((DEBUG_INFO, "FSP-S variables for Intel(R) SGX were NOT updated.\n"));
   }

  if (GetBootMode() == BOOT_ON_FLASH_UPDATE) {
    FspsConfig->PcdLockDownBiosWpd        = 1;
    FspsConfig->PcdLockDownBiosInterface  = 0;
    FspsConfig->PcdLockDownBiosLock       = 0;
    FspsConfig->PcdSbAccessUnlock         = 1;
  }
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
 Update serial port information to global HOB data structure.

 @param SerialPortInfo  Pointer to global HOB data structure.
 **/
VOID
EFIAPI
UpdateSerialPortInfo (
  IN  SERIAL_PORT_INFO  *SerialPortInfo
  )
{
  SerialPortInfo->Type     = 1;
  SerialPortInfo->BaseAddr64 = GetSerialPortBase ();
  SerialPortInfo->BaseAddr   = (UINT32) SerialPortInfo->BaseAddr64;
  SerialPortInfo->RegWidth = GetSerialPortStrideSize ();

  DEBUG ((DEBUG_INFO, "SerialPortInfo Type=%d BaseAddr=0x%08X RegWidth=%d\n",
    SerialPortInfo->Type, SerialPortInfo->BaseAddr, SerialPortInfo->RegWidth));
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
  UINT8                                   Instance;
  OS_BOOT_OPTION                         *PrevBootOption;
  OS_BOOT_OPTION                         *CurrBootOption;
  TCC_CFG_DATA                           *TccCfgData;
  UINT32                                  Idx;

  TccCfgData = (TCC_CFG_DATA *) FindConfigDataByTag(CDATA_TCC_TAG);
  if ((TccCfgData != NULL) && (TccCfgData->TccEnable != 0)) {
    PrevBootOption = NULL;
    for (Idx = 0; Idx < MAX_BOOT_OPTION_CFGDATA_ENTRY; Idx++) {
      CurrBootOption = (OS_BOOT_OPTION *)FindConfigDataByTag (CDATA_BOOT_OPTION_TAG + Idx);

      if (CurrBootOption == NULL) {
        break;
      }
      if ((CurrBootOption->ImageType && 0x1F == 0x1F) &&
          ((PrevBootOption != NULL) && (PrevBootOption->ImageType == 0))) {
        PrevBootOption->BootFlags = 24;
        CurrBootOption->ImageType = 0x1E;
        CurrBootOption->PreOsImageType = 0;
      }
      PrevBootOption = CurrBootOption;
    }
  }

  FillBootOptionListFromCfgData (OsBootOptionList);

  //
  // Depends on the PCI root bridge, pluged PCI devices, the bus number for NVMe device
  // might be different. so update the NVMe bus number in the device table.
  //
  Instance = 0;
  for (Bus = 1; Bus <= PCI_MAX_BUS; Bus++) {
    PciDev = (volatile PCI_DEVICE_INDEPENDENT_REGION *) MM_PCI_ADDRESS (Bus, 0, 0, 0);
    if (PciDev->DeviceId != 0xFFFF) {
      if ((PciDev->ClassCode[0] == 2) && (PciDev->ClassCode[1] == 8) && (PciDev->ClassCode[2] == 1)) {
        SetDeviceAddr (OsBootDeviceNvme, Instance, (UINT32)(Bus << 16));
        Instance += 1;
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
 Update the frame buffer device info.

 @param[out] GfxDeviceInfo   pointer to EFI_PEI_GRAPHICS_DEVICE_INFO_HOB structure.
**/
VOID
UpdateFrameBufferDeviceInfo (
  OUT  EFI_PEI_GRAPHICS_DEVICE_INFO_HOB   *GfxDeviceInfo
  )
{
  PLATFORM_MISC   *PlatformMisc;

  PlatformMisc = (PLATFORM_MISC *)FindConfigDataByTag (CDATA_PLTMISC_TAG);
  if ((PlatformMisc != NULL) && (PlatformMisc->ActiveVideo == 0)) {
    //
    // Aspeed2500 by default
    //
    GfxDeviceInfo->VendorId = 0x1A03;
    GfxDeviceInfo->DeviceId = 0x2000;
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
  PLATFORM_DATA               *PlatformData;
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
  } else if (Guid == &gLoaderSerialPortInfoGuid) {
    UpdateSerialPortInfo (HobInfo);
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
  PSD_CFG_DATA                          *PsdCfgData;

  Status = EFI_SUCCESS;

  Table = (EFI_ACPI_DESCRIPTION_HEADER *) Current;
  switch (Table->Signature) {
  case EFI_ACPI_5_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    Status = PatchDsdtTable (Table);
    break;
  case EFI_ACPI_VTD_DMAR_TABLE_SIGNATURE:
    Status = PatchDmarTable (Table);
    break;
  case EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE:
    Status = PatchTpm2AcpiTable (Table);
    break;
  case EFI_ACPI_5_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    if (Table->OemTableId == SIGNATURE_64 ('S', 'S', 'D', 'T', ' ', ' ', 'P', 'M')) {
      Status = PatchCpuPmSsdtTable (Table);
    }
    if (Table->OemTableId == SIGNATURE_64 ('S', 'p', 's', 'N', 'm', ' ', ' ', ' ')) {
      Status = PatchSpsNmSsdtTable (Table);
    }
    break;
  case OEM1_SSDT_TABLE_SIGNATURE:
    if (Table->OemTableId == SIGNATURE_64 ('C', 'P', 'U', ' ', 'E', 'I', 'S', 'T')) {
      Status = PatchOem1SsdtTable (Table);
    }
    break;
  case EFI_ACPI_RTCT_SIGNATURE:
    if (Table->Signature == SIGNATURE_32 ('R', 'T', 'C', 'T')) {
      DEBUG ((DEBUG_INFO, "Find RTCT table\n"));
      Status = PatchRtctAcpiTable (Table);
    }
    break;
  case EFI_BDAT_TABLE_SIGNATURE:
    Status = PatchBdatTable (Table);
    break;
  case EFI_ACPI_PSD_SIGNATURE:
    Status = EFI_UNSUPPORTED;
    if (FeaturePcdGet (PcdPsdBiosEnabled)) {
      PsdCfgData = (PSD_CFG_DATA *)FindConfigDataByTag (CDATA_PSD_TAG);
      if ((PsdCfgData != NULL) && (PsdCfgData->EnablePsd == 1)) {
        Status = UpdateAcpiPsdTable ( (VOID* )Current);
        DEBUG ( (DEBUG_INFO, "Updated Psd Table in AcpiTable Entries\n") );
      }
    }
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
  RETURN_STATUS             Status;
  UINT32                    IioPresentBitMask;
  EFI_CPUID_REGISTER        CpuidRegisters;
  UINT32                    NumOfBitShift;
  GPIO_GROUP                GroupToGpeDwX[3];
  UINT32                    GroupDw[3];
  UINTN                     RpDev;
  UINTN                     RpFun;
  UINT32                    Data32;
  UINTN                     Index;
  UINTN                     ResIndex;
  UINT8                     PchMaxPciePortNum;
  UINT8                     CoreCount;
  UINT8                     HyperThreading;
  UINTN                     Address;
  GLOBAL_NVS_AREA           *Gnvs;
  PCH_NVS_AREA              *PchNvs;
  BIOS_ACPI_PARAM           *AcpiParameter;
  PCI_RES_ALLOC_TABLE       *ResAllocTable;
  PCI_RES_ALLOC_RANGE       *ResRange;
  SILICON_CFG_DATA          *SiCfgData;
  SYS_CPU_INFO              *CpuInfo;
  MISC_CFG_DATA             *MiscCfgData;
  PWR_CFG_DATA              *PwrMgmtCfgData;
  TYPE_CHOPTYPE_ENUM        ChopType;
  PLATFORM_DATA             *PlatformData;

  Gnvs          = (GLOBAL_NVS_AREA *)GnvsIn;
  PchNvs        = &Gnvs->PchNvs;
  AcpiParameter = &Gnvs->BiosAcpiParam;
  ZeroMem (Gnvs, sizeof (GLOBAL_NVS_AREA));

  SiCfgData       = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  MiscCfgData     = (MISC_CFG_DATA *) FindConfigDataByTag (CDATA_MISC_TAG);
  PwrMgmtCfgData  = (PWR_CFG_DATA *) FindConfigDataByTag (CDATA_PWR_TAG);

  //
  // PCH_NVS_AREA
  //
  PchMaxPciePortNum = GetPchMaxPciePortNum ();
  for (Index = 0; Index < PchMaxPciePortNum; Index++) {
    PchNvs->LtrEnable[Index]            = (UINT8)TRUE;
    PchNvs->PcieLtrMaxSnoopLatency[Index]   = 0x0846;
    PchNvs->PcieLtrMaxNoSnoopLatency[Index] = 0x0846;

    Status = GetPchPcieRpDevFun (Index, &RpDev, &RpFun);
    if (!EFI_ERROR (Status)) {
      Data32 = ((UINT8) RpDev << 16) | (UINT8) RpFun;
      PchNvs->RpAddress[Index]          = Data32;
    }
  }

  PchNvs->MmCfgAddress                  = (UINT32)(UINTN)PcdGet64 (PcdPciExpressBaseAddress);

  PchNvs->PchSeries                     = PchSeries ();
  PchNvs->PchGeneration                 = PchGeneration ();
  PchNvs->PchStepping                   = PchStepping ();

  PchNvs->HPTE                          = TRUE;
  PchNvs->HPTB                          = EFI_ACPI_EVENT_TIMER_BLOCK_ADDRESS;
  PchNvs->PMBS                          = ACPI_BASE_ADDRESS;
  Address = MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_P2SB, PCI_FUNCTION_NUMBER_PCH_P2SB, 0);
  if (MmioRead16 (Address) == 0xFFFF) {
    PchNvs->SBRG                        = PCH_PCR_BASE_ADDRESS;
  } else {
    PchNvs->SBRG                        = MmioRead32 (Address + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFFFF0;
  }
  Address = MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, 0);
  if (MmioRead16 (Address) == 0xFFFF) {
    PchNvs->PWRM                        = PCH_PWRM_BASE_ADDRESS;
  } else {
    PchNvs->PWRM                        = MmioRead32 (Address + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFFFF0;
  }
  PchNvs->SGIR                          = (SiCfgData != NULL) ? (UINT8)SiCfgData->IntConfig.GpioIrqRoute : 21;
  GpioGetGroupDwToGpeDwX (
    &GroupToGpeDwX[0], &GroupDw[0],
    &GroupToGpeDwX[1], &GroupDw[1],
    &GroupToGpeDwX[2], &GroupDw[2]
    );
  PchNvs->GEI0                          = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[0]);
  PchNvs->GEI1                          = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[1]);
  PchNvs->GEI2                          = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[2]);
  PchNvs->GED0                          = (UINT8) GroupDw[0];
  PchNvs->GED1                          = (UINT8) GroupDw[1];
  PchNvs->GED2                          = (UINT8) GroupDw[2];

  if (MiscCfgData != NULL) {
    PchNvs->EMH4                        = (UINT8)MiscCfgData->MiscCfg.EmmcHS400Support;
    PchNvs->EmmcEnabled                 = (UINT8)MiscCfgData->MiscCfg.Emmc;
  } else {
    PchNvs->EMH4                        = (UINT8)1;
    PchNvs->EmmcEnabled                 = (UINT8)1;
  }
  PchNvs->EMDS                          = (UINT8)0x4;

  PchNvs->DynamicS0i3Support            = 0;
  PchNvs->PsOnEnable                    = 0;
  PchNvs->GBES                          = 0;
  PchNvs->PchTraceHubMode               = (UINT8)FALSE;

  PchNvs->UM0[0]                        = 1;
  PchNvs->UP0[0]                        = 0x2F8;
  PchNvs->UI0[0]                        = PchPIRQA;
  PchNvs->UM0[1]                        = 1;
  PchNvs->UP0[1]                        = 0x3E8;
  PchNvs->UI0[1]                        = PchPIRQB;
  PchNvs->UM0[2]                        = 1;
  PchNvs->UP0[2]                        = 0x2E8;
  PchNvs->UI0[2]                        = PchPIRQC;

  //
  // BIOS_ACPI_PARAM
  //
  AcpiParameter->PlatformId             = GetPlatformId ();
  AcpiParameter->IoApicEnable           = PCH_IOAPIC;
  AcpiParameter->PchIoApic_24_119       = 0;
  AsmCpuid (CPUID_VERSION_INFO, &CpuidRegisters.RegEax, &CpuidRegisters.RegEbx,
    &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx);
  DEBUG ((DEBUG_INFO, "CPUID_VERSION_INFO 0x%08X\n", CpuidRegisters.RegEax));
  AcpiParameter->ProcessorId            = (CpuidRegisters.RegEax & 0xFFFF0);

  AsmCpuidEx(CPUID_EXTENDED_TOPOLOGY, 1, &NumOfBitShift, NULL, NULL, NULL);
  DEBUG ((DEBUG_INFO, "CPUID_EXTENDED_TOPOLOGY 0x%08X\n", NumOfBitShift));
  NumOfBitShift &= 0x1F;
  AcpiParameter->CpuSkuNumOfBitShift    = (UINT8) NumOfBitShift;
  AcpiParameter->ProcessorApicIdBase[0] = (UINT32) (0 << NumOfBitShift);
  AcpiParameter->ProcessorApicIdBase[1] = (UINT32) (1 << NumOfBitShift);
  AcpiParameter->ProcessorApicIdBase[2] = (UINT32) (2 << NumOfBitShift);
  AcpiParameter->ProcessorApicIdBase[3] = (UINT32) (3 << NumOfBitShift);
  AcpiParameter->ProcessorApicIdBase[4] = (UINT32) (4 << NumOfBitShift);
  AcpiParameter->ProcessorApicIdBase[5] = (UINT32) (5 << NumOfBitShift);
  AcpiParameter->ProcessorApicIdBase[6] = (UINT32) (6 << NumOfBitShift);
  AcpiParameter->ProcessorApicIdBase[7] = (UINT32) (7 << NumOfBitShift);

  AcpiParameter->SncAnd2Cluster         = 0;
  AcpiParameter->MmCfg                  = (UINT32)(UINTN)PcdGet64 (PcdPciExpressBaseAddress);
  AcpiParameter->TsegSize               = (MiscCfgData != NULL) ? (SIZE_4MB << MiscCfgData->MiscCfg.TsegSize) : SIZE_128MB;

  CpuInfo         = MpGetInfo ();
  CoreCount       = (CpuInfo != NULL) ? (UINT8)CpuInfo->CpuCount : 1;
  AcpiParameter->CpuCoreThreadsCount    = CoreCount;
  PlatformData    = (PLATFORM_DATA *) GetPlatformDataPtr ();
  if (PlatformData != NULL) {
    HyperThreading = (UINT8)PlatformData->PlatformFeatures.HyperThreading;
  } else {
    HyperThreading = (MiscCfgData != NULL) ? (UINT8)MiscCfgData->MiscCfg.HyperThreading : 0;
  }

  if ((HyperThreading != 0) && (CoreCount != 1)) {
    CoreCount = CoreCount / 2;
  }
  for (Index = 0; Index < CoreCount; Index++) {
    AcpiParameter->ProcessorBitMask[0] |= (UINT64)(1 << Index);
  }
  AcpiParameter->ProcessorBitMaskHi[0]  = 0;

  // PatchDsdtTable
  AcpiParameter->SocketBitMask          = (1 << 0);
  Status = GetIioPresentBitMask (0, &IioPresentBitMask);
  if (!RETURN_ERROR (Status)) {
    AcpiParameter->IioPresentBitMask[0] = (UINT16)IioPresentBitMask;
  }

  ResAllocTable = (PCI_RES_ALLOC_TABLE *)(UINTN)PcdGet32 (PcdPciResAllocTableBase);
  if (ResAllocTable != NULL) {
    ChopType = GetChopType (0);
    for (ResIndex = 0, Index = 0; ResIndex < ResAllocTable->NumOfEntries; ResIndex++, Index++) {
      ResRange = &ResAllocTable->ResourceRange[ResIndex];

      // HCC has PCIE1 in stack5
      if ((ChopType == TypeHcc) && (ResAllocTable->NumOfEntries == 4) && (ResIndex == 3)) {
        Index = Index + 2;
      }
      if ((ChopType == TypeHcc) && (ResIndex == 4)) {
        Index++;
      }

      if ((ResRange != NULL) && (ResRange->BusLimit != 0)) {
        AcpiParameter->BusBase[0][Index]      = ResRange->BusBase;
        AcpiParameter->BusLimit[Index]        = ResRange->BusLimit;
        if (ResRange->IoLimit != 0) {
          AcpiParameter->Io16Base[Index]      = (UINT16) ResRange->IoBase;
          AcpiParameter->Io16Length[Index]    = (UINT16) (ResRange->IoLimit - ResRange->IoBase + 1);
        }
        if (ResRange->Mmio32Limit != 0) {
          AcpiParameter->Mmio32Base[Index]    = ResRange->Mmio32Base;
          AcpiParameter->Mmio32Length[Index]  = (ResRange->Mmio32Limit - ResRange->Mmio32Base + 1);
        }
        if (ResRange->Mmio64Limit != 0) {
          AcpiParameter->Mmio64Base[Index]    = ResRange->Mmio64Base;
          AcpiParameter->Mmio64Length[Index]  = (ResRange->Mmio64Limit - ResRange->Mmio64Base + 1);
        }
      }
    }
  }

  AcpiParameter->PropagateSerrOption    = 1;
  AcpiParameter->PropagatePerrOption    = 1;
  AcpiParameter->IioPcieRpCapOffset     = 0x40;

  AcpiParameter->UcBusBase[0]           = PCI_MAX_BUS - 1;
  AcpiParameter->UcBusLimit[0]          = AcpiParameter->UcBusBase[0];
  AcpiParameter->UcBusBase[1]           = PCI_MAX_BUS;
  AcpiParameter->UcBusLimit[1]          = AcpiParameter->UcBusBase[1];
  AcpiParameter->IioPresentBitMask[0]  |= (1 << (MAX_LOGIC_IIO_STACK - 2));
  AcpiParameter->IioPresentBitMask[0]  |= (1 << (MAX_LOGIC_IIO_STACK - 1));

  AcpiParameter->PmBase                 = ACPI_BASE_ADDRESS;

  // Power Management
  if (PwrMgmtCfgData != NULL) {
    if (mTccDsoTuning) {
      AcpiParameter->MonitorMwaitEnable = 0;
      AcpiParameter->CStateEnable       = 0;
      AcpiParameter->C6Enable           = 0;
      AcpiParameter->C3Enable           = 0;
      AcpiParameter->PStateEnable       = 0;
      AcpiParameter->HWPMEnable         = 0;
    } else {
      AcpiParameter->MonitorMwaitEnable = (UINT8)PwrMgmtCfgData->CState.MonitorMWait;
      AcpiParameter->CStateEnable       = (UINT8)PwrMgmtCfgData->CState.EnableCstate;
      AcpiParameter->PStateEnable       = (UINT8)PwrMgmtCfgData->PState.EistEnable;
      AcpiParameter->HWPMEnable         = (UINT8)PwrMgmtCfgData->PState.HWPMEnable;
    }
    if (AcpiParameter->CStateEnable != 0) {
      AcpiParameter->C6Enable           = (UINT8)((PwrMgmtCfgData->CState.C6Enable == 0xFF) ? 1 : PwrMgmtCfgData->CState.C6Enable);
      AcpiParameter->C3Enable           = (UINT8)PwrMgmtCfgData->CState.C3Enable;
      if (AcpiParameter->C6Enable != 0) {
        AcpiParameter->C3Enable         = 0;
      }
      AcpiParameter->OSCX               = (UINT8)PwrMgmtCfgData->CState.ReportOSCX;
    }
    AcpiParameter->HWAllEnable          = (UINT8)PwrMgmtCfgData->PState.PsdFunction;
  } else {
    AcpiParameter->MonitorMwaitEnable   = 1;
    AcpiParameter->CStateEnable         = 1;
    AcpiParameter->C6Enable             = 1;
    AcpiParameter->PStateEnable         = 1;
    AcpiParameter->HWPMEnable           = 1;
  }

  AcpiParameter->TpmEnable              = 1;
  AcpiParameter->DebugModeIndicator     = 1;

}
