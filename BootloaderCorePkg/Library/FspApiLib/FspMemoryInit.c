/** @file

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <FspApiLibInternal.h>
#include <Library/FspApiLib.h>
#include <Library/BoardInitLib.h>
#include <Library/BootloaderCoreLib.h>

/**
  This FSP API is called after TempRamInit and initializes the memory.
  This FSP API accepts a pointer to a data structure that will be platform dependent
  and defined for each FSP binary. This will be documented in Integration guide with
  each FSP release.
  After FspMemInit completes its execution, it passes the pointer to the HobList and
  returns to the boot loader from where it was called. BootLoader is responsible to
  migrate it's stack and data to Memory.
  FspMemoryInit, TempRamExit and FspSiliconInit APIs provide an alternate method to
  complete the silicon initialization and provides bootloader an opportunity to get
  control after system memory is available and before the temporary RAM is torn down.

  @param[in] FspmBase                 The base address of FSPM.
  @param[out] HobList                 Pointer to receive the address of the HOB list.

  @retval EFI_SUCCESS                 FSP execution environment was initialized successfully.
  @retval EFI_INVALID_PARAMETER       Input parameters are invalid.
  @retval EFI_UNSUPPORTED             The FSP calling conditions were not met.
  @retval EFI_DEVICE_ERROR            FSP initialization failed.
  @retval EFI_OUT_OF_RESOURCES        Stack range requested by FSP is not met.
  @retval FSP_STATUS_RESET_REQUIREDx  A reset is reuired. These status codes will not be returned during S3.
**/
EFI_STATUS
EFIAPI
CallFspMemoryInit (
  IN UINT32                  FspmBase,
  OUT VOID                   **HobList
  )
{
  UINT8                       FspmUpd[FixedPcdGet32 (PcdFSPMUpdSize)];
  UINT8                       *DefaultMemoryInitUpd;
  FSP_INFO_HEADER             *FspHeader;
  FSP_MEMORY_INIT             FspMemoryInit;
  FSPM_UPD_COMMON             *FspmUpdCommon;
  FSPM_UPD_COMMON_FSP24       *FspmUpdCommon24;
  EFI_STATUS                  Status;
  UINTN                       NewStack;

  FspHeader = (FSP_INFO_HEADER *)(UINTN)(FspmBase + FSP_INFO_HEADER_OFF);

  ASSERT (FspHeader->Signature == FSP_INFO_HEADER_SIGNATURE);
  ASSERT (FspHeader->ImageBase == FspmBase);

  // Copy default UPD data
  DefaultMemoryInitUpd = (UINT8 *)(UINTN)(FspHeader->ImageBase + FspHeader->CfgRegionOffset);
  CopyMem (&FspmUpd, DefaultMemoryInitUpd, FspHeader->CfgRegionSize);

  /* Update architectural UPD fields */
  if (FspHeader->SpecVersion < 0x24) {
    FspmUpdCommon = (FSPM_UPD_COMMON *)FspmUpd;
    FspmUpdCommon->FspmArchUpd.BootLoaderTolumSize  = 0;
    FspmUpdCommon->FspmArchUpd.BootMode             = (UINT32)GetBootMode();
    FspmUpdCommon->FspmArchUpd.NvsBufferPtr         = (UINT32)(UINTN)FindNvsData();

    NewStack = PcdGet32 (PcdFSPMStackTop);
    if (NewStack == 0xFFFFFFFF) {
      NewStack = (UINT32)FspmUpdCommon->FspmArchUpd.StackBase + (UINT32)FspmUpdCommon->FspmArchUpd.StackSize;
    }
  } else {
    FspmUpdCommon24 = (FSPM_UPD_COMMON_FSP24 *)FspmUpd;
    FspmUpdCommon24->FspmArchUpd.BootLoaderTolumSize  = 0;
    FspmUpdCommon24->FspmArchUpd.BootMode             = (UINT32)GetBootMode();

    // This must be set to NULL if ImageAttribute indicates FSP Variable support
    if ((FspHeader->ImageAttribute & IMAGE_ATTRIBUTE_VAR_SERVICES_SUPPORT) == IMAGE_ATTRIBUTE_VAR_SERVICES_SUPPORT) {
      FspmUpdCommon24->FspmArchUpd.NvsBufferPtr       = (UINTN)NULL;
    } else {
      FspmUpdCommon24->FspmArchUpd.NvsBufferPtr       = (UINTN)FindNvsData();
    }
    NewStack = PcdGet32 (PcdFSPMStackTop);
    if (NewStack == 0xFFFFFFFF) {
      NewStack = (UINT32)FspmUpdCommon24->FspmArchUpd.StackBase + (UINT32)FspmUpdCommon24->FspmArchUpd.StackSize;
    }
  }

  UpdateFspConfig (FspmUpd);

  ASSERT (FspHeader->FspMemoryInitEntryOffset != 0);
  FspMemoryInit = (FSP_MEMORY_INIT)(UINTN)(FspHeader->ImageBase + \
                                           FspHeader->FspMemoryInitEntryOffset);

  DEBUG ((DEBUG_INFO, "Call FspMemoryInit ... "));

  if (IS_X64) {
    if (NewStack != 0) {
      Status = FspmSwitchStack ((VOID *)(UINTN)FspMemoryInit, (VOID *)FspmUpd, (VOID *)HobList, (VOID *)NewStack);
    } else {
      Status = Execute32BitCode ((UINTN)FspMemoryInit, (UINTN)FspmUpd, (UINTN)HobList, FALSE);
    }
    Status = (UINTN)LShiftU64 (Status & ((UINTN)MAX_INT32 + 1), 32) | (Status & MAX_INT32);
  } else {
    if (NewStack != 0) {
      Status = FspmSwitchStack ((VOID *)(UINTN)FspMemoryInit, (VOID *)&FspmUpd, (VOID *)HobList, (VOID *)NewStack);
    } else {
      Status = FspMemoryInit (&FspmUpd, HobList);
    }
  }
  DEBUG ((DEBUG_INFO, "%r\n", Status));

  return Status;
}

