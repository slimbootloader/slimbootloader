/** @file

  Copyright (c) 2017 - 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/DebugLib.h>
#include <GpioConfig.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MpInitLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PciLib.h>
#include <Library/SocInitLib.h>
#include <CpuDataStruct.h>
#include <CpuInitDataHob.h>
#include <CpuPowerMgmt.h>
#include <GlobalNvsAreaDef.h>
#include <PowerMgmtNvsStruct.h>
#include <PlatformData.h>
#include <Register/Cpuid.h>
#include <Register/CpuInit.h>
#include <Register/CpuRegs.h>
#include <Register/GpioPinsVer3.h>
#include <Register/PseRegs.h>
#include <Register/SaRegsHostBridge.h>
#include <FspsUpd.h>
#include <Library/PchSciLib.h>
#include <Library/ConfigDataLib.h>
#include <ConfigDataDefs.h>
#include <TccConfigSubRegions.h>
#include <Library/GpioLib.h>
#include <Library/GpioSiLib.h>
#include <PseConfig.h>

#define XTAL_FREQ_24MHZ      0
#define XTAL_FREQ_38P4MHZ    1
#define PMECTRLSTATUS_OFFSET 0x84
#define POWERSTATE           (BIT0 | BIT1)

extern FVID_TABLE  *mFvidPointer;

typedef union {
  struct {
    UINT32  Low;
    UINT32  High;
  } Data32;
  UINTN Data;
} UINTN_STRUCT;

/**
  Check and update TSN controller device status

  @param[in]  GbeBase          GBE PCI config space address
**/
VOID
CheckGbeStatus (
  UINTN GbeBase
  )
{
  UINT16 PmeCtrlStatus;
  PmeCtrlStatus = PciRead16 (GbeBase + PMECTRLSTATUS_OFFSET);
  //Check device status if not D0 then update it to D0
  if ((PmeCtrlStatus & POWERSTATE) != 0x0) {
    PciWrite16 ((GbeBase + PMECTRLSTATUS_OFFSET), (PmeCtrlStatus & ~POWERSTATE));
    DEBUG ((DEBUG_INFO, "GbeBase:0x%x -> PMECTRLSTATUS:0x%x\n", GbeBase, PmeCtrlStatus));
  }
}

/**
  Check whether TSN controller is enabled in the platform.

  @retval TRUE                    TSN is enabled
  @retval FALSE                   TSN is disabled
**/
BOOLEAN
IsTsnPresent (
  VOID
  )
{
  UINTN  PchTsnBase;
  UINT16 DeviceId;
  UINT16 VendorId;

  PchTsnBase = 0;
  DeviceId   = 0;
  VendorId   = 0;

  PchTsnBase       = PCI_LIB_ADDRESS (0, 30, 4, 0);
  DeviceId         = PciRead16 (PchTsnBase + PCI_DEVICE_ID_OFFSET);
  VendorId         = PciRead16 (PchTsnBase + PCI_VENDOR_ID_OFFSET);

  DEBUG ((DEBUG_INFO, "TSN Device: Device Id: 0x%4X, Vendor Id: 0x%4X\n", DeviceId, VendorId));
  if ((DeviceId == 0xFFFF) || (VendorId == 0xFFFF)) {
    return FALSE;
  }
  else{
    //Check device status
    CheckGbeStatus (PchTsnBase);
  }
  return TRUE;
}

/**
  Check Pse Gbe0 Enabled.

  @retval                               TRUE/FALSE
**/
UINT8
IsPseGbe0Enabled (
  VOID
  )
{
  UINTN  PchPseGbeBase;
  UINT16 DeviceId;
  UINT16 VendorId;

  PchPseGbeBase = 0;
  DeviceId      = 0;
  VendorId      = 0;

  PchPseGbeBase    = PCI_LIB_ADDRESS (0, 29, 1, 0);
  DeviceId         = PciRead16 (PchPseGbeBase + PCI_DEVICE_ID_OFFSET);
  VendorId         = PciRead16 (PchPseGbeBase + PCI_VENDOR_ID_OFFSET);

  DEBUG ((DEBUG_INFO, "Pse Gbe 0 Device: Device Id: 0x%4X, Vendor Id: 0x%4X\n", DeviceId, VendorId));
  if ((DeviceId == 0xFFFF) || (VendorId == 0xFFFF)) {
    return FALSE;
  }
  else{
     //Check device status
     CheckGbeStatus (PchPseGbeBase);
  }
  return TRUE;
}

/**
  Check Pse Gbe1 Enabled.

  @retval                               TRUE/FALSE
**/
UINT8
IsPseGbe1Enabled (
  VOID
  )
{
  UINTN  PchPseGbeBase;
  UINT16 DeviceId;
  UINT16 VendorId;

  PchPseGbeBase = 0;
  DeviceId      = 0;
  VendorId      = 0;

  PchPseGbeBase    = PCI_LIB_ADDRESS (0, 29, 2, 0);
  DeviceId         = PciRead16 (PchPseGbeBase + PCI_DEVICE_ID_OFFSET);
  VendorId         = PciRead16 (PchPseGbeBase + PCI_VENDOR_ID_OFFSET);

  DEBUG ((DEBUG_INFO, "Pse Gbe 1 Device: Device Id: 0x%4X, Vendor Id: 0x%4X\n", DeviceId, VendorId));
  if ((DeviceId == 0xFFFF) || (VendorId == 0xFFFF)) {
    return FALSE;
  }
  else{
    //Check device status
    CheckGbeStatus (PchPseGbeBase);
  }
  return TRUE;
}

/**
  Return CPU Family ID

  @retval CPU_FAMILY              CPU Family ID
**/
CPU_FAMILY
EFIAPI
GetCpuFamily (
  VOID
  )
{
  EFI_CPUID_REGISTER Cpuid;
  ///
  /// Read the CPUID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  return ((CPU_FAMILY) (Cpuid.RegEax & CPUID_FULL_FAMILY_MODEL));
}

