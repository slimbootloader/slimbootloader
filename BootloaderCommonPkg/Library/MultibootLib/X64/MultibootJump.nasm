;------------------------------------------------------------------------------
;
; Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;  MultibootJump.nasm
;
; Abstract:
;
;  This is the code that goes from payload to a Multiboot enabled OS.
;
;------------------------------------------------------------------------------
DEFAULT  REL
SECTION .text

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; AsmJumpToMultibootOs (
;   IA32_BOOT_STATE  *State
;   );
;------------------------------------------------------------------------------
global ASM_PFX(AsmJumpToMultibootOs)
ASM_PFX(AsmJumpToMultibootOs):
BITS 32
    mov    ecx, DWORD [esp + 4]
    mov    eax, DWORD [ecx + 4]
    mov    ebx, DWORD [ecx + 8]
    mov    esi, DWORD [ecx + 12]
    mov    edi, DWORD [ecx + 16]
    cli
    jmp    DWORD [ecx]
    ret
