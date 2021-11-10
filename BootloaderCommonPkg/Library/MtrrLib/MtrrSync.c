/** @file
  MTRR related functions.

Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Register/Intel/Cpuid.h>
#include <Register/Intel/ArchitecturalMsr.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MtrrLib.h>
#include <Library/DebugLib.h>

//
// This table defines the offset, base and length of the fixed MTRRs
//
CONST FIXED_MTRR  mMtrrLibFixedMtrrTable[] = {
  {
    MSR_IA32_MTRR_FIX64K_00000,
    0,
    SIZE_64KB
  },
  {
    MSR_IA32_MTRR_FIX16K_80000,
    0x80000,
    SIZE_16KB
  },
  {
    MSR_IA32_MTRR_FIX16K_A0000,
    0xA0000,
    SIZE_16KB
  },
  {
    MSR_IA32_MTRR_FIX4K_C0000,
    0xC0000,
    SIZE_4KB
  },
  {
    MSR_IA32_MTRR_FIX4K_C8000,
    0xC8000,
    SIZE_4KB
  },
  {
    MSR_IA32_MTRR_FIX4K_D0000,
    0xD0000,
    SIZE_4KB
  },
  {
    MSR_IA32_MTRR_FIX4K_D8000,
    0xD8000,
    SIZE_4KB
  },
  {
    MSR_IA32_MTRR_FIX4K_E0000,
    0xE0000,
    SIZE_4KB
  },
  {
    MSR_IA32_MTRR_FIX4K_E8000,
    0xE8000,
    SIZE_4KB
  },
  {
    MSR_IA32_MTRR_FIX4K_F0000,
    0xF0000,
    SIZE_4KB
  },
  {
    MSR_IA32_MTRR_FIX4K_F8000,
    0xF8000,
    SIZE_4KB
  }
};


/**
  Worker function returns the variable MTRR count for the CPU.

  @return Variable MTRR count

**/
UINT32
EFIAPI
GetVariableMtrrCount (
  VOID
  )
{
  MSR_IA32_MTRRCAP_REGISTER MtrrCap;

  MtrrCap.Uint64 = AsmReadMsr64 (MSR_IA32_MTRRCAP);
  return MtrrCap.Bits.VCNT;
}

/**
  This function gets the content in all MTRRs (variable and fixed)

  @param[out]  MtrrSetting   A buffer to hold all MTRRs content.

  @retval  EFI_INVALID_PARAMETER   MtrrSetting is NULL.
           EFI_OUT_OF_RESOURCES    Insufficient space to save all MTRR values.
           EFI_SUCCESS             MTRR values were retrieved successfully.
**/
EFI_STATUS
EFIAPI
GetCpuMtrrs (
  OUT MTRR_SETTINGS                *MtrrSetting
  )
{
  UINT32                   Index;
  UINT32                   MsrIdx;
  UINT32                   VariableMtrrCount;
  MTRR_FIXED_SETTINGS     *FixedSettings;
  MTRR_VARIABLE_SETTINGS  *VariableSettings;

  if (MtrrSetting == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (MtrrSetting, sizeof(MTRR_SETTINGS));

  FixedSettings = &MtrrSetting->Fixed;
  for (Index = 0; Index < MTRR_NUMBER_OF_FIXED_MTRR; Index++) {
    FixedSettings->Mtrr[Index] =
      AsmReadMsr64 (mMtrrLibFixedMtrrTable[Index].Msr);
  }

  VariableMtrrCount = GetVariableMtrrCount ();
  if (VariableMtrrCount > ARRAY_SIZE (VariableSettings->Mtrr)) {
    return EFI_OUT_OF_RESOURCES;
  }

  VariableSettings = &MtrrSetting->Variables;
  for (Index = 0; Index < VariableMtrrCount; Index++) {
    MsrIdx = MSR_IA32_MTRR_PHYSBASE0 + (Index << 1);
    VariableSettings->Mtrr[Index].Base = AsmReadMsr64 (MsrIdx);
    VariableSettings->Mtrr[Index].Mask = AsmReadMsr64 (MsrIdx + 1);
  }

  return EFI_SUCCESS;
}

/**
  This function sets all MTRRs (variable and fixed)

  @param[in]  MtrrSetting   A buffer to hold all MTRRs content.

  @retval  EFI_INVALID_PARAMETER   MtrrSetting is NULL.
           EFI_SUCCESS             MTRR values were programmed successfully.
**/
EFI_STATUS
EFIAPI
SetCpuMtrrs (
  IN MTRR_SETTINGS                *MtrrSetting
  )
{
  UINT32                   Index;
  UINT32                   MsrIdx;
  UINT32                   VariableMtrrCount;
  UINT64                   Value;
  MTRR_FIXED_SETTINGS     *FixedSettings;
  MTRR_VARIABLE_SETTINGS  *VariableSettings;

  if (MtrrSetting == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FixedSettings = &MtrrSetting->Fixed;
  for (Index = 0; Index < MTRR_NUMBER_OF_FIXED_MTRR; Index++) {
    MsrIdx = mMtrrLibFixedMtrrTable[Index].Msr;
    Value  = AsmReadMsr64 (MsrIdx);
    if (Value != FixedSettings->Mtrr[Index]) {
      AsmWriteMsr64 (MsrIdx, Value);
    }
  }

  VariableMtrrCount = GetVariableMtrrCount ();
  VariableMtrrCount = MIN (VariableMtrrCount, ARRAY_SIZE (VariableSettings->Mtrr));
  VariableSettings  = &MtrrSetting->Variables;
  for (Index = 0; Index < VariableMtrrCount; Index++) {
    MsrIdx = MSR_IA32_MTRR_PHYSBASE0 + (Index << 1);
    Value  = AsmReadMsr64 (MsrIdx);
    if (Value != VariableSettings->Mtrr[Index].Base) {
      AsmWriteMsr64 (MsrIdx, VariableSettings->Mtrr[Index].Base);
    }
    Value  = AsmReadMsr64 (MsrIdx + 1);
    if (Value != VariableSettings->Mtrr[Index].Mask) {
      AsmWriteMsr64 (MsrIdx + 1, VariableSettings->Mtrr[Index].Mask);
    }
  }

  return EFI_SUCCESS;
}


