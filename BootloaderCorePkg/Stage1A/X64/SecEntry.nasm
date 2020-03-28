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
extern  ASM_PFX(TempRamInitParams)
extern  ASM_PFX(PcdGet32(PcdStage1StackSize))
extern  ASM_PFX(PcdGet32(PcdStage1DataSize))
extern  ASM_PFX(PcdGet32(PcdStage1StackBaseOffset))


global  ASM_PFX(_ModuleEntryPoint)
ASM_PFX(_ModuleEntryPoint):
        movd    mm0, eax

        ;
        ; Read time stamp in esi
        ;
        mov     rbx, rdx
        rdtsc
        mov     rsi, rdx
        shl     rsi, 32
        add     rsi, rax
        mov     rdx, rbx

        ;
        ; Add a dummy reference to TempRamInitParams to prevent
        ; compiler from removing this symbol
        ;
        mov     rax, ASM_PFX(TempRamInitParams)

        ;
        ; Setup stack
        ; ECX: Bootloader stack base
        ; EDX: Bootloader stack top
        ;
        mov     rax, ASM_PFX(PcdGet32(PcdStage1StackBaseOffset))
        mov     eax, dword [rax]
        mov     rsp, rax
        add     rsp, rcx
        mov     rax, ASM_PFX(PcdGet32(PcdStage1StackSize))
        mov     eax, dword [rax]
        add     rsp, rax

        ;
        ; Check stage1 stack base offset
        ;
        xor     rbx, rbx             ; Use EBX for Status
        mov     rax, ASM_PFX(PcdGet32(PcdStage1DataSize))
        mov     eax, dword [rax]
        add     rax, rsp
        cmp     rax, rdx
        jle     CheckStackRangeDone

        ;
        ; Error in stack range
        ;
        bts     rbx, 1               ; Set BIT1 in Status
        mov     rsp, rdx             ; Set ESP to the end

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
        bts     rbx, 0               ; Set BIT0 in Status

CheckStatusDone:
        ; Setup HOB
        push    rbx                  ; Status
        push    rsi                  ; TimeStamp[0] [63:0]
        shl     rdx, 32              ; Move CarTop to high 32bit
        add     rdx, rcx             ; Add back CarBase
        push    rdx
        mov     rcx, rsp             ; Argument 1

        sub     rsp, 32              ; 32 bytes shadow store for x64
        call    ASM_PFX(SecStartup)  ; Jump to C code
        jmp     $

