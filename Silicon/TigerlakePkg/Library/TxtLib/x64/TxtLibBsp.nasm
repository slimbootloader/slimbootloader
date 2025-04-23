;------------------------------------------------------------------------------
;
; Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   TxtLibBsp.nasm
;
; Abstract:
;
;   TXT Bsp function to call ACM function for TXT enablement
;
; Notes:
;
;------------------------------------------------------------------------------


DEFAULT REL
SECTION .text


;
; BIOS ACM functions
;
%define TXT_FN_SCLEAN        0x0
%define TXT_FN_RESET_EST_BIT 0x1
%define TXT_FN_RESET_AUX     0x2
%define TXT_FN_SCHECK        0x4
%define TXT_FN_CLEAR_SECRETS 0x5
%define TXT_FN_LOCK_CONFIG   0x6
%define TXT_FN_ACHECK        0x7
;
; Misc
;
%define ENTERACCS            0x2
%define PARAMETERS           0x6
%define ACMOD_SIZE           24
#define PORT80_CODE_PREFIX   0xA0
%define LONG_MODE_CODE_SEGMENT  0x10
;
; Instructions
;
%define _EMMS    db 0xf, 0x77
%define _GETSEC  db 0xf, 0x37
;
; MMX registers QWORD access
;
%define MOVDIN0  db 0x0f, 0x6e, 0xc0       ; movd mm0, eax
%define MOVQIN0  db 0x48, 0xf, 0x6e, 0xc0  ; movq mm0, rax
%define MOVDOUT0 db 0x0f, 0x7e, 0xc0       ; movd eax, mm0
%define MOVQOUT0 db 0x48, 0xf, 0x7e, 0xc0  ; movq rax, mm0
%define MOVQIN1  db 0x48, 0xf, 0x6e, 0xc8  ; movq mm1, rax
%define MOVQOUT1 db 0x48, 0xf, 0x7e, 0xc8  ; movq rax, mm1
%define MOVQIN2  db 0x48, 0xf, 0x6e, 0xd0  ; movq mm2, rax
%define MOVQOUT2 db 0x48, 0xf, 0x7e, 0xd0  ; movq rax, mm2
%define MOVQIN3  db 0x48, 0xf, 0x6e, 0xd8  ; movq mm3, rax
%define MOVQOUT3 db 0x48, 0xf, 0x7e, 0xd8  ; movq rax, mm3
%define MOVQIN4  db 0x48, 0xf, 0x6e, 0xe0  ; movq mm4, rax
%define MOVQOUT4 db 0x48, 0xf, 0x7e, 0xe0  ; movq rax, mm4
%define MOVQIN5  db 0x48, 0xf, 0x6e, 0xe8  ; movq mm5, rax
%define MOVQOUT5 db 0x48, 0xf, 0x7e, 0xe8  ; movq rax, mm5
%define MOVQIN6  db 0x48, 0xf, 0x6e, 0xf0  ; movq mm6, rax
%define MOVQOUT6 db 0x48, 0xf, 0x7e, 0xf0  ; movq rax, mm6
%define MOVQIN7  db 0x48, 0xf, 0x6e, 0xf8  ; movq mm7, rax
%define MOVQOUT7 db 0x48, 0xf, 0x7e, 0xf8  ; movq rax, mm7
;
; MSRs
;
%define IA32_MISC_ENABLE     0x1A0
%define IA32_EFER_MSR        0xC0000080
;
; MTRRs
;
%define IA32_MTRR_PHYSBASE0  0x200
%define IA32_MTRR_PHYSMASK0  0x201
%define IA32_MTRR_PHYSBASE1  0x202
%define IA32_MTRR_PHYSMASK1  0x203
%define IA32_MTRR_PHYSBASE2  0x204
%define IA32_MTRR_PHYSMASK2  0x205
%define IA32_MTRR_PHYSBASE3  0x206
%define IA32_MTRR_PHYSMASK3  0x207
%define IA32_MTRR_PHYSBASE4  0x208
%define IA32_MTRR_PHYSMASK4  0x209
%define IA32_MTRR_PHYSBASE5  0x20A
%define IA32_MTRR_PHYSMASK5  0x20B
%define IA32_MTRR_PHYSBASE6  0x20C
%define IA32_MTRR_PHYSMASK6  0x20D
%define IA32_MTRR_PHYSBASE7  0x20E
%define IA32_MTRR_PHYSMASK7  0x20F
%define IA32_MTRR_DEF_TYPE   0x2FF
%define IA32_MTRR_CAP        0xFE
;
; Only low order bits are assumed
;
%define MTRR_MASK            0xFFFFF000
%define MTRR_ENABLE          (1 << 11)
%define MTRR_FIXED_ENABLE    (1 << 10)
%define MTRR_VALID           (1 << 11)
%define UC                   0x0
%define WB                   0x6
;
; Control register bits
;
%define CR0_NE_MASK          (1 << 5)
%define CR0_NW_MASK          (1 << 29)
%define CR0_CD_MASK          (1 << 30)
%define CR4_DE               (1 << 3)
%define CR4_OSFXSR           (1 << 9)
%define CR4_SMXE             (1 << 14)
;
; Machine check architecture MSR registers
;
%define MCG_CAP 0x179
%define MC0_STATUS 0x401

