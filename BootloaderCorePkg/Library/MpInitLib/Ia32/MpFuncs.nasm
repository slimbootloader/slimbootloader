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
;   MpFuncs.asm
; 
; Abstract:
; 
;   This is the assembly code for initializing the APs in IA32 Protected Mode
;
;-------------------------------------------------------------------------------
SECTION .text

;-------------------------------------------------------------------------------------
;RendezvousFunnelProc  procedure follows. All APs execute their procedure. This
;procedure does NOT serialize the AP processors. So this procedure cannot be the
;vector for a boradcast IPI. 
; 
;It must be noted that APs arrive here very raw...ie: real mode, no stack.
;-------------------------------------------------------------------------------------
SPIN_LOCK_RELEASED   EQU   0
SPIN_LOCK_ACQUIRED   EQU   1

 
global ASM_PFX(RendezvousFunnelProc)
ASM_PFX(RendezvousFunnelProc):
RendezvousFunnelProcStart:
; At this point CS = 0x(vv00) and ip= 0x0.
  
  db 8ch,  0c8h                 ; mov        ax,  cs
  db 8eh,  0d8h                 ; mov        ds,  ax
  db 8eh,  0c0h                 ; mov        es,  ax
  db 8eh,  0d0h                 ; mov        ss,  ax 
  db 33h,  0c0h                 ; xor        ax,  ax
  db 8eh,  0e0h                 ; mov        fs,  ax
  db 8eh,  0e8h                 ; mov        gs,  ax

  db 0BEh
  dw BufferStartLocation        ; mov        si, BufferStartLocation
  db 66h,  8Bh, 14h             ; mov        edx,dword ptr [si]          ; EDX is keeping the start address of wakeup buffer

  ; Since CS and DS point to the start of the buffer, we can just use the offset in SI
  
  db 0BEh
  dw GdtrLocation               ; mov        si, GdtrProfile
  db 66h                        ; db         66h
  db 2Eh,  0Fh, 01h, 14h        ; lgdt       fword ptr cs:[si]

  db 0BEh
  dw IdtrLocation               ; mov        si, IdtrProfile
  db 66h                        ; db         66h
  db 2Eh,  0Fh, 01h, 1Ch        ; lidt       fword ptr cs:[si]
        
  db 0Fh,  20h, 0C0h            ; mov        eax, cr0                    ; Get control register 0
  db 66h,  83h, 0C8h, 03h       ; or         eax, 000000003h             ; Set PE bit (bit #0) and MP
  db 0Fh,  22h, 0C0h            ; mov        cr0, eax

FLAT32_JUMP:

  db 66h, 67h, 0EAh             ; far jump
  dd 0h                         ; 32-bit offset
  dw 0h                         ; 16-bit selector

ProtModeStart:                 ; protected mode entry point

  ; Since CS and DS base is set to 0, we need to add the buffer start to use the offset

  mov        esi, edx           ; EDX is keeping the start address of wakeup buffer
  
  mov        ax, word [cs:esi + DSSelectorLocation]                      
  mov		     ds, ax

  mov        ax, word [cs:esi + ESSelectorLocation]
  mov	    	 es, ax

  mov        ax, word [cs:esi + FSSelectorLocation]
  mov		     fs, ax

  mov        ax, word [cs:esi + GSSelectorLocation]
  mov		     gs, ax

  mov        ax, word [cs:esi + SSSelectorLocation]
  mov		     ss, ax

  cli

  mov        eax, cr0           ; Enable cache
  and        eax, 09fffffffh
  mov        cr0, eax

  mov        eax, cr4           ; ENABLE_SSE
  or         eax, 00000600h
  mov        cr4, eax
  
CallApFunc:
  ;
  ; Acquire Lock
  ;
  mov            eax, SPIN_LOCK_RELEASED
  mov            edx, SPIN_LOCK_ACQUIRED
  lock cmpxchg   dword [esi + SpinLockLocation], edx
  jnz            CallApFunc
  
  ;
  ; Calculate stack
  ;  
  inc            dword [esi + ApCounterLocation]
  mov            eax, dword [esi + ApCounterLocation]
  mov            ebx, eax
  
  ; 
  ; Program AP stack for each thread
  ;    
  mov            ecx, dword [esi + ApStackSizeLocation]  
  shl            eax, cl
  add            eax, dword [esi + StackStartLocation]  
  lea            esp, [eax - 4]
  
  ;
  ; Release Lock
  ;
  mov            eax, SPIN_LOCK_RELEASED
  mov            dword [esi + SpinLockLocation], eax

  ;
  ; Call C Function
  ;
  mov            eax, dword [esi + CProcedureLocation]
  test           eax, eax
  jz             GoToSleep
  
  mov            eax, dword [esi + MpDataStructureLocation]
  push           eax
  push           ebx
  
  mov            eax, dword [esi + CProcedureLocation]
  call           eax
  add            esp, 8
   
  wbinvd

GoToSleep:

  cli
  hlt
  jmp         $-2
        

RendezvousFunnelProcEnd:

MpDataAreaStart:
BufferStartLocation           equ        RendezvousFunnelProcEnd - RendezvousFunnelProcStart
dd		0

GdtrLocation                  equ        BufferStartLocation + 04h
db		0, 0, 0, 0, 0, 0, 0, 0, 0, 0

IdtrLocation                  equ        BufferStartLocation + 0Eh
db		0, 0, 0, 0, 0, 0, 0, 0, 0, 0

ReservedDDLocation            equ        BufferStartLocation + 18h 
dd    0

CSSelectorLocation            equ        BufferStartLocation + 1Ch 
dw		0

DSSelectorLocation            equ        BufferStartLocation + 1Eh 
dw		0

ESSelectorLocation            equ        BufferStartLocation + 20h 
dw		0

SSSelectorLocation            equ        BufferStartLocation + 22h 
dw		0

FSSelectorLocation            equ        BufferStartLocation + 24h 
dw		0

GSSelectorLocation            equ        BufferStartLocation + 26h 
dw		0

StackStartLocation            equ        BufferStartLocation + 28h 
dd		0

CProcedureLocation            equ        BufferStartLocation + 2Ch 
dd      0

SpinLockLocation              equ        BufferStartLocation + 30h 
dd      0

ApCounterLocation             equ        BufferStartLocation + 34h 
dd      0

ApStackSizeLocation           equ        BufferStartLocation + 38h 
dd      0

MpDataStructureLocation       equ        BufferStartLocation + 3Ch 
dd      0

MpDataAreaEnd:

;-------------------------------------------------------------------------------------
;  AsmGetAddressMap (&AddressMap);
;-------------------------------------------------------------------------------------
;typedef struct {
;  UINT8 *RendezvousFunnelAddress;
;  UINT32 CodeSize;
;  UINT32 MpDataSize;
;  UINT32 ProtModeStartOffset;
;  UINT32 ProtModeJmpPatchOffset;
;} MP_ASSEMBLY_ADDRESS_MAP;
global ASM_PFX(AsmGetHotAddCodeAddressMap)
ASM_PFX(AsmGetHotAddCodeAddressMap):
        mov         eax, dword [esp + 4]
        mov         dword [eax], RendezvousFunnelProcStart
        mov         dword [eax+4h], RendezvousFunnelProcEnd - RendezvousFunnelProcStart        
        mov         dword [eax+8h], MpDataAreaEnd - MpDataAreaStart
        mov         dword [eax+0ch], ProtModeStart - RendezvousFunnelProcStart
        mov         dword [eax+10h], FLAT32_JUMP - RendezvousFunnelProcStart
        ret

;-------------------------------------------------------------------------------------
;  AsmGetBspSelectors (&BspSelectors);
;-------------------------------------------------------------------------------------
;typedef struct {
;  UINT16 CSSelector;
;  UINT16 DSSelector;
;  UINT16 ESSelector;
;  UINT16 SSSelector;
;  UINT16 FSSelector;
;  UINT16 GSSelector;
;  UINT   Gdtr;
;  UINT   Idtr;
;} MP_BSP_SELECTORS;
global ASM_PFX(AsmGetBspSelectors)
ASM_PFX(AsmGetBspSelectors):
        push        edi
        mov         edi, dword [esp + 8]
        mov	        ax, cs
        mov         word [edi + 0], ax
        mov	        ax, ds
        mov         word [edi + 2], ax
        mov	        ax, es
        mov         word [edi + 4], ax
        mov	        ax, ds
        mov         word [edi + 6], ax
        mov	        ax, fs
        mov         word [edi + 8], ax
        mov	        ax, gs
        mov         word [edi + 10], ax
        pop         edi
        ret
        


global ASM_PFX(AsmCliHlt)
ASM_PFX(AsmCliHlt):
        cli
        hlt
        jmp         $-2
        


global ASM_PFX(AsmMtrrSynchUpEntry)
ASM_PFX(AsmMtrrSynchUpEntry):
    push eax
    
    ;
    ; Disable Cache in CR0
    ;
    mov     eax, cr0
    bts     eax, 30
    btr     eax, 29
    mov     cr0, eax
    
    ;
    ; Flush cache
    ;
    wbinvd
    
    ;
    ; Flush all TLBs
    ;
    mov eax, cr3
    mov cr3, eax
    
    pop eax

    ret


global ASM_PFX(AsmMtrrSynchUpExit)
ASM_PFX(AsmMtrrSynchUpExit):
    push eax
    
    ;
    ; Flush all TLBs the second time
    ;
    mov eax, cr3
    mov cr3, eax
    
    ;
    ; Enable Normal Mode caching CD=NW=0, CD(Bit30), NW(Bit29)
    ;
    mov eax, cr0
    and eax, 09FFFFFFFh
    mov cr0, eax
    
    pop eax
    ret

