/** @file
  ELF library

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "ElfLibInternal.h"

#define ELF_CR(Record, TYPE, Field)         (((TYPE *) Record)->Field)
#define ELF_CLASS_CR(Record, TYPE, Field, IsElf64)  \
  IsElf64 ? ELF_CR(Record,Elf64_##TYPE,Field) : ELF_CR(Record,Elf32_##TYPE,Field)

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
  Check if the image is 64-bit ELF Format

  @param[in]  ImageBase       Memory address of an image.

  @retval     TRUE if ELF32, otherwise FALSE

**/
STATIC
BOOLEAN
IsElf64Format (
  IN  CONST UINT8             *ImageBase
  )
{
  Elf32_Ehdr                  *ElfHdr;

  ElfHdr = (Elf32_Ehdr *)ImageBase;
  if (ElfHdr->e_ident[EI_CLASS] == ELFCLASS64) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Check if the image is ELF Format

  @param[in]  ImageBase       Memory address of an image.

  @retval     TRUE if ELF32, otherwise FALSE

**/
STATIC
BOOLEAN
IsElfFormat (
  IN  CONST UINT8             *ImageBase
  )
{
  Elf32_Ehdr                  *ElfHdr;
  UINT16                       Data16;
  UINT32                       Data32;
  BOOLEAN                      IsElf64;

  if (ImageBase == NULL) {
    return FALSE;
  }

  ElfHdr = (Elf32_Ehdr *)ImageBase;

  //
  // Check 32/64-bit architecture
  //
  IsElf64 = IsElf64Format (ImageBase);

  //
  // Support little-endian only
  //
  if (ElfHdr->e_ident[EI_DATA] != ELFDATA2LSB) {
    DEBUG ((DEBUG_VERBOSE, "IsElfFormat: Not Little-Endian!\n"));
    return FALSE;
  }

  //
  // Support intel architecture only for now
  //
  Data16 = (UINT16)ELF_CLASS_CR (ImageBase, Ehdr, e_machine, IsElf64);
  if ((Data16 != EM_386) && (Data16 != EM_X86_64)) {
    DEBUG ((DEBUG_VERBOSE, "IsElfFormat: Not Intel Architecture!\n"));
    return FALSE;
  }

  //
  //  Support ELF types: EXEC (Executable file), DYN (Shared object file)
  //
  Data16 = (UINT16)ELF_CLASS_CR (ImageBase, Ehdr, e_type, IsElf64);
  if ((Data16 != ET_EXEC) && (Data16 != ET_DYN)) {
    DEBUG ((DEBUG_VERBOSE, "IsElfFormat: Not Support ELF types(%d)!\n", Data16));
    return FALSE;
  }

  //
  // Support current ELF version only
  //
  Data32 = (UINT32)ELF_CLASS_CR (ImageBase, Ehdr, e_version, IsElf64);
  if (Data32 != EV_CURRENT) {
    DEBUG ((DEBUG_VERBOSE, "IsElfFormat: Not Support ELF version(0x%08X)!\n", Data32));
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
  VOID       *ProgramHdr;
  UINT8      *ProgramHdrBase;
  BOOLEAN     IsElf64;
  UINT16      Index;
  UINT16      ProgramHdrNum;
  UINT16      ProgramHdrEntSize;
  UINT32      ProgramHdrType;
  UINT64      ProgramHdrOffset;
  UINT64      ProgramHdrFileOffset;
  UINT64      ProgramHdrFileSize;
  UINT64      ProgramHdrMemSize;
  UINT64      ProgramHdrPaddr;
  UINT64      ElfEntry;

  if ((ImageBase == NULL) || (EntryPoint == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  IsElf64 = IsElf64Format (ImageBase);
  ProgramHdrOffset  = ELF_CLASS_CR (ImageBase, Ehdr, e_phoff, IsElf64);
  ProgramHdrBase    = (UINT8 *)ImageBase + ProgramHdrOffset;
  ProgramHdrNum     = ELF_CLASS_CR (ImageBase, Ehdr, e_phnum, IsElf64);
  ProgramHdrEntSize = ELF_CLASS_CR (ImageBase, Ehdr, e_phentsize, IsElf64);
  for (Index = 0; Index < ProgramHdrNum; Index++) {
    ProgramHdr = (VOID *)(ProgramHdrBase + Index * ProgramHdrEntSize);

    ProgramHdrType        = ELF_CLASS_CR (ProgramHdr, Phdr, p_type, IsElf64);
    ProgramHdrFileOffset  = ELF_CLASS_CR (ProgramHdr, Phdr, p_offset, IsElf64);
    ProgramHdrMemSize     = ELF_CLASS_CR (ProgramHdr, Phdr, p_memsz, IsElf64);
    if ((ProgramHdrType != PT_LOAD) ||
        (ProgramHdrMemSize == 0) ||
        (ProgramHdrFileOffset == 0)) {
      continue;
    }

    ProgramHdrFileSize = ELF_CLASS_CR (ProgramHdr, Phdr, p_filesz, IsElf64);
    if (ProgramHdrFileSize > ProgramHdrMemSize) {
      return EFI_LOAD_ERROR;
    }

    ProgramHdrPaddr = ELF_CLASS_CR (ProgramHdr, Phdr, p_paddr, IsElf64);
    CopyMem ((VOID *)(UINTN)ProgramHdrPaddr,
             (VOID *)(UINTN)(ImageBase + ProgramHdrFileOffset),
             (UINTN)ProgramHdrFileSize);

    if (ProgramHdrMemSize > ProgramHdrFileSize) {
      ZeroMem ((VOID *)(UINTN)(ProgramHdrPaddr + ProgramHdrFileSize),
               (UINTN)(ProgramHdrMemSize - ProgramHdrFileSize));
    }
  }

  ElfEntry = ELF_CLASS_CR (ImageBase, Ehdr, e_entry, IsElf64);
  *EntryPoint = (VOID *)(UINTN)ElfEntry;

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

  @param[in]  ElfBuffer           Memory address of an image.
  @param[out] EntryPoint          The entry point of loaded ELF image.

  @retval EFI_INVALID_PARAMETER   Input parameters are not valid.
  @retval EFI_UNSUPPORTED         Unsupported binary type.
  @retval EFI_LOAD_ERROR          ELF binary loading error.
  @retval EFI_SUCCESS             ELF binary is loaded successfully.
**/
EFI_STATUS
EFIAPI
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

/**
  Extract and return the machine type from ELF image.

  @param[in]  ElfBuffer           Memory address of an image.
  @param[out] MachinePtr          The pointer to machine type to return.

  @retval EFI_SUCCESS             Machine was returned successfully.
  @retval EFI_UNSUPPORTED         Unsupported image format.
  @retval EFI_INVALID_PARAMETER   The ElfBuffer pointer is NULL.

**/
EFI_STATUS
EFIAPI
GetElfMachine (
  IN  VOID                        *ElfBuffer,
  OUT UINT16                      *MachinePtr      OPTIONAL
  )
{
  if (ElfBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IsElfImage (ElfBuffer)) {
    return EFI_UNSUPPORTED;
  }

  if (MachinePtr != NULL) {
    if (IsElf64Format (ElfBuffer)) {
      *MachinePtr = IMAGE_FILE_MACHINE_X64;
    } else {
      *MachinePtr = IMAGE_FILE_MACHINE_I386;
    }
  }

  return EFI_SUCCESS;
}
