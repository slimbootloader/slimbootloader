;------------------------------------------------------------------------------
;
; Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Intel SHA Extensions optimized implementation of a SHA-256 update function
;
; The function takes a pointer to the current hash values, a pointer to the
; input data, and a number of 64 byte blocks to process.  Once all blocks have
; been processed, the digest pointer is  updated with the resulting hash value.
; The function only processes complete blocks, there is no functionality to
; store partial blocks.  All message padding and hash value initialization must
; be done outside the update function.
;
; The indented lines in the loop are instructions related to rounds processing.
; The non-indented lines are instructions related to the message schedule.
;
; Author: Sean Gulley <sean.m.gulley@intel.com>
; Date:   July 2013
;
; https://software.intel.com/en-us/articles/intel-sha-extensions-implementations
;

    SECTION .text

%define MSG              xmm0
%define STATE0           xmm1
%define STATE1           xmm2
%define MSGTMP0          xmm3
%define MSGTMP1          xmm4
%define MSGTMP2          xmm5
%define MSGTMP3          xmm6
%define MSGTMP4          xmm7

%define SHUF_MASK_DW(x)  [eax+x]
%define SHUF_MASK        [eax+0x00]
%define ABEF_SAVE        [eax+0x10]
%define CDGH_SAVE        [eax+0x20]


%define SHA256CONSTANTS  ebx
%define DIGEST_PTR       edi
%define DATA_PTR         esi
%define DATA_SIZE        ecx

global ASM_PFX(UpdateSHA256Ni)

ASM_PFX(UpdateSHA256Ni):
  push   ebp
  mov    ebp,esp

  push   ebx
  push   esi
  push   edi
  sub    esp,0x40

  ;; Check MSG length
  mov    DATA_SIZE, DWORD  [ebp+0x10]
  test   DATA_SIZE, DATA_SIZE
  je     .done_hash

  ;; Reserve space for local variables
  lea    eax, [esp+0x10]
  and    eax, 0xfffffff0

  ;; Prepare PSHUFFLE_BYTE_FLIP_MASK table
  mov    ebx, 0x00010203
  mov    SHUF_MASK_DW(0), ebx
  mov    ebx, 0x4050607
  mov    SHUF_MASK_DW(4), ebx
  mov    ebx, 0x8090a0b
  mov    SHUF_MASK_DW(8), ebx
  mov    ebx, 0xc0d0e0f
  mov    SHUF_MASK_DW(12), ebx

  ;; Load parameters
  mov    DIGEST_PTR,      [ebp+0x08]
  mov    DATA_PTR,        [ebp+0x0c]
  mov    SHA256CONSTANTS, [ebp+0x14]

  ;; load initial hash values
  ;; Need to reorder these appropriately
  ;; DCBA, HGFE -> ABEF, CDGH
  movdqu    STATE0, [DIGEST_PTR + 0*16]
  movdqu    STATE1, [DIGEST_PTR + 1*16]

  pshufd    STATE0,  STATE0,  0xB1  ; CDAB
  pshufd    STATE1,  STATE1,  0x1B  ; EFGH
  movdqa    MSGTMP4, STATE0
  palignr   STATE0,  STATE1,  8 ; ABEF
  pblendw   STATE1,  MSGTMP4, 0xF0  ; CDGH

