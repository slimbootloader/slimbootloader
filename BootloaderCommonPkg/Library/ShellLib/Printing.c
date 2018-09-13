/** @file
  Helper function for printing from the shell. It uses PrintLib to send debug
  messages to serial port device.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SerialPortLib.h>

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
VOID
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
  SerialPortWrite ((UINT8 *)Buffer, AsciiStrLen (Buffer));
}
