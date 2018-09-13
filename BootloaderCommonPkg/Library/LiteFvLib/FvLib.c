/** @file
These functions assist in parsing and manipulating a Firmware Volume.

Copyright (c) 2004 - 2015, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "FvLib.h"

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
GetFfsFileByName (
  IN      EFI_FIRMWARE_VOLUME_HEADER   *FvHeader,
  IN      EFI_GUID                     *FileName,
  IN  OUT EFI_FFS_FILE_HEADER         **File
  )
{
  EFI_FFS_FILE_HEADER *CurrentFile;
  EFI_FFS_FILE_HEADER *NextFile;
  EFI_FFS_FILE_HEADER *EndFile;

  //
  // Verify library has been initialized.
  //
  if (FvHeader == NULL || FileName == NULL || File == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (FvHeader->Signature != EFI_FVH_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get the first file
  //
  CurrentFile = (EFI_FFS_FILE_HEADER *) ((UINTN)FvHeader + FvHeader->HeaderLength);

  //
  // Loop as long as we have a valid file
  //
  EndFile = (EFI_FFS_FILE_HEADER *) (UINTN) ((UINTN)FvHeader + FvHeader->FvLength) - 1;
  while (CurrentFile != NULL) {
    if (CompareGuid (&CurrentFile->Name, FileName)) {
      *File = CurrentFile;
      return EFI_SUCCESS;
    }

    //
    // Get next file, compensate for 8 byte alignment if necessary.
    //
    NextFile = (EFI_FFS_FILE_HEADER *) ((((UINTN)CurrentFile - (UINTN)FvHeader + \
                                          GET_FFS_LENGTH (CurrentFile) + 0x07) & (~ (UINTN)7)) + (UINTN)FvHeader);

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
  @param[in] Instance         Instance of the matched section
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

  if ((SectionType != EFI_SECTION_TE) && (SectionType != EFI_SECTION_RAW)) {
    return EFI_UNSUPPORTED;
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
