;------------------------------------------------------------------------------
;
; Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   MpFuncs.nasm
;
; Abstract:
;
;   This is the assembly code for initializing the APs in IA32 Protected Mode
;
;-------------------------------------------------------------------------------
SECTION .text

BITS   32
align  16
;-------------------------------------------------------------------------------------
;  AsmGetBspSelectors (&BspSelectors);
;-------------------------------------------------------------------------------------
;typedef struct {
;  UINT16 CSSelector;
;  UINT16 DSSelector;
;  UINT16 ESSelector;
;  UINT16 SSSelector;
;  UINT16 FSSelector;
;  UINT16 GSSelector;
;  UINT   Gdtr;
;  UINT   Idtr;
;} MP_BSP_SELECTORS;
global ASM_PFX(AsmGetBspSelectors)
ASM_PFX(AsmGetBspSelectors):
        push        edi
        mov         edi, dword [esp + 8]
        mov         ax, cs
        mov         word [edi + 0], ax
        mov         ax, ds
        mov         word [edi + 2], ax
        mov         ax, es
        mov         word [edi + 4], ax
        mov         ax, ds
        mov         word [edi + 6], ax
        mov         ax, fs
        mov         word [edi + 8], ax
        mov         ax, gs
        mov         word [edi + 10], ax
        pop         edi
        ret


global ASM_PFX(AsmCliHlt)
ASM_PFX(AsmCliHlt):
        cli
        hlt
        jmp         $-2

