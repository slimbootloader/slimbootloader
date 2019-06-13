;------------------------------------------------------------------------------ ;
; Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   ExceptionHandler.Asm
;
; Abstract:
;
;   IA32 CPU Exception Handler
;
; Notes:
;
;------------------------------------------------------------------------------

SECTION .text

extern ASM_PFX(CommonExceptionHandler)

;
; exception handler stub table
;
ALIGN   4
AsmIdtVectorBegin:
%rep  32
    db      0x6a        ; push  #VectorNum
    db      ($ - AsmIdtVectorBegin) / ((AsmIdtVectorEnd - AsmIdtVectorBegin) / 32) ; VectorNum
    jmp     ASM_PFX(CommonInterruptEntry)
%endrep
AsmIdtVectorEnd:

;----------------------------------------------------------------------------;
; CommonInterruptEntry                                                       ;
;----------------------------------------------------------------------------;
; The follow algorithm is used for the common interrupt routine.
; Stack:
; +---------------------+
; +    EFlags           +
; +---------------------+
; +    CS               +
; +---------------------+
; +    EIP              +
; +---------------------+
; +    Error Code       +
; +---------------------+
; +    Vector Number    +
; +---------------------+
global ASM_PFX(CommonInterruptEntry)
ASM_PFX(CommonInterruptEntry):
    cli
    push    esp
    call    ASM_PFX(CommonExceptionHandler)
    add     esp, 8
    jmp     $

;----------------------------------------------------------------------------;
; _AsmGetTemplateAddressMap                                                  ;
;----------------------------------------------------------------------------;
;
; VOID
; AsmGetTemplateAddressMap (
;     EXCEPTION_HANDLER_TEMPLATE_MAP *AddressMap
;   );
;
; Routine Description:
;
;  Return address map of interrupt handler template so that C code can generate
;  interrupt table.
;
; Arguments:
;  AddressMap:  Address of EXCEPTION_HANDLER_TEMPLATE_MAP structure
;
; Returns:
;   Nothing
;-----------------------------------------------------------------------------;
global ASM_PFX(AsmGetTemplateAddressMap)
ASM_PFX(AsmGetTemplateAddressMap):
    push    ebp
    mov     ebp, esp
    mov     eax, dword [ebp + 0x8]
    mov     dword [eax], AsmIdtVectorBegin
    mov     dword [eax + 4], (AsmIdtVectorEnd - AsmIdtVectorBegin) / 32
    pop     ebp
    ret
