;------------------------------------------------------------------------------
; @file
; Common macros used in the ResetVector VTF module.
;
; Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;------------------------------------------------------------------------------
; Common equates
;

UINT32  TYPEDEF DWORD
UINT64  TYPEDEF QWORD

;-----------------------------------------------------------------------------
; Bit definitions
;
BIT31   EQU (1 SHL 31)
BIT30   EQU (1 SHL 30)
BIT29   EQU (1 SHL 29)
BIT28   EQU (1 SHL 28)
BIT27   EQU (1 SHL 27)
BIT26   EQU (1 SHL 26)
BIT25   EQU (1 SHL 25)
BIT24   EQU (1 SHL 24)
BIT23   EQU (1 SHL 23)
BIT22   EQU (1 SHL 22)
BIT21   EQU (1 SHL 21)
BIT20   EQU (1 SHL 20)
BIT19   EQU (1 SHL 19)
BIT18   EQU (1 SHL 18)
BIT17   EQU (1 SHL 17)
BIT16   EQU (1 SHL 16)
BIT15   EQU (1 SHL 15)
BIT14   EQU (1 SHL 14)
BIT13   EQU (1 SHL 13)
BIT12   EQU (1 SHL 12)
BIT11   EQU (1 SHL 11)
BIT10   EQU (1 SHL 10)
BIT9    EQU (1 SHL  9)
BIT8    EQU (1 SHL  8)
BIT7    EQU (1 SHL  7)
BIT6    EQU (1 SHL  6)
BIT5    EQU (1 SHL  5)
BIT4    EQU (1 SHL  4)
BIT3    EQU (1 SHL  3)
BIT2    EQU (1 SHL  2)
BIT1    EQU (1 SHL  1)
BIT0    EQU (1 SHL  0)

;
; BIOS ACM functions
;
TXT_LAUNCH_SCLEAN   EQU 00h
TXT_RESET_EST_BIT   EQU 01h
TXT_RESET_AUX       EQU 02h
TXT_LAUNCH_SCHECK   EQU 04h
TXT_LAUNCH_CLEAR_SECRETS  EQU 05h
TXT_LAUNCH_LOCK_CONFIG    EQU 06h
TXT_LAUNCH_ACHECK   EQU 07h

PORT80_CODE_PREFIX  EQU 0A0h

;
; EDI parameter sent to SCHECK function
;
COLD_BOOT_PATH      EQU 0
S3_RESUME_PATH      EQU BIT1
;----------------------------------------------------------------------------
; Common stackless calling macros
;
CALL_NS MACRO lbl
    local   retaddr
    if  @WordSize EQ 2
      mov   sp, retaddr
    else
      mov   esp, retaddr
    endif

    pslldq  xmm4, 4
    pinsrw  xmm4, esp, 0
    rol     esp, 10h
    pinsrw  xmm4, esp, 1

    jmp     lbl

retaddr:
ENDM

RET_NS MACRO
    movd    esp, xmm4
    psrldq  xmm4, 4

    if @WordSize EQ 2
      jmp   sp
    else
      jmp   esp
    endif
ENDM

;-----------------------------------------------------------------------------
; AC Module header
;
ACM_HEADER STRUCT
  ORG        24
  AcmSize    dd  ?               ; // 24 4 Module size (in multiples of four bytes)
ACM_HEADER ENDS

;-----------------------------------------------------------------------------
;  TXT enabling and execution
;

CAPABILITIES        EQU 00h
ENTERACCS           EQU 02h
PARAMETERS          EQU 06h

CR0_NE_MASK         EQU (1 SHL 5)
CR0_NW_MASK         EQU (1 SHL 29)
CR0_CD_MASK         EQU (1 SHL 30)
CR0_PG_MASK         EQU (1 SHL 31)

CR4_VME             EQU (1 SHL 0)
CR4_PVI             EQU (1 SHL 1)
CR4_TSD             EQU (1 SHL 2)
CR4_DE              EQU (1 SHL 3)
CR4_PSE             EQU (1 SHL 4)
CR4_PAE             EQU (1 SHL 5)
CR4_MSE             EQU (1 SHL 6)
CR4_PGE             EQU (1 SHL 7)
CR4_PCE             EQU (1 SHL 8)
CR4_OSFXSR          EQU (1 SHL 9)
CR4_VMXE            EQU (1 SHL 13)
CR4_SMXE            EQU (1 SHL 14)

CAPABILITIES_PARAM  EQU BIT6
PARAMETERS_MCi_HDL  EQU BIT6

_GETSEC             EQU db 0fh, 37h

;----------------------------------------------------------------------------
;               DescriptorAccess Rights Definitions
;
PAGEGRANULARITY     EQU 01H ; Page Granularity
BYTEGRANULARITY     EQU 00H ; Byte Granularity
BIGSEGMENT          EQU 01H ; Big Segment 32 bit
SMALLSEGMENT        EQU 00H ; Small Segment 16 bit
SEG_PRESENT         EQU 01H ; Segment Present
SEG_NOTPRESENT      EQU 00H ; Segment Not Present
APPLSEGMENT         EQU 01H ; Application Segment
SYSTEMSEGMENT       EQU 00H ; System Segment

DATATYPE            EQU 03H ; Read, Write, accessed
STACKTYPE           EQU 03H ; Read, Write, accessed
CODETYPE            EQU 0BH ; Execute, Read, accessed

;----------------------------------------------------------------------------
;Descriptor      table   structures and records
;

R_AR0_7 RECORD P:1=0, DPL:2=0, D_T:1=0, sType:4=0

; P - Segment present
;    0=NOTPRESENT
;    1=PRESENT
; DPL - Descriptor priviledge level
; D_T - Descriptor type
;    0=SYSTEMSEGMENT
;    1=APPLSEGMENT
; sType - Segment Type
;    For Application segment:
;    03h - DATATYPE (Read/Write/Accessed)
;    07h - STACKTYPE (Expand Down/Read/Write/Accessed)
;    0Fh - CODETYPE (Execute/Conforming/Read/Accessed)
;
;    For System segment:
;    01h - Available 16 bit TSS
;    02h - LDT
;    03h - Active 16 bit TSS
;    04h - 16 bit call gate
;    05h - 16 + 32 bit task gate
;    06h - 16 bit interrupt gate
;    07h - 16 bit trap gate
;    09h - Available 32 bit TSS
;    0Bh - Active 32 bit TSS
;    0Ch - 32 bit call gate
;    0Eh - 32 bit interrupt gate
;    0Fh - 32 bit trap gate

R_LAR16_23 RECORD G:1=0, D_B:1=0, L:1=0, Avl:1=0, Lim:4=0

; G - Granularity
;    0=BYTEGRANULARITY
;    1=PAGEGRANULARITY
; D_B - Segment size
;    0=SMALLSEGMENT
;    1=BIGSEGMENT
; Res - Reserved
; Avl - Available for OS (Not used)
; Lim - Limit bits 16-19

SEG_DESCRIPTOR STRUCT 4
        Limit0_15   DW  0000            ; Limit Bits 0-15
        Base0_15    DW  0000            ; Base address Bits 0-15
        Base16_23   DB  00              ; Base address Bits 16-23
        AR0_7       R_AR0_7 <>          ; Access Rights 0-7
        LAR16_23    R_LAR16_23 <>       ; Limit Bits 16-19 and Access Bits 8-11
        Base24_31   DB  00              ; Base address Bits 24-31
SEG_DESCRIPTOR ENDS

;-----------------------------------------------------------------------------
; Machine Check register definitions
;
MCG_CAP           EQU 179h

MC0_STATUS        EQU 401h

;-----------------------------------------------------------------------------
; TXT register space definitions
;
TXT_PUBLIC_BASE   EQU 0FED30000h
TXT_SINIT_SIZE2   EQU 027Ch
TXT_SVMM_JOIN     EQU 0290h
TXT_SVMM_JOIN2    EQU 0294h

BIOACM_ADDR       EQU TXT_SINIT_SIZE2

;
; ProcessorMSR Equates
;
EFI_MSR_IA32_THERM_INTERRUPT  EQU 019Bh

