/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PagingLib.h>
#include <Library/BootloaderCommonLib.h>

#define IS_PD_SET     (((Address & 0xFFF) & IA32_PG_PD) == IA32_PG_PD)
#define PD_SET_ADDR   (Address & ~(0xFFFF))
#define PD_UNSET_ADDR (Address & ~(0xFFF))
#define MIN_ADDR_BITS 36

/**
  The function will check if 5-level paging is needed

  @retval TRUE  5-level paging enabling is needed.
  @retval FALSE 5-level paging enabling is not needed.

**/
STATIC
BOOLEAN
Is5LevelPagingNeeded (
  VOID
  )
{
  //
  // Unsupported until a specific usecase requires
  //
  return FALSE;
}

/**
  The function will check if 1G page is supported.

  @retval TRUE   1G page is supported.
  @retval FALSE  1G page is not supported.

**/
STATIC
BOOLEAN
IsPage1GSupport (
  VOID
  )
{
  UINT32            RegEax;
  UINT32            RegEdx;
  BOOLEAN           Page1GSupport;

  Page1GSupport = FALSE;
  AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
  if (RegEax >= 0x80000001) {
    AsmCpuid (0x80000001, NULL, NULL, NULL, &RegEdx);
    if ((RegEdx & BIT26) != 0) {
      Page1GSupport = TRUE;
    }
  }

  return Page1GSupport;
}

/**
  Get physical address bits.

  @return Physical address bits.

**/
UINT8
EFIAPI
GetPhysicalAddressBits (
  VOID
  )
{
  UINT8             PhysicalAddressBits;
  UINT32            RegEax;

  //
  // Default 4GB only
  //
  PhysicalAddressBits = MIN_ADDR_BITS;
  AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
  if (RegEax >= 0x80000008) {
    AsmCpuid (0x80000008, &RegEax, NULL, NULL, NULL);
    PhysicalAddressBits = (UINT8) RegEax;
  }

  return PhysicalAddressBits;
}

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
  This function dumps the current PageTables

  @param None

  @retval None
**/

