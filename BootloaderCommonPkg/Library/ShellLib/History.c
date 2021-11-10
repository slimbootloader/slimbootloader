/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include  "History.h"

/**
  Initialize command line history buffer and index.

  @param[in]  Shell        Shell instance
  @param[in]  IsInit       Indicate initialization or de-initialization

**/
VOID
HistoryInit (
  IN SHELL     *Shell,
  IN BOOLEAN    IsInit
  )
{
  UINTN   BufLen;

  if (IsInit) {
    BufLen = MAX_HISTORY_REC * (Shell->CommandLineMaxLen * sizeof(CHAR16));
    if (Shell->CommandLineHist == NULL) {
      Shell->CommandLineHist = AllocateZeroPool (BufLen);
    } else {
      ZeroMem (Shell->CommandLineHist, BufLen);
    }
  } else {
    if (Shell->CommandLineHist != NULL) {
      FreePool (Shell->CommandLineHist);
      Shell->CommandLineHist = NULL;
    }
  }
  Shell->CommandLineIdx = -1;
}

/**
  Get command line history buffer from index.

  @param[in]  Shell    Shell instance
  @param[in]  Index    Command line history buffer index

  @retval     Pointer to the command line in history buffer
**/
STATIC
CHAR16 *
GetLine (
  IN SHELL     *Shell,
  IN  INTN      Index
)
{
  ASSERT (Shell->CommandLineHist != NULL);

  if ((Index < MAX_HISTORY_REC) && (Index >= 0)) {
    return Shell->CommandLineHist + Index * Shell->CommandLineMaxLen;
  } else {
    return NULL;
  }
}

/**
  Fill command buffer with next command line history.

  @param[in]  Shell   Shell instance
  @param[in]  Line    Command line buffer to fill

  @retval     Length of the command line returned
**/
UINTN
HistoryDown (
  IN SHELL        *Shell,
  IN OUT CHAR16   *Line
  )
{
  Shell->CommandLineIdx -= 1;

  // Find first valid command line history
  while (Shell->CommandLineIdx >= 0) {
    if (StrLen(GetLine (Shell, Shell->CommandLineIdx)) > 0) {
      break;
    }
    Shell->CommandLineIdx--;
  }

  if (Shell->CommandLineIdx < 0)  {
    // -1 indicates the bottom of history buffer
    Shell->CommandLineIdx = -1;
    Line[0] = 0;
    return 0;
  } else {
    StrCpyS (Line, Shell->CommandLineMaxLen, GetLine (Shell, Shell->CommandLineIdx));
    return StrLen (Line);
  }
}

/**
  Fill command buffer with previous command line history.

  @param[in]  Shell   Shell instance
  @param[in]  Line    Command line buffer to fill

  @retval     Length of the command line returned
**/
UINTN
HistoryUp (
  IN SHELL      *Shell,
  IN OUT CHAR16 *Line
  )
{
  Shell->CommandLineIdx ++;
  if (Shell->CommandLineIdx >= MAX_HISTORY_REC) {
    // MAX_HISTORY_REC indicates the top of history buffer
    Shell->CommandLineIdx = MAX_HISTORY_REC;
    Line[0] = 0;
    return 0;
  } else {
    StrCpyS (Line, Shell->CommandLineMaxLen, GetLine (Shell, Shell->CommandLineIdx));
    return StrLen (Line);
  }
}

/**
  Append a command lihe into command line history buffer.

  @param[in]  Shell   Shell instance
  @param[in]  Line    Command line buffer to append

**/
VOID
HistoryAdd (
  IN SHELL      *Shell,
  IN OUT CHAR16 *Line
  )
{
  UINTN   Index;

  Shell->CommandLineIdx = -1;

  if (StrLen (Line) == 0) {
    return;
  }

  // Check if the command line is already in the history buffer
  for (Index = 0; Index < MAX_HISTORY_REC - 1; Index++) {
    if (StrCmp(GetLine (Shell, Index), Line) == 0) {
      break;
    }
  }

  // Move everything backwards to add new command line at index 0
  for (; Index > 0; --Index) {
    StrCpyS (GetLine (Shell, Index), Shell->CommandLineMaxLen, GetLine (Shell, Index - 1));
  }

  StrCpyS (GetLine (Shell, 0), Shell->CommandLineMaxLen, Line);
}

/**
  Print all command lines in history buffer.

  @param[in]  Shell        Shell instance

  This is used for debug only.
**/
VOID
HistoryPrint (
  IN SHELL     *Shell
  )
{
  UINTN  Index;

  for (Index = 0; Index < MAX_HISTORY_REC; Index++) {
    DEBUG ((DEBUG_INFO, "CMD%02d: %s\n", Index, GetLine (Shell, Index)));
  }
  DEBUG ((DEBUG_INFO, "\n"));
}
