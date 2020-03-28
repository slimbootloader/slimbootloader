/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PagingLib.h>

//
// Page Table Entry 4KB
//
typedef union {
  struct {
    UINT32  Present: 1;               // 0 = Not present in memory, 1 = Present in memory
    UINT32  ReadWrite: 1;             // 0 = Read-Only, 1= Read/Write
    UINT32  UserSupervisor: 1;        // 0 = Supervisor, 1=User
    UINT32  WriteThrough: 1;          // 0 = Write-Back caching, 1=Write-Through caching
    UINT32  CacheDisabled: 1;         // 0 = Cached, 1=Non-Cached
    UINT32  Accessed: 1;              // 0 = Not accessed, 1 = Accessed (set by CPU)
    UINT32  Dirty: 1;                 // 0 = Not Dirty, 1 = written by processor on access to page
    UINT32  Pat: 1;                   //
    UINT32  Global: 1;                // 0 = Not global page, 1 = global page TLB not cleared on CR3 write
    UINT32  Available: 3;             // Available for use by system software
    UINT32  PageTableBaseAddress: 20; // Page frame address
  } Bits;
  UINT32    Uint32;
} PAGE_TABLE_ENTRY_4K;

//
// Page Table Entry 4MB
//
typedef union {
  struct {
    UINT32  Present: 1;               // 0 = Not present in memory, 1 = Present in memory
    UINT32  ReadWrite: 1;             // 0 = Read-Only, 1= Read/Write
    UINT32  UserSupervisor: 1;        // 0 = Supervisor, 1=User
    UINT32  WriteThrough: 1;          // 0 = Write-Back caching, 1=Write-Through caching
    UINT32  CacheDisabled: 1;         // 0 = Cached, 1=Non-Cached
    UINT32  Accessed: 1;              // 0 = Not accessed, 1 = Accessed (set by CPU)
    UINT32  Dirty: 1;                 // 0 = Not Dirty, 1 = written by processor on access to page
    UINT32  MustBeOne: 1;             // Must Be One
    UINT32  Global: 1;                // 0 = Not global page, 1 = global page TLB not cleared on CR3 write
    UINT32  Available: 3;             // Available for use by system software
    UINT32  Pat: 1;                   //
    UINT32  Reserved: 9;              //
    UINT32  PageTableBaseAddress: 10; // Page frame address
  } Bits;
  UINT32    Uint32;
} PAGE_TABLE_ENTRY_4M;


//
// Page Directory Entry 4MB
//
typedef union {
  struct {
    UINT32  Present: 1;               // 0 = Not present in memory, 1 = Present in memory
    UINT32  ReadWrite: 1;             // 0 = Read-Only, 1= Read/Write
    UINT32  UserSupervisor: 1;        // 0 = Supervisor, 1=User
    UINT32  WriteThrough: 1;          // 0 = Write-Back caching, 1=Write-Through caching
    UINT32  CacheDisabled: 1;         // 0 = Cached, 1=Non-Cached
    UINT32  Accessed: 1;              // 0 = Not accessed, 1 = Accessed (set by CPU)
    UINT32  Reserved: 1;              // Reserved
    UINT32  MustBeZero: 1;            // Must Be Zero
    UINT32  Available: 4;             // Available for use by system software
    UINT32  PageTableBaseAddress: 20; // Page Table Base Address
  } Bits;
  UINT32    Uint32;
} PAGE_DIRECTORY_ENTRY_4MB;

/**
  Allocates and fills in the Page Directory and Page Table Entries to
  establish a 1:1 Virtual to Physical mapping.

  @param [in] Ranges
  @param [in] PageTableMem

  @retval The address of 4 level page map.

**/
UINT32
EFIAPI
Create4GbPageTablesAndRemapRange (
  IN RANGE   Ranges[1],
  IN VOID   *PageTableMem
  )
{
  UINTN                                         IndexOfPageTableEntries;
  PAGE_TABLE_ENTRY_4M                          *PageTableEntry;
  PAGE_TABLE_ENTRY_4M                          *PageTableEntry4M;
  PAGE_TABLE_ENTRY_4K                          *PageTableEntry4K;
  PAGE_DIRECTORY_ENTRY_4MB                     *PageDirectoryEntry4M;
  UINT32                                        PhysicalAddress4M;
  UINT32                                        PhysicalAddress4K;
  UINT32                                        BasePage4M;

  //
  // Define CAR region and memory mapping
  //
  BasePage4M = Ranges[0].Start & ~0x3FFFFF;
  ASSERT ((Ranges[0].Start & 0xFFF) == 0);
  ASSERT ((Ranges[0].Limit & 0xFFF) == 0xFFF);
  ASSERT ((Ranges[0].Start & ~0x3FFFFF) == BasePage4M);
  ASSERT ((Ranges[0].Limit & ~0x3FFFFF) == BasePage4M);
  ASSERT (PageTableMem != 0);

  PageTableEntry = (PAGE_TABLE_ENTRY_4M *)PageTableMem;

  //
  // Create 4MB pages from 0 to 4GB
  //
  PhysicalAddress4M = 0;
  PageTableEntry4M  = (PAGE_TABLE_ENTRY_4M *)PageTableEntry;
  for (IndexOfPageTableEntries = 0; IndexOfPageTableEntries < 1024;
       IndexOfPageTableEntries++, PageTableEntry4M++, PhysicalAddress4M += SIZE_4MB) {
    //
    // Fill in the Page Table entries
    //
    PageTableEntry4M->Uint32 = PhysicalAddress4M;
    PageTableEntry4M->Bits.ReadWrite = 1;
    PageTableEntry4M->Bits.Present = 1;
    PageTableEntry4M->Bits.MustBeOne = 1;
  }

  //
  // Split the 4MB page containing the CAR region into 4KB pages
  //
  PageTableEntry4K      = (PAGE_TABLE_ENTRY_4K *)&PageTableEntry[1024];
  PageDirectoryEntry4M  = (PAGE_DIRECTORY_ENTRY_4MB *)&PageTableEntry[BasePage4M >> 22];
  PageDirectoryEntry4M->Uint32 = (UINT32) (UINTN)PageTableEntry4K;
  PageDirectoryEntry4M->Bits.ReadWrite = 1;
  PageDirectoryEntry4M->Bits.Present = 1;

  //
  // Create 4KB pages and remap the CAR region into a different memory location
  //
  PhysicalAddress4K = BasePage4M & ~0x3FFFFF;
  for (IndexOfPageTableEntries = 0; IndexOfPageTableEntries < 1024;
       IndexOfPageTableEntries++, PageTableEntry4K++, PhysicalAddress4K += SIZE_4KB) {
    //
    // Fill in the Page Table entries
    //
    if ((PhysicalAddress4K >= Ranges[0].Start) && (PhysicalAddress4K <= Ranges[0].Limit)) {
      PageTableEntry4K->Uint32 = Ranges[0].Mapping;
      Ranges[0].Mapping       += SIZE_4KB;
    } else {
      PageTableEntry4K->Uint32 = PhysicalAddress4K;
    }
    PageTableEntry4K->Bits.ReadWrite = 1;
    PageTableEntry4K->Bits.Present   = 1;
  }

  return (UINT32) (UINTN)PageTableEntry;
}

/**
  This function returns page tables memory size (2 * SIZE_4KB).
**/
UINT32
EFIAPI
GetPageTablesMemorySize (
  VOID
  )
{
  return 2 * SIZE_4KB;
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
