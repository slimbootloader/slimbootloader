/** @file

  Copyright (c) 2021 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/StageCommonBoardInitLib.h>
#include <Library/PagingLib.h>
#include <Register/CpuRegs.h>
#include <CpuDataStruct.h>

/**
  Initialize Variable.

**/
VOID
VariableInitialize (
  VOID
  )
{
  UINT32                    RgnBase;
  UINT32                    RgnSize;
  EFI_STATUS                Status;

  RgnBase = 0;
  RgnSize = 0;

  Status = GetComponentInfo (FLASH_MAP_SIG_VARIABLE, &RgnBase, &RgnSize);
  if (!EFI_ERROR (Status)) {
    Status = VariableConstructor (RgnBase, RgnSize);

DEBUG_CODE_BEGIN ();
    if (!EFI_ERROR (Status)) {
      UINTN   VariableLen;
      CHAR8   Buffer[VAR_LENGTH_VAR_STS + 1];

      VariableLen = VAR_LENGTH_VAR_STS;
      Status = GetVariable (VAR_NAME_VAR_STS, NULL, NULL, &VariableLen, Buffer);
      if (!EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "VariableInitialize: %s %a(%d)\n", VAR_NAME_VAR_STS, Buffer, VariableLen));
      } else if (Status == EFI_NOT_FOUND) {
        Status = AsciiStrCpyS (Buffer, VAR_LENGTH_VAR_STS, "Initialized!\0");
        if (!EFI_ERROR (Status)) {
          Status = SetVariable (VAR_NAME_VAR_STS, NULL, 0, AsciiStrLen (Buffer), Buffer);
        }
      }
    }
DEBUG_CODE_END ();
  }
}

/**
  Initial CodeRegion covers only TopSwap region only.
  This will enlarge CodeRegion cache after TempRamInit and TempRamExit.

**/
VOID
EFIAPI
EnlargeCodeRegionCache (
  VOID
  )
{
  UINT8                     LoaderStage;
  UINT8                     PhysicalAddressBits;
  UINT32                    Index;
  UINT32                    MtrrNumber;
  UINT32                    VariableMtrrCount;
  UINT64                    TempQword;
  UINT64                    MemoryLength;
  UINT64                    ValidMtrrAddressMask;
  EFI_PHYSICAL_ADDRESS      MemoryAddress;
  MSR_IA32_MTRRCAP_REGISTER MtrrCap;

  //
  // Assume this should be done at TempRamInit and TempRamExit only
  //
  LoaderStage = GetLoaderStage ();
  if (LoaderStage >= LOADER_STAGE_2) {
    return;
  }

  // Get Valid MTRR Address Mask
  PhysicalAddressBits  = GetPhysicalAddressBits ();
  ValidMtrrAddressMask = (LShiftU64((UINT64) 1, PhysicalAddressBits) - 1) & (~(UINT64)0x0FFF);

  // Current the number of Variable MTRRs
  MtrrCap.Uint64 = AsmReadMsr64 (MSR_IA32_MTRRCAP);
  VariableMtrrCount = MtrrCap.Bits.VCNT;

  // Search for the same region as TempRamInitParams
  MemoryLength  = (UINT64)INITIAL_CODE_REGION_LENGTH;
  MemoryAddress = (EFI_PHYSICAL_ADDRESS)INTTIAL_CODE_REGION_BASE;
  TempQword = MemoryAddress & ValidMtrrAddressMask;
  for (Index = 0; Index < VariableMtrrCount; Index++) {
    MtrrNumber = MSR_IA32_MTRR_PHYSBASE0 + (Index << 1);
    if ((AsmReadMsr64 (MtrrNumber) & ValidMtrrAddressMask) == TempQword) {
      break;
    }
  }
  // If searched one exist, update it. Otherwise, add new one
  MtrrNumber = MSR_IA32_MTRR_PHYSBASE0 + (Index << 1);

  //
  // At TempRamInit, cover up-to Stage1B region from Redundant A or B
  // At TempRanExit, cover full(SBL) flash region
  //
  if (LoaderStage == LOADER_STAGE_1A) {
    MemoryAddress = (EFI_PHYSICAL_ADDRESS)ALIGN_DOWN (PCD_GET32_WITH_ADJUST (PcdCfgDataBase), SIZE_1MB);
    MemoryLength  = (UINT64)(SIZE_4GB - MemoryAddress);
  } else {
    MemoryLength  = (UINT64)ALIGN_UP (PcdGet32 (PcdFlashSize), SIZE_1MB);
    MemoryAddress = (EFI_PHYSICAL_ADDRESS)(SIZE_4GB - MemoryLength);
  }

  TempQword = (MemoryAddress & ValidMtrrAddressMask) | EFI_CACHE_WRITEPROTECTED;
  AsmWriteMsr64 (MtrrNumber, TempQword);
  TempQword = (~(MemoryLength - 1) & ValidMtrrAddressMask) | B_EFI_CACHE_MTRR_VALID;
  AsmWriteMsr64 (MtrrNumber + 1, TempQword);
}
