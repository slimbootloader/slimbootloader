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
  Get shift bit number from address to page index

  @retval    Bit to shift to get page index.

**/
UINT32
GetPageShiftBits (
  VOID
)
{
  return IsLongModeEnabled() ? 21 : 22;
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
  UINTN          EntryNum;
  UINTN         *PageTable;
  UINTN         *PageTable4K;
  UINT32         PageBase;
  UINT32         Attribute;
  UINT32         Address;
  UINT32         PteOffset;
  UINT32         PdeOffset;
  UINT32         Alignment;
  UINT32         PageBits;

  Attribute    = IA32_PG_P | IA32_PG_RW | IA32_PG_AC;

  // Define CAR region and memory mapping
  if (((Ranges[0].Start & 0xFFF) != 0) || ((Ranges[0].Limit & 0xFFF) != 0xFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  PageBits  = GetPageShiftBits ();
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

  Alignment = (UINT32) LShiftU64 (1, PageBits) - 1;
  PageBase  = Ranges[0].Start & ~Alignment;
  if (((Ranges[0].Start & ~Alignment) != PageBase) || ((Ranges[0].Limit & ~Alignment) != PageBase)) {
    return EFI_INVALID_PARAMETER;
  }

  // Create 4KB pages and remap the CAR region into a different memory location
  PageTable4K = (UINTN *)((UINTN)PageBuffer + PteOffset);
  Address = PageBase;
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
  PageTable      = (UINTN *)((UINTN)PageBuffer + PdeOffset);
  PageTable[Idx] = (UINTN)PageTable4K + Attribute;

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

  if (PageBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // Check if range is 4KB page aligned
  if (((Ranges[0].Start & 0xFFF) != 0) || ((Ranges[0].Limit & 0xFFF) != 0xFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  // Remove 4KB pages from PDE
  PageBits  = GetPageShiftBits ();
  PageTable = (UINTN *)((UINTN)PageBuffer);
  Start = (UINT32) RShiftU64 (Ranges[0].Start, PageBits);
  End   = (UINT32) RShiftU64 (Ranges[0].Limit + 1, PageBits);
  for (Idx = Start; Idx < End; Idx++) {
    PageTable[Idx] |= IA32_PG_PD;
  }

  return EFI_SUCCESS;
}
