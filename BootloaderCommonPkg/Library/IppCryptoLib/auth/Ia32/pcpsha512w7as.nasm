;===============================================================================
; Copyright 2014-2019 Intel Corporation
; All Rights Reserved.
;
; If this  software was obtained  under the  Intel Simplified  Software License,
; the following terms apply:
;
; The source code,  information  and material  ("Material") contained  herein is
; owned by Intel Corporation or its  suppliers or licensors,  and  title to such
; Material remains with Intel  Corporation or its  suppliers or  licensors.  The
; Material  contains  proprietary  information  of  Intel or  its suppliers  and
; licensors.  The Material is protected by  worldwide copyright  laws and treaty
; provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
; modified, published,  uploaded, posted, transmitted,  distributed or disclosed
; in any way without Intel's prior express written permission.  No license under
; any patent,  copyright or other  intellectual property rights  in the Material
; is granted to  or  conferred  upon  you,  either   expressly,  by implication,
; inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
; property rights must be express and approved by Intel in writing.
;
; Unless otherwise agreed by Intel in writing,  you may not remove or alter this
; notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
; suppliers or licensors in any way.
;
;
; If this  software  was obtained  under the  Apache License,  Version  2.0 (the
; "License"), the following terms apply:
;
; You may  not use this  file except  in compliance  with  the License.  You may
; obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
;
;
; Unless  required  by   applicable  law  or  agreed  to  in  writing,  software
; distributed under the License  is distributed  on an  "AS IS"  BASIS,  WITHOUT
; WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;
; See the   License  for the   specific  language   governing   permissions  and
; limitations under the License.
;===============================================================================

;
;
;     Purpose:  Cryptography Primitive.
;               Message block processing according to SHA512
;
;     Content:
;        UpdateSHA512
;
;

    SECTION .text

%define  IPP_ALIGN_FACTOR   32

%macro LD_ADDR 2
       lea      %1, %2
%endmacro

%macro ENDIANNESS 2
   pshufb   %1, %2
%endmacro

;;
;; Rotate Right
;;
%macro PRORQ  3
   movdqa   %3,%1
   psrlq    %1,%2
   psllq    %3,(64-%2)
   por      %1,%3
%endmacro


;;
;; Init and Update W:
;;
;; j = 0-15
;; W[j] = ENDIANNESS(src)
;;
;; j = 16-79
;; W[j] = SIGMA1(W[j- 2]) + W[j- 7]
;;       +SIGMA0(W[j-15]) + W[j-16]
;;
;; SIGMA0(x) = ROR64(x,1) ^ROR64(x,8) ^LSR64(x,7)
;; SIGMA1(x) = ROR64(x,19)^ROR64(x,61)^LSR64(x,6)
;;
%macro SIGMA0  4
   movdqa   %1, %2
   psrlq    %2, 7
   movdqa   %3,%1
   PRORQ    %1, 1, %4
   pxor     %1, %2
   PRORQ    %3,8, %4
   pxor     %1, %3
%endmacro

%macro SIGMA1  4
   movdqa   %1, %2
   psrlq    %2, 6
   movdqa   %3,%1
   PRORQ    %1, 19, %4
   pxor     %1, %2
   PRORQ    %3,61, %4
   pxor     %1, %3
%endmacro


;;
;; SHA512 step
;;
;;    Ipp64u T1 = H + SUM1(E) + CHJ(E,F,G) + K_SHA512[t] + W[t];
;;    Ipp64u T2 =     SUM0(A) + MAJ(A,B,C);
;;    D+= T1;
;;    H = T1 + T2;
;;
;; where
;;    SUM1(x) = ROR64(x,14) ^ ROR64(x,18) ^ ROR64(x,41)
;;    SUM0(x) = ROR64(x,28) ^ ROR64(x,34) ^ ROR64(x,39)
;;
;;    CHJ(x,y,z) = (x & y) ^ (~x & z)                          => x&(y^z) ^z
;;    MAJ(x,y,z) = (x & y) ^ (x & z) ^ (y & z) = (x&y)^((x^y)&z)
;;
;; Input:
;;    A,B,C,D,E,F,G,H   - 8 digest's values
;;    pW                - pointer to the W array
;;    pK512             - pointer to the constants
;;    pBuffer           - temporary buffer
;; Output:
;;    A,B,C,D*,E,F,G,H* - 8 digest's values (D and H updated)
;;    pW                - pointer to the W array
;;    pK512             - pointer to the constants
;;    pBuffer           - temporary buffer (changed)
;;
%macro  CHJ  5
   movdqa      %1,%3   ; R=f
   pxor        %1,%4   ; R=(f^g)
   pand        %1,%2   ; R=e & (f^g)
   pxor        %1,%4   ; R=e & (f^g) ^g
