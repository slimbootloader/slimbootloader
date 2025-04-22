;------------------------------------------------------------------------------
;
; Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;
; Module Name:
;
;  UpdateSelectors.nasm
;
; Abstract:
;
;  This code reloads the segment selectors after GDT update
;
;------------------------------------------------------------------------------
DEFAULT REL
SECTION .text
BITS 32

; LINEAR_CODE_SEL 0x10 for ia32 CS
FarJmpData:
    dd ReloadCs
    dw 10h

global  ASM_PFX(UpdateSelectors)
ASM_PFX(UpdateSelectors):
        push    ax
        ; LINEAR_SEL selector 0x8 for ia32 segments
        mov     ax, 8
        mov     ds, ax
        mov     es, ax
        mov     fs, ax
        mov     gs, ax
        mov     ss, ax
        jmp far [FarJmpData]
ReloadCs:
        pop     ax
        ret
