;------------------------------------------------------------------------------
;
; Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   MpFuncs.asm
;
; Abstract:
;
;   This is the assembly code for initializing the APs in IA32 Protected Mode
;
;-------------------------------------------------------------------------------
DEFAULT  REL
SECTION .text

;-------------------------------------------------------------------------------------
;RendezvousFunnelProc  procedure follows. All APs execute their procedure. This
;procedure does NOT serialize the AP processors. So this procedure cannot be the
;vector for a boradcast IPI.
;
;It must be noted that APs arrive here very raw...ie: real mode, no stack.
;-------------------------------------------------------------------------------------
SPIN_LOCK_RELEASED   EQU   0
SPIN_LOCK_ACQUIRED   EQU   1


global ASM_PFX(RendezvousFunnelProc)
ASM_PFX(RendezvousFunnelProc):
RendezvousFunnelProcStart:
; At this point CS = 0x(vv00) and ip= 0x0.
BITS 16

    mov            eax, cr2
    or             eax, eax

    ; For SMM rebase flow, CR2 will be pre-filled with SMBASE
    ; The same stub code will be shared for SMM rebasing and start IPI.
    jz             StartIpi

    ; The ASM code needs to use same 0x3800 segment as start IPI.
    ; At initial SMM entry the segment is 0x3000, so force a long jump.
    jmp            0x3800:(StartIpi - RendezvousFunnelProcStart)

StartIpi:
    mov            ax,  cs
    mov            ds,  ax
    mov            es,  ax
    mov            ss,  ax
    xor            ax,  ax
    mov            fs,  ax
    mov            gs,  ax

    mov            si, BufferStartLocation
    mov            edx, dword [si]    ; EDX is keeping the start address of wakeup buffer

    mov            si, GdtrLocation
o32 lgdt           [si]

    mov            si, IdtrLocation
