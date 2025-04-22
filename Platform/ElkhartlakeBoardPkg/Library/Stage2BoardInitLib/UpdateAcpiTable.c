/** @file

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiPei.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BdatLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/DebugLib.h>
#include <Library/DmaRemappingTable.h>
#include <Library/PchInfoLib.h>
#include <Library/SocInitLib.h>
#include <Library/TpmLib.h>
#include <Library/VTdLib.h>
#include <CpuDataStruct.h>
#include <CpuPowerMgmt.h>
#include <GlobalNvsAreaDef.h>
#include <Library/TccLib.h>
#include <PowerMgmtNvsStruct.h>
#include <Register/CpuRegs.h>
#include <Library/PsdLib.h>
#include <Library/ConfigDataLib.h>
#include "ConfigDataStruct.h"
#include <Lpit.h>
#include <Register/PmcRegs.h>
#include <Library/PchSciLib.h>
#include <PlatformData.h>

#define NHLT_ACPI_TABLE_SIGNATURE  SIGNATURE_32 ('N', 'H', 'L', 'T')

//
// FVID Table Information
// Default FVID table
// One header field plus states
//
#define NATIVE_PSTATE_LATENCY           10
#define PSTATE_BM_LATENCY               10

FVID_TABLE  *mFvidPointer               = NULL;


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
  CurrPtr     = (UINT8 *) Table;
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
  Patch the native _PSS package with the EIST values
  Uses ratio/VID values from the FVID table to fix up the control values in the _PSS.

  (1) Find _PSS package:
    (1.1) Find the _PR_CPU0 scope.
    (1.2) Save a pointer to the package length.
    (1.3) Find the _PSS AML name object.
  (2) Resize the _PSS package.
  (3) Fix up the _PSS package entries
    (3.1) Check Turbo mode support.
    (3.2) Check Dynamic FSB support.
  (4) Fix up the Processor block and \_PR_CPU0 Scope length.
  (5) Update SSDT Header with new length.

  @retval EFI_SUCCESS   - on success
  @retval EFI_NOT_FOUND - if _PR_.CPU0 scope is not foud in the ACPI tables
**/
EFI_STATUS
AcpiPatchPss (
  EFI_ACPI_DESCRIPTION_HEADER   *Table,
  IN GLOBAL_NVS_AREA            *GlobalNvs
  )
{
  UINT8              *CurrPtr;
  UINT8              *EndOfTable;
  UINT8              Index;
  UINT16             NewPackageLength;
  UINT16             LpssMaxPackageLength;
  UINT16             TpssMaxPackageLength;
  UINT16             Temp;
  UINT16             *PackageLength;
  UINT16             *ScopePackageLengthPtr;
  UINT32             *Signature;
  PSS_PACKAGE_LAYOUT *PssPackage;
  MSR_REGISTER       TempMsr;
  UINT16             MaximumEfficiencyRatio;
  UINT16             MaximumNonTurboRatio;
  UINT16             PnPercent;
  UINT16             mNumberOfStates;
  CPU_NVS_AREA       *CpuNvs;

  CpuNvs      = (CPU_NVS_AREA *) &GlobalNvs->CpuNvs;

  ScopePackageLengthPtr = NULL;
  PssPackage            = NULL;

  //
  // Get Maximum Efficiency bus ratio (LFM) from Platform Info MSR Bits[47:40]
  // Get Maximum Non Turbo bus ratio from Platform Info MSR Bits[15:8]
  //
  TempMsr.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  MaximumEfficiencyRatio = TempMsr.Bytes.SixthByte;
  MaximumNonTurboRatio = TempMsr.Bytes.SecondByte;
  mNumberOfStates = mFvidPointer[0].FvidHeader.EistStates;

  DEBUG ((DEBUG_INFO, "FVID number of states: %d\n", mNumberOfStates));

  ///
  /// Calculate new package length
  ///
  NewPackageLength      = Temp = (UINT16) (mNumberOfStates * sizeof (PSS_PACKAGE_LAYOUT) + 3);
  LpssMaxPackageLength  = (UINT16) (LPSS_FVID_MAX_STATES * sizeof (PSS_PACKAGE_LAYOUT) + 3);
  TpssMaxPackageLength  = (UINT16) (TPSS_FVID_MAX_STATES * sizeof (PSS_PACKAGE_LAYOUT) + 3);
  ///
  /// Locate the SSDT package in the IST table
  ///
  CurrPtr     = (UINT8 *) Table;
  EndOfTable  = (UINT8 *) (CurrPtr + Table->Length);
  for (CurrPtr = (UINT8 *) Table; CurrPtr <= EndOfTable; CurrPtr++) {
    Signature = (UINT32 *) (CurrPtr + 1);
    ///
    /// If we find the _SB_PR00 scope, save a pointer to the package length
    ///
    if ((*CurrPtr == AML_SCOPE_OP) &&
        (*(Signature + 1) == SIGNATURE_32 ('_', 'S', 'B', '_')) &&
        (*(Signature + 2) == SIGNATURE_32 ('P', 'R', '0', '0'))
        ) {
      ScopePackageLengthPtr = (UINT16 *) (CurrPtr + 1);
    }
    ///
    /// Patch the LPSS package with 16 P-states for _PSS Method
    ///
    if ((*CurrPtr == AML_NAME_OP) && (*Signature == SIGNATURE_32 ('L', 'P', 'S', 'S'))) {
      //
      //  Check table dimensions.
      //  PSS package reserve space for LPSS_FVID_MAX_STATES number of P-states so check if the
      //  current number of P- states is more than LPSS_FVID_MAX_STATES. Also need to update the SSDT contents
      //  if the current number of P-states is less than LPSS_FVID_MAX_STATES.
      //
      PackageLength  = (UINT16 *) (CurrPtr + 6);

      if (mNumberOfStates > LPSS_FVID_MAX_STATES) {
        *(CurrPtr + 8)  = (UINT8) LPSS_FVID_MAX_STATES;
        ///
        /// Update the Package length in AML package length format
        ///
        *PackageLength = ((LpssMaxPackageLength & 0x0F) | 0x40) | ((LpssMaxPackageLength << 4) & 0x0FF00);
      } else {
        *(CurrPtr + 8)  = (UINT8) mNumberOfStates;
        ///
        /// Update the Package length in AML package length format
        ///
        *PackageLength = ((NewPackageLength & 0x0F) | 0x40) | ((Temp << 4) & 0x0FF00);
        ///
        /// Move SSDT contents
        ///
        CopyMem ((CurrPtr + NewPackageLength), (CurrPtr + LpssMaxPackageLength), EndOfTable - (CurrPtr + LpssMaxPackageLength));

        ///
        /// Save the new end of the SSDT
        ///
        EndOfTable = EndOfTable - (LpssMaxPackageLength - NewPackageLength);
      }

      PssPackage = (PSS_PACKAGE_LAYOUT *) (CurrPtr + 9);
      if (mNumberOfStates > LPSS_FVID_MAX_STATES) {
        for (Index = 1; Index <= LPSS_FVID_MAX_STATES; Index++) {
          ///
          /// Update the _PSS table. If Turbo mode is supported, add one to the Max Non-Turbo frequency
          ///
          if ((CpuNvs->PpmFlags & PPM_TURBO) && (Index == 1)) {
            PssPackage->CoreFrequency = (UINT32) ((mFvidPointer[Index + 1].FvidState.Limit16BusRatio)* 100) + 1;
          } else if (mFvidPointer[Index].FvidState.Limit16BusRatio < MaximumEfficiencyRatio) {
            //
            // If cTDP Down Ratio == LFM, set it to 1% lower than LFM.
            //
            PnPercent = (MaximumEfficiencyRatio * 100) / MaximumNonTurboRatio;
            PssPackage->CoreFrequency = (MaximumNonTurboRatio * (PnPercent - 1)); // Simplified Calculation.
          } else {
            PssPackage->CoreFrequency = (UINT32) (mFvidPointer[Index].FvidState.Limit16BusRatio) * 100;
          }
          PssPackage->Power = (UINT32) mFvidPointer[Index].FvidState.Limit16Power;
          ///
          /// If it's PSS table, Control is the PERF_CTL value.
          /// Status entry is the same as control entry.
          /// TransLatency uses 10
          ///
          PssPackage->TransLatency  = NATIVE_PSTATE_LATENCY;
          PssPackage->Control       = (UINT32) LShiftU64 (mFvidPointer[Index].FvidState.Limit16BusRatio, 8);
          //
          // Ensure any future OS would not look for the IA32_PERF_STATUS MSR to check if the value matches
          //
          if (mFvidPointer[Index].FvidState.Limit16BusRatio < MaximumEfficiencyRatio) {
            PssPackage->Status        = (UINT32) LShiftU64 (MaximumEfficiencyRatio, 8);
          } else {
            PssPackage->Status        = (UINT32) LShiftU64 (mFvidPointer[Index].FvidState.Limit16BusRatio, 8);
          }
          PssPackage->BmLatency     = PSTATE_BM_LATENCY;
          PssPackage++;
        }
      } else {
        for (Index = 1; Index <= mNumberOfStates; Index++) {
          ///
          /// Update the _PSS table. If Turbo mode is supported, add one to the Max Non-Turbo frequency
          ///
          if ((CpuNvs->PpmFlags & PPM_TURBO) && (Index == 1)) {
            PssPackage->CoreFrequency = (UINT32) ((mFvidPointer[Index + 1].FvidState.BusRatio)* 100) + 1;
          } else if (mFvidPointer[Index].FvidState.BusRatio < MaximumEfficiencyRatio) {
            //
            // If cTDP Down Ratio == LFM, set it to 1% lower than LFM.
            //
            PnPercent = (MaximumEfficiencyRatio * 100) / MaximumNonTurboRatio;
            PssPackage->CoreFrequency = (MaximumNonTurboRatio * (PnPercent - 1)); // Simplified Calculation.
          } else {
            PssPackage->CoreFrequency = (UINT32) (mFvidPointer[Index].FvidState.BusRatio) * 100;
          }
          PssPackage->Power = (UINT32) mFvidPointer[Index].FvidState.Power;
          ///
          /// If it's PSS table, Control is the PERF_CTL value.
          /// Status entry is the same as control entry.
          /// TransLatency uses 10
          ///
          PssPackage->TransLatency  = NATIVE_PSTATE_LATENCY;
          PssPackage->Control       = (UINT32) LShiftU64 (mFvidPointer[Index].FvidState.BusRatio, 8);
          //
          // Ensure any future OS would not look for the IA32_PERF_STATUS MSR to check if the value matches
          //
          if (mFvidPointer[Index].FvidState.BusRatio < MaximumEfficiencyRatio) {
            PssPackage->Status        = (UINT32) LShiftU64 (MaximumEfficiencyRatio, 8);
          } else {
            PssPackage->Status        = (UINT32) LShiftU64 (mFvidPointer[Index].FvidState.BusRatio, 8);
          }
          PssPackage->BmLatency     = PSTATE_BM_LATENCY;
          PssPackage++;
        }
      }
    }

    ///
    /// Patch the TPSS package with no limit P-states for _PSS Method
    ///
    if ((*CurrPtr == AML_NAME_OP) && (*Signature == SIGNATURE_32 ('T', 'P', 'S', 'S'))) {
      ASSERT (mNumberOfStates <= TPSS_FVID_MAX_STATES);
      if (mNumberOfStates <= TPSS_FVID_MAX_STATES) {

        *(CurrPtr + 8)  = (UINT8) mNumberOfStates;
        PackageLength   = (UINT16 *) (CurrPtr + 6);
        ///
        /// Update the Package length in AML package length format
        ///
        *PackageLength = ((NewPackageLength & 0x0F) | 0x40) | ((Temp << 4) & 0x0FF00);
        ///
        /// Move SSDT contents
        ///
        CopyMem ((CurrPtr + NewPackageLength), (CurrPtr + TpssMaxPackageLength), EndOfTable - (CurrPtr + TpssMaxPackageLength));
        ///
        /// Save the new end of the SSDT
        ///
        EndOfTable = EndOfTable - (TpssMaxPackageLength - NewPackageLength);
      }

      PssPackage = (PSS_PACKAGE_LAYOUT *) (CurrPtr + 9);
      for (Index = 1; Index <= mNumberOfStates; Index++) {
        ///
        /// Update the _PSS table. If Turbo mode is supported, add one to the Max Non-Turbo frequency
        ///
        if ((CpuNvs->PpmFlags & PPM_TURBO) && (Index == 1)) {
          PssPackage->CoreFrequency = (UINT32) ((mFvidPointer[Index + 1].FvidState.BusRatio)* 100) + 1;
        } else if (mFvidPointer[Index].FvidState.BusRatio < MaximumEfficiencyRatio) {
          //
          // If cTDP Down Ratio == LFM, set it to 1% lower than LFM.
          //
          PnPercent = (MaximumEfficiencyRatio * 100) / MaximumNonTurboRatio;
          PssPackage->CoreFrequency = (MaximumNonTurboRatio * (PnPercent - 1)); // Simplified Calculation.
        } else {
          PssPackage->CoreFrequency = (UINT32) (mFvidPointer[Index].FvidState.BusRatio) * 100;
        }
        PssPackage->Power = (UINT32) mFvidPointer[Index].FvidState.Power;
        ///
        /// If it's PSS table, Control is the PERF_CTL value.
        /// Status entry is the same as control entry.
        /// TransLatency uses 10
        ///
        PssPackage->TransLatency  = NATIVE_PSTATE_LATENCY;
        PssPackage->Control       = (UINT32) LShiftU64 (mFvidPointer[Index].FvidState.BusRatio, 8);
        //
        // Ensure any future OS would not look for the IA32_PERF_STATUS MSR to check if the value matches
        //
        if (mFvidPointer[Index].FvidState.BusRatio < MaximumEfficiencyRatio) {
          PssPackage->Status        = (UINT32) LShiftU64 (MaximumEfficiencyRatio, 8);
        } else {
          PssPackage->Status        = (UINT32) LShiftU64 (mFvidPointer[Index].FvidState.BusRatio, 8);
        }
        PssPackage->BmLatency     = PSTATE_BM_LATENCY;
        PssPackage++;
      }
    }
  }
  ASSERT (ScopePackageLengthPtr != NULL);
  if (ScopePackageLengthPtr == NULL) {
    return EFI_NOT_FOUND;
  }
  ///
  /// Update the Package length in AML package length format
  ///
  CurrPtr                 = (UINT8 *) ScopePackageLengthPtr;
  NewPackageLength        = Temp = (UINT16) (EndOfTable - CurrPtr);
  *ScopePackageLengthPtr  = ((NewPackageLength & 0x0F) | 0x40) | ((Temp << 4) & 0x0FF00);
  Table->Length   = (UINT32) (EndOfTable - (UINT8 *) Table);

  return EFI_SUCCESS;
}
/**
  Update PCH NVS and SA NVS area address and size in ACPI table

  @param[in] Current       Pointer to ACPI description header

  @retval   EFI_SUCCESS

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
  EFI_STATUS                   Status;
  VOID                        *FspHobList;
  EFI_ACPI_6_1_FIXED_ACPI_DESCRIPTION_TABLE *FadtPointer;

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
    PatchCpuSsdtTable (Table, GlobalNvs);
  } else if (Table->OemTableId == SIGNATURE_64 ('C', 'p', 'u', '0', 'I', 's', 't', 0)) {
    if (GetBootMode () != BOOT_ON_FLASH_UPDATE) {
      AcpiPatchPss (Table, GlobalNvs);
    }
  } else if (Table->Signature == EFI_BDAT_TABLE_SIGNATURE) {
    FspHobList = GetFspHobListPtr ();
    if (FspHobList != NULL) {
      UpdateBdatAcpiTable (Table, FspHobList);
      DEBUG ((DEBUG_INFO, "Updated BDAT Table in AcpiTable Entries\n"));
    }
  } else if (Table->Signature == EFI_ACPI_6_1_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
    DEBUG ( (DEBUG_INFO, "Updated FADT Table entries in AcpiTable\n") );
    FadtPointer = (EFI_ACPI_6_1_FIXED_ACPI_DESCRIPTION_TABLE *) Table;
    //
    // The Flags field within the FADT (offset 112)
    //   1) will have a new Low Power S0 Idle Capable ACPI flag (bit offset 21).
    //
    if (S0IX_STATUS()) {
      DEBUG ( (DEBUG_INFO, "Enabled S0ix ACPI flag\n") );
      FadtPointer->Flags = (EFI_ACPI_6_1_LOW_POWER_S0_IDLE_CAPABLE | FadtPointer->Flags);
    }
  } else if (Table->Signature == EFI_ACPI_6_1_LOW_POWER_IDLE_TABLE_STRUCTURE_SIGNATURE){
      UINT8                                  LpitStateEntries = 0;
      EFI_ACPI_6_1_GENERIC_ADDRESS_STRUCTURE SetResidencyCounter[2] = { ACPI_LPI_RES_SLP_S0_COUNTER, ACPI_LPI_RES_C10_COUNTER };
      UINT64                                 ResidencyCounterFrequency = 0;
      DEBUG ((DEBUG_INFO, "Updated LPIT Table in AcpiTable Entries\n"));
      LpitStateEntries = (UINT8)(((EFI_ACPI_DESCRIPTION_HEADER *)Table)->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(ACPI_LPI_NATIVE_CSTATE_DESCRIPTOR);
      if (LpitStateEntries != 0) {
        //
        // LpitResidencyCounter : 0 - SLP_S0 Based, 1 - C10 Counter, 2 - ATX Shutdown/PS_ON
        //
          //
          // Read PWRM Base Address to fill in Residency counter Address Space
          //
          SetResidencyCounter[0].Address = (UINT64)PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_BIOS_SCRATCHPAD_2;
          ResidencyCounterFrequency = 32678; //Counter runs at 100us granularity which implies 10KHz frequency (10000Hz)

        (((ACPI_LOW_POWER_IDLE_TABLE *)Table)->LpiStates[LpitStateEntries - 1].ResidencyCounter) = SetResidencyCounter[0];
        (((ACPI_LOW_POWER_IDLE_TABLE *)Table)->LpiStates[LpitStateEntries - 1].ResidencyCounterFrequency) = ResidencyCounterFrequency;
      }
    } else if (Table->Signature == EFI_ACPI_VTD_DMAR_TABLE_SIGNATURE) {
    if (FeaturePcdGet (PcdVtdEnabled)) {
      DEBUG ((DEBUG_INFO, "Updated DMAR Table in AcpiTable Entries\n"));
      UpdateDmarAcpi (Table);
    }
  } else if ((Table->Signature == EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE) ||
        (Table->OemTableId == ACPI_SSDT_TPM2_DEVICE_OEM_TABLE_ID)) {
    if (MEASURED_BOOT_ENABLED ()) {
      Status = UpdateTpm2AcpiTable (Table);
      ASSERT_EFI_ERROR (Status);
    }
  } else if (Table->Signature == EFI_ACPI_PSD_SIGNATURE) {
    if (FeaturePcdGet (PcdPsdBiosEnabled)) {
      PSD_CFG_DATA *PsdCfgData = NULL;
      PsdCfgData = (PSD_CFG_DATA *)FindConfigDataByTag (CDATA_PSD_TAG);
      if (PsdCfgData != NULL) {
        if (PsdCfgData->EnablePsd == 1) {
          UpdateAcpiPsdTable ((VOID* )Current);
          DEBUG ((DEBUG_INFO, "Updated Psd Table in AcpiTable Entries\n"));
        }
      }
    }
  }
  return EFI_SUCCESS;
}
