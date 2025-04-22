/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Pi/PiBootMode.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <Library/ConfigDataLib.h>
#include <FspsUpd.h>
#include <PlatformBoardId.h>
#include <Register/Cpuid.h>
#include <Library/BoardSupportLib.h>
#include <IgdOpRegionDefines.h>
#include <PchLimits.h>
#include <ConfigDataDefs.h>
#include <ConfigDataCommonStruct.h>
#include <CpuDataStruct.h>
#include <CpuRegs.h>
#include <Library/PchInfoLib.h>
#include <PlatformBase.h>
#include <Library/IoLib.h>
#include <Library/SpiFlashLib.h>
#include <Register/PmcRegs.h>
#include <Library/VTdLib.h>
#include <Library/WatchDogTimerLib.h>
#include <PlatformData.h>
#include <Library/ContainerLib.h>
#include <Library/PciePm.h>
#include <IndustryStandard/UefiTcgPlatform.h>
#include <Library/TpmLib.h>

#define CPU_PCIE_DT_HALO_MAX_ROOT_PORT     3
#define CPU_PCIE_ULT_ULX_MAX_ROOT_PORT     3
#define MAX_TCSS_USB3_PORTS                4
#define TURBO_RATIO_LIMIT_ARRAY_SIZE       8

/**
  Return CPU Family ID

  @retval CPU_FAMILY              CPU Family ID
**/
CPU_FAMILY
EFIAPI
GetCpuFamily1 (
  VOID
  )
{
  CPUID_VERSION_INFO_EAX  Eax;
  ///
  /// Read the CPUID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Eax.Uint32, NULL, NULL, NULL);
  return (Eax.Uint32 & CPUID_FULL_FAMILY_MODEL);
}

/**
  Get Maximum CPU Pcie Root Port Number

  @retval Maximum CPU Pcie Root Port Number
**/
UINT8
GetMaxCpuPciePortNum (
  VOID
  )
{
  switch (GetCpuFamily1()) {
    case CPUID_FULL_FAMILY_MODEL_ALDERLAKE_DT_HALO:
      return CPU_PCIE_DT_HALO_MAX_ROOT_PORT;
    default:
      return CPU_PCIE_ULT_ULX_MAX_ROOT_PORT;
  }
}

