;------------------------------------------------------------------------------
;
; Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
; This program and the accompanying materials
; are licensed and made available under the terms and conditions of the BSD License
; which accompanies this distribution.  The full text of the license may be found at
; http://opensource.org/licenses/bsd-license.php.
;
; THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
; WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
;
;
; Module Name:
;
;  Stage1AEarlyBoardInit.nasm
;
; Abstract:
;
;  This is the code that goes from real-mode to protected mode.
;  It consumes the reset vector.
;
;------------------------------------------------------------------------------

SECTION .text

global  ASM_PFX(EarlyBoardInit)
ASM_PFX(EarlyBoardInit):
        ;
        ; This hook is called before FSP TempRamInit API call
        ; ESI, EDI need to be preserved
        ; ESP contains return address
        ;
        jmp     esp

