/** @file

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/TpmLib.h>
#include <PlatformData.h>
#include <Library/MeExtMeasurementLib.h>
#include "Stage2BoardInitLib.h"

#define MWAIT_C6                     0x20
#define MWAIT_C6_1                   0x21
#define MWAIT_C7                     0x30
#define MWAIT_C7_1                   0x31
#define MWAIT_C7_2                   0x32
#define MWAIT_C7_3                   0x33
#define MWAIT_CD                     0x40
#define MWAIT_CD_1                   0x50
#define MWAIT_CD_2                   0x60

CONST PCH_SERIAL_IO_CONFIG_INFO mPchSSerialIoSPIMode[PCH_MAX_SERIALIO_SPI_CONTROLLERS] = {
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_SPI0},
  {1,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_SPI1},
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_SPI2},
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_SPI3},
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_SPI4},
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_SPI5},
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_SPI6}
};

CONST PCH_SERIAL_IO_CONFIG_INFO mPchSSerialIoI2CMode[PCH_MAX_SERIALIO_I2C_CONTROLLERS] = {
  {1,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C0},
  {1,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C1},
  {1,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C2},
  {1,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C3},
  {1,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C4},
  {1,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C5},
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C6},
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C7}
};

CONST PCH_SERIAL_IO_UART_CONFIG_INFO mPchSSerialIoUartMode[PCH_MAX_SERIALIO_UART_CONTROLLERS] = {
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART0, 1, 2, 16 },
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART1, 1, 2, 17 },
  {2,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART2, 1, 2, 33 },
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART3, 0, 0, 0 },
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART4, 0, 0, 0 },
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART5, 0, 0, 0 },
  {0,R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART6, 0, 0, 0 }
};

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
  TCC_CFG_DATA                *TccCfgData;
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
  } else if (Table->Signature == NHLT_ACPI_TABLE_SIGNATURE) {
    GlobalNvs->PchNvs.NHLA  = (UINT64)(UINTN) Table;
    GlobalNvs->PchNvs.NHLL  = Table->Length;
    DEBUG ((DEBUG_INFO, "NHLT Base 0x%08X, Size 0x%08X\n", (UINT32)(UINTN)GlobalNvs->PchNvs.NHLA, GlobalNvs->PchNvs.NHLL));
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
  } else if (Table->Signature == SIGNATURE_32 ('R', 'T', 'C', 'T')) {
    DEBUG ((DEBUG_INFO, "Find RTCT table\n"));

    if (FeaturePcdGet (PcdTccEnabled)) {
      TccCfgData = (TCC_CFG_DATA *) FindConfigDataByTag(CDATA_TCC_TAG);
      if ((TccCfgData != NULL) && (TccCfgData->TccEnable != 0)) {
        Status = UpdateAcpiRtctTable(Table);
        DEBUG ( (DEBUG_INFO, "Updated Rtct Table entries in AcpiTable status: %r\n", Status) );
        return Status;
      }
    }
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
        Lpi_Counter.Data8 = 0x3;  //todo: add option to choose in Cfgdata
//        if (Lpi_Counter.Bits.C10) { //todo: hardcode to this for now
          Address = MSR_PC10_RCNTR;
          ResidencyCounterFrequency = 0; //Counter runs at TSC frequency
          UpdLpiStat((ACPI_LOW_POWER_IDLE_TABLE *)Table, 0, Flags, Address, ResidencyCounterFrequency);
//        }
//        if (Lpi_Counter.Bits.Slp_S0)  { //todo: hardcode to this for now
          if (IsPchP () || IsPchN ()) {
            //
            // Read PWRM Base Address to fill in Residency counter Address Space
            //
            Address = PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_SLP_S0_RESIDENCY;
            ResidencyCounterFrequency = 8197;  //Counter runs at 122us granularity which implies 10KHz frequency (8197Hz)
            UpdLpiStat((ACPI_LOW_POWER_IDLE_TABLE *)Table, 1, Flags, Address, ResidencyCounterFrequency);
          } else {
            //
            // Read PWRM Base Address to fill in Residency counter Address Space
            //
            Address = PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_BIOS_SCRATCHPAD_2;
            ResidencyCounterFrequency = 32768;  //Counter runs at 30.5us granularity which implies 10KHz frequency (32768Hz)
            UpdLpiStat((ACPI_LOW_POWER_IDLE_TABLE *)Table, 1, Flags, Address, ResidencyCounterFrequency);
          }
//        }
        if (Lpi_Counter.Bits.Ps_On) {
          //
          // Read PWRM Base Address to fill in Residency counter Address Space
          //
          Address = PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_PS_ON_COUNTER;
          ResidencyCounterFrequency = 8197;  //Counter runs at 122us granularity which implies 10KHz frequency (8197Hz)
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
    if (GetCpuSku() == 0) {   // ADL-S
      FadtPointer->PreferredPmProfile = 0x2;  //mobile
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


  CpuNvs->EnableItbm = CpuInitDataHob->EnableItbm;                                         /// Enable/Disable Intel Turbo Boost Max Technology 3.0.
  //CpuNvs->EnableItbmDriver = CpuInitDataHob->EnableItbmDriver;                             /// Enable/Disable Intel Turbo Boost Max Technology 3.0 Driver.


  CpuNvs->LowestMaxPerf = CpuInitDataHob->LowestMaxPerf;                                   /// Max ratio of the slowest core.
  CpuNvs->ConfigurablePpc = CpuInitDataHob->ConfigurablePpc;                               /// Max ratio of the slowest core.

  //CpuNvs->DtsIoTrapAddress = 0x0810;
  //CpuNvs->DtsIoTrapLength = 0x4;
  //CpuNvs->DtsAcpiEnable = 0x0;

  CpuNvs->CoreType       = 0xFFFF; //Need to check the way to get ProcessorNum
  CpuNvs->HeteroStatus   = IsHeteroCoreSupported();
  CpuNvs->ScalingFactorSmallCore=12;
  CpuNvs->ScalingFactorBigCore=15;
  //DEBUG ((DEBUG_INFO, "BigCore Count %d SmallCore Count %d\n",CpuNvs->BigCoreCount, CpuNvs->SmallCoreCount));

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
  FSPS_UPD                *FspsUpd;
  FSP_S_CONFIG            *FspsConfig;
  TCC_CFG_DATA            *TccCfgData;
  SILICON_CFG_DATA        *SiCfgData;
  FEATURES_CFG_DATA       *FeaturesCfgData;
  UINT8                    Index;
  UINT8                    Length;
  UINT8                    RpNum;
  UINTN                    RpDev;
  UINTN                    RpFun;
  UINT32                   Data32;
  GPIO_GROUP               GroupToGpeDwX[3];
  UINT32                   GroupDw[3];
  CPUID_EXTENDED_TIME_STAMP_COUNTER_EDX  Edx;
  CPUID_PROCESSOR_FREQUENCY_EBX          Ebx;
  PLATFORM_DATA           *PlatformData;
  EFI_STATUS              Status;

  GlobalNvs = (GLOBAL_NVS_AREA *)GnvsIn;
  ZeroMem (GlobalNvs, sizeof (GLOBAL_NVS_AREA));
  PlatformNvs = (PLATFORM_NVS_AREA *) &GlobalNvs->PlatformNvs;
  PchNvs      = (PCH_NVS_AREA *) &GlobalNvs->PchNvs;
  CpuNvs      = (CPU_NVS_AREA *) &GlobalNvs->CpuNvs;
  SaNvs       = (SYSTEM_AGENT_NVS_AREA *) &GlobalNvs->SaNvs;

  FspsUpd     = (FSPS_UPD *)(UINTN)PcdGet32 (PcdFspsUpdPtr);
  FspsConfig  = &FspsUpd->FspsConfig;

  //PCH NVS Init
  PchNvs->PchSeries     = PchSeries ();
  PchNvs->PchGeneration = (UINT16) PchGeneration ();
  PchNvs->PchStepping   = (UINT16) PchStepping ();

  Length = GetPchMaxPciePortNum ();
  for (RpNum = 0; RpNum < Length; RpNum++) {
    Status = GetPchPcieRpDevFun (RpNum, &RpDev, &RpFun);
    if (!EFI_ERROR (Status)) {
      Data32 = ((UINT8) RpDev << 16) | (UINT8) RpFun;
      PchNvs->RpAddress[RpNum] = Data32;
      DEBUG ((DEBUG_INFO, "RpAddress[%d] = 0x%08X\n", RpNum, PchNvs->RpAddress[RpNum]));
    }
  }
  for (Index = 0; Index < Length; Index++) {
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
  PchNvs->GBES          = 1;

  //
  // GPIO device
  //
  PchNvs->SGIR  = FspsConfig->GpioIrqRoute;
  PchNvs->GPHD  = 0;

  GpioGetGroupDwToGpeDwX (
    &GroupToGpeDwX[0], &GroupDw[0],
    &GroupToGpeDwX[1], &GroupDw[1],
    &GroupToGpeDwX[2], &GroupDw[2]
    );

  PchNvs->GEI0 = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[0]);
  PchNvs->GEI1 = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[1]);
  PchNvs->GEI2 = (UINT8) GpioGetGroupIndexFromGroup (GroupToGpeDwX[2]);
  PchNvs->GED0 = (UINT8) GroupDw[0];
  PchNvs->GED1 = (UINT8) GroupDw[1];
  PchNvs->GED2 = (UINT8) GroupDw[2];
  DEBUG ((DEBUG_INFO, "GEI [0x%X 0x%X 0x%X], GED [0x%X 0x%X 0x%X]\n",
    PchNvs->GEI0, PchNvs->GEI1, PchNvs->GEI2,
    PchNvs->GED0, PchNvs->GED1, PchNvs->GED2));

  //Serial IO Devices
  //SPI
  Length = GetPchMaxSerialIoSpiControllersNum ();
  for (Index = 0; Index < Length ; Index++) {
    PchNvs->SC0[Index] = mPchSSerialIoSPIMode[Index].SerialIoPCIeConfig;
    PchNvs->SM0[Index] = FspsConfig->SerialIoSpiMode[Index];
  }

  PchNvs->SC0[0] = 991232;
  PchNvs->SC0[1] = 995328;
  PchNvs->SC0[2] = 614400;
  PchNvs->SC0[3] = 622592;
  PchNvs->SC0[4] = 626688;
  PchNvs->SC0[5] = 573440;
  PchNvs->SC0[5] = 577536;

  //I2C
  Length = GetPchMaxSerialIoI2cControllersNum ();
  for (Index = 0; Index < Length ; Index++) {
    PchNvs->IC0[Index] = mPchSSerialIoI2CMode[Index].SerialIoPCIeConfig;
    PchNvs->IM0[Index] = FspsConfig->SerialIoI2cMode[Index];;
  }
  PchNvs->IC0[0] = 688128;
  PchNvs->IC0[1] = 692224;
  PchNvs->IC0[2] = 696320;
  PchNvs->IC0[3] = 700416;
  PchNvs->IC0[4] = 819200;
  PchNvs->IC0[5] = 823296;
  PchNvs->IC0[6] = 524288;
  PchNvs->IC0[7] = 528384;

  PchNvs->ClockToRootPortMap[0] = 0xFF;
  PchNvs->ClockToRootPortMap[1] = 0xFF;
  PchNvs->ClockToRootPortMap[2] = 0xFF;
  PchNvs->ClockToRootPortMap[3] = 0xFF;
  PchNvs->ClockToRootPortMap[4] = 0xFF;
  PchNvs->ClockToRootPortMap[5] = 0xFF;
  PchNvs->ClockToRootPortMap[6] = 0xFF;
  PchNvs->ClockToRootPortMap[7] = 0xFF;
  PchNvs->ClockToRootPortMap[8] = 0xFF;
  PchNvs->ClockToRootPortMap[9] = 0x7;
  PchNvs->ClockToRootPortMap[10] = 0x0;
  PchNvs->ClockToRootPortMap[11] = 0xFF;
  PchNvs->ClockToRootPortMap[12] = 0x10;
  PchNvs->ClockToRootPortMap[13] = 0x24;
  PchNvs->ClockToRootPortMap[14] = 0x8;
  PchNvs->ClockToRootPortMap[15] = 0x4;
  PchNvs->ClockToRootPortMap[16] = 0xFF;
  PchNvs->ClockToRootPortMap[17] = 0xFF;

  //UART
  Length = GetPchMaxSerialIoUartControllersNum ();
  for (Index = 0; Index < Length ; Index++) {
    PchNvs->UM0[Index] = FspsConfig->SerialIoUartMode[Index];
    PchNvs->UC0[Index] = SerialIoUartPciCfgBase(Index);
    PchNvs->UD0[Index] = FspsConfig->SerialIoUartDmaEnable[Index];
    PchNvs->UP0[Index] = FspsConfig->SerialIoUartPowerGating[Index];
    PchNvs->UI0[Index] = mPchSSerialIoUartMode[Index].SerialIoUARTIrq;
  }

  PchNvs->UI0[3]=25;

  PchNvs->CnviMode           = FspsConfig->CnviMode;
  PchNvs->CnviBtCore         = FspsConfig->CnviBtCore;
  PchNvs->CnviBtAudioOffload = FspsConfig->CnviBtAudioOffload;
  PchNvs->PsOnEnable         = FspsConfig->PsOnEnable;
  PchNvs->CnviPortId         = PID_CNVI;
  PchNvs->IclkPid            = PID_ICLK;
  PchNvs->GBES               = PchIsGbeSupported();
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

  PchNvs->ClkreqIpcCmdSupported = 1;
  PchNvs->HybridStorageCpuRpLocation = 0xFF;

  //Platform NVS Init
  PlatformNvs->ApicEnable                   = 1;
  PlatformNvs->PlatformId = (UINT8) GetPlatformId ();
  PlatformNvs->PowerState                   = 1;
  if ((SiCfgData != NULL) && (SiCfgData->EcAvailable)) {
    PlatformNvs->EcAvailable            = SiCfgData->EcAvailable;
    if (PlatformNvs->EcAvailable == 1) {
      PlatformNvs->EcLowPowerMode         = 0;
      PlatformNvs->EcSmiGpioPin           = GPIO_VER4_S_GPP_B4;
      PlatformNvs->EcLowPowerModeGpioPin  = 0;
    }
  }

  SysCpuInfo = MpGetInfo ();
  DEBUG ((DEBUG_INFO, "Num CPUs  = 0x%X\n",(UINT8)SysCpuInfo->CpuCount));
  if ( SysCpuInfo != NULL ) {
    PlatformNvs->ThreadCount      = (UINT8)SysCpuInfo->CpuCount;
  }

  PlatformNvs->Ps2MouseEnable    = 0x0;
  PlatformNvs->Ps2KbMsEnable     = 1;
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
  }

  PlatformNvs->I2SC     = 0x0;
  PlatformNvs->I2SI     = GPIO_VER4_S_GPP_F23;
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
  PlatformNvs->EnableDptf                   = 0;

  //
  // Wireless
  //
  PlatformNvs->PowerSharingManagerEnable    = 0;

  PlatformNvs->WifiEnable                   = 0;


  // Miscellaneous
  //
  PlatformNvs->ConfigTdpBios = 0;
  PlatformNvs->PL1LimitCS = 0;
  PlatformNvs->PL1LimitCSValue = 4500;

  PlatformNvs->PL1LimitCS        = 0x0;
  PlatformNvs->PL1LimitCSValue   = 4500;

  PlatformNvs->PcieSlot1WakeGpio = GPIO_VER4_S_GPP_F0;
  PlatformNvs->PcieSlot1RpNumber = 21;  //todo need to set for ADL P also
  PlatformNvs->PcieSlot1PowerEnableGpioPolarity = 1;
  PlatformNvs->PcieSlot1RstGpio = GPIO_VER4_S_GPP_F11;
  PlatformNvs->PcieSlot1RstGpioPolarity = 0;

  PlatformNvs->PcieSlot2WakeGpio = GPIO_VER4_S_GPP_F1;
  PlatformNvs->PcieSlot2RpNumber = 9;
  PlatformNvs->PcieSlot2PowerEnableGpio = GPIO_VER4_S_GPP_H11;
  PlatformNvs->PcieSlot2PowerEnableGpioPolarity = 1;
  PlatformNvs->PcieSlot2RstGpio = GPIO_VER4_S_GPP_F12;
  PlatformNvs->PcieSlot2RstGpioPolarity = 0;

  PlatformNvs->PcieSlot3WakeGpio = GPIO_VER4_S_GPP_F2;
  PlatformNvs->PcieSlot3RpNumber = 5;
  PlatformNvs->PcieSlot3PowerEnableGpio = GPIO_VER4_S_GPP_B21;
  PlatformNvs->PcieSlot3PowerEnableGpioPolarity = 1;
  PlatformNvs->PcieSlot3RstGpio = GPIO_VER4_S_GPP_F13;
  PlatformNvs->PcieSlot3RstGpioPolarity = 0;

  PlatformNvs->WlanWakeGpio = GPIO_VER4_S_GPP_H2;
  PlatformNvs->WlanRootPortNumber = 8;

  PlatformNvs->PegSlot1PwrEnableGpioNo = GPIO_VER4_S_GPP_E0;
  PlatformNvs->PegSlot1PwrEnableGpioPolarity = 1;
  PlatformNvs->PegSlot1RstGpioNo = GPIO_VER4_S_GPP_E2;
  PlatformNvs->PegSlot1RstGpioPolarity = 0;
  PlatformNvs->PegSlot1WakeGpioPin = GPIO_VER4_S_GPP_E6;

  PlatformNvs->M2Ssd2PowerEnableGpio = GPIO_VER4_S_GPP_H16;
  PlatformNvs->M2Ssd2PowerEnableGpioPolarity = 1;
  PlatformNvs->M2Ssd2RstGpio = GPIO_VER4_S_GPP_F18;
  PlatformNvs->M2Ssd2RstGpioPolarity = 0;

  PlatformNvs->PchM2SsdPowerEnableGpioPolarity = 1;
  PlatformNvs->PchM2SsdRstGpio = GPIO_VER4_S_GPP_C10;
  PlatformNvs->PchM2SsdRstGpioPolarity = 0;

  PlatformNvs->PchM2Ssd2PowerEnableGpio = GPIO_VER4_S_GPP_B22;
  PlatformNvs->PchM2Ssd2PowerEnableGpioPolarity = 1;
  PlatformNvs->PchM2Ssd2RstGpio = GPIO_VER4_S_GPP_F16;
  PlatformNvs->PchM2Ssd2RstGpioPolarity = 0;

  PlatformNvs->PchM2Ssd3PowerEnableGpio = GPIO_VER4_S_GPP_K2;
  PlatformNvs->PchM2Ssd3PowerEnableGpioPolarity = 1;
  PlatformNvs->PchM2Ssd3RstGpio = GPIO_VER4_S_GPP_B6;
  PlatformNvs->PchM2Ssd3RstGpioPolarity = 0;

  PlatformNvs->SataPortPowerEnableGpio = GPIO_VER4_S_GPP_R11;
  PlatformNvs->SataPortPowerEnableGpioPolarity = 1;

  switch (GetPlatformId ()) {
  case PLATFORM_ID_ADL_S_ADP_S_CRB:
  case PLATFORM_ID_ADL_S_ADP_S_DDR4_SODIMM_CRB:
    PlatformNvs->PcieSlot1PowerEnableGpio = GPIO_VER4_S_GPP_F22;

    PlatformNvs->PegSlot2PwrEnableGpioNo = GPIO_VER4_S_GPP_E1;
    PlatformNvs->PegSlot2PwrEnableGpioPolarity = 1;
    PlatformNvs->PegSlot2RstGpioNo = GPIO_VER4_S_GPP_E3;
    PlatformNvs->PegSlot2RstGpioPolarity = 0;
    PlatformNvs->PegSlot2WakeGpioPin = GPIO_VER4_S_GPP_E7;

    PlatformNvs->PchM2SsdPowerEnableGpio = GPIO_VER4_S_GPP_C2;
    break;
  case PLATFORM_ID_ADL_S_ADP_S_DDR5_UDIMM_1DC_CRB:
  case PLATFORM_ID_ADL_S_ADP_S_DDR5_SODIMM_CRB:
    PlatformNvs->PcieSlot1PowerEnableGpio = GPIO_VER4_S_GPP_E1;

    PlatformNvs->PegSlot2PwrEnableGpioPolarity = 0;

    PlatformNvs->PchM2SsdPowerEnableGpio = GPIO_VER4_S_GPP_K11;
    break;
  case PLATFORM_ID_TEST_S_DDR5_UDIMM_RVP:
    PlatformNvs->PcieSlot1PowerEnableGpio = GPIO_VER4_S_GPP_E1;
    PlatformNvs->PcieSlot2RpNumber = 5;
    break;
  case PLATFORM_ID_ADL_P_DDR5_RVP:
    PlatformNvs->PcieSlot1WakeGpio = 0;
    PlatformNvs->PcieSlot1PowerEnableGpio = GPIO_VER2_LP_GPP_A22;
    PlatformNvs->PcieSlot1PowerEnableGpioPolarity = 0;
    PlatformNvs->PcieSlot1RstGpio = GPIO_VER2_LP_GPP_F10;
    PlatformNvs->PcieSlot1RpNumber = 8;
    PlatformNvs->PcieSlot2WakeGpio = 0;
    PlatformNvs->PcieSlot2PowerEnableGpio = 0;
    PlatformNvs->PcieSlot2PowerEnableGpioPolarity = 0;
    PlatformNvs->PcieSlot2RstGpio = 0;
    PlatformNvs->PcieSlot2RpNumber = 0;
    PlatformNvs->PcieSlot3WakeGpio = 0;
    PlatformNvs->PcieSlot3PowerEnableGpio = 0;
    PlatformNvs->PcieSlot3PowerEnableGpioPolarity = 0;
    PlatformNvs->PcieSlot3RstGpio = 0;
    PlatformNvs->PcieSlot3RpNumber = 0;
    PlatformNvs->M2Ssd2PowerEnableGpio = GPIO_VER2_LP_GPP_D14;
    PlatformNvs->M2Ssd2RstGpio = GPIO_VER2_LP_GPP_F20;
    PlatformNvs->M2Ssd3PowerEnableGpio = GPIO_VER2_LP_GPP_C2;
    PlatformNvs->PchM2SsdPowerEnableGpio = GPIO_VER2_LP_GPP_D16;
    PlatformNvs->PchM2SsdRstGpio = GPIO_VER2_LP_GPP_H0;
    PlatformNvs->PchM2Ssd2PowerEnableGpio = 0;
    PlatformNvs->PchM2Ssd2PowerEnableGpioPolarity = 0;
    PlatformNvs->PchM2Ssd2RstGpio = 0;
    PlatformNvs->PchM2Ssd3PowerEnableGpio = 0;
    PlatformNvs->PchM2Ssd3PowerEnableGpioPolarity = 0;
    PlatformNvs->PchM2Ssd3RstGpio = 0;
    PlatformNvs->SataPortPowerEnableGpio = 0;
    PlatformNvs->SataPortPowerEnableGpioPolarity = 0;
    PlatformNvs->PegSlot1PwrEnableGpioNo = 0;
    PlatformNvs->PegSlot1PwrEnableGpioPolarity = 0;
    PlatformNvs->PegSlot1RstGpioNo = 0;
    PlatformNvs->PegSlot1WakeGpioPin = GPIO_VER2_LP_GPP_A20;
    PlatformNvs->PegSlot1RootPort = 0;
    PlatformNvs->PegSlot2PwrEnableGpioNo = 0;
    PlatformNvs->PegSlot2PwrEnableGpioPolarity = 0;
    PlatformNvs->PegSlot2RstGpioNo = 0;
    PlatformNvs->PegSlot2RstGpioPolarity = 0;
    PlatformNvs->PegSlot2WakeGpioPin = 0;
    PlatformNvs->PegSlot2RootPort = 0;
    PlatformNvs->WlanWakeGpio = GPIO_VER2_LP_GPP_D13;
    PlatformNvs->WlanRootPortNumber = 5;
    PlatformNvs->PL1LimitCSValue = 0x1194;

  default:
    DEBUG ((DEBUG_ERROR, "Could not find PlatformNvs settings for PlatformId 0x%X!\n", GetPlatformId ()));
    break;
  }

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

  PlatformNvs->Rp08D3ColdDisable = 0x0;
  PlatformNvs->Rp08D3ColdSupport = 0x0;
  PlatformNvs->Rp08WakeReworkDone = 0x0;

  //CPU NVS Init
  UpdateCpuNvs (CpuNvs);

  //System Agent NVS Init
  SaNvs->Mmio64Base               = 0;
  SaNvs->Mmio64Length             = 0;
  SaNvs->Mmio32Base               = PcdGet32(PcdPciResourceMem32Base);
  SaNvs->Mmio32Length             = ACPI_MMIO_BASE_ADDRESS - SaNvs->Mmio32Base;
  SaNvs->XPcieCfgBaseAddress      = (UINT32)(PcdGet64(PcdPciExpressBaseAddress));
  SaNvs->CpuPcieRp0Enable = 1;
  SaNvs->CpuPcieRp1Enable = 1;
  SaNvs->CpuPcieRp2Enable = 1;
  SaNvs->CpuPcieRp3Enable = 1;
  SaNvs->SimicsEnvironment = 0;
  SaNvs->AlsEnable = 0;
  SaNvs->IgdState = 1;
  SaNvs->BrightnessPercentage = 100;
  SaNvs->IgdBootType = 0;
  SaNvs->IgdPanelType = 0;
  SaNvs->IgdPanelScaling = 0;
  SaNvs->IgdDvmtMemSize = 2;
  SaNvs->IgdFunc1Enable = 0;
  SaNvs->IgdSciSmiMode = 0;
  SaNvs->GfxTurboIMON = 31;
  SaNvs->EdpValid = 0;
  SaNvs->Peg0LtrEnable  = 0x1;
  SaNvs->Peg0ObffEnable = 0x1;
  SaNvs->Peg1LtrEnable  = 0x1;
  SaNvs->Peg1ObffEnable = 0x1;
  SaNvs->Peg2LtrEnable  = 0x1;
  SaNvs->Peg2ObffEnable = 0x1;
  SaNvs->Peg3LtrEnable  = 0x1;
  SaNvs->Peg3ObffEnable = 0x1;
  SaNvs->PegLtrMaxSnoopLatency = 0x846;
  SaNvs->PegLtrMaxNoSnoopLatency = 0x846;
  SaNvs->Pcie0HoldRstExpanderNo = 0x0;
  SaNvs->Pcie1HoldRstExpanderNo = 0x0;
  SaNvs->Pcie2HoldRstExpanderNo = 0x0;
  SaNvs->Pcie3HoldRstExpanderNo = 0x0;
  SaNvs->HgMode = 0x03;
  SaNvs->LtrEnable[0] = 1;
  SaNvs->LtrEnable[1] = 1;
  SaNvs->LtrEnable[2] = 1;
  SaNvs->LtrEnable[3] = 1;
  SaNvs->PcieLtrMaxSnoopLatency[0] = 0x8C8;
  SaNvs->PcieLtrMaxSnoopLatency[1] = 0x8C8;
  SaNvs->PcieLtrMaxSnoopLatency[2] = 0x8C8;
  SaNvs->PcieLtrMaxSnoopLatency[3] = 0x8C8;
  SaNvs->PcieLtrMaxNoSnoopLatency[0] = 0x8C8;
  SaNvs->PcieLtrMaxNoSnoopLatency[1] = 0x8C8;
  SaNvs->PcieLtrMaxNoSnoopLatency[2] = 0x8C8;
  SaNvs->PcieLtrMaxNoSnoopLatency[3] = 0x8C8;
  SaNvs->SlotSelection = 1;
  SaNvs->CpuPcieRtd3 = 1;

  PlatformNvs->PpmFlags           = CpuNvs->PpmFlags;
  SocUpdateAcpiGnvs ((VOID *)GnvsIn);

  // TCC mode enabling
  if (FeaturePcdGet (PcdTccEnabled)) {
    TccCfgData = (TCC_CFG_DATA *) FindConfigDataByTag(CDATA_FEATURES_TAG);
    if ((TccCfgData != NULL) && (TccCfgData->TccEnable != 0)) {
      AsmCpuid (CPUID_TIME_STAMP_COUNTER, NULL, &Ebx.Uint32, NULL, NULL);
      AsmCpuid (CPUID_EXTENDED_TIME_STAMP_COUNTER, NULL, NULL, NULL, &Edx.Uint32);

      if (Edx.Bits.InvariantTsc == 1 && Ebx.Uint32 != 0) {
        DEBUG ((DEBUG_INFO, "ATSC True\n"));
        PlatformNvs->ATSC = 1;
      } else {
        DEBUG ((DEBUG_INFO, "ATSC False\n"));
        PlatformNvs->ATSC = 0;
      }
    }

    // If TCC is enabled, use the TCC policy from subregion
    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
    if((PlatformData != NULL) && PlatformData->PlatformFeatures.TccDsoTuning){
      PlatformNvs->Rtd3Support    = PlatformData->PlatformFeatures.TccRtd3Support;
      PlatformNvs->LowPowerS0Idle = PlatformData->PlatformFeatures.TccLowPowerS0Idle;
    }
  }

    // Expose Timed GPIO to OS through Nvs variables
    if (SiCfgData != NULL) {
        PchNvs->EnableTimedGpio0 = (UINT8)SiCfgData->EnableTimedGpio0;
        PchNvs->EnableTimedGpio1 = (UINT8)SiCfgData->EnableTimedGpio1;
    }
}

