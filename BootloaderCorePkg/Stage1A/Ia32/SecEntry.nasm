;------------------------------------------------------------------------------
;
; Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
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
extern  ASM_PFX(PcdGet32(PcdStage1StackSize))
extern  ASM_PFX(PcdGet32(PcdStage1DataSize))
extern  ASM_PFX(PcdGet32(PcdStage1StackBaseOffset))
extern  ASM_PFX(EarlyBoardInit)
extern  ASM_PFX(FspTempRamInit)

global  ASM_PFX(_ModuleEntryPoint)
ASM_PFX(_ModuleEntryPoint):
        movd    mm0, eax

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
        add     esp, dword [ASM_PFX(PcdGet32(PcdStage1StackBaseOffset))]
        add     esp, dword [ASM_PFX(PcdGet32(PcdStage1StackSize))]

        xor     ebx, ebx             ; Use EBX for Status
        ;
        ; Check stage1 stack base offset
        ;
        mov     eax, esp
        add     eax, dword [ASM_PFX(PcdGet32(PcdStage1DataSize))]
        cmp     eax, edx
        jle     CheckStackRangeDone

        ;
        ; Error in stack range
        ;
        bts     ebx, 1               ; Set BIT1 in Status
        sub     esp, dword [ASM_PFX(PcdGet32(PcdStage1StackBaseOffset))]

CheckStackRangeDone:
        ;
        ; CpuBist error check
        ;
        movd    eax, mm0
        emms                         ; Exit MMX Instruction
        cmp     eax, 0
        jz      CheckStatusDone

        ;
        ; Error in CpuBist
        ;
        bts     ebx, 0               ; Set BIT0 in Status

CheckStatusDone:
        ;
        ; Setup HOB
        ; This structure has to match the layout of STAGE1A_ASM_PARAM
        ;
        push    $0                   ; Status[63:32]
        push    ebx                  ; Status[31:0]
        push    edi                  ; TimeStamp[0] [63:32]
        push    esi                  ; TimeStamp[0] [31:0]
        push    edx                  ; CarTop
        push    ecx                  ; CarBase
        push    $0                   ; Keep the stack 16-byte aligned

        lea     ecx, [esp + 4]
        push    ecx
        call    ASM_PFX(SecStartup)  ; Jump to C code
        jmp     $

