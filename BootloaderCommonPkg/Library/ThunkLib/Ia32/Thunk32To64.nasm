;
; Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;
; Module Name:
;
;    Thunk32To64To.nasm
;
; Abstract:
;
;   This is the assembly code to transition from compatibility mode to long mode
;   to execute 64-bit code and then transit back to compatibility mode.
;
;-------------------------------------------------------------------------------
    DEFAULT REL
    SECTION .text

;----------------------------------------------------------------------------
; Procedure:    AsmExecute64BitCode
;
; Input:        None
;
; Output:       None
;
; Prototype:    UINT32
;               AsmExecute64BitCode (
;                 IN UINT64           Function,
;                 IN UINT64           Param1,
;                 IN UINT64           Param2,
;                 IN UINT64           GdtrPtr
;                 );
;
;
; Description:  A thunk function to execute 64-bit code.
;
;----------------------------------------------------------------------------
global ASM_PFX(AsmExecute64BitCode)
ASM_PFX(AsmExecute64BitCode):
    push    ebp
    mov     ebp, esp
    push    ebx

    sub     esp, 8
    sidt    [esp]                       ; save IDT
    sub     esp, 8
    sgdt    [esp]                       ; save GDT

    cli
    push    0x38                        ; seg for far retf
    push    LongModeStart               ; off for far retf
    mov     eax, cr4
    or      al, (1 << 5)
    mov     cr4, eax                    ; enable PAE
    mov     ecx, 0xc0000080
    rdmsr
    or      ah, 1                       ; set LME
    wrmsr
    mov     eax, cr0
    bts     eax, 31                     ; set PG
    mov     cr0, eax                    ; enable paging
    retf                                ; topmost 2 dwords hold the address

LongModeStart:
    DB      0x67
    mov     eax, [ebp + 8]              ; function address
    DB      0x67
    mov     ecx, [ebp + 16]             ; arg1
    DB      0x67
    mov     edx, [ebp + 24]             ; arg2
    mov     ebp, esp                    ; save esp
    DB      0x48
    add     esp, -0x20                  ; add rsp, -20h
    DB      0x48
    and     esp, -0x10                  ; align to 16
    call    eax                         ; call rbx
    DB      0x48
    mov     ebx, eax                    ; convert status to 32bit
    DB      0x48
    shr     ebx, 32
    or      ebx, eax
    mov     eax, 0x10                   ; load compatible mode code selector
    DB      0x48
    shl     eax, 32
    mov     edx, Compatible             ; assume address < 4G
    DB      0x48
    or      eax, edx
    push    eax
    retf

Compatible:
    ; Reload DS/ES/SS to make sure they are correct referred to current GDT
    mov     ax, 0x18                    ; load compatible mode data selector
    mov     ds, ax
    mov     es, ax
    mov     ss, ax

    ; Disable paging
    mov     eax, cr0
    btc     eax, 31
    mov     cr0, eax

    ; Clear EFER.LME
    mov     ecx, 0xC0000080
    rdmsr
    btc     eax, 8
    wrmsr

    ; clear CR4 PAE
    mov     eax, cr4
    btc     eax, 5
    mov     cr4, eax

    ; Restore stack pointer
    mov     esp, ebp

    ; Restore GDT/IDT
    lgdt    [esp]
    add     esp, 8
    lidt    [esp]
    add     esp, 8

    mov     eax, ebx
    pop     ebx
    pop     ebp
    ret