%endmacro

%macro  MAJ  5
   movdqa      %5,%3   ; T=b
   movdqa      %1,%2   ; R=a
   pxor        %5,%2   ; T=a^b
   pand        %1,%3   ; R=a&b
   pand        %5,%4   ; T=(a^b)&c
   pxor        %1,%5   ; R=(a&b)^((a^b)&c)
%endmacro


%macro SUM0  3
   movdqa   %1,%2
   PRORQ    %1,28,%3             ; R=ROR(X,28)
   PRORQ    %2,34,%3             ; X=ROR(X,34)
   pxor     %1,%2
   PRORQ    %2,(39-34),%3        ; X=ROR(x,39)
   pxor     %1,%2
%endmacro

%macro SUM1  3
   movdqa   %1,%2
   PRORQ    %1,14,%3             ; R=ROR(X,14)
   PRORQ    %2,18,%3             ; X=ROR(X,18)
   pxor     %1,%2
   PRORQ    %2,(41-18),%3        ; X=ROR(x,41)
   pxor     %1,%2
%endmacro

%macro SHA512_STEP  11
   movdqa      oword [%11+0*oSize],%5   ; save E
   movdqa      oword [%11+1*oSize],%1   ; save A

   movdqa      oword [%11+2*oSize],%4   ; save D
   movdqa      oword [%11+3*oSize],%8   ; save H

   CHJ         %4,%5,%6,%7, %8                             ; t1 = h+CHJ(e,f,g)+pW[]+pK512[]
   movq        %8, qword [%9]
   paddq       %4, %8                                   ; +[pW]
   movq        %8, qword [%10]
   paddq       %4, %8                                   ; +[pK512]
   paddq       %4,oword [%11+3*oSize]
   movdqa      oword [%11+3*oSize],%4   ; save t1

   MAJ         %8,%1,%2,%3, %4        ; t2 = MAJ(a,b,c)
   movdqa      oword [%11+4*oSize],%8   ; save t2

   SUM1        %4,%5,%8             ; D = SUM1(e)
   paddq       %4,oword [%11+3*oSize]   ; t1 = h+CHJ(e,f,g)+pW[]+pK512[] + SUM1(e)

   SUM0        %8,%1,%5             ; H = SUM0(a)
   paddq       %8,oword [%11+4*oSize]   ; t2 = MAJ(a,b,c)+SUM0(a)

   paddq       %8,%4               ; h = t1+t2
   paddq       %4,oword [%11+2*oSize]   ; d+= t1

   movdqa      %5,oword [%11+0*oSize]   ; restore E
   movdqa      %1,oword [%11+1*oSize]   ; restore A
%endmacro


ALIGN IPP_ALIGN_FACTOR
SWP_BYTE:
pByteSwp DB    7,6,5,4,3,2,1,0, 15,14,13,12,11,10,9,8


;*******************************************************************************************
;* Purpose:     Update internal digest according to message block
;*
;* void UpdateSHA512(DigestSHA512 digest, const Ipp64u* mblk, int mlen, const void* pParam)
;*
;*******************************************************************************************

;;
;; Lib = W7, V8, P8
;;
;; Caller = ippsSHA512Update
;; Caller = ippsSHA512Final
;; Caller = ippsSHA512MessageDigest
;;
;; Caller = ippsSHA384Update
;; Caller = ippsSHA384Final
;; Caller = ippsSHA384MessageDigest
;;
;; Caller = ippsHMACSHA512Update
;; Caller = ippsHMACSHA512Final
;; Caller = ippsHMACSHA512MessageDigest
;;
;; Caller = ippsHMACSHA384Update
;; Caller = ippsHMACSHA384Final
;; Caller = ippsHMACSHA384MessageDigest
;;
ALIGN IPP_ALIGN_FACTOR
;; IPPASM UpdateSHA512 PROC NEAR C PUBLIC \
;; USES esi edi,\
;; digest:  PTR QWORD,\        ; digest address
;; mblk:    PTR BYTE,\         ; buffer address
;; mlen:    DWORD,\            ; buffer length
;; pSHA512: PTR QWORD         ; address of SHA constants
global ASM_PFX(UpdateSHA512W7)
ASM_PFX(UpdateSHA512W7):

%define  oSize         16
%define  qSize         8
%define  digest       [ebp+0x08]
%define  mblk         [ebp+0x0C]
%define  mlen         [ebp+0x10]
%define  pSHA512      [ebp+0x14]
%define  MBS_SHA512   128                  ; SHA512 block data size

