/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/TpmLib.h>
#include <PlatformData.h>
#include <Library/MeExtMeasurementLib.h>
#include "Stage2BoardInitLib.h"
#include <Library/PsdLib.h>
#include <Library/TimerLib.h>

#if FixedPcdGetBool (PcdWclSupport) == 1
#include <GpioV2PinsWclPcd.h>
#else
#include <GpioV2PinsPtlPcd.h>
#endif

#include <Include/GpioV2Config.h>
#include <Library/GpioV2Lib.h>
#include <GlobalNvsAreaDef.h>
#include <CpuRegs.h>
#include <Library/FusaConfigLib.h>
#include <Library/PciePm.h>
#include <Library/AmtLib.h>
#include <Library/TxtLib.h>
#include <Dtpr.h>
#include <Library/PciEnumerationLib.h>
#include <Library/MadtLib.h>
#include <PlatformBase.h>
#include <Library/AcpiInitLib.h>
#include <Library/BootloaderCommonLib.h>

#define ICH_IOAPIC_ID 0x02

// Define the TPR_INFO_HOB GUID
extern EFI_GUID gTprInfoHobGuid;

CONST PCH_SERIAL_IO_CONFIG_INFO mPchSSerialIoSPIMode[PCH_MAX_SERIALIO_SPI_CONTROLLERS] = {
  {0, PCI_SEGMENT_LIB_ADDRESS(0, 0, 30, 2, 0)},
  {0, PCI_SEGMENT_LIB_ADDRESS(0, 0, 30, 3, 0)},
  {0, PCI_SEGMENT_LIB_ADDRESS(0, 0, 18, 6, 0)},
  {0, 0},
};

CONST PCH_SERIAL_IO_CONFIG_INFO mPchSSerialIoI2CMode[PCH_MAX_SERIALIO_I2C_CONTROLLERS] = {
  {1, PCI_SEGMENT_LIB_ADDRESS(0, 0, 21, 0, 0)},
  {1, PCI_SEGMENT_LIB_ADDRESS(0, 0, 21, 1, 0)},
  {1, PCI_SEGMENT_LIB_ADDRESS(0, 0, 21, 2, 0)},
  {1, PCI_SEGMENT_LIB_ADDRESS(0, 0, 21, 3, 0)},
  {1, PCI_SEGMENT_LIB_ADDRESS(0, 0, 25, 0, 0)},
  {0, PCI_SEGMENT_LIB_ADDRESS(0, 0, 25, 1, 0)},
};

CONST PCH_SERIAL_IO_UART_CONFIG_INFO mPchSSerialIoUartMode[PCH_MAX_SERIALIO_UART_CONTROLLERS] = {
  {0, R_SERIAL_IO_PCR_PCICFGCTRL_UART0, 1, 2, 16 },
  {0, R_SERIAL_IO_PCR_PCICFGCTRL_UART1, 1, 2, 17 },
  {2, R_SERIAL_IO_PCR_PCICFGCTRL_UART2, 1, 2, 24 },
};

GLOBAL_REMOVE_IF_UNREFERENCED UINT8    mBigCoreCount;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8    mSmallCoreCount;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8    mModuleId;

// Forward declaration
EFI_STATUS
UpdateDtprAcpiTable (
  IN EFI_ACPI_DESCRIPTION_HEADER   *Table
  );

// Forward declaration
/**
  Disable CR4.SMXE on BSP when TXT is enabled at ReadyToBoot.
  This ensures SMX is disabled on BSP before OS boot.

  @retval EFI_SUCCESS     CR4.SMXE disabled on all APs successfully.
  @retval EFI_NOT_FOUND   Failed to get CPU task structure.
  @retval EFI_TIMEOUT     Timeout waiting for APs to complete.
**/
EFI_STATUS
EFIAPI
DisableSmxOnBSP (
  VOID
  );

STATIC SMMBASE_INFO mSmmBaseInfo = {
  { BL_PLD_COMM_SIG, SMMBASE_INFO_COMM_ID, 0, 0 }
};