VOID
EFIAPI
DumpPageTables (
  VOID
  )
{
  BOOLEAN       Is64Bit;
  UINT64        *PagePml4;
  UINT64        *PagePdp3;
  UINT64        *PagePde2;
  UINT64        *PagePte1;
  UINT32        *Page32Pde2;
  UINT32        *Page32Pte1;
  UINT32        L4Idx;
  UINT32        L3Idx;
  UINT32        L2Idx;
  UINT32        L1Idx;
  UINTN         EntryNum;
  UINTN         Address;
  BOOLEAN       IsPdSet;

  Is64Bit = IS_X64;
  IsPdSet = FALSE;

  if (Is64Bit) {
    EntryNum    = 512;
    PagePml4    = (UINT64 *) AsmReadCr3 ();
    for (L4Idx  = 0; L4Idx < EntryNum; L4Idx++) {
      Address   = (UINTN) PagePml4[L4Idx];
      if (Address != 0) {
        DEBUG ((DEBUG_INFO, "L4[%03d]=0x%016lX\t\t\t\t(@ 0x%08X)\n", L4Idx, Address, &PagePml4[L4Idx] ));
      } else {
        continue;
      }
      IsPdSet   = IS_PD_SET;
      Address   = IsPdSet ? PD_SET_ADDR : PD_UNSET_ADDR;
      if (!IsPdSet) {
        PagePdp3   = (UINT64 *) Address;
        for (L3Idx = 0; L3Idx < EntryNum; L3Idx++) {
          Address  = (UINTN) PagePdp3[L3Idx];
          if (Address != 0) {
            DEBUG ((DEBUG_INFO, "\tL3[%03d]=0x%016lX\t\t\t(@ 0x%08X)\n", L3Idx, Address, &PagePdp3[L3Idx]));
          } else {
            continue;
          }
          IsPdSet  = IS_PD_SET;
          Address  = IsPdSet ? PD_SET_ADDR : PD_UNSET_ADDR;
          if (!IsPdSet) {
            PagePde2   = (UINT64 *) Address;
            for (L2Idx = 0; L2Idx < EntryNum; L2Idx++) {
              Address  = (UINTN) PagePde2[L2Idx];
              if (Address != 0) {
                DEBUG ((DEBUG_INFO, "\t\tL2[%03d]=0x%016lX\t\t(@ 0x%08X)\n", L2Idx, Address, &PagePde2[L2Idx]));
              } else {
                continue;
              }
              IsPdSet  = IS_PD_SET;
              Address  = IsPdSet ? PD_SET_ADDR : PD_UNSET_ADDR;
              if (!IsPdSet ) {
                PagePte1   = (UINT64 *) Address;
                for (L1Idx = 0; L1Idx < EntryNum; L1Idx++) {
                  Address  = (UINTN) PagePte1[L1Idx];
                  if (Address != 0) {
                    DEBUG ((DEBUG_INFO, "\t\t\tL1[%03d]=0x%016lX\t(@ 0x%08X)\n", L1Idx, Address, &PagePte1[L1Idx]));
                  } else {
                    continue;
                  }
                }
              }
            }
          }
        }
      }
    }
  } else {
    EntryNum    = 1024;
    Page32Pde2  = (UINT32 *) AsmReadCr3 ();
    for (L2Idx  = 0; L2Idx < EntryNum; L2Idx++) {
      Address   = (UINTN) Page32Pde2[L2Idx];
      if (Address != 0) {
        DEBUG ((DEBUG_INFO, "L2[%03d]=0x%016lX\t\t\t(@ 0x%08X)\n", L2Idx, Address, &Page32Pde2[L2Idx]));
      } else {
        continue;
      }
      IsPdSet   = IS_PD_SET;
      Address   = IsPdSet ? PD_SET_ADDR : PD_UNSET_ADDR;
      if (!IsPdSet) {
        Page32Pte1 = (UINT32 *) Address;
        for (L1Idx = 0; L1Idx < EntryNum; L1Idx++) {
          Address  = (UINTN) Page32Pte1[L1Idx];
          if (Address != 0) {
            DEBUG ((DEBUG_INFO, "\tL1[%03d]=0x%016lX\t\t(@ 0x%08X)\n", L1Idx, Address, &Page32Pte1[L1Idx]));
          } else {
            continue;
          }
        }
      }
    }
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
  Allocates and fills in the Page Directory and Page Table Entries to
  establish a 1:1 Virtual to Physical mapping.

  @param[in] RequestedAddressBits   If RequestedAddressBits is in valid range
                                    (MIN_ADDR_BITS < RequestedAddressBits < PhysicalAddressBits),
                                    paging table will cover the requested physical address range only.
                                    When RequestedAddressBits is 0, it will build the address range
                                    that the CPU can support.

  @retval    EFI_SUCCESS            Page table was created successfully.
  @retval    EFI_OUT_OF_RESOURCES   Failed to allocate page buffer

**/
EFI_STATUS
EFIAPI
CreateIdentityMappingPageTables (
  IN  UINT8         RequestedAddressBits
  )
{
  VOID             *PageBuffer;
  BOOLEAN           Page1GSupport;
  BOOLEAN           Page5LevelSupport;
  UINT8             PhysicalAddressBits;
  UINTN             TotalPagesNum;
  UINT32            NumOfPml5Entries;
  UINT32            NumOfPml4Entries;
  UINT32            NumOfPdpEntries;
  UINT64           *Page64;
  UINTN             Idx;
  UINTN             Entries;
  UINTN             Address;
  UINT32            Attribute;
  UINTN             Cr0;

  PhysicalAddressBits = GetPhysicalAddressBits ();
  if (RequestedAddressBits == 0) {
    RequestedAddressBits = PhysicalAddressBits;
  }

  Page1GSupport       = IsPage1GSupport ();
  Page5LevelSupport   = Is5LevelPagingNeeded ();
  DEBUG ((DEBUG_INFO, "RequestedAddressBits=%u PhysicalAddressBits=%u 5LevelPaging=%u 1GPage=%u\n",
    RequestedAddressBits, PhysicalAddressBits, Page5LevelSupport, Page1GSupport));

  ASSERT (PhysicalAddressBits <= 52);
  if (RequestedAddressBits < MIN_ADDR_BITS) {
    RequestedAddressBits = MIN_ADDR_BITS;
  }
  if (PhysicalAddressBits > RequestedAddressBits) {
    PhysicalAddressBits = RequestedAddressBits;
  }
  if (!Page5LevelSupport && (PhysicalAddressBits > 48)) {
    PhysicalAddressBits = 48;
  }

  NumOfPml5Entries = 1;
  if (PhysicalAddressBits > 48) {
    NumOfPml5Entries = (UINT32) LShiftU64 (1, PhysicalAddressBits - 48);
    PhysicalAddressBits = 48;
  }

  NumOfPml4Entries = 1;
  if (PhysicalAddressBits > 39) {
    NumOfPml4Entries = (UINT32) LShiftU64 (1, PhysicalAddressBits - 39);
    PhysicalAddressBits = 39;
  }

  NumOfPdpEntries = 1;
  ASSERT (PhysicalAddressBits > 30);
  NumOfPdpEntries = (UINT32) LShiftU64 (1, PhysicalAddressBits - 30);

  if (!Page1GSupport) {
    TotalPagesNum = ((UINTN)(NumOfPdpEntries + 1) * NumOfPml4Entries + 1) * NumOfPml5Entries + 1;
  } else {
    TotalPagesNum = (UINTN)(NumOfPml4Entries + 1) * NumOfPml5Entries + 1;
  }

  if (!Page5LevelSupport) {
    TotalPagesNum--;
  }

  DEBUG ((DEBUG_INFO, "Pml5=%u Pml4=%u Pdp=%u TotalPage=%Lu\n",
    NumOfPml5Entries, NumOfPml4Entries, NumOfPdpEntries, (UINT64)TotalPagesNum));

  PageBuffer = AllocatePages (TotalPagesNum);
  if (PageBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (PageBuffer, EFI_PAGES_TO_SIZE (TotalPagesNum));

  Address   = 0;
  Attribute = IA32_PG_P | IA32_PG_RW;
  Page64 = (UINT64 *)PageBuffer;

  // PML5
  if (Page5LevelSupport) {
    for (Idx = 0; Idx < NumOfPml5Entries; Idx++) {
      Page64[Idx] = (UINT64)(UINTN)Page64 + SIZE_4KB * (Idx + 1) + Attribute;
    }
    // PML4 Entry
    Page64 = (UINT64 *)((UINTN)Page64 + SIZE_4KB);
  }

  // PML4
  Entries = (NumOfPml5Entries == 1) ? NumOfPml4Entries : 512;
  for (Idx = 0; Idx < Entries; Idx++) {
    Page64[Idx] = (UINT64)(UINTN)Page64 + SIZE_4KB * (Idx + 1) + Attribute;
  }

  Page64 = (UINT64 *)((UINTN)Page64 + SIZE_4KB);
  if (Page1GSupport) {
    // PDE
    Entries *= 512;
    for (Idx = 0; Idx < Entries; Idx++, Address += SIZE_1GB) {
      Page64[Idx] = Address + (Attribute | IA32_PG_PD);
    }
  } else {
    // PDP
    Entries *= (NumOfPml4Entries == 1 ? NumOfPdpEntries : 512);
    for (Idx = 0; Idx < Entries; Idx++) {
      Page64[Idx] = (UINT64)(UINTN)Page64 + (SIZE_4KB * (Idx + 1)) + Attribute;
    }
    Page64 = (UINT64 *)((UINTN)Page64 + SIZE_4KB);

    // PDE
    Entries *= 512;
    for (Idx = 0; Idx < Entries; Idx++, Address += SIZE_2MB) {
      Page64[Idx] = Address + (Attribute | IA32_PG_PD);
    }
  }

  Cr0 = AsmReadCr0 ();
  // Set PAE
  AsmWriteCr4 (AsmReadCr4() | BIT5);
  AsmWriteCr3 ((UINTN)PageBuffer);
  if ((Cr0 & BIT31) != BIT31) {
    AsmWriteCr0 (Cr0 | BIT31);
  }

  return EFI_SUCCESS;
}
