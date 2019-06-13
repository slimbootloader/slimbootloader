;------------------------------------------------------------------------------
;
; Copyright (c) 2006 - 2015, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;
; Module Name:
;
;  FspTempRamInit.nasm
;
; Abstract:
;
;  This is the code that will call into FSP TempRamInit API
;
;------------------------------------------------------------------------------


SECTION .text

; FSP API offset
%define FSP_HEADER_TEMPRAMINIT_OFFSET 0x30

extern  ASM_PFX(PcdGet32(PcdFSPTBase))
extern  ASM_PFX(TempRamInitParams)

global  ASM_PFX(FspTempRamInit)
ASM_PFX(FspTempRamInit):
        ;
        ; This hook is called to initialize temporay RAM
        ; ESI, EDI need to be preserved
        ; ESP contains return address
        ; ECX, EDX return the temprary RAM start and end
        ;

        ;
        ; Get FSP-T base in EAX
        ;
        mov     ebp, esp
        mov     eax, dword [ASM_PFX(PcdGet32(PcdFSPTBase))]

        ;
        ; Find the fsp info header
        ; Jump to TempRamInit API
        ;
        add     eax, dword [eax + 094h + FSP_HEADER_TEMPRAMINIT_OFFSET]
        mov     esp, TempRamInitStack
        jmp     eax

TempRamInitDone:
        mov     esp, ebp
        jmp     esp

align 16
TempRamInitStack:
        DD      TempRamInitDone
        DD      ASM_PFX(TempRamInitParams)