/**
  Update FSP-S UPD config data.

  @param  FspsUpdPtr    The pointer to the FSP-S UPD to be updated.

**/
VOID
UpdateFspConfig (
  VOID     *FspsUpdPtr
  )
{
  FSPS_UPD              *FspsUpd;
  FSP_S_CONFIG          *FspsConfig;
  SILICON_CFG_DATA      *SiCfgData;
  FEATURES_CFG_DATA     *FeaturesCfgData;
  POWER_CFG_DATA        *PowerCfgData;
  UINT8                 DebugPort;
  UINTN                 Length;
  UINT8                 Index;
  UINT8                 MaxPcieRootPorts;
  UINT8                 PrIndex;
  UINT32                SmiEn;
  UINT32                BaseAddress;
  UINT32                TotalSize;
  UINT32                Address;
  UINT32                VarSize;
  BOOLEAN               BiosProtected;
  EFI_STATUS            Status;

  Address              = 0;
  FspsUpd              = (FSPS_UPD *) FspsUpdPtr;
  FspsConfig           = &FspsUpd->FspsConfig;

  FspsUpd->FspUpdHeader.Signature = FSPS_UPD_SIGNATURE;
  FspsUpd->FspUpdHeader.Revision  = 0x2;

  FspsUpd->FspsArchUpd.EnableMultiPhaseSiliconInit = 0;

  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);

  if (GetBootMode () != BOOT_ON_FLASH_UPDATE) {
    BiosProtected = FALSE;
    PrIndex = 0;
    Status = SpiGetRegionAddress (FlashRegionBios, &BaseAddress, &TotalSize);
    if (!EFI_ERROR (Status)) {
      BiosProtected = TRUE;
      Status = GetComponentInfo (FLASH_MAP_SIG_UEFIVARIABLE, &Address, &VarSize);
      if (!EFI_ERROR (Status)) {
        //
        // Protect the BIOS region except for the UEFI variable region
        //
        Address -= ((UINT32)(~TotalSize) + 1);

        FspsConfig->PchWriteProtectionEnable[PrIndex] = TRUE;
        FspsConfig->PchReadProtectionEnable[PrIndex]  = FALSE;
        FspsConfig->PchProtectedRangeBase[PrIndex]    = (UINT16) (BaseAddress >> 12);
        FspsConfig->PchProtectedRangeLimit[PrIndex]   = (UINT16) ((BaseAddress + Address - 1) >> 12);
        PrIndex++;

        FspsConfig->PchWriteProtectionEnable[PrIndex] = TRUE;
        FspsConfig->PchReadProtectionEnable[PrIndex]  = FALSE;
        FspsConfig->PchProtectedRangeBase[PrIndex]    = (UINT16) ((BaseAddress + Address + VarSize) >> 12);
        FspsConfig->PchProtectedRangeLimit[PrIndex]   = (UINT16) ((BaseAddress + TotalSize - 1) >> 12);
        PrIndex++;
      } else {
        //
        // Protect the whole BIOS region
        //
        FspsConfig->PchWriteProtectionEnable[PrIndex] = TRUE;
        FspsConfig->PchReadProtectionEnable[PrIndex]  = FALSE;
        FspsConfig->PchProtectedRangeBase[PrIndex]    = (UINT16) (BaseAddress >> 12);
        FspsConfig->PchProtectedRangeLimit[PrIndex]   = (UINT16) ((BaseAddress + TotalSize - 1) >> 12);
        PrIndex++;
      }
    }
    DEBUG (((BiosProtected) ? DEBUG_INFO : DEBUG_WARN, "BIOS SPI region will %a protected\n", (BiosProtected) ? "be" : "NOT BE"));
  }

  DEBUG ((DEBUG_INFO, "Frame Buffer Disabled\n"));
  FspsConfig->GraphicsConfigPtr = 0;
  FspsConfig->PeiGraphicsPeimInit = 0;
  FspsConfig->LidStatus = 0x0;

  SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if (SiCfgData == NULL) {
    DEBUG ((DEBUG_INFO, "Failed to find Silicon Cfg Data\n"));
  } else {
    FspsUpd->FspsConfig.PchDmiAspmCtrl                   = SiCfgData->PchDmiAspmCtrl;// ASPM configuration on the PCH side of the DMI/OPI Link
    FspsUpd->FspsConfig.PchLegacyIoLowLatency            = SiCfgData->PchLegacyIoLowLatency;
    FspsUpd->FspsConfig.RenderStandby                    = SiCfgData->RenderStandby; // IGFX RenderStandby
    FspsUpd->FspsConfig.PmSupport                        = SiCfgData->PmSupport;     // IGFX PmSupport
    MaxPcieRootPorts = GetMaxCpuPciePortNum ();
    for (Index = 0; Index < MaxPcieRootPorts; Index++) {
      FspsUpd->FspsConfig.CpuPcieClockGating[Index]      = SiCfgData->CpuPcieClockGating;   // Describes whether PCI Express Clock Gating for each root port is enabled by platform modules
      FspsUpd->FspsConfig.CpuPcieRpAspm[Index]           = SiCfgData->CpuPcieRpAspm[Index];        // Cpu Aspm
      FspsUpd->FspsConfig.CpuPcieRpL1Substates[Index]    = SiCfgData->CpuPcieRpL1Substates[Index]; // Cpu L1 substates
      FspsUpd->FspsConfig.CpuPcieRpPtmEnabled[Index]     = SiCfgData->CpuPcieRpPtmEnabled[Index] ;
      FspsUpd->FspsConfig.CpuPcieRpMultiVcEnabled[Index] = SiCfgData->CpuPcieRpMultiVcEnabled[Index];
    }

    // USB
    CopyMem (&FspsConfig->PortUsb20Enable,    SiCfgData->PortUsb20Enable,    sizeof(SiCfgData->PortUsb20Enable));
    CopyMem (&FspsConfig->Usb2OverCurrentPin, SiCfgData->Usb2OverCurrentPin, sizeof(SiCfgData->Usb2OverCurrentPin));
    CopyMem (&FspsConfig->Usb3OverCurrentPin, SiCfgData->Usb3OverCurrentPin, sizeof(SiCfgData->Usb3OverCurrentPin));
    CopyMem (&FspsConfig->PortUsb30Enable,    SiCfgData->PortUsb30Enable,    sizeof(SiCfgData->PortUsb30Enable));
    for (Index = 0; Index < GetPchUsb2MaxPhysicalPortNum (); Index++) {
      CopyMem (&FspsConfig->Usb2PhyPetxiset,  SiCfgData->Usb2PhyPetxiset,  sizeof(SiCfgData->Usb2PhyPetxiset));
      CopyMem (&FspsConfig->Usb2PhyTxiset,    SiCfgData->Usb2PhyTxiset,    sizeof(SiCfgData->Usb2PhyTxiset));
      CopyMem (&FspsConfig->Usb2PhyPredeemp,  SiCfgData->Usb2PhyPredeemp,  sizeof(SiCfgData->Usb2PhyPredeemp));
      CopyMem (&FspsConfig->Usb2PhyPehalfbit, SiCfgData->Usb2PhyPehalfbit, sizeof(SiCfgData->Usb2PhyPehalfbit));
    }

    //PCH Serial UART
    Length = GetPchMaxSerialIoUartControllersNum ();
    for (Index = 0; Index < Length ; Index++) {
      FspsConfig->SerialIoUartMode[Index]        = SiCfgData->SerialIoUartMode[Index];
      FspsConfig->SerialIoUartPowerGating[Index] = SiCfgData->SerialIoUartPowerGating[Index];
      FspsConfig->SerialIoUartDmaEnable[Index]   = SiCfgData->SerialIoUartDmaEnable[Index];
      FspsConfig->SerialIoUartAutoFlow[Index]    = SiCfgData->SerialIoUartAutoFlow[Index];
    }
    DebugPort = GetDebugPort ();
    if (DebugPort < Length) {
      // Inform FSP to skip debug UART init
      FspsConfig->SerialIoDebugUartNumber = DebugPort;
      FspsConfig->SerialIoUartMode[DebugPort] = 0x4;
    }

    // SPI
    FspsConfig->SerialIoSpiMode[1] = 0x1;
    CopyMem (&FspsConfig->SerialIoSpiCsPolarity, SiCfgData->SerialIoSpiCsPolarity, sizeof(SiCfgData->SerialIoSpiCsPolarity));

    // I2C
    Length = GetPchMaxSerialIoI2cControllersNum ();
    for (Index = 0; Index < Length ; Index++) {
      FspsConfig->SerialIoI2cMode[Index]               = SiCfgData->SerialIoI2cMode[Index];
      FspsConfig->PchSerialIoI2cPadsTermination[Index] = SiCfgData->PchSerialIoI2cPadsTermination[Index];
    }

    for (Index = 0; Index < MAX_TCSS_USB3_PORTS; Index++) {
      FspsConfig->CpuUsb3OverCurrentPin[Index] = SiCfgData->CpuUsb3OverCurrentPin[Index];
    }

    // ISH
    CopyMem (&FspsConfig->PchIshI2cEnable, SiCfgData->PchIshI2cEnable, sizeof(SiCfgData->PchIshI2cEnable));
    CopyMem (&FspsConfig->PchIshGpEnable, SiCfgData->PchIshGpEnable, sizeof(SiCfgData->PchIshGpEnable));

    // Pcie Root Port
    MaxPcieRootPorts = GetPchMaxPciePortNum ();
    for (Index = 0; Index < MaxPcieRootPorts; Index++) {
      FspsConfig->PcieRpMaxPayload[Index]             = SiCfgData->PcieRpMaxPayload[Index];
      FspsConfig->PcieRpL1Substates[Index]            = SiCfgData->PcieRpL1Substates[Index];
      FspsConfig->PcieRpLtrEnable[Index]              = SiCfgData->PcieRpLtrEnable[Index];
      FspsConfig->PcieRpClkReqDetect[Index]           = SiCfgData->PcieRpClkReqDetect[Index];
      FspsConfig->PcieRpAdvancedErrorReporting[Index] = SiCfgData->PcieRpAdvancedErrorReporting[Index];
      FspsConfig->PcieRpAspm[Index]                   = SiCfgData->PcieRpAspm[Index];
      FspsConfig->PcieRpLtrMaxSnoopLatency[Index]     = 0x1003;
      FspsConfig->PcieRpLtrMaxNoSnoopLatency[Index]   = 0x1003;
      FspsConfig->PciePtm[Index]                      = SiCfgData->PciePtm[Index];
      FspsConfig->PcieRpPcieSpeed[Index]              = SiCfgData->PcieRpPcieSpeed[Index];
    }

    for (Index = 0; Index < 4; Index++) {
      FspsConfig->PtmEnabled[Index]   = SiCfgData->PtmEnabled[Index];
    }

    FspsConfig->PchPmSlpAMinAssert    = SiCfgData->PchPmSlpAMinAssert;
    FspsConfig->EnableTcoTimer        = 0x1;

    // UFS
    if (IsPchLp ()) {
      FspsConfig->UfsEnable[0]        = SiCfgData->PchUfsEnable[0];
      FspsConfig->UfsEnable[1]        = SiCfgData->PchUfsEnable[1];
    }

    // When fast boot is enabled, program SLP_A_MIN_ASST_WDTH to 0
    // to reduce boottime.
    if (PcdGetBool (PcdFastBootEnabled)) {
      FspsConfig->PchPmSlpAMinAssert = 1;
    }

    FspsConfig->IehMode               = SiCfgData->IehMode;
    FspsConfig->D3HotEnable           = SiCfgData->D3HotEnable;
    FspsConfig->D3ColdEnable          = SiCfgData->D3ColdEnable;
    FspsConfig->TcCstateLimit         = SiCfgData->TcCstateLimit;
    FspsConfig->VccSt                 = SiCfgData->VccSt;
    // Cpu Pcie
    FspsConfig->CpuPciePowerGating[0] = SiCfgData->CpuPciePowerGating;
    FspsConfig->CpuPciePowerGating[1] = SiCfgData->CpuPciePowerGating;
    FspsConfig->CpuPciePowerGating[2] = SiCfgData->CpuPciePowerGating;
    FspsConfig->CpuPciePowerGating[3] = SiCfgData->CpuPciePowerGating;
  }

  for (Index = 0; Index < 8; Index++) {
    FspsConfig->PcieRpSnoopLatencyOverrideMode[Index]    = 0x2;
    FspsConfig->PcieRpNonSnoopLatencyOverrideMode[Index] = 0x2;
  }

  if(GetPayloadId () == 0) {
    // Disable SMI sources
    SmiEn = IoRead32((UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
    IoWrite32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN), SmiEn & (UINT32) (~B_ACPI_IO_SMI_EN_GBL_SMI));

    // Lock down SMI
    FspsConfig->PchLockDownGlobalSmi = 0x1;
  }

  //Disable VMD by default
  FspsConfig->VmdEnable = 0;

  // Cpu Pcie
  FspsConfig->CpuPcieRpFunctionSwap        = 0x0;
  for (Index = 0; Index < 3; Index++) {
    FspsConfig->CpuPcieRpMaxPayload[Index] = 0x1;
    FspsConfig->CpuPcieRpLtrEnable[Index]  = 0x1;
    FspsConfig->CpuPcieRpPmSci[Index]      = 0x1;
  }

  // PCH Flash protection
  FspsConfig->PchPwrOptEnable              = 0x1;

  // MISC
  FspsConfig->PchFivrExtV1p05RailVoltage                 = 0x1a4;
  FspsConfig->PchFivrExtVnnRailVoltage                   = 0x1a4;
  FspsConfig->PchFivrExtVnnRailSxVoltage                 = 0x1a4;
  FspsConfig->PchFivrVccinAuxLowToHighCurModeVolTranTime = 0xc;
  FspsConfig->PchFivrVccinAuxRetToHighCurModeVolTranTime = 0x36;
  FspsConfig->PchFivrVccinAuxRetToLowCurModeVolTranTime  = 0x2b;
  FspsConfig->PchFivrVccinAuxOffToHighCurModeVolTranTime = 0x96;
  FspsConfig->PchFivrDynPm                               = 0x1;
  FspsConfig->PchFivrExtV1p05RailIccMaximum              = 0x1f4;
  FspsConfig->PchFivrExtVnnRailIccMaximum                = 0x1f4;
  FspsConfig->PchFivrExtVnnRailSxIccMaximum              = 0x1f4;
  FspsConfig->PchMemoryPmsyncEnable[0]                   = 0x1;
  FspsConfig->PchMemoryPmsyncEnable[1]                   = 0x1;
  FspsConfig->PchMemoryC0TransmitEnable[0]               = 0x1;
  FspsConfig->PchMemoryC0TransmitEnable[1]               = 0x1;
  FspsConfig->PchPmSlpS3MinAssert                        = 0x3;
  FspsConfig->PchPmSlpS4MinAssert                        = 0x1;
  FspsConfig->PchPmSlpSusMinAssert                       = 0x4;
  FspsConfig->PchPmPwrBtnOverridePeriod                  = 0x0;
  FspsConfig->PchEspiLgmrEnable                          = 0x0;
  FspsConfig->ForcMebxSyncUp                             = 0x0;
  FspsConfig->PmcModPhySusPgEnable                       = 0x0;
  FspsConfig->IomTypeCPortPadCfg[2]                      = 0x6040000;
  FspsConfig->IomTypeCPortPadCfg[3]                      = 0x6040011;
  FspsConfig->DmiTS0TW                                   = 0x3;
  FspsConfig->DmiTS1TW                                   = 0x2;
  FspsConfig->DmiTS2TW                                   = 0x1;
  FspsConfig->MeUnconfigOnRtcClear                       = 0x1;
  FspsConfig->PsOnEnable                                 = 0x1;
  FspsConfig->PmcAdrTimerEn                              = 0x1;
  FspsConfig->PmcAdrTimer1Val                            = 0x20;
  FspsConfig->PmcAdrMultiplier1Val                       = 0x6;
  FspsConfig->PmcAdrHostPartitionReset                   = 0x1;
  FspsConfig->PmcLpmS0ixSubStateEnableMask               = 0x3;
  FspsConfig->PmcUsb2PhySusPgEnable                      = 0x0;
  FspsConfig->SkipCdClockInit                            = 0x0;
  FspsConfig->VccInAuxImonIccImax                        = 0x0;
  FspsConfig->CpuPcieRpGen3Uptp[0]                       = 0x5;
  FspsConfig->CpuPcieRpGen4Uptp[0]                       = 0x8;
  FspsConfig->CpuPcieRpGen4Uptp[1]                       = 0x7;
  FspsConfig->CpuPcieRpGen4Uptp[2]                       = 0x7;
  FspsConfig->CpuPcieRpGen4Dptp[0]                       = 0x9;
  FspsConfig->CpuPcieRpGen4Dptp[1]                       = 0x5;
  FspsConfig->CpuPcieRpGen4Dptp[2]                       = 0x5;
  FspsConfig->CpuPcieRpGen5Uptp[0]                       = 0x5;
  FspsConfig->CpuPcieRpGen5Uptp[1]                       = 0x5;
  FspsConfig->CpuPcieRpGen5Uptp[2]                       = 0x5;

  // EndOfPost Upd
  FspsUpd->FspsConfig.EndOfPostMessage                   = 1;

  if (FeaturesCfgData != NULL) {
    if (FeaturesCfgData->Features.S0ix == 1) {
      FspsConfig->XdciEnable = 0;
      FspsConfig->EnableItbm = 0;

      ZeroMem (FspsConfig->CpuPcieRpMultiVcEnabled, sizeof (FspsConfig->CpuPcieRpMultiVcEnabled));
      ZeroMem (FspsConfig->PchIshI2cEnable,         sizeof (FspsConfig->PchIshI2cEnable));
      ZeroMem (FspsConfig->PchIshGpEnable,          sizeof (FspsConfig->PchIshGpEnable));
      DEBUG ((DEBUG_INFO, "Stage 2 S0ix config applied.\n"));
    }
  }

  for (Index = 0; Index < 3; Index++) {
    FspsConfig->CpuPcieRpLtrMaxSnoopLatency[Index]   = 0x100f;
    FspsConfig->CpuPcieRpLtrMaxNoSnoopLatency[Index] = 0x100f;
  }

  // Force Eiss and BiosLock off for now.
  // Enable it later in OS loader/EndofStages
  FspsConfig->PchLockDownBiosLock = 0x0;

  // Disable IEH
  FspsConfig->IehMode = 0x0;
  FspsConfig->PchCrid = 0x0;
  if (GetBootMode() == BOOT_ON_FLASH_UPDATE) {
    FspsUpd->FspsConfig.PchLockDownBiosInterface = FALSE;
    FspsUpd->FspsConfig.PchSbAccessUnlock        = TRUE;
    FspsUpd->FspsConfig.RtcBiosInterfaceLock     = FALSE;
    DEBUG ((DEBUG_INFO, "Firmware update mode, unlock Bios setting\n"));
  } else {
    FspsUpd->FspsConfig.PchLockDownBiosInterface = TRUE;
    FspsUpd->FspsConfig.PchSbAccessUnlock        = FALSE;
    FspsUpd->FspsConfig.RtcBiosInterfaceLock     = TRUE;
  }

  FspsConfig->AmtEnabled        = 0x0;
  FspsConfig->VccInAuxImonSlope = 0x6f;

  PowerCfgData = (POWER_CFG_DATA *) FindConfigDataByTag (CDATA_POWER_TAG);
  if (PowerCfgData == NULL) {
    DEBUG ((DEBUG_ERROR, "Missing power Cfg Data!\n"));
  } else {
    //CPU Power Management Basic Config
    FspsConfig->Hwp                           = PowerCfgData->Hwp;
    FspsConfig->HdcControl                    = PowerCfgData->HdcControl;
    FspsConfig->EnableItbm                    = PowerCfgData->EnableItbm;
    FspsConfig->PowerLimit2                   = PowerCfgData->PowerLimit2;
    FspsConfig->TurboPowerLimitLock           = PowerCfgData->TurboPowerLimitLock;
    FspsConfig->PowerLimit3DutyCycle          = PowerCfgData->PowerLimit3DutyCycle;
    FspsConfig->PowerLimit3Lock               = PowerCfgData->PowerLimit3Lock;
    FspsConfig->PowerLimit4Lock               = PowerCfgData->PowerLimit4Lock;
    FspsConfig->TccOffsetClamp                = PowerCfgData->TccOffsetClamp;
    FspsConfig->TccOffsetLock                 = PowerCfgData->TccOffsetLock;
    FspsConfig->TurboMode                     = PowerCfgData->TurboMode;
    FspsConfig->TccActivationOffset           = PowerCfgData->TccActivationOffset;
    FspsConfig->PowerLimit1                   = PowerCfgData->PowerLimit1;
    FspsConfig->PowerLimit2Power              = PowerCfgData->PowerLimit2Power;
    FspsConfig->PowerLimit3                   = PowerCfgData->PowerLimit3;
    FspsConfig->PowerLimit4                   = PowerCfgData->PowerLimit4;
    FspsConfig->PowerLimit1Time               = PowerCfgData->PowerLimit1Time;
    FspsConfig->PowerLimit3Time               = PowerCfgData->PowerLimit3Time;
    FspsConfig->TccOffsetTimeWindowForRatl    = PowerCfgData->TccOffsetTimeWindowForRatl;
    FspsConfig->HwpInterruptControl           = PowerCfgData->HwpInterruptControl;
    FspsConfig->EnablePerCorePState           = PowerCfgData->EnablePerCorePState;
    FspsConfig->EnableHwpAutoPerCorePstate    = PowerCfgData->EnableHwpAutoPerCorePstate;
    FspsConfig->EnableHwpAutoEppGrouping      = PowerCfgData->EnableHwpAutoEppGrouping;
    FspsConfig->EnableEpbPeciOverride         = PowerCfgData->EnableEpbPeciOverride;
    FspsConfig->EnableFastMsrHwpReq           = PowerCfgData->EnableFastMsrHwpReq;
    FspsConfig->MinRingRatioLimit             = PowerCfgData->MinRingRatioLimit;
    FspsConfig->MaxRingRatioLimit             = PowerCfgData->MaxRingRatioLimit;

     //CPU VR Power Management Custom Config
    CopyMem (FspsConfig->VrConfigEnable,      PowerCfgData->VrConfigEnable,        sizeof(PowerCfgData->VrConfigEnable));
    CopyMem (FspsConfig->TdcCurrentLimit,     PowerCfgData->TdcCurrentLimit,       sizeof(PowerCfgData->TdcCurrentLimit));
    CopyMem (FspsConfig->DcLoadline,          PowerCfgData->DcLoadline,            sizeof(PowerCfgData->DcLoadline));
    CopyMem (FspsConfig->AcLoadline,          PowerCfgData->AcLoadline,            sizeof(PowerCfgData->AcLoadline));
    CopyMem (FspsConfig->Psi1Threshold,       PowerCfgData->Psi1Threshold,         sizeof(PowerCfgData->Psi1Threshold));
    CopyMem (FspsConfig->Psi2Threshold,       PowerCfgData->Psi2Threshold,         sizeof(PowerCfgData->Psi2Threshold));
    CopyMem (FspsConfig->Psi3Threshold,       PowerCfgData->Psi3Threshold,         sizeof(PowerCfgData->Psi3Threshold));
    CopyMem (FspsConfig->Psi3Enable,          PowerCfgData->Psi3Enable,            sizeof(PowerCfgData->Psi3Enable));
    CopyMem (FspsConfig->Psi4Enable,          PowerCfgData->Psi4Enable,            sizeof(PowerCfgData->Psi4Enable));
    CopyMem (FspsConfig->ImonSlope,           PowerCfgData->ImonSlope,             sizeof(PowerCfgData->ImonSlope));
    CopyMem (FspsConfig->IccMax,              PowerCfgData->IccMax,                sizeof(PowerCfgData->IccMax));
    CopyMem (FspsConfig->ImonOffset,          PowerCfgData->ImoniOffset,           sizeof(PowerCfgData->ImoniOffset));

    CopyMem (FspsConfig->VrVoltageLimit,      PowerCfgData->VrVoltageLimit,        sizeof(PowerCfgData->VrVoltageLimit));
    CopyMem (FspsConfig->TdcEnable,           PowerCfgData->TdcEnable,             sizeof(PowerCfgData->TdcEnable));
    CopyMem (FspsConfig->TdcTimeWindow,       PowerCfgData->TdcTimeWindow,         sizeof(PowerCfgData->TdcTimeWindow));

    FspsConfig->AcousticNoiseMitigation       = 0x0;
    FspsConfig->FivrSpreadSpectrum            = 0x8;

    //CPU Power Management Custom Config
    FspsConfig->MaxRatio                      = PowerCfgData->MaxRatio;
    FspsConfig->Custom1PowerLimit1            = PowerCfgData->Custom1PowerLimit1;
    FspsConfig->Custom1PowerLimit2            = PowerCfgData->Custom1PowerLimit2;
    FspsConfig->Custom1PowerLimit1Time        = PowerCfgData->Custom1PowerLimit1Time;
    FspsConfig->Custom1TurboActivationRatio   = PowerCfgData->Custom1TurboActivationRatio;
    FspsConfig->Custom2PowerLimit1            = PowerCfgData->Custom2PowerLimit1;
    FspsConfig->Custom2PowerLimit2            = PowerCfgData->Custom2PowerLimit2;
    FspsConfig->Custom2PowerLimit1Time        = PowerCfgData->Custom2PowerLimit1Time;
    FspsConfig->Custom2TurboActivationRatio   = PowerCfgData->Custom2TurboActivationRatio;
    FspsConfig->Custom3PowerLimit1            = PowerCfgData->Custom3PowerLimit1;
    FspsConfig->Custom3PowerLimit2            = PowerCfgData->Custom3PowerLimit2;
    FspsConfig->Custom3PowerLimit1Time        = PowerCfgData->Custom3PowerLimit1Time;
    FspsConfig->Custom3TurboActivationRatio   = PowerCfgData->Custom3TurboActivationRatio;

    //CPU Power Management Psys Config
    FspsConfig->PsysPowerLimit1Power          = PowerCfgData->PsysPowerLimit1Power;
    FspsConfig->PsysPowerLimit2Power          = PowerCfgData->PsysPowerLimit2Power;
    FspsConfig->PsysPmax                      = PowerCfgData->PsysPmax;
    FspsConfig->PsysPowerLimit1               = PowerCfgData->PsysPowerLimit1;
    FspsConfig->PsysPowerLimit1Time           = PowerCfgData->PsysPowerLimit1Time;
    FspsConfig->PsysPowerLimit2               = PowerCfgData->PsysPowerLimit2;

    //CPU Power Management Test Config
    FspsConfig->Eist                          = PowerCfgData->Eist;
    FspsConfig->EnergyEfficientPState         = PowerCfgData->EnergyEfficientPState;
    FspsConfig->EnergyEfficientTurbo          = PowerCfgData->EnergyEfficientTurbo;
    FspsConfig->TStates                       = PowerCfgData->TStates;
    FspsConfig->BiProcHot                     = PowerCfgData->BiProcHot;
    FspsConfig->DisableProcHotOut             = PowerCfgData->DisableProcHotOut;
    FspsConfig->ProcHotResponse               = PowerCfgData->ProcHotResponse;
    FspsConfig->DisableVrThermalAlert         = PowerCfgData->DisableVrThermalAlert;
    FspsConfig->EnableAllThermalFunctions     = PowerCfgData->EnableAllThermalFunctions;
    FspsConfig->ThermalMonitor                = PowerCfgData->ThermalMonitor;
    FspsConfig->Cx                            = PowerCfgData->Cx;
    FspsConfig->PmgCstCfgCtrlLock             = PowerCfgData->PmgCstCfgCtrlLock;
    FspsConfig->C1e                           = PowerCfgData->C1e;
    FspsConfig->C1StateAutoDemotion           = PowerCfgData->C1StateAutoDemotion;
    FspsConfig->C1StateUnDemotion             = PowerCfgData->C1StateUnDemotion;
    FspsConfig->CStatePreWake                 = PowerCfgData->CStatePreWake;
    FspsConfig->TimedMwait                    = PowerCfgData->TimedMwait;
    FspsConfig->CstCfgCtrIoMwaitRedirection   = PowerCfgData->CstCfgCtrIoMwaitRedirection;
    FspsConfig->ProcHotLock                   = PowerCfgData->ProcHotLock;
    FspsConfig->ConfigTdpLevel                = PowerCfgData->ConfigTdpLevel;
    FspsConfig->RaceToHalt                    = PowerCfgData->RaceToHalt;
    FspsConfig->CstateLatencyControl1TimeUnit = PowerCfgData->CstateLatencyControl1TimeUnit;
    FspsConfig->CstateLatencyControl2TimeUnit = PowerCfgData->CstateLatencyControl2TimeUnit;
    FspsConfig->CstateLatencyControl3TimeUnit = PowerCfgData->CstateLatencyControl3TimeUnit;
    FspsConfig->CstateLatencyControl4TimeUnit = PowerCfgData->CstateLatencyControl4TimeUnit;
    FspsConfig->CstateLatencyControl5TimeUnit = PowerCfgData->CstateLatencyControl5TimeUnit;
    FspsConfig->PkgCStateLimit                = PowerCfgData->PkgCStateLimit;
    FspsConfig->CstateLatencyControl1Irtl     = PowerCfgData->CstateLatencyControl1Irtl;
    FspsConfig->CstateLatencyControl2Irtl     = PowerCfgData->CstateLatencyControl2Irtl;
    FspsConfig->CstateLatencyControl3Irtl     = PowerCfgData->CstateLatencyControl3Irtl;
    FspsConfig->CstateLatencyControl4Irtl     = PowerCfgData->CstateLatencyControl4Irtl;
    FspsConfig->CstateLatencyControl5Irtl     = PowerCfgData->CstateLatencyControl5Irtl;

    // Cpu power related settings
    for (Index = 0; Index < TURBO_RATIO_LIMIT_ARRAY_SIZE; Index++) {
      FspsConfig->TurboRatioLimitRatio[Index]       = PowerCfgData->TurboRatioLimitRatio[Index];
      FspsConfig->TurboRatioLimitNumCore[Index]     = PowerCfgData->TurboRatioLimitNumCore[Index];
      FspsConfig->AtomTurboRatioLimitRatio[Index]   = PowerCfgData->AtomTurboRatioLimitRatio[Index];
      FspsConfig->AtomTurboRatioLimitNumCore[Index] = PowerCfgData->AtomTurboRatioLimitNumCore[Index];
    }
  }

  FspsConfig->TdcTimeWindow[0] = 0x3e8;
  FspsConfig->TdcTimeWindow[1] = 0x3e8;
  FspsConfig->Device4Enable    = 0x1;

  if (IsPchLp ()) {
    for (Index = 0; Index < 4; Index++) {
      FspsConfig->CpuPcieRpLtrMaxSnoopLatency[Index]       = 0x100f;
      FspsConfig->CpuPcieRpLtrMaxNoSnoopLatency[Index]     = 0x100f;
    }

    FspsConfig->SataPortDevSlpPinMux[0]                    = 0x59673e0c;
    FspsConfig->SataPortDevSlpPinMux[1]                    = 0x5967400d;
    FspsConfig->PchSerialIoI2cSdaPinMux[0]                 = 0x1947c404;
    FspsConfig->PchSerialIoI2cSdaPinMux[1]                 = 0x1947c606;
    FspsConfig->PchSerialIoI2cSdaPinMux[4]                 = 0;
    FspsConfig->PchSerialIoI2cSdaPinMux[7]                 = 0x1947d20c;
    FspsConfig->PchSerialIoI2cSclPinMux[0]                 = 0x1947a405;
    FspsConfig->PchSerialIoI2cSclPinMux[1]                 = 0x1947a607;
    FspsConfig->PchSerialIoI2cSclPinMux[4]                 = 0;
    FspsConfig->PchSerialIoI2cSclPinMux[7]                 = 0x1947b20d;
    FspsConfig->IshGpGpioPinMuxing[4]                      = 0x290ea809;
    FspsConfig->IshGpGpioPinMuxing[5]                      = 0x4900aa04;
    FspsConfig->IshGpGpioPinMuxing[6]                      = 0x4907ac0c;
    FspsConfig->IshGpGpioPinMuxing[7]                      = 0x5900ae0f;
    FspsConfig->PchFivrExtV1p05RailSupportedVoltageStates  = 0x2;
    FspsConfig->PchFivrExtV1p05RailVoltage                 = 0x0;
    FspsConfig->PchFivrExtVnnRailVoltage                   = 0x0;
    FspsConfig->PchFivrExtVnnRailSxVoltage                 = 0x0;
    FspsConfig->PchFivrVccinAuxLowToHighCurModeVolTranTime = 0xc;
    FspsConfig->PchFivrVccinAuxRetToHighCurModeVolTranTime = 0x36;
    FspsConfig->PchFivrVccinAuxRetToLowCurModeVolTranTime  = 0x2b;
    FspsConfig->PchFivrVccinAuxOffToHighCurModeVolTranTime = 0x96;
    FspsConfig->PchFivrExtVnnRailSxIccMaximum              = 0x0;
    FspsConfig->CnviRfResetPinMux                          = 0x194ce404;
    FspsConfig->CnviClkreqPinMux                           = 0x294ce605;
    FspsConfig->PmcUsb2PhySusPgEnable                      = 0x1;
    FspsConfig->PmcModPhySusPgEnable                       = 0x1;
    FspsConfig->IomTypeCPortPadCfg[2]                      = 0x0;
    FspsConfig->IomTypeCPortPadCfg[3]                      = 0x0;
    FspsConfig->ITbtPcieTunnelingForUsb4                   = 0x1;
    FspsConfig->ITbtPcieRootPortEn[0]                      = 0x1;
    FspsConfig->ITbtPcieRootPortEn[1]                      = 0x1;
    FspsConfig->ITbtPcieRootPortEn[2]                      = 0x1;
    FspsConfig->ITbtPcieRootPortEn[3]                      = 0x1;
    FspsConfig->SaPcieItbtRpSnoopLatencyOverrideValue[2]   = 0xc8;
    FspsConfig->TdcTimeWindow[0]                           = 0x3e8;
    FspsConfig->TdcTimeWindow[1]                           = 0x3e8;
    FspsConfig->IehMode                                    = 0x0;
    FspsConfig->PortResetMessageEnable[0]                  = 0x1;
    FspsConfig->PortResetMessageEnable[1]                  = 0x1;
    FspsConfig->PortResetMessageEnable[2]                  = 0x1;
    FspsConfig->PortResetMessageEnable[4]                  = 0x1;
    FspsConfig->PkgCStateLimit                             = 0xff;
    FspsConfig->CstCfgCtrIoMwaitRedirection                = 0;
    FspsConfig->PmcLpmS0ixSubStateEnableMask               = 0x9;
    FspsConfig->VbtSize                                    = 0x2200;
    FspsConfig->CpuPcieRpGen3Uptp[2]                       = 0x5;
    FspsConfig->CpuPcieRpGen4Uptp[2]                       = 0x8;
    FspsConfig->CpuPcieRpGen4Dptp[2]                       = 0x9;
    FspsConfig->PchPmPwrBtnOverridePeriod                  = 0x3;
    FspsConfig->PsOnEnable                                 = 0x0;
    FspsConfig->IomStayInTCColdSeconds                     = 0x0;
    FspsConfig->IomBeforeEnteringTCColdSeconds             = 0x0;
    FspsConfig->Irms[0]                                    = 0x0;
    FspsConfig->AtomTurboRatioLimitNumCore[1]              = 0x2;
    FspsConfig->AtomTurboRatioLimitNumCore[2]              = 0x3;
    FspsConfig->AtomTurboRatioLimitNumCore[3]              = 0x4;
    FspsConfig->AtomTurboRatioLimitNumCore[4]              = 0x5;
    FspsConfig->AtomTurboRatioLimitNumCore[5]              = 0x6;
    FspsConfig->AtomTurboRatioLimitNumCore[6]              = 0x7;
    FspsConfig->AtomTurboRatioLimitNumCore[7]              = 0x8;

    switch (GetPlatformId ()) {
      case PLATFORM_ID_AZB_LP5_CRB2A:
        FspsConfig->PcieRpDetectTimeoutMs[1]               = 0x12c;
        //
        // Maximum Tcss port support is 2 and minimum is 1;
        // In case of CRB it is 1.
        //
        FspsConfig->NumberOfTcssPortsSupported             = 0x1;
        FspsConfig->EnableTcssCovTypeA[0]                  = 0x1;
        FspsConfig->MappingPchXhciUsbA[0]                  = 0x1;
        FspsConfig->PchLanEnable                           = 0x0;
        FspsConfig->PchPmVrAlert                           = 0x1;
        FspsConfig->PcieRpDetectTimeoutMs[3]               = 0x12c;
        FspsConfig->UsbTcPortEn                            = 0x3;
        FspsConfig->PchPmSlpSusMinAssert                   = 0x4;
        FspsConfig->PchPmSlpAMinAssert                     = 0x4;
        FspsConfig->UfsEnable[1]                           = 0x1;
        FspsConfig->SendEcCmd                              = 0x42335518;
        FspsConfig->EcCmdProvisionEav                      = 0xb1;
        FspsConfig->EcCmdLock                              = 0xb2;
        FspsConfig->PortResetMessageEnable[2]              = 0x0;
        FspsConfig->PortResetMessageEnable[4]              = 0x0;
        FspsConfig->CpuPcieRpPmSci[1]                      = 0x0;
        FspsConfig->CpuPcieRpPmSci[2]                      = 0x0;
        FspsConfig->CpuPcieRpMaxPayload[1]                 = 0x0;
        FspsConfig->CpuPcieRpMaxPayload[2]                 = 0x0;
        FspsConfig->CpuPcieRpAspm[1]                       = 0x3;
        FspsConfig->CpuPcieRpAspm[2]                       = 0x3;
        FspsConfig->CpuPcieRpLtrEnable[1]                  = 0x0;
        FspsConfig->CpuPcieRpLtrEnable[2]                  = 0x0;
        FspsConfig->PkgCStateLimit                         = 0x0;
        FspsConfig->CpuPcieRpGen3Uptp[2]                   = 0x7;
        FspsConfig->CpuPcieRpGen4Uptp[1]                   = 0x8;
        FspsConfig->CpuPcieRpGen4Dptp[1]                   = 0x9;
        FspsConfig->CpuPcieRpGen5Uptp[1]                   = 0x7;
        FspsConfig->CpuPcieRpGen5Uptp[2]                   = 0x7;
        break;
      default:
        break;
    }
  }

  if (FeaturePcdGet (PcdEnablePciePm)) {
    StoreRpConfig (FspsConfig);
  }
}
