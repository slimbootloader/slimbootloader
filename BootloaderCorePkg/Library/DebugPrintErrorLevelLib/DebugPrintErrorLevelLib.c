/** @file
  Debug Print Error Level library instance that retrieves the current error
  level from PcdDebugPrintErrorLevel.  This generic library instance does not
  support the setting of the global debug print error level mask for the platform.

  Copyright (c) 2011 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/BootloaderCoreLib.h>

/**
  Returns the debug print error level mask for the current module.

  @return  Debug print error level mask for the current module.

**/
UINT32
EFIAPI
GetDebugPrintErrorLevel (
  VOID
  )
{
  return GetDebugErrorLevel ();
}

/**
  Sets the global debug print error level mask fpr the entire platform.

  @param   ErrorLevel     Global debug print error level.

  @retval  TRUE           The debug print error level mask was sucessfully set.
  @retval  FALSE          The debug print error level mask could not be set.

**/
BOOLEAN
EFIAPI
SetDebugPrintErrorLevel (
  UINT32  ErrorLevel
  )
{
  SetDebugErrorLevel (ErrorLevel);
  return TRUE;
}
