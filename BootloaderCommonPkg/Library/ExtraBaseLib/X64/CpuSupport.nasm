;------------------------------------------------------------------------------
;
; Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   FlushCacheLineRange.Asm
;
; Abstract:
;
;   FlushCacheLineRange function
;
; Notes:
;
;------------------------------------------------------------------------------

    SECTION .text

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; AsmFlushCacheRange (
;   IN      VOID                      *StartAddress,
;   IN      UINT32                     Length
;   );
;------------------------------------------------------------------------------
global ASM_PFX(AsmFlushCacheRange)
ASM_PFX(AsmFlushCacheRange):
    mov     rax, rcx
    mov     rcx, rdx
    cmp     rcx, 0
    jz      FlushExit
    add     rcx, rax
    dec     rcx
    and     rax, 0xffffffc0
    or      rcx, 0x3f
    sub     rcx, rax
    shr     rcx, 6
    inc     rcx
FlushNext:
    clflush [rax]
    add     rax, 64
    loop    FlushNext
FlushExit:
    ret


;------------------------------------------------------------------------------
; VOID
; EFIAPI
; AsmEnableAvx (
;   VOID
;   );
;------------------------------------------------------------------------------
global ASM_PFX(AsmEnableAvx)
ASM_PFX(AsmEnableAvx):
    push    rbx
    ;
    ; Check if AVX is supported
    ;
    mov     rax, 1
    cpuid
    and     rcx, 014000000h
    cmp     rcx, 014000000h       ; check both XSAVE and AVX feature flags
    jne     NoAvxSupport

    ;
    ; Enable AVX
    ;
    mov     rax, cr4
    or      rax, 00040000h
    mov     cr4, rax
    xor     rcx, rcx              ; XFEATURE_ENABLED_MASK register
    xor     rdx, rdx
    xor     rax, rax
    mov     ax,  7h               ; mask in edx:eax
    xsetbv

NoAvxSupport:
    pop     rbx
    ret
