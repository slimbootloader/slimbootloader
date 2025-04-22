;------------------------------------------------------------------------------
;
; Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
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

%define         sizeof_oword        16
%define         sizeof_dword        4

%define         vA                  0
%define         vB                  1
%define         vC                  2
%define         vD                  3
%define         vE                  4
%define         vF                  5
%define         vG                  6
%define         vH                  7

%define         XMM_SHUFB_BSWAP     xmm6
%define         W0                  xmm0
%define         W4                  xmm1
%define         W8                  xmm2
%define         W12                 xmm3
%define         SIG1                xmm4
%define         SIG0                xmm5
%define         X                   xmm6
%define         W                   xmm7
%define         regTbl              ebx

%define         MBS_SHA256          64

%define         hSize               sizeof_dword*8 ; size of hash
%define         wSize               sizeof_oword   ; W values queue (dwords)
%define         cntSize             sizeof_dword   ; local counter

%define         hashOff             0              ; hash address
%define         wOff                hashOff+hSize  ; W values offset
%define         cntOff              wOff+wSize
%define         stackSize           (hSize+wSize+cntSize)   ; stack size


%macro   LD_ADDR  2
   lea   %1, %2
%endmacro

;; we are considering x, y, z are polynomials over GF(2)
;;                    & - multiplication
;;                    ^ - additive
;;                    operations

;;
;; Chj(x,y,z) = (x&y) ^ (~x & z)
;;            = (x&y) ^ ((1^x) &z)
;;            = (x&y) ^ (z ^ x&z)
;;            = x&y ^ z ^ x&z
;;            = x&(y^z) ^z
;;
%macro   Chj  4
   mov   %1, %3
   xor   %1, %4
   and   %1, %2
   xor   %1, %4
%endmacro

;;
;; Maj(x,y,z) = (x&y) ^ (x&z) ^ (y&z)
;;            = (x&y) ^ (x&z) ^ (y&z) ^ (z&z) ^z   // note: ((z&z) ^z) = 0
;;            = x&(y^z) ^ z&(y^z) ^z
;;            = (x^z)&(y^z) ^z
;;
%macro   Maj  4
   mov   %1, %2
   xor   %1, %4
   xor   %4, %3
   and   %1, %4
   xor   %4, %3
   xor   %1, %4
%endmacro

%macro   ROTR  2
   ;;shrd  %1, %1, %2
   ror   %1, %2
%endmacro

;;
;; Summ0(x) = ROR(x,2) ^ ROR(x,13) ^ ROR(x,22)
;;
%macro   Summ0  3
   mov   %1, %2
   ROTR  %1, 2
   mov   %3, %2
   ROTR  %3, 13
   xor   %1, %3
   ROTR  %3, (22-13)
   xor   %1, %3
%endmacro

;;
;; Summ1(x) = ROR(x,6) ^ ROR(x,11) ^ ROR(x,25)
;;
%macro   Summ1  3
   mov   %1, %2
   ROTR  %1, 6
   mov   %3, %2
   ROTR  %3, 11
   xor   %1, %3
   ROTR  %3, (25-11)
   xor   %1, %3
%endmacro

;;
;; regular round (i):
;;
;; T1 = h + Sigma1(e) + Ch(e,f,g) + K[i] + W[i]
;; T2 = Sigma0(a) + Maj(a,b,c)
;; h = g
;; g = f
;; f = e
;; e = d + T1
;; d = c
;; c = b
;; b = a
;; a = T1+T2
;;
;;    or
;;
;; h += Sigma1(e) + Ch(e,f,g) + K[i] + W[i]  (==T1)
;; d += h
;; T2 = Sigma0(a) + Maj(a,b,c)
;; h += T2
;; and following textual shift {a,b,c,d,e,f,g,h} => {h,a,b,c,d,e,f,g}
;;
%macro   ROUND  6
   Summ1    %4, eax, %6
   Chj      %5, eax, [%2+((vF-%1) & 7)*sizeof_dword], [%2+((vG-%1) & 7)*sizeof_dword]
   mov      eax, [%2+((vH-%1) & 7)*sizeof_dword]
   add      eax, %4
   add      eax, %5
   add      eax, dword [%3+(%1 & 3)*sizeof_dword]

   mov      %4, dword [%2+((vB-%1) & 7)*sizeof_dword]
   mov      %6, dword [%2+((vC-%1) & 7)*sizeof_dword]
   Maj      %5, edx,%4, %6
   Summ0    %4, edx, %6
   lea      edx, [%4+%5]

   add      edx,eax
   add      eax,[%2+((vD-%1) & 7)*sizeof_dword]

   mov      [%2+((vH-%1) & 7)*sizeof_dword],edx
   mov      [%2+((vD-%1) & 7)*sizeof_dword],eax
%endmacro

