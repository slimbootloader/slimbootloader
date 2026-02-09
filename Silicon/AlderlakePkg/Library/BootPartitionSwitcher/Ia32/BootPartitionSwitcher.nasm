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

SECTION .text
BITS 32
ASM_PFX(BootPartitionSwitcherAsmStart):
    mov     edx, [esp + 4]
    mov     eax, [edx + CONTEXT_TOP_SWAP_BASE]
    mov     edi, eax
    mov     al, [edx + CONTEXT_PARTITION]
    test    al, al
    jz      ASM_PFX(BootPartitionSwitcherAsmStartClear)
    mov     eax, [edi]
    or      eax, 1
    jmp     ASM_PFX(BootPartitionSwitcherAsmStartWrite)
ASM_PFX(BootPartitionSwitcherAsmStartClear):
    mov     eax, [edi]
    and     eax, 0xFFFFFFFE
ASM_PFX(BootPartitionSwitcherAsmStartWrite):
    mov     [edi], eax
    mov     al, [edx + CONTEXT_P2SB_HIDDEN]
    test    al, al
    jz      ASM_PFX(BootPartitionSwitcherAsmStartReset)
    mov     edi, [edx + CONTEXT_P2SB_BASE]
    mov     byte [edi + P2SB_HIDE_OFFSET], 1
ASM_PFX(BootPartitionSwitcherAsmStartReset):
    mov     dx, RESET_PORT
    mov     al, RESET_FULL
    out     dx, al
ASM_PFX(BootPartitionSwitcherAsmStartSpin):
    cli
    hlt
    jmp     ASM_PFX(BootPartitionSwitcherAsmStartSpin)
ASM_PFX(BootPartitionSwitcherAsmEnd):
