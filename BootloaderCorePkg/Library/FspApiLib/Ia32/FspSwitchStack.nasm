;------------------------------------------------------------------------------
;
; Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;
; Module Name:
;
;  FspSwitchStack.nasm
;
; Abstract:
;
;  This is the code that will call into FspMemoryInit API with a new stack.
;
;------------------------------------------------------------------------------

SECTION .text

;------------------------------------------------------------------------------
; EFI_STATUS
; EFIAPI
; FspmSwitchStack (
;   IN VOID        *EntryPoint,
;   IN VOID        *Context1,   OPTIONAL
;   IN VOID        *Context2,   OPTIONAL
;   IN VOID        *NewStack
;   );
;------------------------------------------------------------------------------
global ASM_PFX(FspmSwitchStack)
ASM_PFX(FspmSwitchStack):
    push  ebp
    mov   ebp, esp
    mov   esp, [ebp + 20]    ; switch stack
    sub   esp, 8
    mov   eax, [ebp + 16]
    mov   [esp + 4], eax     ; prepare 2nd parameter
    mov   eax, [ebp + 12]
    mov   [esp], eax         ; prepare 1st parameter
    call  dword [ebp + 8]
    mov   esp, ebp
    pop   ebp
    ret
