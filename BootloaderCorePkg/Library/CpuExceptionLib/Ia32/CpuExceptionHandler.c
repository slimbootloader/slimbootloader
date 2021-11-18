/** @file
  IA32 CPU Exception Handler functons.

  Copyright (c) 2012 - 2015, Intel Corporation. All rights reserved.<BR>
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
  IN UINT32        *Stack,
  IN UINT8          ExceptionType
  )
{
  UINT32  *Ptr;

  // Skip the ExceptionType on the stack
  Ptr = Stack + 1;

  // Skip the ErrorCode on the stack
  if (ExceptionType < CPU_EXCEPTION_NUM) {
    if (mErrorCodeFlag & (1 << ExceptionType)) {
      Ptr++;
    }
  }

  DEBUG ((DEBUG_ERROR, "Exception #%d from 0x%04X:0x%08X !!!\n", ExceptionType, Ptr[1], Ptr[0]));
  DumpHex(0, Ptr[0] - 32, 64, (UINT8 *)(Ptr[0] - 32));

  CpuHalt (NULL);
}

/**
  Update exception handler in IDT table .

  This function is used to update the IDT exception handler with current stage.

  @param[in]  IdtDescritor   If non-zero, it is new IDT descriptor to be updated.
                             if it is 0,  the IDT descriptor will be retrieved from IDT base register.

**/
STATIC
VOID
UpdateExceptionHandler2 (
  IN VOID                    *IdtDescriptor
)
{
  UINT64                     *IdtTable;
  IA32_IDT_GATE_DESCRIPTOR    IdtGateDescriptor;
  UINT32                      Index;
  UINT32                      Address;
  IA32_DESCRIPTOR             Idtr;
  EXCEPTION_HANDLER_TEMPLATE_MAP  TemplateMap;

  if (IdtDescriptor == NULL) {
    AsmReadIdtr (&Idtr);
  } else {
    Idtr = *(IA32_DESCRIPTOR *) IdtDescriptor;
  }
  IdtTable                          = (UINT64 *)Idtr.Base;
  IdtGateDescriptor.Uint64          = 0;
  IdtGateDescriptor.Bits.Selector   = AsmReadCs ();
  IdtGateDescriptor.Bits.GateType   = IA32_IDT_GATE_TYPE_INTERRUPT_32;

  AsmGetTemplateAddressMap (&TemplateMap);
  for (Index = 0; Index < CPU_EXCEPTION_NUM; Index ++) {
    Address = TemplateMap.ExceptionStart + TemplateMap.ExceptionStubHeaderSize * Index;
    IdtGateDescriptor.Bits.OffsetHigh = (UINT16)(Address >> 16);
    IdtGateDescriptor.Bits.OffsetLow  = (UINT16)Address;
    IdtTable[Index] = IdtGateDescriptor.Uint64;
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
