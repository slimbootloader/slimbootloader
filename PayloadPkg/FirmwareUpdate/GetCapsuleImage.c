/** @file
  This file contains the implementation of FirmwareUpdateLib library.

  Copyright (c) 2017 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiPei.h>
#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/FileSystemLib.h>
#include <Guid/OsBootOptionGuid.h>
#include <Library/HobLib.h>
#include <Library/MediaAccessLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/CryptoLib.h>
#include <Library/FirmwareUpdateLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/BootOptionLib.h>
#include <ConfigDataCommonStruct.h>

/**
  Initialize Boot Device (Media)

  This function will initialize boot device based on the info in
  capsule config data.

  @param[in]  CapsuleInfo     Pointer to capsule information config data

  @retval  RETURN_SUCCESS     If partition was found successfully
  @retval  Others             If partition was not found
**/
EFI_STATUS
InitBootDevice (
  IN  CAPSULE_INFO_CFG_DATA *CapsuleInfo
  )
{
  RETURN_STATUS   Status;
  UINTN           BootMediumPciBase;

  if (CapsuleInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get OS boot device address
  //
  BootMediumPciBase = GetDeviceAddr (CapsuleInfo->DevType, CapsuleInfo->DevInstance);
  BootMediumPciBase = TO_MM_PCI_ADDRESS (BootMediumPciBase);

  DEBUG ((DEBUG_INFO, "BootMediumPciBase(0x%x)\n", BootMediumPciBase));

  //
  // Init Boot device functions
  //
  Status = MediaSetInterfaceType (CapsuleInfo->DevType);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Invalid Boot device configured\n"));
    return RETURN_UNSUPPORTED;
  }

  Status = MediaInitialize (BootMediumPciBase, DevInitAll);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return RETURN_SUCCESS;
}

/**
  Get hardware partition handle from boot option info

  This function will get hardware partition handle based on boot option.

  @param[in]  CapsuleInfo     Pointer to capsule information config data
  @param[out] HwPartHandle    Hardware partition handle for boot image

  @retval  RETURN_SUCCESS     If partition was found successfully
  @retval  Others             If partition was not found
**/
EFI_STATUS
FindBootPartition (
  IN  CAPSULE_INFO_CFG_DATA *CapsuleInfo,
  OUT EFI_HANDLE            *HwPartHandle
  )
{
  RETURN_STATUS   Status;

  if (CapsuleInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "find boot partition\n"));
  Status = FindPartitions (CapsuleInfo->HwPart, HwPartHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "Find partion success\n"));
  return RETURN_SUCCESS;
}

/**
  Get Capsule image from raw partition

  Using capsule info, this function will read capsule image from raw
  partition based on hardware partition info saved in capsule info.

  @param[in]  CapsuleInfo       Pointer to capsule information config data
  @param[in]  HwPartHandle      Partition handler
  @param[out] CapsuleImage      Pointer to the buffer address for the
                                capsule image.
  @param[out] CapsuleImageSize  Pointer to the length of the capsule image.

  @retval  EFI_OUT_OF_RESOURCES   Out of memory resource
  @retval  EFI_NOT_FOUND          Cannot find the capsule image
  @retval  EFI_LOAD_ERROR         Image loading error
  @retval  EFI_SUCCESS            Image was loaded successfully

**/
EFI_STATUS
GetCapsuleFromRawPartition (
  IN  CAPSULE_INFO_CFG_DATA   *CapsuleInfo,
  IN  EFI_HANDLE              HwPartHandle,
  OUT VOID                    **CapsuleImage,
  OUT UINTN                   *CapsuleImageSize
  )
{
  EFI_STATUS                Status;
  DEVICE_BLOCK_INFO         BlockInfo;
  VOID                      *Buffer;
  UINTN                     ImageSize;
  LOGICAL_BLOCK_DEVICE      LogicBlkDev;
  UINTN                     AlginedHeaderSize;
  UINTN                     AlginedImageSize;
  UINT32                    BlockSize;
  UINT8                     BlockData[4096];
  FIRMWARE_UPDATE_HEADER    *FwUpdHeader;

  DEBUG ((DEBUG_INFO, "Load image from SwPart (0x%x), LbaAddr(0x%x)\n", 0, 0));
  Status = GetLogicalPartitionInfo (CapsuleInfo->SwPart, HwPartHandle, &LogicBlkDev);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Get logical partition error, Status = %r\n", Status));
    return Status;
  }

  Status = MediaGetMediaInfo (CapsuleInfo->HwPart, &BlockInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "GetMediaInfo Error %r\n", Status));
    return Status;
  }

  //
  // Read the capsule Header first to get total size of the capsule image.
  // Make sure to round the Header size to be block aligned in bytes.
  //
  BlockSize = BlockInfo.BlockSize;
  AlginedHeaderSize = ((sizeof (FIRMWARE_UPDATE_HEADER) % BlockSize) == 0) ? \
                      sizeof (FIRMWARE_UPDATE_HEADER) : \
                      ((sizeof (FIRMWARE_UPDATE_HEADER) / BlockSize) + 1) * BlockSize;
  Status = MediaReadBlocks (
             CapsuleInfo->HwPart,
             LogicBlkDev.StartBlock + CapsuleInfo->LbaAddr,
             AlginedHeaderSize,
             BlockData
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Read firmware update header error, Status = %r\n", Status));
    return Status;
  }

  //
  // Firmware update header
  //
  FwUpdHeader = (FIRMWARE_UPDATE_HEADER *) BlockData;

  //
  // Make sure this is the capsule image
  //
  if (!CompareGuid (&gFirmwareUpdateImageFileGuid, &FwUpdHeader->FileGuid)) {
    DEBUG ((DEBUG_INFO, "Invalid Capsule image found, Guid mismatch\n"));
    return EFI_NOT_FOUND;
  }

  if (FwUpdHeader->HeaderSize != sizeof (FIRMWARE_UPDATE_HEADER)) {
    DEBUG ((DEBUG_INFO, "Invalid Capsule image found, Header size mismatch\n"));
    return EFI_NOT_FOUND;
  }

  //
  // Make sure to round the image size to be block aligned in bytes.
  //
  ImageSize        = CAPSULE_IMAGE_SIZE ((FIRMWARE_UPDATE_HEADER *) BlockData);
  AlginedImageSize = ((ImageSize % BlockSize) == 0) ? \
                     ImageSize : \
                     ((ImageSize / BlockSize) + 1) * BlockSize;

  Buffer = (UINT8 *) AllocatePages (EFI_SIZE_TO_PAGES (AlginedImageSize));
  if (Buffer == NULL) {
    DEBUG ((DEBUG_INFO, "Allocate memory (size:0x%x) fail.\n", AlginedImageSize));
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Read the capsule image into the buffer
  //
  Status = MediaReadBlocks (
             CapsuleInfo->HwPart,
             LogicBlkDev.StartBlock + CapsuleInfo->LbaAddr,
             AlginedImageSize,
             Buffer
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Read capsule image error, Status = %r\n", Status));
    return  Status;
  }

  if ((Buffer == NULL) || (ImageSize == 0)) {
    return EFI_LOAD_ERROR;
  }

  *CapsuleImage = Buffer;
  *CapsuleImageSize = ImageSize;
  return EFI_SUCCESS;
}


