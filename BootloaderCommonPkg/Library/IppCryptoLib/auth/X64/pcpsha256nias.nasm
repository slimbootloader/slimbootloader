;===============================================================================
; Copyright 2015-2020 Intel Corporation
;
; Licensed under the Apache License, Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
;     http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the specific language governing permissions and
; limitations under the License.
;===============================================================================

;
;
;     Purpose:  Cryptography Primitive.
;               Message block processing according to SHA-256
;
;     Content:
;        UpdateSHA256ni
;
;
    SECTION .text

%include "ia_32e.inc"

%xdefine LOCAL_FRAME  sizeof(oword) * 2

align IPP_ALIGN_FACTOR
PSHUFFLE_BYTE_FLIP_MASK \
      DB     3,2,1,0, 7,6,5,4, 11,10,9,8, 15,14,13,12

align IPP_ALIGN_FACTOR
;*****************************************************************************************
;* Purpose:     Update internal digest according to message block
;*
;* void UpdateSHA256ni(DigestSHA256 digest, const Ipp8u* msg, int mlen, const void* pParam)
;*
;*****************************************************************************************

global ASM_PFX(UpdateSHA256Ni)

ASM_PFX(UpdateSHA256Ni):

%xdefine MBS_SHA256 (64) ; SHA-256 message block length (bytes)

%xdefine HASH_PTR   rcx  ; 1st arg
%xdefine MSG_PTR    rdx  ; 2nd arg
%xdefine MSG_LEN    r8   ; 3rd arg
%xdefine K256_PTR   r9   ; 4th arg

%xdefine MSG        xmm0
%xdefine STATE0     xmm1
%xdefine STATE1     xmm2
%xdefine MSGTMP0    xmm3
%xdefine MSGTMP1    xmm4
%xdefine MSGTMP2    xmm5
%xdefine MSGTMP3    xmm6
%xdefine MSGTMP4    xmm7

%xdefine SHUF_MASK  xmm8

