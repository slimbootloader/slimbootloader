;------------------------------------------------------------------------------
;
; Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   EnablePaging32.Asm
;
; Abstract:
;
;   AsmEnablePaging32 function
;
; Notes:
;
;------------------------------------------------------------------------------

    SECTION .text

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; EnablePaging32 (
;   IN      UINT32     Pdpe
;   );
;------------------------------------------------------------------------------
global ASM_PFX(EnablePaging32)
ASM_PFX(EnablePaging32):
    pushfd
    cli

    ; Set Page Global Enable (Set PGE in CR4)
    mov     eax, cr4
    or      eax, 10h
    mov     cr4, eax

    ; Load PDPE
    mov     eax, [esp + 8]
    mov     cr3, eax

    ; Enable Paging (Set PG bit in CR0)
    mov     eax, cr0
    bts     eax, 31
    mov     cr0, eax

    popfd
    ret

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; DisablePaging32 (
;   VOID
;   );
;------------------------------------------------------------------------------
global ASM_PFX(DisablePaging32)
ASM_PFX(DisablePaging32):
    pushfd
    cli

    ; Disable Paging (Clear PG bit in CR0)
    mov     eax, cr0
    and     eax, 0x7fffffff
    mov     cr0, eax

    popfd
    ret
