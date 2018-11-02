;------------------------------------------------------------------------------
;
; Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
; This program and the accompanying materials
; are licensed and made available under the terms and conditions of the BSD License
; which accompanies this distribution.  The full text of the license may be found at
; http://opensource.org/licenses/bsd-license.php.
;
; THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
; WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
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

  mov eax, cr2
  or  eax, eax

  ; For SMM rebase flow, CR2 will be pre-filled with SMBASE
  ; The same stub code will be shared for SMM rebasing and start IPI.
  jz StartIpi

  ; The ASM code needs to use same 0x3800 segment as start IPI.
  ; At initial SMM entry the segment is 0x3000, so force a long jump.
  jmp 0x3800:(StartIpi - RendezvousFunnelProcStart)

StartIpi:
    mov        ax,  cs
    mov        ds,  ax
    mov        es,  ax
    mov        ss,  ax
    xor        ax,  ax
    mov        fs,  ax
    mov        gs,  ax

    mov        si, BufferStartLocation
    mov        edx, dword [si]    ; EDX is keeping the start address of wakeup buffer

    mov        si, GdtrLocation
o32 lgdt       [cs:si]

    mov        si, IdtrLocation
o32 lidt       [cs:si]

    ; Switch to protected mode
    mov        eax, cr0           ; Get control register 0
    or         eax, 000000003h    ; Set PE bit (bit #0) & MP
    mov        cr0, eax

FLAT32_JUMP:

    db 66h, 67h, 0EAh             ; far jump
    dd 0h                         ; 32-bit offset
    dw 0h                         ; 16-bit selector

BITS 32
SmmRebase:
    mov         edi,  0x3fef8     ; SMBASE offset
    mov         dword [edi], eax  ; change to new  SMBASE
    mov         dword [eax + 0x8000], 0x9090AA0F  ; write 'RSM' at new SMM handler entry
    rsm
    jmp         $

ProtModeStart:                    ; protected mode entry point
    ; Since CS and DS base is set to 0, we need to add the buffer start to use the offset

    mov        esi, edx           ; EDX is keeping the start address of wakeup buffer

    mov        ax, word [cs:esi + DSSelectorLocation]
    mov        ds, ax

    mov        ax, word [cs:esi + ESSelectorLocation]
    mov        es, ax

    mov        ax, word [cs:esi + FSSelectorLocation]
    mov        fs, ax

    mov        ax, word [cs:esi + GSSelectorLocation]
    mov        gs, ax

    mov        ax, word [cs:esi + SSSelectorLocation]
    mov        ss, ax

    cli

    mov        eax, cr2           ; CR2 will be set to the new SMBASE before SMI IPI is generated
    or         eax, eax
    jnz        SmmRebase

    mov        eax, cr0           ; Enable cache
    and        eax, 09fffffffh
    mov        cr0, eax

    mov        eax, cr4           ; ENABLE_SSE
    or         eax, 00000600h
    mov        cr4, eax

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
    lea            esp, [eax - 4]

    ; Release Lock
    mov            eax, SPIN_LOCK_RELEASED
    mov            dword [esi + SpinLockLocation], eax

    ; Call C Function
    mov            eax, dword [esi + CProcedureLocation]
    test           eax, eax
    jz             GoToSleep

    mov            eax, dword [esi + MpDataStructureLocation]
    push           eax
    push           ebx

    mov            eax, dword [esi + CProcedureLocation]
    call           eax
    add            esp, 8

    wbinvd

GoToSleep:
    cli
    hlt
    jmp         $-2

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

MpDataAreaEnd:

%if (MpDataAreaEnd - RendezvousFunnelProcStart) > 0x8000   ;AP_BUFFER_SIZE
%error "MP waking up stub combined code & data size should be less than 32KB !"
%endif

;-------------------------------------------------------------------------------------
;  AsmGetAddressMap (&AddressMap);
;-------------------------------------------------------------------------------------
;typedef struct {
;  UINT8 *RendezvousFunnelAddress;
;  UINT32 CodeSize;
;  UINT32 MpDataSize;
;  UINT32 ProtModeStartOffset;
;  UINT32 ProtModeJmpPatchOffset;
;} MP_ASSEMBLY_ADDRESS_MAP;
global ASM_PFX(AsmGetAddressMap)
ASM_PFX(AsmGetAddressMap):
        mov         eax, dword [esp + 4]
        mov         dword [eax], RendezvousFunnelProcStart
        mov         dword [eax+4h], RendezvousFunnelProcEnd - RendezvousFunnelProcStart
        mov         dword [eax+8h], MpDataAreaEnd - MpDataAreaStart
        mov         dword [eax+0ch], ProtModeStart - RendezvousFunnelProcStart
        mov         dword [eax+10h], FLAT32_JUMP - RendezvousFunnelProcStart
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



global ASM_PFX(AsmMtrrSynchUpEntry)
ASM_PFX(AsmMtrrSynchUpEntry):
    push eax

    ;
    ; Disable Cache in CR0
    ;
    mov     eax, cr0
    bts     eax, 30
    btr     eax, 29
    mov     cr0, eax

    ;
    ; Flush cache
    ;
    wbinvd

    ;
    ; Flush all TLBs
    ;
    mov eax, cr3
    mov cr3, eax

    pop eax

    ret


global ASM_PFX(AsmMtrrSynchUpExit)
ASM_PFX(AsmMtrrSynchUpExit):
    push eax

    ;
    ; Flush all TLBs the second time
    ;
    mov eax, cr3
    mov cr3, eax

    ;
    ; Enable Normal Mode caching CD=NW=0, CD(Bit30), NW(Bit29)
    ;
    mov eax, cr0
    and eax, 09FFFFFFFh
    mov cr0, eax

    pop eax
    ret

