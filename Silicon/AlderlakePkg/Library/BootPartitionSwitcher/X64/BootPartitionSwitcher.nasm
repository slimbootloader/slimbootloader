;------------------------------------------------------------------------------
; Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;   BootPartitionSwitcher.nasm
;
; Abstract:
;   Runtime-copied stub to update top swap and issue cold reset.
;
;------------------------------------------------------------------------------

%include "Nasm.inc"

global ASM_PFX(BootPartitionSwitcherAsmStart)
global ASM_PFX(BootPartitionSwitcherAsmEnd)

%define CONTEXT_TOP_SWAP_BASE   0
%define CONTEXT_P2SB_BASE       4
%define CONTEXT_PARTITION       8
%define CONTEXT_P2SB_HIDDEN     9

%define P2SB_HIDE_OFFSET        0xE1
%define RESET_PORT              0xCF9
%define RESET_FULL              0x0E

DEFAULT REL
SECTION .text
BITS 64
ASM_PFX(BootPartitionSwitcherAsmStart):
    mov     rdx, rcx
    mov     eax, [rdx + CONTEXT_TOP_SWAP_BASE]
    mov     r8d, eax
    mov     al, [rdx + CONTEXT_PARTITION]
    test    al, al
    jz      ASM_PFX(BootPartitionSwitcherAsmStartClear)
    mov     ecx, [r8]
    or      ecx, 1
    jmp     ASM_PFX(BootPartitionSwitcherAsmStartWrite)
ASM_PFX(BootPartitionSwitcherAsmStartClear):
    mov     ecx, [r8]
    and     ecx, 0xFFFFFFFE
ASM_PFX(BootPartitionSwitcherAsmStartWrite):
    mov     [r8], ecx
    mov     al, [rdx + CONTEXT_P2SB_HIDDEN]
    test    al, al
    jz      ASM_PFX(BootPartitionSwitcherAsmStartReset)
    mov     r9d, [rdx + CONTEXT_P2SB_BASE]
    mov     byte [r9 + P2SB_HIDE_OFFSET], 1
ASM_PFX(BootPartitionSwitcherAsmStartReset):
    mov     dx, RESET_PORT
    mov     al, RESET_FULL
    out     dx, al
ASM_PFX(BootPartitionSwitcherAsmStartSpin):
    cli
    hlt
    jmp     ASM_PFX(BootPartitionSwitcherAsmStartSpin)
ASM_PFX(BootPartitionSwitcherAsmEnd):
