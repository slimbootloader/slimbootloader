;------------------------------------------------------------------------------
;
; Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;  IbblSramEarlyBoardInit.nasm
;
; Abstract:
;
;  This is the code that will be called before FSP TempRamInit API
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

