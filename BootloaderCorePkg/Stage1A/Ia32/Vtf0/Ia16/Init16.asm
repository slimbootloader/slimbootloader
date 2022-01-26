;------------------------------------------------------------------------------
; @file
; 16-bit initialization code
;
; Copyright (c) 2008 - 2009, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;------------------------------------------------------------------------------


BITS    16

;
; @param[out] DI    'BP' to indicate boot-strap processor
;
EarlyBspInitReal16:
    mov     di, 'BP'
    jmp     Main16

;
; @param[out] DI    'AP' to indicate application processor
;
EarlyApInitReal16:
    mov     di, 'AP'
    jmp     Main16

;
; Modified:  EAX
;
; @param[in]  EAX   Initial value of the EAX register (BIST: Built-in Self Test)
; @param[out] ESP   Initial value of the EAX register (BIST: Built-in Self Test)
;
EarlyInit16:
    cli
    ;
    ; ESP -  Initial value of the EAX register (BIST: Built-in Self Test)
    ;
    OneTimeCallRet EarlyInit16