;;
;; W[i] = Sigma1(W[i-2]) + W[i-7] + Sigma0(W[i-15]) + W[i-16], i=16,..,63
;;
;;for next rounds 16,17,18 and 19:
;; W[0] <= W[16] = Sigma1(W[14]) + W[ 9] + Sigma0(W[1]) + W[0]
;; W[1] <= W[17] = Sigma1(W[15]) + W[10] + Sigma0(W[2]) + W[1]
;; W[2] <= W[18] = Sigma1(W[ 0]) + W[11] + Sigma0(W[3]) + W[1]
;; W[3] <= W[19] = Sigma1(W[ 1]) + W[12] + Sigma0(W[4]) + W[2]
;;
;; the process is repeated exactly because texual round of W[]
;;
;; Sigma1() and Sigma0() functions are defined as following:
;; Sigma1(X) = ROR(X,17)^ROR(X,19)^SHR(X,10)
;; Sigma0(X) = ROR(X, 7)^ROR(X,18)^SHR(X, 3)
;;
%macro   UPDATE_W  8
   pshufd   %6, %5, 11111010b          ;; %6 = {W[15],W[15],W[14],W[14]}
   movdqa   X, %6
   psrld    %6, 10
   psrlq    X, 17
   pxor     %6, X
   psrlq    X, (19-17)
   pxor     %6, X

   pshufd   %7, %2, 10100101b          ;; %7 = {W[2],W[2],W[1],W[1]}
   movdqa   X, %7
   psrld    %7, 3
   psrlq    X, 7
   pxor     %7, X
   psrlq    X, (18-7)
   pxor     %7, X

   pshufd   %1, %2, 01010000b          ;; {W[ 1],W[ 1],W[ 0],W[ 0]}
   pshufd   X, %4, 10100101b           ;; {W[10],W[10],W[ 9],W[ 9]}
   paddd    %1, %6
   paddd    %1, %7
   paddd    %1, X


   pshufd   %6, %1, 10100000b          ;; %6 = {W[1],W[1],W[0],W[0]}
   movdqa   X, %6
   psrld    %6, 10
   psrlq    X, 17
   pxor     %6, X
   psrlq    X, (19-17)
   pxor     %6, X

   movdqa   %7, %3                     ;; %7 = {W[4],W[4],W[3],W[3]}
   palignr  %7, %2, (3*sizeof_dword)
   pshufd   %7, %7, 01010000b
   movdqa   X, %7
   psrld    %7, 3
   psrlq    X, 7
   pxor     %7, X
   psrlq    X, (18-7)
   pxor     %7, X

   movdqa   X, %5
   palignr  X, %4, (3*sizeof_dword)   ;; {W[14],W[13],W[12],W[11]}
   pshufd   %2, %2, 11111010b          ;; {W[ 3],W[ 3],W[ 2],W[ 2]}
   pshufd   X, X, 01010000b            ;; {W[12],W[12],W[11],W[11]}
   paddd    %2, %6
   paddd    %2, %7
   paddd    %2, X

   pshufd   %1, %1, 10001000b          ;; {W[1],W[0],W[1],W[0]}
   pshufd   %2, %2, 10001000b          ;; {W[3],W[2],W[3],W[2]}
   palignr  %2, %1, (2*sizeof_dword)  ;; {W[3],W[2],W[1],W[0]}
   movdqa   %1, %2
%endmacro

align  32
SWP_BYTE:
pByteSwp DB    3,2,1,0, 7,6,5,4, 11,10,9,8, 15,14,13,12


%define   pHash    dword [ebp + 8]
%define   pData    dword [ebp + 12]
%define   dataLen  dword [ebp + 16]
%define   pTbl     dword [ebp + 20]


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; SHA256(Ipp32u digest[], Ipp8u dataBlock[], int dataLen, Ipp32u K_256[])
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global ASM_PFX(UpdateSHA256V8)
ASM_PFX(UpdateSHA256V8):
   push           ebp
   mov            ebp,esp

   push           ebx
   push           esi
   push           edi

   sub            esp, stackSize

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; process next data block
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

