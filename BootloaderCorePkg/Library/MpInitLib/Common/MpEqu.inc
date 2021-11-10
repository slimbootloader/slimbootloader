;------------------------------------------------------------------------------
;
; Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   MpEqu.inc
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

;
; Equivalent NASM structure of MP_CPU_EXCHANGE_INFO
;
struc AP_DATA_STRUCT
  .BufferStart:     CTYPE_UINT32   1
  .Gdtr:            CTYPE_UINT8    10
  .Idtr:            CTYPE_UINT8    10
  .Reserved:        CTYPE_UINT32   1
  .CSSelector:      CTYPE_UINT16   1
  .DSSelector:      CTYPE_UINT16   1
  .ESSelector:      CTYPE_UINT16   1
  .SSSelector:      CTYPE_UINT16   1
  .FSSelector:      CTYPE_UINT16   1
  .GSSelector:      CTYPE_UINT16   1
  .StackStart:      CTYPE_UINT32   1
  .CProcedure:      CTYPE_UINT32   1
  .SpinLock:        CTYPE_UINT32   1
  .ApCounter:       CTYPE_UINT32   1
  .ApStackSize:     CTYPE_UINT32   1
  .MpDataStruct:    CTYPE_UINT32   1
  .Cr3:             CTYPE_UINT32   1
  .CpuArch:         CTYPE_UINT32   1
  .SmrrBase:        CTYPE_UINT32   1
  .SmrrMask:        CTYPE_UINT32   1
  .Terminator:      CTYPE_UINT32   1
endstruc

AP_DATA_STRUCT_OFFSET equ (MpDataAreaStart - RendezvousFunnelProc)

%define AP_DATA_STRUCT_FIELD(Field) (AP_DATA_STRUCT_OFFSET + AP_DATA_STRUCT. %+ Field)