%define ACM_SIZE_TO_CACHE    xmm0
%define ACM_BASE_TO_CACHE    xmm1
%define NEXT_MTRR_INDEX      xmm2
%define NEXT_MTRR_SIZE       xmm3
%define MTRR_PHYS_MASK_HIGH  xmm4

%define BIOS_GDT [rbp-0x10]
%define BIOS_IDT [rbp-0x20]


;-----------------------------------------------------------------------------
;  Macro:        PUSHA_64
;
;  Description:  Saves all registers on stack
;
;  Input:        None
;
;  Output:       None
;-----------------------------------------------------------------------------
%macro PUSHA_64 0
    push    rsp
    push    rbp
    push    rax
    push    rbx
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    push    r12
    push    r13
    push    r14
    push    r15
%endmacro

;-----------------------------------------------------------------------------
;  Macro:        POPA_64
;
;  Description:  Restores all registers from stack
;
;  Input:        None
;
;  Output:       None
;-----------------------------------------------------------------------------
%macro POPA_64 0
    pop    r15
    pop    r14
    pop    r13
    pop    r12
    pop    r11
    pop    r10
    pop    r9
    pop    r8
    pop    rdi
    pop    rsi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    pop    rbp
    pop    rsp
%endmacro


;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;
;  DoPowerCycleReset
;
;  @param[in]   None
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
global ASM_PFX(DoPowerCycleReset)
ASM_PFX(DoPowerCycleReset):

ResetSystem:
    mov     dx, 0xCF9
    mov     al, 0
    out     dx, al
    mov     al, 0xE
    out     dx, al
    cli
    hlt
    jmp     $


;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;
;  Procedure:   LaunchBiosAcm
;
;  @param[in]   AcmBase   Base address of LT BIOS ACM
;  @param[in]   Function  function number to execute
;
;  Output:
;
;  Stack:       Available at beginning of routine, but routine goes 'stackless'
;
;  Registers:
;
;  Description: Setup GETSEC environment (protected mode, mtrrs, etc) and
;               invoke GETSEC:ENTERACCS with requested BIOS ACM entry point.
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
global ASM_PFX(LaunchBiosAcm)
ASM_PFX(LaunchBiosAcm):
;    START_FRAME
;    MAKE_LOCAL BIOS_GDT[2]:QWORD
;    MAKE_LOCAL BIOS_IDT[2]:QWORD
;    END_FRAME

    enter  0x30,0

    ;
    ; Save the general purpose register state
    ;
    pushfq
    cli
    PUSHA_64

    ; check if the AcmBase is a good address (none zero)
    cmp     rcx, 0                  ; bad ACM address?
    je      LaunchBiosAcm_SkipLaunch              ; Yes, then exit without any further work

    ;
    ; Save the parameters passed to us
    ;
    mov     rax, rcx                ; save address of BIOS ACM in MMX0
    MOVQIN0
    mov     rax, cr3                ; save page table in MMX1
    MOVQIN1
    mov     rax, rdx                ; save value of ESI for GETSEC[ENTERACCS] in MMX2
    MOVQIN2

    sgdt  BIOS_GDT                  ; save gdtr
    sidt  BIOS_IDT                  ; save idtr

    lea   rax, BIOS_GDT             ; rax = address of saved gdtr
    MOVQIN3                         ; mm3 = address of saved gdtr

    mov     rax, rbp
    MOVQIN4                         ; mm4 = rbp
    xor     rax, rax
    mov     ax, ss
    MOVQIN5                         ; mm5 = ss
    mov     ax, cs
    MOVQIN6                         ; mm6 = cs

    ;
    ; Save control registers
    ;
    mov     rax, cr4
    push    rax
    mov     rax, cr0
    push    rax

    ;
    ; Save IA32_MISC_ENABLES MSR
    ;
    mov     rcx, IA32_MISC_ENABLE
    rdmsr
    push    rax
    push    rdx

    ;
    ; Save IA32_EFER MSR
    ;
    mov     rcx, IA32_EFER_MSR
    rdmsr
    push    rax
    push    rdx

