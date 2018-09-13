/** @file
  Basic graphics rendering support

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __LITE_FV_LIB_H__
#define __LITE_FV_LIB_H__

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