;
; Extended Signature Table (EST)
;
EST STRUCT
        count     DD  ?   ; EST Count
        checksum  DD  ?   ; EST Checksum
        reserved  DD  3 dup(?)
EST ENDS

;----------------------------------------------------------------------------
; APIC definitions
;
IA32_APIC_BASE              EQU 001Bh   ; APIC base MSR
BASE_ADDR_MASK              EQU 0FFFFF000h
EFI_MSR_EXT_XAPIC_LVT_THERM EQU 0833h
APIC_ID                     EQU 20h
ICR_LOW                     EQU 300h
ICR_HIGH                    EQU 310h
SPURIOUS_VECTOR_1           EQU 0F0h
LOCAL_APIC_THERMAL_DEF      EQU 330h
B_INTERRUPT_MASK            EQU (1 SHL 16)
B_DELIVERY_MODE             EQU (07h SHL 8)
V_MODE_SMI                  EQU (02h SHL 8)
B_VECTOR                    EQU (0FFh SHL 0)


;-----------------------------------------------------------------------------
; MTRRs
;
IA32_MTRR_CAP               EQU 0FEh
IA32_MTRR_PHYSBASE0         EQU 200h
IA32_MTRR_PHYSMASK0         EQU 201h
IA32_MTRR_PHYSBASE1         EQU 202h
IA32_MTRR_PHYSMASK1         EQU 203h
IA32_MTRR_PHYSBASE2         EQU 204h
IA32_MTRR_PHYSMASK2         EQU 205h
IA32_MTRR_PHYSBASE3         EQU 206h
IA32_MTRR_PHYSMASK3         EQU 207h
IA32_MTRR_PHYSBASE4         EQU 208h
IA32_MTRR_PHYSMASK4         EQU 209h
IA32_MTRR_PHYSBASE5         EQU 20Ah
IA32_MTRR_PHYSMASK5         EQU 20Bh
IA32_MTRR_PHYSBASE6         EQU 20Ch
IA32_MTRR_PHYSMASK6         EQU 20Dh
IA32_MTRR_PHYSBASE7         EQU 20Eh
IA32_MTRR_PHYSMASK7         EQU 20Fh
IA32_MTRR_PHYSBASE8         EQU 210h
IA32_MTRR_PHYSMASK8         EQU 211h
IA32_MTRR_PHYSBASE9         EQU 212h
IA32_MTRR_PHYSMASK9         EQU 213h
IA32_MTRR_FIX64K_00000      EQU 250h
IA32_MTRR_FIX16K_80000      EQU 258h
IA32_MTRR_FIX16K_A0000      EQU 259h
IA32_MTRR_FIX4K_C0000       EQU 268h
IA32_MTRR_FIX4K_C8000       EQU 269h
IA32_MTRR_FIX4K_D0000       EQU 26Ah
IA32_MTRR_FIX4K_D8000       EQU 26Bh
IA32_MTRR_FIX4K_E0000       EQU 26Ch
IA32_MTRR_FIX4K_E8000       EQU 26Dh
IA32_MTRR_FIX4K_F0000       EQU 26Eh
IA32_MTRR_FIX4K_F8000       EQU 26Fh
IA32_CR_PAT                 EQU 277h
IA32_MTRR_DEF_TYPE          EQU 2FFh
NO_EVICT_MODE               EQU 2E0h
MSR_PRMRR_PHYS_MASK         EQU 1F5h
B_MSR_PRMRR_PHYS_MASK_LOCK  EQU BIT10

;
; Only low order bits are assumed
;
MTRR_MASK         EQU 0FFFFF000h

MTRR_ENABLE       EQU (1 SHL 11)
MTRR_FIXED_ENABLE EQU (1 SHL 10)
MTRR_VALID        EQU (1 SHL 11)
UC                EQU 00h
WB                EQU 06h

MTRR_VCNT         EQU 8

;-----------------------------------------------------------------------------
;
; CPU generic definition
;
IA32_CPUID_SMX_B  EQU 6

IA32_MISC_ENABLE_MSR  EQU 1A0h
IA32_EFER_MSR         EQU 0C0000080h
LME                   EQU BIT8