/**
 * This calls the FspMultiPhaseMemInit entry point to find out if more phases of
 * memory init remain, and executes them if so.
 *
 * @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
FspMultiPhaseMemInitHandler(VOID)
{
  EFI_STATUS                                  Status;
  FSP_MULTI_PHASE_PARAMS                      MultiPhaseInitParams;
  FSP_MULTI_PHASE_GET_NUMBER_OF_PHASES_PARAMS GetNumPhasesParams;

  MultiPhaseInitParams.MultiPhaseAction   = EnumMultiPhaseGetNumberOfPhases;
  MultiPhaseInitParams.PhaseIndex         = 0;
  MultiPhaseInitParams.MultiPhaseParamPtr = (VOID*)&GetNumPhasesParams;
  GetNumPhasesParams.PhasesExecuted       = 0;
  // If FSP binary doesn't support 2.4 spec, multi phase mem init will return EFI_UNSUPPORTED
  Status = CallFspMultiPhaseMemoryInit(&MultiPhaseInitParams);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  MultiPhaseInitParams.MultiPhaseAction   = EnumMultiPhaseExecutePhase;
  MultiPhaseInitParams.MultiPhaseParamPtr = NULL;

  // Loop through all phases. Break on error status or FSP_STATUS_* not
  // handled by variable services handler
  for (MultiPhaseInitParams.PhaseIndex = 1;
    MultiPhaseInitParams.PhaseIndex < GetNumPhasesParams.NumberOfPhases &&
      !EFI_ERROR(Status) &&
      !(Status & ENCODE_RESET_REQUEST(0));
    MultiPhaseInitParams.PhaseIndex++)
  {
    Status = CallFspMultiPhaseMemoryInit(&MultiPhaseInitParams);
    ASSERT_EFI_ERROR(Status);
    Status = FspVariableHandler(Status,CallFspMultiPhaseMemoryInit);
    ASSERT_EFI_ERROR(Status);
  }
  return Status;
}

/**
  This FSP API provides multi-phase memory and silicon initialization, which brings
  greater modularity to the existing FspMemoryInit() and FspSiliconInit() API. Increased
  modularity is achieved by adding an extra API to FSP-M and FSP-S. This allows the
  bootloader to add board specific initialization steps throughout the MemoryInit and
  SiliconInit flows as needed. The FspMemoryInit() API is always called before
  FspMultiPhaseMemInit(); it is the first phase of memory initialization. Similarly, the
  FspSiliconInit() API is always called before FspMultiPhaseSiInit(); it is the first phase of
  silicon initialization. After the first phase, subsequent phases are invoked by calling the
  FspMultiPhaseMem/SiInit() API.
  The FspMultiPhaseMemInit() API may only be called after the FspMemoryInit() API and
  before the FspSiliconInit() API; or in the case that FSP-T is being used, before the
  TempRamExit() API. The FspMultiPhaseSiInit() API may only be called after the
  FspSiliconInit() API and before NotifyPhase() API; or in the case that FSP-I is being used,
  before the FspSmmInit() API. The multi-phase APIs may not be called at any other time.

  @param[in] MultiPhaseInitParamPtr   Pointer to provide multi-phase init parameters.

  @retval EFI_SUCCESS                 FSP execution environment was initialized successfully.
  @retval EFI_INVALID_PARAMETER       Input parameters are invalid.
  @retval EFI_UNSUPPORTED             The FSP calling conditions were not met.
  @retval EFI_DEVICE_ERROR            FSP initialization failed.
  @retval EFI_OUT_OF_RESOURCES        Stack range requested by FSP is not met.
  @retval FSP_STATUS_RESET_REQUIREDx  A reset is reuired. These status codes will not be returned during S3.
  @retval FSP_STATUS_VARIABLE_REQUEST An FSP variable access is required.
**/
EFI_STATUS
EFIAPI
CallFspMultiPhaseMemoryInit (
  IN FSP_MULTI_PHASE_PARAMS   *MultiPhaseInitParamPtr
  )
{
  FSP_INFO_HEADER             *FspHeader;
  FSP_MULTI_PHASE_MEM_INIT    FspMultiPhaseMemoryInit;
  EFI_STATUS                  Status;
  UINT32                      FspmBase;

  FspmBase = PCD_GET32_WITH_ADJUST (PcdFSPMBase);

  FspHeader = (FSP_INFO_HEADER *)(UINTN)(FspmBase + FSP_INFO_HEADER_OFF);

  ASSERT (FspHeader->Signature == FSP_INFO_HEADER_SIGNATURE);
  ASSERT (FspHeader->ImageBase == FspmBase);

  if (FspHeader->HeaderRevision < FSP24_HEADER_REVISION
    || FspHeader->FspMultiPhaseMemInitEntryOffset == 0)
  {
    return EFI_UNSUPPORTED;
  }

  FspMultiPhaseMemoryInit = (FSP_MULTI_PHASE_MEM_INIT)(UINTN)(FspHeader->ImageBase +
                              FspHeader->FspMultiPhaseMemInitEntryOffset);

  DEBUG ((DEBUG_INFO, "Call FspMultiPhaseMemoryInit ... "));

  if (IS_X64) {
    Status = Execute32BitCode ((UINTN)FspMultiPhaseMemoryInit, (UINTN)MultiPhaseInitParamPtr, (UINTN)NULL, FALSE);
    Status = (UINTN)LShiftU64 (Status & ((UINTN)MAX_INT32 + 1), 32) | (Status & MAX_INT32);
  } else {
    Status = FspMultiPhaseMemoryInit (MultiPhaseInitParamPtr);
  }
  DEBUG ((DEBUG_INFO, "%r\n", Status));

  return Status;
}
