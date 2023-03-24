/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Uefi.h>
#include <FspApiLibInternal.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/FspApiLib.h>
#include <Library/FspVariableServicesLib.h>

/**
  This function handles variable services requests from FspMemoryInit or FspMultPhaseMemInit.
  This should be called immediately after returning from the respective FSP entrypoint.

  @param[in] Status             The status return by the FSP entry point.

  @retval Status
 */
EFI_STATUS
EFIAPI
FspVariableHandler(
  EFI_STATUS                FspStatus,
  FSP_MULTI_PHASE_FUNCTION  MultiPhaseFunction
  )
{
  EFI_STATUS                                    Status;
  FSP_MULTI_PHASE_PARAMS                        MultiPhaseInitParams;
  FSP_MULTI_PHASE_VARIABLE_REQUEST_INFO_PARAMS  *FspVarReqParams;

  Status = EFI_SUCCESS;

  while (FspStatus == FSP_STATUS_VARIABLE_REQUEST)
  {
    DEBUG((DEBUG_VERBOSE, "FspVariableHandler: Got FSP_STATUS_VARIABLE_REQUEST\n"));
    // Request details of variable request from Multi-phase API.
    MultiPhaseInitParams.MultiPhaseAction = EnumMultiPhaseGetVariableRequestInfo;
    MultiPhaseInitParams.PhaseIndex = 0;
    Status = MultiPhaseFunction(&MultiPhaseInitParams);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) break;

    FspVarReqParams = (FSP_MULTI_PHASE_VARIABLE_REQUEST_INFO_PARAMS*)MultiPhaseInitParams.MultiPhaseParamPtr;
    ASSERT (FspVarReqParams != NULL);
    if (FspVarReqParams == NULL)
    {
      Status = EFI_UNSUPPORTED;
      break;
    }

    DEBUG((DEBUG_VERBOSE, "FSP Variable Request %d\n", FspVarReqParams->VariableRequest));
    if (NULL != FspVarReqParams->VariableName)
      DEBUG((DEBUG_VERBOSE, "\tVariableName %s\n", FspVarReqParams->VariableName));
    if (NULL != FspVarReqParams->VariableGuid)
      DEBUG((DEBUG_VERBOSE, "\tVariableGuid %g\n", FspVarReqParams->VariableGuid));
    if (NULL != FspVarReqParams->DataSize)
      DEBUG((DEBUG_VERBOSE, "\tDataSize 0x%X\n", *(FspVarReqParams->DataSize)));
    DEBUG((DEBUG_VERBOSE, "\tData ptr 0x%X\n", FspVarReqParams->Data));

    switch (FspVarReqParams->VariableRequest)
    {
      case EnumFspVariableRequestGetVariable:
        Status = FspGetVariable(
          FspVarReqParams->VariableName,
          FspVarReqParams->VariableGuid,
          FspVarReqParams->Attributes,
          FspVarReqParams->DataSize,
          FspVarReqParams->Data
        );
        break;
      case EnumFspVariableRequestGetNextVariableName:
        Status = FspGetNextVariableName(
          FspVarReqParams->VariableNameSize,
          FspVarReqParams->VariableName,
          FspVarReqParams->VariableGuid
        );
        break;
      case EnumFspVariableRequestSetVariable:
        Status = FspSetVariable(
          FspVarReqParams->VariableName,
          FspVarReqParams->VariableGuid,
          FspVarReqParams->Attributes,
          FspVarReqParams->DataSize,
          FspVarReqParams->Data
        );
        break;
      case EnumFspVariableRequestQueryVariableInfo:
        Status = FspQueryVariableInfo(
          FspVarReqParams->Attributes,
          FspVarReqParams->MaximumVariableStorageSize,
          FspVarReqParams->RemainingVariableStorageSize,
          FspVarReqParams->MaximumVariableSize
        );
        break;
      default:
        break;
    }
    FspVarReqParams->Status = Status;

    MultiPhaseInitParams.MultiPhaseAction = EnumMultiPhaseCompleteVariableRequest;
    Status = FspStatus = MultiPhaseFunction(&MultiPhaseInitParams);
    ASSERT_EFI_ERROR(FspStatus);
    if (EFI_ERROR(FspStatus)) return FspStatus;
  }
  return Status;
}
