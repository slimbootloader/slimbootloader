/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Uefi.h>
#include <FspApiLibInternal.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/FspApiLib.h>
#include <Library/VariableLib.h>

/**
  This function handles variable services requests from FspMemoryInit and FspSiliconInit.
  This should be called immediately after returning from the respective FSP entrypoint.

  @param[in] FspStatus            The status returned by the FSP entry point.
  @param[in] MultiPhaseFunction   The Multiphase function for this FSP phase (Mem vs Si)

  @retval Status
 */
EFI_STATUS
EFIAPI
FspVariableHandler(
  IN EFI_STATUS                FspStatus,
  IN FSP_MULTI_PHASE_FUNCTION  MultiPhaseFunction
  )
{
  EFI_STATUS                                    Status;
  FSP_MULTI_PHASE_PARAMS                        MultiPhaseInitParams;
  FSP_MULTI_PHASE_VARIABLE_REQUEST_INFO_PARAMS  *FspVarReqParams;

  Status = EFI_SUCCESS;

  while (FspStatus == FSP_STATUS_VARIABLE_REQUEST) {
    DEBUG((DEBUG_VERBOSE, "FspVariableHandler: Got FSP_STATUS_VARIABLE_REQUEST\n"));
    // Request details of variable request from Multi-phase API.
    MultiPhaseInitParams.MultiPhaseAction = EnumMultiPhaseGetVariableRequestInfo;
    MultiPhaseInitParams.PhaseIndex = 0;
    Status = MultiPhaseFunction(&MultiPhaseInitParams);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
      break;
    }

    FspVarReqParams = (FSP_MULTI_PHASE_VARIABLE_REQUEST_INFO_PARAMS*)MultiPhaseInitParams.MultiPhaseParamPtr;
    ASSERT (FspVarReqParams != NULL);
    if (FspVarReqParams == NULL) {
      Status = EFI_UNSUPPORTED;
      break;
    }

    DEBUG((DEBUG_VERBOSE, "FSP Variable Request %d\n", FspVarReqParams->VariableRequest));
    if (NULL != FspVarReqParams->VariableName) {
      DEBUG((DEBUG_VERBOSE, "\tVariableName %s\n", FspVarReqParams->VariableName));
    }
    if (NULL != FspVarReqParams->VariableGuid) {
      DEBUG((DEBUG_VERBOSE, "\tVariableGuid %g\n", FspVarReqParams->VariableGuid));
    }
    if (NULL != FspVarReqParams->DataSize) {
      DEBUG((DEBUG_VERBOSE, "\tDataSize 0x%X\n", *(FspVarReqParams->DataSize)));
    }
    DEBUG((DEBUG_VERBOSE, "\tData ptr 0x%X\n", FspVarReqParams->Data));

    switch (FspVarReqParams->VariableRequest) {
      case EnumFspVariableRequestGetVariable:
        Status = GetVariable(
                   FspVarReqParams->VariableName,
                   FspVarReqParams->VariableGuid,
                   FspVarReqParams->Attributes,
                   (UINTN*)FspVarReqParams->DataSize,
                   FspVarReqParams->Data
                   );
        break;
      case EnumFspVariableRequestGetNextVariableName:
        Status = GetNextVariableName(
                   (UINTN*)FspVarReqParams->VariableNameSize,
                   FspVarReqParams->VariableName,
                   FspVarReqParams->VariableGuid
                   );
        break;
      case EnumFspVariableRequestSetVariable:
        if (FspVarReqParams->Attributes == NULL
          || FspVarReqParams->DataSize == NULL
          || (*(FspVarReqParams->Attributes) & EFI_VARIABLE_NON_VOLATILE) == 0
          || (*(FspVarReqParams->Attributes) & (EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
            | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)) != 0)
        {
          Status = EFI_INVALID_PARAMETER;
        }
        else {
          Status = SetVariable(
                     FspVarReqParams->VariableName,
                     FspVarReqParams->VariableGuid,
                     *(FspVarReqParams->Attributes),
                     (UINTN)*(FspVarReqParams->DataSize),
                     FspVarReqParams->Data
                     );
        }
        break;
      case EnumFspVariableRequestQueryVariableInfo:
        if (FspVarReqParams->Attributes == NULL
          || (*(FspVarReqParams->Attributes) & EFI_VARIABLE_NON_VOLATILE) == 0
          || (*(FspVarReqParams->Attributes) & (EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
            | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)) != 0)
        {
          Status = EFI_INVALID_PARAMETER;
        }
        else {
          Status = QueryVariableInfo(
                     *(FspVarReqParams->Attributes),
                     FspVarReqParams->MaximumVariableStorageSize,
                     FspVarReqParams->RemainingVariableStorageSize,
                     FspVarReqParams->MaximumVariableSize
                     );
        }
        break;
      default:
        break;
    }
    FspVarReqParams->Status = Status;

    MultiPhaseInitParams.MultiPhaseAction = EnumMultiPhaseCompleteVariableRequest;
    Status = FspStatus = MultiPhaseFunction(&MultiPhaseInitParams);
    ASSERT_EFI_ERROR(FspStatus);
    if (EFI_ERROR(FspStatus)) {
      return FspStatus;
    }
  }
  return Status;
}