%define  sSize         5                   ; size of save area (oword)
%define  dSize         8                   ; size of digest (oword)
%define  wSize        80                   ; W values queue (qword)
%define  stackSize    sSize*oSize+dSize*oSize+wSize*qSize

%define  sOffset      0                           ; save area
%define  dOffset      sOffset+sSize*oSize ; digest offset
%define  wOffset      dOffset+dSize*oSize ; W values offset
%define  acualOffset  wOffset+wSize*qSize ; actual stack size offset

   ; Save
   push   ebp
   mov    ebp,esp

   push   ebx
   push   esi
   push   edi

   ; Start
   mov      edi,digest           ; digest address
   mov      esi,mblk             ; source data address
   mov      eax,mlen             ; source data length

   mov      edx, pSHA512         ; table constant address

   sub      esp,stackSize        ; allocate local buffer (probably unaligned)
   mov      ecx,esp
   and      esp,-16              ; 16-byte aligned stack
   sub      ecx,esp
   add      ecx,stackSize        ; acual stack size (bytes)
   mov      [esp+acualOffset],ecx

   movq     xmm0,qword [edi+qSize*0]    ; A = digest[0]
   movq     xmm1,qword [edi+qSize*1]    ; B = digest[1]
   movq     xmm2,qword [edi+qSize*2]    ; C = digest[2]
   movq     xmm3,qword [edi+qSize*3]    ; D = digest[3]
   movq     xmm4,qword [edi+qSize*4]    ; E = digest[4]
   movq     xmm5,qword [edi+qSize*5]    ; F = digest[5]
   movq     xmm6,qword [edi+qSize*6]    ; G = digest[6]
   movq     xmm7,qword [edi+qSize*7]    ; H = digest[7]
   movdqa   oword [esp+dOffset+oSize*0], xmm0
   movdqa   oword [esp+dOffset+oSize*1], xmm1
   movdqa   oword [esp+dOffset+oSize*2], xmm2
   movdqa   oword [esp+dOffset+oSize*3], xmm3
   movdqa   oword [esp+dOffset+oSize*4], xmm4
   movdqa   oword [esp+dOffset+oSize*5], xmm5
   movdqa   oword [esp+dOffset+oSize*6], xmm6
   movdqa   oword [esp+dOffset+oSize*7], xmm7

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; process next data block
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sha512_block_loop:

;;
;; initialize the first 16 qwords in the array W (remember about endian)
;;

  ;movdqa   xmm1, oword  pByteSwp ; load shuffle mask
   LD_ADDR  ecx, [pByteSwp]
   movdqa   xmm1, oword [ecx+(pByteSwp-SWP_BYTE)]

   mov      ecx,0
   ALIGN IPP_ALIGN_FACTOR
loop1:
   movdqu   xmm0, oword  [esi+ecx*qSize]   ; swap input
   ENDIANNESS xmm0, xmm1
   movdqa   oword [esp+wOffset+ecx*qSize],xmm0
   add      ecx,oSize/qSize
   cmp      ecx,16
   jl       loop1

;;
;; initialize another 80-16 qwords in the array W
;;
   ALIGN IPP_ALIGN_FACTOR
loop2:
   movdqa   xmm1,oword [esp+ecx*qSize+wOffset- 2*qSize]    ; xmm1 = W[j-2]
   SIGMA1   xmm0,xmm1,xmm2,xmm3

   movdqu   xmm5,oword [esp+ecx*qSize+wOffset-15*qSize]    ; xmm5 = W[j-15]
   SIGMA0   xmm4,xmm5,xmm6,xmm3

   movdqu   xmm7,oword [esp+ecx*qSize+wOffset- 7*qSize]    ; W[j-7]
   paddq    xmm0,xmm4
   paddq    xmm7,oword [esp+ecx*qSize+wOffset-16*qSize]    ; W[j-16]
   paddq    xmm0,xmm7
   movdqa   oword [esp+ecx*qSize+wOffset],xmm0

   add      ecx,oSize/qSize
   cmp      ecx,80
   jl       loop2

;;
;; init A,B,C,D,E,F,G,H by the internal digest
;;
   movdqa   xmm0,oword [esp+dOffset+oSize*0]    ; A = digest[0]
   movdqa   xmm1,oword [esp+dOffset+oSize*1]    ; B = digest[1]
   movdqa   xmm2,oword [esp+dOffset+oSize*2]    ; C = digest[2]
   movdqa   xmm3,oword [esp+dOffset+oSize*3]    ; D = digest[3]
   movdqa   xmm4,oword [esp+dOffset+oSize*4]    ; E = digest[4]
   movdqa   xmm5,oword [esp+dOffset+oSize*5]    ; F = digest[5]
   movdqa   xmm6,oword [esp+dOffset+oSize*6]    ; G = digest[6]
   movdqa   xmm7,oword [esp+dOffset+oSize*7]    ; H = digest[7]