%xdefine ABEF_SAVE  xmm9
%xdefine CDGH_SAVE  xmm10

   SAVE_XMM  xmm6,xmm7,xmm8,xmm9,xmm10

   test     MSG_LEN, MSG_LEN
   jz       .quit

   ;; load input hash value, reorder these appropriately
   movdqu   STATE0, oword [HASH_PTR+0*16]
   movdqu   STATE1, oword [HASH_PTR+1*16]

   pshufd   STATE0,  STATE0,  0B1h  ; CDAB
   pshufd   STATE1,  STATE1,  01Bh  ; EFGH
   movdqa   MSGTMP4, STATE0
   palignr  STATE0,  STATE1,  8     ; ABEF
   pblendw  STATE1,  MSGTMP4, 0F0h  ; CDGH

   movdqa   SHUF_MASK, oword [rel PSHUFFLE_BYTE_FLIP_MASK]

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; process next data block
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.sha256_block_loop:
   movdqa   ABEF_SAVE, STATE0 ; save for addition after rounds
   movdqa   CDGH_SAVE, STATE1

   ;; rounds 0-3
   movdqu      MSG, oword [MSG_PTR + 0*16]
   pshufb      MSG, SHUF_MASK
   movdqa      MSGTMP0, MSG
   paddd       MSG, oword [K256_PTR + 0*16]
   sha256rnds2 STATE1, STATE0, MSG
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG

   ;; rounds 4-7
   movdqu      MSG, oword [MSG_PTR + 1*16]
   pshufb      MSG, SHUF_MASK
   movdqa      MSGTMP1, MSG
   paddd       MSG, oword [K256_PTR + 1*16]
   sha256rnds2 STATE1, STATE0, MSG
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG
   sha256msg1  MSGTMP0, MSGTMP1

   ;; rounds 8-11
   movdqu      MSG, oword [MSG_PTR + 2*16]
   pshufb      MSG, SHUF_MASK
   movdqa      MSGTMP2, MSG
   paddd       MSG, oword [K256_PTR + 2*16]
   sha256rnds2 STATE1, STATE0, MSG
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG
   sha256msg1  MSGTMP1, MSGTMP2

   ;; rounds 12-15
   movdqu      MSG, oword [MSG_PTR + 3*16]
   pshufb      MSG, SHUF_MASK
   movdqa      MSGTMP3, MSG
   paddd       MSG, oword [K256_PTR + 3*16]
   sha256rnds2 STATE1, STATE0, MSG
   movdqa      MSGTMP4, MSGTMP3
   palignr     MSGTMP4, MSGTMP2, 4
   paddd       MSGTMP0, MSGTMP4
   sha256msg2  MSGTMP0, MSGTMP3
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG
   sha256msg1  MSGTMP2, MSGTMP3
   ;; rounds 16-19
   movdqa      MSG, MSGTMP0
   paddd       MSG, oword [K256_PTR + 4*16]
   sha256rnds2 STATE1, STATE0, MSG
   movdqa      MSGTMP4, MSGTMP0
   palignr     MSGTMP4, MSGTMP3, 4
   paddd       MSGTMP1, MSGTMP4
   sha256msg2  MSGTMP1, MSGTMP0
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG
   sha256msg1  MSGTMP3, MSGTMP0

   ;; rounds 20-23
   movdqa      MSG, MSGTMP1
   paddd       MSG, oword [K256_PTR + 5*16]
   sha256rnds2 STATE1, STATE0, MSG
   movdqa      MSGTMP4, MSGTMP1
   palignr     MSGTMP4, MSGTMP0, 4
   paddd       MSGTMP2, MSGTMP4
   sha256msg2  MSGTMP2, MSGTMP1
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG
   sha256msg1  MSGTMP0, MSGTMP1

   ;; rounds 24-27
   movdqa      MSG, MSGTMP2
   paddd       MSG, oword [K256_PTR + 6*16]
   sha256rnds2 STATE1, STATE0, MSG
   movdqa      MSGTMP4, MSGTMP2
   palignr     MSGTMP4, MSGTMP1, 4
   paddd       MSGTMP3, MSGTMP4
   sha256msg2  MSGTMP3, MSGTMP2
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG
   sha256msg1  MSGTMP1, MSGTMP2

   ;; rounds 28-31
   movdqa      MSG, MSGTMP3
   paddd       MSG, oword [K256_PTR + 7*16]
   sha256rnds2 STATE1, STATE0, MSG
   movdqa      MSGTMP4, MSGTMP3
   palignr     MSGTMP4, MSGTMP2, 4
   paddd       MSGTMP0, MSGTMP4
   sha256msg2  MSGTMP0, MSGTMP3
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG
   sha256msg1  MSGTMP2, MSGTMP3

   ;; rounds 32-35
   movdqa      MSG, MSGTMP0
   paddd       MSG, oword [K256_PTR + 8*16]
   sha256rnds2 STATE1, STATE0, MSG
   movdqa      MSGTMP4, MSGTMP0
   palignr     MSGTMP4, MSGTMP3, 4
   paddd       MSGTMP1, MSGTMP4
   sha256msg2  MSGTMP1, MSGTMP0
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG
   sha256msg1  MSGTMP3, MSGTMP0

   ;; rounds 36-39
   movdqa      MSG, MSGTMP1
   paddd       MSG, oword [K256_PTR + 9*16]
   sha256rnds2 STATE1, STATE0, MSG
   movdqa      MSGTMP4, MSGTMP1
   palignr     MSGTMP4, MSGTMP0, 4
   paddd       MSGTMP2, MSGTMP4
   sha256msg2  MSGTMP2, MSGTMP1
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG
   sha256msg1  MSGTMP0, MSGTMP1

   ;; rounds 40-43
   movdqa      MSG, MSGTMP2
   paddd       MSG, oword [K256_PTR + 10*16]
   sha256rnds2 STATE1, STATE0, MSG
   movdqa      MSGTMP4, MSGTMP2
   palignr     MSGTMP4, MSGTMP1, 4
   paddd       MSGTMP3, MSGTMP4
   sha256msg2  MSGTMP3, MSGTMP2
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG
   sha256msg1  MSGTMP1, MSGTMP2

   ;; rounds 44-47
   movdqa      MSG, MSGTMP3
   paddd       MSG, oword [K256_PTR + 11*16]
   sha256rnds2 STATE1, STATE0, MSG
   movdqa      MSGTMP4, MSGTMP3
   palignr     MSGTMP4, MSGTMP2, 4
   paddd       MSGTMP0, MSGTMP4
   sha256msg2  MSGTMP0, MSGTMP3
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG
   sha256msg1  MSGTMP2, MSGTMP3

   ;; rounds 48-51
   movdqa      MSG, MSGTMP0
   paddd       MSG, oword [K256_PTR + 12*16]
   sha256rnds2 STATE1, STATE0, MSG
   movdqa      MSGTMP4, MSGTMP0
   palignr     MSGTMP4, MSGTMP3, 4
   paddd       MSGTMP1, MSGTMP4
   sha256msg2  MSGTMP1, MSGTMP0
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG
   sha256msg1  MSGTMP3, MSGTMP0

   ;; rounds 52-55
   movdqa      MSG, MSGTMP1
   paddd       MSG, oword [K256_PTR + 13*16]
   sha256rnds2 STATE1, STATE0, MSG
   movdqa      MSGTMP4, MSGTMP1
   palignr     MSGTMP4, MSGTMP0, 4
   paddd       MSGTMP2, MSGTMP4
   sha256msg2  MSGTMP2, MSGTMP1
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG

   ;; rounds 56-59
   movdqa      MSG, MSGTMP2
   paddd       MSG, oword [K256_PTR + 14*16]
   sha256rnds2 STATE1, STATE0, MSG
   movdqa      MSGTMP4, MSGTMP2
   palignr     MSGTMP4, MSGTMP1, 4
   paddd       MSGTMP3, MSGTMP4
   sha256msg2  MSGTMP3, MSGTMP2
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG

   ;; rounds 60-63
   movdqa      MSG, MSGTMP3
   paddd       MSG, oword [K256_PTR + 15*16]
   sha256rnds2 STATE1, STATE0, MSG
   pshufd      MSG, MSG, 0Eh
   sha256rnds2 STATE0, STATE1, MSG

   paddd       STATE0, ABEF_SAVE    ; update previously saved hash
   paddd       STATE1, CDGH_SAVE

   add         MSG_PTR, MBS_SHA256
   sub         MSG_LEN, MBS_SHA256
   jg          .sha256_block_loop

   ; reorder hash
   pshufd      STATE0,  STATE0,  01Bh  ; FEBA
   pshufd      STATE1,  STATE1,  0B1h  ; DCHG
   movdqa      MSGTMP4, STATE0
   pblendw     STATE0,  STATE1,  0F0h  ; DCBA
   palignr     STATE1,  MSGTMP4, 8     ; HGFE

   ; and store it back
   movdqu      oword [HASH_PTR + 0*16], STATE0
   movdqu      oword [HASH_PTR + 1*16], STATE1

.quit:
   REST_XMM  xmm6,xmm7,xmm8,xmm9,xmm10

   ret

