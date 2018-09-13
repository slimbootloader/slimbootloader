;------------------------------------------------------------------------------
;
; Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
; This program and the accompanying materials
; are licensed and made available under the terms and conditions of the BSD License
; which accompanies this distribution.  The full text of the license may be found at
; http://opensource.org/licenses/bsd-license.php.
;
; THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
; WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
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
