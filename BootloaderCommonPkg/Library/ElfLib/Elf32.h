/** @file
Ported ELF include files from FreeBSD

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
Copyright (c) 2009 - 2010, Apple Inc. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


**/
/*-
 * Copyright (c) 1996-1998 John D. Polstra.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/sys/sys/elf32.h,v 1.8.14.2 2007/12/03 21:30:36 marius Exp $
 */

#ifndef _SYS_ELF32_H_
#define _SYS_ELF32_H_   1

#include "ElfCommon.h"

//
// ELF definitions common to all 32-bit architectures.
//
typedef UINT32       ELF32_ADDR;
typedef UINT16       ELF32_HALF;
typedef UINT32       ELF32_OFF;
typedef INT32        ELF32_SWORD;
typedef UINT32       ELF32_WORD;
typedef UINT64       Elf32_LWORD;

typedef ELF32_WORD   Elf32_HASHELT;

// Non-standard class-dependent datatype used for abstraction.
typedef ELF32_WORD   ELF32_SIZE;
typedef ELF32_SWORD  ELF32_SSIZE;

//
// ELF header.
//
typedef struct {
  UINT8             E_Ident[EI_NIDENT];      // File identification.
  ELF32_HALF        E_Type;                  // File type.
  ELF32_HALF        E_Machine;               // Machine architecture.
  ELF32_WORD        E_Version;               // ELF format version.
  ELF32_ADDR        E_Entry;                 // Entry point.
  ELF32_OFF         E_Phoff;                 // Program header file offset.
  ELF32_OFF         E_Shoff;                 // Section header file offset.
  ELF32_WORD        E_Flags;                 // Architecture-specific flags.
  ELF32_HALF        E_Ehsize;                // Size of ELF header in bytes.
  ELF32_HALF        E_Phentsize;             // Size of program header entry.
  ELF32_HALF        E_Phnum;                 // Number of program header entries.
  ELF32_HALF        E_Shentsize;             // Size of section header entry */
  ELF32_HALF        E_Shnum;                 // Number of section header entries.
  ELF32_HALF        E_Shstrndx;              // Section name strings section.
} ELF32_EHDR;

//
// Section header.
//
typedef struct {
  ELF32_WORD  Sh_Name;       // Section name (index into the section header string table).
  ELF32_WORD  Sh_Type;       // Section type.
  ELF32_WORD  Sh_Flags;      // Section flags.
  ELF32_ADDR  Sh_Addr;       // Address in memory image.
  ELF32_OFF   Sh_Offset;     // Offset in file.
  ELF32_WORD  Sh_Size;       // Size in bytes.
  ELF32_WORD  Sh_Link;       // Index of a related section.
  ELF32_WORD  Sh_Info;       // Depends on section type.
  ELF32_WORD  Sh_Addralign;  // Alignment in bytes.
  ELF32_WORD  Sh_Entsize;    // Size of each entry in section.
} ELF32_SHDR;

//
// Program header.
//
typedef struct {
  ELF32_WORD  P_Type;    // Entry type.
  ELF32_OFF   P_Offset;  // File offset of contents.
  ELF32_ADDR  P_Vaddr;   // Virtual address in memory image.
  ELF32_ADDR  P_Paddr;   // Physical address (not used).
  ELF32_WORD  P_Filesz;  // Size of contents in file.
  ELF32_WORD  P_Memsz;   // Size of contents in memory.
  ELF32_WORD  P_Flags;   // Access permission flags.
  ELF32_WORD  P_Align;   // Alignment in memory and file.
} ELF32_PHDR;

//
// Dynamic structure.  The ".dynamic" section contains an array of them.
//

typedef struct {
  ELF32_SWORD   D_Tag;   // Entry type.
  union {
    ELF32_WORD  D_Val;   // Integer value.
    ELF32_ADDR  D_Ptr;   // Address value.
  } D_UN;
} ELF32_DYN;

//
// Relocation entries.
//

// Relocations that don't need an addend field.
typedef struct {
  ELF32_ADDR  r_offset;  // Location to be relocated.
  ELF32_WORD  r_info;    // Relocation type and symbol index.
} ELF32_REL;

