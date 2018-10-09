/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "Stage1B.h"

//
// Global Descriptor Table (GDT)
//
CONST GLOBAL_REMOVE_IF_UNREFERENCED IA32_SEGMENT_DESCRIPTOR mGdtEntries[] = {
  /* selector { Global Segment Descriptor                              } */
  /* 0x00 */  {{0,      0,  0,  0,    0,  0,  0,  0,    0,  0, 0,  0,  0}}, //null descriptor
  /* 0x08 */  {{0xffff, 0,  0,  0x2,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //linear data segment descriptor
  /* 0x10 */  {{0xffff, 0,  0,  0xf,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //linear code segment descriptor
  /* 0x18 */  {{0xffff, 0,  0,  0x3,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //system data segment descriptor
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
  Remap the current stage image.

  It will create page tables to remap current Stage1B from CAR to Memory.
  It is necessary since CAR will be destroyed during CallFspTempRamExit().
**/
VOID
RemapStage (
  VOID
  )
{
  VOID                *Pdpe;
  RANGE                Ranges[2];
  UINT32               PageTblSize;
  UINT32               RoundSize;
  VOID                *GdtBase;
  IA32_DESCRIPTOR      GdtDesc;
  VOID                *IbbMemBase;

  GdtBase = AllocatePool (sizeof (mGdtEntries));
  CopyMem ((VOID *)GdtBase, mGdtEntries, sizeof (mGdtEntries));
  GdtDesc.Base  = (UINTN)GdtBase;
  GdtDesc.Limit = sizeof (mGdtEntries) - 1;
  AsmWriteGdtr (&GdtDesc);

  IbbMemBase  = AllocateTemporaryPages (EFI_SIZE_TO_PAGES (PcdGet32 (PcdStage1BFdSize)));
  PageTblSize = GetPageTablesMemorySize ();
  Pdpe        = AllocateTemporaryPages (EFI_SIZE_TO_PAGES (PageTblSize));

  DEBUG ((DEBUG_INFO, "Enable Paging ...\n"));
  CopyMem (IbbMemBase, (VOID *)PcdGet32 (PcdStage1BFdBase), PcdGet32 (PcdStage1BFdSize));
  EnableCodeExecution ();

  RoundSize         = ALIGN_UP (PcdGet32 (PcdStage1BFdSize), EFI_PAGE_SIZE);
  Ranges[0].Start   = PcdGet32 (PcdStage1BFdBase);
  Ranges[0].Limit   = Ranges[0].Start + RoundSize - 1;
  Ranges[0].Mapping = (UINT32)IbbMemBase;
  Create4GbPageTablesAndRemapRange (Ranges, Pdpe);
  EnablePaging32 (Pdpe);
}
