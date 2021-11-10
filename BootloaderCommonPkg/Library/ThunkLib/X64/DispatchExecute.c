/** @file
  Execute 32-bit code in Long Mode.
  Provide a thunk function to transition from long mode to compatibility mode to execute 32-bit code and then transit
  back to long mode.

  Copyright (c) 2014 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BlMemoryAllocationLib.h>

typedef EFI_STATUS (EFIAPI *EXECUTE_64BIT_CODE) (UINT64 Param1, UINT64 Param2);

#pragma pack(1)
typedef union {
  struct {
    UINT32  LimitLow    : 16;
    UINT32  BaseLow     : 16;
    UINT32  BaseMid     : 8;
    UINT32  Type        : 4;
    UINT32  System      : 1;
    UINT32  Dpl         : 2;
    UINT32  Present     : 1;
    UINT32  LimitHigh   : 4;
    UINT32  Software    : 1;
    UINT32  Reserved    : 1;
    UINT32  DefaultSize : 1;
    UINT32  Granularity : 1;
    UINT32  BaseHigh    : 8;
  } Bits;
  UINT64  Uint64;
} IA32_GDT;
#pragma pack()


/**
  Wrapper for a thunk  to transition from long mode to compatibility mode to
  execute 32-bit code and then transit back to long mode.

  @param[in] Function     The 32bit code entry to be executed.
  @param[in] Param1       The first parameter to pass to 32bit code.
  @param[in] Param2       The second parameter to pass to 32bit code.
  @param[in] ExeInMem     If thunk needs to be executed from memory copy.

  @return Status.         Status returned from the calling function.
**/
UINT32
EFIAPI
AsmExecute32BitCode (
  IN UINT64           Function,
  IN UINT64           Param1,
  IN UINT64           Param2,
  IN IA32_DESCRIPTOR  *InternalGdtr
  );

/**
  Assembly function to get the length of AsmExecute32BitCode() function

  @return  The length of AsmExecute32BitCode() function.
**/
UINT32
EFIAPI
AsmGetExecute32CodeLength (
  VOID
);

typedef EFI_STATUS (EFIAPI *EXECUTE_32BIT_CODE) \
        (UINT64  Function, UINT64 Param1, UINT64 Param2, IA32_DESCRIPTOR  *InternalGdtr);

/**
  Wrapper for a thunk  to transition from long mode to compatibility mode to execute 32-bit code and then transit back to
  long mode.

  @param[in] Function     The 32bit code entry to be executed.
  @param[in] Param1       The first parameter to pass to 32bit code.
  @param[in] Param2       The second parameter to pass to 32bit code.
  @param[in] ExeInMem     If thunk needs to be executed from memory copy.

  @return EFI_STATUS.
**/
EFI_STATUS
EFIAPI
Execute32BitCode (
  IN UINT64      Function,
  IN UINT64      Param1,
  IN UINT64      Param2,
  IN BOOLEAN     ExeInMem
  )
{
  EFI_STATUS       Status;
  IA32_DESCRIPTOR  Idtr;
  IA32_DESCRIPTOR  IdtrNul;
  IA32_DESCRIPTOR  Gdtr;
  EXECUTE_32BIT_CODE ExecuteCode;

  //
  // Idtr might be changed inside of FSP. 32bit FSP only knows the <4G address.
  // If IDTR.Base is >4G, FSP can not handle. So we need save/restore IDTR here for X64 only.
  // Interrupt is already disabled here, so it is safety to update IDTR.
  //
  AsmReadGdtr (&Gdtr);
  if (ExeInMem && (Param1 == 0) && (Param2 == 0)) {
    // For TempRamInit in XIP, it might be running from temp memory.
    // To be safe, need to copy the thunk code into memory for execution to prevent crash.
    ExecuteCode = (EXECUTE_32BIT_CODE)(UINTN)AllocateTemporaryMemory (0);
    CopyMem ((VOID *)(UINTN)ExecuteCode, (VOID *)(UINTN)AsmExecute32BitCode, AsmGetExecute32CodeLength());
  } else {
    ExecuteCode = (EXECUTE_32BIT_CODE)AsmExecute32BitCode;
  }

  AsmReadIdtr (&Idtr);
  // Let FSP to setup the IDT
  IdtrNul.Base  = 0x0000;
  IdtrNul.Limit = 0xFFFF;
  AsmWriteIdtr (&IdtrNul);
  Status = ExecuteCode (Function, Param1, Param2, &Gdtr);
  AsmWriteIdtr (&Idtr);

  return Status;
}


/**
  Call into funciton in X64 mode.

  @param[in] Function     The 32bit code entry to be executed.
  @param[in] Param1       The first parameter to pass to 32bit code.
  @param[in] Param2       The second parameter to pass to 32bit code.
  @param[in] ExeInMem     If thunk needs to be executed from memory copy.

  @return EFI_STATUS.
**/
EFI_STATUS
EFIAPI
Execute64BitCode (
  IN UINT64      Function,
  IN UINT64      Param1,
  IN UINT64      Param2,
  IN BOOLEAN     ExeInMem
  )
{
  EFI_STATUS            Status;
  EXECUTE_64BIT_CODE    Func;

  Func   = (EXECUTE_64BIT_CODE)(UINTN)Function;
  Status = Func (Param1, Param2);

  return Status;
}