STATIC S3_SAVE_REG mS3SaveReg = {
  { BL_PLD_COMM_SIG, S3_SAVE_REG_COMM_ID, 1, 0 },
  { { REG_TYPE_IO, WIDE32, { 0, 0}, (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN), 0x00000000 } }
};

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
    DEBUG ((DEBUG_INIT, "Obtaining CSME Boot performance data failed with status: %r\n", Status));
    return Status;
  } else {
    DEBUG ((DEBUG_INIT, "Found CSME Boot performance data!\n"));
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
  UINTN           SpiBaseAddress;
  UINT8           Data8;

  SpiBaseAddress = GetDeviceAddr (OsBootDeviceSpi, 0);
  SpiBaseAddress = TO_MM_PCI_ADDRESS (SpiBaseAddress);

  // Set the BIOS Lock Enable and EISS bits
  MmioOr8 (SpiBaseAddress + R_SPI_BCR, (UINT8) (B_SPI_BCR_BLE | B_SPI_BCR_EISS));
  // Read back the BIOS Control register to ensure the write has completed before proceeding
  Data8 = MmioRead8 (SpiBaseAddress + R_SPI_BCR);
  DEBUG ((DEBUG_INFO, "BIOS Control settings 0x%x\n", Data8));
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
  UINT16                Pm1Sts;

  SmiEn = IoRead32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
  if (((SmiEn & B_ACPI_IO_SMI_EN_GBL_SMI_EN) !=0) && ((SmiEn & B_ACPI_IO_SMI_EN_EOS) !=0)) {
    return;
  }

  //
  // Clear the status before setting smi enable. For RTC this has to be
  // done in a specific order to avoid race condition re-triggering status.
  //
  SmiSts = IoRead32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN + 4));
  Pm1Sts = IoRead16 ((UINTN)(ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS));

  // Clear RTC alarm and any SMI/PM1/GPE0 status bits
  IoWrite8 (R_RTC_IO_INDEX, R_RTC_IO_REGC);
  IoRead8 (R_RTC_IO_TARGET); // RTC alarm is cleared upon read

  // Clear GPE0 STS in case some bits are set
  IoOr32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_STS_127_96), 0);

  // Clear GPE0_EN PME_EN
  IoAnd32 ((UINTN)(UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_EN_127_96), (UINT32)~B_ACPI_IO_GPE0_EN_127_96_PME_EN);

  SmiSts |=
    (
      B_ACPI_IO_SMI_STS_ESPI_SMI_STS |
      B_ACPI_IO_SMI_STS_SMBUS_SMI_STS |
      B_ACPI_IO_SMI_STS_PERIODIC_STS |
      B_ACPI_IO_SMI_STS_TCO_STS |
      B_ACPI_IO_SMI_STS_MCSMI_STS |
      B_ACPI_IO_SMI_STS_SWSMI_TMR_STS |
      B_ACPI_IO_SMI_STS_APM_STS |
      B_ACPI_IO_SMI_STS_SMI_ON_SLP_EN_STS |
      B_ACPI_IO_SMI_STS_BIOS_STS |
      B_ACPI_IO_SMI_STS_PM1_STS
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
  Diagnose and sanitize S3 resume SMI/GPE state before triggering payload SW SMI.

  This routine logs SMI/GPE registers, masks deliverable pending GPE sources,
  clears sticky status bits, and enables the minimum SMI path required for the
  payload SW SMI flow when no real enabled GPE source remains pending.

  @retval None
**/
STATIC
VOID
EFIAPI
HandleS3GpeResume (
  VOID
  )
{
  UINT32          DiagSmiEn;
  UINT32          DiagSmiSts;
  UINT16          DiagPm1Sts;
  UINT32          DiagGpe0Sts;
  UINT32          FixSmiSts;
  UINT32          FixSmiEn;
  UINT32          FixGpeSts;
  UINT32          FixGpeEn;
  UINT32          FixRealPending;
  UINTN           FixGrp;
  // GPE = ACPI General-Purpose Events.
  // Each GPE bit is an event source. A bit causes an SMI only when both:
  // 1) its status bit (GPE0_STS) is set, and
  // 2) its enable bit (GPE0_EN) is set.
  //
  // The PCH exposes GPE0 in four 32-bit slices (total 128 bits):
  //   [0] -> GPE0 bits [31:0]
  //   [1] -> GPE0 bits [63:32]
  //   [2] -> GPE0 bits [95:64]
  //   [3] -> GPE0 bits [127:96]
  //
  // mGpeStsOff[] and mGpeEnOff[] provide matching STS/EN register offsets per slice
  // so we can compute (STS & EN), mask enabled pending sources, and clear sticky STS
  // bits during S3 resume SMI diagnostics/fixup.
  STATIC CONST UINT16  mGpeStsOff[4] = {
    R_ACPI_IO_GPE0_STS_31_0,
    R_ACPI_IO_GPE0_STS_63_32,
    R_ACPI_IO_GPE0_STS_95_64,
    R_ACPI_IO_GPE0_STS_127_96
  };
  // GPE0_EN register offsets for the same four slices; each enable bit gates
  // whether the corresponding GPE0_STS bit can become a deliverable SMI source.
  STATIC CONST UINT16  mGpeEnOff[4]  = {
    R_ACPI_IO_GPE0_EN_31_0,
    R_ACPI_IO_GPE0_EN_63_32,
    R_ACPI_IO_GPE0_EN_95_64,
    R_ACPI_IO_GPE0_EN_127_96
  };

  DiagSmiEn   = IoRead32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
  DiagSmiSts  = IoRead32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN + 4));
  DiagPm1Sts  = IoRead16 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_STS));
  DiagGpe0Sts = IoRead32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_GPE0_STS_127_96));
  DEBUG ((DEBUG_INFO,
    "[S3-SMI-DIAG] (PostPci) After RestoreS3RegInfo: SMI_EN=0x%x (GBL=%d APMC=%d EOS=%d) SMI_STS=0x%x PM1_STS=0x%x GPE0_STS[127:96]=0x%x\n",
    DiagSmiEn,
    (DiagSmiEn & B_ACPI_IO_SMI_EN_GBL_SMI_EN) ? 1 : 0,
    (DiagSmiEn & B_ACPI_IO_SMI_EN_APMC_EN) ? 1 : 0,
    (DiagSmiEn & B_ACPI_IO_SMI_EN_EOS) ? 1 : 0,
    DiagSmiSts, DiagPm1Sts, DiagGpe0Sts));

  for (FixGrp = 0; FixGrp < 4; FixGrp++) {
    FixGpeSts = IoRead32 ((UINTN) (ACPI_BASE_ADDRESS + mGpeStsOff[FixGrp]));
    FixGpeEn  = IoRead32 ((UINTN) (ACPI_BASE_ADDRESS + mGpeEnOff[FixGrp]));
    DEBUG ((DEBUG_INFO, "GPE0[%d] STS@0x%x=0x%x EN@0x%x=0x%x (STS&EN=0x%x)\n",
            FixGrp, mGpeStsOff[FixGrp], FixGpeSts, mGpeEnOff[FixGrp], FixGpeEn, FixGpeSts & FixGpeEn));
    if ((FixGpeSts & FixGpeEn) != 0) {
      FixGpeEn &= ~FixGpeSts;
      IoWrite32 ((UINTN) (ACPI_BASE_ADDRESS + mGpeEnOff[FixGrp]), FixGpeEn);
      DEBUG ((DEBUG_INFO, "masked EN, now 0x%x\n", FixGpeEn));
    }
  }

  for (FixGrp = 0; FixGrp < 4; FixGrp++) {
    FixGpeSts = IoRead32 ((UINTN) (ACPI_BASE_ADDRESS + mGpeStsOff[FixGrp]));
    if (FixGpeSts != 0) {
      IoWrite32 ((UINTN) (ACPI_BASE_ADDRESS + mGpeStsOff[FixGrp]), FixGpeSts);
    }
  }
  FixSmiSts = IoRead32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN + 4));
  if (FixSmiSts != 0) {
    IoWrite32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN + 4), FixSmiSts);
  }

  FixRealPending = 0;
  for (FixGrp = 0; FixGrp < 4; FixGrp++) {
    FixGpeSts = IoRead32 ((UINTN) (ACPI_BASE_ADDRESS + mGpeStsOff[FixGrp]));
    FixGpeEn  = IoRead32 ((UINTN) (ACPI_BASE_ADDRESS + mGpeEnOff[FixGrp]));
    FixRealPending |= (FixGpeSts & FixGpeEn);
  }
  FixSmiSts = IoRead32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN + 4));
  DEBUG ((DEBUG_INFO,
    "Real deliverable GPE0 pending (STS&EN)=0x%x; SMI_STS=0x%x (bit9 raw rollup ignored)\n",
    FixRealPending, FixSmiSts));

  if (FixRealPending == 0) {
    UINT16  FixPm1Cnt;

    // GPE0 routes to SMI only while SCI_EN=0. Set SCI_EN=1 BEFORE enabling GBL_SMI
    // so stale GPE status routes to SCI, not SMM.
    FixPm1Cnt = IoRead16 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT));
    IoWrite16 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT),
               FixPm1Cnt | B_ACPI_IO_PM1_CNT_SCI_EN);

    // Prepare SMI enables so the payload SW SMI can run safely.
    FixSmiEn = IoRead32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
    IoWrite32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN),
               FixSmiEn | B_ACPI_IO_SMI_EN_GBL_SMI_EN);

    FixSmiEn = IoRead32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN));
    IoWrite32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN),
               FixSmiEn | B_ACPI_IO_SMI_EN_APMC_EN);

    DEBUG ((DEBUG_INFO, "keeping SCI_EN=1 (no restore); PM1_CNT=0x%x SMI_STS=0x%x\n",
            IoRead16 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_PM1_CNT)),
            IoRead32 ((UINTN) (ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN + 4))));
    return;
  }

  DEBUG ((DEBUG_INFO, "Real enabled GPE0 SMI persists (0x%x) -> NOT triggering payload SW SMI (avoiding reset).\n", FixRealPending));
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
  Gnvs->IgpuNvs.IgdOpRegionAddress = (UINT32)(UINTN)PcdGet32 (PcdIgdOpRegionAddress);
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
    FusaConfigPostSi();

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
        DEBUG ((DEBUG_INFO, "FspGfxHob->FrameBufferBase = 0x%llx\n", FspGfxHob->FrameBufferBase));
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

    // Disable AMT features
    DisableAmtFeatures ();
    break;
  case PrePciEnumeration:
    (VOID) PcdSet32S (PcdPciEnumHookProc, (UINT32)(UINTN) PlatformPciEnumHookProc);
    // Prepare platform ACPI tables
    Status = PcdSet32S (PcdAcpiTableTemplatePtr, (UINT32)(UINTN)mPlatformAcpiTables);
    break;
  case PostPciEnumeration:
    if (FeaturePcdGet (PcdEnablePciePm)) {
      PciePmConfig ();
    }
    Status = SetFrameBufferWriteCombining (0, MAX_UINT32);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "Failed to set GFX framebuffer as WC\n"));
    }

    //
    // Mirror the known-good ADL ordering. ADL restores SMRR via
    // the payload SW SMI in PostPciEnumeration and only AFTERWARDS calls
    // TxtS3Restore() to re-arm LT/TXT. PTL previously restored SMRR much later at
    // EndOfStages -- i.e. AFTER TxtS3Restore() (here) + ProgramSecuritySetting --
    // so the first deliverable SMI entered SMM with an INVALID SMRR while LT was
    // already armed -> LT integrity violation -> LT reset (next boot TXT.E2STS=
    // 0x08 RESET_STS=1). Restoring SMRR HERE, BEFORE TxtS3Restore(), keeps SMRR
    // valid before LT is re-armed. The instrumented step1..4 prints (each followed
    // by a UART drain) identify the exact faulting instruction if it still dies.
    if ((GetPayloadId () == UEFI_PAYLOAD_ID_SIGNATURE) && (GetBootMode() == BOOT_ON_S3_RESUME)) {
      ClearSmi ();
      RestoreS3RegInfo (FindS3Info (S3_SAVE_REG_COMM_ID));
      HandleS3GpeResume ();

      //
      // If payload registered a software SMI handler for bootloader to restore
      // SMRR base and mask in S3 resume path, trigger sw smi
      //
      BlSwSmiInfo = FindS3Info (BL_SW_SMI_COMM_ID);
      if (BlSwSmiInfo != NULL) {
        TriggerPayloadSwSmi (BlSwSmiInfo->BlSwSmiHandlerInput);
      }
    }

    //
    // Restore TXT device memory registers on S3 before FSP PostPciEnumeration
    // notification.
    //
    if (FeaturePcdGet (PcdTxtEnabled) && (GetBootMode() == BOOT_ON_S3_RESUME)) {
      FeaturesCfgData = (FEATURES_DATA *) FindConfigDataByTag(CDATA_FEATURES_TAG);
      if ((FeaturesCfgData != NULL) && (FeaturesCfgData->TxtEnabled == 1)) {
        Status = TxtS3Restore();
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "TxtS3Restore failed: %r\n", Status));
        }
      }
    }

    break;
  case PrePayloadLoading:
    ///
    /// Initialize the IGD OpRegion
    ///
    IgdOpRegionPlatformInit ();
    ///
    /// Initialize Txt
    ///
    if (FeaturePcdGet (PcdTxtEnabled)) {
      FeaturesCfgData = (FEATURES_DATA *) FindConfigDataByTag(CDATA_FEATURES_TAG);
      if ((FeaturesCfgData != NULL) && (FeaturesCfgData->TxtEnabled == 1)) {
        if (GetBootMode() != BOOT_ON_S3_RESUME) {
          InitTxt();

          Status = DisableSmxOnBSP();
          if (EFI_ERROR(Status)) {
            DEBUG ((DEBUG_ERROR, "TXT: Failed to disable CR4.SMXE: %r\n", Status));
          }
        }
      }
    }
    ///
    /// Initialize the HECI device (for test HeciInitLib only)
    ///
    Status = HeciInitialize (HECI1_DEVICE);
    DEBUG ((DEBUG_INFO, "HeciInitLib init Status: %r\n", Status));

    // Initialize and setup MeMeasureboot when MEASURED_BOOT_ENABLED is enabled
    if (MEASURED_BOOT_ENABLED()) {
      MeMeasuredBootInit();
    }
    break;
  case EndOfStages:
    // Register Heci Service
    HeciRegisterHeciService ();
    ClearSmi ();
    if (GetPayloadId () == UEFI_PAYLOAD_ID_SIGNATURE) {
      if (GetBootMode() != BOOT_ON_S3_RESUME) {
        // Clear S3 save region to avoid using stale data from previous boot
        ClearS3SaveRegion ();

        //
        // Set SMMBASE_INFO dummy structure in TSEG before others
        //
        mSmmBaseInfo.SmmBaseHdr.Count     = (UINT8) MpGetInfo()->CpuCount;
        mSmmBaseInfo.SmmBaseHdr.TotalSize = sizeof(BL_PLD_COMM_HDR) + mSmmBaseInfo.SmmBaseHdr.Count * sizeof(CPU_SMMBASE);
        AppendS3Info ((VOID *)&mSmmBaseInfo, TRUE);
        //
        // Set REG_INFO struct in TSEG region except 'Val' for regs
        //
        mS3SaveReg.S3SaveHdr.TotalSize = sizeof(BL_PLD_COMM_HDR) + mS3SaveReg.S3SaveHdr.Count * sizeof(REG_INFO);
        AppendS3Info ((VOID *)&mS3SaveReg, FALSE);

        // Need to Save ACPI info again after clearing region
        Status = SaveAcpiDataForS3 ();
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "Failed to save ACPI S3 data after clearing region: %r - S3 Resume expected to fail.\n", Status));
        }
      }
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
    if (!PcdGetBool(PcdEnableSetup) && (GetBootMode() != BOOT_ON_FLASH_UPDATE) && (GetPayloadId() == 0)) {
      ProgramSecuritySetting ();

      // Set SMI LOCK (SMI_LOCK)
      DEBUG ((DEBUG_INFO, "Set SMI Lock\n"));
      MmioOr8 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_GEN_PMCON_B, (UINT8)B_PMC_PWRM_GEN_PMCON_B_SMI_LOCK);
    }
    break;
  case EndOfFirmware:
    //
    // TxtS3Resume runs at EndOfFirmware so that it executes AFTER both
    // FspNotifyPhase(ReadyToBoot) and FspNotifyPhase(EndOfFirmware).
    // The ACM requires BIOS_DONE to be set before it will dispatch
    // SCHECK/LOCK_CONFIG.  FSP-S sets BIOS_DONE during one of these
    // notification phases ("at EndOfPei" per FSP UPD), so GETSEC must
    // run after all FSP notifications complete.
    //
    if (FeaturePcdGet (PcdTxtEnabled) && (GetBootMode() == BOOT_ON_S3_RESUME)) {
      FeaturesCfgData = (FEATURES_DATA *) FindConfigDataByTag(CDATA_FEATURES_TAG);
      if ((FeaturesCfgData != NULL) && (FeaturesCfgData->TxtEnabled == 1)) {
        UINT32 SmiEnAddr = (UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN);
        UINT32 SmiEnSave;

        SmiEnSave = IoRead32 ((UINTN) SmiEnAddr);
        IoWrite32 ((UINTN) SmiEnAddr, SmiEnSave & (UINT32)~B_ACPI_IO_SMI_EN_APMC_EN);

        Status = TxtS3Resume();
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_INFO, "TxtS3Resume failed: %r\n", Status));
        }

        IoWrite32 ((UINTN) SmiEnAddr, SmiEnSave);
      }
    }
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
  EFI_STATUS                             Status;

  FillBootOptionListFromCfgData (OsBootOptionList);

  if ((OsBootOptionList != NULL)
    && (OsBootOptionList->OsBootOptionCount < PcdGet32(PcdOsBootOptionNumber))
    && (FeaturePcdGet(PcdEnableSetup))) {
      OsBootOptionList->OsBootOption[OsBootOptionList->OsBootOptionCount].DevType = OsBootDeviceMemory;
      OsBootOptionList->OsBootOption[OsBootOptionList->OsBootOptionCount].FsType = EnumFileSystemTypeAuto;
      AsciiStrCpyS ((CHAR8*)OsBootOptionList->OsBootOption[OsBootOptionList->OsBootOptionCount].Image[0].FileName, MAX_FILE_PATH_LEN, "!SETP/MPYM:STPY");
      OsBootOptionList->OsBootOptionCount++;
  }

  Status = FusaConfigBootOption(OsBootOptionList);
  DEBUG((DEBUG_INFO, "FusaConfigBootOption Status %r\n", Status));

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
  if (PcdGetBool (PcdIntelGfxEnabled)) {
    DEBUG ((DEBUG_INFO, "Updating FB \n"));
    GfxInfo->FrameBufferBase = PciRead32 (PCI_LIB_ADDRESS (IGD_BUS_NUM, IGD_DEV_NUM, IGD_FUN_NUM, 0x18)) & 0xFFFFFF00;
    GfxInfo->FrameBufferBase |= LShiftU64(PciRead32 (PCI_LIB_ADDRESS (IGD_BUS_NUM, IGD_DEV_NUM, IGD_FUN_NUM, 0x1C)),32);
    GfxInfo->FrameBufferBase += SIZE_8MB;
  }
  DEBUG ((DEBUG_INFO, "GfxDeviceInfo Frame Buffer =0x%llx\n", GfxInfo->FrameBufferBase));

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
  LdrSmmInfo->SmiCtrlReg.SmiGblPos  = (UINT8)HighBitSet32 (B_ACPI_IO_SMI_EN_GBL_SMI_EN);
  LdrSmmInfo->SmiCtrlReg.SmiApmPos  = (UINT8)HighBitSet32 (B_ACPI_IO_SMI_EN_APMC_EN);
  LdrSmmInfo->SmiCtrlReg.SmiEosPos  = (UINT8)HighBitSet32 (B_ACPI_IO_SMI_EN_EOS);
  LdrSmmInfo->SmiCtrlReg.Address    = (UINT32)(ACPI_BASE_ADDRESS + R_ACPI_IO_SMI_EN);

  //
  // Update smi status register data
  //
  LdrSmmInfo->SmiStsReg.RegType    = (UINT8)REG_TYPE_IO;
  LdrSmmInfo->SmiStsReg.RegWidth   = (UINT8)WIDE32;
  LdrSmmInfo->SmiStsReg.SmiApmPos  = (UINT8)HighBitSet32 (B_ACPI_IO_SMI_EN_APMC_EN);
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
  UINT8                   *End;
  UINT32                  *Signature;
  UINT32                   *Base;
  UINT16                   *Size;

  CpuNvs = (CPU_NVS_AREA *) &GlobalNvs->CpuNvs;
  End = (UINT8 *) Table + Table->Length;

  // Make sure we stop searching when there's not enough room left for the entire PNVS structure
  for (CurrPtr = (UINT8 *) Table;
       IsPtrRangeValid (CurrPtr, 1 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1 + sizeof (UINT16), End);
       CurrPtr++)
  {
    Signature = (UINT32 *) (CurrPtr + 1);
    if (*CurrPtr != AML_EXT_REGION_OP)
       continue;

    ///
    /// Update the CPU GlobalNvs area
    ///
    if (*Signature == SIGNATURE_32('P', 'N', 'V', 'S')) {
      Base = (UINT32 *) (CurrPtr + 1 + sizeof (*Signature) + 2);
      Size = (UINT16 *) (CurrPtr + 1 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1);
      ASSERT (*Base == 0xFFFF0000);
      ASSERT (*Size == 0xAA55);
      ///
      /// Cpu Nvs Area address
      ///
      DEBUG ((DEBUG_INFO, "CPU PNVS Base Old=0x%08X New=0x%08X\n", *Base, CpuNvs));
      *(UINT32 *) Base = (UINT32) (UINTN) CpuNvs;
      ///
      /// Cpu Nvs Area size
      ///
      DEBUG ((DEBUG_INFO, "CPU PNVS Size Old=0x%04X New=0x%04X\n", *Size, sizeof (CPU_NVS_AREA)));
      *(UINT16 *) Size = sizeof (CPU_NVS_AREA);
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
EFIAPI
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
  @retval Others            Errors during updating PSS table

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
  for (Lpss = NULL, Tpss = NULL; IsPtrRangeValid (Ptr, sizeof (UINT32), End); Ptr++) {
    if (Ptr == (UINT8 *)Table) {
      continue;
    }
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
  @param[in] Flags        The ACPI_LPI_STATE_FLAGS
  @param[in] Address      Residency counter address
  @param[in] Frequency    Residency counter freqency

  **/
VOID
UpdLpiStat (
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
  Update ACPI Various Non volatile storage address in DSDT

  @param[in]  Table        Pointer to ACPI table
  @param[in]  GlobalNvs    Pointer to platform global NVS data

  @retval

**/
static
VOID
UpdateXNvsAddress (
  IN EFI_ACPI_DESCRIPTION_HEADER *Table,
  IN GLOBAL_NVS_AREA             *GlobalNvs
  )
{
  UINT8                       *Ptr;
  UINT8                       *End;
  UINT32                       Base;
  UINT16                       Size;
  Ptr  = (UINT8 *)Table;
  End  = (UINT8 *)Table + Table->Length;

  for (; IsPtrRangeValid (Ptr, sizeof (UINT32), End); Ptr++) {
    if ((Ptr == (UINT8 *)Table) || (*(Ptr-1) != AML_NAME_OP))
      continue;

    switch (*(UINT32 *)Ptr) {
      // PCH NVS address
      case SIGNATURE_32('P', 'N', 'V', 'B'):
        if (!IsPtrRangeValid (Ptr, 5 + sizeof (UINT32), End)) {
          break;
        }
        Base = (UINT32)(UINTN)&GlobalNvs->PchNvs;
        DEBUG((DEBUG_INFO, "PNVB Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 5), Base));
        *(UINT32 *)(Ptr + 5) = Base;
        break;

      case SIGNATURE_32('P', 'N', 'V', 'L'):
        if (!IsPtrRangeValid (Ptr, 5 + sizeof (UINT16), End)) {
          break;
        }
        Size = sizeof(PCH_NVS_AREA);
        DEBUG((DEBUG_INFO, "PNVL Old=0x%08X New=0x%08X\n", *(UINT16 *)(Ptr + 5), Size));
        *(UINT16 *)(Ptr + 5) = Size;
        break;

      //Update IGPU NVS Address
      case SIGNATURE_32('I', 'G', 'N', 'B'):
        if (!IsPtrRangeValid (Ptr, 5 + sizeof (UINT32), End)) {
          break;
        }
        Base = (UINT32)(UINTN)&GlobalNvs->IgpuNvs;
        DEBUG((DEBUG_INFO, "IGNB Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 5), Base));
        *(UINT32 *)(Ptr + 5) = Base;
        break;

      case SIGNATURE_32('I', 'G', 'N', 'L'):
        if (!IsPtrRangeValid (Ptr, 5 + sizeof (UINT16), End)) {
          break;
        }
        Size = sizeof(IGPU_NVS_AREA);
        DEBUG((DEBUG_INFO, "IGNL Old=0x%08X New=0x%08X\n", *(UINT16 *)(Ptr + 5), Size));
        *(UINT16 *)(Ptr + 5) = Size;
        break;

      // Update TCSS Nvs Address
      case SIGNATURE_32('T', 'C', 'N', 'B'):
        if (!IsPtrRangeValid (Ptr, 5 + sizeof (UINT32), End)) {
          break;
        }
        Base = (UINT32)(UINTN)&GlobalNvs->TcssNvs;
        DEBUG((DEBUG_INFO, "TCNB Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 5), Base));
        *(UINT32 *)(Ptr + 5) = Base;
        break;

      case SIGNATURE_32('T', 'C', 'N', 'L'):
        if (!IsPtrRangeValid (Ptr, 5 + sizeof (UINT16), End)) {
          break;
        }
        Size = sizeof(TCSS_NVS_AREA);
        DEBUG((DEBUG_INFO, "TCNL Old=0x%08X New=0x%08X\n", *(UINT16 *)(Ptr + 5), Size));
        *(UINT16 *)(Ptr + 5) = Size;
        break;

      // Update Host Bridge Nvs Address
      case SIGNATURE_32('H', 'B', 'N', 'B'):
        if (!IsPtrRangeValid (Ptr, 5 + sizeof (UINT32), End)) {
          break;
        }
        Base = (UINT32)(UINTN)&GlobalNvs->HostBridgeNvs;
        DEBUG((DEBUG_INFO, "HBNB Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 5), Base));
        *(UINT32 *)(Ptr + 5) = Base;
        break;

      case SIGNATURE_32('H', 'B', 'N', 'L'):
        if (!IsPtrRangeValid (Ptr, 5 + sizeof (UINT16), End)) {
          break;
        }
        Size = sizeof(HOST_BRIDGE_NVS_AREA);
        DEBUG((DEBUG_INFO, "HBNL Old=0x%08X New=0x%08X\n", *(UINT16 *)(Ptr + 5), Size));
        *(UINT16 *)(Ptr + 5) = Size;
        break;

      // Update IPU Nvs Address
      case SIGNATURE_32('I', 'P', 'N', 'B'):
        if (!IsPtrRangeValid (Ptr, 5 + sizeof (UINT32), End)) {
          break;
        }
        Base = (UINT32)(UINTN)&GlobalNvs->IpuNvs;
        DEBUG((DEBUG_INFO, "IPNB Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 5), Base));
        *(UINT32 *)(Ptr + 5) = Base;
        break;

      case SIGNATURE_32('I', 'P', 'N', 'L'):
        if (!IsPtrRangeValid (Ptr, 5 + sizeof (UINT16), End)) {
          break;
        }
        Size = sizeof(IPU_NVS_AREA);
        DEBUG((DEBUG_INFO, "IPNL Old=0x%08X New=0x%08X\n", *(UINT16 *)(Ptr + 5), Size));
        *(UINT16 *)(Ptr + 5) = Size;
        break;

      default:
        break;
    }
  }
}

/**
  Check if the RTD3 ACPI table matches the board's expected signature.

  @param[in] Table        Pointer to the ACPI table header

  @retval TRUE            Table matches the expected RTD3 signature for this platform
  @retval FALSE           Table does not match and should not be loaded
**/
BOOLEAN
IsRtd3SignatureMatched (
  IN EFI_ACPI_DESCRIPTION_HEADER *Table
  )
{
  UINT8 PlatformId;

  PlatformId = (UINT8) GetPlatformId();
  switch (PlatformId) {
    case PLATFORM_ID_PTL_UH_DDR5_EMB_CRB:
      return Table->OemTableId == SIGNATURE_64 ('P', 't', 'l', 'C', 'r', 'b', 0, 0);
    case PLATFORM_ID_PTL_UH_DDR5_T3_RVP:
    case PLATFORM_ID_PTL_UH_LP5X_T4_RVP:
      return Table->OemTableId == SIGNATURE_64 ('P', 't', 'l', 'R', 'v', 'p', '3', '4');
    case PLATFORM_ID_PTL_UH_LP5X_Robinson:
    case PLATFORM_ID_PTL_UH_LP5X_T3_RVP:
      return Table->OemTableId == SIGNATURE_64 ('P', 't', 'l', 'U', 'H', 'R', 'v', 'p');
    case PLATFORM_ID_WCL_DDR5_CRB:
      return Table->OemTableId == SIGNATURE_64 ('W', 'c', 'l', 'N', 'C', 'r', 'b', 0);
    case PLATFORM_ID_WCL_DDR5_RVP:
      return Table->OemTableId == SIGNATURE_64 ('W', 'c', 'l', 'N', 'R', 'v', 'p', '1');
    case PLATFORM_ID_WCL_LP5X_RVP:
      return Table->OemTableId == SIGNATURE_64 ('W', 'c', 'l', 'N', 'R', 'v', 'p', '2');
    default:
      return Table->OemTableId == SIGNATURE_64 ('P', 't', 'l', 'R', 'v', 'p', '3', '4');
  }
}

/**
  Update the MCFG table with platform-specific configuration data.

  This serves as the main entry point for building the MCFG table at runtime.
  It initializes the table header and adds configuration space structures
  based on platform configuration.

  @param[in, out] AcpiTable   Pointer to the ACPI table buffer.

**/
VOID
McfgTableUpdate (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER   *AcpiTable
  )
{
  EFI_STATUS                      Status;

  // Initialize the table header
  Status = AddAcpiMcfgHdr (AcpiTable);
  if (EFI_ERROR (Status)) {
    return;
  }

  // Add configuration space structure for single root complex.
  AddMcfgConfigSpaceStruct (
                            AcpiTable,
                            PcdGet64(PcdPciExpressBaseAddress), // BaseAddress - to be updated by platform
                            0,                // PciSegmentGroupNumber
                            0,                // StartBusNumber
                            0xFF              // EndBusNumber
                            );
}

/**
  Update the MADT table

  @param[in, out] AcpiHeader         - The MADT table header to update
**/
VOID
MadtTableUpdate (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER *AcpiHeader
  )
{
  EFI_STATUS                         Status;

  Status = AddAcpiMadtHdr (AcpiHeader, LOCAL_APIC_BASE_ADDRESS, EFI_ACPI_6_4_PCAT_COMPAT);
  if (EFI_ERROR (Status)) {
    return;
  }

  // Add all Local APICs
  AddMadtAllLocalApics(AcpiHeader);

  // Add IO APIC entry
  AddMadtIoApic (AcpiHeader, ICH_IOAPIC_ID, IO_APIC_BASE_ADDRESS, 0x18 * 0);
  AddMadtIntSrcOverride (AcpiHeader, 0, 0, 2, 0);   // IRQ0=>IRQ2
  AddMadtIntSrcOverride (AcpiHeader, 0, 9, 9, 0xD); // SCI Active High

  // NMI Entry for all processors, level triggered, active high, on LINT 1
  AddLocalX2ApicNmi (AcpiHeader, 0xD, 0xFFFFFFFF, 1);
}

/**
  Update the DMAR table

  @param[in, out] AcpiHeader         - The DMAR table header to update
**/
VOID
DmarTableUpdate (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER *AcpiHeader
  )
{
  EFI_STATUS                         Status;
  MEMORY_CFG_DATA                    *MemCfgData;
  SILICON_CFG_DATA                   *SiCfgData;
  UINT8                              Flags;
  UINT64                             BaseAddress;
  EFI_ACPI_DMAR_STRUCTURE_HEADER     *DmarHdr;

  Flags = 0;
  SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if ((SiCfgData != NULL) && (SiCfgData->InterruptRemappingSupport != 0)) {
    Flags |= BIT0;
  }

  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);
  if (MemCfgData != NULL) {
    if (MemCfgData->X2ApicOptOut == 1) {
      Flags |= BIT1;
    } else {
      Flags &= 0xFD;
    }
    /// Set DMA_CONTROL_GUARANTEE bit (BIT 2) if Dma Control Guarantee is supported
    if (MemCfgData->DmaControlGuarantee == 1) {
      Flags |= BIT2;
    }
  }

  Status = AddAcpiDmarHdr (AcpiHeader, Flags);
  if (EFI_ERROR (Status)) {
    return;
  }
  // Update Host Address Width for MKTME support
  ((EFI_ACPI_DMAR_HEADER *)AcpiHeader)->HostAddressWidth = MAX_PHYSICAL_ADDRESS_SIZE_WITH_MKTME - 1;

  BaseAddress = ReadVtdBaseAddress(GFX_VTD);
  Flags   = 0;
  DmarHdr = AddDrhdHdr (AcpiHeader, Flags, SIZE_64KB, 0, BaseAddress);
  ASSERT (DmarHdr != NULL);
  AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0, 0, 0, IGD_DEV_NUM, IGD_FUN_NUM);

  BaseAddress = ReadVtdBaseAddress(NON_GFX_VTD);
  if (IsVtdEngineEnabled(NON_GFX_VTD)) {
    DEBUG ((DEBUG_INFO, "%a NON_GFX_VTD Enabled! \n", __FUNCTION__));

    DmarHdr = AddDrhdHdr (AcpiHeader, Flags, SIZE_64KB, 0, BaseAddress);
    ASSERT (DmarHdr != NULL);
    AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0, 0, 0, 5, 0);
    AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0, 0, 0, 0xA, 0);
    AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0, 0, 0, 0xB, 0);
  }

  BaseAddress = ReadVtdBaseAddress(IOP_VTD);
  DmarHdr = AddDrhdHdr (AcpiHeader, 1, SIZE_64KB, 0, BaseAddress);
  ASSERT (DmarHdr != NULL);
  AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_IOAPIC, 0, 2, V_P2SB_CFG_IBDF_BUS, V_P2SB_CFG_IBDF_DEV, V_P2SB_CFG_IBDF_FUNC);
  AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_MSI_CAPABLE_HPET, 0, 0, V_P2SB_CFG_HBDF_BUS, V_P2SB_CFG_HBDF_DEV, V_P2SB_CFG_HBDF_FUNC);

  DmarHdr = AddSatcHdr (AcpiHeader, 1, 0);
  ASSERT (DmarHdr != NULL);
  AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0, 0, 0, 2, 0);
  AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0, 0, 0, 5, 0);
  AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0, 0, 0, 0xB, 0);

  DmarHdr = AddSidpHdr (AcpiHeader, 0);
  ASSERT (DmarHdr != NULL);
  AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0x1F, 0, 0, 2, 0);
  AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0x1F, 0, 0, 5, 0);
  AddScopeData (AcpiHeader, DmarHdr, EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_PCI_ENDPOINT, 0x1C, 0, 0, 0xB, 0);

  // Calculate DMAR table checksum
  AcpiHeader->Checksum = CalculateCheckSum8 ((UINT8 *)AcpiHeader, AcpiHeader->Length);
}

/**
  Update PCH NVS and SA NVS area address and size in ACPI table.

  @param[in] Current    Pointer to ACPI description header

  @retval    EFI_SUCCESS   Operation completed successfully.
  @retval    EFI_INVALID_PARAMETER Invalid parameter.

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
  SILICON_CFG_DATA            *SiCfgData;

  GlobalNvs  = (GLOBAL_NVS_AREA *)(UINTN) PcdGet32 (PcdAcpiGnvsAddress);

  Table = (EFI_ACPI_DESCRIPTION_HEADER *) Current;
  Ptr  = (UINT8 *)Table;
  End  = (UINT8 *)Table + Table->Length;

  if (Table->Signature == EFI_ACPI_5_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
    UpdateXNvsAddress (Table, GlobalNvs);
  } else if (Table->Signature == SIGNATURE_32 ('N', 'H', 'L', 'T')) {
    GlobalNvs->PchNvs.NHLA[0]  = (UINT64)(UINTN) Table;
    GlobalNvs->PchNvs.NHLL[0]  = Table->Length;
    DEBUG ((DEBUG_INFO, "NHLT Base 0x%08X, Size 0x%08X\n", (UINT32)(UINTN)GlobalNvs->PchNvs.NHLA[0], GlobalNvs->PchNvs.NHLL[0]));
  } else if (Table->OemTableId == SIGNATURE_64 ('C', 'p', 'u', 'S', 's', 'd', 't', 0)) {
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
          Address = PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_PS_ON_RESIDENCY;
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
  } else if (Table->Signature == EFI_ACPI_6_4_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE &&
    // Check first four byte of OemID if it is equal to Rtd3
    CompareMem (&Table->OemId,  "Rtd3", 4) == 0) {
    DEBUG ((DEBUG_INFO, "Found Rtd3 Table\n"));
    if (IsRtd3SignatureMatched (Table)) {
      DEBUG((DEBUG_INFO, "Successfully loaded Current Rtd3 Table\n"));
      return EFI_SUCCESS;
    } else {
      DEBUG((DEBUG_INFO, "Skipped loading current Rtd3 table due to unmatched table signature\n"));
      return  EFI_UNSUPPORTED;
    }
  } else if (Table->Signature == EFI_ACPI_6_4_DMA_REMAPPING_TABLE_SIGNATURE) {
    DEBUG ((DEBUG_INFO, "Updated DMAR Table entries\n"));
    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
    if (FeaturePcdGet (PcdVtdEnabled) && (PlatformData != NULL) && (PlatformData->PlatformFeatures.VtdEnable == 1)) {
      DmarTableUpdate (Table);
    } else {
      return EFI_UNSUPPORTED;
    }
  } else if (Table->Signature == EFI_ACPI_6_4_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE) {
    DEBUG ((DEBUG_INFO, "Updating MADT Table entries\n"));
    MadtTableUpdate (Table);
  } else if (Table->Signature == EFI_ACPI_6_4_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE) {
    DEBUG ((DEBUG_INFO, "Updating MCFG Table entries\n"));
    McfgTableUpdate (Table);
    // McfgTableUpdate appends the config space entries, so re-derive the end from the grown table.
    End   = (UINT8 *)Table + Table->Length;
    MmCfg = (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE *)
            ((EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER *)Ptr + 1);
    Base  = 0;
    while (IsPtrRangeValid ((UINT8 *)MmCfg, sizeof (*MmCfg), End)) {
      MmCfg->BaseAddress = PcdGet64 (PcdPciExpressBaseAddress) + Base;
      Base += 0x10000000;
      MmCfg++;
    }
  }

  if (MEASURED_BOOT_ENABLED()) {
    if ((Table->Signature  == EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE) ||
        (Table->OemTableId == ACPI_SSDT_TPM2_DEVICE_OEM_TABLE_ID)) {
      Status = UpdateTpm2AcpiTable(Table);
      ASSERT_EFI_ERROR (Status);
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
          UpdateAcpiPsdTable ( (VOID* )Current);
          DEBUG ( (DEBUG_INFO, "Updated Psd Table in AcpiTable Entries\n") );
        }
      }
    }
  }

  SiCfgData = (SILICON_CFG_DATA *)FindConfigDataByTag (CDATA_SILICON_TAG);
  if ((Table->Signature == EFI_ACPI_5_0_EMBEDDED_CONTROLLER_BOOT_RESOURCES_TABLE_SIGNATURE) &&
     ((SiCfgData != NULL) && (SiCfgData->EcAvailable == 0))) {
      DEBUG ((DEBUG_INFO, "Skip ECDT table update.\n"));
    return EFI_UNSUPPORTED;
  }

  //
  // EC SSDT
  //
  if (Table->Signature == EFI_ACPI_6_4_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE &&
      Table->OemTableId == SIGNATURE_64 ('E', 'c', 'S', 's', 'd', 't', ' ', 0)) {
    if ((SiCfgData != NULL) && (SiCfgData->EcAvailable == 0)) {
      DEBUG ((DEBUG_INFO, "Skip EcSsdt table update.\n"));
      return EFI_UNSUPPORTED;
    }
    DEBUG((DEBUG_INFO, "Found EcSsdt\n"));
    for (; IsPtrRangeValid (Ptr, sizeof (UINT32), End); Ptr++) {
      if ((*(UINT32 *)Ptr == SIGNATURE_32 ('E','N','V','S')) && IsPtrRangeValid (Ptr, 11 + sizeof (UINT16), End)) {
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

  //
  // Tcss SSDT
  //
  if (Table->Signature == EFI_ACPI_6_4_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE &&
      Table->OemTableId == SIGNATURE_64 ('T', 'c', 's', 's', 'S', 's', 'd', 't')) {
    DEBUG((DEBUG_INFO, "Found TcssSsdt\n"));
    for (; IsPtrRangeValid (Ptr, sizeof (UINT32), End); Ptr++) {
      if ((*(UINT32 *)Ptr == SIGNATURE_32 ('I','T','N','B')) && IsPtrRangeValid (Ptr, 5 + sizeof (UINT32), End)) {
        Base = (UINT32) (UINTN) &GlobalNvs->ItbtNvs;
        DEBUG ((DEBUG_INFO, "ITNV Base Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 5), Base));
        *(UINT32 *)(Ptr + 5) = Base;
      }
      if ((*(UINT32 *)Ptr == SIGNATURE_32 ('I','T','N','L')) && IsPtrRangeValid (Ptr, 5 + sizeof (UINT16), End)) {
        Size = sizeof(ITBT_NVS_AREA);
        DEBUG ((DEBUG_INFO, "ITNV Size Old=0x%04X New=0x%04X\n", *(UINT16 *)(Ptr + 5), Size));
        *(UINT16 *)(Ptr + 5) = Size;
        break;
      }
    }
  }

  //
  // Ucsi SSDT
  //
  if (Table->Signature == EFI_ACPI_6_4_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE &&
      Table->OemTableId == SIGNATURE_64 ('U', 'c', 's', 'i', 'T', 'a', 'b', 'l')) {
    DEBUG((DEBUG_INFO, "Found UcsiSsdt\n"));
    for (; IsPtrRangeValid (Ptr, sizeof (UINT32), End); Ptr++) {
      if ((*(UINT32 *)Ptr == SIGNATURE_32 ('U','P','N','V')) && IsPtrRangeValid (Ptr, 11 + sizeof (UINT16), End)) {
        Base = (UINT32) (UINTN) &GlobalNvs->UcsiNvs;
        DEBUG ((DEBUG_INFO, "UPNV Base Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 6), Base));
        *(UINT32 *)(Ptr + 6) = Base;

        Size = sizeof (UCSI_NVS_AREA);
        DEBUG ((DEBUG_INFO, "UPNV Size Old=0x%04X New=0x%04X\n", *(UINT16 *)(Ptr + 11), Size));
        *(UINT16 *)(Ptr + 11) = Size;
        break;
      }
    }
  }

  //
  // Pepo SSDT
  //
  if (Table->Signature == EFI_ACPI_6_4_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE &&
      Table->OemTableId == SIGNATURE_64 ('P', 'e', 'p', 'o', 'S', 's', 'd', 't')) {
    DEBUG((DEBUG_INFO, "Found PepoSsdt\n"));
    for (; IsPtrRangeValid (Ptr, sizeof (UINT32), End); Ptr++) {
      if ((*(UINT32 *)Ptr == SIGNATURE_32 ('P','T','N','V')) && IsPtrRangeValid (Ptr, 11 + sizeof (UINT16), End)) {
        Base = (UINT32) (UINTN) &GlobalNvs->PepNvs;
        DEBUG ((DEBUG_INFO, "PTNV Base Old=0x%08X New=0x%08X\n", *(UINT32 *)(Ptr + 6), Base));
        *(UINT32 *)(Ptr + 6) = Base;

        Size = sizeof (PEP_NVS_AREA);
        DEBUG ((DEBUG_INFO, "PTNV Size Old=0x%04X New=0x%04X\n", *(UINT16 *)(Ptr + 11), Size));
        *(UINT16 *)(Ptr + 11) = Size;
        break;
      }
    }
  }

  //
  // TXT SSDT
  //
  if (Table->Signature == EFI_ACPI_6_4_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE &&
      Table->OemTableId == SIGNATURE_64 ('T', 'x', 't', 'S', 's', 'd', 't', 0)) {
    FeaturesCfgData = (FEATURES_CFG_DATA *)FindConfigDataByTag (CDATA_FEATURES_TAG);
    if ((FeaturesCfgData == NULL) || (FeaturesCfgData->Features.TxtEnabled != 1)) {
      DEBUG ((DEBUG_INFO, "TXT SSDT: TxtEnabled not set in config, skipping table installation\n"));
      return EFI_UNSUPPORTED;
    }
    // Install TXT SSDT table when TxtEnabled is set in configuration
    DEBUG ((DEBUG_INFO, "TXT SSDT: Installing ACPI table \n"));
  }

  if (Table->Signature == EFI_ACPI_6_4_BOOT_ERROR_RECORD_TABLE_SIGNATURE) {
    Status = UpdateCrashLogBertTable((EFI_ACPI_6_4_BOOT_ERROR_RECORD_TABLE_HEADER*)Current);
    DEBUG ( (DEBUG_INFO, "Updated BERT Table in AcpiTable Entries %r\n", Status) );
    // If error updating BERT, don't install the table
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  if (Table->Signature == DTPR_ACPI_TABLE_SIGNATURE) {
    DEBUG ((DEBUG_INFO, "Updating DTPR Table with runtime data\n"));
    Status = UpdateDtprAcpiTable (Table);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Updated DTPR Table Failed %r\n", Status));
      return Status;
    }
    DEBUG ((DEBUG_INFO, "Updated DTPR Table Success\n"));
  }

  return EFI_SUCCESS;
}

/**
 Update CPU NVS from CpuInitDataHob

 @param[in, out]  CpuNvs      Pointer to CPU NVS region

**/
VOID
UpdateCpuNvs (
  IN OUT CPU_NVS_AREA        *CpuNvs
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
    DEBUG ((DEBUG_ERROR, "CPU Init Data HOB not available\n"));
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
  CpuSku = GetCpuSku ();
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
    CpuNvs->CDPowerValue = C10_POWER;
    CpuNvs->CDMwaitValue = MWAIT_CD_2;
    if (C10Latency != 0) {
      CpuNvs->CDLatency = C10Latency;
    }
  }
  else if(CpuNvs->PpmFlags & PPM_C9) { // C9 supported
    CpuNvs->CDPowerValue = C9_POWER;
    CpuNvs->CDMwaitValue = MWAIT_CD_1;
    if (C9Latency != 0) {
      CpuNvs->CDLatency = C9Latency;
    }
  }
  else if (CpuNvs->PpmFlags & PPM_C8) { // C8 supported
    CpuNvs->CDPowerValue = C8_POWER;
    CpuNvs->CDMwaitValue = MWAIT_CD;
    if (C8Latency != 0) {
      CpuNvs->CDLatency = C8Latency;
    }
  }

  if (CpuNvs->PpmFlags & PPM_HWP) {
    CpuNvs->HwpEnable = 1;
  }

  if (CpuNvs->PpmFlags & PPM_TURBO) {
    CpuNvs->TurboMode = 1;
  }

  CpuNvs->CtdpLevelsSupported = CpuInitDataHob->CtdpLevelsSupported;                        /// ConfigTdp Number Of Levels.
  CpuNvs->ConfigTdpBootModeIndex = CpuInitDataHob->ConfigTdpBootModeIndex;                             /// CTDP Levels TAR
  CpuNvs->CtdpPowerLimit1 = CpuInitDataHob->CtdpPowerLimit1;             /// CTDP Levels Power Limit1
  CpuNvs->CtdpPowerLimit2 = CpuInitDataHob->CtdpPowerLimit2;             /// CTDP Levels Power Limit2
  CpuNvs->CtdpPowerLimitWindow = CpuInitDataHob->CtdpPowerLimitWindow;    /// CTDP Levels Power Limit1 Time Window
  CpuNvs->ConfigurablePpc = CpuInitDataHob->ConfigurablePpc;                             /// Max ratio of the slowest core
  CpuNvs->EnableRp        = CpuPowerMgmtDataHob->EnableRp;                               /// Enable/Disable Resource Priority Feature
  DEBUG (( DEBUG_INFO, "CPU NVS: Resource Priority Feature = %d\n", CpuNvs->EnableRp));

  CpuNvs->ThETAIbattEnable = CpuInitDataHob->ThETAIbattEnable;             /// CTDP Levels Power Limit1
  CpuNvs->IsysLimitL1Enable = CpuInitDataHob->IsysLimitL1Enable;             /// CTDP Levels Power Limit2
  CpuNvs->IsysLimitL2Enable = CpuInitDataHob->IsysLimitL2Enable;
  CpuNvs->HeteroStatus    = IsHeteroCoreSupported();

  AsmCpuid (CPUID_PROCESSOR_FREQUENCY, NULL, NULL, &ProcessorFreq.Uint32, NULL);
  CpuNvs->BusFrequency = (UINT16) ProcessorFreq.Bits.BusFrequency;

  PlatformInfoMsr.Uint64 = AsmReadMsr64 (MSR_PLATFORM_INFO);
  CpuNvs->MaxP1Ratio     = (UINT8) PlatformInfoMsr.Bits.MaxNonTurboLimRatio;

  CpuNvs->DomainIndex[0] = 0x0;
  CpuNvs->DomainIndex[1] = 0x1;
  CpuNvs->DomainIndex[2] = 0x3;
  CpuNvs->DomainIndex[3] = 0x3;
  CpuNvs->DomainIndex[4] = 0x3;
  CpuNvs->DomainIndex[5] = 0x3;
  CpuNvs->DomainIndex[6] = 0x8;
  CpuNvs->DomainIndex[7] = 0x8;
  CpuNvs->DomainIndex[8] = 0x8;
  CpuNvs->DomainIndex[9] = 0x8;
  CpuNvs->CoreCountInDomain[0] = 0x1;
  CpuNvs->CoreCountInDomain[1] = 0x1;
  CpuNvs->CoreCountInDomain[2] = 0x4;
  CpuNvs->CoreCountInDomain[3] = 0x4;
  CpuNvs->CoreCountInDomain[4] = 0x4;
  CpuNvs->CoreCountInDomain[5] = 0x4;
  CpuNvs->CoreCountInDomain[6] = 0x4;
  CpuNvs->CoreCountInDomain[7] = 0x4;
  CpuNvs->CoreCountInDomain[8] = 0x4;
  CpuNvs->CoreCountInDomain[9] = 0x4;
  CpuNvs->P1Ratio[0] = 0x14;
  CpuNvs->P1Ratio[1] = 0x14;
  CpuNvs->P1Ratio[2] = 0x10;
  CpuNvs->P1Ratio[3] = 0x10;
  CpuNvs->P1Ratio[4] = 0x10;
  CpuNvs->P1Ratio[5] = 0x10;
  CpuNvs->P1Ratio[6] = 0x10;
  CpuNvs->P1Ratio[7] = 0x10;
  CpuNvs->P1Ratio[8] = 0x10;
  CpuNvs->P1Ratio[9] = 0x10;
  CpuNvs->IpcScaling[0] = 0x70;
  CpuNvs->IpcScaling[1] = 0x70;
  CpuNvs->IpcScaling[2] = 0x64;
  CpuNvs->IpcScaling[3] = 0x64;
  CpuNvs->IpcScaling[4] = 0x64;
  CpuNvs->IpcScaling[5] = 0x64;
  CpuNvs->IpcScaling[6] = 0x64;
  CpuNvs->IpcScaling[7] = 0x64;
  CpuNvs->IpcScaling[8] = 0x64;
  CpuNvs->IpcScaling[9] = 0x64;

  DEBUG ((DEBUG_INFO, "CPU NVS: BusFrequency / MaxP1Ratio = %d / %d\n",
                 CpuNvs->BusFrequency, CpuNvs->MaxP1Ratio));
  DEBUG ((DEBUG_INFO, "Update Cpu Nvs Done\n"));
}

/**
 Update TCSS NVS

 @param[in, out]  TcssNvs  Pointer to TCSS NVS region

**/
VOID
UpdateTcssNvs (
  IN OUT TCSS_NVS_AREA    *TcssNvs
  )
{
  MEMORY_CFG_DATA         *MemCfgData;
  MemCfgData = (MEMORY_CFG_DATA *) FindConfigDataByTag (CDATA_MEMORY_TAG);
  if (MemCfgData != NULL) {
    TcssNvs->ItbtXhciEn = MemCfgData->TcssXhciEn;
  }

  TcssNvs->ItbtDmaEn[0] = 0x1;
  TcssNvs->ItbtDmaEn[1] = 0x1;
  TcssNvs->ItbtPcieRpEn[0] = 0x1;
  TcssNvs->ItbtPcieRpEn[1] = 0x1;
  TcssNvs->ItbtPcieRpEn[2] = 0x1;
  TcssNvs->ItbtPcieRpAddress[0] = 0x70000;
  TcssNvs->ItbtPcieRpAddress[1] = 0x70001;
  TcssNvs->ItbtPcieRpAddress[2] = 0x70002;
  TcssNvs->ItbtPcieRpAddress[3] = 0x70003;
  TcssNvs->TcssRtd3      = 0x1;
  TcssNvs->LtrEnable[0]  = 0x1;
  TcssNvs->LtrEnable[1]  = 0x1;
  TcssNvs->LtrEnable[2]  = 0x1;
  TcssNvs->LtrEnable[3]  = 0x1;
  TcssNvs->PcieLtrMaxSnoopLatency[0] = 0x100F;
  TcssNvs->PcieLtrMaxSnoopLatency[1] = 0x100F;
  TcssNvs->PcieLtrMaxSnoopLatency[2] = 0x100F;
  TcssNvs->PcieLtrMaxSnoopLatency[3] = 0x100F;
  TcssNvs->PcieLtrMaxNoSnoopLatency[0] = 0x100F;
  TcssNvs->PcieLtrMaxNoSnoopLatency[1] = 0x100F;
  TcssNvs->PcieLtrMaxNoSnoopLatency[2] = 0x100F;
  TcssNvs->PcieLtrMaxNoSnoopLatency[3] = 0x100F;
  TcssNvs->IomReady = 0x1;
  TcssNvs->TcssIomVccSt = 0x1;
  TcssNvs->RegBarOffset = 0x5400;
  TcssNvs->IomBase = 0x4010800000;
  TcssNvs->RetimerMap = 0x0F;
}

/**
 Update UCSI NVS

 @param[in, out]  UcsiNvs    Pointer to UCSI NVS region

**/
VOID
UpdateUcsiNvs (
  IN OUT UCSI_NVS_AREA       *UcsiNvs
  )
{
  GLOBAL_NVS_AREA            *GlobalNvs;

  GlobalNvs  = (GLOBAL_NVS_AREA *)(UINTN) PcdGet32 (PcdAcpiGnvsAddress);
  UcsiNvs->UcsiOpBaseAddr = (UINT32)(UINTN)&GlobalNvs->OpmBuffer;

  UcsiNvs->TypeCPortsMap = 0xF;
  UcsiNvs->ConnectorGroupPosition[0] = 0x20;
  UcsiNvs->ConnectorGroupPosition[1] = 0x21;
  UcsiNvs->ConnectorGroupPosition[2] = 0x22;
  UcsiNvs->ConnectorGroupPosition[3] = 0x0;
  UcsiNvs->ConnectorVisibility[0] = 0x1;
  UcsiNvs->ConnectorVisibility[1] = 0x1;
  UcsiNvs->ConnectorVisibility[2] = 0x1;
  UcsiNvs->ConnectorVisibility[3] = 0x0;
  UcsiNvs->ConnectorConnectable[0] = 0x1;
  UcsiNvs->ConnectorConnectable[1] = 0x1;
  UcsiNvs->ConnectorConnectable[2] = 0x1;
  UcsiNvs->ConnectorConnectable[3] = 0x0;
  UcsiNvs->ConnectorType[0] = 0x9;
  UcsiNvs->ConnectorType[1] = 0x9;
  UcsiNvs->ConnectorType[2] = 0x9;
  UcsiNvs->ConnectorType[3] = 0x0;
  UcsiNvs->ConnectorCapability[0] = 0x3D;
  UcsiNvs->ConnectorCapability[1] = 0x3D;
  UcsiNvs->ConnectorCapability[2] = 0x3D;
  UcsiNvs->ConnectorCapability[3] = 0x0;

  GlobalNvs->OpmBuffer.Version = 0x0200;
}

/**
 Update Itbt NVS

 @param[in, out]  ItbtNvs      Pointer to Itbt NVS region

**/
VOID
UpdateItbtNvs (
  IN OUT ITBT_NVS_AREA        *ItbtNvs
  )
{
  ItbtNvs->IntegratedTbtSupport = 0x1;
  ItbtNvs->IntegratedTbtDma0 = 0x1;
  ItbtNvs->IntegratedTbtDma1 = 0x1;
  ItbtNvs->ITbtPcieRootPortEn0 = 0x1;
  ItbtNvs->ITbtPcieRootPortEn1 = 0x1;
  ItbtNvs->ITbtPcieRootPortEn2 = 0x1;
  ItbtNvs->ITbtPcieRootPortEn3 = 0x1;
  ItbtNvs->ITbtRtd3 = 0x1;
  ItbtNvs->ITbtImrValid = 0x1;
  ItbtNvs->ITbtForcePowerOnTimeoutInMs = 0x1F4;
  ItbtNvs->ITbtConnectTopologyTimeoutInMs = 0x1388;
}

/**
 Update Pep NVS

 @param[in, out]  PepNvs       Pointer to PepNvs NVS region

**/
VOID
UpdatePepNvs (
  IN OUT PEP_NVS_AREA          *PepNvs
  )
{
  PepNvs->PepPcieStorage = 0x1;
  PepNvs->PepPcieLan = 0x1;
  PepNvs->PepPcieWlan = 0x1;
  PepNvs->PepPcieGfx = 0x3;
  PepNvs->PepPcieOther = 0x0;
  PepNvs->PepAudio = 0x1;
  PepNvs->PepCpu = 0x1;
  PepNvs->PepGfx = 0x1;
  PepNvs->PepIpu = 0x1;
  PepNvs->PepUart = 0x1;
  PepNvs->PepI2c0 = 0x1;
  PepNvs->PepI2c1 = 0x1;
  PepNvs->PepI2c2 = 0x1;
  PepNvs->PepI2c3 = 0x1;
  PepNvs->PepI2c4 = 0x1;
  PepNvs->PepI2c5 = 0x1;
  PepNvs->PepI2c6 = 0x0;
  PepNvs->PepI2c7 = 0x0;
  PepNvs->PepSpi = 0x1;
  PepNvs->PepXhci = 0x1;
  PepNvs->PepCsme = 0x1;
  PepNvs->PepHeci3 = 0x1;
  PepNvs->PepGbe = 0x1;
  PepNvs->PepThc0 = 0x1;
  PepNvs->PepThc1 = 0x1;
  PepNvs->PepUfs0 = 0x1;
  PepNvs->PepUfs1 = 0x1;
  PepNvs->PepTcss = 0x1;
  PepNvs->PepNpu = 0x1;
  PepNvs->PepIsh = 0x1;
  PepNvs->PepGna = 0x0;
  PepNvs->PepVmd = 0x1;
  PepNvs->PepSata = 0x0;
  PepNvs->PepSataEnum = 0x0;
  PepNvs->PepEmmc = 0x0;
  PepNvs->PepSdxc = 0x0;
  PepNvs->PepOsse = 0x0;
  PepNvs->PepEse = 0x1;
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
  TCSS_NVS_AREA           *TcssNvs;
  HOST_BRIDGE_NVS_AREA    *HostBridgeNvs;
  IGPU_NVS_AREA           *IgpuNvs;
  PEP_NVS_AREA            *PepNvs;
  SYS_CPU_INFO            *SysCpuInfo;
  FSPS_UPD                *FspsUpd;
  FSP_S_CONFIG            *FspsConfig;
  MEMORY_CFG_DATA         *MemCfgData;
  FEATURES_CFG_DATA       *FeaturesCfgData;
  UINT8                    Index;
  UINT8                    Length;
  UINT8                    RpDev;
  UINT8                    RpFun;
  UINT32                   Data32;
  UINT8                    GroupIndex[3];
  UCSI_NVS_AREA            *UcsiNvs;
  ITBT_NVS_AREA            *ItbtNvs;

  GlobalNvs = (GLOBAL_NVS_AREA *)GnvsIn;
  ZeroMem (GlobalNvs, sizeof (GLOBAL_NVS_AREA));
  PlatformNvs = (PLATFORM_NVS_AREA *) &GlobalNvs->PlatformNvs;
  PchNvs      = (PCH_NVS_AREA *) &GlobalNvs->PchNvs;
  CpuNvs      = (CPU_NVS_AREA *) &GlobalNvs->CpuNvs;
  TcssNvs     = (TCSS_NVS_AREA *) &GlobalNvs->TcssNvs;
  IgpuNvs     = (IGPU_NVS_AREA *) &GlobalNvs->IgpuNvs;
  HostBridgeNvs = (HOST_BRIDGE_NVS_AREA *) &GlobalNvs->HostBridgeNvs;
  UcsiNvs     = (UCSI_NVS_AREA *) &GlobalNvs->UcsiNvs;
  ItbtNvs     = (ITBT_NVS_AREA *) &GlobalNvs->ItbtNvs;
  PepNvs      = (PEP_NVS_AREA *) &GlobalNvs->PepNvs;
  FspsUpd     = (FSPS_UPD *)(UINTN)PcdGet32 (PcdFspsUpdPtr);
  FspsConfig  = &FspsUpd->FspsConfig;
  MemCfgData  = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);

  //PCH NVS Init
  PchNvs->PchSeries     = PchSeries();
  PchNvs->PchGeneration = (UINT16)PchGeneration();
  PchNvs->PchStepping   = PchStepping();
  PchNvs->PchAttached   = 0;
  DEBUG ((DEBUG_INFO, "PchSeries = 0x%x\n", PchNvs->PchSeries));
  DEBUG ((DEBUG_INFO, "PchGeneration = 0x%x\n", PchNvs->PchGeneration));
  DEBUG ((DEBUG_INFO, "PchStepping = 0x%x\n", PchNvs->PchStepping));
  DEBUG ((DEBUG_INFO, "PchAttached = 0x%x\n", PchNvs->PchAttached));

  //
  // Update ASL PCIE port address according to root port device and function
  //
  for (Index = 0; Index < PtlPcdGetMaxPcieControllerNum (); Index++) {
    RpDev = PchPcieRpDevNumber (Index);
    RpFun = PchPcieRpFuncNumber (Index);
    DEBUG ((DEBUG_INFO, "RpDev: 0x%x\n", RpDev));
    DEBUG ((DEBUG_INFO, "RpFun: 0x%x\n", RpFun));
    Data32 = ((UINT8) RpDev << 16) | (UINT8) RpFun;
    PchNvs->RpAddress[Index] = Data32;
    DEBUG ((DEBUG_INFO, "RpAddress[%d] = 0x%08X\n", Index, PchNvs->RpAddress[Index]));

    // // Need to match with FSP-S UPD
    PchNvs->LtrEnable[Index]                = FspsConfig->PcieRpLtrEnable[Index];
    PchNvs->PcieLtrMaxSnoopLatency[Index]   = FspsConfig->PcieRpLtrMaxSnoopLatency[Index];
    PchNvs->PcieLtrMaxNoSnoopLatency[Index] = FspsConfig->PcieRpLtrMaxNoSnoopLatency[Index];
  }

  PchNvs->CpuSku = 0;
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

  PchNvs->EmmcEnabled = 0;
  PchNvs->SdCardEnabled = 0;

  #if FixedPcdGetBool (PcdWclSupport) == 1
  GpioGetGpeMapping (GPIOV2_WCL_PCD_CHIPSET_ID, &GroupIndex[0], &GroupIndex[1], &GroupIndex[2]);
  #else
  GpioGetGpeMapping (GPIOV2_PTL_PCD_CHIPSET_ID, &GroupIndex[0], &GroupIndex[1], &GroupIndex[2]);
  #endif
  PchNvs->GEI0 = GroupIndex[0];
  PchNvs->GEI1 = GroupIndex[1];
  PchNvs->GEI2 = GroupIndex[2];
  PchNvs->GED0 = 0;
  PchNvs->GED1 = 0;
  PchNvs->GED2 = 0;
  DEBUG ((DEBUG_INFO, "GEI [0x%X 0x%X 0x%X], GED [0x%X 0x%X 0x%X]\n",
    PchNvs->GEI0, PchNvs->GEI1, PchNvs->GEI2,
    PchNvs->GED0, PchNvs->GED1, PchNvs->GED2));

  //Serial IO Devices
  //SPI
  Length = GetPchMaxSerialIoSpiControllersNum ();
  for (Index = 0; Index < Length ; Index++) {
    PchNvs->SC0[Index] = mPchSSerialIoSPIMode[Index].SerialIoPCIeConfig;
    PchNvs->SM0[Index] = mPchSSerialIoSPIMode[Index].SerialIoMode;
  }

  //I2C
  Length = GetPchMaxSerialIoI2cControllersNum ();
  for (Index = 0; Index < Length ; Index++) {
    PchNvs->IC0[Index] = mPchSSerialIoI2CMode[Index].SerialIoPCIeConfig;
    PchNvs->IM0[Index] = mPchSSerialIoI2CMode[Index].SerialIoMode;
  }

  if (MemCfgData != NULL) {
    Length = GetPchMaxPcieClockReqNum ();
    for (Index = 0; Index < Length ; Index++) {
      PchNvs->ClockToRootPortMap[Index] = MemCfgData->PcieClkSrcUsage[Index];
    }
  }

  //UART
  Length = GetPchMaxSerialIoUartControllersNum ();
  for (Index = 0; Index < Length ; Index++) {
     PchNvs->UM0[Index] = FspsConfig->SerialIoUartMode[Index];
     PchNvs->UC0[Index] = SerialIoUartPciCfgBase(Index);
     PchNvs->UD0[Index] = FspsConfig->SerialIoUartDmaEnable[Index];
     PchNvs->UP0[Index] = FspsConfig->SerialIoUartPowerGating[Index];
     PchNvs->UI0[Index] = mPchSSerialIoUartMode[Index].SerialIoUARTIrq;
  }

  PchNvs->UaolEnabled        = 1;
  PchNvs->CnviWifiCore       = FspsConfig->CnviWifiCore;
  PchNvs->CnviBtCore         = FspsConfig->CnviBtCore;
  PchNvs->CnviBtAudioOffload = FspsConfig->CnviBtAudioOffload;
  PchNvs->CnviBtInterface    = FspsConfig->CnviBtInterface;
  PchNvs->PsOnEnable         = FspsConfig->PsOnEnable;
  PchNvs->CnviPortId         = PID_CNVI;
  PchNvs->IclkPid            = PID_ICLK;
  PchNvs->GBES               = 1;
  PchNvs->CpuSku             = GetCpuSku();

  if (MemCfgData != NULL && MemCfgData->PchHdaEnable == 1) {
    // Update HDA ACPI
    PchNvs->ADFM = (BIT0 | BIT1 | BIT10);
    PchNvs->SWQ0 = 2;
    PchNvs->SWQ1 = 2;
    PchNvs->SWQ2 = 2;
    PchNvs->SWQ3 = 0;
    PchNvs->SWQ4 = 2;
    PchNvs->ACS0 = 0;
    PchNvs->ACS1 = 0;
    PchNvs->ACS2 = 0;
    PchNvs->ACS3 = 0;
    PchNvs->ACS4 = 0;
    PchNvs->DAI0 = 3;
    PchNvs->DAI1 = 3;
    PchNvs->DAI2 = 3;
    PchNvs->DAI3 = 3;
    PchNvs->DAI4 = 3;
    PchNvs->DAE0 = 1;
    PchNvs->DAE1 = 1;
    PchNvs->DAE2 = 1;
    PchNvs->DAE3 = 1;
    PchNvs->DAE4 = 1;
    PchNvs->DAX0 = 0;
    PchNvs->DAX1 = 0;
    PchNvs->DAX2 = 0;
    PchNvs->DAX3 = 0;
    PchNvs->DAX4 = 0;
    PchNvs->DOD0 = 0;
    PchNvs->DOD1 = 0;
    PchNvs->DOD2 = 0;
    PchNvs->DOD3 = 0;
    PchNvs->DOD4 = 0;
    PchNvs->DOE0 = 1;
    PchNvs->DOE1 = 1;
    PchNvs->DOE2 = 1;
    PchNvs->DOE3 = 1;
    PchNvs->DOE4 = 1;
    PchNvs->DOX0 = 1;
    PchNvs->DOX1 = 1;
    PchNvs->DOX2 = 1;
    PchNvs->DOX3 = 1;
    PchNvs->DOX4 = 1;
    PchNvs->CLE0 = 0;
    PchNvs->CLE1 = 0;
    PchNvs->CLE2 = 0;
    PchNvs->CLE3 = 0;
    PchNvs->CLE4 = 0;
    PchNvs->CLT0 = 2;
    PchNvs->CLT1 = 2;
    PchNvs->CLT2 = 2;
    PchNvs->CLT3 = 2;
    PchNvs->CLT4 = 2;
    PchNvs->CLS0 = 1;
    PchNvs->CLS1 = 1;
    PchNvs->CLS2 = 1;
    PchNvs->CLS3 = 1;
    PchNvs->CLS4 = 1;
    PchNvs->HDBO = 0;
    PchNvs->SWMC = 5;
    PchNvs->XTAL = V_EPOC_XTAL_38_4_MHZ;

    PchNvs->SML0 = 0;
    PchNvs->SML1 = 0;
    PchNvs->SML2 = 0;
    PchNvs->SML3 = (BIT0 | BIT1 | BIT2);
    PchNvs->SML4 = 0;
    PchNvs->DFS0 = 1;
    PchNvs->DFS1 = 1;
    PchNvs->DFS2 = 1;
    PchNvs->DFS3 = 1;
    PchNvs->DFS4 = 1;
    PchNvs->FRS0 = 0x32;
    PchNvs->FRS1 = 0x32;
    PchNvs->FRS2 = 0x32;
    PchNvs->FRS3 = 0x32;
    PchNvs->FRS4 = 0x32;
    PchNvs->FCS0 = 4;
    PchNvs->FCS1 = 4;
    PchNvs->FCS2 = 4;
    PchNvs->FCS3 = 4;
    PchNvs->FCS4 = 4;
    PchNvs->SSF1 = 0;
    PchNvs->SSF2 = 0;
    PchNvs->SSF3 = 0;
    PchNvs->SSF4 = 0;
  }

  //
  // Update PCH USB ports count
  //
  PchNvs->PchUsb2PortCount = (UINT8) GetPchUsb2MaxPhysicalPortNum ();
  PchNvs->PchUsb3PortCount = (UINT8) GetPchXhciMaxUsb3PortNum ();
  PchNvs->SataPortPresence = 0;

  //
  // UFS Enablement
  //
  PchNvs->Ufs0Enabled = FspsConfig->UfsEnable[0];
  PchNvs->Ufs1Enabled = FspsConfig->UfsEnable[1];

  PchNvs->LtrEnable[0] = 1;
  PchNvs->LtrEnable[1] = 1;
  PchNvs->LtrEnable[2] = 1;
  PchNvs->LtrEnable[3] = 1;

  PchNvs->PcieLtrMaxSnoopLatency[0] = 0x100F;
  PchNvs->PcieLtrMaxSnoopLatency[1] = 0x100F;
  PchNvs->PcieLtrMaxSnoopLatency[2] = 0x100F;
  PchNvs->PcieLtrMaxSnoopLatency[3] = 0x100F;
  PchNvs->PcieLtrMaxNoSnoopLatency[0] = 0x100F;
  PchNvs->PcieLtrMaxNoSnoopLatency[1] = 0x100F;
  PchNvs->PcieLtrMaxNoSnoopLatency[2] = 0x100F;
  PchNvs->PcieLtrMaxNoSnoopLatency[3] = 0x100F;

  PchNvs->PchxDCIPwrDnScale = 0x102000;

  //Platform NVS Init
  PlatformNvs->ApicEnable                   = 1;
  PlatformNvs->PlatformBoardId = (UINT8) GetPlatformId ();
  PlatformNvs->PowerState                   = 1;

  SysCpuInfo = MpGetInfo ();
  DEBUG ((DEBUG_INFO, "Num CPUs  = 0x%X\n",(UINT8)SysCpuInfo->CpuCount));
  if ( SysCpuInfo != NULL ) {
    PlatformNvs->ThreadCount      = (UINT8)SysCpuInfo->CpuCount;
  }

  PlatformNvs->Ps2MouseEnable    = 0x0;
  PlatformNvs->NativePCIESupport = 1;
  PlatformNvs->PciDelayOptimizationEcr = 0;
  if (MemCfgData != NULL) {
    PlatformNvs->UsbTypeCSupport = MemCfgData->TcssXhciEn;
  }

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
  //PlatformNvs->I2SI     = GPIOV2_MTL_SOC_M_GPP_F9;
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

  // Miscellaneous

  PlatformNvs->ConfigTdpBios = 0;
  PlatformNvs->PL1LimitCS = 0;
  PlatformNvs->PL1LimitCSValue = 4500;

  PlatformNvs->XdciFnEnable = FspsConfig->XdciEnable;

  PlatformNvs->HidEventFilterEnable         = 0x01;
  PlatformNvs->Rtd3Support = 0x1;
  PlatformNvs->Rtd3P0dl = 0x64;
  PlatformNvs->Rtd3AudioDelay = 0xC8;
  PlatformNvs->Rtd3SensorHub = 0x44;
  PlatformNvs->Rtd3TouchPanelDelay = 0x44;
  PlatformNvs->Rtd3TouchPadDelay = 0x44;
  PlatformNvs->VRRampUpDelay = 0x10;
  PlatformNvs->PstateCapping = 0x0;
  PlatformNvs->SensorStandby = 0x0;
  PlatformNvs->Rtd3Config0 = 0x0;
  PlatformNvs->StorageRtd3Support = 0x1;
  PlatformNvs->DeepestUSBSleepWakeCapability = 0x4;

  PlatformNvs->EnablePcieTunnelingOverUsb4 = 0x1;
  PlatformNvs->PreBootCmMode = 0x91;
  PlatformNvs->CmTbtMask = 0x3;

  PlatformNvs->Gpe1BlockEnable = 1;
  PlatformNvs->PcieDevOnOffDelay = 0xC8;
  PlatformNvs->StorageDynamicLinkManagement = 0x1;

  #if FixedPcdGetBool (PcdWclSupport) == 1
  PlatformNvs->PcieSlot1RpNumber = 3;
  PlatformNvs->PcieSlot1PowerEnableGpio = GPIOV2_WCL_PCD_XXGPP_F_10;
  PlatformNvs->PcieSlot1PowerEnableGpioPolarity = 0x1;
  PlatformNvs->PcieSlot1RstGpio = GPIOV2_WCL_PCD_XXGPP_D_19;
  PlatformNvs->PcieSlot1RstGpioPolarity = 0;

  PlatformNvs->PchM2SsdPowerEnableGpio = GPIOV2_WCL_PCD_XXGPP_H_18;
  PlatformNvs->PchM2SsdPowerEnableGpioPolarity = 0x1;
  PlatformNvs->PchM2SsdRstGpio = GPIOV2_WCL_PCD_XXGPP_A_8;
  PlatformNvs->PchM2SsdRstGpioPolarity = 0;

  PlatformNvs->PewakePin[3] = GPIOV2_WCL_PCD_XXGPP_A_12;
  #else
  PlatformNvs->PcieSlot1RpNumber = 3;
  PlatformNvs->PcieSlot1PowerEnableGpio = GPIOV2_PTL_PCD_XXGPP_A_8;
  PlatformNvs->PcieSlot1PowerEnableGpioPolarity = 0x1;
  PlatformNvs->PcieSlot1RstGpio = GPIOV2_PTL_PCD_XXGPP_D_19;
  PlatformNvs->PcieSlot1RstGpioPolarity = 0;

  PlatformNvs->PcieSlot2RpNumber = 0xB;
  PlatformNvs->PcieSlot2PowerEnableGpio = GPIOV2_PTL_PCD_XXGPP_F_10;
  PlatformNvs->PcieSlot2PowerEnableGpioPolarity = 0;
  PlatformNvs->PcieSlot2RstGpio = GPIOV2_PTL_PCD_XXGPP_D_9;
  PlatformNvs->PcieSlot2RstGpioPolarity = 0;

  PlatformNvs->PchM2SsdPowerEnableGpio = GPIOV2_PTL_PCD_XXGPP_B_10;
  PlatformNvs->PchM2SsdPowerEnableGpioPolarity = 0x1;
  PlatformNvs->PchM2SsdRstGpio = GPIOV2_PTL_PCD_XXGPP_B_9;
  PlatformNvs->PchM2SsdRstGpioPolarity = 0;

  PlatformNvs->M2Ssd2PowerEnableGpio = GPIOV2_PTL_PCD_XXGPP_B_16;
  PlatformNvs->M2Ssd2PowerEnableGpioPolarity = 0x1;
  PlatformNvs->M2Ssd2RstGpio = GPIOV2_PTL_PCD_XXGPP_E_3;
  PlatformNvs->M2Ssd2RstGpioPolarity = 0;

  PlatformNvs->PewakePin[1] = GPIOV2_PTL_PCD_XXGPP_F_23;
  PlatformNvs->PewakePin[2] = GPIOV2_PTL_PCD_XXGPP_B_25;
  PlatformNvs->PewakePin[10] = GPIOV2_PTL_PCD_XXGPP_D_24;
  #endif

  //CPU NVS Init
  UpdateCpuNvs (CpuNvs);

  //System Agent NVS Init
  HostBridgeNvs->LuxLowValue = 0;
  HostBridgeNvs->LuxHighValue = 0;

  // PCI config space must lie within 4GB address space with 256MB MMCONFIG window.
  ASSERT (PcdGet64 (PcdPciExpressBaseAddress) <= (BASE_4GB - 0x10000000));
  HostBridgeNvs->Mmio64Base               = PcdGet64 (PcdPciResourceMem64Base);
  HostBridgeNvs->Mmio64Length             = RShiftU64 (PcdGet64 (PcdPciResourceMem64Base), 1);
  HostBridgeNvs->Mmio32Base               = PcdGet32(PcdPciResourceMem32Base);
  HostBridgeNvs->Mmio32Length             = (UINT32)(PcdGet64(PcdPciExpressBaseAddress)) - HostBridgeNvs->Mmio32Base;
  HostBridgeNvs->XPcieCfgBaseAddress      = (UINT32)(PcdGet64(PcdPciExpressBaseAddress));
  HostBridgeNvs->SimicsEnvironment = 1;

  IgpuNvs->AlsEnable = 0;
  IgpuNvs->IgdState = 1;
  IgpuNvs->BrightnessPercentage = 100;
  IgpuNvs->EdpValid = 0;

  UpdateTcssNvs (TcssNvs);

  UpdateUcsiNvs (UcsiNvs);

  // Update TCSS USB3 port NVS
  for (Index = 0; Index < 4; Index++) {
    PlatformNvs->TcssUsb3PortGroupPosition[Index] = UcsiNvs->ConnectorGroupPosition[Index];
    PlatformNvs->TcssUsb3PortVisibility[Index] = UcsiNvs->ConnectorVisibility[Index];
    PlatformNvs->TcssUsb3PortConnectable[Index] = UcsiNvs->ConnectorVisibility[Index];
    PlatformNvs->TcssUsb3PortType[Index] = UcsiNvs->ConnectorType[Index];
    PlatformNvs->TcssUsb3PortCapability[Index] = UcsiNvs->ConnectorCapability[Index];
  }

  UpdateItbtNvs (ItbtNvs);

  UpdatePepNvs (PepNvs);
}

