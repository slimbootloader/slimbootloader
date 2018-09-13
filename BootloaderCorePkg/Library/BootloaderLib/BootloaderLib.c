/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Library/BootloaderCoreLib.h>
#include <KeyStore.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>

/**
  Get the component hash data by the component type.

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
  LOADER_GLOBAL_DATA  *LdrGlobal;
  KEYSTORE_TABLE      *KeyStorePtr;
  UINT8               HashIndex;

  if (HashData == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  HashIndex = ComponentType;
  LdrGlobal = GetLoaderGlobalDataPointer ();
  KeyStorePtr = (KEYSTORE_TABLE *)LdrGlobal->KeyStorePtr;
  ASSERT (KeyStorePtr != NULL);

  *HashData = NULL;
  if (HashIndex >= HASH_INDEX_MAX_NUM) {
    return RETURN_UNSUPPORTED;
  }

  if (! (KeyStorePtr->Valid & (1 << HashIndex))) {
    return RETURN_NOT_FOUND;
  }

  *HashData = (UINT8 *)KeyStorePtr->Data[HashIndex].Data;

  return RETURN_SUCCESS;
}

/**
  Set the component hash data by the component type.

  @param[in]  ComponentType   Component type.
  @param[in] HashData        Hash data pointer corresponding Component type.

  @retval RETURN_SUCCESS             Set hash data succeeded.
  @retval RETURN_UNSUPPORTED         Hash component type is not supported.
  @retval RETURN_NOT_FOUND           Hash data is not found.
  @retval RETURN_INVALID_PARAMETER   HashData is NULL.

**/
RETURN_STATUS
SetComponentHash (
  IN       UINT8             ComponentType,
  IN CONST UINT8            *HashData
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;
  KEYSTORE_TABLE      *KeyStorePtr;
  UINT8               HashIndex;

  if (HashData == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  HashIndex = ComponentType;
  LdrGlobal = GetLoaderGlobalDataPointer ();
  KeyStorePtr = (KEYSTORE_TABLE *)LdrGlobal->KeyStorePtr;
  ASSERT (KeyStorePtr != NULL);

  if (HashIndex != HASH_INDEX_PAYLOAD_DYNAMIC) {
    return RETURN_UNSUPPORTED;
  }

  CopyMem ((UINT8 *)KeyStorePtr->Data[HashIndex].Data, HashData, KEYSTORE_DIGEST_LENGTH);

  return RETURN_SUCCESS;
}

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
  return GetLoaderGlobalDataPointer()->ConfDataPtr;
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

  if (FeaturePcdGet (PcdFlashMapEnabled) == FALSE) {
    //
    // If Flash Map is not enabled, get base and size from PCD's
    //
    Status = GetComponentPcdInfo (Signature, Base, Size);
    return Status;
  }

  if (GetCurrentBootPartition() == 1) {
    Status = GetComponentInfoByPartition (Signature, TRUE, Base, Size);
  } else {
    Status = GetComponentInfoByPartition (Signature, FALSE, Base, Size);
  }

  return Status;
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

