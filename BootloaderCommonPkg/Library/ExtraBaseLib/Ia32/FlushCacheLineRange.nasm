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