/**
  Get capsule image for firmware update.

  This function read firmware update capsule image starting with firmware update
  header. It could be read from EMMC, UFS, USB, SATA, etc. block device. Often the
  Capsule image could be saved in the root directory of a FAT system.

  @param[in]  CapsuleInfo      Pointer to capsule information config data.
  @param[out] CapsuleImage     The firmware update capsule image.
  @param[out] CapsuleImageSize The capsule image size.

  @retval  EFI_SUCCESS         Get the capsule image successfully.
  @retval  others              Error happening when getting capsule image.
**/
EFI_STATUS
LoadCapsuleImage (
  IN  CAPSULE_INFO_CFG_DATA *CapsuleInfo,
  OUT VOID                  **CapsuleImage,
  OUT UINT32                *CapsuleImageSize
  )
{
  EFI_STATUS          Status;
  UINTN               HardwareDeviceBlockIndex;
  DEVICE_BLOCK_INFO   BlockInfo;
  EFI_HANDLE          FsHandle;
  EFI_HANDLE          HwPartHandle;
  CHAR16              FileName[MAX_FILE_LEN];
  EFI_HANDLE          FileHandle;


  if ((CapsuleImage == NULL) || (CapsuleImageSize == NULL)) {
    Status = EFI_INVALID_PARAMETER;
    return Status;
  }

  *CapsuleImageSize = 0;
  *CapsuleImage     = NULL;
  FileHandle        = NULL;
  HwPartHandle      = NULL;
  FsHandle          = NULL;

  Status = FindBootPartition (CapsuleInfo, &HwPartHandle);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // If we do not have file system, try reading capsule from raw partition
  //
  if (CapsuleInfo->FsType >= EnumFileSystemMax) {
    Status = GetCapsuleFromRawPartition (CapsuleInfo, HwPartHandle, CapsuleImage, (UINTN *)CapsuleImageSize);
    goto Done;
  }

  DEBUG ((DEBUG_ERROR, "Find partition\n"));
  HardwareDeviceBlockIndex = CapsuleInfo->HwPart;
  Status = MediaGetMediaInfo (HardwareDeviceBlockIndex, &BlockInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "GetInfo Error %r\n", Status));
    goto Done;
  }

  if (CapsuleInfo->FsType >= EnumFileSystemMax) {
    Status = InitFileSystem (CapsuleInfo->SwPart, EnumFileSystemTypeAuto, HwPartHandle, &FsHandle);
  } else {
    Status = InitFileSystem (CapsuleInfo->SwPart, CapsuleInfo->FsType, HwPartHandle, &FsHandle);
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "No partitions found, Status = %r\n", Status));
    goto Done;
  }

  //
  // Find capsule using the name provided in configuration data
  //
  if (CapsuleInfo->FileName[0] != 0) {
    AsciiStrToUnicodeStrS ((CONST CHAR8 *)&CapsuleInfo->FileName[0], FileName, MAX_FILE_LEN);

    Status = OpenFile (FsHandle, FileName, &FileHandle);
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, " Open Capsule File '%s' Status : %r\n", FileName, Status));
      goto Done;
    }

    Status = GetFileSize (FileHandle, (UINTN *)CapsuleImageSize);
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, " Get Capsule File '%s' size Status : %r\n", FileName, Status));
      goto Done;
    }

    *CapsuleImage = AllocatePool (*CapsuleImageSize);
    if (*CapsuleImage == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    Status = ReadFile (FileHandle, CapsuleImage, (UINTN *)CapsuleImageSize);
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, " Read Capsule File '%s' Status : %r\n", FileName, Status));
      goto Done;
    }
  } else {
    Status = EFI_NOT_FOUND;
  }

