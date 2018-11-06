/** @file
  This file provides payload common library interfaces.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/PayloadLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Guid/PayloadKeyHashGuid.h>
#include <Guid/FlashMapInfoGuid.h>
#include <Guid/LoaderPlatformInfoGuid.h>

/**
  Get the component hash data by the component type

  @param[in]  ComponentType   Component type.
  @param[out] HashData        Hash data pointer corresponding Component type.

  @retval RETURN_SUCCESS             Get hash data succeeded.
  @retval RETURN_UNSUPPORTED         Hash component type is not supported.
  @retval RETURN_NOT_FOUND           Hash data is not found.
  @retval RETURN_INVALID_PARAMETER   HashData is NULL.

**/
RETURN_STATUS
GetComponentHash (
  IN        UINT8            ComponentType,
  OUT CONST UINT8            **HashData
  )
{
  VOID                       *GuidHob;
  PAYLOAD_KEY_HASH           *PayloadKeyHash;
  UINT32                     Index;

  if (HashData == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  //
  // Get public key hash from HOB
  //
  GuidHob = GetNextGuidHob (&gPayloadKeyHashGuid, GetHobList());
  ASSERT (GuidHob != NULL);

  *HashData = NULL;
  PayloadKeyHash = (PAYLOAD_KEY_HASH *)GET_GUID_HOB_DATA (GuidHob);
  for (Index = 0; Index < PayloadKeyHash->DigestCount; Index++) {
    if (PayloadKeyHash->KeyHash[Index].ComponentType == ComponentType) {
      break;
    }
  }
  if (PayloadKeyHash->DigestCount == Index) {
    DEBUG ((DEBUG_ERROR, "NOT found hash data for component type %d!\n", ComponentType));
    return EFI_NOT_FOUND;
  }

  *HashData = &PayloadKeyHash->KeyHash[Index].Digest[0];

  return RETURN_SUCCESS;
}

/**
  Set the component hash data by the component type.

  @param[in]  ComponentType   Component type.
  @param[in] HashData        Hash data pointer corresponding component type.

  @retval RETURN_UNSUPPORTED  Hash component type is not supported for PayloadLib.

**/
RETURN_STATUS
SetComponentHash (
  IN       UINT8             ComponentType,
  IN CONST UINT8            *HashData
  )
{
  return RETURN_UNSUPPORTED;
}

/**
  Returns the pointer to the HOB list.

  If the pointer to the HOB list is NULL, then ASSERT().

  @return The pointer to the HOB list.

**/
VOID *
EFIAPI
GetHobListPtr (
  VOID
  )
{
  VOID                  *HobList;

  HobList = (VOID *)PcdGet32 (PcdPayloadHobList);
  ASSERT (HobList != NULL);

  return HobList;
}

/**
  Returns the pointer to the Flash Map.

  @return The pointer to the Flash Map.
**/
VOID *
EFIAPI
GetFlashMapPtr (
  VOID
  )
{
  FLASH_MAP             *FlashMap;
  EFI_HOB_GUID_TYPE     *GuidHob;

  GuidHob = GetNextGuidHob (&gFlashMapInfoGuid, (VOID *)PcdGet32 (PcdPayloadHobList));
  if (GuidHob == NULL) {
    return NULL;
  }

  FlashMap = (FLASH_MAP *)GET_GUID_HOB_DATA (GuidHob);
  if (FlashMap == NULL) {
    return NULL;
  }

  return FlashMap;
}

/**
  This function retrieves performance data pointer.

  @retval    The performance data pointer.

**/
BL_PERF_DATA *
EFIAPI
GetPerfDataPtr (
  VOID
  )
{
  PAYLOAD_GLOBAL_DATA     *PayloadGlobalDataPtr;

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *) PcdGet32 (PcdGlobalDataAddress);

  return &PayloadGlobalDataPtr->PerfData;
}

/**
  This function gets the configuration data pointer.

**/
VOID *
EFIAPI
GetConfigDataPtr (
  VOID
  )
{
  PAYLOAD_GLOBAL_DATA     *PayloadGlobalDataPtr;

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *) PcdGet32 (PcdGlobalDataAddress);

  return PayloadGlobalDataPtr->CfgDataPtr;
}

/**
  This function retrieves current platform id.

  @retval    The current platform id.

**/
UINT16
EFIAPI
GetPlatformId (
  VOID
  )
{
  LOADER_PLATFORM_INFO      *LoaderPlatformInfo;

  LoaderPlatformInfo = (LOADER_PLATFORM_INFO *)GetLoaderPlatformInfoPtr();
  ASSERT (LoaderPlatformInfo != NULL);

  return LoaderPlatformInfo->PlatformId;
}

