;------------------------------------------------------------------------------
;
; Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   FlushCacheLineRange.Asm
;
; Abstract:
;
;   FlushCacheLineRange function
;
; Notes:
;
;------------------------------------------------------------------------------

    SECTION .text

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; AsmFlushCacheRange (
;   IN      VOID                      *StartAddress,
;   IN      UINT32                     Length
;   );
;------------------------------------------------------------------------------
global ASM_PFX(AsmFlushCacheRange)
ASM_PFX(AsmFlushCacheRange):
    ret


;------------------------------------------------------------------------------
; VOID
; EFIAPI
; AsmEnableAvx (
;   VOID
;   );
;------------------------------------------------------------------------------
global ASM_PFX(AsmEnableAvx)
ASM_PFX(AsmEnableAvx):
    ret
