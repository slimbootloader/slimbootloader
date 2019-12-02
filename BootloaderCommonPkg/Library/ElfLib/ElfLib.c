/** @file
  ELF library

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include "ElfCommon.h"
#include "Elf32.h"

/**
  Load a single segment from each program header

  @param[in]  ImageBase           Memory address of an image.
  @param[out] ProgramHdr          A specific program header to be loaded

  @retval EFI_INVALID_PARAMETER   Input parameters are not valid.
  @retval EFI_LOAD_ERROR          ELF binary loading error.
  @retval EFI_SUCCESS             ELF binary is loaded successfully.

**/
STATIC
EFI_STATUS
LoadSegments32 (
  IN  CONST VOID                  *ImageBase,
  IN  CONST Elf32_Phdr            *ProgramHdr
  )
{
  ASSERT (ImageBase != NULL);
  ASSERT (ProgramHdr != NULL);
  if (ImageBase == NULL || ProgramHdr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (ProgramHdr->p_type != PT_LOAD) {
    return EFI_SUCCESS;
  }

  //
  // Skip 0 size segment. But, allow 0x0 address.
  //
  if (ProgramHdr->p_memsz == 0) {
    return EFI_SUCCESS;
  }

  if (ProgramHdr->p_filesz > ProgramHdr->p_memsz) {
    return EFI_LOAD_ERROR;
  }

  CopyMem ((VOID *)(UINTN)ProgramHdr->p_paddr,
      (UINT8 *)ImageBase + ProgramHdr->p_offset,
      ProgramHdr->p_filesz);

  if (ProgramHdr->p_memsz > ProgramHdr->p_filesz) {
    ZeroMem ((VOID *)((UINTN)ProgramHdr->p_paddr + ProgramHdr->p_filesz),
      ProgramHdr->p_memsz - ProgramHdr->p_filesz);
  }

  return EFI_SUCCESS;
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
LoadElf32 (
  IN  CONST VOID             *ImageBase,
  OUT       VOID            **EntryPoint
  )
{
  EFI_STATUS    Status;
  Elf32_Ehdr   *ElfHdr;
  Elf32_Phdr   *ProgramHdr;
  UINT16        Index;

  ElfHdr      = (Elf32_Ehdr *)ImageBase;
  ProgramHdr  = (Elf32_Phdr *)((UINT8 *)ElfHdr + ElfHdr->e_phoff);
  for (Index = 0; Index < ElfHdr->e_shnum; Index++) {
    Status = LoadSegments32 (ImageBase, ProgramHdr);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    ProgramHdr = (Elf32_Phdr *)((UINT8 *)ProgramHdr + ElfHdr->e_phentsize);
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
  Elf32_Ehdr *ElfHdr;

  ElfHdr = (Elf32_Ehdr *)ImageBase;

  //
  // Support 32-bit architecture only
  //
  if (ElfHdr->e_ident[EI_CLASS] != ELFCLASS32) {
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
  if (ElfHdr->e_machine != EM_386) {
    return FALSE;
  }

  //
  //  Support ELF types: EXEC (Executable file), DYN (Shared object file)
  //
  if ((ElfHdr->e_type != ET_EXEC) && (ElfHdr->e_type != ET_DYN)) {
    return FALSE;
  }

  if (ElfHdr->e_version != EV_CURRENT) {
    return FALSE;
  }

  return TRUE;
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
  IN  CONST VOID                  *ImageBase,
  OUT       VOID                 **EntryPoint
  )
{
  EFI_STATUS    Status;
  UINT8         EiClass;

  ASSERT (ImageBase != NULL);
  ASSERT (EntryPoint != NULL);
  if (ImageBase == NULL || EntryPoint == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (!IsElfImage (ImageBase)) {
    return EFI_UNSUPPORTED;
  }

  Status = EFI_LOAD_ERROR;
  EiClass = ((UINT8 *)ImageBase)[EI_CLASS];
  if (EiClass == ELFCLASS32) {
    Status = LoadElf32 (ImageBase, EntryPoint);
  } else {
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}
