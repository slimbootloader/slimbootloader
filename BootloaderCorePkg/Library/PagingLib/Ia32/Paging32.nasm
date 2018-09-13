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

;------------------------------------------------------------------------------
; VOID *
; EFIAPI
; FlushCacheLine (
;   IN      VOID                      *StartAddress,
;   IN      VOID                      *EndAddress
;   );
;------------------------------------------------------------------------------
global ASM_PFX(FlushCacheLine)
ASM_PFX(FlushCacheLine):
    ;
    ; If the CPU does not support CLFLUSH instruction, 
    ; then promote flush range to flush entire cache.
    ;    
    push    ecx
    mov     eax, [esp +  8]    
    mov     ecx, [esp + 12]    
    sub     ecx, eax
    shr     ecx, 6
FlushNext:
    clflush [eax]
    add     eax, 64
    loop    FlushNext
    pop     ecx    
    ret

