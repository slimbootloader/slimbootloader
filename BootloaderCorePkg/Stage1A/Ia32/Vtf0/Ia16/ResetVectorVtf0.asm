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
%ifdef ALIGN_TOP_TO_4K_FOR_PAGING
;
; Pad the image size to 4k when page tables are in VTF0
;
; If the VTF0 image has page tables built in, then we need to make
; sure the end of VTF0 is 4k above where the page tables end.
;
; This is required so the page tables will be 4k aligned when VTF0 is
; located just below 0x100000000 (4GB) in the firmware device.
;
    TIMES (0x1000 - ($ - EndOfPageTables) - (fourGigabytes - EndOfPageTableAlignmentPadding)) DB 0
EndOfPageTableAlignmentPadding:
%endif
ALIGN 16

FITHeader:
    DQ      0,0,0,0

    DQ      0
ALIGN   8

    DD      0 ; Tools expect VTF0 signature 0x14 bytes from the end

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
    jmp     EarlyBspInitReal16
ALIGN   16

fourGigabytes:
