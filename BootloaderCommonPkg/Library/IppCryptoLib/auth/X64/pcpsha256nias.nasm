;------------------------------------------------------------------------------
;
; Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
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

global ASM_PFX(UpdateSHA256Ni)
ASM_PFX(UpdateSHA256Ni):
  ret
