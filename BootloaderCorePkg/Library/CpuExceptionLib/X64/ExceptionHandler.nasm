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

extern ASM_PFX(CommonExceptionHandler)


SECTION .text

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
    mov     rcx, rsp
    mov     rdx, [rsp]
    call    ASM_PFX(CommonExceptionHandler)
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
    mov     rax, AsmIdtVectorBegin
    mov     [rcx], rax
    mov     rax, (AsmIdtVectorEnd - AsmIdtVectorBegin) / 32
    mov     [rcx + 8], rax
    ret
