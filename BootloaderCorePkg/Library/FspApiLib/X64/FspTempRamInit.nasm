;------------------------------------------------------------------------------
;
; Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;
; Module Name:
;
;  FspTempRamInit.nasm
;
; Abstract:
;
;  This is the code that will call into FSP TempRamInit API
;
;------------------------------------------------------------------------------


SECTION .text

;
; FspTampInit has been moved into VTF0 in x64 mode
;
global ASM_PFX(DummyCode)
ASM_PFX(DummyCode):
    ret
