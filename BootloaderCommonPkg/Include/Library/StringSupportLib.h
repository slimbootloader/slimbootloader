/** @file
Header file for string function support library

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _STRING_SUPPORT_LIB_H_
#define _STRING_SUPPORT_LIB_H_

#include <Library/BootloaderCommonLib.h>

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
  );

/**
  Trim leading white space for a line.

  @param[in]   Line     The pointer to the line buffer.

  @retval      The pointer to the trimmed line.

**/
CHAR8 *
EFIAPI
TrimLeft (
  IN  CHAR8  *Line
  );

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
  );

/**
  Trim trailing white space for a line.

  @param[in]   Line     The pointer to the line buffer.

  @retval      The pointer to the trimmed line.

**/
CHAR8 *
EFIAPI
TrimRight (
  IN  CHAR8  *Line
  );

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
  );

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
  );

#endif
