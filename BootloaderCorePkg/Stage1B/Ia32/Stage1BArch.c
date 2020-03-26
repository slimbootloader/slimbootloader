/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "Stage1B.h"

/**
  Load GDT table for current processor.

  It function initializes GDT table and loads it into current processor.

  @param  GdtTable  Pointer to STAGE_GDT_TABLE structure.

**/
VOID
LoadGdt (
  IN STAGE_GDT_TABLE   *GdtTable
  )
{
  IA32_DESCRIPTOR     Gdtr;
  UINTN               GdtLen;

  AsmReadGdtr (&Gdtr);
  GdtLen = sizeof(GdtTable->GdtTable);
  if (GdtLen > (UINTN)Gdtr.Limit + 1) {
    GdtLen = Gdtr.Limit + 1;
  }
  CopyMem (GdtTable->GdtTable, (VOID *)Gdtr.Base, GdtLen);

  Gdtr.Base  = (UINTN) GdtTable->GdtTable;
  Gdtr.Limit = (UINT16) (GdtLen - 1);
  AsmWriteGdtr (&Gdtr);
}

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
  IA32_DESCRIPTOR   Idtr;

  IdtTable->LdrGlobal  = Data;

  Idtr.Base  = (UINTN) &IdtTable->IdtTable;
  Idtr.Limit = (UINT16) (sizeof (IdtTable->IdtTable) - 1);
  UpdateExceptionHandler (&Idtr);
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
  VOID                *IbbMemBase;

  IbbMemBase  = AllocateTemporaryMemory (PcdGet32 (PcdStage1BFdSize));
  PageTblSize = GetPageTablesMemorySize ();
  Pdpe        = AllocateTemporaryMemory (PageTblSize);

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
