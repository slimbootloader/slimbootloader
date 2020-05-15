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
