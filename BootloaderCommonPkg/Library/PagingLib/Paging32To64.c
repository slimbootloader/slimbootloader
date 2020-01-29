/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
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
  Set one page of page table pool memory to be read-only.

  @param[in] PageTableBase    Base address of page table (CR3).
  @param[in] Address          Start address of a page to be set as read-only.
  @param[in] Level4Paging     Level 4 paging flag.

**/
STATIC
VOID
SetPageTablePoolReadOnly (
  IN  UINTN                             PageTableBase,
  IN  EFI_PHYSICAL_ADDRESS              Address,
  IN  BOOLEAN                           Level4Paging
  )
{
  UINTN                 Index;
  UINTN                 EntryIndex;
  UINT64                AddressEncMask;
  EFI_PHYSICAL_ADDRESS  PhysicalAddress;
  UINT64                *PageTable;
  UINT64                *NewPageTable;
  UINT64                PageAttr;
  UINT64                LevelSize[5];
  UINT64                LevelMask[5];
  UINTN                 LevelShift[5];
  UINTN                 Level;
  UINT64                PoolUnitSize;

  ASSERT (PageTableBase != 0);

  //
  // Since the page table is always from page table pool, which is always
  // located at the boundary of PcdPageTablePoolAlignment, we just need to
  // set the whole pool unit to be read-only.
  //
  Address = Address & PAGE_TABLE_POOL_ALIGN_MASK;

  LevelShift[1] = PAGING_L1_ADDRESS_SHIFT;
  LevelShift[2] = PAGING_L2_ADDRESS_SHIFT;
  LevelShift[3] = PAGING_L3_ADDRESS_SHIFT;
  LevelShift[4] = PAGING_L4_ADDRESS_SHIFT;

  LevelMask[1] = PAGING_4K_ADDRESS_MASK_64;
  LevelMask[2] = PAGING_2M_ADDRESS_MASK_64;
  LevelMask[3] = PAGING_1G_ADDRESS_MASK_64;
  LevelMask[4] = PAGING_1G_ADDRESS_MASK_64;

  LevelSize[1] = SIZE_4KB;
  LevelSize[2] = SIZE_2MB;
  LevelSize[3] = SIZE_1GB;
  LevelSize[4] = SIZE_512GB;

  //  AddressEncMask  = PcdGet64 (PcdPteMemoryEncryptionAddressOrMask) &
  //                    PAGING_1G_ADDRESS_MASK_64;
  AddressEncMask  = 0;
  PageTable       = (UINT64 *)(UINTN)PageTableBase;
  PoolUnitSize    = PAGE_TABLE_POOL_UNIT_SIZE;

  for (Level = (Level4Paging) ? 4 : 3; Level > 0; --Level) {
    Index = ((UINTN)RShiftU64 (Address, LevelShift[Level]));
    Index &= PAGING_PAE_INDEX_MASK;

    PageAttr = PageTable[Index];
    if ((PageAttr & IA32_PG_PS) == 0) {
      //
      // Go to next level of table.
      //
      PageTable = (UINT64 *)(UINTN)(PageAttr & ~AddressEncMask &
                                    PAGING_4K_ADDRESS_MASK_64);
      continue;
    }

    if (PoolUnitSize >= LevelSize[Level]) {
      //
      // Clear R/W bit if current page granularity is not larger than pool unit
      // size.
      //
      if ((PageAttr & IA32_PG_RW) != 0) {
        while (PoolUnitSize > 0) {
          //
          // PAGE_TABLE_POOL_UNIT_SIZE and PAGE_TABLE_POOL_ALIGNMENT are fit in
          // one page (2MB). Then we don't need to update attributes for pages
          // crossing page directory. ASSERT below is for that purpose.
          //
          ASSERT (Index < EFI_PAGE_SIZE/sizeof (UINT64));

          PageTable[Index] &= ~(UINT64)IA32_PG_RW;
          PoolUnitSize    -= LevelSize[Level];

          ++Index;
        }
      }

      break;

    } else {
      //
      // The smaller granularity of page must be needed.
      //
      ASSERT (Level > 1);

      NewPageTable = AllocatePages (1);
      ASSERT (NewPageTable != NULL);
      ZeroMem (NewPageTable, EFI_PAGES_TO_SIZE (1));

      PhysicalAddress = PageAttr & LevelMask[Level];
      for (EntryIndex = 0;
            EntryIndex < EFI_PAGE_SIZE/sizeof (UINT64);
            ++EntryIndex) {
        NewPageTable[EntryIndex] = PhysicalAddress  | AddressEncMask |
                                   IA32_PG_P | IA32_PG_RW;
        if (Level > 2) {
          NewPageTable[EntryIndex] |= IA32_PG_PS;
        }
        PhysicalAddress += LevelSize[Level - 1];
      }

      PageTable[Index] = (UINT64)(UINTN)NewPageTable | AddressEncMask |
                                        IA32_PG_P | IA32_PG_RW;
      PageTable = NewPageTable;
    }
  }
}