Done:
  if (FileHandle != NULL) {
    CloseFile (FileHandle);
  }

  if (EFI_ERROR (Status)) {
    if (*CapsuleImage != NULL) {
      FreePool (*CapsuleImage);
      *CapsuleImage = NULL;
    }
    if (FsHandle != NULL) {
      CloseFileSystem (FsHandle);
    }
    if (HwPartHandle != NULL) {
      FreePool (HwPartHandle);
    }
  }

  return Status;
}


/**
  Get OS boot option list.

  This function will get OS boot options from HOB, and return boot list
  pointer that point to HOB data.

  @retval Boot option list   Return OS boot option list.
  @retval NULL               Return NULL if there is error.
**/
OS_BOOT_OPTION_LIST *
GetOsImageList (
  VOID
  )
{
  EFI_HOB_GUID_TYPE   *GuidHob;

  GuidHob = GetNextGuidHob (&gOsBootOptionGuid, GetHobListPtr());
  if (GuidHob == NULL) {
    ASSERT (GuidHob);
    return NULL;
  }
  return (OS_BOOT_OPTION_LIST *)GET_GUID_HOB_DATA (GuidHob);
}


/**
  Get capsule image for firmware update.

  This function read firmware update capsule image starting with firmware update
  header. It could be read from EMMC, UFS, USB, SATA, etc. block device. Often the
  Capsule image could be saved in the root directory of a FAT system.

  @param[out] CapsuleImage      The firmware update capsule image.
  @param[out] CapsuleImageSize  The capsule image size.

  @retval  EFI_SUCCESS        Get the capsule image successfully.
  @retval  others             Error happening when getting capsule image.
**/
EFI_STATUS
EFIAPI
GetCapsuleImage (
  OUT VOID     **CapsuleImage,
  OUT UINT32   *CapsuleImageSize
  )
{
  EFI_STATUS              Status;
  CAPSULE_INFO_CFG_DATA   *CapsuleInfo;
  UINT8                   HwPart;
  UINT8                   StartPart;
  UINT8                   EndPart;

  Status = EFI_UNSUPPORTED;
  CapsuleInfo = NULL;

  if ((CapsuleImage == NULL ) || (CapsuleImageSize == NULL )) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "\n=================Read Capsule Image==============\n"));

  //
  // Get capsule configuration data
  //
  CapsuleInfo = (CAPSULE_INFO_CFG_DATA *) FindConfigDataByTag (CDATA_CAPSULE_INFO_TAG);
  //
  // If we do not find capsule information, return error
  //
  if (CapsuleInfo == NULL) {
    DEBUG((DEBUG_ERROR, " CapsuleInfo not found \n"));
    return EFI_NOT_FOUND;
  }

  Status = InitBootDevice (CapsuleInfo);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((CapsuleInfo->DevType == OsBootDeviceUsb) && (CapsuleInfo->HwPart == 0xFF)) {
    StartPart = 0;
    EndPart   = 0x10;
  } else {
    StartPart = CapsuleInfo->HwPart;
    EndPart   = CapsuleInfo->HwPart;
  }

  for (HwPart = StartPart; HwPart <= EndPart; HwPart++) {
    CapsuleInfo->HwPart = HwPart;

    DEBUG ((DEBUG_INFO, "Read capsule image from %a DevInstance (%4x) HwPart (%4x) SwPart (%4x) FS (%4a)",
      GetBootDeviceNameString(CapsuleInfo->DevType), CapsuleInfo->DevInstance, CapsuleInfo->HwPart,
      CapsuleInfo->SwPart, GetFsTypeString (CapsuleInfo->FsType)));
    if (CapsuleInfo->FsType < EnumFileSystemMax) {
      DEBUG ((DEBUG_INFO, " file name: %a\n", CapsuleInfo->FileName));
    } else {
      DEBUG ((DEBUG_INFO, " LBA offset: 0x%x \n", CapsuleInfo->LbaAddr));
    }

    Status = LoadCapsuleImage (CapsuleInfo, CapsuleImage, CapsuleImageSize);
    if (!EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "Capsule Image found, ImageSize=0x%x\n", *CapsuleImageSize));
      DEBUG ((DEBUG_INFO, "First 256Bytes of capsule image\n"));
      DumpHex (2, 0, 256, (VOID *)*CapsuleImage);
      break;
    }
  }

  return Status;
}

