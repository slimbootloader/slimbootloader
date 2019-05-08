;------------------------------------------------------------------------------
;
; Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
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
;   CopyMem.Asm
;
; Abstract:
;
;   CopyMem function
;
; Notes:
;
;------------------------------------------------------------------------------

    SECTION .text

;------------------------------------------------------------------------------
;  VOID *
;  InternalMemCopyMem (
;    IN VOID   *Destination,
;    IN VOID   *Source,
;    IN UINTN  Count
;    )
;------------------------------------------------------------------------------
global ASM_PFX(InternalMemCopyMem)
ASM_PFX(InternalMemCopyMem):
    push    esi
    push    edi
    mov     esi, [esp + 16]             ; esi <- Source
    mov     edi, [esp + 12]             ; edi <- Destination
    mov     edx, [esp + 20]             ; edx <- Count
    shr     edx, 7
    jz      @CopyRest
@Copy128:
    movdqu  xmm0, [esi + 0 ]
    movdqu  xmm1, [esi + 16]
    movdqu  xmm2, [esi + 32]
    movdqu  xmm3, [esi + 48]
    movdqu  xmm4, [esi + 64]
    movdqu  xmm5, [esi + 80]
    movdqu  xmm6, [esi + 96]
    movdqu  xmm7, [esi + 112]
    movdqu  [edi + 0 ], xmm0
    movdqu  [edi + 16], xmm1
    movdqu  [edi + 32], xmm2
    movdqu  [edi + 48], xmm3
    movdqu  [edi + 64], xmm4
    movdqu  [edi + 80], xmm5
    movdqu  [edi + 96], xmm6
    movdqu  [edi + 112], xmm7
    add     esi, 128
    add     edi, 128
    dec     edx
    jnz     @Copy128                    ; Copy as many 128-byte blocks as possible
@CopyRest:
    mov     edx, [esp + 20]
    and     edx, 7Fh
    jz      @End
    lea     eax, [esi + edx - 1]        ; eax <- End of Source
    cmp     esi, edi
    jae     .0
    cmp     eax, edi
    jae     @CopyBackward               ; Copy backward if overlapped
.0:
    mov     ecx, edx
    and     edx, 3
    shr     ecx, 2
    rep     movsd                       ; Copy as many Dwords as possible
    jmp     @CopyBytes
@CopyBackward:
    mov     esi, eax                    ; esi <- End of Source
    lea     edi, [edi + edx - 1]        ; edi <- End of Destination
    std
@CopyBytes:
    mov     ecx, edx
    rep     movsb                       ; Copy bytes backward
    cld
@End:
    mov     eax, [esp + 12]             ; eax <- Destination as return value
    pop     edi
    pop     esi
    ret
