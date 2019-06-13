;------------------------------------------------------------------------------
;
; Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
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

SECTION .text

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; JumpToMultibootOs (
;   IA32_BOOT_STATE state
;   );
;------------------------------------------------------------------------------
global ASM_PFX(JumpToMultibootOs)
ASM_PFX(JumpToMultibootOs):
    mov    ecx, DWORD [esp + 4]
    mov    eax, DWORD [ecx + 4]
    mov    ebx, DWORD [ecx + 8]
    mov    esi, DWORD [ecx + 12]
    mov    edi, DWORD [ecx + 16]
    cli
    jmp    DWORD [ecx]
    ret
