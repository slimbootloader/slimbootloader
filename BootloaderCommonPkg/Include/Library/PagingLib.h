/** @file

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PAGING_LIB_H_
#define _PAGING_LIB_H_

#define  IA32_PG_P                   BIT0
#define  IA32_PG_RW                  BIT1
#define  IA32_PG_AC                  BIT5
#define  IA32_PG_D                   BIT6
#define  IA32_PG_PD                  BIT7

typedef struct {
  UINTN   Start;
  UINTN   Limit;
  UINTN   Mapping;
  UINT32  PageSize;
} MAP_RANGE;

/**
  This function returns page tables memory size.

  @param[in] IsX64Mode     Determine to build page table for x64 mode or not.

  @retval   Page table memory size required.

**/
UINT32
EFIAPI
GetPageTablesMemorySize (
  IN BOOLEAN     IsX64Mode
  );

/**
  This function dumps the current PageTables

  @param None

  @retval None
**/

VOID
EFIAPI
DumpPageTables (
  VOID
  );

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
  );

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
  );

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
  );

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
  Load page table for long mode.

**/
VOID
EFIAPI
LoadPageTableForLongMode (
  VOID
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
  Get physical address bits.

  @return Physical address bits.

**/
UINT8
EFIAPI
GetPhysicalAddressBits (
  VOID
  );

#endif
