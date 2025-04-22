/** @file
  ELF library

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _EFI_LIB_INTERNAL_H_
#define _EFI_LIB_INTERNAL_H_

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ElfLib.h>
#include "ElfCommon.h"
#include "Elf32.h"
#include "Elf64.h"


Elf32_Shdr *
GetElf32SectionByIndex (
  IN  UINT8                 *ImageBase,
  IN  UINT32                Index
  );

Elf64_Shdr *
GetElf64SectionByIndex (
  IN  UINT8                 *ImageBase,
  IN  UINT32                Index
  );

Elf32_Phdr *
GetElf32SegmentByIndex (
  IN  UINT8                 *ImageBase,
  IN  UINT32                Index
  );

Elf64_Phdr *
GetElf64SegmentByIndex (
  IN  UINT8                 *ImageBase,
  IN  UINT32                Index
  );

/**
  Load ELF image which has 32-bit architecture

  @param[in]  ElfCt               ELF image context pointer.

  @retval EFI_SUCCESS         ELF binary is loaded successfully.
  @retval Others              Loading ELF binary fails.

**/
EFI_STATUS
LoadElf32Image (
  IN    ELF_IMAGE_CONTEXT    *ElfCt
  );

/**
  Load ELF image which has 64-bit architecture

  @param[in]  ImageBase       Memory address of an image.
  @param[out] EntryPoint      The entry point of loaded ELF image.

  @retval EFI_SUCCESS         ELF binary is loaded successfully.
  @retval Others              Loading ELF binary fails.

**/
EFI_STATUS
LoadElf64Image (
  IN    ELF_IMAGE_CONTEXT    *ElfCt
  );


EFI_STATUS
RelocateElf32Sections  (
  IN    ELF_IMAGE_CONTEXT      *ElfCt
  );


EFI_STATUS
RelocateElf64Sections  (
  IN    ELF_IMAGE_CONTEXT      *ElfCt
  );

#endif