;;
;; perform 0-79 steps
;;
   xor      ecx,ecx
  ALIGN IPP_ALIGN_FACTOR
loop3:
;;             A,   B,   C,   D,   E,   F,   G,   H     W[],                                             K[],                                  buffer
;;             --------------------------------------------------------------------------------------------------------------------------------------
   SHA512_STEP xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7, esp+ecx*qSize+wOffset+qSize*0, edx+ecx*qSize+qSize*0, esp
   SHA512_STEP xmm7,xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6, esp+ecx*qSize+wOffset+qSize*1, edx+ecx*qSize+qSize*1, esp
   SHA512_STEP xmm6,xmm7,xmm0,xmm1,xmm2,xmm3,xmm4,xmm5, esp+ecx*qSize+wOffset+qSize*2, edx+ecx*qSize+qSize*2, esp
   SHA512_STEP xmm5,xmm6,xmm7,xmm0,xmm1,xmm2,xmm3,xmm4, esp+ecx*qSize+wOffset+qSize*3, edx+ecx*qSize+qSize*3, esp
   SHA512_STEP xmm4,xmm5,xmm6,xmm7,xmm0,xmm1,xmm2,xmm3, esp+ecx*qSize+wOffset+qSize*4, edx+ecx*qSize+qSize*4, esp
   SHA512_STEP xmm3,xmm4,xmm5,xmm6,xmm7,xmm0,xmm1,xmm2, esp+ecx*qSize+wOffset+qSize*5, edx+ecx*qSize+qSize*5, esp
   SHA512_STEP xmm2,xmm3,xmm4,xmm5,xmm6,xmm7,xmm0,xmm1, esp+ecx*qSize+wOffset+qSize*6, edx+ecx*qSize+qSize*6, esp
   SHA512_STEP xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7,xmm0, esp+ecx*qSize+wOffset+qSize*7, edx+ecx*qSize+qSize*7, esp

   add      ecx,8
   cmp      ecx,80
   jl       loop3

;;
;; update digest
;;
   paddq    xmm0,oword [esp+dOffset+oSize*0]    ; A += digest[0]
   paddq    xmm1,oword [esp+dOffset+oSize*1]    ; B += digest[1]
   paddq    xmm2,oword [esp+dOffset+oSize*2]    ; C += digest[2]
   paddq    xmm3,oword [esp+dOffset+oSize*3]    ; D += digest[3]
   paddq    xmm4,oword [esp+dOffset+oSize*4]    ; E += digest[4]
   paddq    xmm5,oword [esp+dOffset+oSize*5]    ; F += digest[5]
   paddq    xmm6,oword [esp+dOffset+oSize*6]    ; G += digest[6]
   paddq    xmm7,oword [esp+dOffset+oSize*7]    ; H += digest[7]

   movdqa   oword [esp+dOffset+oSize*0],xmm0    ; digest[0] = A
   movdqa   oword [esp+dOffset+oSize*1],xmm1    ; digest[1] = B
   movdqa   oword [esp+dOffset+oSize*2],xmm2    ; digest[2] = C
   movdqa   oword [esp+dOffset+oSize*3],xmm3    ; digest[3] = D
   movdqa   oword [esp+dOffset+oSize*4],xmm4    ; digest[4] = E
   movdqa   oword [esp+dOffset+oSize*5],xmm5    ; digest[5] = F
   movdqa   oword [esp+dOffset+oSize*6],xmm6    ; digest[6] = G
   movdqa   oword [esp+dOffset+oSize*7],xmm7    ; digest[7] = H

   add         esi, MBS_SHA512
   sub         eax, MBS_SHA512
   jg          sha512_block_loop

   movq     qword [edi+qSize*0], xmm0    ; A = digest[0]
   movq     qword [edi+qSize*1], xmm1    ; B = digest[1]
   movq     qword [edi+qSize*2], xmm2    ; C = digest[2]
   movq     qword [edi+qSize*3], xmm3    ; D = digest[3]
   movq     qword [edi+qSize*4], xmm4    ; E = digest[4]
   movq     qword [edi+qSize*5], xmm5    ; F = digest[5]
   movq     qword [edi+qSize*6], xmm6    ; G = digest[6]
   movq     qword [edi+qSize*7], xmm7    ; H = digest[7]

   add      esp,[esp+acualOffset]

   ; Restore
   pop    edi
   pop    esi
   pop    ebx
   pop    ebp
   ret

