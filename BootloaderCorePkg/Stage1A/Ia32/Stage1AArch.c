/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "Stage1A.h"

//
// Global Descriptor Table (GDT)
//
CONST GLOBAL_REMOVE_IF_UNREFERENCED IA32_SEGMENT_DESCRIPTOR
mGdtEntries[STAGE_GDT_ENTRY_COUNT] = {
  /* selector { Global Segment Descriptor                              } */
  /* 0x00 */  {{0,      0,  0,  0,    0,  0,  0,  0,    0,  0, 0,  0,  0}}, //null descriptor
  /* 0x08 */  {{0xffff, 0,  0,  0x2,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //linear data segment descriptor
  /* 0x10 */  {{0xffff, 0,  0,  0xf,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //linear code segment descriptor
  /* 0x18 */  {{0xffff, 0,  0,  0x3,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //system data segment descriptor
  /* 0x20 */  {{0xffff, 0,  0,  0x2,  1,  0,  1,  0x0,  0,  0, 0,  0,  0}}, //16-bit data segment descriptor
  /* 0x28 */  {{0xffff, 0,  0,  0xB,  1,  0,  1,  0x0,  0,  0, 0,  0,  0}}, //16-bit code segment descriptor
};

/**
  Load IDT table for current processor.

  It function initializes the exception handlers in IDT table and
  loads it into current processor.  It also try to store a specific DWORD
  data just before actual IDT base. It is used to set/get bootloader
  global data structure pointer.

  @param  IdtTable  Contain the IDT table pointer.
                    It must point to a STAGE_IDT_TABLE buffer preallocated.

  @param  Data      It contains a DWORD data that will be stored just before the actual IDT table.
                    It can be used to set/get global data using IDT pointer.

**/
VOID
LoadIdt (
  IN STAGE_IDT_TABLE   *IdtTable,
  IN UINT32             Data
  )
{
  IA32_DESCRIPTOR             IdtDescriptor;

  IdtTable->LdrGlobal  = Data;

  IdtDescriptor.Base  = (UINTN) &IdtTable->IdtTable;
  IdtDescriptor.Limit = (UINT16) (sizeof (IdtTable->IdtTable) - 1);
  UpdateExceptionHandler (&IdtDescriptor);
}

/**
  Load GDT table for current processor.

  It function initializes GDT table and loads it into current processor.

  @param[in]  GdtTable  Pointer to STAGE_GDT_TABLE structure.

**/
VOID
LoadGdt (
  IN STAGE_GDT_TABLE   *GdtTable
  )
{
  IA32_DESCRIPTOR     Gdtr;
  UINT32              GdtLen;

  AsmReadGdtr (&Gdtr);
  GdtLen = sizeof(GdtTable->GdtTable);
  if (GdtLen > sizeof(mGdtEntries)) {
    GdtLen = sizeof(mGdtEntries);
  }
  CopyMem (GdtTable->GdtTable, mGdtEntries, GdtLen);
  Gdtr.Base  = (UINTN) GdtTable->GdtTable;
  Gdtr.Limit = (UINT16) (GdtLen - 1);
  AsmWriteGdtr (&Gdtr);
}


/**
  Execute necessary actions after a stage is relocated.

**/
VOID
PostStageRelocation (
  VOID
  )
{
  // Update exception handler in IDT
  UpdateExceptionHandler (NULL);
}
