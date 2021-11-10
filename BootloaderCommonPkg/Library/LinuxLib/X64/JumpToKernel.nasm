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
    mov     rsi, rdx
    call    rcx
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
    jmp    JumpToKernel
