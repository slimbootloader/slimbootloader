;; @file
;   This is the assembly code for transferring to control to OS S3 waking vector
;   for IA32 platform
;
; Copyright (c) 2006 - 2020, Intel Corporation. All rights reserved.<BR>
;
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;;

SECTION .text

global ASM_PFX(WakeUpBuffer)
ASM_PFX(WakeUpBuffer):
    dd    600h

global ASM_PFX(WakeUp)
ASM_PFX(WakeUp):
    retf

global ASM_PFX(WakeUpSize)
ASM_PFX(WakeUpSize):
    dd    $ - ASM_PFX(WakeUp)