/**
  Return Cpu stepping type

  @retval UINT8                   Cpu stepping type
**/
CPU_STEPPING
EFIAPI
GetCpuStepping (
  VOID
  )
{
  EFI_CPUID_REGISTER Cpuid;
  ///
  /// Read the CPUID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  return ((CPU_STEPPING) (Cpuid.RegEax & CPUID_FULL_STEPPING));
}

/**
  Return SIO Uart Pci Cfg base

  @param[in] UartNumber          Uart index

  @retval UINT64             Pci Cfg base
**/
UINT64
EFIAPI
GetSerialIoUartPciCfgBase (
  IN UINT8       UartNumber
  )
{
  if (GetPchMaxSerialIoUartControllersNum () <= UartNumber) {
    ASSERT (FALSE);
    return 0;
  }
  switch (UartNumber) {
    case 0:
      return 0xF0000; // B0:D30:F0
    case 1:
      return 0xF1000; // B0:D30:F1
    case 2:
      return 0xCA000; // B0:D25:F2
    default:
      ASSERT (FALSE);
      return 0;
  }
}

/**
  Return SIO Uart Irq

  @param[in] UartNumber          Uart index

  @retval UINT8              irq
**/
UINT8
EFIAPI
GetSerialIoUartIrq (
  IN UINT8       UartNumber
  )
{
  if (GetPchMaxSerialIoUartControllersNum () <= UartNumber) {
    ASSERT (FALSE);
    return 0xFF;
  }
  switch (UartNumber) {
    case 0:
      return 16;
    case 1:
      return 17;
    case 2:
      return 33;
    default:
      ASSERT (FALSE);
      return 0xFF;
  }
}

