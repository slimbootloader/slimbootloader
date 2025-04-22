/** @file
  X64 CPU Exception Handler functons.

  Copyright (c) 2012 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <CpuExceptionCommon.h>

CONST UINT32 mErrorCodeFlag = 0x00027d00;

/**
  Common exception handler.

  It will print out the location where exception occured and then halt the system.
  This function will never return.

  @param[in] Stack          Current stack address pointer.
  @param[in] ExceptionType  Exception type code.
**/
VOID
EFIAPI
CommonExceptionHandler (
  IN UINTN         *Stack,
  IN UINT8          ExceptionType
  )
{
  UINTN  *Ptr;

  // Skip the ExceptionType on the stack
  Ptr = Stack + 1;

  // Skip the ErrorCode on the stack
  if (ExceptionType < CPU_EXCEPTION_NUM) {
    if (mErrorCodeFlag & (1 << ExceptionType)) {
      Ptr++;
    }
  }

  DEBUG ((DEBUG_ERROR, "Exception #%d from 0x%04X:0x%08X !!!\n", ExceptionType, (UINT32)Ptr[1], (UINT32)Ptr[0]));
  DumpHex(0, Ptr[0] - 32, 64, (UINT8 *)(Ptr[0] - 32));

  CpuHalt (NULL);
}

/**
  Update exception handler in IDT table .

  This function is used to update the IDT exception handler with current stage.

  @param[in]  IdtDescritor   If non-zero, it is new IDT descriptor to be updated.
                             if it is 0,  the IDT descriptor will be retrieved from IDT base register.

**/
VOID
EFIAPI
UpdateExceptionHandler2 (
  IN VOID                    *IdtDescriptor
)
{
  IA32_IDT_GATE_DESCRIPTOR       *IdtEntry;
  VOID                           *InterruptHandler;
  UINT32                          Index;
  IA32_DESCRIPTOR                 Idtr;
  EXCEPTION_HANDLER_TEMPLATE_MAP  TemplateMap;
  UINT16                          Cs;

  if (IdtDescriptor == NULL) {
    AsmReadIdtr (&Idtr);
  } else {
    Idtr = *(IA32_DESCRIPTOR *) IdtDescriptor;
  }

  Cs = AsmReadCs ();
  AsmGetTemplateAddressMap (&TemplateMap);
  IdtEntry = (IA32_IDT_GATE_DESCRIPTOR *)Idtr.Base;
  for (Index = 0; Index < CPU_EXCEPTION_NUM; Index ++) {
    InterruptHandler = (VOID *)(UINTN)(TemplateMap.ExceptionStart + TemplateMap.ExceptionStubHeaderSize * Index);
    IdtEntry->Uint128.Uint64   = 0;
    IdtEntry->Uint128.Uint64_1 = 0;
    IdtEntry->Bits.OffsetLow   = (UINT16)(UINTN)InterruptHandler;
    IdtEntry->Bits.OffsetHigh  = (UINT16)((UINTN)InterruptHandler >> 16);
    IdtEntry->Bits.OffsetUpper = (UINT32)((UINTN)InterruptHandler >> 32);
    IdtEntry->Bits.Selector    = Cs;
    IdtEntry->Bits.GateType    = IA32_IDT_GATE_TYPE_INTERRUPT_32;
    IdtEntry++;
  }

  if (IdtDescriptor != NULL) {
    AsmWriteIdtr (&Idtr);
  }
}

/**
  Update exception handler in IDT table .

  This function is used to update the IDT exception handler with current stage.

  @param[in]  IdtDescriptor   If not NULL, it is new IDT descriptor to be updated.
                              if it is NULL, the IDT descriptor will be retrieved from IDT base register.

**/
VOID
UpdateExceptionHandler (
  IN IA32_DESCRIPTOR         *IdtDescriptor
)
{
  UpdateDebugAgentIdt (IdtDescriptor, UpdateExceptionHandler2);
}
