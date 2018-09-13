;------------------------------------------------------------------------------
;
; Copyright (c) 2006 - 2015, Intel Corporation. All rights reserved.<BR>
; This program and the accompanying materials
; are licensed and made available under the terms and conditions of the BSD License
; which accompanies this distribution.  The full text of the license may be found at
; http://opensource.org/licenses/bsd-license.php.
;
; THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
; WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
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
