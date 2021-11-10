;; @file
;   This is the assembly code for transferring to control to OS S3 waking vector
;   for IA32 platform
;
; Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
;
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;;

SECTION .text

global ASM_PFX(WakeUpBuffer)
ASM_PFX(WakeUpBuffer):
    dd    600h

global ASM_PFX(WakeUp)
ASM_PFX(WakeUp):
    push  ebp
    mov   ebp, esp
    mov   edx, dword [ASM_PFX(WakeUpBuffer)]
    lea   eax, [jmp_retf - ASM_PFX(WakeUp) + edx ]
    push  28h               ; CS
    push  eax
    mov   ecx, [ebp + 8]
    shrd  ebx, ecx, 20
    and   ecx, 0fh
    mov   bx, cx
    lea   eax, [os_jmp_addr - ASM_PFX(WakeUp) + edx ]
    mov   [eax], ebx
    retf
jmp_retf:
    db    0b8h, 30h, 0      ; mov ax, 30h as selector
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax
    mov   eax, cr0          ; Get control register 0
    db    66h
    db    83h, 0e0h, 0feh   ; and    eax, 0fffffffeh  ; Clear PE bit (bit #0)
    db    0fh, 22h, 0c0h    ; mov    cr0, eax         ; Activate real mode
    db    0eah              ; jmp far @jmp_addr
os_jmp_addr   dd  0

global ASM_PFX(WakeUpSize)
ASM_PFX(WakeUpSize):
    dd    $ - ASM_PFX(WakeUp)