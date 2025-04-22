/** @file
These functions assist in parsing and manipulating a Firmware Volume.

Copyright (c) 2004 - 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "FvLib.h"

/**
  Check if buffer contains a valid FV header.

  @param[in]        Pointer to a data buffer

  @retval TRUE      The data block contains a valid FV header.
  @retval FALSE     The data block does not contain a valid FV header.
**/
BOOLEAN
EFIAPI
IsValidFvHeader (
  IN    VOID *Buffer
)
{
  EFI_FIRMWARE_VOLUME_HEADER   *FvHeader;

  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)Buffer;
  if ((FvHeader != NULL) && (FvHeader->Signature == EFI_FVH_SIGNATURE)) {
    return TRUE;
  } else {
    return FALSE;
  }
}


/**
  Get first FFS file inside a FV.

  @param[in]   Pointer to a firmware volume header.

  @retval      First FFS file pointer.  NULL if it is not a valid FV image.
**/
STATIC
EFI_FFS_FILE_HEADER  *
GetFirstFfsFileInFv (
  IN    EFI_FIRMWARE_VOLUME_HEADER   *FvHeader
)
{
  EFI_FIRMWARE_VOLUME_EXT_HEADER  *FvExHeader;
  EFI_FFS_FILE_HEADER             *CurrentFile;

  if (!IsValidFvHeader ((VOID *)FvHeader)) {
    return NULL;
  }

  // Get the first file
  CurrentFile = (EFI_FFS_FILE_HEADER *) ((UINTN)FvHeader + FvHeader->HeaderLength);
  if (FvHeader->ExtHeaderOffset != 0) {
    FvExHeader  = (EFI_FIRMWARE_VOLUME_EXT_HEADER *)(((UINT8 *)FvHeader) + FvHeader->ExtHeaderOffset);
    CurrentFile = (EFI_FFS_FILE_HEADER *)(((UINT8 *)FvExHeader) + FvExHeader->ExtHeaderSize);
  }
  CurrentFile = (EFI_FFS_FILE_HEADER *) ALIGN_POINTER (CurrentFile, 8);
  return CurrentFile;
}