/**
  Return CPU Sku

  @retval UINT8              CPU Sku
**/
UINT8
EFIAPI
GetCpuSku (
  VOID
  )
{
  UINT8              CpuType;
  UINT16             CpuDid;
  UINT32             CpuFamilyModel;
  EFI_CPUID_REGISTER Cpuid;
  BOOLEAN            SkuFound;

  SkuFound  = TRUE;
  CpuType   = EnumCpuUnknown;

  ///
  /// Read the CPUID & DID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  CpuFamilyModel = Cpuid.RegEax & CPUID_FULL_FAMILY_MODEL;
  CpuDid = PciRead16 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, 0) + 0x02);

  switch (CpuFamilyModel) {
    case CPUID_FULL_FAMILY_MODEL_ELKHARTLAKE:
      switch (CpuDid) {
        case V_SA_DEVICE_ID_EHL_0:
        case V_SA_DEVICE_ID_EHL_1:
        case V_SA_DEVICE_ID_EHL_2:
        case V_SA_DEVICE_ID_EHL_3:
        case V_SA_DEVICE_ID_EHL_4:
        case V_SA_DEVICE_ID_EHL_5:
        case V_SA_DEVICE_ID_EHL_6:
        case V_SA_DEVICE_ID_EHL_7:
        case V_SA_DEVICE_ID_EHL_8:
        case V_SA_DEVICE_ID_EHL_9:
        case V_SA_DEVICE_ID_EHL_10:
        case V_SA_DEVICE_ID_EHL_11:
        case V_SA_DEVICE_ID_EHL_12:
        case V_SA_DEVICE_ID_EHL_13:
        case V_SA_DEVICE_ID_EHL_14:
        case V_SA_DEVICE_ID_EHL_15:
        case V_SA_DEVICE_ID_EHL_16:
        case V_SA_DEVICE_ID_EHL_17:
        case V_SA_DEVICE_ID_EHL_18:
        case V_SA_DEVICE_ID_EHL_19:
        case V_SA_DEVICE_ID_EHL_20:
        case V_SA_DEVICE_ID_EHL_21:
        case V_SA_DEVICE_ID_EHL_22:
        case V_SA_DEVICE_ID_EHL_23:
          CpuType = EnumCpuUlx;
          break;
        default:
          SkuFound = FALSE;
          break;
      }
      break;
  }

  if (!SkuFound) {
    DEBUG ((DEBUG_ERROR, "Unsupported CPU SKU, Device ID: 0x%02X, CPUID: 0x%08X!\n", CpuDid, CpuFamilyModel));
    ASSERT (FALSE);
  }
  return CpuType;
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
  EFI_HOB_GUID_TYPE                     *GuidHob;
  CPU_INIT_DATA_HOB                     *CpuInitDataHob;
  CPU_CONFIG_DATA                       *CpuConfigData;
  VOID                                  *FspHobList;
  UINT16                                C6Latency = 0;
  UINT16                                C7Latency = 0;
  UINT16                                C8Latency = 0;
  UINT16                                C9Latency = 0;
  UINT16                                C10Latency = 0;
  CPU_SKU                               CpuSku;
  MSR_REGISTER                          TempMsr;
  UINT8                                 MaxRefTemp;
  UINT8                                 Index;

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
  CpuConfigData = (CPU_CONFIG_DATA *)(UINTN)CpuInitDataHob->CpuConfigData;
  CpuSku = GetCpuSku();

  CpuNvs->Cpuid = (UINT32)GetCpuFamily() | (UINT32)GetCpuStepping();
  CpuNvs->Revision = CPU_NVS_AREA_REVISION;
  ///
  /// Calculate the number of Oc bins supported. Read in MSR 194h FLEX_RATIO bits (19:17)
  ///
  TempMsr.Qword = AsmReadMsr64(MSR_FLEX_RATIO);
  CpuNvs->OcBins = (UINT8)RShiftU64((TempMsr.Dwords.Low & B_MSR_FLEX_RATIO_OC_BINS_MASK), N_MSR_FLEX_RATIO_OC_BINS_OFFSET);

  ///
  /// Update NVS ASL items.
  ///

  ///
  /// Intel Turbo Boost Max Technology 3.0
  ///
  CpuNvs->ItbmSmi = 0x29; ///< SMI for resuming the periodic SMM if the OS supports frequency re-read.

  ///
  /// Automatic Thermal Reporting for Thermal Management
  ///
  if (CpuConfigData->AutoThermalReporting) {
    ///
    /// Thermal Reporting for Critical trip
    /// MSR 1A2 bits 23:16 define the temperature that this specific processor can
    /// function upto. It is recommended that this value + 5  be used as default Critical trip point
    /// _CRT.
    ///
    TempMsr.Qword = AsmReadMsr64(MSR_TEMPERATURE_TARGET);
    TempMsr.Qword &= (UINT64)B_MSR_TEMPERATURE_TARGET_REF_TEMP_MASK;
    MaxRefTemp = (UINT8)RShiftU64(TempMsr.Qword, N_MSR_TEMPERATURE_TARGET_REF_TEMP_OFFSET);
    CpuNvs->AutoCriticalTripPoint = MaxRefTemp + 5;
    ///
    /// Thermal Reporting for Active Thermal Management
    /// It is recommended that the processor specific value in MSR 1A2 bits 15:8
    /// be used as the highest Active trip point i.e. _AC0.
    ///
    TempMsr.Qword = AsmReadMsr64(MSR_TEMPERATURE_TARGET);
    TempMsr.Qword &= (UINT64)B_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFST_MASK;
    CpuNvs->AutoActiveTripPoint = MaxRefTemp - (UINT8)RShiftU64(
      TempMsr.Qword,
      N_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFST_OFFSET
      );

    ///
    /// Thermal Reporting for Passive Thermal Management
    /// On all turbo enabled systems, it is recommended that the ACPI _PSV point be
    /// set to a temperature above the Active cooling temperature and Tcc activation
    /// temperature.
    /// If platform embedded controller will issue PECI commands to reduce power as a
    /// passive thermal action, then it is recommended to use the package's max temperature
    /// for passive thermal control.
    ///
    TempMsr.Qword = AsmReadMsr64(MSR_TEMPERATURE_TARGET);
    TempMsr.Qword &= B_MSR_TEMPERATURE_TARGET_TJ_MAX_TCC_OFFSET_MASK;
    CpuNvs->AutoPassiveTripPoint = CpuNvs->AutoCriticalTripPoint + 3;
  }

  ///
  ///  Update the PPM NVS area PPM flags
  ///
  CpuNvs->PpmFlags = CpuInitDataHob->PpmFlags;

  ///
  /// For U/Y, hard coded latency values are used instead of calculations. Set values here.
  ///
  switch (CpuSku) {
    case EnumCpuUlx:
      C6Latency = 127;
      C7Latency = 253;
      C8Latency = 260;
      C9Latency = 487;
      C10Latency = 1048;
      break;

    default:
      break;
  }

  ///
  /// Update PPM NVRAM Values for C6
  ///
  if (CpuNvs->PpmFlags & C6_LONG_LATENCY_ENABLE) {
    CpuNvs->C6MwaitValue = MWAIT_C6_1;
  } else {
    CpuNvs->C6MwaitValue = MWAIT_C6;
  }

  ///
  /// Update PPM NVRAM Values for C7 - select the C-state supported among- C7 / C7S
  ///
  if (CpuNvs->PpmFlags & PPM_C7) { // Is C7 supported ?
    if (CpuNvs->PpmFlags & C7_LONG_LATENCY_ENABLE) {
      CpuNvs->C7MwaitValue = MWAIT_C7_1;
    } else {
      CpuNvs->C7MwaitValue = MWAIT_C7;
    }
  }
  if (CpuNvs->PpmFlags & PPM_C7S) { // Is C7S supported ?
    if (CpuNvs->PpmFlags & C7s_LONG_LATENCY_ENABLE) {
      CpuNvs->C7MwaitValue = MWAIT_C7_3;
    } else {
      CpuNvs->C7MwaitValue = MWAIT_C7_2;
    }
  }

  ///
  /// For ICL, hard coded values are used instead of calculations. Reinitialize here to simplify the code for C6 and C7.
  ///

  if (C6Latency != 0) {
    CpuNvs->C6Latency = LATENCY_C6;
  }

  if (C7Latency != 0) {
    CpuNvs->C7Latency = LATENCY_C7;
  }

  ///
  /// Update PPM NVRAM Values for CD - select the deepest C-state supported among- C8 / C9 / C10
  ///
  if (CpuNvs->PpmFlags & PPM_C10) { // C10 supported
    CpuNvs->CDIOLevel = PCH_ACPI_LV7;
    CpuNvs->CDPowerValue = C10_POWER;
    CpuNvs->CDMwaitValue = MWAIT_CD_2;
    if (C10Latency != 0) {
      CpuNvs->CDLatency = LATENCY_C10;
    }
  } else if (CpuNvs->PpmFlags & PPM_C9) { // C9 supported
    CpuNvs->CDIOLevel = PCH_ACPI_LV6;
    CpuNvs->CDPowerValue = C9_POWER;
    CpuNvs->CDMwaitValue = MWAIT_CD_1;
    if (C9Latency != 0) {
      CpuNvs->CDLatency = LATENCY_C9;
    }
  } else if (CpuNvs->PpmFlags & PPM_C8) { // C8 supported
    CpuNvs->CDIOLevel = PCH_ACPI_LV5;
    CpuNvs->CDPowerValue = C8_POWER;
    CpuNvs->CDMwaitValue = MWAIT_CD;
    if (C8Latency != 0) {
      CpuNvs->CDLatency = LATENCY_C8;
    }
  }

  CpuNvs->CtdpLevelsSupported = CpuInitDataHob->CtdpLevelsSupported;                     /// ConfigTdp Number Of Levels.
  CpuNvs->ConfigTdpBootModeIndex = CpuInitDataHob->ConfigTdpBootModeIndex;               /// CTDP Boot Mode Index
  for (Index = 0; Index < 3; Index++) {
    CpuNvs->CtdpTar[Index] = CpuInitDataHob->CtdpTar[Index];                             /// CTDP Levels TAR
    CpuNvs->CtdpPowerLimit1[Index] = CpuInitDataHob->CtdpPowerLimit1[Index];             /// CTDP Levels Power Limit1
    CpuNvs->CtdpPowerLimit2[Index] = CpuInitDataHob->CtdpPowerLimit2[Index];             /// CTDP Levels Power Limit2
    CpuNvs->CtdpPowerLimitWindow[Index] = CpuInitDataHob->CtdpPowerLimitWindow[Index];   /// CTDP Levels Power Limit1 Time Window
    CpuNvs->CtdpCtc[Index] = CpuInitDataHob->CtdpCtc[Index];                             /// CTDP Levels CTC
    CpuNvs->CtdpPpc[Index] = CpuInitDataHob->CtdpPpc[Index];                             /// CTDP Levels PPC
  }
  CpuNvs->EnableItbm = CpuInitDataHob->EnableItbm;                                       /// Enable/Disable Intel Turbo Boost Max Technology 3.0.
  CpuNvs->EnableItbmDriver = CpuInitDataHob->EnableItbmDriver;                           /// Enable/Disable Intel Turbo Boost Max Technology 3.0 Driver.
  CpuNvs->LowestMaxPerf = CpuInitDataHob->LowestMaxPerf;                                 /// Max ratio of the slowest core.
  CpuNvs->ConfigurablePpc = CpuInitDataHob->ConfigurablePpc;                             /// Max ratio of the slowest core.
  //CpuNvs->DtsIoTrapAddress = DTS_IO_TRAP_ADDRESS;
  //CpuNvs->DtsIoTrapLength = DTS_IO_TRAP_LENGTH;
  //CpuNvs->DtsAcpiEnable = DTS_ACPI_DISABLE;

  DEBUG ((DEBUG_INFO, "Update Cpu Nvs Done\n"));

  DEBUG ((DEBUG_INFO, "Revision 0x%X, PpmFlags 0x%08X\n", CpuNvs->Revision, CpuNvs->PpmFlags));

  ///
  /// Initialize FVID table pointer
  ///
  mFvidPointer = (FVID_TABLE *) (UINTN) CpuInitDataHob->FvidTable;
}

