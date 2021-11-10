;------------------------------------------------------------------------------
; @file
; First code exectuted by processor after resetting.
;
; Copyright (c) 2008 - 2011, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;------------------------------------------------------------------------------

BITS    16

ALIGN   16
FITHeader:
        DQ 0,0,0,0

applicationProcessorEntryPoint:
;
; Application Processors entry point
;
; GenFv generates code aligned on a 4k boundary which will jump to this
; location.  (0xffffffe0)  This allows the Local APIC Startup IPI to be
; used to wake up the application processors.
;
    jmp     short EarlyApInitReal16

ALIGN   8

    DD      0

;
; The VTF signature
;
; VTF-0 means that the VTF (Volume Top File) code does not require
; any fixups.
;
vtfSignature:
    DB      'V', 'T', 'F', 0

ALIGN   16 ; 0fffffff0

resetVector:
;
; Reset Vector
;
; This is where the processor will begin execution
;
    nop
    nop
    jmp     short EarlyBspInitReal16
ALIGN   16

fourGigabytes:

