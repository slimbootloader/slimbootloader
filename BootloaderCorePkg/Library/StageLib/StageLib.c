/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/CpuExceptionLib.h>
#include <Library/PagingLib.h>

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
  IN UINTN              Data
  )
{
  IA32_DESCRIPTOR             Idtr;

  IdtTable->LdrGlobal  = Data;

  Idtr.Base  = (UINTN) &IdtTable->IdtTable;
  Idtr.Limit = (UINT16) (sizeof (IdtTable->IdtTable) - 1);
  UpdateExceptionHandler (&Idtr);
}

/**
  Copy GDT to memory and Load GDT table for current processor.

  It function initializes GDT table and loads it into current processor.

  @param[in]  GdtTable  Pointer to STAGE_GDT_TABLE structure to fill the GDT.
  @param[in]  GdtrPtr   Pointer to the source IA32_DESCRIPTOR structure.

**/
VOID
LoadGdt (
  IN STAGE_GDT_TABLE   *GdtTable,
  IN IA32_DESCRIPTOR   *GdtrPtr
  )
{
  IA32_DESCRIPTOR     Gdtr;
  UINT32              GdtLen;

  if (GdtrPtr == NULL) {
    AsmReadGdtr (&Gdtr);
    GdtrPtr = &Gdtr;
  }

  GdtLen = sizeof(GdtTable->GdtTable);
  if (GdtLen > (UINTN)GdtrPtr->Limit + 1) {
    GdtLen = GdtrPtr->Limit + 1;
  }
  CopyMem (GdtTable->GdtTable, (VOID *)GdtrPtr->Base, GdtLen);

  Gdtr.Base  = (UINTN) GdtTable->GdtTable;
  Gdtr.Limit = (UINT16) (GdtLen - 1);
  AsmWriteGdtr (&Gdtr);
}

/**
  Remap the current stage image.

  It will create page tables to remap current Stage1B from CAR to Memory.
  It is necessary since CAR will be destroyed during CallFspTempRamExit().
**/
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
  VOID                *PageBuffer;
  MAP_RANGE            Ranges[1];
  UINT32               PageTblSize;
  UINT32               RoundSize;
  VOID                *Stage1bBase;
  EFI_STATUS           Status;
  BOOLEAN              Is64Bit;
  UINTN                Cr0;

  Is64Bit = IS_X64;
  if (!Is64Bit && FeaturePcdGet (PcdStage1BXip)) {
    return;
  }

  PageTblSize = GetPageTablesMemorySize (Is64Bit);
  if (Is64Bit) {
    // 64bit paging table needs to be perserved across all stages
    PageBuffer  = AllocatePages (EFI_SIZE_TO_PAGES(PageTblSize));
  } else {
    // 32bit paging table is just for temporary use
    PageBuffer  = AllocateTemporaryMemory (PageTblSize + EFI_PAGE_SIZE);
    PageBuffer  = (VOID *)ALIGN_UP((UINTN)PageBuffer, EFI_PAGE_SIZE);
  }
  DEBUG ((DEBUG_INFO, "Load page table from memory @ 0x%08X\n", (UINT32)(UINTN)PageBuffer));
  Status = Create4GbPageTables (PageBuffer, Is64Bit);

  if (!EFI_ERROR(Status) && !FeaturePcdGet (PcdStage1BXip)) {
    Stage1bBase = AllocateTemporaryMemory (PcdGet32 (PcdStage1BFdSize));
    DEBUG ((DEBUG_INFO, "Remapping Stage to 0x%08X\n", Stage1bBase));
    CopyMem (Stage1bBase, (VOID *)(UINTN)PcdGet32 (PcdStage1BFdBase), PcdGet32 (PcdStage1BFdSize));
    RoundSize         = ALIGN_UP (PcdGet32 (PcdStage1BFdSize), EFI_PAGE_SIZE);
    Ranges[0].Start   = PcdGet32 (PcdStage1BFdBase);
    Ranges[0].Limit   = Ranges[0].Start + RoundSize - 1;
    Ranges[0].Mapping = (UINT32)(UINTN)Stage1bBase;
    Status = MapMemoryRange (Ranges, PageBuffer);
  }
  ASSERT_EFI_ERROR (Status);

  Cr0 = AsmReadCr0 ();
  if (Cr0 & BIT31) {
    // Alreay in paging mode
    AsmWriteCr3 ((UINTN)PageBuffer);
  } else {
    // Enable paging
    AsmWriteCr4 (AsmReadCr4() | BIT4);
    AsmWriteCr3 ((UINTN)PageBuffer);
    AsmWriteCr0 (Cr0 | BIT31);
  }
}

/**
  Unmap the previous mapped stage images.

  It will reload GDT and disable paging so that the memory goes back to 1:1 mapping.

**/
VOID
UnmapStage (
  VOID
  )
{
  MAP_RANGE            Ranges[1];
  VOID                *PageBuffer;
  UINT32               RoundSize;
  EFI_STATUS           Status;

  // Reload Exception handler
  UpdateExceptionHandler (NULL);

  if (!FeaturePcdGet (PcdStage1BXip)) {
    DEBUG ((DEBUG_INFO, "Unmapping Stage\n"));
    if (IS_X64) {
      // 64 bit needs to remove the remapped 4KB tables
      PageBuffer = (VOID *)AsmReadCr3 ();
      RoundSize         = ALIGN_UP (PcdGet32 (PcdStage1BFdSize), EFI_PAGE_SIZE);
      Ranges[0].Start   = PcdGet32 (PcdStage1BFdBase);
      Ranges[0].Limit   = Ranges[0].Start + RoundSize - 1;
      Ranges[0].Mapping = 0;
      Status = UnmapMemoryRange (Ranges, PageBuffer);
      ASSERT_EFI_ERROR (Status);
      AsmWriteCr3 ((UINTN)PageBuffer);
    } else {
      // 32 bit needs to disable paging
      if (AsmReadCr0 () & BIT31) {
        AsmWriteCr0 (AsmReadCr0() & ~BIT31);
      }
    }
  }
}