/**
  Dynamic update of Global NVS data

  @param[in] GnvsIn       Pointer to global nvs region

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
  UINT8                   Index;
  UINT8                   Length;
  UINTN                   RpDev;
  UINTN                   RpFun;
  UINT32                  Data32;
  GPIO_GROUP              GroupToGpeDwX[3];
  UINT32                  GroupDw[3];
  EFI_CPUID_REGISTER      CpuidRegs;
  UINTN                   UfsPciBase;
  UINTN                   PseCanPciMmBase;
  UINTN_STRUCT            MchBarBase;
  EFI_STATUS              Status;
  FEATURES_CFG_DATA       *FeaturesCfgData;
  BOOLEAN                 PchSciSupported;
  SILICON_CFG_DATA        *SiCfgData;
  FSPS_UPD                *FspsUpd;
  FSP_S_CONFIG            *FspsConfig;

  PchSciSupported         = PchIsSciSupported ();
  GlobalNvs               = (GLOBAL_NVS_AREA *) GnvsIn;
  PlatformNvs             = (PLATFORM_NVS_AREA *) &GlobalNvs->PlatformNvs;
  PchNvs                  = (PCH_NVS_AREA *) &GlobalNvs->PchNvs;
  CpuNvs                  = (CPU_NVS_AREA *) &GlobalNvs->CpuNvs;
  SaNvs                   = (SYSTEM_AGENT_NVS_AREA *) &GlobalNvs->SaNvs;
  ZeroMem (GlobalNvs, sizeof (GLOBAL_NVS_AREA));

  //
  // Update ASL PCIE port address according to root port device and function
  //
  Length = GetPchMaxPciePortNum ();
  for (Index = 0; Index < Length; Index++) {
    Status = GetPchPcieRpDevFun (Index, &RpDev, &RpFun);
    ASSERT_EFI_ERROR (Status);
    Data32 = ((UINT8) RpDev << 16) | (UINT8) RpFun;
    PchNvs->RpAddress[Index] = Data32;
    DEBUG ((DEBUG_INFO, "RpAddress[%d] = 0x%08X\n", Index, PchNvs->RpAddress[Index]));

    // Need to match with FSP-S UPD
    PchNvs->PcieLtrMaxSnoopLatency[Index]       = 0x1003;
    PchNvs->PcieLtrMaxNoSnoopLatency[Index]     = 0x1003;
  }

  PchNvs->PchSeries                             = PCH_LP;
  PchNvs->PchGeneration                         = EHL_PCH;

  // Update HPET base address.
  PchNvs->HPTE                                  = TRUE;
  PchNvs->HPTB                                  = HPET_BASE_ADDRESS;

  // Update SBREG_BAR.
  PchNvs->SBRG                                  = PCH_PCR_BASE_ADDRESS;

  // Update PMC ACPIBASE, PWRMBASE, TCO
  PchNvs->PMBS                                  = ACPI_BASE_ADDRESS;
  PchNvs->PWRM                                  = PCH_PWRM_BASE_ADDRESS;
  PchNvs->TcoBase                               = 0x400;

  // GPIO
  PchNvs->SGIR                                  = 0xE;
  PchNvs->GPHD                                  = 0;

  //
  // Update GPP_X to GPE_DWX mapping.
  //
  GpioGetGroupDwToGpeDwX (
    &GroupToGpeDwX[0], &GroupDw[0],
    &GroupToGpeDwX[1], &GroupDw[1],
    &GroupToGpeDwX[2], &GroupDw[2]
  );

  //
  // GEI0/1/2 and GED0/1/2 are objects for informing how GPIO groups are mapped to GPE0.
  // If Group is mapped to 1-Tier GPE information is also stored on what Group DW
  // is mapped to GPE_DWx. Because GPE_DWx register is 32 bits large if groups have more than
  // 32 pads only part of it can be mapped.
  //
  //  GEIx - GroupIndex mapped to GPE0_DWx
  //  GEDx - DoubleWorld part of Group: 0 - pins 31-0, 1 - pins 63-32, ...
  //
  PchNvs->GEI0                                  = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[0]);
  PchNvs->GEI1                                  = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[1]);
  PchNvs->GEI2                                  = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[2]);
  PchNvs->GED0                                  = (UINT8) GroupDw[0];
  PchNvs->GED1                                  = (UINT8) GroupDw[1];
  PchNvs->GED2                                  = (UINT8) GroupDw[2];

  // Update eMMC/ SdCard/ UFS
  PchNvs->EMH4                                  = 1;
  PchNvs->EmmcEnabled                           = 1;
  PchNvs->EMDS                                  = 1;
  PchNvs->SdCardEnabled                         = 1;

  UfsPciBase = PciRead32 (PCI_LIB_ADDRESS(0, 18, 5, 0));
  DEBUG((DEBUG_INFO, "UfsPciBase0 = 0x%x\n", UfsPciBase));
  if (UfsPciBase != 0xFFFFFFFF) {
    PchNvs->Ufs0Enabled                         = 1;
  }

  UfsPciBase = PciRead32 (PCI_LIB_ADDRESS(0, 18, 7, 0));
  DEBUG((DEBUG_INFO, "UfsPciBase1 = 0x%x\n", UfsPciBase));
  if (UfsPciBase != 0xFFFFFFFF) {
    PchNvs->Ufs1Enabled                         = 1;
  }

  PchNvs->ClockToRootPortMap[0]                 = 0;
  PchNvs->ClockToRootPortMap[1]                 = 6;
  PchNvs->ClockToRootPortMap[2]                 = 4;
  PchNvs->ClockToRootPortMap[3]                 = 0;

  PchNvs->CpuSku = GetCpuSku();
  for (Index = 0; Index < GetPchMaxPciePortNum (); Index++) {
    PchNvs->LtrEnable[Index]                    = 1;
  }

  PchNvs->GBES                                  = 0;

  // Update HDA ACPI
  PchNvs->XTAL                                  = XTAL_FREQ_38P4MHZ;
  PchNvs->ADFM                                  = 0x101; //DspFeatureMask
  PchNvs->SWQ0                                  = 0;
  PchNvs->SWQ1                                  = 2;
  PchNvs->SWQ2                                  = 2;
  PchNvs->SWQ3                                  = 2;
  PchNvs->ACS0                                  = 1;
  PchNvs->ACS1                                  = 1;
  PchNvs->ACS2                                  = 1;
  PchNvs->ACS3                                  = 1;
  PchNvs->DAI0                                  = 0;
  PchNvs->DAI1                                  = 0;
  PchNvs->DAI2                                  = 0;
  PchNvs->DAI3                                  = 0;
  PchNvs->DOD0                                  = 1;
  PchNvs->DOD1                                  = 1;
  PchNvs->DOD2                                  = 1;
  PchNvs->DOD3                                  = 1;

  SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);

  if ((SiCfgData != NULL) && (SiCfgData->PchPseDmaEnable[1] == HOST_OWNED)) {
    //
    // Saving PSE DMA1 address to DSDT
    //
    PchNvs->PseDma1Address                      = PCH_PSE_DMA1_BASE_ADDRESS;
    DEBUG ((DEBUG_INFO, "Patching DSDT with PSE DMA1: 0x%x\n", PchNvs->PseDma1Address));
    PchNvs->PseDma1Length                       = V_PCH_PSE_DMAC_BAR_SIZE;
    PchNvs->PseDma1En                           = 1;
  } else {
    PchNvs->PseDma1Address                      = 0;
    PchNvs->PseDma1Length                       = 0;
    PchNvs->PseDma1En                           = 0;
  }

  if ((SiCfgData != NULL) && (SiCfgData->PchPseDmaEnable[2] == HOST_OWNED)) {
    //
    // Saving PSE DMA2 address to DSDT
    //
    PchNvs->PseDma2Address                      = PCH_PSE_DMA2_BASE_ADDRESS;
    DEBUG ((DEBUG_INFO, "Patching DSDT with PSE DMA2: 0x%x\n", PchNvs->PseDma2Address));
    PchNvs->PseDma2Length                       = V_PCH_PSE_DMAC_BAR_SIZE;
    PchNvs->PseDma2En                           = 1;
  } else {
    PchNvs->PseDma2Address                      = 0;
    PchNvs->PseDma2Length                       = 0;
    PchNvs->PseDma2En                           = 0;
  }

  PseCanPciMmBase = PciRead16 (PCI_LIB_ADDRESS(0, 24, 1, 0));
  DEBUG((DEBUG_INFO, "PseCanPciMmBase0 = 0x%x\n ", PseCanPciMmBase));
  if (PseCanPciMmBase != 0xFFFF) {
    PchNvs->PseCan0Enabled                        = 1;
  }

  PseCanPciMmBase = PciRead16 (PCI_LIB_ADDRESS(0, 24, 2, 0));
  DEBUG((DEBUG_INFO, "PseCanPciMmBase1 = 0x%x\n ", PseCanPciMmBase));
  if (PseCanPciMmBase != 0xFFFF) {
    PchNvs->PseCan1Enabled                        = 1;
  }

  if (SiCfgData != NULL) {
    PchNvs->EnableTimedGpio0 = (UINT8)SiCfgData->EnableTimedGpio0;
    PchNvs->EnableTimedGpio1 = (UINT8)SiCfgData->EnableTimedGpio1;
  }

  // Update Platform
  PlatformNvs->Ac1TripPoint                     = 55;
  PlatformNvs->Ac0TripPoint                     = 71;
  PlatformNvs->Ac1FanSpeed                      = 75;
  PlatformNvs->Ac0FanSpeed                      = 100;
  PlatformNvs->PassiveThermalTripPoint          = 95;
  PlatformNvs->PassiveTc1Value                  = 1;
  PlatformNvs->PassiveTc2Value                  = 5;
  PlatformNvs->PassiveTspValue                  = 10;
  PlatformNvs->CriticalThermalTripPoint         = 120;
  SysCpuInfo = MpGetInfo ();
  if (SysCpuInfo != NULL) {
    PlatformNvs->ThreadCount                    = (UINT8) SysCpuInfo->CpuCount;
    DEBUG((DEBUG_INFO, "ThreadCount = 0x%x\n ", PlatformNvs->ThreadCount));
  }
  PlatformNvs->EnableDptf                       = 0;
  PlatformNvs->HebcValue                        = 0x233F3;
  PlatformNvs->WifiEnable                       = 0;

  PlatformNvs->PlatformFlavor                   = 1;

  PlatformNvs->Rtd3P0dl                         = 0x64;
  PlatformNvs->Rtd3AudioDelay                   = 0xC8;
  PlatformNvs->Rtd3SensorHub                    = 0x44;
  PlatformNvs->VRRampUpDelay                    = 0x10;
  PlatformNvs->Rtd3Config0                      = 0;
  PlatformNvs->Rtd3Config1                      = 2;
  PlatformNvs->PowerState                       = 1; //AX 1 for mobile platform

  PlatformNvs->NativePCIESupport                = 1;

  PlatformNvs->ApicEnable                       = 1;
  PlatformNvs->EcAvailable                      = 0;
  FeaturesCfgData = (FEATURES_CFG_DATA *) FindConfigDataByTag (CDATA_FEATURES_TAG);
  if (FeaturesCfgData != NULL) {
    PlatformNvs->NewGpioSchemeEnable = (UINT8) FeaturesCfgData->Features.NewGpioSchemeEnable;
  }
  DEBUG((DEBUG_INFO, "PlatformNvs->NewGpioSchemeEnable = 0x%x\n ", PlatformNvs->NewGpioSchemeEnable));
  PlatformNvs->TenSecondPowerButtonEnable       = 8;

  //
  // WWAN GPIO pin configuration
  //
  PlatformNvs->WwanRtd3Option                   = 1;
  PlatformNvs->WwanPerstGpio                    = GPIO_VER3_GPP_B14;
  PlatformNvs->WwanPerstGpioPolarity            = 0;
  PlatformNvs->WwanBbrstGpio                    = GPIO_VER3_GPP_V13;
  PlatformNvs->WwanBbrstGpioPolarity            = 0; //PIN_GPIO_ACTIVE_LOW;
  PlatformNvs->WwanWakeGpio                     = GPIO_VER3_GPP_B17;
  PlatformNvs->WwanFullCardPowerOffGpio         = GPIO_VER3_GPP_E0;
  PlatformNvs->WwanFullCardPowerOffGpioPolarity = 1; //PIN_GPIO_ACTIVE_HIGH;

  //
  // PCH/CPU PCIe x4 Slot 1 GPIO pin configuration
  //
  PlatformNvs->PcieSlot1WakeGpio                = GPIO_VER3_GPP_E2;
  PlatformNvs->PcieSlot1PowerEnableGpio         = 0;
  PlatformNvs->PcieSlot1PowerEnableGpioPolarity = 0;
  PlatformNvs->PcieSlot1RstGpio                 = GPIO_VER3_GPD11;
  PlatformNvs->PcieSlot1RstGpioPolarity         = 0;
  PlatformNvs->PcieSlot1RpNumber                = 1;
  PlatformNvs->PcieSlot1ClkSrc                  = 0;
  //
  // PCH PCIe x1 Slot 2 GPIO pin configuration
  //
  PlatformNvs->PcieSlot2WakeGpio                = GPIO_VER3_GPP_F4;
  PlatformNvs->PcieSlot2PowerEnableGpio         = 0;
  PlatformNvs->PcieSlot2PowerEnableGpioPolarity = 0;
  PlatformNvs->PcieSlot2RstGpio                 = 0;
  PlatformNvs->PcieSlot2RstGpioPolarity         = 0;
  PlatformNvs->PcieSlot2RpNumber                = 7;

  //
  // CPU M.2 SSD GPIO pin configuration
  //

  PlatformNvs->M2Ssd2PowerEnableGpio            = 0;
  PlatformNvs->M2Ssd2PowerEnableGpioPolarity    = 0;
  PlatformNvs->M2Ssd2RstGpio                    = 0;
  PlatformNvs->M2Ssd2RstGpioPolarity            = 0;

  //
  // PCH M.2 SSD GPIO pin configuration
  //
  PlatformNvs->PchM2SsdPowerEnableGpio          = GPIO_VER3_GPP_F21;
  PlatformNvs->PchM2SsdPowerEnableGpioPolarity  = 1;
  PlatformNvs->PchM2SsdRstGpio                  = GPIO_VER3_GPP_V13;
  PlatformNvs->PchM2SsdRstGpioPolarity          = 1;

  //
  // PCH Sata ports GPIO pin configuration
  //
  PlatformNvs->SataPortPowerEnableGpio          = GPIO_VER3_GPP_F21;
  PlatformNvs->SataPortPowerEnableGpioPolarity  = 1;

  //
  // WLAN GPIO pin configuration
  //
  PlatformNvs->WlanWakeGpio                     = GPIO_VER3_GPP_F4;

  //
  // Micro-PEP constraint list
  //
  /**
  PlatformNvs->LowPowerS0IdleConstraint = (((mPchSetup.PchSata == 1 ? 0x3:0) & mSystemConfiguration.PepSataContraints)  <<  0) | // Bit[1:0] - Storage (0:None, 1:Adapter D0/F1, 2:Raid, 3:Adapter D3)
                                                            ((PepSerialIoUart[0] && mSystemConfiguration.PepUart0)                        <<  2) | // Bit[2]   - En/Dis UART0
                                                            ((PepSerialIoUart[1] && mSystemConfiguration.PepUart1)                        <<  3) | // Bit[3]   - En/Dis UART1
                                                            ( mSystemConfiguration.PepPcieStorage                                         <<  4) | // Bit[4]   - En/Dis PCIe NVMe/AHCI
                                                            ((PepSerialIoI2c[0] && mSystemConfiguration.PepI2c0)                          <<  5) | // Bit[5]   - En/Dis I2C0
                                                            ((PepSerialIoI2c[1] && mSystemConfiguration.PepI2c1)                          <<  6) | // Bit[6]   - En/Dis I2C1
                                                            ( mSystemConfiguration.PepXhci                                                <<  7) | // Bit[7]   - En/Dis XHCI
                                                            ((mPchSetup.PchHdAudio && mSystemConfiguration.PepAudio)                      <<  8) | // Bit[8]   - En/Dis HD Audio (includes ADSP)
                                                            ( mSystemConfiguration.PepGfx                                                 <<  9) | // Bit[9]   - En/Dis GFX
                                                            ( mSystemConfiguration.PepCpu                                                 << 10) | // Bit[10]  - En/Dis CPU
                                                            ((mPchSetup.PchScsEmmcEnabled && mSystemConfiguration.PepEmmc)                << 11) | // Bit[11]  - En/Dis EMMC
                                                            ((mPchSetup.PchScsSdCardEnabled && mSystemConfiguration.PepSdxc)              << 12) | // Bit[12]  - En/Dis SDXC
                                                            ((PepSerialIoI2c[2] && mSystemConfiguration.PepI2c2)                          << 13) | // Bit[13]  - En/Dis I2C2
                                                            ((PepSerialIoI2c[3] && mSystemConfiguration.PepI2c3)                          << 14) | // Bit[14]  - En/Dis I2C3
                                                            ((PepSerialIoI2c[4] && mSystemConfiguration.PepI2c4)                          << 15) | // Bit[15]  - En/Dis I2C4
                                                            ((PepSerialIoI2c[5] && mSystemConfiguration.PepI2c5)                          << 16) | // Bit[16]  - En/Dis I2C5
                                                            ((PepSerialIoUart[2] && mSystemConfiguration.PepUart2)                        << 17) | // Bit[17]  - En/Dis UART2
                                                            ((PepSerialIoSpi[0] && mSystemConfiguration.PepSpi0)                          << 18) | // Bit[18]  - En/Dis SPI0
                                                            ((PepSerialIoSpi[1] && mSystemConfiguration.PepSpi1)                          << 19) | // Bit[19]  - En/Dis SPI1
                                                            ((PepSerialIoSpi[2] && mSystemConfiguration.PepSpi2)                          << 20) | // Bit[20]  - En/Dis SPI2
                                                            (((!mPchSetup.PchHdaIDisplayCodecDisconnect) && mSystemConfiguration.PepAudio)<< 21) | // Bit[21]  - En/Dis IDA
                                                            ( mSystemConfiguration.PepCsme                                                << 22) | // Bit[22]  - En/Dis CSME
                                                            ((PepSerialIoI2c[6] && mSystemConfiguration.PepI2c6)                          << 23) | // Bit[23]   -En/Dis I2C6
                                                            ((PepSerialIoI2c[7] && mSystemConfiguration.PepI2c7)                          << 24) | // Bit[24]  - En/Dis I2C7
                                                            (mSystemConfiguration.PepThc0                                                 << 25) | // Bit[25]  - En/Dis THC0
                                                            (mSystemConfiguration.PepThc1                                                 << 26) | // Bit[26]  - En/Dis THC1
                                                            (mSystemConfiguration.PepIsi                                                  << 27) | // Bit[27]  - En/Dis ISI
                                                            (mSystemConfiguration.PepGbeTsn                                               << 28) | // Bit[28]  - En/Dis GBETSN
                                                            (mSystemConfiguration.PepPse                                                  << 29);  // Bit[29]  - En/Dis PSE
  DEBUG((DEBUG_INFO, "ACPI NVS, LowPowerS0IdleConstraint(Micro-pep constraints) = 0x%X \n", mPlatformNvsAreaProtocol.Area->LowPowerS0IdleConstraint )); //0x1F6FFBD
**/
  PlatformNvs->LowPowerS0IdleConstraint = 0x11E2FFBC;
  DEBUG((DEBUG_INFO, "ACPI NVS, LowPowerS0IdleConstraint(Micro-pep constraints) = 0x%X \n", PlatformNvs->LowPowerS0IdleConstraint ));
  PlatformNvs->LowPowerS0IdleConstraint2 = 1;
  DEBUG((DEBUG_INFO, "ACPI NVS, LowPowerS0IdleConstraint(Micro-pep constraints 2) = 0x%X \n", PlatformNvs->LowPowerS0IdleConstraint2 ));

  PlatformNvs->TsnEnabled                       = IsTsnPresent();
  PlatformNvs->PseTsn0Enabled                   = IsPseGbe0Enabled();
  PlatformNvs->PseTsn1Enabled                   = IsPseGbe1Enabled();
  PlatformNvs->PseUart0Rs485Enabled             = 1;
  PlatformNvs->PseUart1Rs485Enabled             = 1;
  PlatformNvs->PseUart2Rs485Enabled             = 1;
  PlatformNvs->PseUart3Rs485Enabled             = 0;

  PlatformNvs->IC0S                             = 400000;
  PlatformNvs->IC1S                             = 400000;
  PlatformNvs->IC2S                             = 400000;
  PlatformNvs->IC3S                             = 400000;
  PlatformNvs->IC4S                             = 400000;
  PlatformNvs->IC5S                             = 400000;
  PlatformNvs->IC6S                             = 400000;
  PlatformNvs->IC7S                             = 400000;

  FspsUpd     = (FSPS_UPD *)(UINTN)PcdGet32 (PcdFspsUpdPtr);
  FspsConfig  = &FspsUpd->FspsConfig;
  Length = GetPchMaxSerialIoUartControllersNum ();
  for (Index = 0; Index < Length ; Index++) {
    PchNvs->UM0[Index] = FspsConfig->SerialIoUartMode[Index];
    if (FspsConfig->SerialIoUartMode[Index] == 1) {
      PchNvs->UC0[Index] = GetSerialIoUartPciCfgBase(Index);
    } else if (FspsConfig->SerialIoUartMode[Index] == 3) {
      PchNvs->UC0[Index] = PCH_SERIAL_IO_BASE_ADDRESS + 0x1F000 + 0x2000 * Index; /* COM */
    }
    PchNvs->UD0[Index] = FspsConfig->SerialIoUartDmaEnable[Index];
    PchNvs->UP0[Index] = FspsConfig->SerialIoUartPowerGating[Index];
    PchNvs->UI0[Index] = GetSerialIoUartIrq(Index);
  }

  PlatformNvs->USTP                             = 1;

  PlatformNvs->Ps2MouseEnable                   = 0;
  PlatformNvs->Ps2KbMsEnable                    = 0;
  PlatformNvs->UsbTypeCSupport                  = 1;

  PlatformNvs->HomeButtonInterrupt              = 0;
  PlatformNvs->VolumeUpButtonInterrupt          = 0;
  PlatformNvs->VolumeDownButtonInterrupt        = 0;
  PlatformNvs->PcdPmPowerButtonGpioPin          = GPIO_VER3_GPD2;

  // Update Audio

  PlatformNvs->I2SC                             = 2; // I2S Codec Select
  PlatformNvs->I2SI                             = GPIO_VER3_GPP_B15; // Codec IRQ GPIO
  PlatformNvs->I2SB                             = 3; // I2S Codec I2C Bus number
  PlatformNvs->HdaDspPpModuleMask               = 0;

  // System Agent
  SaNvs->XPcieCfgBaseAddress                    = (UINT32) (PcdGet64 (PcdPciExpressBaseAddress));

  AsmCpuid (1, &CpuidRegs.RegEax, 0, 0, 0);
  SaNvs->CpuIdInfo                              = (CpuidRegs.RegEax & 0x0FFFFF);
  PlatformNvs->PlatformCpuId                    = (CpuidRegs.RegEax & 0x0FFFFF);
  SaNvs->VmdEnable                              = 0;

  SaNvs->SimicsEnvironment                      = 0;

  SaNvs->AlsEnable                              = 0;
  SaNvs->IgdState                               = 0;
  SaNvs->BrightnessPercentage                   = 100;
  SaNvs->IgdBootType                            = 0;
  SaNvs->IgdPanelType                           = 0;
  SaNvs->IgdPanelScaling                        = 0;
  SaNvs->IgdDvmtMemSize                         = 2;
  SaNvs->IgdFunc1Enable                         = 0;

  MchBarBase.Data32.High = PciRead32 (PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, R_SA_MCHBAR + 4));
  MchBarBase.Data32.Low  = PciRead32 (PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, R_SA_MCHBAR));
  MchBarBase.Data        &= (UINTN) ~BIT0;

  SaNvs->IgdHpllVco                             = MmioRead8 (MchBarBase.Data + 0xC0F) & 0x07;
  SaNvs->IgdSciSmiMode                          = 0;
  SaNvs->GfxTurboIMON                           = 0x1F;
  SaNvs->EdpValid                               = 0;

  SaNvs->Mmio32Base                             = PcdGet32(PcdPciResourceMem32Base);
  if (SaNvs->Mmio32Base < SaNvs->XPcieCfgBaseAddress) {
    SaNvs->Mmio32Length = SaNvs->XPcieCfgBaseAddress - SaNvs->Mmio32Base;
  } else if (SaNvs->Mmio32Length < 0xF0000000) {
    SaNvs->Mmio32Length = 0xF0000000 - SaNvs->Mmio32Base;
  } else {
    DEBUG((DEBUG_INFO, "acpi: Unable to configure M32L with M32B=0x%08X\n", SaNvs->Mmio32Base));
  }

  UpdateCpuNvs (CpuNvs);
  PlatformNvs->PpmFlags                         = CpuNvs->PpmFlags;
  SocUpdateAcpiGnvs ((VOID *)GnvsIn);

  PlatformNvs->Rtd3Support = PchSciSupported? 0 : 1;

  PlatformNvs->LowPowerS0Idle = S0IX_STATUS();
  DEBUG((DEBUG_INFO, "PlatformNvs->S0ix = 0x%x\n ", PlatformNvs->LowPowerS0Idle));
}
