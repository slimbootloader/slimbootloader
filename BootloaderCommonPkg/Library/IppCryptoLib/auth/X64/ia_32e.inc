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

%ifndef __IA_32E_INC__
%define __IA_32E_INC__ 1

%define  IPP_ALIGN_FACTOR   32

%iassign OWORD_size 16 ; octo-word
%iassign QWORD_size 8  ; quad-word
%iassign DWORD_size 4  ; double-word
%idefine sizeof(_x_) _x_%+_size

%macro SAVE_GPR 1-*
  %rep  %0
        push    %1
  %rotate 1
  %endrep
%endmacro

%macro REST_GPR 1-*
  %rep %0
  %rotate -1
        pop    %1
  %endrep
%endmacro

%macro SAVE_XMM 1-*
        sub         rsp, 16 * %0
  %assign i 0
  %rep  %0
        movdqu      [rsp + i * 16], %1
  %rotate 1
  %assign i i+1
  %endrep
%endmacro

%macro REST_XMM 1-*
  %assign i 0
  %rep  %0
        movdqu      %1,  [rsp + i * 16]
  %rotate 1
  %assign i i+1
  %endrep
        add         rsp,  16 * %0
%endmacro

%endif
