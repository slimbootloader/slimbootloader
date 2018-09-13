;------------------------------------------------------------------------------
;
; Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
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
extern  ASM_PFX(PcdGet32(PcdStage1StackSize))
extern  ASM_PFX(EarlyBoardInit)
extern  ASM_PFX(FspTempRamInit)

global  ASM_PFX(_ModuleEntryPoint)
ASM_PFX(_ModuleEntryPoint):
        ;
        ; Read time stamp
        ;
        rdtsc
        mov     esi, eax
        mov     edi, edx

        ;
        ; Early board hooks
        ;
        mov     esp, EarlyBoardInitRet
        jmp     ASM_PFX(EarlyBoardInit)

EarlyBoardInitRet:
        mov     esp, FspTempRamInitRet
        jmp     ASM_PFX(FspTempRamInit)

FspTempRamInitRet:
        cmp     eax, 8000000Eh      ;Check if EFI_NOT_FOUND returned. Error code for Microcode Update not found.
        je      FspApiSuccess       ;If microcode not found, don't hang, but continue.

        cmp     eax, 0              ;Check if EFI_SUCCESS returned.
        jz      FspApiSuccess

        ; FSP API failed:
        jmp     $

FspApiSuccess:
        ;
        ; Setup stack
        ; ECX: Bootloader stack base
        ; EDX: Bootloader stack top
        ;
        mov     esp, ecx
        add     esp, dword [ASM_PFX(PcdGet32(PcdStage1StackSize))]
        mov     eax, esp

        ; Setup HOB
        push    edi                  ; TimeStamp[0] [63:32]
        push    esi                  ; TimeStamp[0] [31:0]
        push    edx                  ; CarTop
        push    ecx                  ; CarBase

        push    esp
        call    ASM_PFX(SecStartup)  ; Jump to C code
        jmp     $

