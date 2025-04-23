;; @file
;  This file contains macros supporting 64 bit assembly
;
; @copyright
;  INTEL CONFIDENTIAL
;  Copyright 1999 - 2016 Intel Corporation.
;
;  The source code contained or described herein and all documents related to the
;  source code ("Material") are owned by Intel Corporation or its suppliers or
;  licensors. Title to the Material remains with Intel Corporation or its suppliers
;  and licensors. The Material may contain trade secrets and proprietary and
;  confidential information of Intel Corporation and its suppliers and licensors,
;  and is protected by worldwide copyright and trade secret laws and treaty
;  provisions. No part of the Material may be used, copied, reproduced, modified,
;  published, uploaded, posted, transmitted, distributed, or disclosed in any way
;  without Intel's prior express written permission.
;
;  No license under any patent, copyright, trade secret or other intellectual
;  property right is granted to or conferred upon you by disclosure or delivery
;  of the Materials, either expressly, by implication, inducement, estoppel or
;  otherwise. Any license under such intellectual property rights must be
;  express and approved by Intel in writing.
;
;  Unless otherwise agreed by Intel in writing, you may not remove or alter
;  this notice or any other notice embedded in Materials by Intel or
;  Intel's suppliers or licensors in any way.
;
;  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
;  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
;  the terms of your license agreement with Intel or your vendor. This file may
;  be modified by the user, subject to additional terms of the license agreement.
;
;@par Specification Reference:
;;


;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Macro:	_movq
;
;  Input:	rxx - general perpose 64 bit register
;               mmx - MMX register
;
;  Output:	None
;
;  Registers:
;
;  Description:	Performs "movq   rxx, mmx" and "movq    mmx, rxx" operations
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

_movq    MACRO toReg:REQ, frReg:REQ
        LOCAL   gReg, mReg, op
        db      48h                     ; ; REX.W prefix
IFIDNI <toReg>, <rax>
        gReg = 00h
        op = 7Eh
ELSEIFIDNI <toReg>, <rcx>
        gReg = 01h
        op = 7Eh
ELSEIFIDNI <toReg>, <rdx>
        gReg = 02h
        op = 7Eh
ELSEIFIDNI <toReg>, <rbx>
        gReg = 03h
        op = 7Eh
ELSEIFIDNI <toReg>, <rsp>
        gReg = 04h
        op = 7Eh
ELSEIFIDNI <toReg>, <rbp>
        gReg = 05h
        op = 7Eh
ELSEIFIDNI <toReg>, <rsi>
        gReg = 06h
        op = 7Eh
ELSEIFIDNI <toReg>, <rdi>
        gReg = 07h
        op = 7Eh
ELSEIFIDNI <toReg>, <mm0>
        mReg = 0C0h + 0 * 08h
        op = 6Eh
ELSEIFIDNI <toReg>, <mm1>
        mReg = 0C0h + 1 * 08h
        op = 6Eh
ELSEIFIDNI <toReg>, <mm2>
        mReg = 0C0h + 2 * 08h
        op = 6Eh
ELSEIFIDNI <toReg>, <mm3>
        mReg = 0C0h + 3 * 08h
        op = 6Eh
ELSEIFIDNI <toReg>, <mm4>
        mReg = 0C0h + 4 * 08h
        op = 6Eh
ELSEIFIDNI <toReg>, <mm5>
        mReg = 0C0h + 5 * 08h
        op = 6Eh
ELSEIFIDNI <toReg>, <mm6>
        mReg = 0C0h + 6 * 08h
        op = 6Eh
ELSEIFIDNI <toReg>, <mm7>
        mReg = 0C0h + 7 * 08h
        op = 6Eh
ENDIF

;; If op = 7E format movq rxx, mmx

IFIDNI <frReg>, <rax>
        gReg = 00h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <rcx>
        gReg = 01h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <rdx>
        gReg = 02h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <rbx>
        gReg = 03h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <rsp>
        gReg = 04h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <rbp>
        gReg = 05h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <rsi>
        gReg = 06h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <rdi>
        gReg = 07h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm0>
        mReg = 0C0h + 0 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm1>
        mReg = 0C0h + 1 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm2>
        mReg = 0C0h + 2 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm3>
        mReg = 0C0h + 3 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm4>
        mReg = 0C0h + 4 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm5>
        mReg = 0C0h + 5 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm6>
        mReg = 0C0h + 6 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm7>
        mReg = 0C0h + 7 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ENDIF
        db 0Fh, op, gReg + mReg
ENDM



;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Macro:	_movd
;
;  Input:	exx - general perpose 32 bit register
;               mmx - MMX register
;
;  Output:	None
;
;  Registers:
;
;  Description:	Performs "movd   exx, mmx" and "movd    mmx, exx" operations
;               coded to perform in compatibility or protected mode but
;               assembled by ml64
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------


_movd    MACRO toReg:REQ, frReg:REQ
        LOCAL   gReg, mReg, op
IFIDNI <toReg>, <eax>
        gReg = 00h
        op = 7Eh
ELSEIFIDNI <toReg>, <ecx>
        gReg = 01h
        op = 7Eh
ELSEIFIDNI <toReg>, <edx>
        gReg = 02h
        op = 7Eh
