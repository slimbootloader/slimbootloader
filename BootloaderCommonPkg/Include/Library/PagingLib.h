/** @file

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PAGING_LIB_H_
#define _PAGING_LIB_H_

#define CR0_WP                      BIT16

#define IA32_PG_P                   BIT0
#define IA32_PG_RW                  BIT1
#define IA32_PG_PS                  BIT7

#define PAGING_PAE_INDEX_MASK       0x1FF

#define PAGING_4K_ADDRESS_MASK_64   0x000FFFFFFFFFF000ull
#define PAGING_2M_ADDRESS_MASK_64   0x000FFFFFFFE00000ull
#define PAGING_1G_ADDRESS_MASK_64   0x000FFFFFC0000000ull

#define PAGING_L1_ADDRESS_SHIFT     12
#define PAGING_L2_ADDRESS_SHIFT     21
#define PAGING_L3_ADDRESS_SHIFT     30
#define PAGING_L4_ADDRESS_SHIFT     39

#define PAGE_TABLE_POOL_ALIGNMENT   BASE_4KB
#define PAGE_TABLE_POOL_UNIT_SIZE   SIZE_4KB
#define PAGE_TABLE_POOL_ALIGN_MASK  \
  (~(EFI_PHYSICAL_ADDRESS)(PAGE_TABLE_POOL_ALIGNMENT - 1))

//
// Page-Map Level-4 Offset (PML4) and
// Page-Directory-Pointer Offset (PDPE) entries 4K & 2MB
//
typedef union {
  struct {
    UINT64  Present:1;                // 0 = Not present in memory,
                                      //   1 = Present in memory
    UINT64  ReadWrite:1;              // 0 = Read-Only, 1= Read/Write
    UINT64  UserSupervisor:1;         // 0 = Supervisor, 1=User
    UINT64  WriteThrough:1;           // 0 = Write-Back caching,
                                      //   1 = Write-Through caching
    UINT64  CacheDisabled:1;          // 0 = Cached, 1=Non-Cached
    UINT64  Accessed:1;               // 0 = Not accessed,
                                      //   1 = Accessed (set by CPU)
    UINT64  Reserved:1;               // Reserved
    UINT64  MustBeZero:2;             // Must Be Zero
    UINT64  Available:3;              // Available for use by system software
    UINT64  PageTableBaseAddress:40;  // Page Table Base Address
    UINT64  AvabilableHigh:11;        // Available for use by system software
    UINT64  Nx:1;                     // No Execute bit
  } Bits;
  UINT64    Uint64;
} PAGE_MAP_AND_DIRECTORY_POINTER;

//
// Page Table Entry 2MB
//
typedef union {
  struct {
    UINT64  Present:1;                // 0 = Not present in memory, 1 = Present in memory
    UINT64  ReadWrite:1;              // 0 = Read-Only, 1= Read/Write
    UINT64  UserSupervisor:1;         // 0 = Supervisor, 1=User
    UINT64  WriteThrough:1;           // 0 = Write-Back caching, 1=Write-Through caching
    UINT64  CacheDisabled:1;          // 0 = Cached, 1=Non-Cached
    UINT64  Accessed:1;               // 0 = Not accessed, 1 = Accessed (set by CPU)
    UINT64  Dirty:1;                  // 0 = Not Dirty, 1 = written by processor on access to page
    UINT64  MustBe1:1;                // Must be 1
    UINT64  Global:1;                 // 0 = Not global page, 1 = global page TLB not cleared on CR3 write
    UINT64  Available:3;              // Available for use by system software
    UINT64  PAT:1;                    //
    UINT64  MustBeZero:8;             // Must be zero;
    UINT64  PageTableBaseAddress:31;  // Page Table Base Address
    UINT64  AvabilableHigh:11;        // Available for use by system software
    UINT64  Nx:1;                     // 0 = Execute Code, 1 = No Code Execution
  } Bits;
  UINT64    Uint64;
} PAGE_TABLE_ENTRY;

//
// Page Table Entry 1GB
//
typedef union {
  struct {
    UINT64  Present:1;                // 0 = Not present in memory, 1 = Present in memory
    UINT64  ReadWrite:1;              // 0 = Read-Only, 1= Read/Write
    UINT64  UserSupervisor:1;         // 0 = Supervisor, 1=User
    UINT64  WriteThrough:1;           // 0 = Write-Back caching, 1=Write-Through caching
    UINT64  CacheDisabled:1;          // 0 = Cached, 1=Non-Cached
    UINT64  Accessed:1;               // 0 = Not accessed, 1 = Accessed (set by CPU)
    UINT64  Dirty:1;                  // 0 = Not Dirty, 1 = written by processor on access to page
    UINT64  MustBe1:1;                // Must be 1
    UINT64  Global:1;                 // 0 = Not global page, 1 = global page TLB not cleared on CR3 write
    UINT64  Available:3;              // Available for use by system software
    UINT64  PAT:1;                    //
    UINT64  MustBeZero:17;            // Must be zero;
    UINT64  PageTableBaseAddress:22;  // Page Table Base Address
    UINT64  AvabilableHigh:11;        // Available for use by system software
    UINT64  Nx:1;                     // 0 = Execute Code, 1 = No Code Execution
  } Bits;
  UINT64    Uint64;
} PAGE_TABLE_1G_ENTRY;

typedef struct {
  UINT32  Start;
  UINT32  Limit;
  UINT32  Mapping;
} RANGE;

/**
  This function returns page tables memory size (2 * SIZE_4KB).
**/
UINT32
EFIAPI
GetPageTablesMemorySize (
  VOID
  );

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
  );

/**
  ASM inline function Paging32.nasm - Enable Paging
  Set Page Global Enable (Set PGE in CR4)
  loads Pdpe to CR3 and Set PG bit in CR0.

  @param [in] Pdpe pointer to Page Directory and Page Table Entries
**/
VOID
EFIAPI
EnablePaging32 (
  IN      UINT32     *Pdpe
  );

/**
  ASM inline function Paging32.nasm - Disable Paging (Clear PG bit in CR0).
**/
VOID
EFIAPI
DisablePaging32 (
  VOID
  );

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
  );

/**
  Whether 64-bit long mode is enabled or not.

  @return   TRUE if long mode has already been enabled, otherwise FALSE

**/
BOOLEAN
EFIAPI
IsLongModeEnabled (
  VOID
  );

/**
  Whether processor supports 64-bit long mode or not.

  @return   TRUE if processor supports 64-bit long mode, otherwise FALSE

**/
BOOLEAN
EFIAPI
IsLongModeSupported (
  VOID
  );

/**
  This function create and load 4GB identical mapping paging table.
**/
VOID
EFIAPI
LoadPageTable (
  VOID
  );

#endif
