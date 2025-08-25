;===============================================================================
; Copyright 2025 Intel Corporation
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
