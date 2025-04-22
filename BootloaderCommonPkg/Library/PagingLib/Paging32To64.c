/** @file

  Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PagingLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PagingLib.h>

VOID    *mPageTablesX64;

//
// Global Descriptor Table (GDT)
//
GLOBAL_REMOVE_IF_UNREFERENCED CONST IA32_SEGMENT_DESCRIPTOR gGdtEntries[] = {
/* selector { Global Segment Descriptor                              } */
/* 0x00 */  {{0,      0,  0,  0,    0,  0,  0,  0,    0,  0, 0,  0,  0}}, //null descriptor
/* 0x08 */  {{0xffff, 0,  0,  0x2,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //linear data segment descriptor
/* 0x10 */  {{0xffff, 0,  0,  0xf,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //linear code segment descriptor
/* 0x18 */  {{0xffff, 0,  0,  0x3,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //system data segment descriptor
/* 0x20 */  {{0xffff, 0,  0,  0xa,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //system code segment descriptor
/* 0x28 */  {{0,      0,  0,  0,    0,  0,  0,  0,    0,  0, 0,  0,  0}}, //spare segment descriptor
/* 0x30 */  {{0xffff, 0,  0,  0x2,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //system data segment descriptor
/* 0x38 */  {{0xffff, 0,  0,  0xa,  1,  0,  1,  0xf,  0,  1, 0,  1,  0}}, //system code segment descriptor
/* 0x40 */  {{0,      0,  0,  0,    0,  0,  0,  0,    0,  0, 0,  0,  0}}, //spare segment descriptor
};

//
// IA32 Gdt register
//
GLOBAL_REMOVE_IF_UNREFERENCED CONST IA32_DESCRIPTOR gGdt = {
  sizeof (gGdtEntries) - 1,
  (UINTN) gGdtEntries
  };

/**
  Load page table for long mode.

**/
VOID
EFIAPI
LoadPageTableForLongMode (
  VOID
  )
{
  VOID                     *PageTables;
  UINTN                     PageTblSize;

  //
  // Load the GDT of Go64. Since the GDT of 32-bit Tiano locates in the BS_DATA
  // memory, it may be corrupted when copying FV to high-end memory
  //
  AsmWriteGdtr (&gGdt);

  //
  // Create page table and save PageMapLevel4 to CR3
  //
  if (mPageTablesX64 == NULL) {
    PageTblSize = GetPageTablesMemorySize (TRUE);
    PageTables  = AllocatePages (EFI_SIZE_TO_PAGES(PageTblSize));
    ASSERT (PageTables != NULL);
    Create4GbPageTables (PageTables, TRUE);
    DEBUG ((DEBUG_VERBOSE, "PageTables 0x%X\n", PageTables));
    mPageTablesX64 = PageTables;
  } else {
    PageTables = mPageTablesX64;
  }

  //
  // Paging might be already enabled. To avoid conflict configuration,
  // disable paging first anyway.
  //
  AsmWriteCr0 (AsmReadCr0 () & (~BIT31));
  AsmWriteCr3 ((UINTN)PageTables);
}

/**
  Create 64-bit pagetables, enable long mode and jump to 64-bit entrypoint.

  Enables the 64-bit paging mode on the CPU. CR0, CR3, CR4, and the page tables
  must be properly initialized prior to calling this service. This function
  assumes the current execution mode is 32-bit protected mode with flat
  descriptors. This function is only available on IA-32. After the 64-bit
  paging mode is enabled, control is transferred to the function specified by
  EntryPoint using the new stack specified by NewStack and passing in the
  parameters specified by Context1 and Context2. Context1 and Context2 are
  optional and may be 0. The function EntryPoint must never return.

  If the current execution mode is not 32-bit protected mode with flat
  descriptors, then ASSERT().
  If EntryPoint is 0, then ASSERT().
  If NewStack is 0, then ASSERT().

  @param[in]  EntryPoint  The 64-bit virtual address of the function to call
                          with the new stack after paging is enabled.
  @param[in]  Context1    The 64-bit virtual address of the context to pass into
                          the EntryPoint function as the first parameter after
                          paging is enabled.
  @param[in]  Context2    The 64-bit virtual address of the context to pass into
                          the EntryPoint function as the second parameter after
                          paging is enabled.
  @param[in]  NewStack    The 64-bit virtual address of the new stack to use for
                          the EntryPoint function after paging is enabled.

**/
VOID
EFIAPI
JumpToLongMode (
  IN  UINT64                EntryPoint,
  IN  UINT64                Context1,  OPTIONAL
  IN  UINT64                Context2,  OPTIONAL
  IN  UINT64                NewStack
  )
{
  ASSERT (!IsLongModeEnabled ());

  DEBUG ((DEBUG_VERBOSE, "EntryPoint 0x%lX, Context1 0x%lX, Context2 0x%lX, "
    "NewStack 0x%lX\n", EntryPoint, Context1, Context2, NewStack));

  LoadPageTableForLongMode ();

  //
  // Go to Long Mode and transfer control to EntryPoint.
  //
  AsmEnablePaging64 (
    0x38,
    EntryPoint,
    Context1,
    Context2,
    NewStack
    );
}

/**
  Whether 64-bit long mode is enabled or not.

  @return   TRUE if long mode has already been enabled, otherwise FALSE

**/
BOOLEAN
EFIAPI
IsLongModeEnabled (
  VOID
  )
{
  UINT64  Msr;

  Msr = AsmReadMsr64 (0xc0000080);
  return ((Msr & BIT8) == BIT8) ? TRUE : FALSE;
}

/**
  Whether processor supports 64-bit long mode or not.

  @return   TRUE if processor supports 64-bit long mode, otherwise FALSE

**/
BOOLEAN
EFIAPI
IsLongModeSupported (
  VOID
  )
{
  UINT32  RegEax;

  AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
  return (RegEax >= 0x80000001) ? TRUE : FALSE;
}
