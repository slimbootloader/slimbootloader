/** @file

  Copyright (c) 2021 - 2023 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/ConfigDataLib.h>
#include <ConfigDataDefs.h>
#include <Library/VariableLib.h>
#include <Library/TmeVarLib.h>
#include "TmeVarLibInternal.h"

/**
  Get TME Status

  @retval     TRUE if TME is enabled, FALSE otherwise
**/
BOOLEAN
EFIAPI
IsTmeEnabled (
  VOID
  )
{
  MEMORY_CFG_DATA            *MemCfgData;

  MemCfgData = (MEMORY_CFG_DATA *)FindConfigDataByTag (CDATA_MEMORY_TAG);

  if (MemCfgData != NULL) {
    return (BOOLEAN) MemCfgData->GeneralConfig.TmeEnable;
  }

  return (BOOLEAN) 0;
}

/**
  Get TME Keys From NVRAM

  @param[in, out]  TmeKeydata  Pointer to TME Key stored in TME variable

  @retval          EFI_SUCCESS if TME keys are successfully read from TME NVRAM variable
  @retval          EFI_UNSUPPORTED if TME is not enabled
  @retval          EFI_INVALID_PARAMETER if TmeKeyData pointer is NULL
**/
EFI_STATUS
EFIAPI
GetTmeVar (
  IN OUT TME_KEY  *TmeKeyData
  )
{
  EFI_STATUS              Status;
  UINTN                   TmeKeyVarSize;

  DEBUG ((DEBUG_INFO, "GetTmeVar Enter\n"));

  if (!IsTmeEnabled ()) {
    DEBUG ((DEBUG_INFO, "TME is Disabled. TME Variable will not be read.\n"));
    return EFI_UNSUPPORTED;
  }

  if (TmeKeyData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // read the TME keys
  //
  DEBUG ((DEBUG_INFO, "reading the keys from the TME NVRAM variable...\n"));
  Status = GetVariable (TME_KEY_VAR, NULL, NULL, &TmeKeyVarSize, TmeKeyData);

  DEBUG ((DEBUG_INFO, "GetTmeVar Exit\n"));

  return Status;
}

/**
  Get TME Keys from CPU and Store in NVRAM

  @retval     EFI_SUCCESS if TME keys are successfully stored in TME NVRAM variable
  @retval     EFI_UNSUPPORTED if TME is not enabled
  @retval     EFI_NOT_FOUND if TME Keys are not found in FSP HOB
**/
EFI_STATUS
EFIAPI
SetTmeVar (
  VOID
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  VOID                          *FspHobList;
  EFI_HOB_GUID_TYPE             *GuidHob;
  TME_STATUS_HOB_DATA           *pTmeStatusHobData;
  TME_KEY                       TmeKeyData;

  DEBUG ((DEBUG_INFO, "SetTmeVar Enter\n"));

  if (!IsTmeEnabled ()) {
    DEBUG ((DEBUG_INFO, "TME is Disabled. TME Variable will not be written.\n"));
    return EFI_UNSUPPORTED;
  }

  //
  // Get the TME Key HOB from CPU
  //
  GuidHob    = NULL;
  FspHobList = GetFspHobListPtr();
  if (FspHobList != NULL) {
    GuidHob = GetNextGuidHob (&gEfiFspTmeStatusDataHobGuid, FspHobList);
    if (GuidHob == NULL) {
      DEBUG ((DEBUG_INFO, "FSP TME HOB not found.\n"));
      return EFI_NOT_FOUND;
    }

    //
    // Copy TmeStatusData.
    //
    pTmeStatusHobData = (TME_STATUS_HOB_DATA *)GET_GUID_HOB_DATA (GuidHob);
    if (pTmeStatusHobData == NULL) {
      DEBUG ((DEBUG_INFO, "FSP TME HOB Empty.\n"));
      return EFI_NOT_FOUND;
    }

    if (pTmeStatusHobData->Lock == 1 &&
        pTmeStatusHobData->Enable == 1 &&
        pTmeStatusHobData->KeySave == 1) {
      // save the keys to the variable
      TmeKeyData.TmeKey[0].Key0 = pTmeStatusHobData->TME_Key0;
      TmeKeyData.TmeKey[0].Key1 = pTmeStatusHobData->TME_Key1;

      DEBUG ((DEBUG_INFO, "Writing the keys to the TME NVRAM variable...\n"));
      Status = SetVariable (TME_KEY_VAR, NULL, 0, sizeof(TME_KEY), &TmeKeyData);
    }
  } else {
    DEBUG ((DEBUG_INFO, "FSP HOB List not found.\n"));
    Status = EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_INFO, "SetTmeVar Exit\n"));

  return Status;
}
