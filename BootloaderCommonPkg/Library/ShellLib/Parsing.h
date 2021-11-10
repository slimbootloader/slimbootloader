/** @file
  Functions to help parsing shell commands.

  (C) Copyright 2016 Hewlett Packard Enterprise Development LP<BR>
  Copyright (C) 2014, Red Hat, Inc.
  (C) Copyright 2013-2014 Hewlett-Packard Development Company, L.P.<BR>
  Copyright (c) 2009 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent


**/

#ifndef _PARSING_H_
#define _PARSING_H_

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

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
  );

/**
  Cleans off leading and trailing spaces and tabs.

  @param[in] String pointer to the string to trim them off.
**/
EFI_STATUS
TrimSpaces (
  IN CHAR16 **String
  );

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
  );

#endif //_PARSING_H_