o32 lidt           [si]

    ; Switch to protected mode
    mov            eax, cr0           ; Get control register 0
    or             eax, 000000003h    ; Set PE bit (bit #0) & MP
    mov            cr0, eax

FLAT32_JUMP:
    db             66h, 67h, 0EAh     ; far jump
    dd             0h                 ; 32-bit offset
    dw             0x10               ; 16-bit selector

BITS 32
SmmRebase:
    mov            edi,  0x3fef8     ; SMBASE offset
    mov            [edi], eax        ; change to new  SMBASE

    mov            eax, dword [esi + SmrrMaskLocation]
    cmp            eax, 0
    jz             SkipSmrrProg

    xchg           eax, ebx
    mov            eax, dword [esi + SmrrBaseLocation]
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
    mov            ax, 0x08
    mov            ds, ax
    mov            es, ax
    mov            fs, ax
    mov            gs, ax
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
    lock cmpxchg   dword [esi + SpinLockLocation], edx
    jnz            CallApFunc

    ; Calculate stack
    inc            dword [esi + ApCounterLocation]
    mov            eax, dword [esi + ApCounterLocation]
    mov            ebx, eax

    ; Program AP stack for each thread
    mov            ecx, dword [esi + ApStackSizeLocation]
    shl            eax, cl
    add            eax, dword [esi + StackStartLocation]
    lea            esp, [eax - 0x10]

    ; Release Lock
    mov            eax, SPIN_LOCK_RELEASED
    mov            dword [esi + SpinLockLocation], eax

    ; Prepare stack for RETF
    xor            eax, eax
    mov            ax, word [esi + CSSelectorLocation]
    push           eax
    lea            eax, [LongModeStart - RendezvousFunnelProcStart]
    add            eax, esi
    push           eax

    ; Enable PAE
    mov            eax, cr4
    bts            eax, 5
    mov            cr4, eax

    ; Load page table
    mov            eax, [esi + Cr3Location]
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
    mov            eax, dword [esi + CProcedureLocation]
    test           eax, eax
    jz             GoToSleep

    mov            eax, dword [esi + CProcedureLocation]
    mov            edx, dword [esi + MpDataStructureLocation]
    mov            ecx, ebx
    call           rax

    wbinvd

GoToSleep:
    cli
    hlt
    jmp            $-2

RendezvousFunnelProcEnd:

MpDataAreaStart:
BufferStartLocation           equ        RendezvousFunnelProcEnd - RendezvousFunnelProcStart
dd    0

GdtrLocation                  equ        BufferStartLocation + 04h
db    0, 0, 0, 0, 0, 0, 0, 0, 0, 0

IdtrLocation                  equ        BufferStartLocation + 0Eh
db    0, 0, 0, 0, 0, 0, 0, 0, 0, 0

ReservedDDLocation            equ        BufferStartLocation + 18h
dd    0

CSSelectorLocation            equ        BufferStartLocation + 1Ch
dw    0

DSSelectorLocation            equ        BufferStartLocation + 1Eh
dw    0

ESSelectorLocation            equ        BufferStartLocation + 20h
dw    0

SSSelectorLocation            equ        BufferStartLocation + 22h
dw    0

FSSelectorLocation            equ        BufferStartLocation + 24h
dw    0

GSSelectorLocation            equ        BufferStartLocation + 26h
dw    0

StackStartLocation            equ        BufferStartLocation + 28h
dd    0

CProcedureLocation            equ        BufferStartLocation + 2Ch
dd    0

SpinLockLocation              equ        BufferStartLocation + 30h
dd    0

ApCounterLocation             equ        BufferStartLocation + 34h
dd      0

ApStackSizeLocation           equ        BufferStartLocation + 38h
dd      0

MpDataStructureLocation       equ        BufferStartLocation + 3Ch
dd      0

Cr3Location                   equ        BufferStartLocation + 40h
dd      0

SmrrBaseLocation              equ        BufferStartLocation + 44h
dd      0

SmrrMaskLocation              equ        BufferStartLocation + 48h
dd      0

MpDataAreaEnd:

%if (MpDataAreaEnd - RendezvousFunnelProcStart) > 0x8000   ;AP_BUFFER_SIZE
%error "MP waking up stub combined code & data size should be less than 32KB !"
%endif


BITS 64

;-------------------------------------------------------------------------------------
;  AsmGetAddressMap (&AddressMap);
;-------------------------------------------------------------------------------------
;typedef struct {
;  UINT8  *RendezvousFunnelAddress;
;  UINT32 CodeSize;
;  UINT32 MpDataSize;
;  UINT32 ProtModeStartOffset;
;  UINT32 ProtModeJmpPatchOffset;
;} MP_ASSEMBLY_ADDRESS_MAP;
global ASM_PFX(AsmGetAddressMap)
ASM_PFX(AsmGetAddressMap):
    lea     rax,  [ASM_PFX(RendezvousFunnelProc)]
    mov     qword [rcx],      rax
    mov     dword [rcx+0x08], RendezvousFunnelProcEnd - RendezvousFunnelProcStart
    mov     dword [rcx+0x0C], MpDataAreaEnd - MpDataAreaStart
    mov     dword [rcx+0x10], ProtModeStart - RendezvousFunnelProcStart
    mov     dword [rcx+0x14], FLAT32_JUMP - RendezvousFunnelProcStart
    ret

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
    mov     ax, cs
    mov     [rcx + 0], ax
    mov     ax, ds
    mov     [rcx + 2], ax
    mov     ax, es
    mov     [rcx + 4], ax
    mov     ax, ds
    mov     [rcx + 6], ax
    mov     ax, fs
    mov     [rcx + 8], ax
    mov     ax, gs
    mov     [rcx + 10], ax
    ret


global ASM_PFX(AsmCliHlt)
ASM_PFX(AsmCliHlt):
    cli
    hlt
    jmp     $-2


global ASM_PFX(AsmMtrrSynchUpEntry)
ASM_PFX(AsmMtrrSynchUpEntry):
    ;
    ; Disable Cache in CR0
    ;
    mov     rax, cr0
    bts     rax, 30
    btr     rax, 29
    mov     cr0, rax

    ;
    ; Flush cache
    ;
    wbinvd

    ;
    ; Flush all TLBs
    ;
    mov     rax, cr3
    mov     cr3, rax

    ret


global ASM_PFX(AsmMtrrSynchUpExit)
ASM_PFX(AsmMtrrSynchUpExit):
    ;
    ; Flush all TLBs the second time
    ;
    mov     rax, cr3
    mov     cr3, rax

    ;
    ; Enable Normal Mode caching CD=NW=0, CD(Bit30), NW(Bit29)
    ;
    mov     rax, cr0
    and     rax, 09FFFFFFFh
    mov     cr0, rax

    ret


BITS 16
global ASM_PFX(mDefaultSmiHandlerStart)
global ASM_PFX(mDefaultSmiHandlerRet)
global ASM_PFX(mDefaultSmiHandlerEnd)
ASM_PFX(mDefaultSmiHandlerStart):
    ; Enable valid bit in SMRR mask
    mov ecx, 01f3h
    rdmsr
    cmp eax, 0
    jz  ASM_PFX(mDefaultSmiHandlerRet)
    or  eax, (1<<11)
    wrmsr
ASM_PFX(mDefaultSmiHandlerRet):
    rsm
ASM_PFX(mDefaultSmiHandlerEnd):
    nop