/**
  Find a file within a volume by its name.

  This service searches for files with a specific name, within
  either the specified firmware volume or all firmware volumes.

  @param[in]       FvHeader    Pointer to a firmware volume header
  @param[in]       FileName    The GUID file name of the file to search for.
  @param[in, out]  File        Return pointer.  In the case of an error,
                               contents are undefined.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             An error was encountered.
  @retval EFI_NOT_FOUND           File not found.
  @retval EFI_INVALID_PARAMETER   One of the parameters was NULL.

**/
EFI_STATUS
EFIAPI
GetFfsFileByName (
  IN      EFI_FIRMWARE_VOLUME_HEADER   *FvHeader,
  IN      EFI_GUID                     *FileName,
  IN  OUT EFI_FFS_FILE_HEADER         **File
  )
{
  EFI_FFS_FILE_HEADER             *CurrentFile;
  EFI_FFS_FILE_HEADER             *NextFile;
  EFI_FFS_FILE_HEADER             *EndFile;
  UINTN                            Offset;

  //
  // Verify library has been initialized.
  //
  if (FvHeader == NULL || FileName == NULL || File == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Loop as long as we have a valid file
  //
  CurrentFile = GetFirstFfsFileInFv (FvHeader);
  EndFile     = (EFI_FFS_FILE_HEADER *) (UINTN) ((UINTN)FvHeader + FvHeader->FvLength) - 1;
  while (CurrentFile != NULL) {
    if (CompareGuid (&CurrentFile->Name, FileName)) {
      *File = CurrentFile;
      return EFI_SUCCESS;
    }

    //
    // Get next file, compensate for 8 byte alignment if necessary.
    //
    Offset   = (UINTN)CurrentFile - (UINTN)FvHeader + GET_FFS_LENGTH (CurrentFile);
    NextFile = (EFI_FFS_FILE_HEADER *)((UINTN)FvHeader + ALIGN_UP (Offset, 8));
    if ((NextFile <= CurrentFile) || (NextFile > EndFile)) {
      return EFI_ABORTED;
    } else {
      CurrentFile = NextFile;
    }
  }

  //
  // File not found in this FV.
  //
  *File = NULL;
  return EFI_NOT_FOUND;
}


/**
  Find a file within a volume by its type.

  This service searches for files with a specific name, within
  either the specified firmware volume or all firmware volumes.

  @param[in]       FvHeader    Pointer to a firmware volume header
  @param[in]       FileType    The file type to search for.
  @param[in]       Instance    0 based instance of the matched file.
  @param[in, out]  File        Return pointer.  In the case of an error,
                               contents are undefined.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             An error was encountered.
  @retval EFI_NOT_FOUND           File not found.
  @retval EFI_INVALID_PARAMETER   One of the parameters was NULL.

**/
EFI_STATUS
EFIAPI
GetFfsFileByType (
  IN  EFI_FIRMWARE_VOLUME_HEADER      *FvHeader,
  IN  EFI_FV_FILETYPE                  FileType,
  IN  UINT32                           Instance,
  IN  OUT EFI_FFS_FILE_HEADER        **File
  )
{
  EFI_FFS_FILE_HEADER             *CurrentFile;
  EFI_FFS_FILE_HEADER             *NextFile;
  EFI_FFS_FILE_HEADER             *EndFile;
  UINT32                           Count;
  UINTN                            Offset;

  //
  // Verify library has been initialized.
  //
  if (FvHeader == NULL || File == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Loop as long as we have a valid file
  //
  Count = 0;
  EndFile     = (EFI_FFS_FILE_HEADER *) (UINTN) ((UINTN)FvHeader + FvHeader->FvLength) - 1;
  CurrentFile = GetFirstFfsFileInFv (FvHeader);
  while (CurrentFile != NULL) {
    if (CurrentFile->Type == FileType) {
      if (Count == Instance) {
        *File = CurrentFile;
        return EFI_SUCCESS;
      }
      Count++;
    }

    //
    // Get next file, compensate for 8 byte alignment if necessary.
    //
    Offset   = (UINTN)CurrentFile - (UINTN)FvHeader + GET_FFS_LENGTH (CurrentFile);
    NextFile = (EFI_FFS_FILE_HEADER *)((UINTN)FvHeader + ALIGN_UP (Offset, 8));
    if ((NextFile <= CurrentFile) || (NextFile > EndFile)) {
      return EFI_ABORTED;
    } else {
      CurrentFile = NextFile;
    }
  }

  //
  // File not found in this FV.
  //
  *File = NULL;
  return EFI_NOT_FOUND;
}



/**
  Find the next matching section in the firmware file.

  This service enables PEI modules to discover sections
  of a given type within a valid file.

  @param[in] File             Points to FFS file pointer.
  @param[in] SectionType      A filter to find only sections of this
                              type.
  @param[in] Instance         0 based instance of the matched section.
  @param[out] SectionData     Updated upon return to point to the
                              section found.

  @retval EFI_SUCCESS     Section was found.
  @retval EFI_UNSUPPORTED Section type is not supported.
  @retval EFI_NOT_FOUND   Section of the specified type was not
                          found. SectionData contains NULL.
**/
EFI_STATUS
EFIAPI
GetSectionByType (
  IN  EFI_FFS_FILE_HEADER           *File,
  IN  EFI_SECTION_TYPE               SectionType,
  IN  UINT32                         Instance,
  OUT VOID                         **SectionData
  )
{
  UINT8                      *SearchEnd;
  EFI_COMMON_SECTION_HEADER  *CommonSecHeader;
  UINT32                      Count;

  //
  // Verify input parameters
  //
  if (File == NULL || SectionData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Initialize the number of matching sections found.
  //
  SearchEnd = (UINT8 *) ((UINTN)File + GET_FFS_LENGTH (File)),

  //
  // Get the first section
  //
  Count = 0;
  CommonSecHeader = (EFI_COMMON_SECTION_HEADER *) ((UINTN)File + sizeof (EFI_FFS_FILE_HEADER));
  while ((UINTN)CommonSecHeader < (UINTN)SearchEnd) {
    if (CommonSecHeader->Type == SectionType) {
      if (Count == Instance) {
        *SectionData = (VOID *) ((UINTN)CommonSecHeader + sizeof (EFI_COMMON_SECTION_HEADER));
        return EFI_SUCCESS;
      }
      Count++;
    }

    //
    // Find next section (including compensating for alignment issues.
    //
    CommonSecHeader = (EFI_COMMON_SECTION_HEADER *) ((((UINTN)CommonSecHeader) + \
                      GET_SEC_LENGTH (CommonSecHeader) + 0x03) & (~ (UINTN)3));
  }

  //
  // File not found in this FV.
  //
  *SectionData = NULL;
  return EFI_NOT_FOUND;
}


/**
  Load FV to preferred location and return entry point of SEC core.

  This function will search whole FV to find SEC core file, and then check SEC core
  file if relocation is required. If relocation is required, this function will copy
  the whole FV to preferred location and SEC core entry point will be returned if
  success.

  @param[in]  FvBase                   Point to the boot firmware volume.
  @param[in]  FvLength                 The actural length of FV.
  @param[out] EntryPoint               The pointer to receive SecCore entry point.
  @param[out] Machine                  The pointer to receive machine type.

  @retval RETURN_SUCCESS               The FV is loaded successfully.
  @retval Others                       Failed to load the FV.
**/
EFI_STATUS
EFIAPI
LoadFvImage (
  IN  UINT32                            *FvBase,
  IN  UINT32                             FvLength,
  OUT VOID                             **EntryPoint,
  OUT UINT16                            *Machine   OPTIONAL
  )
{
  EFI_STATUS                            Status;
  EFI_FIRMWARE_VOLUME_HEADER           *FvHeader;
  EFI_FFS_FILE_HEADER                  *SecCoreFile;
  EFI_COMMON_SECTION_HEADER            *Section;
  UINT32                                PreferredBase;
  UINT32                                SecCoreImageBase;
  INT32                                 Gap;

  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)FvBase;

  // Find SecCore file first
  Status = GetFfsFileByType (FvHeader, EFI_FV_FILETYPE_SECURITY_CORE, 0, &SecCoreFile);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  // Find PE32 or TE section in SecCore file
  Status = GetSectionByType (SecCoreFile, EFI_SECTION_PE32, 0, (VOID **)&Section);
  if (Status == EFI_NOT_FOUND) {
    Status = GetSectionByType (SecCoreFile, EFI_SECTION_TE, 0, (VOID **)&Section);
  }
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  // Check preferred image base
  SecCoreImageBase = (UINT32)(UINTN)Section;
  Status = PeCoffGetPreferredBase ((VOID *)(UINTN)SecCoreImageBase, &PreferredBase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Move FV to preferred base if required
  Gap = (UINT32)PreferredBase - SecCoreImageBase;
  if (Gap != 0) {
    CopyMem ((UINT8 *)FvHeader + Gap, FvHeader, FvLength);
    SecCoreImageBase += Gap;
  }

  Status = PeCoffLoaderGetMachine ((VOID *)(UINTN)SecCoreImageBase, Machine);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = PeCoffLoaderGetEntryPoint ((VOID *)(UINTN)SecCoreImageBase, EntryPoint);
  return Status;
}
