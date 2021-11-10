;------------------------------------------------------------------------------
; @file
; Build paging tables dynamically for x64 mode
;
; Copyright (c) 2020 Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;------------------------------------------------------------------------------

%define FSP_HEADER_TEMPRAMINIT_OFFSET 0x30
%define PAGE_REGION_SIZE              0x6000

%define PAGE_PRESENT            0x01
%define PAGE_READ_WRITE         0x02
%define PAGE_USER_SUPERVISOR    0x04
%define PAGE_WRITE_THROUGH      0x08
%define PAGE_CACHE_DISABLE      0x010
%define PAGE_ACCESSED           0x020
%define PAGE_DIRTY              0x040
%define PAGE_PAT                0x080
%define PAGE_GLOBAL             0x0100
%define PAGE_2M_MBO             0x080
%define PAGE_2M_PAT             0x01000

%define PAGE_2M_PDE_ATTR (PAGE_2M_MBO + \
                          PAGE_ACCESSED + \
                          PAGE_DIRTY + \
                          PAGE_READ_WRITE + \
                          PAGE_PRESENT)

%define PAGE_PDP_ATTR    (PAGE_ACCESSED + \
                          PAGE_READ_WRITE + \
                          PAGE_PRESENT)

BITS    32
PreparePagingTable:
    ; Input:
    ;   ECX:  Page table base, need 6 pages
    ; Modify:
    ;   ECX, EDX, ESI

    ;
    ; Set up identical paging table for x64
    ;
    mov     esi, ecx
    mov     ecx, PAGE_REGION_SIZE / 4
    xor     eax, eax
    xor     edx, edx
clearPageTablesMemoryLoop:
    mov     dword[ecx * 4 + esi - 4], eax
    loop    clearPageTablesMemoryLoop

    ;
    ; Top level Page Directory Pointers (1 * 512GB entry)
    ;
    lea     eax, [esi + (0x1000) + PAGE_PDP_ATTR]
    mov     dword[esi + (0)], eax
    mov     dword[esi + (4)], edx

    ;
    ; Next level Page Directory Pointers (4 * 1GB entries => 4GB)
    ;
    lea     eax, [esi + (0x2000) + PAGE_PDP_ATTR]
    mov     dword[esi + (0x1000)], eax
    mov     dword[esi + (0x1004)], edx
    lea     eax, [esi + (0x3000) + PAGE_PDP_ATTR]
    mov     dword[esi + (0x1008)], eax
    mov     dword[esi + (0x100C)], edx
    lea     eax, [esi + (0x4000) + PAGE_PDP_ATTR]
    mov     dword[esi + (0x1010)], eax
    mov     dword[esi + (0x1014)], edx
    lea     eax, [esi + (0x5000) + PAGE_PDP_ATTR]
    mov     dword[esi + (0x1018)], eax
    mov     dword[esi + (0x101C)], edx

    ;
    ; Page Table Entries (2048 * 2MB entries => 4GB)
    ;
    mov     ecx, 0x800
pageTableEntriesLoop:
    mov     eax, ecx
    dec     eax
    shl     eax, 21
    add     eax, PAGE_2M_PDE_ATTR
    mov     [ecx * 8 + esi + (0x2000 - 8)], eax
    mov     [(ecx * 8 + esi + (0x2000 - 8)) + 4], edx
    loop    pageTableEntriesLoop

    mov     eax, esi
    OneTimeCallRet  PreparePagingTable
