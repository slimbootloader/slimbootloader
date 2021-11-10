/** @file
  String support library implementation.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/StringSupportLib.h>

/**
  Get next line start pointer.

  @param[in]   Start    The pointer for current line.
  @param[out]  Length   The UINT32 pointer to receive the length of current line.

  @retval      The pointer for next line.
               NULL if no more line is available.
**/
CHAR8 *
EFIAPI
GetNextLine (
  IN   CHAR8    *Start,
  OUT  UINT32   *Length
  )
{
  CHAR8 *Ptr;

  if (Start == NULL) {
    return NULL;
  }

  Ptr = Start;
  while ((Ptr[0] != 0) && (Ptr[0] != '\n')) {
    Ptr++;
  }

  if (Length != NULL){
    *Length = (UINT32)(Ptr - Start);
  }

  if (Ptr[0] == 0) {
    return NULL;
  } else {
    return Ptr + 1;
  }
}


/**
  Trim leading white space for a line.

  @param[in]   Line     The pointer to the line buffer.

  @retval      The pointer to the trimmed line.

**/
CHAR8 *
EFIAPI
TrimLeft (
  IN  CHAR8  *Line
  )
{
  if (Line == NULL) {
    return NULL;
  }

  while ((Line[0] == ' ') || (Line[0] == '\t')) {
    Line++;
  }
  return Line;
}


/**
  Get next space for the start of a line.

  @param[in]   LineStart   The pointer to the start of the line buffer.
  @param[in]   LineEnd     The pointer to the end of the line buffer.

  @retval      The pointer to the trimmed line.

**/
CHAR8 *
EFIAPI
GetNextSpace (
  IN  CHAR8  *LineStart,
  IN  CHAR8  *LineEnd
  )
{
  CHAR8  *Line;

  if ((LineStart == NULL) || (LineEnd == NULL)) {
    return NULL;
  }

  Line = LineStart;
  while ((Line[0] != 0) && (Line[0] != ' ') && (Line[0] != '\t') && (Line < LineEnd)) {
    Line++;
  }
  return Line;
}


/**
  Trim trailing white space for a line.

  @param[in]   Line     The pointer to the line buffer.

  @retval      The pointer to the trimmed line.

**/
CHAR8 *
EFIAPI
TrimRight (
  IN  CHAR8  *Line
  )
{
  if (Line == NULL) {
    return NULL;
  }

  while ((Line[0] == ' ') || (Line[0] == '\t') || (Line[0] == '\r') || (Line[0] == '\n')) {
    Line--;
  }
  return Line;
}


/**
  Check if a line starts with a known keyword.

  @param[in]   Line     The pointer to the line buffer.
  @param[in]   Keyword  The pointer to the keyword.

  @retval      Length of the matched keyword.
               0 if there is no match.

**/
UINT32
EFIAPI
MatchKeyWord (
  IN  CHAR8  *Line,
  IN  CHAR8  *Keyword
  )
{
  UINT32 KeywordLen;

  if ((Line == NULL) || (Keyword == NULL)) {
    return 0;
  }

  KeywordLen = (UINT32)AsciiStrLen (Keyword);
  if (AsciiStrnCmp (Line, Keyword, KeywordLen) == 0) {
    // A separator is required to follow the keyword
    if (((Line[KeywordLen] == 0)) || (Line[KeywordLen] == ' ') || (Line[KeywordLen] == '\t')) {
      return KeywordLen;
    }
  }
  return 0;
}


/**
  Check if a line matches varaible assignment syntax.

  @param[in]   Line      The pointer to the line buffer.
  @param[in]   Variable  The pointer to the variable name.

  @retval      Length of the matched variable name.
               0 if there is no match.

**/
UINT32
EFIAPI
MatchAssignment (
  IN  CHAR8  *Line,
  IN  CHAR8  *Variable
  )
{
  UINT32 VarLen;

  if ((Line == NULL) || (Variable == NULL)) {
    return 0;
  }

  VarLen = (UINT32)AsciiStrLen (Variable);
  if (AsciiStrnCmp (Line, Variable, VarLen) == 0) {
    if (Line[VarLen] == '=') {
      return VarLen;
    }
  }
  return 0;
}

