/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _ELF_LIB_INTERNAL_H_
#define _ELF_LIB_INTERNAL_H_

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/PeImage.h>
#include "ElfCommon.h"
#include "Elf32.h"
#include "Elf64.h"

#if defined (MDE_CPU_IA32)

typedef Elf32_Shdr      Elf_Shdr;
typedef Elf32_Ehdr      Elf_Ehdr;
typedef Elf32_Rel       Elf_Rel;
typedef Elf32_Sym       Elf_Sym;
typedef Elf32_Phdr      Elf_Phdr;
typedef Elf32_Dyn       Elf_Dyn;
#define ELFCLASS        ELFCLASS32
#define ELF_R_TYPE(r)   ELF32_R_TYPE(r)
#define ELF_R_SYM(r)    ELF32_R_SYM(r)
#define ELF_EM          EM_386

#elif defined (MDE_CPU_X64)

typedef Elf64_Shdr      Elf_Shdr;
typedef Elf64_Ehdr      Elf_Ehdr;
typedef Elf64_Rel       Elf_Rel;
typedef Elf64_Rela      Elf_Rela;
typedef Elf64_Sym       Elf_Sym;
typedef Elf64_Phdr      Elf_Phdr;
typedef Elf64_Dyn       Elf_Dyn;

#define ELFCLASS        ELFCLASS64
#define ELF_R_TYPE(r)   ELF64_R_TYPE(r)
#define ELF_R_SYM(r)    ELF64_R_SYM(r)
#define ELF_EM          EM_X86_64

#else
#error Unknown Processor Type
#endif

#endif // _ELF_LIB_INTERNAL_H_
