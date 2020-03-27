;------------------------------------------------------------------------------
;
; Copyright (c) 2016 - 2020, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;
; Module Name:
;
;  SecEntry.nasm
;
; Abstract:
;
;  This is the code that goes from real-mode to protected mode.
;  It consumes the reset vector.
;
;------------------------------------------------------------------------------

SECTION .text

extern  ASM_PFX(SecStartup)
extern  ASM_PFX(TempRamInitParams)


global  ASM_PFX(_ModuleEntryPoint)
ASM_PFX(_ModuleEntryPoint):
        call    ASM_PFX(SecStartup)  ; Jump to C code
        jmp     $

        ; To avoid compiler optimization
        jmp     ASM_PFX(TempRamInitParams)
