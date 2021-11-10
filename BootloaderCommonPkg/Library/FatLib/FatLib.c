/** @file
  FatLib APIs

Copyright (c) 2006 - 2020, Intel Corporation. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/FatLib.h>
#include <Library/ConsoleOutLib.h>
#include "FatLitePeim.h"
#include "FatLiteApi.h"
#include "FatLiteFmt.h"

/**
  Print directory or file name in debug output

  @param[in]     File             The File pointer to be printed

  @retval        none

*/
STATIC
VOID
PrintFileName (
  IN  PEI_FAT_FILE        *File
  )
{
  CHAR16  *FileName;

  if (File == NULL) {
    return;
  }

  FileName = File->LongFileName;
  if (FileName[0] == '\0') {
    FileName = File->FileName;
  }

  if (File->Attributes & FAT_ATTR_DIRECTORY) {
    CONSOLE_PRINT_UNICODE ((L"  %s/\n", FileName));
  } else {
    CONSOLE_PRINT_UNICODE ((L"  %-16s %u\n", FileName, File->FileSize));
  }
}

/**
  Gets next path node from a full file path string.

  @param[in]     FileName         The full file path string.
  @param[out]    Length           The current path node length.

  @retval        The next path node pointer in the file path string.
                 NULL indicates no more path node found.

**/
STATIC
CHAR16 *
GetNextFilePathNode (
  IN  CHAR16 *FileName,
  OUT UINT32 *Length
  )
{
  CHAR16 *Ptr;
  UINT32  Len;

  Len = 0;
  Ptr = FileName;
  if (Ptr != NULL) {
    while (TRUE) {
      if ((*Ptr == 0) || (*Ptr == '\\') || (*Ptr == '/')) {
        Len = (UINT32)(Ptr - FileName);
        break;
      }
      Ptr++;
    }
  }

  if (Length != NULL) {
    *Length = (UINT32)Len;
  }

  if ((Ptr != NULL) && (*Ptr != 0)) {
    return Ptr + 1;
  } else {
    return NULL;
  }
}

/**
  Finds a file on a FAT volume.
  This function finds the the file named FileName on a specified FAT volume and
  returns its FileHandle pointer.

  @param  PrivateData             Global memory map for accessing global
                                  variables.
  @param  VolumeIndex             The index of the volume.
  @param  FileName                The file name to find.
  @param  Handle                  The output file handle.

  @retval EFI_DEVICE_ERROR        Some error occured when operating the FAT
                                  volume.
  @retval EFI_NOT_FOUND           The file was not found.
  @retval EFI_SUCCESS             The file was successfully found on the
                                  FAT volume.

**/
STATIC
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
  CHAR16        *NodeCurr;
  CHAR16        *NodeNext;
  UINT32         NodeLen;

  File = &PrivateData->File;

  //
  // VolumeIndex must be less than PEI_FAT_MAX_VOLUME because PrivateData->VolumeCount
  // cannot be larger than PEI_FAT_MAX_VOLUME when detecting a volume.
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

  CopyMem (File, &Parent, sizeof (PEI_FAT_FILE));

  //
  // Search for a file or directory
  //
  NodeCurr = FileName;
  while (NodeCurr != NULL) {
    NodeNext = GetNextFilePathNode (NodeCurr, &NodeLen);
    if (NodeLen > 0) {
      do {
        Status   = FatReadNextDirectoryEntry (
                       PrivateData,
                       &Parent,
                       (NodeNext == NULL) ? 0 : FAT_ATTR_DIRECTORY,
                       File
                   );
        if (Status == EFI_SUCCESS) {
          //
          // Compare whether the file name is matched.
          //
          if (EngStrniColl (NodeCurr, File->FileName, NodeLen)) {
            if (NodeLen == ARRAY_SIZE (File->FileName)) {
              break;
            }
            if ((NodeLen < ARRAY_SIZE (File->FileName)) && (File->FileName[NodeLen] == 0)) {
              break;
            }
          }

          if (EngStrniColl (NodeCurr, File->LongFileName, NodeLen)) {
            if (NodeLen == ARRAY_SIZE (File->LongFileName)) {
              break;
            }
            if ((NodeLen < ARRAY_SIZE (File->LongFileName)) && (File->LongFileName[NodeLen] == 0)) {
              break;
            }
          }
        }
      } while (Status == EFI_SUCCESS);
      if (EFI_ERROR (Status)) {
        return EFI_NOT_FOUND;
      } else {
        CopyMem (&Parent, File, sizeof (PEI_FAT_FILE));
      }
    }
    NodeCurr = NodeNext;
  }

  //
  // Get the file, set its file position to 0.
  //
  if (File->StartingCluster != 0) {
    Status = FatSetFilePos (PrivateData, File, 0);
  }

  *Handle = File;

  return EFI_SUCCESS;

}

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

  PrivateData->Signature = FS_FAT_SIGNATURE;

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
  Clean-up allocated memory/etc. for FAT file system

  @param[in]     FsHandle         FAT file system handle.

  @retval                         none

