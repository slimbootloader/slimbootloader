/** @file
  Thunk to 32 bit mode for execution from Long Mode.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _THUNK_LIB_H_
#define _THUNK_LIB_H_

#include <PiPei.h>
#include <Library/BaseLib.h>

/**
  Wrapper for a thunk  to transition from long mode to compatibility mode to execute 32-bit code and then transit back to
  long mode.

  @param[in] Function     The 32bit code entry to be executed.
  @param[in] Param1       The first parameter to pass to 32bit code.
  @param[in] Param2       The second parameter to pass to 32bit code.
  @param[in] ExeInMem     If thunk needs to be executed from memory copy.

  @return EFI_UNSUPPORTED.   This should not be called in 32 bit mode.
**/
EFI_STATUS
EFIAPI
Execute32BitCode (
  IN UINT64      Function,
  IN UINT64      Param1,
  IN UINT64      Param2,
  IN BOOLEAN     ExeInMem
  );

/**
  Jump into funciton in X64 mode.
  This function will not return.

  @param[in] Function     The 32bit code entry to be executed.
  @param[in] Param1       The first parameter to pass to 32bit code.
  @param[in] Param2       The second parameter to pass to 32bit code.
  @param[in] ExeInMem     If thunk needs to be executed from memory copy.

  @return EFI_SUCCESS.
**/
EFI_STATUS
EFIAPI
Execute64BitCode (
  IN UINT64      Function,
  IN UINT64      Param1,
  IN UINT64      Param2,
  IN BOOLEAN     ExeInMem
  );


#endif
