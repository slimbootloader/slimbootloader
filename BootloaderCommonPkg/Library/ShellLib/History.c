/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include  "History.h"

CHAR16    *mCommandLineHist;
INTN       mCommandLineIdx;

/**
  Initialize command line history buffer and index.

  @param[in]  IsInit       Indicate initialization or de-initialization

**/
VOID
HistoryInit (
  IN BOOLEAN    IsInit
  )
{
  UINTN   BufLen;

  if (IsInit) {
    BufLen = MAX_HISTORY_REC * MAX_COMMAND_LINE_LEN * sizeof(CHAR16);
    if (mCommandLineHist == NULL) {
      mCommandLineHist = AllocateZeroPool (BufLen);
    } else {
      ZeroMem (mCommandLineHist, BufLen);
    }
  } else {
    if (mCommandLineHist != NULL) {
      FreePool (mCommandLineHist);
      mCommandLineHist = NULL;
    }
  }
  mCommandLineIdx = -1;
}

/**
  Get command line history buffer from index.

  @param[in]  Index    Command line history buffer index

  @retval     Pointer to the command line in history buffer
**/
STATIC
CHAR16 *
GetLine (
  IN  INTN    Index
)
{
  ASSERT (mCommandLineHist != NULL);

  if ((Index < MAX_HISTORY_REC) && (Index >= 0)) {
    return mCommandLineHist + Index * MAX_COMMAND_LINE_LEN;
  } else {
    return NULL;
  }
}

/**
  Fill command buffer with next command line history.

  @param[in]  Line    command line buffer to fill

  @retval     Length of the command line returned
**/
UINTN
HistoryDown (
  IN OUT CHAR16   *Line
  )
{
  mCommandLineIdx -= 1;

  // Find first valid command line history
  while (mCommandLineIdx >= 0) {
    if (StrLen(GetLine (mCommandLineIdx)) > 0) {
      break;
    }
    mCommandLineIdx--;
  }

  if (mCommandLineIdx < 0)  {
    // -1 indicates the bottom of history buffer
    mCommandLineIdx = -1;
    Line[0] = 0;
    return 0;
  } else {
    StrCpyS (Line, MAX_COMMAND_LINE_LEN, GetLine (mCommandLineIdx));
    return StrLen (Line);
  }
}

/**
  Fill command buffer with previous command line history.

  @param[in]  Line    Command line buffer to fill

  @retval     Length of the command line returned
**/
UINTN
HistoryUp (
  IN OUT CHAR16 *Line
  )
{
  mCommandLineIdx ++;
  if (mCommandLineIdx >= MAX_HISTORY_REC) {
    // MAX_HISTORY_REC indicates the top of history buffer
    mCommandLineIdx = MAX_HISTORY_REC;
    Line[0] = 0;
    return 0;
  } else {
    StrCpyS (Line, MAX_COMMAND_LINE_LEN, GetLine (mCommandLineIdx));
    return StrLen (Line);
  }
}

/**
  Append a command lihe into command line history buffer.

  @param[in]  Line    Command line buffer to append

**/
VOID
HistoryAdd (
  IN OUT CHAR16 *Line
  )
{
  UINTN   Index;

  mCommandLineIdx = -1;

  if (StrLen (Line) == 0) {
    return;
  }

  // Check if the command line is already in the history buffer
  for (Index = 0; Index < MAX_HISTORY_REC - 1; Index++) {
    if (StrCmp(GetLine (Index), Line) == 0) {
      break;
    }
  }

  // Move everything backwards to add new command line at index 0
  for (; Index > 0; --Index) {
    StrCpyS (GetLine (Index), MAX_COMMAND_LINE_LEN, GetLine (Index - 1));
  }

  StrCpyS (GetLine (0), MAX_COMMAND_LINE_LEN, Line);
}

/**
  Print all command lines in history buffer.

  This is used for debug only.
**/
VOID
HistoryPrint (
  VOID
  )
{
  UINTN  Index;

  for (Index = 0; Index < MAX_HISTORY_REC; Index++) {
    DEBUG ((DEBUG_INFO, "CMD%02d: %s\n", Index, GetLine (Index)));
  }
  DEBUG ((DEBUG_INFO, "\n"));
}