;-----------------------------------------------------------------------------
;
;  Section:      Save variable MTRRs
;
;  Description:  All variable MTRRs are saved in local variables.
;                They will be restored after running of BIOS ACM.
;
;-----------------------------------------------------------------------------
    mov     rcx, IA32_MTRR_CAP
    rdmsr
    and     rax, 0xFF
    shl     rax, 1
    mov     rcx, rax

LaunchBiosAcm_SaveNextMtrr:
    add     rcx, IA32_MTRR_PHYSBASE0 - 1
    rdmsr
    push    rdx
    push    rax
    sub     rcx, IA32_MTRR_PHYSBASE0 - 1
    loop    LaunchBiosAcm_SaveNextMtrr

    ;
    ; Save IA32_MTRR_DEF_TYPE MSR
    ;
    mov     rcx, IA32_MTRR_DEF_TYPE
    rdmsr
    push    rax
    push    rdx

    ; Save the rest of the segment registers
    xor     rax, rax
    mov     ax, gs
    push    rax
    mov     ax, fs
    push    rax
    mov     ax, es
    push    rax
    mov     ax, ds
    push    rax

    ;
    ; For reloading CS to Long Mode
    ;
    mov     rcx, cs
    shl     rcx, 32
    lea     rdx, [LaunchBiosAcm_ReloadCS]           ; Assume it is below 4G
    or      rcx, rdx
    push    rcx

    ;
    ; Now that all of our state is on the stack, save esp
    ;
    mov     rax, rsp
    MOVQIN7                         ; mm7 = rsp

;-----------------------------------------------------------------------------
;
;  Section:      Program MTRRs
;
;  Description:  Variable MTRRs are programmed to cache ACM as WB
;
;-----------------------------------------------------------------------------
    ;
    ; Configure MTRR_PHYS_MASK_HIGH for proper addressing above 4GB
    ; based on the physical address size supported for this processor
    ; This is based on read from CPUID EAX = 080000008h, EAX bits [7:0]
    ;
    ; Examples:
    ;  MTRR_PHYS_MASK_HIGH = 00000000Fh  For 36 bit addressing
    ;  MTRR_PHYS_MASK_HIGH = 0000000FFh  For 40 bit addressing
    ;
    mov     eax, 80000008h            ; Address sizes leaf
    cpuid
    sub     al, 32
    movzx   eax, al
    xor     rsi, rsi
    bts     esi, eax
    dec     esi                       ; esi <- MTRR_PHYS_MASK_HIGH
    movd    MTRR_PHYS_MASK_HIGH, esi

    ;
    ; Enable SMXE, SSE and debug extensions
    ;
    mov     rax, cr4
    or      rax, CR4_OSFXSR + CR4_DE + CR4_SMXE
    mov     cr4, rax

    ;
    ; Disable cache
    ;
    mov     rax, cr0                ; set CR0:CD and CR0:NE, clear CR0:NW
    or      rax, CR0_CD_MASK | CR0_NE_MASK
    and     rax, ~CR0_NW_MASK
    mov     cr0, rax

    ;
    ; Check to see how we should invalidate the cache
    ;
    MOVQOUT2
    cmp     eax, TXT_FN_SCLEAN      ; have we been called to run SCLEAN?
    je      LaunchBiosAcm_ScleanInvd
    cmp     eax, TXT_FN_CLEAR_SECRETS ; have we been called to run CLEAR_SECRETS?
    je      LaunchBiosAcm_ScleanInvd
    cmp     eax, TXT_FN_ACHECK      ; have we been called to run ACHECK?
    je      LaunchBiosAcm_ScleanInvd
    wbinvd                          ; Writeback is necessary
    jmp     LaunchBiosAcm_OverScleanInvd

LaunchBiosAcm_ScleanInvd:
    invd                            ; SCLEAN or ACHECK. Invalidate the cache.

LaunchBiosAcm_OverScleanInvd:

    ;
    ; Disable MTRRs, set Default Type to UC
    ;
    mov     rcx, IA32_MTRR_DEF_TYPE
    xor     rax, rax
    xor     rdx, rdx
    wrmsr

    ;
    ; Clear all of the Variable MTRRs
    ;
    mov     rcx, IA32_MTRR_CAP
    rdmsr
    and     rax, 0xFF
    shl     rax, 1
    mov     rcx, rax
    xor     rax, rax
    xor     rdx, rdx
