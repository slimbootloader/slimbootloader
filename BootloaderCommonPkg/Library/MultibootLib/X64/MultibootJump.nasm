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
    ; TBD: Need porting
    jmp    $
    ret