**/
VOID
EFIAPI
FatCloseFileSystem (
  IN  EFI_HANDLE                                   FsHandle
  )
{
  PEI_FAT_PRIVATE_DATA     *PrivateData;

  PrivateData = NULL;
  if (FsHandle != NULL) {
    PrivateData = (PEI_FAT_PRIVATE_DATA *)FsHandle;
    ASSERT (PrivateData->Signature == FS_FAT_SIGNATURE);
  }

  if (PrivateData != NULL && PrivateData->Signature == FS_FAT_SIGNATURE) {
    FreePool (PrivateData);
  }
}

/**
  Open a file by its name and return its file handle.

  @param[in]     FsHandle         file system handle.
  @param[in]     FileName         The file name to get.
  @param[out]    FileHandle       file handle

  @retval EFI_SUCCESS             The file opened correctly.
  @retval EFI_INVALID_PARAMETER   Parameter is not valid.
  @retval EFI_DEVICE_ERROR        A device error occurred.
  @retval EFI_NOT_FOUND           A requested file cannot be found.
  @retval EFI_OUT_OF_RESOURCES    Insufficant memory resource pool.

**/
EFI_STATUS
EFIAPI
FatFsOpenFile (
  IN  EFI_HANDLE                                    FsHandle,
  IN  CHAR16                                       *FileName,
  OUT EFI_HANDLE                                   *FileHandle
  )
{
  EFI_STATUS              Status;
  PEI_FILE_HANDLE         Handle;
  PEI_FAT_FILE           *File;
  PEI_FAT_PRIVATE_DATA   *PrivateData;

  PrivateData = (PEI_FAT_PRIVATE_DATA *)FsHandle;
  if (PrivateData == NULL || FileName == NULL || PrivateData->Signature != FS_FAT_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Search each volume in the root directory
  //
  if (PrivateData->VolumeCount == 0) {
    return EFI_NOT_FOUND;
  }

  Status = FindFile (PrivateData, 0, FileName, &Handle);
  if (!EFI_ERROR (Status)) {
    File = (PEI_FAT_FILE *) AllocatePool (sizeof (PEI_FAT_FILE));
    ASSERT (File != NULL);
    if (File == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (File, Handle, sizeof (PEI_FAT_FILE));

    *FileHandle = (EFI_HANDLE)File;

    DEBUG ((DEBUG_VERBOSE, "  FatFsOpenFile: %s opened\n", File->FileName));
  }

  return Status;
}

/**
  Get file size by opened file handle.

  @param[in]     FileHandle       file handle
  @param[out]    FileSize         Pointer to file buffer size.

  @retval EFI_SUCCESS             The file was loaded correctly.
  @retval EFI_INVALID_PARAMETER   Parameter is not valid.

**/
EFI_STATUS
EFIAPI
FatFsGetFileSize (
  IN  EFI_HANDLE                                  FileHandle,
  OUT UINTN                                      *FileSize
  )
{
  PEI_FAT_FILE           *File;

  File = (PEI_FAT_FILE *)FileHandle;
  ASSERT (File != NULL);
  if (File == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *FileSize = File->FileSize;
  return EFI_SUCCESS;
}

/**
  Read file into memory by opened file handle.

  @param[in]     FsHandle         FAT file system handle.
  @param[in]     FileHandle       file handle
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
FatFsReadFile (
  IN  EFI_HANDLE                                  FsHandle,
  IN  EFI_HANDLE                                  FileHandle,
  OUT VOID                                      **FileBufferPtr,
  OUT UINTN                                      *FileSize
  )
{
  EFI_STATUS              Status;
  PEI_FAT_FILE           *File;
  VOID                   *FileBuffer;
  PEI_FAT_PRIVATE_DATA   *PrivateData;

  File = (PEI_FAT_FILE *)FileHandle;
  ASSERT (File != NULL);
  if (File == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (File->FileSize == 0) {
    return EFI_SUCCESS;
  }

  PrivateData = (PEI_FAT_PRIVATE_DATA *)FsHandle;
  ASSERT (PrivateData != NULL);
  ASSERT (PrivateData->Signature == FS_FAT_SIGNATURE);
  if (PrivateData == NULL || PrivateData->Signature != FS_FAT_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  ASSERT (FileBufferPtr != NULL);
  if (FileBufferPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FileBuffer = *FileBufferPtr;
  Status = FatReadFile (
             PrivateData,
             File,
             File->FileSize,
             FileBuffer
             );

  if (!EFI_ERROR (Status)) {
    if (FileSize != NULL) {
      *FileSize = (UINTN)File->FileSize;
    }

    DEBUG ((DEBUG_VERBOSE, "  FatFsReadFile: %s(%d bytes) loaded\n", File->FileName, File->FileSize));
  }

  return Status;
}

/**
  Close a file by opened file handle

  @param[in]     FileHandle       file handle

  @retval                         none

**/
VOID
EFIAPI
FatFsCloseFile (
  IN  EFI_HANDLE                                  FileHandle
  )
{
  PEI_FAT_FILE           *File;

  File = (PEI_FAT_FILE *)FileHandle;
  ASSERT (File != NULL);
  if (File == NULL) {
    return;
  }

  DEBUG ((DEBUG_VERBOSE, "  FatFsCloseFile: %s closed\n", File->FileName));
  FreePool (File);
}

/**
  List directories or files

  @param[in]     FsHandle         file system handle.
  @param[in]     DirFilePath      directory or file path

  @retval EFI_SUCCESS             list directories of files successfully
  @retval EFI_UNSUPPORTED         this api is not supported
  @retval Others                  an error occurs

**/
EFI_STATUS
EFIAPI
FatFsListDir (
  IN  EFI_HANDLE                                    FsHandle,
  IN  CHAR16                                       *DirFilePath
  )
{
  EFI_STATUS              Status;
  PEI_FAT_PRIVATE_DATA   *PrivateData;
  PEI_FAT_FILE           *File;
  PEI_FAT_FILE            Parent;
  EFI_HANDLE              FileHandle;

  Status = EFI_UNSUPPORTED;

DEBUG_CODE_BEGIN ();
  FileHandle = NULL;
  Status = FatFsOpenFile (FsHandle, DirFilePath, &FileHandle);
  if (!EFI_ERROR (Status)) {
    PrivateData = (PEI_FAT_PRIVATE_DATA *)FsHandle;
    File = (PEI_FAT_FILE *)FileHandle;

    if (File->Attributes & FAT_ATTR_DIRECTORY) {
      CopyMem (&Parent, File, sizeof (PEI_FAT_FILE));
      do {
        Status = FatReadNextDirectoryEntry (PrivateData, &Parent, 0, File);
        if (EFI_ERROR (Status)) {
          Status = EFI_SUCCESS;
          break;
        }
        PrintFileName (File);
      } while (Status == EFI_SUCCESS);
    } else {
      PrintFileName (File);
    }
  }
  if (FileHandle != NULL) {
    FatFsCloseFile (FileHandle);
  }
DEBUG_CODE_END ();

  return Status;
}
