;; @file
;   This is the assembly code for transferring to control to OS S3 waking vector
;   for IA32 platform
;
; Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
;
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;;

DEFAULT REL
SECTION .text

global ASM_PFX(WakeUpBuffer)
ASM_PFX(WakeUpBuffer):
    dd    600h

global ASM_PFX(WakeUp)
ASM_PFX(WakeUp):
    ; rcx S3WakingVector    :DWORD
    lea   eax, [.0]
    mov   r8, 0x2800000000
    or    rax, r8
    push  rax
    shrd  ebx, ecx, 20
    and   ecx, 0xf
    mov   bx, cx
    mov   [@jmp_addr + 1], ebx
    retf
BITS 16
.0:
    mov   ax, 0x30
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax
    mov   eax, cr0
    mov   ebx, cr4
    and   eax, ((~ 0x80000001) & 0xffffffff)
    and   bl, ~ (1 << 5)
    mov   cr0, eax
    mov   ecx, 0xc0000080
    rdmsr
    and   ah, ~ 1
    wrmsr
    mov   cr4, ebx
@jmp_addr:
    jmp   0x0:0x0

global ASM_PFX(WakeUpSize)
ASM_PFX(WakeUpSize):
    dd    $ - ASM_PFX(WakeUp)
