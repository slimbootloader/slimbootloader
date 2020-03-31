/** @file
  Execute 32-bit code in Long Mode.
  Provide a thunk function to transition from long mode to compatibility mode to execute 32-bit code and then transit
  back to long mode.

  Copyright (c) 2014 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <FspEas.h>


/**
  Wrapper for a thunk  to transition from long mode to compatibility mode to execute 32-bit code and then transit back to
  long mode.

  @param[in] Function     The 32bit code entry to be executed.
  @param[in] Param1       The first parameter to pass to 32bit code.
  @param[in] Param2       The second parameter to pass to 32bit code.

  @return EFI_STATUS.
**/
EFI_STATUS
Execute32BitCode (
  IN UINT64      Function,
  IN UINT64      Param1,
  IN UINT64      Param2
  )
{
  return EFI_SUCCESS;
}

