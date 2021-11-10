/** @file
  Basic graphics rendering support

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __LITE_FV_LIB_H__
#define __LITE_FV_LIB_H__

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
);

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
  OUT UINT16                            *Machine  OPTIONAL
  );

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
  );


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
  );

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
  IN        EFI_FFS_FILE_HEADER           *File,
  IN        EFI_SECTION_TYPE               SectionType,
  IN        UINT32                         Instance,
  OUT       VOID                         **SectionData
  );

#endif