LaunchBiosAcm_ClearMtrrContinue:
    add     rcx, IA32_MTRR_PHYSBASE0 - 1
    wrmsr
    sub     rcx, IA32_MTRR_PHYSBASE0 - 1
    loop    LaunchBiosAcm_ClearMtrrContinue

    ;
    ; Determine size of AC module
    ;
    MOVQOUT0                        ; rax = AcmBase
    mov     rsi, rax                ; rsi = AcmBase
    mov     rax, [rsi+ACMOD_SIZE]   ; rax = size of ACM
    shl     rax, 2                  ;  ...in bytes (ACM header has size in dwords)

    ;
    ; Round up to page size
    ;
    mov     rcx, rax                ; Save
    and     rcx, 0xFFFFF000         ; Number of pages in AC module
    and     rax, 0xFFF              ; Number of "less-than-page" bytes
    jz      LaunchBiosAcm_rounded_2
    mov     rax, 0x1000             ; Add the whole page size
LaunchBiosAcm_rounded_2:
    add     rax, rcx                ; rax = rounded up AC module size
    mov     rbx, rax                ; rbx = rounded up AC module size

    ;
    ; Program MTRRs to cover BIOS ACM
    ;
    xor     rcx, rcx
    movd    NEXT_MTRR_INDEX, ecx    ; Start from MTRR0

    ;
    ; Save remaining size to cache
    ;
    movd    ACM_SIZE_TO_CACHE, ebx  ; Size of ACM that must be cached
    movd    ACM_BASE_TO_CACHE, esi  ; Base ACM address

LaunchBiosAcm_nextMtrr_2:
    ;
    ; Get remaining size to cache
    ;
    xor     rax, rax
    movd    eax, ACM_SIZE_TO_CACHE
    and     rax, rax
    jz      LaunchBiosAcm_mtrr_done  ; If no left size - we are done
    ;
    ; Determine next size to cache.
    ; We start from bottom up. Use the following algorythm:
    ; 1. Get our own alignment. Max size we can cache equals to our alignment
    ; 2. Determine what is bigger - alignment or remaining size to cache.
    ;    If aligment is bigger - cache it.
    ;      Adjust remaing size to cache and base address
    ;      Loop to 1.
    ;    If remaining size to cache is bigger
    ;      Determine the biggest 2^N part of it and cache it.
    ;      Adjust remaing size to cache and base address
    ;      Loop to 1.
    ; 3. End when there is no left size to cache or no left MTRRs
    ;
    xor     rsi, rsi
    movd    esi, ACM_BASE_TO_CACHE
    bsf     rcx, rsi                ; Get index of lowest bit set in base address
    ;
    ; Convert index into size to be cached by next MTRR
    ;
    mov     rdx, 0x1
    shl     rdx, cl                 ; Alignment is in rdx
    cmp     rdx, rax                ; What is bigger, alignment or remaining size?
    jbe     LaunchBiosAcm_gotSize_2               ; JIf aligment is less
    ;
    ; Remaining size is bigger. Get the biggest part of it, 2^N in size
    ;
    bsr     rcx, rax                ; Get index of highest set bit
    ;
    ; Convert index into size to be cached by next MTRR
    ;
    mov     rdx, 1
    shl     rdx, cl                 ; Size to cache

LaunchBiosAcm_gotSize_2:
    mov     rax, rdx
    movd    NEXT_MTRR_SIZE, eax     ; Save

    ;
    ; Compute MTRR mask value:  Mask = NOT (Size - 1)
    ;
    dec     rax                     ; rax - size to cache less one byte
    not     eax                     ; eax contains low 32 bits of mask
    or      rax, MTRR_VALID         ; Set valid bit
    ;
    ; Program mask register
    ;
    mov     rcx, IA32_MTRR_PHYSMASK0  ; setup variable mtrr
    xor     rbx, rbx
    movd    ebx, NEXT_MTRR_INDEX
    add     rcx, rbx
    movd    edx, MTRR_PHYS_MASK_HIGH   ; edx <- MTRR_PHYS_MASK_HIGH
    wrmsr
    ;
    ; Program base register
    ;
    xor     rdx, rdx
    mov     rcx, IA32_MTRR_PHYSBASE0  ; setup variable mtrr
    add     ecx, ebx                ; ebx is still NEXT_MTRR_INDEX

    xor     rax, rax
    movd    eax, ACM_BASE_TO_CACHE
    or      eax, WB                 ; set type to write back
    wrmsr
    ;
    ; Advance and loop
    ; Reduce remaining size to cache
    ;
    movd    ebx, ACM_SIZE_TO_CACHE
    movd    eax, NEXT_MTRR_SIZE
    sub     ebx, eax
    movd    ACM_SIZE_TO_CACHE, ebx

    ;
    ; Increment MTRR index
    ;
    movd    ebx, NEXT_MTRR_INDEX
    add     ebx, 2
    movd    NEXT_MTRR_INDEX, ebx
    ;
    ; Increment base address to cache
    ;
    movd    ebx, ACM_BASE_TO_CACHE
    movd    eax, NEXT_MTRR_SIZE
    add     ebx, eax
    movd    ACM_BASE_TO_CACHE, ebx

    jmp     LaunchBiosAcm_nextMtrr_2

