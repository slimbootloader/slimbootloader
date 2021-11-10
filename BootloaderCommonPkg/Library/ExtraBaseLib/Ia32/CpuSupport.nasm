;------------------------------------------------------------------------------
;
; Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
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
    push    ecx
    mov     ecx, [esp + 12]
    cmp     ecx, 0
    jz      FlushExit
    mov     eax, [esp +  8]
    add     ecx, eax
    dec     ecx
    and     eax, 0xffffffc0
    or      ecx, 0x3f
    sub     ecx, eax
    shr     ecx, 6
    inc     ecx
FlushNext:
    clflush [eax]
    add     eax, 64
    loop    FlushNext
FlushExit:
    pop     ecx
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
    push    ebx
    ;
    ; Check if AVX is supported
    ;
    mov     eax, 1
    cpuid
    and     ecx, 014000000h
    cmp     ecx, 014000000h       ; check both XSAVE and AVX feature flags
    jne     NoAvxSupport

    ;
    ; Enable AVX
    ;
    mov     eax, cr4
    or      eax, 00040000h
    mov     cr4, eax
    xor     ecx, ecx              ; XFEATURE_ENABLED_MASK register
    xor     edx, edx
    xor     eax, eax
    mov     ax,  7h               ; mask in edx:eax
    xsetbv

NoAvxSupport:
    pop     ebx
    ret
