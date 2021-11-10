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

    DEFAULT REL
    SECTION .text
;---------------------

extern ASM_PFX(Execute32BitCode)

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
    push  rbp
    mov   rbp, rsp
    mov   rsp, r9      ; Set new stack
    mov   r9,  0       ; 4th parameter for Execute32BitCode
    call  ASM_PFX(Execute32BitCode)
    mov   rsp, rbp     ; Restore old stack
    pop   rbp
    ret
