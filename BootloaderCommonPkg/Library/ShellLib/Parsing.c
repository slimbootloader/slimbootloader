/** @file
  Functions to help parsing shell commands.

  (C) Copyright 2016 Hewlett Packard Enterprise Development LP<BR>
  Copyright (C) 2014, Red Hat, Inc.
  (C) Copyright 2013-2014 Hewlett-Packard Development Company, L.P.<BR>
  Copyright (c) 2009 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseMemoryLib.h>
#include "Parsing.h"

/**
  Cleans off leading and trailing spaces and tabs.

  @param[in] String pointer to the string to trim them off.
**/
EFI_STATUS
TrimSpaces (
  IN CHAR16 **String
  )
{
  ASSERT (String != NULL);
  ASSERT (*String != NULL);
  //
  // Remove any spaces and tabs at the beginning of the (*String).
  //
  while (((*String)[0] == L' ') || ((*String)[0] == L'\t')) {
    CopyMem ((*String), (*String) + 1, StrSize ((*String)) - sizeof ((*String)[0]));
  }

  //
  // Remove any spaces and tabs at the end of the (*String).
  //
  while ((StrLen (*String) > 0) && (((*String)[StrLen ((*String)) - 1] == L' ')
                                    || ((*String)[StrLen ((*String)) - 1] == L'\t'))) {
    (*String)[StrLen ((*String)) - 1] = CHAR_NULL;
  }

  return (EFI_SUCCESS);
}

/**
  Return the pointer to the first occurrence of any character from a list of characters.

  @param[in] String           the string to parse
  @param[in] CharacterList    the list of character to look for
  @param[in] EscapeCharacter  An escape character to skip

  @return the location of the first character in the string
  @retval CHAR_NULL no instance of any character in CharacterList was found in String
**/
CONST CHAR16 *
FindFirstCharacter (
  IN CONST CHAR16 *String,
  IN CONST CHAR16 *CharacterList,
  IN CONST CHAR16 EscapeCharacter
  )
{
  UINT32 WalkChar;
  UINT32 WalkStr;

  for (WalkStr = 0; WalkStr < StrLen (String); WalkStr++) {
    if (String[WalkStr] == EscapeCharacter) {
      WalkStr++;
      continue;
    }
    for (WalkChar = 0; WalkChar < StrLen (CharacterList); WalkChar++) {
      if (String[WalkStr] == CharacterList[WalkChar]) {
        return (&String[WalkStr]);
      }
    }
  }
  return (String + StrLen (String));
}

/**
  Return the next parameter's end from a command line string.

  @param[in] String        the string to parse
**/
CONST CHAR16 *
FindEndOfParameter (
  IN CONST CHAR16 *String
  )
{
  CONST CHAR16 *First;
  CONST CHAR16 *CloseQuote;

  First = FindFirstCharacter (String, L" \"", L'^');

  //
  // nothing, all one parameter remaining
  //
  if (*First == CHAR_NULL) {
    return (First);
  }

  //
  // If space before a quote (or neither found, i.e. both CHAR_NULL),
  // then that's the end.
  //
  if (*First == L' ') {
    return (First);
  }

  CloseQuote = FindFirstCharacter (First + 1, L"\"", L'^');

  //
  // We did not find a terminator...
  //
  if (*CloseQuote == CHAR_NULL) {
    return (NULL);
  }

  return (FindEndOfParameter (CloseQuote + 1));
}

/**
  Return the next parameter from a command line string.

  This function moves the next parameter from Walker into TempParameter and moves
  Walker up past that parameter for recursive calling.  When the final parameter
  is moved *Walker will be set to NULL;

  Temp Parameter must be large enough to hold the parameter before calling this
  function.

  This will also remove all remaining ^ characters after processing.

  @param[in, out] Walker          pointer to string of command line.  Adjusted to
                                  reminaing command line on return
  @param[in, out] TempParameter   pointer to string of command line item extracted.
  @param[in]      Length          buffer size of TempParameter.
  @param[in]      StripQuotation  if TRUE then strip the quotation marks surrounding
                                  the parameters.

  @return   EFI_INALID_PARAMETER  A required parameter was NULL or pointed to a NULL or empty string.
  @return   EFI_NOT_FOUND         A closing " could not be found on the specified string
**/
EFI_STATUS
GetNextParameter (
  IN OUT CONST CHAR16   **Walker,
  IN OUT       CHAR16   **TempParameter,
  IN CONST     UINTN    Length,
  IN           BOOLEAN  StripQuotation
  )
{
  CONST CHAR16 *NextDelim;

  if (Walker           == NULL
      || *Walker          == NULL
      || TempParameter    == NULL
      || *TempParameter   == NULL
      ) {
    return (EFI_INVALID_PARAMETER);
  }


  //
  // make sure we dont have any leading spaces
  //
  while ((*Walker)[0] == L' ') {
    (*Walker)++;
  }

  //
  // make sure we still have some params now...
  //
  if (StrLen (*Walker) == 0) {
    DEBUG_CODE_BEGIN();
    *Walker        = NULL;
    DEBUG_CODE_END();
    return (EFI_INVALID_PARAMETER);
  }

  NextDelim = FindEndOfParameter (*Walker);

  if (NextDelim == NULL) {
    DEBUG_CODE_BEGIN();
    *Walker        = NULL;
    DEBUG_CODE_END();
    return (EFI_NOT_FOUND);
  }

  StrnCpyS (*TempParameter, Length / sizeof (CHAR16), (*Walker), NextDelim - *Walker);

  //
  // Add a CHAR_NULL if we didnt get one via the copy
  //
  if (*NextDelim != CHAR_NULL) {
    (*TempParameter)[NextDelim - *Walker] = CHAR_NULL;
  }

  //
  // Update Walker for the next iteration through the function
  //
  *Walker = (CHAR16 *)NextDelim;

  //
  // Remove any non-escaped quotes in the string
  // Remove any remaining escape characters in the string
  //
  for (NextDelim = FindFirstCharacter (*TempParameter, L"\"^", CHAR_NULL)
                   ; *NextDelim != CHAR_NULL
       ; NextDelim = FindFirstCharacter (NextDelim, L"\"^", CHAR_NULL)
                     ) {
    if (*NextDelim == L'^') {

      //
      // eliminate the escape ^
      //
      CopyMem ((CHAR16 *)NextDelim, NextDelim + 1, StrSize (NextDelim + 1));
      NextDelim++;
    } else if (*NextDelim == L'\"') {

      //
      // eliminate the unescaped quote
      //
      if (StripQuotation) {
        CopyMem ((CHAR16 *)NextDelim, NextDelim + 1, StrSize (NextDelim + 1));
      } else {
        NextDelim++;
      }
    }
  }

  return EFI_SUCCESS;
}
