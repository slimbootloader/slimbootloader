/** @file
  ELF library

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "ElfLibInternal.h"

/**
  Check if the image has ELF Header

  @param[in]  ImageBase       Memory address of an image.

  @retval     TRUE if found ELF Header, otherwise FALSE

**/
STATIC
BOOLEAN
IsElfHeader (
  IN  CONST UINT8             *ImageBase
  )
{
  return ((ImageBase != NULL) &&
          (ImageBase[EI_MAG0] == ELFMAG0) &&
          (ImageBase[EI_MAG1] == ELFMAG1) &&
          (ImageBase[EI_MAG2] == ELFMAG2) &&
          (ImageBase[EI_MAG3] == ELFMAG3));
}

/**
  Check if the image is 32-bit ELF Format

  @param[in]  ImageBase       Memory address of an image.

  @retval     TRUE if ELF32, otherwise FALSE

**/
STATIC
BOOLEAN
IsElfFormat (
  IN  CONST UINT8             *ImageBase
  )
{
  Elf_Ehdr                  *ElfHdr;

  if (ImageBase == NULL) {
    return FALSE;
  }

  ElfHdr = (Elf_Ehdr *)ImageBase;

  //
  // Check 32/64-bit architecture
  //
  if (ElfHdr->e_ident[EI_CLASS] != ELFCLASS) {
    return FALSE;
  }

  //
  // Support little-endian only
  //
  if (ElfHdr->e_ident[EI_DATA] != ELFDATA2LSB) {
    return FALSE;
  }

  //
  // Support intel architecture only for now
  //
  if (ElfHdr->e_machine != ELF_EM) {
    return FALSE;
  }

  //
  //  Support ELF types: EXEC (Executable file), DYN (Shared object file)
  //
  if ((ElfHdr->e_type != ET_EXEC) && (ElfHdr->e_type != ET_DYN)) {
    return FALSE;
  }

  //
  // Support current ELF version only
  //
  if (ElfHdr->e_version != EV_CURRENT) {
    return FALSE;
  }

  return TRUE;
}

/**
  Load ELF image which has 32-bit architecture

  @param[in]  ImageBase       Memory address of an image.
  @param[out] EntryPoint      The entry point of loaded ELF image.

  @retval EFI_SUCCESS         ELF binary is loaded successfully.
  @retval Others              Loading ELF binary fails.

**/
STATIC
EFI_STATUS
LoadElfSegments (
  IN  CONST UINT8            *ImageBase,
  OUT       VOID            **EntryPoint
  )
{
  Elf_Ehdr   *ElfHdr;
  Elf_Phdr   *ProgramHdr;
  UINT16        Index;

  if ((ImageBase == NULL) || (EntryPoint == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ElfHdr      = (Elf_Ehdr *)ImageBase;
  ProgramHdr  = (Elf_Phdr *)(ImageBase + ElfHdr->e_phoff);
  for (Index = 0; Index < ElfHdr->e_phnum; Index++) {
    ProgramHdr = (Elf_Phdr *)((UINT8 *)ProgramHdr + Index * ElfHdr->e_phentsize);

    if ((ProgramHdr->p_type != PT_LOAD) ||
        (ProgramHdr->p_memsz == 0) ||
        (ProgramHdr->p_offset == 0)) {
      continue;
    }

    if (ProgramHdr->p_filesz > ProgramHdr->p_memsz) {
      return EFI_LOAD_ERROR;
    }

    CopyMem ((VOID *)(UINTN)ProgramHdr->p_paddr,
        ImageBase + ProgramHdr->p_offset,
        (UINTN)ProgramHdr->p_filesz);

    if (ProgramHdr->p_memsz > ProgramHdr->p_filesz) {
      ZeroMem ((VOID *)(UINTN)(ProgramHdr->p_paddr + ProgramHdr->p_filesz),
        (UINTN)(ProgramHdr->p_memsz - ProgramHdr->p_filesz));
    }
  }

  *EntryPoint = (VOID *)(UINTN)ElfHdr->e_entry;

  return EFI_SUCCESS;
}

/**
  Check if the image is a bootable ELF image.

  @param[in]  ImageBase      Memory address of an image

  @retval     TRUE           Image is a bootable ELF image
  @retval     FALSE          Not a bootable ELF image
**/
BOOLEAN
EFIAPI
IsElfImage (
  IN  CONST VOID            *ImageBase
  )
{
  return ((ImageBase != NULL) &&
          (IsElfHeader (ImageBase)) &&
          (IsElfFormat ((CONST UINT8 *)ImageBase)));
}

/**
  Load the ELF image to specified address in ELF header.

  This function loads ELF image segments into memory address specified
  in ELF program header.

  @param[in]  ImageBase           Memory address of an image.
  @param[out] EntryPoint          The entry point of loaded ELF image.

  @retval EFI_INVALID_PARAMETER   Input parameters are not valid.
  @retval EFI_UNSUPPORTED         Unsupported binary type.
  @retval EFI_LOAD_ERROR          ELF binary loading error.
  @retval EFI_SUCCESS             ELF binary is loaded successfully.
**/
EFI_STATUS
LoadElfImage (
  IN  CONST VOID                  *ElfBuffer,
  OUT       VOID                 **EntryPoint
  )
{
  EFI_STATUS    Status;
  CONST UINT8  *ImageBase;

  ASSERT (ElfBuffer != NULL);
  ASSERT (EntryPoint != NULL);
  if (ElfBuffer == NULL || EntryPoint == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ImageBase = ElfBuffer;
  if (IsElfImage (ImageBase)) {
    Status = LoadElfSegments (ImageBase, EntryPoint);
  } else {
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}
