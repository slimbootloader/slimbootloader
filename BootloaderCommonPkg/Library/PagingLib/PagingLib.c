/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PagingLib.h>

/**
  This function returns page tables memory size.

  @param[in] IsX64Mode     Determine to build page table for x64 mode or not.

  @retval   Page table memory size required.

**/
UINT32
EFIAPI
GetPageTablesMemorySize (
  IN BOOLEAN     IsX64Mode
  )
{
  if (IsX64Mode) {
    return 8 * SIZE_4KB;
  } else {
    return 2 * SIZE_4KB;
  }
}

/**
  Allocates and fills in the Page Directory and Page Table Entries to
  establish a 1:1 Virtual to Physical mapping.

  @param[in] PageBuffer    Page table root pointer. The buffer size needs
                           to be at least GetPageTablesMemorySize().
  @param[in] IsX64Mode     Determine to build page table for x64 mode or not.

  @retval    EFI_SUCCESS            Page table was created successfully.
  @retval    EFI_INVALID_PARAMETER  Invalid PageBuffer.

**/

EFI_STATUS
EFIAPI
Create4GbPageTables (
  IN VOID       *PageBuffer,
  IN BOOLEAN     IsX64Mode
  )
{
  UINT32         PageLen;
  UINT32         Attribute;
  UINT32        *Page32;
  UINT64        *Page64;
  UINTN          Idx;
  UINT32         Address;

  if (PageBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PageLen = GetPageTablesMemorySize (IsX64Mode);
  ZeroMem (PageBuffer, PageLen);

  Address   = 0;
  Attribute = IA32_PG_P | IA32_PG_RW | IA32_PG_AC;
  if (IsX64Mode) {
    // PML4
    Page64    = (UINT64 *)PageBuffer;
    Page64[0] = (UINTN)Page64 + EFI_PAGE_SIZE * 1 + Attribute;

    // PDP 1GB
    Page64    = (UINT64 *)((UINTN)PageBuffer + EFI_PAGE_SIZE * 1);
    Page64[0] = (UINTN)Page64 + EFI_PAGE_SIZE * 1 + Attribute;
    Page64[1] = (UINTN)Page64 + EFI_PAGE_SIZE * 2 + Attribute;
    Page64[2] = (UINTN)Page64 + EFI_PAGE_SIZE * 3 + Attribute;
    Page64[3] = (UINTN)Page64 + EFI_PAGE_SIZE * 4 + Attribute;

    // PDE 2MB
    Page64 = (UINT64 *)((UINTN)PageBuffer + EFI_PAGE_SIZE * 2);
    for (Idx = 0; Idx < 2048; Idx++) {
      Page64[Idx] = Address + (Attribute | IA32_PG_PD);
      Address += 0x200000;
    }
  } else {
    // PDE 4MB
    Page32 = (UINT32 *)((UINTN)PageBuffer);
    for (Idx = 0; Idx < 1024; Idx++) {
      Page32[Idx] = Address + (Attribute | IA32_PG_PD);
      Address += 0x400000;
    }
  }

  return EFI_SUCCESS;
}


/**
  This function create and load 4GB identical mapping paging table.
**/
VOID
EFIAPI
LoadPageTable (
  VOID
  )
{
  VOID     *Buffer;
  UINT64   *PageTable;
  UINT64    Base;
  UINTN     Idx;

  Buffer    = AllocatePages (8);
  ZeroMem (Buffer, 2 * EFI_PAGE_SIZE);

  PageTable    = (UINT64 *)Buffer;
  PageTable[0] = (UINTN)PageTable + EFI_PAGE_SIZE * 1 + 0x23;

  PageTable    = (UINT64 *)((UINTN)Buffer + EFI_PAGE_SIZE * 1);
  PageTable[0] = (UINTN)PageTable + EFI_PAGE_SIZE * 1 + 0x23;
  PageTable[1] = (UINTN)PageTable + EFI_PAGE_SIZE * 2 + 0x23;
  PageTable[2] = (UINTN)PageTable + EFI_PAGE_SIZE * 3 + 0x23;
  PageTable[3] = (UINTN)PageTable + EFI_PAGE_SIZE * 4 + 0x23;

  Base = 0;
  PageTable    = (UINT64 *)((UINTN)Buffer + EFI_PAGE_SIZE * 2);
  for (Idx = 0; Idx < 2048; Idx++) {
    PageTable[Idx] = Base + 0xE3;
    Base += SIZE_2MB;
  }
  AsmWriteCr3 ((UINTN)Buffer);
}
