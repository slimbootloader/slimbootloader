/** @file
  Provide Console output library functions.

  Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Guid/LoaderPlatformDataGuid.h>
#include <Library/GraphicsLib.h>
#include <Library/PrintLib.h>
#include <Library/SerialPortLib.h>
#include <Library/HobLib.h>
#include <Library/ConsoleOutLib.h>
#include <Library/DebugPortLib.h>

/**
  Write data from buffer to the console.

  Writes NumberOfBytes data bytes from Buffer to the virtual console.

  If Buffer is NULL, then ASSERT().

  If NumberOfBytes is zero, then return 0.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to write to the console.

  @retval >0               The number of bytes written to the console.
**/
UINTN
EFIAPI
ConsoleWrite (
  IN UINT8     *Buffer,
  IN UINTN      NumberOfBytes
  )
{
  if (PcdGet32 (PcdConsoleOutDeviceMask) & ConsoleOutSerialPort) {
    SerialPortWrite ((UINT8 *)Buffer, NumberOfBytes);
  }

  if (PcdGet32 (PcdConsoleOutDeviceMask) & ConsoleOutFrameBuffer) {
    FrameBufferWrite ((UINT8 *)Buffer, NumberOfBytes);
  }

  if (PcdGet32 (PcdConsoleOutDeviceMask) & ConsoleOutDebugPort) {
    DebugPortWrite ((UINT8 *)Buffer, NumberOfBytes);
  }

  return NumberOfBytes;
}

/**
  Print Ascii debug message to consoles

  If Format is NULL, then ASSERT().

  @param  Format      The format string for the debug message to print.
  @param  ...         The variable argument list whose contents are accessed
                      based on the format string specified by Format.

**/
UINTN
EFIAPI
ConsolePrint (
  IN  CONST CHAR8          *Format,
  ...
  )
{
  CHAR8    Buffer[MAX_MESSAGE_LENGTH];
  VA_LIST  Marker;
  UINTN    Length;

  VA_START (Marker, Format);
  Length = AsciiVSPrint (Buffer, sizeof (Buffer), Format, Marker);
  VA_END (Marker);

  return ConsoleWrite ((UINT8 *)Buffer, Length);
}

/**
  Print Unicode debug message to consoles

  If Format is NULL, then ASSERT().

  @param  Format      The format string for the debug message to print.
  @param  ...         The variable argument list whose contents are accessed
                      based on the format string specified by Format.

**/
UINTN
EFIAPI
ConsolePrintUnicode (
  IN  CONST CHAR16         *Format,
  ...
  )
{
  CHAR8    Buffer[MAX_MESSAGE_LENGTH];
  VA_LIST  Marker;
  UINTN    Length;

  VA_START (Marker, Format);
  Length = AsciiVSPrintUnicodeFormat (Buffer, sizeof (Buffer), Format, Marker);
  VA_END (Marker);

  return ConsoleWrite ((UINT8 *)Buffer, Length);
}
