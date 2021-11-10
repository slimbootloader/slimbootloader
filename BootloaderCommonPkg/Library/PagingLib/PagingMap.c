/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PagingLib.h>

#define PDE_PG_LVL  2
#define PDP_PG_LVL  3

/**
  Get shift bit number from address to page index

  @param[in] Level    Page level.

  @retval             Bit to shift to get specific page level index.

**/
UINT32
GetPageShiftBits (
  IN  UINT8   Level
)
{
  if (Level == PDE_PG_LVL) {
    return IsLongModeEnabled() ? 21 : 22;
  } else {
    return IsLongModeEnabled() ? 30 : 31;
  }
}

/**
  Map a memory range from the current paging table.

  @param[in] Ranges        Memory range info to map.
  @param[in] PageBuffer    Page table root pointer.

  @retval    EFI_SUCCESS            Map completed successfully.
  @retval    EFI_INVALID_PARAMETER  Invalid PageBuffer or Ranges.

**/
EFI_STATUS
EFIAPI
MapMemoryRange (
  IN MAP_RANGE   Ranges[1],
  IN VOID       *PageBuffer
  )
{
  UINTN          Idx;
  UINTN          End;
  UINTN          EntryNum;
  UINTN         *PageTable1G;
  UINTN         *PageTable2M;
  UINTN         *PageTable4K;
  UINTN          PageBase;
  UINT32         Attribute;
  UINTN          Address;
  UINT32         PteOffset;
  UINT32         PdeOffset;
  UINT32         PdpOffset;
  UINTN          Alignment;
  UINT32         PageBits;

  Attribute = IA32_PG_P | IA32_PG_RW | IA32_PG_AC;

  Alignment = Ranges[0].PageSize - 1;
  // Check for Start and Limit PageSize alignment
  if (((Ranges[0].Start & Alignment) != 0) || ((Ranges[0].Limit & Alignment) != Alignment)) {
    return EFI_INVALID_PARAMETER;
  }
  // Check for Mapping PageSize alignment
  if ( (Ranges[0].Mapping & Alignment) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  switch (Ranges[0].PageSize) {
  case SIZE_4KB:
    PageBits  = GetPageShiftBits (PDE_PG_LVL);
    if (PageBits == 21) {
      // 64 bit mode
      PteOffset = 6 * EFI_PAGE_SIZE;
      PdeOffset = 2 * EFI_PAGE_SIZE;
      EntryNum  = 512;
    } else {
      // 32 bit mode
      PteOffset = 1 * EFI_PAGE_SIZE;
      PdeOffset = 0;
      EntryNum  = 1024;
    }
    PageTable4K = (UINTN *)((UINTN)PageBuffer + PteOffset);
    PageTable2M = (UINTN *)((UINTN)PageBuffer + PdeOffset);

    Alignment = (UINTN) LShiftU64 (1, PageBits) - 1;
    PageBase  = Ranges[0].Start & ~Alignment;

    // Check if the limit is in the same page
    if ((Ranges[0].Limit & ~Alignment) != PageBase) {
      return EFI_INVALID_PARAMETER;
    }

    Address = PageBase;
    // Create 4KB pages and remap the CAR region into a different memory location
    for (Idx = 0; Idx < EntryNum; Idx++) {
      if ((Address >= Ranges[0].Start) && (Address <= Ranges[0].Limit)) {
        PageTable4K[Idx]   = Ranges[0].Mapping + (Attribute | IA32_PG_PD);
        Ranges[0].Mapping += SIZE_4KB;
      } else {
        PageTable4K[Idx]   = Address + (Attribute | IA32_PG_PD);
      }
      Address += SIZE_4KB;
    }

    // Split the 2MB page containing the CAR region into 4KB pages
    Idx = (UINT32) RShiftU64 (PageBase, PageBits);
    PageTable2M[Idx] = (UINTN)PageTable4K + Attribute;
    break;
  case SIZE_1GB:
    ASSERT (Ranges[0].Limit < SIZE_512GB);
    PageBits    = GetPageShiftBits (PDP_PG_LVL);
    PdpOffset   = 1 * EFI_PAGE_SIZE;
    EntryNum    = 512;
    PageTable1G = (UINTN *)((UINTN)PageBuffer + PdpOffset);

    Alignment = (UINTN) LShiftU64 (1, PageBits) - 1;
    PageBase  = Ranges[0].Start & ~Alignment;

    // Get the start and ending index into 1G table and add 1G entries
    Address = Ranges[0].Mapping;
    Idx = (UINT32) RShiftU64 (PageBase, PageBits);
    End = (UINT32) RShiftU64 (Ranges[0].Limit + 1, PageBits);
    for (; Idx < End; Idx++) {
      PageTable1G[Idx] = Address + (Attribute | IA32_PG_PD);
      Address += SIZE_1GB;
    }
    break;
  default:
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Unmap a memory range from the current paging table.

  @param[in] Ranges        Memory range info to un map.
  @param[in] PageBuffer    Page table root pointer.

  @retval    EFI_SUCCESS            Unmap completed successfully.
  @retval    EFI_INVALID_PARAMETER  Invalid PageBuffer or Ranges.

**/
EFI_STATUS
EFIAPI
UnmapMemoryRange (
  IN MAP_RANGE   Ranges[1],
  IN VOID       *PageBuffer
  )
{
  UINTN         *PageTable;
  UINTN          Idx;
  UINTN          Start;
  UINTN          End;
  UINT32         PageBits;
  UINT32         PdeOffset;
  UINT32         Attribute;

  if (PageBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // Check if range is 4KB page aligned
  if (((Ranges[0].Start & 0xFFF) != 0) || ((Ranges[0].Limit & 0xFFF) != 0xFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  Attribute    = IA32_PG_P | IA32_PG_RW | IA32_PG_AC;

  // Remove 4KB pages from PDE
  PageBits  = GetPageShiftBits (PDE_PG_LVL);
  if (PageBits == 21) {
    // 64 bit mode
    PdeOffset = 2 * EFI_PAGE_SIZE;
  } else {
    // 32 bit mode
    PdeOffset = 0;
  }

  PageTable = (UINTN *)((UINTN)PageBuffer + PdeOffset);
  Start = (UINTN) RShiftU64 (Ranges[0].Start, PageBits);
  End   = (UINTN) RShiftU64 (Ranges[0].Limit, PageBits);
  for (Idx = Start; Idx < End + 1; Idx++) {
    PageTable[Idx] = (UINTN)LShiftU64 (Idx, PageBits) | Attribute | IA32_PG_PD;
  }

  return EFI_SUCCESS;
}