ELSEIFIDNI <toReg>, <ebx>
        gReg = 03h
        op = 7Eh
ELSEIFIDNI <toReg>, <esp>
        gReg = 04h
        op = 7Eh
ELSEIFIDNI <toReg>, <ebp>
        gReg = 05h
        op = 7Eh
ELSEIFIDNI <toReg>, <esi>
        gReg = 06h
        op = 7Eh
ELSEIFIDNI <toReg>, <edi>
        gReg = 07h
        op = 7Eh
ELSEIFIDNI <toReg>, <mm0>
        mReg = 0C0h + 0 * 08h
        op = 6Eh
ELSEIFIDNI <toReg>, <mm1>
        mReg = 0C0h + 1 * 08h
        op = 6Eh
ELSEIFIDNI <toReg>, <mm2>
        mReg = 0C0h + 2 * 08h
        op = 6Eh
ELSEIFIDNI <toReg>, <mm3>
        mReg = 0C0h + 3 * 08h
        op = 6Eh
ELSEIFIDNI <toReg>, <mm4>
        mReg = 0C0h + 4 * 08h
        op = 6Eh
ELSEIFIDNI <toReg>, <mm5>
        mReg = 0C0h + 5 * 08h
        op = 6Eh
ELSEIFIDNI <toReg>, <mm6>
        mReg = 0C0h + 6 * 08h
        op = 6Eh
ELSEIFIDNI <toReg>, <mm7>
        mReg = 0C0h + 7 * 08h
        op = 6Eh
ENDIF

;; If op = 7E format movq exx, mmx

IFIDNI <frReg>, <eax>
        gReg = 00h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <ecx>
        gReg = 01h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <edx>
        gReg = 02h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <ebx>
        gReg = 03h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <esp>
        gReg = 04h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <ebp>
        gReg = 05h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <esi>
        gReg = 06h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <edi>
        gReg = 07h
        IF op EQ 7Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm0>
        mReg = 0C0h + 0 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm1>
        mReg = 0C0h + 1 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm2>
        mReg = 0C0h + 2 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm3>
        mReg = 0C0h + 3 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm4>
        mReg = 0C0h + 4 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm5>
        mReg = 0C0h + 5 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm6>
        mReg = 0C0h + 6 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ELSEIFIDNI <frReg>, <mm7>
        mReg = 0C0h + 7 * 08h
        IF op EQ 6Eh
        .ERR
        ENDIF
ENDIF
        db 0Fh, op, gReg + mReg
ENDM

;-----------------------------------------------------------------------------
; 64 bit macros
;

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Macro:	PUSHA_64
;
;  Input:	None
;
;  Output:	None
;
;  Registers:
;
;  Description:	Saves all registers on stack
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
PUSHA_64   MACRO
	push	rsp
	push	rbp
	push	rax
	push	rbx
	push	rcx
	push	rdx
	push	rsi
	push	rdi
	push	r8
	push	r9
	push	r10
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
ENDM

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Macro:	POPA_64
;
;  Input:	None
;
;  Output:	None
;
;  Registers:
;
;  Description:	Restores all registers from stack
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

POPA_64	MACRO
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	rdi
	pop	rsi
	pop	rdx
	pop	rcx
	pop	rbx
	pop	rax
	pop	rbp
	pop	rsp
ENDM

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Macro:	START_FRAME
;
;  Input:	None
;
;  Output:	None
;
;  Registers:
;
;  Description:	Starts frame declaration. Creates variable to hold total
;               size of all local vars.
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

START_FRAME MACRO
        SZ = 0
ENDM


;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Macro:	MAKE_LOCAL
;
;  Input:	ARG - var definition in form Label[Count]:QuolifiedType
;
;  Output:	None
;
;  Registers:
;
;  Description:	Defines local procedure variable. Adds size of created variable
;               to total size of locals.
;               size of all local vars.
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

MAKE_LOCAL MACRO ARG:REQ
        LOCAL brk, rbrk, clm, lbl, cnt, qtp
        brk  INSTR <ARG>, <[>
        rbrk INSTR <ARG>, <]>
        clm INSTR <ARG>, <:>
        IF brk GT 0
          lbl SUBSTR <ARG>, 1, brk-1
          cnt SUBSTR <ARG>, brk+1, rbrk-brk-1
          qtp SUBSTR <ARG>, clm+1
          LOCAL lbl[cnt]:qtp
        ELSE
          lbl SUBSTR <ARG>, 1, clm-1
          qtp SUBSTR <ARG>, clm+1
          LOCAL lbl:qtp
        ENDIF

        SZ = SZ + sizeof lbl
ENDM

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Macro:	END_FRAME
;
;  Input:	None
;
;  Output:	None
;
;  Registers:
;
;  Description:	Ends frame declaration. Creates stack sufficient to hold
;               all declared variables.
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

END_FRAME MACRO
        SZ = SZ + 10h                   ; Margin
	enter SZ, 0                     ; With spare room on stack
	.allocstack	SZ
	.endprolog
ENDM



SAVED_GDT       TEXTEQU  <mm0>
SAVED_SS        TEXTEQU  <mm1>
SAVED_ESP       TEXTEQU  <mm2>
SAVED_EBP       TEXTEQU  <mm3>
