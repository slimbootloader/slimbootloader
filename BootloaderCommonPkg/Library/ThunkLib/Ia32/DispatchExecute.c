/** @file
  Execute 32-bit code in Long Mode.
  Provide a thunk function to transition from long mode to compatibility mode to execute 32-bit code and then transit
  back to long mode.

  Copyright (c) 2014 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PagingLib.h>

typedef EFI_STATUS (EFIAPI *EXECUTE_32BIT_CODE) (UINT32 Param1, UINT32 Param2);

/**
  Wrapper for a thunk to switch to long mode to execute 64-bit code and then
  switch back to 32-bit mode.

  @param[in] Function     The 64bit code entry to be executed.
  @param[in] Param1       The first parameter to pass to 64bit code.
  @param[in] Param2       The second parameter to pass to 64bit code.
  @param[in] Unused       Unused.

  @return Status.         Status returned from the calling function.
**/
EFI_STATUS
EFIAPI
AsmExecute64BitCode (
  IN UINT64           Function,
  IN UINT64           Param1,
  IN UINT64           Param2,
  IN UINT64           Unused
  );

/**
  Wrapper for a thunk  to transition from long mode to compatibility mode to execute 32-bit code and then transit back to
  long mode.

  @param[in] Function     The 32bit code entry to be executed.
  @param[in] Param1       The first parameter to pass to 32bit code.
  @param[in] Param2       The second parameter to pass to 32bit code.
  @param[in] ExeInMem     If thunk needs to be executed from memory copy.

  @return Status.         Status returned from the calling function.
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
  EFI_STATUS            Status;
  EXECUTE_32BIT_CODE    Func;

  Func   = (EXECUTE_32BIT_CODE)(UINTN)Function;
  Status = Func ((UINT32)Param1, (UINT32)Param2);

  return Status;
}

/**
  Jump into funciton in X64 mode.
  This function will return back to 32bit mode.

  @param[in] Function     The 64bit code entry to be executed.
  @param[in] Param1       The first parameter to pass to 64bit code.
  @param[in] Param2       The second parameter to pass to 64bit code.
  @param[in] ExeInMem     Not used.

  @return EFI_SUCCESS     Dummy return value.
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
  EFI_STATUS       Status;

  LoadPageTableForLongMode ();

  Status = AsmExecute64BitCode (
              (UINT64)(UINTN)Function,
              (UINT64)(UINTN)Param1,
              (UINT64)(UINTN)Param2,
              (UINT64)(UINTN)0
           );

  return Status;
}

