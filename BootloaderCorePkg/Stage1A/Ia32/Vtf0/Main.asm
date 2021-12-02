;------------------------------------------------------------------------------
; @file
; Main routine of the pre-SEC code up through the jump into SEC
;
; Copyright (c) 2008 - 2015, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;------------------------------------------------------------------------------

BITS    16

;
; Modified:  EBX, ECX, EDX, EBP
;
; @param[in,out]  RAX/EAX  Initial value of the EAX register
;                          (BIST: Built-in Self Test)
; @param[in,out]  DI       'BP': boot-strap processor, or
;                          'AP': application processor
; @param[out]     RBP/EBP  Address of Boot Firmware Volume (BFV)
;
; @return         None  This routine jumps to SEC and does not return
;
Main16:
    OneTimeCall EarlyInit16

    ;
    ; Transition the processor from 16-bit real mode to 32-bit flat mode
    ;
    OneTimeCall TransitionFromReal16To32BitFlat

BITS    32

%ifdef ARCH_IA32
    ;
    ; Get BFV Entrypoint
    ;
    mov     eax, 0FFFFFFFCh
    mov     eax, dword [eax]
    mov     esi, dword [eax]
    ; Restore the BIST value to EAX register
    movd    eax, mm0
    jmp     esi
%endif

%ifdef ARCH_X64
    jmp     Continue

align 16
    ;
    ; Below fields will be patched by post build process
    ;
BuildPatchData:
    DD      0x55AA0FF0 ; Pagee Table Offset
    DD      0x12345678 ; FSP-T Base
TempRamInitStack:
    DD      ADDR_OF(TempRamInitDone)
    DD      0x12345678 ; FSP-T UPD override

Continue:
    ;
    ; Read time stamp
    ;
    rdtsc
    mov     esi, eax
    mov     edi, edx

    ;
    ; Get FSP-T base in EAX
    ;
    mov     eax, ADDR_OF(BuildPatchData)
    mov     eax, dword [eax + 0x04]

    ;
    ; Find the fsp info header
    ; Jump to TempRamInit API
    ;
    add     eax, dword [eax + 094h + FSP_HEADER_TEMPRAMINIT_OFFSET]
    mov     esp, ADDR_OF(TempRamInitStack)
    jmp     eax


TempRamInitDone:
    cmp     eax, 8000000Eh      ;Check if EFI_NOT_FOUND returned. Error code for Microcode Update not found.
    je      FspApiSuccess       ;If microcode not found, don't hang, but continue.

    cmp     eax, 0              ;Check if EFI_SUCCESS returned.
    jz      FspApiSuccess

    ; FSP API failed:
    jmp     $

FspApiSuccess:
    ;
    ; FSP-T NEM returned range
    ;   ECX: NEM stack base
    ;   EDX: NEM stack top
    ;
    mov     ebp, ecx
    mov     esp, edx

    ;
    ; Save time stamp to stack
    ;
    push    esi
    push    edi

    ; Create page tables
    ;   ECX: Page base
    mov     eax, ADDR_OF(BuildPatchData)
    add     ecx, dword [eax + 0x00]
    OneTimeCall  PreparePagingTable

    ;
    ; Restore time stamp from stack
    ;
    pop     edi
    pop     esi

    ;
    ; Set CR3 now that the paging structures are available
    ;
    mov     cr3, eax

    mov     eax, cr4
    bts     eax, 5                      ; enable PAE
    mov     cr4, eax

    mov     ecx, 0xc0000080
    rdmsr
    bts     eax, 8                      ; set LME
    wrmsr

    mov     ecx, ebp                    ; restore ecx/edx
    mov     edx, esp

    mov     eax, cr0
    bts     eax, 31                     ; set PG
    mov     cr0, eax                    ; enable paging

    jmp     LINEAR_CODE64_SEL:ADDR_OF(jumpTo64BitAndLandHere)

BITS    64
jumpTo64BitAndLandHere:
    ; Set stack
    ;   ECX: NEM start
    ;   EDX: NEM end
    ;

    ;
    ; Get BFV Entrypoint
    ;
    xor     rax, rax
    mov     eax, 0FFFFFFFCh
    mov     eax, dword [rax]

    ;
    ; EDI/ESI time stamp to RDI
    ;
    shl     rdi, 32
    add     rdi, rsi

    ; Restore the BIST value to EAX register
    ;
    xor     rsi, rsi
    mov     esi, dword [rax]
    movd    eax, mm0

    jmp     rsi
%endif
