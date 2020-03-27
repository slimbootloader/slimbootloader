;------------------------------------------------------------------------------
;
; Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   MpFuncs.asm
;
; Abstract:
;
;   This is the assembly code for initializing the APs in IA32 Protected Mode
;
;-------------------------------------------------------------------------------
SECTION .text

;-------------------------------------------------------------------------------------
;  AsmGetAddressMap (&AddressMap);
;-------------------------------------------------------------------------------------
;typedef struct {
;  UINT8 *RendezvousFunnelAddress;
;  UINT32 CodeSize;
;  UINT32 MpDataSize;
;  UINT32 ProtModeStartOffset;
;  UINT32 ProtModeJmpPatchOffset;
;} MP_ASSEMBLY_ADDRESS_MAP;
global ASM_PFX(AsmGetAddressMap)
ASM_PFX(AsmGetAddressMap):
   ret

;-------------------------------------------------------------------------------------
;  AsmGetBspSelectors (&BspSelectors);
;-------------------------------------------------------------------------------------
;typedef struct {
;  UINT16 CSSelector;
;  UINT16 DSSelector;
;  UINT16 ESSelector;
;  UINT16 SSSelector;
;  UINT16 FSSelector;
;  UINT16 GSSelector;
;  UINT   Gdtr;
;  UINT   Idtr;
;} MP_BSP_SELECTORS;
global ASM_PFX(AsmGetBspSelectors)
ASM_PFX(AsmGetBspSelectors):
  ret

global ASM_PFX(AsmCliHlt)
ASM_PFX(AsmCliHlt):
  cli
  hlt
  jmp         $-2

global ASM_PFX(AsmMtrrSynchUpEntry)
ASM_PFX(AsmMtrrSynchUpEntry):
  ret

global ASM_PFX(AsmMtrrSynchUpExit)
ASM_PFX(AsmMtrrSynchUpExit):
  ret
