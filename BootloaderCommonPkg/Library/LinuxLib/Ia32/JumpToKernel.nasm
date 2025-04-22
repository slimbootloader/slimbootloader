;------------------------------------------------------------------------------
;
; Copyright (c) 2006 - 2020, Intel Corporation. All rights reserved.<BR>
;
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;------------------------------------------------------------------------------

  SECTION .text

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; JumpToKernel (
;   VOID *KernelStart,
;   VOID *KernelBootParams
;   );
;------------------------------------------------------------------------------
global ASM_PFX(JumpToKernel)
ASM_PFX(JumpToKernel):
    mov     esi, [esp + 8]
    call    DWORD [esp + 4]
    ret

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; JumpToKernel64 (
;   VOID *KernelStart,
;   VOID *KernelBootParams
;   );
;------------------------------------------------------------------------------
global ASM_PFX(JumpToKernel64)
ASM_PFX(JumpToKernel64):
    DB      0x67, 0x48                ; 32-bit address size, 64-bit operand size
    mov     esi, edx                  ; mov esi, rdx
    call    ecx
    hlt
