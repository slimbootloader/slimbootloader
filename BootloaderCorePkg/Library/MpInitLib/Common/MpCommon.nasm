;------------------------------------------------------------------------------ ;
; Copyright (c) 2015 - 2021, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   MpCommon.nasm
;
; Abstract:
;
;   This is the  common funcitons for Multiple Processor support
;
;-------------------------------------------------------------------------------

%include "Nasm.inc"
%include "MpEqu.inc"

global ASM_PFX(RendezvousFunnelProcStart)
ASM_PFX(RendezvousFunnelProcStart):
RendezvousFunnelProc:
; At this point CS = 0x(vv00) and ip= 0x0.
BITS 16
    mov            eax, cr2
    or             eax, eax
    ; For SMM rebase flow, CR2 will be pre-filled with SMBASE
    ; The same stub code will be shared for SMM rebasing and start IPI.
    jz             StartIpi


    ; The ASM code needs to use same 0x3800 segment as start IPI.
    ; At initial SMM entry the segment is 0x3000, so force a long jump.
    jmp            0x3800:(StartIpi - RendezvousFunnelProc)

StartIpi:
    mov            ax,  cs
    mov            ds,  ax
    mov            es,  ax
    mov            ss,  ax
    xor            ax,  ax
    mov            fs,  ax
    mov            gs,  ax

    mov            si, AP_DATA_STRUCT_FIELD (BufferStart)
    mov            edx, [si]    ; EDX is keeping the start address of wakeup buffer

    mov            si, AP_DATA_STRUCT_FIELD (Gdtr)
o32 lgdt           [si]

    mov            si, AP_DATA_STRUCT_FIELD (Idtr)
