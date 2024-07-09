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

BITS 64

; LINEAR_CODE64_SEL 0x20 for x64 CS
FarJmpData:
    dq ReloadCs
    dw 20h

global  ASM_PFX(UpdateSelectors)
ASM_PFX(UpdateSelectors):
        push    ax
        ; LINEAR_SEL 0x8 for x64 segments
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
