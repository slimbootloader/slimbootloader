/** @file

  Copyright (c) 2016 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _FSP_API_LIB_H_
#define _FSP_API_LIB_H_

#include <FspEas/FspApi.h>

typedef EFI_STATUS (EFIAPI *FSP_MULTI_PHASE_FUNCTION)(FSP_MULTI_PHASE_PARAMS *MultiPhaseInitParamPtr);

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
  @param[out] HobListPtr              Pointer to receive the address of the HOB list.

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
  UINT32                     FspmBase,
  VOID                       **HobList
  );

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
  IN FSP_MULTI_PHASE_PARAMS     *MultiPhaseInitParamPtr
  );

/**
  This FSP API is called after FspMemoryInit API. This FSP API tears down the temporary
  memory setup by TempRamInit API. This FSP API accepts a pointer to a data structure
  that will be platform dependent and defined for each FSP binary. This will be
  documented in Integration Guide.
  FspMemoryInit, TempRamExit and FspSiliconInit APIs provide an alternate method to
  complete the silicon initialization and provides bootloader an opportunity to get
  control after system memory is available and before the temporary RAM is torn down.

  @param[in] FspmBase            The base address of FSPM.
  @param[in] Params              Pointer to the Temp Ram Exit parameters structure.
                                 This structure is normally defined in the Integration Guide.
                                 And if it is not defined in the Integration Guide, pass NULL.

  @retval EFI_SUCCESS            FSP execution environment was initialized successfully.
  @retval EFI_INVALID_PARAMETER  Input parameters are invalid.
  @retval EFI_UNSUPPORTED        The FSP calling conditions were not met.
  @retval EFI_DEVICE_ERROR       FSP initialization failed.
**/
EFI_STATUS
EFIAPI
CallFspTempRamExit (
  UINT32              FspmBase,
  VOID                *Params
  );


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
  );

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
  );

/**
  This FSP API is used to notify the FSP about the different phases in the boot process.
  This allows the FSP to take appropriate actions as needed during different initialization
  phases. The phases will be platform dependent and will be documented with the FSP
  release. The current FSP supports following notify phases:
    Post PCI enumeration
    Ready To Boot
    End of firmware

  @param[in] Phase              Phase parameter for FspNotifyPhase

  @retval EFI_SUCCESS           The notification was handled successfully.
  @retval EFI_UNSUPPORTED       The notification was not called in the proper order.
  @retval EFI_INVALID_PARAMETER The notification code is invalid.
**/
EFI_STATUS
EFIAPI
CallFspNotifyPhase (
  FSP_INIT_PHASE  Phase
  );


/**
  This FSP API is used to notify the FSP about the different phases in the boot process.
  This allows the FSP to take appropriate actions as needed during different initialization
  phases. The phases will be platform dependent and will be documented with the FSP
  release. The current FSP supports following notify phases:
    Post PCI enumeration
    Ready To Boot
    End of firmware

  @param[in] Delta              The delta between the new and old base.
                                Delta = NewBase - OldBase

  @retval EFI_SUCCESS           The rebasing was handled successfully.

**/
EFI_STATUS
EFIAPI
RebaseFspComponent (
  UINT32   Delta
  );

/**
  This function handles variable services requests from FspMemoryInit and FspSiliconInit.
  This should be called immediately after returning from the respective FSP entrypoint.

  @param[in] FspStatus            The status returned by the FSP entry point.
  @param[in] MultiPhaseFunction   The Multiphase function for this FSP phase (Mem vs Si)

  @retval Status
 */
EFI_STATUS
EFIAPI
FspVariableHandler (
  IN EFI_STATUS                FspStatus,
  IN FSP_MULTI_PHASE_FUNCTION  MultiPhaseFunction
  );

/**
 * This calls the FspMultiPhaseMemInit entry point to find out if more phases of
 * memory init remain, and executes them if so.
 *
 * @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
FspMultiPhaseMemInitHandler (VOID);

/**
 * This calls the FspMultiPhaseSiliconInit entry point to find out
 * if more phases of Silicon init remain, and executes them if so.
 *
 * @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
FspMultiPhaseSiliconInitHandler (VOID);

/**
  This function will handle FSP reset request.

  @param[in] Status     FSP API return status.

**/
VOID
EFIAPI
FspResetHandler (
  IN  EFI_STATUS   Status
  );

/**
  Wrapper for a thunk  to transition from long mode to compatibility mode
  to execute 32-bit code and then transit back to long mode.

  @param[in] Function     The 32bit code entry to be executed.
  @param[in] Param1       The first parameter to pass to 32bit code.
  @param[in] Param2       The second parameter to pass to 32bit code.
  @param[in] ExeInMem     If thunk needs to be executed from memory copy.

  @return EFI_STATUS
**/
EFI_STATUS
EFIAPI
Execute32BitCode (
  IN UINT64      Function,
  IN UINT64      Param1,
  IN UINT64      Param2,
  IN BOOLEAN     ExeInMem
  );

#endif