o32 lidt           [si]

    ; Switch to protected mode
    mov            eax, cr0           ; Get control register 0
    or             eax, 000000003h    ; Set PE bit (bit #0) & MP
    mov            cr0, eax

    ; Switch to 32-bit code in executable memory (>1MB)
    db             66h, 67h, 0EAh     ; far jump
    dd             0x38000 + ProtModeStart - RendezvousFunnelProc; 32-bit offset
    dw             0x10               ; 16-bit selector

BITS 32
SmmRebase:
    mov            edi, 0x3fefc      ; SMBASE revision
    mov            cl,  [edi]
    mov            edi, 0x3fef8
    cmp            cl, 0x64
    jnz            SmmEm64t
    mov            edi, 0x3ff00      ; AMD64, required by QEMU
    mov            ecx, [edi]
    cmp            ecx, 0x30000      ; Should contain default SMBASE
    jz             SmmEm64t
    mov            edi, 0x3fef8      ; Fallback to support SIMICS QSP model
SmmEm64t:
    mov            [edi], eax        ; change to new  SMBASE
    mov            eax, [esi + AP_DATA_STRUCT_FIELD (SmrrMask)]
    cmp            eax, 0
    jz             SkipSmrrProg

    xchg           eax, ebx
    mov            eax, [esi + AP_DATA_STRUCT_FIELD (SmrrBase)]
    xor            edx, edx
    mov            ecx, 0x1f2        ; MSR_IA32_SMRR_PHYSBASE
    wrmsr
    inc            ecx
    xchg           eax, ebx
    wrmsr

SkipSmrrProg:
    rsm
    jmp            $

ProtModeStart:                    ; protected mode entry point
    ; Since CS and DS base is set to 0, we need to add the buffer start to use the offset

    mov            esi, edx           ; EDX is keeping the start address of wakeup buffer

    mov            ax, word [cs:esi + AP_DATA_STRUCT_FIELD (DSSelector)]
    mov            ds, ax

    mov            ax, word [cs:esi + AP_DATA_STRUCT_FIELD (ESSelector)]
    mov            es, ax

    mov            ax, word [cs:esi + AP_DATA_STRUCT_FIELD (FSSelector)]
    mov            fs, ax

    mov            ax, word [cs:esi + AP_DATA_STRUCT_FIELD (GSSelector)]
    mov            gs, ax

    mov            ax, word [cs:esi + AP_DATA_STRUCT_FIELD (SSSelector)]
    mov            ss, ax

    cli

    mov            eax, cr2           ; CR2 will be set to the new SMBASE before SMI IPI is generated
    or             eax, eax
    jnz            SmmRebase

    mov            eax, cr0           ; Enable cache
    and            eax, 09fffffffh
    mov            cr0, eax

    mov            eax, cr4           ; ENABLE_SSE
    or             eax, 00000600h
    mov            cr4, eax

CallApFunc:
    ; Acquire Lock
    mov            eax, SPIN_LOCK_RELEASED
    mov            edx, SPIN_LOCK_ACQUIRED
    lock cmpxchg   [esi + AP_DATA_STRUCT_FIELD (SpinLock)], edx
    jnz            CallApFunc

    ; Calculate stack
    inc            dword [esi + AP_DATA_STRUCT_FIELD (ApCounter)]
    mov            eax, [esi + AP_DATA_STRUCT_FIELD (ApCounter)]
    mov            ebx, eax

    ; Program AP stack for each thread
    mov            ecx, [esi + AP_DATA_STRUCT_FIELD (ApStackSize)]
    shl            eax, cl
    add            eax, [esi + AP_DATA_STRUCT_FIELD (StackStart)]
    lea            esp, [eax - 0x10]

    ; Release Lock
    mov            eax, SPIN_LOCK_RELEASED
    mov            [esi + AP_DATA_STRUCT_FIELD (SpinLock)], eax

    mov            eax, [esi + AP_DATA_STRUCT_FIELD (CpuArch)]
    cmp            eax, 0
    jnz            SwitchToLongMode

    ; Call C Function
    mov            eax, [esi + AP_DATA_STRUCT_FIELD (CProcedure)]
    test           eax, eax
    jz             GoToSleep

    mov            eax, [esi + AP_DATA_STRUCT_FIELD (MpDataStruct)]
    push           eax
    push           ebx

    mov            eax, [esi + AP_DATA_STRUCT_FIELD (CProcedure)]
    call           eax
    add            esp, 8

    wbinvd

GoToSleep:
    cli
    hlt
    jmp            $-2


SwitchToLongMode:
    ; Prepare stack for RETF
    xor            eax, eax
    mov            ax, word [esi + AP_DATA_STRUCT_FIELD (CSSelector)]
    push           eax
    lea            eax, [LongModeStart - RendezvousFunnelProc]
    add            eax, esi
    push           eax

    ; Enable PAE
    mov            eax, cr4
    bts            eax, 5
    mov            cr4, eax

    ; Load page table
    mov            eax, [esi + AP_DATA_STRUCT_FIELD (Cr3)]
    mov            cr3, eax                    ; Load CR3

    ; Enable long mode
    mov            ecx, 0c0000080h             ; EFER MSR number
    rdmsr                                      ; Read EFER
    bts            eax, 8                      ; Set LME=1
    wrmsr                                      ; Write EFER

    ; Enable paging
    mov            eax, cr0                    ; Read CR0
    bts            eax, 31                     ; Set PG=1
    mov            cr0, eax                    ; Write CR0

    ;
    ; Change to long mode
    ;
    retf

BITS 64
LongModeStart:
    ; Call C Function
    mov            eax, [esi + AP_DATA_STRUCT_FIELD (CProcedure)]
    test           eax, eax
    jz             GoToSleep

    mov            eax, [esi + AP_DATA_STRUCT_FIELD (CProcedure)]
    mov            edx, [esi + AP_DATA_STRUCT_FIELD (MpDataStruct)]
    mov            ecx, ebx
    call           rax

    wbinvd

GoToSleep64:
    cli
    hlt
    jmp            $-2



BITS 16
global ASM_PFX(mDefaultSmiHandlerStart)
global ASM_PFX(mDefaultSmiHandlerRet)
global ASM_PFX(mDefaultSmiHandlerEnd)
ASM_PFX(mDefaultSmiHandlerStart):
    ; Enable valid bit in SMRR mask
    mov            ecx, 01f3h
    rdmsr
    cmp            eax, 0
    jz             ASM_PFX(mDefaultSmiHandlerRet)
    bts            eax, 11
    jc             ASM_PFX(mDefaultSmiHandlerRet)
    wrmsr
ASM_PFX(mDefaultSmiHandlerRet):
    rsm
ASM_PFX(mDefaultSmiHandlerEnd):
    jmp            $

align  16
global ASM_PFX(RendezvousFunnelProcEnd)
ASM_PFX(RendezvousFunnelProcEnd):

MpDataAreaStart:
    buffer         times AP_DATA_STRUCT_FIELD(Terminator)   db  0
MpDataAreaEnd:

%if AP_DATA_STRUCT_OFFSET > 0x8000   ;AP_BUFFER_SIZE
%error "MP waking up stub combined code & data size should be less than 32KB !"
%endif

