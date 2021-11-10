/** @file
  This file provides payload common library interfaces.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/PayloadLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Guid/KeyHashGuid.h>
#include <Guid/FlashMapInfoGuid.h>
#include <Guid/LoaderPlatformInfoGuid.h>

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

  HobList = (VOID *)(UINTN)PcdGet32 (PcdPayloadHobList);
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

  GuidHob = GetNextGuidHob (&gFlashMapInfoGuid, (VOID *)(UINTN)PcdGet32 (PcdPayloadHobList));
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

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *)(UINTN)PcdGet32 (PcdGlobalDataAddress);

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

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *)(UINTN)PcdGet32 (PcdGlobalDataAddress);

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

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *)(UINTN)PcdGet32 (PcdGlobalDataAddress);

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

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *)(UINTN)PcdGet32 (PcdGlobalDataAddress);

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

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *)(UINTN)PcdGet32 (PcdGlobalDataAddress);

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

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *)(UINTN)PcdGet32 (PcdGlobalDataAddress);

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

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *)(UINTN)PcdGet32 (PcdGlobalDataAddress);

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

  GuidHob = GetNextGuidHob (&gLoaderPlatformInfoGuid, (VOID *)(UINTN)PcdGet32 (PcdPayloadHobList));
  if (GuidHob == NULL) {
    return 1;
  }

  LoaderPlatformInfo = (LOADER_PLATFORM_INFO *)GET_GUID_HOB_DATA (GuidHob);
  return LoaderPlatformInfo->BootPartition;
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

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *)(UINTN)PcdGet32 (PcdGlobalDataAddress);

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

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *)(UINTN)PcdGet32 (PcdGlobalDataAddress);

  return PayloadGlobalDataPtr->DeviceTable;
}

/**
  This function retrieves container list pointer.

  @retval    The container list pointer.

**/
VOID *
EFIAPI
GetContainerListPtr (
  VOID
  )
{
  PAYLOAD_GLOBAL_DATA     *PayloadGlobalDataPtr;

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *)(UINTN)PcdGet32 (PcdGlobalDataAddress);

  return PayloadGlobalDataPtr->ContainerList;
}

/**
  This function retrieves hash store pointer.

  @retval    The hash store pointer.

**/
VOID *
EFIAPI
GetHashStorePtr (
  VOID
  )
{
  PAYLOAD_GLOBAL_DATA     *PayloadGlobalDataPtr;

  PayloadGlobalDataPtr = (PAYLOAD_GLOBAL_DATA *)(UINTN)PcdGet32 (PcdGlobalDataAddress);

  return PayloadGlobalDataPtr->HashStorePtr;
}
