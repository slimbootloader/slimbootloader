;------------------------------------------------------------------------------
;
; Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
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