/**
  Prevent the memory pages used for page table from been overwritten.

  @param[in] PageTableBase    Base address of page table (CR3).
  @param[in] Level4Paging     Level 4 paging flag.

**/
STATIC
VOID
EnablePageTableProtection (
  IN  UINTN     PageTableBase,
  IN  UINTN     TotalPagesNum,
  IN  BOOLEAN   Level4Paging
  )
{
  INT64                   TotalSize;
  EFI_PHYSICAL_ADDRESS    Address;

  if (PageTableBase == 0) {
    return;
  }

  //
  // Disable write protection, because we need to mark page table to be write
  // protected.
  //
  AsmWriteCr0 (AsmReadCr0() & ~CR0_WP);

  Address = (EFI_PHYSICAL_ADDRESS)PageTableBase;
  TotalSize = EFI_PAGES_TO_SIZE (TotalPagesNum);
  while (TotalSize > 0) {
    SetPageTablePoolReadOnly(PageTableBase, Address, Level4Paging);
    Address   += PAGE_TABLE_POOL_UNIT_SIZE;
    TotalSize -= PAGE_TABLE_POOL_UNIT_SIZE;
  }

  //
  // Enable write protection, after page table attribute updated.
  //
  AsmWriteCr0 (AsmReadCr0() | CR0_WP);
}

/**
  Allocates and fills in the Page Directory and Page Table Entries to
  establish a 1:1 Virtual to Physical mapping.

  @param[in] StackBase  Stack base address.
  @param[in] StackSize  Stack size.

  @return The address of 4 level page map.

**/
STATIC
UINTN
CreateIdentityMappingPageTables (
  IN EFI_PHYSICAL_ADDRESS   StackBase,
  IN UINTN                  StackSize
  )
{
  UINT32                                        RegEax;
  UINT32                                        RegEdx;
  BOOLEAN                                       Page1GSupport;
  BOOLEAN                                       Page5LevelSupport;
  UINT8                                         PhysicalAddressBits;
  UINT32                                        NumberOfPml5EntriesNeeded;
  UINT32                                        NumberOfPml4EntriesNeeded;
  UINT32                                        NumberOfPdpEntriesNeeded;
  UINTN                                         TotalPagesNum;
  UINTN                                         BigPageAddress;
  PAGE_MAP_AND_DIRECTORY_POINTER                *PageMap;
  PAGE_MAP_AND_DIRECTORY_POINTER                *PageMapLevel5Entry;
  PAGE_MAP_AND_DIRECTORY_POINTER                *PageMapLevel4Entry;
  PAGE_MAP_AND_DIRECTORY_POINTER                *PageDirectoryPointerEntry;
  PAGE_TABLE_1G_ENTRY                           *PageDirectory1GEntry;
  PAGE_TABLE_ENTRY                              *PageDirectoryEntry;
  EFI_PHYSICAL_ADDRESS                          PageAddress;
  UINTN                                         IndexOfPml5Entries;
  UINTN                                         IndexOfPml4Entries;
  UINTN                                         IndexOfPdpEntries;
  UINTN                                         IndexOfPageDirectoryEntries;
  UINT64                                        AddressEncMask;
  IA32_CR4                                      Cr4;

  //
  // Set PageMapLevel5Entry to suppress incorrect compiler/analyzer warnings
  //
  PageMapLevel5Entry = NULL;

  //
  // Make sure AddressEncMask is contained to smallest supported address field
  //
  // AddressEncMask = PcdGet64 (PcdPteMemoryEncryptionAddressOrMask)
  //                  & PAGING_1G_ADDRESS_MASK_64;
  AddressEncMask = 0;

  Page1GSupport = FALSE;
  AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
  if (RegEax >= 0x80000001) {
    AsmCpuid (0x80000001, NULL, NULL, NULL, &RegEdx);
    if ((RegEdx & BIT26) != 0) {
      Page1GSupport = TRUE;
    }
  }
  if (RegEax >= 0x80000008) {
    AsmCpuid (0x80000008, &RegEax, NULL, NULL, NULL);
    PhysicalAddressBits = (UINT8) RegEax;
  } else {
    PhysicalAddressBits = 36;
  }

  //
  // TBD: Page5LevelSupport = FALSE for now, but keep the supporting code
  //
  Page5LevelSupport = FALSE;

  DEBUG ((DEBUG_VERBOSE, "AddressBits=%u 5LevelPaging=%u 1GPage=%u\n",
    PhysicalAddressBits, Page5LevelSupport, Page1GSupport));

  if (!Page5LevelSupport && PhysicalAddressBits > 48) {
    PhysicalAddressBits = 48;
  }

  //
  // Calculate the table entries needed.
  //
  NumberOfPml5EntriesNeeded = 1;
  NumberOfPml4EntriesNeeded = 1;
  if (PhysicalAddressBits > 39) {
    NumberOfPml4EntriesNeeded = (UINT32) LShiftU64 (1, PhysicalAddressBits - 39);
    PhysicalAddressBits = 39;
  }
  NumberOfPdpEntriesNeeded = 1;
  ASSERT (PhysicalAddressBits > 30);
  NumberOfPdpEntriesNeeded = (UINT32) LShiftU64 (1, PhysicalAddressBits - 30);

  //
  // Pre-allocate big pages to avoid later allocations.
  //
  if (!Page1GSupport) {
    TotalPagesNum = ((NumberOfPdpEntriesNeeded + 1) * NumberOfPml4EntriesNeeded + 1)
                     * NumberOfPml5EntriesNeeded + 1;
  } else {
    TotalPagesNum = (NumberOfPml4EntriesNeeded + 1) * NumberOfPml5EntriesNeeded + 1;
  }

  //
  // Substract the one page occupied by PML5 entries if 5-Level Paging is disabled.
  //
  if (!Page5LevelSupport) {
    TotalPagesNum--;
  }

  DEBUG ((DEBUG_VERBOSE, "Pml5=%u Pml4=%u Pdp=%u TotalPage=%Lu\n",
    NumberOfPml5EntriesNeeded, NumberOfPml4EntriesNeeded,
    NumberOfPdpEntriesNeeded, (UINT64)TotalPagesNum));

  BigPageAddress = (UINTN) AllocatePages (TotalPagesNum);
  ASSERT (BigPageAddress != 0);
  ZeroMem ((VOID *)BigPageAddress, EFI_PAGES_TO_SIZE (TotalPagesNum));
  DEBUG ((DEBUG_VERBOSE, "PageMap 0x%X\n", BigPageAddress));

  //
  // By architecture only one PageMapLevel4 exists - so lets allocate storage for it.
  //
  PageMap         = (VOID *) BigPageAddress;
  if (Page5LevelSupport) {
    //
    // By architecture only one PageMapLevel5 exists - so lets allocate storage for it.
    //
    PageMapLevel5Entry = PageMap;
    BigPageAddress    += SIZE_4KB;
  }
  PageAddress        = 0;

  for ( IndexOfPml5Entries = 0
      ; IndexOfPml5Entries < NumberOfPml5EntriesNeeded
      ; IndexOfPml5Entries++) {
    //
    // Each PML5 entry points to a page of PML4 entires.
    // So lets allocate space for them and fill them in in the IndexOfPml4Entries loop.
    // When 5-Level Paging is disabled, below allocation happens only once.
    //
    PageMapLevel4Entry = (VOID *) BigPageAddress;
    BigPageAddress    += SIZE_4KB;

    if (Page5LevelSupport) {
      //
      // Make a PML5 Entry
      //
      PageMapLevel5Entry->Uint64 = (UINT64) (UINTN) PageMapLevel4Entry | AddressEncMask;
      PageMapLevel5Entry->Bits.ReadWrite = 1;
      PageMapLevel5Entry->Bits.Present   = 1;
      PageMapLevel5Entry++;
    }

    for ( IndexOfPml4Entries = 0
        ; IndexOfPml4Entries < (NumberOfPml5EntriesNeeded == 1 ? NumberOfPml4EntriesNeeded : 512)
        ; IndexOfPml4Entries++, PageMapLevel4Entry++) {
      //
      // Each PML4 entry points to a page of Page Directory Pointer entires.
      // So lets allocate space for them and fill them in in the IndexOfPdpEntries loop.
      //
      PageDirectoryPointerEntry = (VOID *) BigPageAddress;
      BigPageAddress += SIZE_4KB;

      //
      // Make a PML4 Entry
      //
      PageMapLevel4Entry->Uint64 = (UINT64)(UINTN)PageDirectoryPointerEntry | AddressEncMask;
      PageMapLevel4Entry->Bits.ReadWrite = 1;
      PageMapLevel4Entry->Bits.Present = 1;

      if (Page1GSupport) {
        PageDirectory1GEntry = (VOID *) PageDirectoryPointerEntry;

        for (IndexOfPageDirectoryEntries = 0;
             IndexOfPageDirectoryEntries < 512;
             IndexOfPageDirectoryEntries++, PageDirectory1GEntry++, PageAddress += SIZE_1GB) {
          //
          // Fill in the Page Directory entries
          //
          PageDirectory1GEntry->Uint64 = (UINT64)PageAddress | AddressEncMask;
          PageDirectory1GEntry->Bits.ReadWrite = 1;
          PageDirectory1GEntry->Bits.Present = 1;
          PageDirectory1GEntry->Bits.MustBe1 = 1;
        }
      } else {
        for ( IndexOfPdpEntries = 0
            ; IndexOfPdpEntries < (NumberOfPml4EntriesNeeded == 1 ? NumberOfPdpEntriesNeeded : 512)
            ; IndexOfPdpEntries++, PageDirectoryPointerEntry++) {
          //
          // Each Directory Pointer entries points to a page of Page Directory entires.
          // So allocate space for them and fill them in in the IndexOfPageDirectoryEntries loop.
          //
          PageDirectoryEntry = (VOID *) BigPageAddress;
          BigPageAddress += SIZE_4KB;

          //
          // Fill in a Page Directory Pointer Entries
          //
          PageDirectoryPointerEntry->Uint64 = (UINT64)(UINTN)PageDirectoryEntry | AddressEncMask;
          PageDirectoryPointerEntry->Bits.ReadWrite = 1;
          PageDirectoryPointerEntry->Bits.Present = 1;

          for (IndexOfPageDirectoryEntries = 0;
               IndexOfPageDirectoryEntries < 512;
               IndexOfPageDirectoryEntries++, PageDirectoryEntry++, PageAddress += SIZE_2MB) {
            //
            // Fill in the Page Directory entries
            //
            PageDirectoryEntry->Uint64 = (UINT64)PageAddress | AddressEncMask;
            PageDirectoryEntry->Bits.ReadWrite = 1;
            PageDirectoryEntry->Bits.Present = 1;
            PageDirectoryEntry->Bits.MustBe1 = 1;
          }
        }

        //
        // Fill with null entry for unused PDPTE
        //
        ZeroMem (PageDirectoryPointerEntry, (512 - IndexOfPdpEntries) * sizeof(PAGE_MAP_AND_DIRECTORY_POINTER));
      }
    }

    //
    // For the PML4 entries we are not using fill in a null entry.
    //
    ZeroMem (PageMapLevel4Entry, (512 - IndexOfPml4Entries) * sizeof (PAGE_MAP_AND_DIRECTORY_POINTER));
  }

  if (Page5LevelSupport) {
    Cr4.UintN = AsmReadCr4 () | BIT12;
    AsmWriteCr4 (Cr4.UintN);
    //
    // For the PML5 entries we are not using fill in a null entry.
    //
    ZeroMem (PageMapLevel5Entry, (512 - IndexOfPml5Entries) * sizeof (PAGE_MAP_AND_DIRECTORY_POINTER));
  }

  //
  // Protect the page table by marking the memory used for page table to be
  // read-only.
  //
  EnablePageTableProtection ((UINTN)PageMap, TotalPagesNum, TRUE);

  return (UINTN)PageMap;
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
  UINTN                     PageTables;

  ASSERT (!IsLongModeEnabled ());

  DEBUG ((DEBUG_VERBOSE, "EntryPoint 0x%lX, Context1 0x%lX, Context2 0x%lX, "
    "NewStack 0x%lX\n", EntryPoint, Context1, Context2, NewStack));

  //
  // Load the GDT of Go64. Since the GDT of 32-bit Tiano locates in the BS_DATA
  // memory, it may be corrupted when copying FV to high-end memory
  //
  AsmWriteGdtr (&gGdt);

  //
  // Create page table and save PageMapLevel4 to CR3
  //
  PageTables = CreateIdentityMappingPageTables (0, 0);
  DEBUG ((DEBUG_VERBOSE, "PageTables 0x%X\n", PageTables));

  //
  // Paging might be already enabled. To avoid conflict configuration,
  // disable paging first anyway.
  //
  AsmWriteCr0 (AsmReadCr0 () & (~BIT31));
  AsmWriteCr3 (PageTables);

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
  return ((Msr & BIT0) == BIT0) ? TRUE : FALSE;
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
