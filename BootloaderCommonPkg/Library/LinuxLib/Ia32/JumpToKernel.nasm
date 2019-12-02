;------------------------------------------------------------------------------
;
; Copyright (c) 2006 - 2013, Intel Corporation. All rights reserved.<BR>
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

