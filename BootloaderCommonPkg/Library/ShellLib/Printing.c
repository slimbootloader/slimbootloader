/** @file
  Helper function for printing from the shell.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ConsoleOutLib.h>

//
// Define the maximum message length that this library supports
//
#define MAX_MESSAGE_LENGTH  0x100

/**
  Prints a message to the serial port.

  If Format is NULL, then ASSERT().

  @param  Format      Format string for the message to print.
  @param  ...         Variable argument list whose contents are accessed
                      based on the format string specified by Format.

**/
UINTN
EFIAPI
ShellPrint (
  IN  CONST CHAR16 *Format,
  ...
  )
{
  CHAR8    Buffer[MAX_MESSAGE_LENGTH];
  VA_LIST  Marker;

  VA_START (Marker, Format);
  AsciiVSPrintUnicodeFormat (Buffer, sizeof (Buffer), Format, Marker);
  VA_END (Marker);
  return ConsoleWrite ((UINT8 *)Buffer, AsciiStrLen (Buffer));
}
