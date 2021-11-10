/** @file
  Boot Media Library.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/BootMediaLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Guid/LoaderFspInfoGuid.h>

typedef struct {
  UINT32      BPDTOffset;
  UINT32      PhysicalData;
  UINT32      PhysicalDeviceArea;
  UINT32      LogicalData;
}CURRENT_BOOT_MEDIA;

/**
  Determines the boot media that the platform firmware is booting from

  @param[out] BootMediaType   The Boot Media the platform is booting from

  @retval     EFI_SUCCESS     The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetBootMediaType (
  OUT BOOT_MEDIA_TYPE            *BootMediaType
  )
{
  EFI_HOB_GUID_TYPE              *GuidHob;
  CURRENT_BOOT_MEDIA             *BootMediaData;
  VOID                           *HobList;
  LOADER_FSP_INFO                *FspInfo;

  HobList = GetHobList();
  ASSERT(HobList != NULL);

  FspInfo = GetNextGuidHob(&gLoaderFspInfoGuid, HobList);
  ASSERT(FspInfo != NULL);
  FspInfo = GET_GUID_HOB_DATA(FspInfo);

  GuidHob = GetNextGuidHob(&gEfiBootMediaHobGuid, FspInfo->FspHobList);
  ASSERT (GuidHob != NULL);
  BootMediaData = (CURRENT_BOOT_MEDIA *)GET_GUID_HOB_DATA (GuidHob);

  if (BootMediaData->PhysicalData >= BootMediaMax) {
    return EFI_NOT_FOUND;
  }

  *BootMediaType   = BootMediaData->PhysicalData;

  return EFI_SUCCESS;
}


/**
  Determines the boot partition that the platform firmware is booting from

  @param[out] BootPartition   The Boot partition the platform is booting from

  @retval     EFI_SUCCESS     The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetBootPartition (
  OUT BOOT_PARTITION_SELECT      *BootPartition
  )
{
  EFI_HOB_GUID_TYPE              *GuidHob;
  CURRENT_BOOT_MEDIA             *BootMediaData;
  VOID                           *HobList;
  LOADER_FSP_INFO                *FspInfo;

  HobList = GetHobList();
  ASSERT(HobList != NULL);

  FspInfo = GetNextGuidHob(&gLoaderFspInfoGuid, HobList);
  ASSERT(FspInfo != NULL);
  FspInfo = GET_GUID_HOB_DATA(FspInfo);

  GuidHob = GetNextGuidHob(&gEfiBootMediaHobGuid, FspInfo->FspHobList);
  ASSERT (GuidHob != NULL);
  BootMediaData = (CURRENT_BOOT_MEDIA *)GET_GUID_HOB_DATA (GuidHob);

  if (BootMediaData->LogicalData >= BootPartitionMax) {
    return EFI_NOT_FOUND;
  }

  *BootPartition = BootMediaData->LogicalData;

  return EFI_SUCCESS;
}


/**
  Determines if the platform firmware is booting from SPI or not

  @retval TRUE        Platform firmware is booting from SPI
  @retval FALSE       Platform firmware is booting from a non-SPI device (eMMC, UFS, etc.)
**/
BOOLEAN
EFIAPI
BootMediaIsSpi (
  VOID
  )
{
  EFI_STATUS                     Status;
  BOOT_MEDIA_TYPE                BootMedia;

  Status = GetBootMediaType (&BootMedia);
  if (EFI_ERROR (Status) || BootMedia != BootMediaSpi) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  Determines if the platform firmware is booting from eMMC or not

  @retval TRUE        Platform firmware is booting from eMMC
  @retval FALSE       Platform firmware is booting from a non-eMMC device (SPI, UFS, etc.)
**/
BOOLEAN
EFIAPI
BootMediaIsEmmc (
  VOID
  )
{
  EFI_STATUS                     Status;
  BOOT_MEDIA_TYPE                BootMedia;

  Status = GetBootMediaType (&BootMedia);
  if (EFI_ERROR (Status) || BootMedia != BootMediaEmmc) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  Determines if the platform firmware is booting from UFS or not

  @retval TRUE        Platform firmware is booting from UFS
  @retval FALSE       Platform firmware is booting from a non-UFS device (eMMC, SPI, etc.)
**/
BOOLEAN
EFIAPI
BootMediaIsUfs (
  VOID
  )
{
  EFI_STATUS                     Status;
  BOOT_MEDIA_TYPE                BootMedia;

  Status = GetBootMediaType (&BootMedia);
  if (EFI_ERROR (Status) || BootMedia != BootMediaUfs) {
    return FALSE;
  } else {
    return TRUE;
  }
}