LaunchBiosAcm_mtrr_done:
    ;
    ; Re-enable Variable MTRRs
    ;
    xor     rdx, rdx
    mov     rax, MTRR_ENABLE        ; enable mtrrs (but not fixed ones)
    mov     rcx, IA32_MTRR_DEF_TYPE
    wrmsr

    ;
    ; Re-enable cache
    ;
    mov     rax, cr0                ; Enable caching - WB (NW stays clear)
    and     rax, ~CR0_CD_MASK
    mov     cr0, rax

    ;
    ; Clean all MCi_STATUS MSR registers
    ; SCLEAN will generate GPF otherwise
    ;
    mov     rcx, MCG_CAP
    rdmsr
    movzx   rbx, al                 ; rbx = MCR bank count
    xor     rax, rax                ; Write 0 into all MCi_STATUS registers
    xor     rdx, rdx
    mov     rcx, MC0_STATUS

LaunchBiosAcm_McaErrorCleanLoopStart:
    wrmsr
    dec     rbx
    jz      LaunchBiosAcm_CallGetsec
    add     rcx, 4                  ; rcx = number of MSRs per bank
    jmp     LaunchBiosAcm_McaErrorCleanLoopStart

LaunchBiosAcm_CallGetsec:

    ;
    ; Change to Compatible Segment
    ;
    mov     rcx, LONG_MODE_CODE_SEGMENT  ; Hardcode Compatible mode segment
    shl     rcx, 32
    lea     rdx, [LaunchBiosAcm_Compatible]         ; assume address < 4G
    or      rcx, rdx
    push    rcx
    retf

LaunchBiosAcm_Compatible:
    ;
    ; disable paging
    ;
    mov     rax, cr0
    btr     eax, 31                 ; set PG=0
    mov     cr0, rax
    ;
    ; set EFER.LME = 0 to leave long mode
    ;
    mov     ecx, IA32_EFER_MSR      ; EFER MSR number.
    rdmsr                           ; Read EFER.
    btr     eax, 8                  ; Set LME=0.
    wrmsr                           ; Write EFER.
    and     eax, 0800h              ; Isolate NXE bit
    mov     edx, eax                ; edx = NXE bit

    ;
    ; Call GETSEC[ENTERACCS]
    ;
    MOVQOUT2                        ; rax = ACM function
    mov     esi, eax                ; esi = ACM function
    MOVQOUT0                        ; rax = AcmBase
    mov     ebx, eax                ; ebx = AcmBase
    mov     ecx, DWORD [rbx+ACMOD_SIZE]  ; ecx = size of ACM in dwords
    shl     ecx, 2                  ; ecx = size of ACM in bytes
    mov     eax, edx                ; eax = NXE flag
    MOVDIN0                         ; Save NXE state in mm0
    xor     edx, edx
    xor     edi, edi
    mov     eax, ENTERACCS          ; eax = ENTERACCS
    _GETSEC

    ;
    ; Return point after ACEXIT.
    ;

    ;
    ; Check if we need to restore BIOS state and configuration
    ;
    MOVQOUT2
    cmp     eax, TXT_FN_SCLEAN      ; Have we been called to run SCLEAN?
    je      ResetSystem             ; No-need to restore state, because system will reset.
    cmp     eax, TXT_FN_CLEAR_SECRETS ; Have we been called to run CLEAR_SECRETS?
    je      ResetSystem             ; No-need to restore state, because system will reset.
    cmp     eax, TXT_FN_ACHECK      ; Have we been called to run ACHECK?
    je      ResetSystem             ; No-need to restore state, because system will reset.

    ;
    ; Reload the GDTR
    ;
    MOVQOUT3                        ; rax = the address of our local variable
    lgdt    [rax]

    ;
    ; Restore the stack
    ;
    MOVQOUT4
    mov     ebp, eax                ; restore ebp
    MOVQOUT7
    mov     esp, eax                ; restore esp
    MOVQOUT5
    mov     ss, ax                  ; restore ss

    ;
    ; Enable protected mode
    ;
    mov     ecx, 0x33
    mov     cr0, rcx

    ;
    ; Enable PAE in CR4
    ;
    mov     ecx, 0x668 | CR4_SMXE
    mov     cr4, rcx

    ;
    ; Reload CR3
    ;
    MOVQOUT1
    mov     cr3, rax                ; restore page table from MMX1

    ;
    ; Setup re-enable of paging
    ;

    ;
    ; Set EFER.LME to re-enable ia32-e and set previous NXE state
    ;
    mov     ecx, IA32_EFER_MSR
    xor     edx, edx
    MOVDOUT0            ; eax = NXE state
    or      eax, 0x500  ; enable ia32-e
    wrmsr

    ;
    ; Re-enable paging
    ;
    mov     rbx, cr0
    bts     ebx, 31
    mov     cr0, rbx

    ;
    ; Now we're in Compatibility mode
    ;
    MOVQOUT3                        ; rax = the address of our local variable
    lgdt    [rax]
    lidt    [rax-0x10]

    ;
    ; Reload cs register
    ;
    retf