sha256_block_loop:

   mov            eax, pHash  ; pointer to the hash
   movdqu         W0, oword [eax]              ; load initial hash value
   movdqu         W4, oword [eax+sizeof_oword]
   movdqu         oword [esp+hashOff], W0
   movdqu         oword [esp+hashOff+sizeof_oword*1], W4

  ;movdqa         XMM_SHUFB_BSWAP, oword [SWP_BYTE] ; load shuffle mask
   LD_ADDR        eax, [SWP_BYTE]
   movdqa         XMM_SHUFB_BSWAP, oword [eax+(pByteSwp-SWP_BYTE)]
   mov            eax, pData     ; pointer to the data block
   mov            regTbl, pTbl   ; pointer to SHA256 table (points K_256[] constants)

   movdqu         W0, oword [eax]       ; load buffer content
   movdqu         W4, oword [eax+sizeof_oword]
   movdqu         W8, oword [eax+sizeof_oword*2]
   movdqu         W12,oword [eax+sizeof_oword*3]

   mov      eax, [esp+hashOff+vE*sizeof_dword]
   mov      edx, [esp+hashOff+vA*sizeof_dword]

   ;; perform 0-3 regular rounds
   pshufb   W0, XMM_SHUFB_BSWAP                    ; swap input
   movdqa   W, W0                                  ; T = W[0-3]
   paddd    W, oword [regTbl+sizeof_oword*0]  ; T += K_SHA256[0-3]
   movdqu   oword [esp+wOff], W
   ROUND    0, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND    1, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND    2, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND    3, (esp+hashOff),(esp+wOff), esi,edi,ecx

   ;; perform next 4-7 regular rounds
   pshufb   W4, XMM_SHUFB_BSWAP                    ; swap input
   movdqa   W, W4                                  ; T = W[4-7]
   paddd    W, oword [regTbl+sizeof_oword*1]  ; T += K_SHA256[4-7]
   movdqu   oword [esp+wOff], W
   ROUND    4, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND    5, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND    6, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND    7, (esp+hashOff),(esp+wOff), esi,edi,ecx

   ;; perform next 8-11 regular rounds
   pshufb   W8, XMM_SHUFB_BSWAP                    ; swap input
   movdqa   W, W8                                  ; T = W[8-11]
   paddd    W, oword [regTbl+sizeof_oword*2]  ; T += K_SHA256[8-11]
   movdqu   oword [esp+wOff], W
   ROUND    8, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND    9, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   10, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   11, (esp+hashOff),(esp+wOff), esi,edi,ecx

   ;; perform next 12-15 regular rounds
   pshufb   W12, XMM_SHUFB_BSWAP                   ; swap input
   movdqa   W, W12                                 ; T = W[12-15]
   paddd    W, oword [regTbl+sizeof_oword*3]  ; T += K_SHA256[12-15]
   movdqu   oword [esp+wOff], W
   ROUND   12, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   13, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   14, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   15, (esp+hashOff),(esp+wOff), esi,edi,ecx

   mov      dword [esp+cntOff], (64-16)        ; init counter
loop_16_63:
   add      regTbl, sizeof_oword*4                ; update SHA_256 pointer

   UPDATE_W    W, W0, W4, W8, W12, SIG1,SIG0,X        ; round: 16*i - 16*i+3
   paddd       W, oword [regTbl+sizeof_oword*0]  ; T += K_SHA256[16-19]
   movdqu      oword [esp+wOff], W
   ROUND   16, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   17, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   18, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   19, (esp+hashOff),(esp+wOff), esi,edi,ecx

   UPDATE_W    W, W4, W8, W12,W0,  SIG1,SIG0,X        ; round: 20*i 20*i+3
   paddd       W, oword [regTbl+sizeof_oword*1]  ; T += K_SHA256[20-23]
   movdqu      oword [esp+wOff], W
   ROUND   20, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   21, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   22, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   23, (esp+hashOff),(esp+wOff), esi,edi,ecx

   UPDATE_W    W, W8, W12,W0, W4,  SIG1,SIG0,X        ; round: 24*i - 24*i+3
   paddd       W, oword [regTbl+sizeof_oword*2]  ; T += K_SHA256[24-27]
   movdqu      oword [esp+wOff], W
   ROUND   24, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   25, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   26, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   27, (esp+hashOff),(esp+wOff), esi,edi,ecx

   UPDATE_W    W, W12,W0, W4, W8,  SIG1,SIG0,X        ; round: 28*i - 28*i+3
   paddd       W, oword [regTbl+sizeof_oword*3]  ; T += K_SHA256[28-31]
   movdqu      oword [esp+wOff], W
   ROUND   28, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   29, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   30, (esp+hashOff),(esp+wOff), esi,edi,ecx
   ROUND   31, (esp+hashOff),(esp+wOff), esi,edi,ecx

   sub         dword [esp+cntOff], 16
   jg          loop_16_63

   mov            eax, pHash  ; pointer to the hash
   movdqu         W0, oword [esp+hashOff]
   movdqu         W4, oword [esp+hashOff+sizeof_oword*1]

   ; update hash
   movdqu         W, oword [eax]
   paddd          W, W0
   movdqu         oword [eax], W
   movdqu         W, oword [eax+sizeof_oword]
   paddd          W, W4
   movdqu         oword [eax+sizeof_oword], W

   add            pData, MBS_SHA256
   sub            dataLen, MBS_SHA256
   jg             sha256_block_loop

   add            esp, stackSize
   pop            edi
   pop            esi
   pop            ebx
   pop            ebp
   ret
