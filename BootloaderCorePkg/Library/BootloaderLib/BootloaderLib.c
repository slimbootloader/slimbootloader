/** @file

  Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Library/SecureBootLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <BootloaderCoreGlobal.h>

/**
  Returns the pointer to the Flash Map.

  This function will get the flash map pointer from global data

  @return The pointer to the Flash Map.

**/
VOID *
EFIAPI
GetFlashMapPtr (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->FlashMapPtr;
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
  LOADER_GLOBAL_DATA  *LdrGlobal;

  //
  // Only check this parameter in debug mode
  //
  LdrGlobal = GetLoaderGlobalDataPointer();
  return LdrGlobal->LdrHobList;
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
  return &GetLoaderGlobalDataPointer()->PerfData;
}

/**
  This function gets the configuration data pointer.

  @retval    The configuration data blob pointer.

**/
VOID *
EFIAPI
GetConfigDataPtr (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->CfgDataPtr;
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
  return GetLoaderGlobalDataPointer()->PlatformId;
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
  return GetLoaderGlobalDataPointer()->LogBufPtr;
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
  return GetLoaderGlobalDataPointer()->LibDataPtr;
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
  return GetLoaderGlobalDataPointer()->ServicePtr;
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
  return GetLoaderGlobalDataPointer()->PcdDataPtr;
}


/**
  This function retrieves current boot partition.

  @retval    The current boot partition.

**/
UINT8
EFIAPI
GetCurrentBootPartition (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->CurrentBootPartition;
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
  return GetLoaderGlobalDataPointer()->LoaderStage;
}

/**
  This function retrieves platform data pointer.

  @retval    The platform data pointer.

**/
VOID *
EFIAPI
GetPlatformDataPtr (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->PlatDataPtr;
}

/**
  This function retrieves container list pointer.

  @retval    The container list pointer.

**/
VOID *
EFIAPI
GetContainerListPtr  (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->ContainerList;
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
  return GetLoaderGlobalDataPointer()->HashStorePtr;
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
  return GetLoaderGlobalDataPointer()->LdrFeatures;
}


/**
  This function sets device table.

  @param DeviceTable   The pointer to device table.

**/
VOID
EFIAPI
SetDeviceTable (
  IN VOID         *DeviceTable
  )
{
  GetLoaderGlobalDataPointer()->DeviceTable = DeviceTable;
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
  return GetLoaderGlobalDataPointer()->DeviceTable;
}