// Relocations that need an addend field.
typedef struct {
  ELF32_ADDR  r_offset;  // Location to be relocated.
  ELF32_WORD  r_info;    // Relocation type and symbol index.
  ELF32_SWORD  r_addend; // Addend.
} ELF32_RELA;

// Macros for accessing the fields of r_info.
#define ELF32_R_SYM(info)  ((info) >> 8)
#define ELF32_R_TYPE(info)  ((unsigned char)(info))

// Macro for constructing r_info from field values.
#define ELF32_R_INFO(sym, type)  (((sym) << 8) + (unsigned char)(type))

//
//  Note entry header
//
typedef ELF_NOTE   ELF32_NHDR;

//
// Move entry
//
typedef struct {
  Elf32_LWORD  M_Value;   // symbol value
  ELF32_WORD   M_Info;    // size + index
  ELF32_WORD   M_Poffset; // symbol offset
  ELF32_HALF   M_Repeat;  // repeat count
  ELF32_HALF   M_Stride;  // stride info
} ELF32_MOVE;

//
//  The macros compose and decompose values for Move.r_info
//
//  sym  = ELF32_M_SYM(M.m_info)
//  size = ELF32_M_SIZE(M.m_info)
//  M.m_info = ELF32_M_INFO(sym, size)
//
#define  ELF32_M_SYM(info)        ((info)>>8)
#define  ELF32_M_SIZE(info)       ((unsigned char)(info))
#define  ELF32_M_INFO(sym, size)  (((sym)<<8)+(unsigned char)(size))

//
//  Hardware/Software capabilities entry
//
typedef struct {
  ELF32_WORD    C_Tag;    // how to interpret value
  union {
    ELF32_WORD  C_Val;
    ELF32_ADDR  C_Ptr;
  } C_UN;
} ELF32_CAP;

//
// Symbol table entries.
//

typedef struct {
  ELF32_WORD  St_Name;  // String table index of name.
  ELF32_ADDR  St_Value; // Symbol value.
  ELF32_WORD  St_Size;  // Size of associated object.
  CHAR8       St_Info;  // Type and binding information.
  CHAR8       St_Other; // Reserved (not used).
  ELF32_HALF  St_Shndx; // Section index of symbol.
} ELF32_SYM;

// Macros for accessing the fields of st_info.
#define ELF32_ST_BIND(info)    ((info) >> 4)
#define ELF32_ST_TYPE(info)    ((info) & 0xf)

// Macro for constructing st_info from field values.
#define ELF32_ST_INFO(bind, type)  (((bind) << 4) + ((type) & 0xf))

// Macro for accessing the fields of st_other.
#define ELF32_ST_VISIBILITY(oth)  ((oth) & 0x3)

// Structures used by Sun & GNU symbol versioning.
typedef struct
{
  ELF32_HALF  Vd_Version;
  ELF32_HALF  Vd_Flags;
  ELF32_HALF  Vd_Ndx;
  ELF32_HALF  Vd_Cnt;
  ELF32_WORD  Vd_Hash;
  ELF32_WORD  Vd_Aux;
  ELF32_WORD  Vd_Next;
} ELF32_VERDEF;

typedef struct
{
  ELF32_WORD  Vda_Name;
  ELF32_WORD  Vda_Next;
} ELF32_VERDAUX;

typedef struct
{
  ELF32_HALF  Vn_Version;
  ELF32_HALF  Vn_Cnt;
  ELF32_WORD  Vn_File;
  ELF32_WORD  Vn_Aux;
  ELF32_WORD  Vn_Next;
} ELF32_VERNEED;

typedef struct
{
  ELF32_WORD  Vna_Hash;
  ELF32_HALF  Vna_Flags;
  ELF32_HALF  Vna_Other;
  ELF32_WORD  Vna_Name;
  ELF32_WORD  Vna_Next;
} ELF32_VERNAUX;

typedef ELF32_HALF   ELF32_VERSYM;

typedef struct {
  ELF32_HALF  Si_Boundto;  // direct bindings - symbol bound to
  ELF32_HALF  Si_Flags;    // per symbol flags
} ELF32_SYMINFO;

#endif // _SYS_ELF32_H_
