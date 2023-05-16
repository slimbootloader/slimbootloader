/** @file

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <FspApiLibInternal.h>
#include <Library/BoardInitLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/FspApiLib.h>


/**
  This FSP API is called after TempRamExit API.
  FspMemoryInit, TempRamExit and FspSiliconInit APIs provide an alternate method to complete the
  silicon initialization.

  @retval EFI_SUCCESS                 FSP execution environment was initialized successfully.
  @retval EFI_INVALID_PARAMETER       Input parameters are invalid.
  @retval EFI_UNSUPPORTED             The FSP calling conditions were not met.
  @retval EFI_DEVICE_ERROR            FSP initialization failed.
  @retval FSP_STATUS_RESET_REQUIREDx  A reset is reuired. These status codes will not be returned during S3.
**/
EFI_STATUS
EFIAPI
CallFspSiliconInit (
  VOID
  )
{
  VOID                       *FspsUpdptr;
  UINT8                      *DefaultSiliconInitUpd;
  FSP_INFO_HEADER            *FspHeader;
  FSP_SILICON_INIT            FspSiliconInit;
  EFI_STATUS                  Status;

  FspHeader = (FSP_INFO_HEADER *)(UINTN)(PcdGet32 (PcdFSPSBase) + FSP_INFO_HEADER_OFF);

  ASSERT (FspHeader->Signature == FSP_INFO_HEADER_SIGNATURE);
  ASSERT (FspHeader->ImageBase == PcdGet32 (PcdFSPSBase));
  FspsUpdptr = AllocatePool (FspHeader->CfgRegionSize);
  if (FspsUpdptr == NULL) {
      return EFI_OUT_OF_RESOURCES;
  }
  (VOID) PcdSet32S (PcdFspsUpdPtr,(UINT32)(UINTN)FspsUpdptr);
  // Copy default UPD data
  DefaultSiliconInitUpd = (UINT8 *)(UINTN)(FspHeader->ImageBase + FspHeader->CfgRegionOffset);
  CopyMem (FspsUpdptr, DefaultSiliconInitUpd, FspHeader->CfgRegionSize);

  /* Update architectural UPD fields */
  UpdateFspConfig (FspsUpdptr);

  ASSERT (FspHeader->FspSiliconInitEntryOffset != 0);
  FspSiliconInit = (FSP_SILICON_INIT)(UINTN)(FspHeader->ImageBase + \
                                             FspHeader->FspSiliconInitEntryOffset);

  DEBUG ((DEBUG_INFO, "Call FspSiliconInit ... \n"));
  if (IS_X64) {
    Status = Execute32BitCode ((UINTN)FspSiliconInit,(UINTN) FspsUpdptr, (UINTN)0, FALSE);
    Status = (UINTN)LShiftU64 (Status & ((UINTN)MAX_INT32 + 1), 32) | (Status & MAX_INT32);
  } else {
    Status = FspSiliconInit (FspsUpdptr);
  }
  DEBUG ((DEBUG_INFO, "%r\n", Status));

  return Status;
}

/**
 * This calls the FspMultiPhaseSiliconInit entry point to find out
 * if more phases of Silicon init remain, and executes them if so.
 *
 * @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
FspMultiPhaseSiliconInitHandler(VOID)
{
  EFI_STATUS                                  Status;
  FSP_MULTI_PHASE_PARAMS                      MultiPhaseInitParams;
  FSP_MULTI_PHASE_GET_NUMBER_OF_PHASES_PARAMS GetNumPhasesParams;

  MultiPhaseInitParams.MultiPhaseAction   = EnumMultiPhaseGetNumberOfPhases;
  MultiPhaseInitParams.PhaseIndex         = 0;
  MultiPhaseInitParams.MultiPhaseParamPtr = (VOID*)&GetNumPhasesParams;
  GetNumPhasesParams.PhasesExecuted       = 0;

  Status = CallFspMultiPhaseSiliconInit(&MultiPhaseInitParams);
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
    Status = CallFspMultiPhaseSiliconInit(&MultiPhaseInitParams);
    ASSERT_EFI_ERROR(Status);
    Status = FspVariableHandler(Status,CallFspMultiPhaseSiliconInit);
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
CallFspMultiPhaseSiliconInit (
  IN FSP_MULTI_PHASE_PARAMS     *MultiPhaseInitParamPtr
  )
{
  FSP_INFO_HEADER             *FspHeader;
  FSP_MULTI_PHASE_MEM_INIT    FspMultiPhaseSiliconInit;
  EFI_STATUS                  Status;
  UINT32                      FspsBase;

  FspsBase = PcdGet32 (PcdFSPSBase);

  FspHeader = (FSP_INFO_HEADER *)(UINTN)(FspsBase + FSP_INFO_HEADER_OFF);

  ASSERT (FspHeader->Signature == FSP_INFO_HEADER_SIGNATURE);
  ASSERT (FspHeader->ImageBase == FspsBase);

  // Multi Phase silicon init support added in FSP 2.2
  if (FspHeader->HeaderRevision < FSP22_HEADER_REVISION || FspHeader->FspMultiPhaseSiInitEntryOffset == 0) {
    return EFI_UNSUPPORTED;
  }

  FspMultiPhaseSiliconInit = (FSP_MULTI_PHASE_MEM_INIT)(UINTN)(FspHeader->ImageBase + \
                               FspHeader->FspMultiPhaseSiInitEntryOffset);

  DEBUG ((DEBUG_INFO, "Call FspMultiPhaseSiliconInit ... "));

  if (IS_X64) {
    Status = Execute32BitCode ((UINTN)FspMultiPhaseSiliconInit, (UINTN)MultiPhaseInitParamPtr, (UINTN)NULL, FALSE);
    Status = (UINTN)LShiftU64 (Status & ((UINTN)MAX_INT32 + 1), 32) | (Status & MAX_INT32);
  } else {
    Status = FspMultiPhaseSiliconInit (MultiPhaseInitParamPtr);
  }
  DEBUG ((DEBUG_INFO, "%r\n", Status));

  return Status;
}