/**
  This function retrieves log buffer pointer.

  @retval    The log buffer pointer.

**/
VOID *
EFIAPI
GetDebugLogBufferPtr (
  VOID
  )
{
  PAYLOAD_GLOBAL_DATA     *PayloadGlobalDataPtr;

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *) PcdGet32 (PcdGlobalDataAddress);

  return PayloadGlobalDataPtr->LogBufPtr;
}

/**
  This function retrieves global library data pointer.

  @retval    The library data pointer.

**/
VOID *
EFIAPI
GetLibraryDataPtr (
  VOID
  )
{
  PAYLOAD_GLOBAL_DATA     *PayloadGlobalDataPtr;

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *) PcdGet32 (PcdGlobalDataAddress);

  return PayloadGlobalDataPtr->LibDataPtr;
}

/**
  This function retrieves global library data pointer.

  @retval    The library data pointer.

**/
VOID *
EFIAPI
GetServiceListPtr (
  VOID
  )
{
  PAYLOAD_GLOBAL_DATA     *PayloadGlobalDataPtr;

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *) PcdGet32 (PcdGlobalDataAddress);

  return PayloadGlobalDataPtr->ServiceList;
}

/**
  This function retrieves features configuration.

  @retval    The feature configuration.

**/
UINT32
EFIAPI
GetFeatureCfg (
  VOID
  )
{
  PAYLOAD_GLOBAL_DATA     *PayloadGlobalDataPtr;

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *) PcdGet32 (PcdGlobalDataAddress);

  return PayloadGlobalDataPtr->LdrFeatures;
}

/**
  This function retrieves global PCD data pointer.

  @retval    The PCD data pointer.

**/
VOID *
EFIAPI
GetPcdDataPtr (
  VOID
  )
{
  PAYLOAD_GLOBAL_DATA     *PayloadGlobalDataPtr;

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *) PcdGet32 (PcdGlobalDataAddress);

  return PayloadGlobalDataPtr->PcdDataPtr;
}

/**
  Get cpu number of cores.

  @retval   cpu number of cores
**/
UINT8
EFIAPI
GetCurrentBootPartition (
  VOID
  )
{
  EFI_HOB_GUID_TYPE     *GuidHob;
  LOADER_PLATFORM_INFO  *LoaderPlatformInfo;

  GuidHob = GetNextGuidHob (&gLoaderPlatformInfoGuid, (VOID *)PcdGet32 (PcdPayloadHobList));
  if (GuidHob == NULL) {
    return 1;
  }

  LoaderPlatformInfo = (LOADER_PLATFORM_INFO *)GET_GUID_HOB_DATA (GuidHob);
  return LoaderPlatformInfo->BootPartition;
}

/**
  Gets component information from the flash map.

  This function will look for the component based on the input signature
  in the flash map, if found, will return the base address and size of the component.

  @param[in]  Signature     Signature of the component information required
  @param[out] Base          Base address of the component
  @param[out] Size          Size of the component

  @retval    EFI_SUCCESS    Found the component with the matching signature.
  @retval    EFI_NOT_FOUND  Component with the matching signature not found.

**/
EFI_STATUS
EFIAPI
GetComponentInfo (
  IN  UINT32     Signature,
  OUT UINT32     *Base,
  OUT UINT32     *Size
  )
{
  EFI_STATUS            Status;

  if (GetCurrentBootPartition() == 1) {
    Status = GetComponentInfoByPartition (Signature, TRUE, Base, Size);
  } else {
    Status = GetComponentInfoByPartition (Signature, FALSE, Base, Size);
  }

  return Status;
}


/**
  Returns the current stage of Bootloader execution.

  @retval LOADER_STAGE  Current stage of bootloader execution.
**/
LOADER_STAGE
EFIAPI
GetLoaderStage (
  VOID
  )
{
  return LOADER_STAGE_PAYLOAD;
}


/**
  This function sets device table.

  @param DeviceTable   The pointer to device table.

**/
VOID
EFIAPI
SetDeviceTable (
  IN VOID                 *DeviceTable
  )
{
  PAYLOAD_GLOBAL_DATA     *PayloadGlobalDataPtr;

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *) PcdGet32 (PcdGlobalDataAddress);

  PayloadGlobalDataPtr->DeviceTable = DeviceTable;
}

/**
  This function retrieves the platform device table.

  @retval    The platform device table.

**/
VOID *
EFIAPI
GetDeviceTable (
  VOID
  )
{
  PAYLOAD_GLOBAL_DATA     *PayloadGlobalDataPtr;

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *) PcdGet32 (PcdGlobalDataAddress);

  return PayloadGlobalDataPtr->DeviceTable;
}