.loop0:
  ;; Save hash values for addition after rounds
  movdqa    ABEF_SAVE, STATE0
  movdqa    CDGH_SAVE, STATE1

  ;; Rounds 0-3
  movdqu    MSG, [DATA_PTR + 0*16]
  pshufb    MSG, SHUF_MASK
  movdqa    MSGTMP0, MSG
  paddd     MSG, [SHA256CONSTANTS + 0*16]
  sha256rnds2 STATE1, STATE0, MSG
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG

  ;; Rounds 4-7
  movdqu    MSG, [DATA_PTR + 1*16]
  pshufb    MSG, SHUF_MASK
  movdqa    MSGTMP1, MSG
  paddd     MSG, [SHA256CONSTANTS + 1*16]
  sha256rnds2 STATE1, STATE0, MSG
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG
  sha256msg1  MSGTMP0, MSGTMP1

  ;; Rounds 8-11
  movdqu    MSG, [DATA_PTR + 2*16]
  pshufb    MSG, SHUF_MASK
  movdqa    MSGTMP2, MSG
  paddd     MSG, [SHA256CONSTANTS + 2*16]
  sha256rnds2 STATE1, STATE0, MSG
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG
  sha256msg1  MSGTMP1, MSGTMP2

  ;; Rounds 12-15
  movdqu    MSG, [DATA_PTR + 3*16]
  pshufb    MSG, SHUF_MASK
  movdqa    MSGTMP3, MSG
  paddd     MSG, [SHA256CONSTANTS + 3*16]
  sha256rnds2 STATE1, STATE0, MSG
  movdqa    MSGTMP4, MSGTMP3
  palignr   MSGTMP4, MSGTMP2, 4
  paddd     MSGTMP0, MSGTMP4
  sha256msg2  MSGTMP0, MSGTMP3
  pshufd      MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG
  sha256msg1  MSGTMP2, MSGTMP3

  ;; Rounds 16-19
  movdqa    MSG, MSGTMP0
  paddd     MSG, [SHA256CONSTANTS + 4*16]
  sha256rnds2 STATE1, STATE0, MSG
  movdqa    MSGTMP4, MSGTMP0
  palignr   MSGTMP4, MSGTMP3, 4
  paddd     MSGTMP1, MSGTMP4
  sha256msg2  MSGTMP1, MSGTMP0
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG
  sha256msg1  MSGTMP3, MSGTMP0

  ;; Rounds 20-23
  movdqa    MSG, MSGTMP1
  paddd     MSG, [SHA256CONSTANTS + 5*16]
  sha256rnds2 STATE1, STATE0, MSG
  movdqa    MSGTMP4, MSGTMP1
  palignr   MSGTMP4, MSGTMP0, 4
  paddd     MSGTMP2, MSGTMP4
  sha256msg2  MSGTMP2, MSGTMP1
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG
  sha256msg1  MSGTMP0, MSGTMP1

  ;; Rounds 24-27
  movdqa    MSG, MSGTMP2
  paddd     MSG, [SHA256CONSTANTS + 6*16]
  sha256rnds2 STATE1, STATE0, MSG
  movdqa    MSGTMP4, MSGTMP2
  palignr   MSGTMP4, MSGTMP1, 4
  paddd     MSGTMP3, MSGTMP4
  sha256msg2  MSGTMP3, MSGTMP2
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG
  sha256msg1  MSGTMP1, MSGTMP2

  ;; Rounds 28-31
  movdqa    MSG, MSGTMP3
  paddd     MSG, [SHA256CONSTANTS + 7*16]
  sha256rnds2 STATE1, STATE0, MSG
  movdqa    MSGTMP4, MSGTMP3
  palignr   MSGTMP4, MSGTMP2, 4
  paddd     MSGTMP0, MSGTMP4
  sha256msg2  MSGTMP0, MSGTMP3
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG
  sha256msg1  MSGTMP2, MSGTMP3

  ;; Rounds 32-35
  movdqa    MSG, MSGTMP0
  paddd     MSG, [SHA256CONSTANTS + 8*16]
  sha256rnds2 STATE1, STATE0, MSG
  movdqa    MSGTMP4, MSGTMP0
  palignr   MSGTMP4, MSGTMP3, 4
  paddd     MSGTMP1, MSGTMP4
  sha256msg2  MSGTMP1, MSGTMP0
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG
  sha256msg1  MSGTMP3, MSGTMP0

  ;; Rounds 36-39
  movdqa    MSG, MSGTMP1
  paddd     MSG, [SHA256CONSTANTS + 9*16]
  sha256rnds2 STATE1, STATE0, MSG
  movdqa    MSGTMP4, MSGTMP1
  palignr   MSGTMP4, MSGTMP0, 4
  paddd     MSGTMP2, MSGTMP4
  sha256msg2  MSGTMP2, MSGTMP1
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG
  sha256msg1  MSGTMP0, MSGTMP1

  ;; Rounds 40-43
  movdqa    MSG, MSGTMP2
  paddd     MSG, [SHA256CONSTANTS + 10*16]
  sha256rnds2 STATE1, STATE0, MSG
  movdqa    MSGTMP4, MSGTMP2
  palignr   MSGTMP4, MSGTMP1, 4
  paddd     MSGTMP3, MSGTMP4
  sha256msg2  MSGTMP3, MSGTMP2
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG
  sha256msg1  MSGTMP1, MSGTMP2

  ;; Rounds 44-47
  movdqa    MSG, MSGTMP3
  paddd     MSG, [SHA256CONSTANTS + 11*16]
  sha256rnds2 STATE1, STATE0, MSG
  movdqa    MSGTMP4, MSGTMP3
  palignr   MSGTMP4, MSGTMP2, 4
  paddd     MSGTMP0, MSGTMP4
  sha256msg2  MSGTMP0, MSGTMP3
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG
  sha256msg1  MSGTMP2, MSGTMP3

  ;; Rounds 48-51
  movdqa    MSG, MSGTMP0
  paddd     MSG, [SHA256CONSTANTS + 12*16]
  sha256rnds2 STATE1, STATE0, MSG
  movdqa    MSGTMP4, MSGTMP0
  palignr   MSGTMP4, MSGTMP3, 4
  paddd     MSGTMP1, MSGTMP4
  sha256msg2  MSGTMP1, MSGTMP0
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG
  sha256msg1  MSGTMP3, MSGTMP0

  ;; Rounds 52-55
  movdqa    MSG, MSGTMP1
  paddd     MSG, [SHA256CONSTANTS + 13*16]
  sha256rnds2 STATE1, STATE0, MSG
  movdqa    MSGTMP4, MSGTMP1
  palignr   MSGTMP4, MSGTMP0, 4
  paddd     MSGTMP2, MSGTMP4
  sha256msg2  MSGTMP2, MSGTMP1
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG

  ;; Rounds 56-59
  movdqa    MSG, MSGTMP2
  paddd     MSG, [SHA256CONSTANTS + 14*16]
  sha256rnds2 STATE1, STATE0, MSG
  movdqa    MSGTMP4, MSGTMP2
  palignr   MSGTMP4, MSGTMP1, 4
  paddd     MSGTMP3, MSGTMP4
  sha256msg2  MSGTMP3, MSGTMP2
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG

  ;; Rounds 60-63
  movdqa    MSG, MSGTMP3
  paddd     MSG, [SHA256CONSTANTS + 15*16]
  sha256rnds2 STATE1, STATE0, MSG
  pshufd    MSG, MSG, 0x0E
  sha256rnds2 STATE0, STATE1, MSG

  ;; Add current hash values with previously saved
  paddd   STATE0, ABEF_SAVE
  paddd   STATE1, CDGH_SAVE

  ;; Increment data pointer and loop if more to process
  add   DATA_PTR,  64
  sub   DATA_SIZE, 64
  jg    .loop0

  ;; Write hash values back in the correct order
  pshufd    STATE0,  STATE0,  0x1B  ; FEBA
  pshufd    STATE1,  STATE1,  0xB1  ; DCHG
  movdqa    MSGTMP4, STATE0
  pblendw   STATE0,  STATE1,  0xF0  ; DCBA
  palignr   STATE1,  MSGTMP4, 8 ; HGFE

  movdqu    [DIGEST_PTR + 0*16], STATE0
  movdqu    [DIGEST_PTR + 1*16], STATE1

.done_hash:
  add    esp,0x40
  pop    edi
  pop    esi
  pop    ebx
  pop    ebp
  ret
