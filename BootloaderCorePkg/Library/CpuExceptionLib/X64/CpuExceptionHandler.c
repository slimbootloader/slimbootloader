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
  IN UINT32        *Stack,
  IN UINT8          ExceptionType
  )
{
  CpuHalt (NULL);
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
}
