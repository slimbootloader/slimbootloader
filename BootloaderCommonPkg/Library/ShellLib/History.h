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

#define  MAX_HISTORY_REC                  8
#define  MAX_COMMAND_LINE_LEN             256
#define  MAX_COMMAND_LINE_LEN_MINI_SHELL  80

/**
  Initialize command line history buffer and index.

  @param[in]  Shell        Shell instance
  @param[in]  IsInit       Indicate initialization or de-initialization

**/
VOID
HistoryInit (
  IN  SHELL    *Shell,
  IN  BOOLEAN   IsInit
  );

/**
  Fill command buffer with next command line history.

  @param[in]  Shell   Shell instance
  @param[in]  Line    Command line buffer to fill

  @retval     Length of the command line returned
**/
UINTN
HistoryDown (
  IN  SHELL       *Shell,
  IN OUT CHAR16   *Line
  );

/**
  Fill command buffer with previous command line history.

  @param[in]  Shell   Shell instance
  @param[in]  Line    Command line buffer to fill

  @retval     Length of the command line returned
**/
UINTN
HistoryUp (
  IN  SHELL     *Shell,
  IN OUT CHAR16 *Line
  );

/**
  Append a command lihe into command line history buffer.

  @param[in]  Shell   Shell instance
  @param[in]  Line    Command line buffer to append

**/
VOID
HistoryAdd (
  IN  SHELL     *Shell,
  IN OUT CHAR16 *Line
  );

#endif
