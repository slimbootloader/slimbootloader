/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _HISTORY_H_
#define _HISTORY_H_

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ShellLib.h>

#define   MAX_HISTORY_REC    32

/**
  Initialize command line history buffer and index.

  @param[in]  IsInit       Indicate initialization or de-initialization

**/
VOID
HistoryInit (
  IN  BOOLEAN  IsInit
  );

/**
  Fill command buffer with next command line history.

  @param[in]  Line    command line buffer to fill

  @retval     Length of the command line returned
**/
UINTN
HistoryDown (
  IN OUT CHAR16   *Line
  );

/**
  Fill command buffer with previous command line history.

  @param[in]  Line    Command line buffer to fill

  @retval     Length of the command line returned
**/
UINTN
HistoryUp (
  IN OUT CHAR16 *Line
  );

/**
  Append a command lihe into command line history buffer.

  @param[in]  Line    Command line buffer to append

**/
VOID
HistoryAdd (
  IN OUT CHAR16 *Line
  );

#endif