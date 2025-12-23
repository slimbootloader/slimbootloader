/** @file
  Support functions for rebasing FSP components.

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/LitePeCoffLib.h>
#include <Library/LiteFvLib.h>
#include <FspApiLibInternal.h>

#if FixedPcdGetBool(PcdEnableFspSFvRebase)
#define GET_OCCUPIED_SIZE(ActualSize, Alignment) \
  (ActualSize) + (((Alignment) - ((ActualSize) & ((Alignment) - 1))) & ((Alignment) - 1))

/**
  Given the input file pointer, search for the next matching section in the
  FFS volume.

  @param[in]      SectionType     Filter to find only sections of this type.
  @param[in]      FfsFileHeader   Pointer to the current file to search.
  @param[in,out]  SectionData     Pointer to the Section matching SectionType in FfsFileHeader.
                                  NULL if section not found
  @param[in,out]  SectionDataSize The size of SectionData, excluding the section header.

  @retval  EFI_NOT_FOUND  No files matching the search criteria were found
  @retval  EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
FfsFindSectionData (
  IN EFI_SECTION_TYPE     SectionType,
  IN EFI_FFS_FILE_HEADER  *FfsFileHeader,
  IN OUT VOID             **SectionData,
  IN OUT UINTN            *SectionDataSize
  )
{
  UINT32                     FileSize;
  EFI_COMMON_SECTION_HEADER  *Section;
  UINT32                     SectionLength;
  UINT32                     ParsedLength;

  //
  // Size is 24 bits wide so mask upper 8 bits.
  // Does not include FfsFileHeader header size
  // FileSize is adjusted to FileOccupiedSize as it is 8 byte aligned.
  //
  if (IS_FFS_FILE2 (FfsFileHeader)) {
    Section  = (EFI_COMMON_SECTION_HEADER *)((EFI_FFS_FILE_HEADER2 *)FfsFileHeader + 1);
    FileSize = FFS_FILE2_SIZE (FfsFileHeader) - sizeof (EFI_FFS_FILE_HEADER2);
  } else {
    Section  = (EFI_COMMON_SECTION_HEADER *)(FfsFileHeader + 1);
    FileSize = FFS_FILE_SIZE (FfsFileHeader) - sizeof (EFI_FFS_FILE_HEADER);
  }

  *SectionData = NULL;
  ParsedLength = 0;
  while (ParsedLength < FileSize) {
    if (Section->Type == SectionType) {
      if (IS_SECTION2 (Section)) {
        *SectionData     = (VOID *)((EFI_COMMON_SECTION_HEADER2 *)Section + 1);
        *SectionDataSize = SECTION2_SIZE (Section) - sizeof (EFI_COMMON_SECTION_HEADER2);
      } else {
        *SectionData     = (VOID *)(Section + 1);
        *SectionDataSize = SECTION_SIZE (Section) - sizeof (EFI_COMMON_SECTION_HEADER);
      }

      return EFI_SUCCESS;
    }

    //
    // Size is 24 bits wide so mask upper 8 bits.
    // SectionLength is adjusted it is 4 byte aligned.
    // Go to the next section
    //
    SectionLength = IS_SECTION2 (Section) ? SECTION2_SIZE (Section) : SECTION_SIZE (Section);
    SectionLength = GET_OCCUPIED_SIZE (SectionLength, 4);

    ParsedLength += SectionLength;
    Section       = (EFI_COMMON_SECTION_HEADER *)((UINT8 *)Section + SectionLength);
  }

  return EFI_NOT_FOUND;
}

/**
  Rebase a PE32/TE Image from an FFS file.

  @param FileHeader   Pointer to the FFS file header.

  @return Status of the rebase operation.
**/
EFI_STATUS
RebasePeTeFromFfs (
  EFI_FFS_FILE_HEADER  *FileHeader
  )
{
  EFI_STATUS                    Status;
  VOID                          *ImageBase;
  UINTN                         ImageSize;

  Status = FfsFindSectionData (EFI_SECTION_PE32, FileHeader, &ImageBase, &ImageSize);
  if (EFI_ERROR (Status)) {
    Status = FfsFindSectionData (EFI_SECTION_TE, FileHeader, &ImageBase, &ImageSize);
  }

  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  Status = PeCoffRelocateImage ((UINT32)(UINTN)ImageBase);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Rebase Fsp Fv.
  Only SEC and PEI Core FFS files need to be rebased. Other PEIMs will
  be rebased by PEI Core when dispatching.
  FSP FV must contain SEC FFS and may contains PEI Core FFS files.

  @param FspFvBaseAddress   Fsp Fv base address.

  @return Status of the rebase result.
**/
EFI_STATUS
RebaseFspFv (
  UINT64  FspFvBaseAddress
  )
{
  EFI_STATUS                  Status;
  EFI_FIRMWARE_VOLUME_HEADER  *FwVolHeader;
  EFI_FFS_FILE_HEADER         *FileHeader;

  FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)FspFvBaseAddress;

  //
  // Find and rebase SEC FFS file
  //
  FileHeader = NULL;
  Status     = GetFfsFileByType (FwVolHeader, EFI_FV_FILETYPE_SECURITY_CORE, 0, &FileHeader);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  Status = RebasePeTeFromFfs (FileHeader);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Find and rebase PEI Core FFS file
  //
  FileHeader = NULL;
  Status     = GetFfsFileByType (FwVolHeader, EFI_FV_FILETYPE_PEI_CORE, 0, &FileHeader);
  if (!EFI_ERROR (Status)) {
    Status = RebasePeTeFromFfs (FileHeader);
    ASSERT_EFI_ERROR (Status);
  } else {
    return EFI_SUCCESS;
  }

  return Status;
}
#endif

/**
  This FSP API is used to notify the FSP about the different phases in the boot process.
  This allows the FSP to take appropriate actions as needed during different initialization
  phases. The phases will be platform dependent and will be documented with the FSP
  release. The current FSP supports following notify phases:
    Post PCI enumeration
    Ready To Boot
    End of firmware

  @param[in] Delta              The delta between the new and old base.
                                Delta = NewBase - OldBase

  @retval EFI_SUCCESS           The rebasing was handled successfully.

**/
EFI_STATUS
EFIAPI
RebaseFspComponent (
  UINT32   Delta
  )
{
  FSP_INFO_HEADER   *FspHeader;

  FspHeader = (FSP_INFO_HEADER *)(UINTN)(PcdGet32 (PcdFSPSBase) + Delta + FSP_INFO_HEADER_OFF);
  FspHeader->ImageBase += Delta;

#if FixedPcdGetBool(PcdEnableFspSFvRebase)
  RebaseFspFv ((UINT64)(UINTN)(PcdGet32 (PcdFSPSBase) + Delta));
#endif

  return EFI_SUCCESS;
}
