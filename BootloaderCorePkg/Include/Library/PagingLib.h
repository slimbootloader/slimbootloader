/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PAGING_LIB_H_
#define _PAGING_LIB_H_

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
  ASM inline function Paging32.nasm

  @param [in] StartAddress Pointer to Start address where cache needs to be flushed
  @param [in] EndAddress   Pointer to End address where cache needs to be flushed
**/
VOID *
EFIAPI
FlushCacheLine (
  IN      VOID                      *StartAddress,
  IN      VOID                      *EndAddress
  );

#endif