/**
  Update DTPR ACPI table with TPR information from HOB.

  @param[in] Table          Pointer to ACPI table header

  @retval EFI_SUCCESS       DTPR table updated successfully
  @retval EFI_NOT_FOUND     TPR_INFO_HOB not found
  @retval EFI_UNSUPPORTED   TXT not enabled
**/
EFI_STATUS
UpdateDtprAcpiTable (
  IN EFI_ACPI_DESCRIPTION_HEADER   *Table
  )
{
  TPR_INFO_HOB                *TprInfoHob;
  DTPR_TABLE_STRUCT           *DtprTable;
  UINT32                      Index;
  UINT32                      Count;
  UINT64                      *DrainCtrlArrayPtr;
  VOID                        *FspHobList;
  FEATURES_CFG_DATA           *FeaturesCfgData;
  EFI_HOB_GUID_TYPE           *GuidHob;
  UINT8                       *Buffer;
  UINT32                      Offset;

  DEBUG ((DEBUG_INFO, "Updating DTPR ACPI Table\n"));

  // Check if TXT is enabled
  FeaturesCfgData = (FEATURES_CFG_DATA *)FindConfigDataByTag (CDATA_FEATURES_TAG);
  if ((FeaturesCfgData == NULL) || (FeaturesCfgData->Features.TxtEnabled != 1)) {
    DEBUG ((DEBUG_INFO, "DTPR: TXT not enabled\n"));
    return EFI_UNSUPPORTED;
  }

  // Get TPR_INFO_HOB from FSP HOB list
  FspHobList = GetFspHobListPtr ();
  if (FspHobList == NULL) {
    DEBUG ((DEBUG_ERROR, "DTPR: FSP HOB List not found\n"));
    return EFI_NOT_FOUND;
  }

  GuidHob = GetNextGuidHob (&gTprInfoHobGuid, FspHobList);
  if (GuidHob == NULL) {
    DEBUG ((DEBUG_ERROR, "DTPR: TPR_INFO_HOB not found\n"));
    return EFI_NOT_FOUND;
  }

  TprInfoHob = GET_GUID_HOB_DATA (GuidHob);
  if (TprInfoHob == NULL) {
    DEBUG ((DEBUG_ERROR, "DTPR: Failed to get HOB data\n"));
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_INFO, "DTPR: Found TPR_INFO_HOB, populating table\n"));
  DEBUG ((DEBUG_INFO, "DTPR: InstanceCount=%d\n", TprInfoHob->InstanceCount));

  DtprTable = (DTPR_TABLE_STRUCT *)Table;
  ZeroMem (DtprTable, sizeof (DTPR_TABLE_STRUCT));

  // Set ACPI header
  DtprTable->Header.Signature = DTPR_ACPI_TABLE_SIGNATURE;
  DtprTable->Header.Revision = 1;
  CopyMem (DtprTable->Header.OemId, "INTEL ", sizeof (DtprTable->Header.OemId));
  DtprTable->Header.OemTableId = 0x20202020324B4445;  // "EDK2    "
  DtprTable->Header.OemRevision = 0x00000002;
  DtprTable->Header.CreatorId = 0x20202020;  // "    " (spaces)
  DtprTable->Header.CreatorRevision = 0x01000013;
  DtprTable->Flags = 0;

  // Set instance count - ensure at least 1 for SINIT compatibility
  DtprTable->InsCnt = TprInfoHob->InstanceCount;
  if (DtprTable->InsCnt == 0) {
    DEBUG ((DEBUG_INFO, "DTPR: Correcting InstanceCount from 0 to 1\n"));
    DtprTable->InsCnt = 1;
  }

  if (DtprTable->InsCnt > MAX_TPR_INSTANCES) {
    DEBUG ((DEBUG_ERROR, "DTPR: Instance count exceeds maximum (%d > %d)\n",
            DtprTable->InsCnt, MAX_TPR_INSTANCES));
    return EFI_INVALID_PARAMETER;
  }

  // Populate TPR instances
  for (Index = 0; Index < DtprTable->InsCnt; Index++) {
    DtprTable->TprIns[Index].Flags = 0;
    if (Index < TprInfoHob->InstanceCount && TprInfoHob->InstanceCount > 0) {
      DtprTable->TprIns[Index].TprCnt = TprInfoHob->InstanceArray[Index].TprCount;
      DtprTable->TprIns[Index].TprArr[0].TprBase = TprInfoHob->InstanceArray[Index].Tpr0BaseOffset;
      DtprTable->TprIns[Index].TprArr[1].TprBase = TprInfoHob->InstanceArray[Index].Tpr1BaseOffset;
      DEBUG ((DEBUG_INFO, "DTPR: Instance[%d] TprCnt=%d Tpr0=0x%llx Tpr1=0x%llx\n",
              Index, DtprTable->TprIns[Index].TprCnt,
              DtprTable->TprIns[Index].TprArr[0].TprBase,
              DtprTable->TprIns[Index].TprArr[1].TprBase));
    } else {
      DtprTable->TprIns[Index].TprCnt = 2;
      DtprTable->TprIns[Index].TprArr[0].TprBase = 0;
      DtprTable->TprIns[Index].TprArr[1].TprBase = 0;
    }
  }

  // Extract drain control registers from HOB structure
  // The DrainControlArray is a DRAIN_CTL_REG structure with individual fields
  DrainCtrlArrayPtr = (UINT64 *)&(TprInfoHob->DrainControlArray);
  Count = 0;

  // Iterate through the DRAIN_CTL_REG structure fields (10 UINT64 fields)
  for (Index = 0; Index < 10; Index++) {
    if (*DrainCtrlArrayPtr != 0 && Count < MAX_SRL_REG_COUNT) {
      DtprTable->SrlRegArr[Count] = *DrainCtrlArrayPtr;
      DEBUG ((DEBUG_INFO, "DTPR: DrainCtrl[%d]=0x%llx\n", Count, DtprTable->SrlRegArr[Count]));
      Count++;
    }
    DrainCtrlArrayPtr++;
  }

  if (Count == 0) {
    DEBUG ((DEBUG_ERROR, "DTPR: No drain controls found\n"));
    return EFI_UNSUPPORTED;
  }

  DtprTable->SrlCnt = Count;

  // Calculate total table length
  DtprTable->Header.Length = sizeof(EFI_ACPI_DESCRIPTION_HEADER) +
                            sizeof(UINT32) + sizeof(UINT32) +
                            sizeof(TPR_INS_STRUCT) * DtprTable->InsCnt +
                            sizeof(UINT32) +
                            sizeof(UINT64) * Count;

  DEBUG ((DEBUG_INFO, "DTPR: Instances=%d, DrainControls=%d, Length=%d\n",
          DtprTable->InsCnt, DtprTable->SrlCnt, DtprTable->Header.Length));

  // Copy the complete DTPR table to the output buffer
  Buffer = (UINT8 *)Table;
  Offset = 0;

  // Copy ACPI header
  CopyMem (Buffer + Offset, &DtprTable->Header, sizeof(EFI_ACPI_DESCRIPTION_HEADER));
  Offset += sizeof(EFI_ACPI_DESCRIPTION_HEADER);

  // Copy Flags
  CopyMem (Buffer + Offset, &(DtprTable->Flags), sizeof(UINT32));
  Offset += sizeof(UINT32);

  // Copy InsCnt
  CopyMem (Buffer + Offset, &(DtprTable->InsCnt), sizeof(UINT32));
  Offset += sizeof(UINT32);

  // Copy TPR instances
  if (DtprTable->InsCnt > 0) {
    CopyMem (Buffer + Offset, &(DtprTable->TprIns[0]), sizeof(TPR_INS_STRUCT) * DtprTable->InsCnt);
    Offset += sizeof(TPR_INS_STRUCT) * DtprTable->InsCnt;
  }

  // Copy SrlCnt
  CopyMem (Buffer + Offset, &(DtprTable->SrlCnt), sizeof(UINT32));
  Offset += sizeof(UINT32);

  // Copy Drain control registers
  if (Count > 0) {
    CopyMem (Buffer + Offset, &(DtprTable->SrlRegArr[0]), sizeof(UINT64) * Count);
    Offset += sizeof(UINT64) * Count;
  }

  // Verify the table was copied correctly
  if (Offset != DtprTable->Header.Length) {
    DEBUG ((DEBUG_ERROR, "DTPR: Length mismatch! Expected=0x%x, Actual=0x%x\n",
            DtprTable->Header.Length, Offset));
    return EFI_INVALID_PARAMETER;
  }

  // Calculate and set ACPI checksum
  ((EFI_ACPI_DESCRIPTION_HEADER *)Table)->Checksum = 0;
  ((EFI_ACPI_DESCRIPTION_HEADER *)Table)->Checksum = CalculateCheckSum8 (
    (UINT8 *)Table,
    ((EFI_ACPI_DESCRIPTION_HEADER *)Table)->Length
  );

  DEBUG ((DEBUG_INFO, "DTPR: Table populated - InsCnt=%d, SrlCnt=%d, Length=%d, Checksum=0x%02X\n",
          DtprTable->InsCnt, DtprTable->SrlCnt,
          ((EFI_ACPI_DESCRIPTION_HEADER *)Table)->Length,
          ((EFI_ACPI_DESCRIPTION_HEADER *)Table)->Checksum));

  return EFI_SUCCESS;
}
