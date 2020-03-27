;------------------------------------------------------------------------------
;
; Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;
;     Purpose:  Cryptography Primitive.
;               Message block processing according to SHA256
;
;     Content:
;        UpdateSHA256
;
;


    SECTION .text

;;
;; SHA256(Ipp32u digest[], Ipp8u dataBlock[], int dataLen, Ipp32u K_256[])
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global ASM_PFX(UpdateSHA256V8)
ASM_PFX(UpdateSHA256V8):
   ret