LaunchBiosAcm_ReloadCS:
    ;
    ; Now we're in Long Mode
    ;

    ;
    ; Restore the rest of the segment registers
    ;
    pop     rax
    mov     ds, ax
    pop     rax
    mov     es, ax
    pop     rax
    mov     fs, ax
    pop     rax
    mov     gs, ax

;-----------------------------------------------------------------------------
;
;  Section:  Restore MTRRs
;
;  Description:  BIOS MTRR values are restored.
;
;-----------------------------------------------------------------------------
    ;
    ; Clear out the cache
    ;
    mov     rax, cr0                ; Set CR0:CD and CR0:NE, clear CR0:NW
    or      rax, CR0_CD_MASK | CR0_NE_MASK
    and     rax, ~CR0_NW_MASK
    mov     cr0, rax
    wbinvd                          ; Flush and invalidate the cache

    ;
    ; Restore def MTRR type
    ;
    mov     rcx, IA32_MTRR_DEF_TYPE
    pop     rdx
    pop     rax
    wrmsr

    ;
    ; Reset Variable MTRRs to Pre-GETSEC state
    ;
    mov     rcx, IA32_MTRR_CAP
    rdmsr
    and     rax, 0xFF
    shl     rax, 1
    mov     rcx, rax
    mov     rbx, rax
    shl     rbx, 4                  ; rbx = the total length for MTRR register
    add     rsp, rbx                ; adjust rsp to skip MTRR register
    mov     rbx, rsp
    sub     rbx, 8                  ; let rbx point to head of MTRR register now

LaunchBiosAcm_RestoreNextMtrr:
    add     rcx, IA32_MTRR_PHYSBASE0 - 1
    mov     rdx, [rbx]
    mov     rax, [rbx - 8]
    wrmsr
    sub     rcx, IA32_MTRR_PHYSBASE0 - 1
    sub     rbx, 16
    loop    LaunchBiosAcm_RestoreNextMtrr

    ;
    ; Restore IA32_EFER MSR
    ;
    pop     rdx
    pop     rax
    mov     rcx, IA32_EFER_MSR
    wrmsr

    ;
    ; Restore MISC ENABLE MSR
    ;
    pop     rdx
    pop     rax
    mov     rcx, IA32_MISC_ENABLE
    wrmsr

;-----------------------------------------------------------------------------
;
;  Section:      Restore registers, stack and exit.
;
;  Description:  Previously saved registers are restored. Stack is restored
;                by execution leave instruction and control is returned to
;                caller.
;
;         NOTE!
;         This section ends differences between programmed and executed code.
;
;-----------------------------------------------------------------------------
    ;
    ; Now we're in Long Mode
    ; Restore control registers
    ;
    pop     rax
    mov     cr0, rax
    pop     rax
    mov     cr4, rax

    wbinvd                          ; Flush and invalidate the cache

LaunchBiosAcm_SkipLaunch:
    POPA_64
    popfq

    _EMMS

    leave
    ret 0

