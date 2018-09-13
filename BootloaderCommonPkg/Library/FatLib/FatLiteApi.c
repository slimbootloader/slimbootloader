/** @file
  FAT recovery PEIM entry point, Ppi Functions and FAT Api functions.

Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials are licensed and made available
under the terms and conditions of the BSD License which accompanies this
distribution. The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "FatLitePeim.h"
#include <Library/PartitionLib.h>
#include <Library/FatLib.h>

/**
Initialize FAT file system volumes.

  @param[in]     SwPart           Software partition index.
  @param[in]     PartHandle       Partition handle.
  @param[out]    FsHandle         FAT file system handle.

  @retval EFI_SUCCESS             The file system was initialized successfully.
  @retval EFI_INVALID_PARAMETER   Parameter is not valid.
  @retval EFI_NOT_FOUND           FAT file system was not detected on this partition.
  @retval EFI_OUT_OF_RESOURCES    Insufficant memory resource pool.

**/
EFI_STATUS
EFIAPI
FatInitFileSystem (
  IN  UINT32                                       SwPart,
  IN  EFI_HANDLE                                   PartHandle,
  OUT EFI_HANDLE                                  *FsHandle
  )
{
  EFI_STATUS                Status;
  PEI_FAT_PRIVATE_DATA     *PrivateData;
  PEI_FAT_BLOCK_DEVICE     *FatBlockDevice;
  PART_BLOCK_DEVICE        *PartBlockDev;
  UINT32                    BlockSize;

  // Valid parameters
  PartBlockDev = (PART_BLOCK_DEVICE *)PartHandle;
  if ( (FsHandle == NULL) || (PartBlockDev == NULL) || \
       (PartBlockDev->Signature != PART_INFO_SIGNATURE) ) {
    return EFI_INVALID_PARAMETER;
  }

  if (SwPart >= PartBlockDev->BlockDeviceCount) {
    return EFI_INVALID_PARAMETER;
  }

  PrivateData = (PEI_FAT_PRIVATE_DATA *) AllocateZeroPool (sizeof (PEI_FAT_PRIVATE_DATA));
  if (PrivateData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PrivateData->Signature = PEI_FAT_PRIVATE_DATA_SIGNATURE;

  // Add first hardware partition info
  FatBlockDevice = &PrivateData->BlockDevice[0];
  FatBlockDevice->FoundDevNo    = TRUE;
  FatBlockDevice->PhysicalDevNo = (UINT8)PartBlockDev->HarewareDevice;
  FatBlockDevice->BlockSize     = PartBlockDev->BlockInfo.BlockSize;
  FatBlockDevice->LastBlock     = PartBlockDev->BlockInfo.BlockNum - 1;

  PrivateData->BlockDeviceCount = 1;

  // Create child block devices for
  if ( (PartBlockDev->PartitionType != EnumPartTypeUnknown) && \
       (PrivateData->BlockDeviceCount < PEI_FAT_MAX_BLOCK_DEVICE) ) {
    BlockSize = PartBlockDev->BlockInfo.BlockSize;
    FatBlockDevice = &PrivateData->BlockDevice[PrivateData->BlockDeviceCount];
    FatBlockDevice->BlockSize         = BlockSize;
    FatBlockDevice->LastBlock         = PartBlockDev->BlockDevice[SwPart].LastBlock;
    FatBlockDevice->Logical           = TRUE;
    FatBlockDevice->PartitionChecked  = TRUE;
    FatBlockDevice->StartingPos       = MultU64x32 (
                                          PartBlockDev->BlockDevice[SwPart].StartBlock,
                                          BlockSize
                                          );
    FatBlockDevice->PhysicalDevNo     = (UINT8)PartBlockDev->HarewareDevice;
    PrivateData->BlockDeviceCount++;
  }

  Status = FatGetVolumeData (PrivateData);
  if (EFI_ERROR (Status)) {
    FreePool (PrivateData);
  } else {
    DEBUG ((DEBUG_INFO, "Detected FAT on HwDev %d Part %d\n",  PartBlockDev->HarewareDevice, SwPart));
    *FsHandle = (EFI_HANDLE)PrivateData;
  }

  return Status;
}


/**
Loads file into memory by its name.

  @param[in]     FsHandle         FAT file system handle.
  @param[in]     FileName         The file name to get.
  @param[out]    FileBufferPtr    Allocated file buffer pointer.
  @param[out]    FileSize         Pointer to file buffer size.

  @retval EFI_SUCCESS             The file was loaded correctly.
  @retval EFI_INVALID_PARAMETER   Parameter is not valid.
  @retval EFI_DEVICE_ERROR        A device error occurred.
  @retval EFI_NOT_FOUND           A requested file cannot be found.
  @retval EFI_OUT_OF_RESOURCES    Insufficant memory resource pool.
  @retval EFI_BUFFER_TOO_SMALL    Buffer size is too small.

**/
EFI_STATUS
EFIAPI
FatGetFileByName (
  IN EFI_HANDLE                                    FsHandle,
  IN CHAR16                                       *FileName,
  OUT VOID                                       **FileBufferPtr,
  OUT UINTN                                       *FileSize
  )
{
  EFI_STATUS            Status;
  PEI_FILE_HANDLE       Handle;
  UINTN                 Size;
  PEI_FAT_FILE         *File;
  VOID                 *FileBuffer;
  PEI_FAT_PRIVATE_DATA *PrivateData;

  PrivateData = (PEI_FAT_PRIVATE_DATA *)FsHandle;
  if ((FileName == NULL) || (FileBufferPtr == NULL) || (PrivateData == NULL) || \
      (PrivateData->Signature != PEI_FAT_PRIVATE_DATA_SIGNATURE)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Search each volume in the root directory for the Recovery capsule
  //
  if (PrivateData->VolumeCount == 0) {
    return EFI_NOT_FOUND;
  }

  FileBuffer = *FileBufferPtr;
  Status = FindFile (PrivateData, 0, FileName, &Handle);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  File = (PEI_FAT_FILE *)Handle;
  Size = ALIGN_VALUE (File->FileSize, PrivateData->BlockDevice[File->Volume->BlockDeviceNo].BlockSize);
  if (FileBuffer == NULL) {
    FileBuffer = AllocatePages (EFI_SIZE_TO_PAGES (Size));
    if (FileBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    *FileBufferPtr = FileBuffer;
  } else {
    if (FileSize == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    if (*FileSize < Size) {
      *FileSize = Size;
      return EFI_BUFFER_TOO_SMALL;
    }
  }

  Status = FatReadFile (
             PrivateData,
             Handle,
             File->FileSize,
             FileBuffer
             );

  if (!EFI_ERROR (Status)) {
    if (FileSize != NULL) {
      *FileSize = (UINTN)File->FileSize;
    }
  } else {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}


/**
  Finds the recovery file on a FAT volume.
  This function finds the the recovery file named FileName on a specified FAT volume and returns
  its FileHandle pointer.

  @param  PrivateData             Global memory map for accessing global
                                  variables.
  @param  VolumeIndex             The index of the volume.
  @param  FileName                The recovery file name to find.
  @param  Handle                  The output file handle.

  @retval EFI_DEVICE_ERROR        Some error occured when operating the FAT
                                  volume.
  @retval EFI_NOT_FOUND           The recovery file was not found.
  @retval EFI_SUCCESS             The recovery file was successfully found on the
                                  FAT volume.

**/
EFI_STATUS
FindFile (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  UINTN                 VolumeIndex,
  IN  CHAR16                *FileName,
  OUT PEI_FILE_HANDLE       *Handle
  )
{
  EFI_STATUS    Status;
  PEI_FAT_FILE  Parent;
  PEI_FAT_FILE  *File;

  File = &PrivateData->File;

  //
  // VolumeIndex must be less than PEI_FAT_MAX_VOLUME because PrivateData->VolumeCount
  // cannot be larger than PEI_FAT_MAX_VOLUME when detecting recovery volume.
  //
  ASSERT (VolumeIndex < PEI_FAT_MAX_VOLUME);

  //
  // Construct root directory file
  //
  ZeroMem (&Parent, sizeof (PEI_FAT_FILE));
  Parent.IsFixedRootDir   = (BOOLEAN) ((PrivateData->Volume[VolumeIndex].FatType == Fat32) ? FALSE : TRUE);
  Parent.Attributes       = FAT_ATTR_DIRECTORY;
  Parent.CurrentPos       = 0;
  Parent.CurrentCluster   = Parent.IsFixedRootDir ? 0 : PrivateData->Volume[VolumeIndex].RootDirCluster;
  Parent.StartingCluster  = Parent.CurrentCluster;
  Parent.Volume           = &PrivateData->Volume[VolumeIndex];

  Status                  = FatSetFilePos (PrivateData, &Parent, 0);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Search for recovery capsule in root directory
  //
  Status = FatReadNextDirectoryEntry (PrivateData, &Parent, File);
  while (Status == EFI_SUCCESS) {
    //
    // Compare whether the file name is recovery file name.
    //
    if (EngStriColl (PrivateData, FileName, File->FileName)) {
      break;
    }

    Status = FatReadNextDirectoryEntry (PrivateData, &Parent, File);
  }

  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  //
  // Get the recovery file, set its file position to 0.
  //
  if (File->StartingCluster != 0) {
    Status = FatSetFilePos (PrivateData, File, 0);
  }

  *Handle = File;

  return EFI_SUCCESS;

}
