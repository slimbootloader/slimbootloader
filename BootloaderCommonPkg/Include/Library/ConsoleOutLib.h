/** @file
  Provide Console output library functions.

  Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __CONSOLE_OUT_LIB_H__
#define __CONSOLE_OUT_LIB_H__

#include <PiPei.h>
#include <Library/BaseLib.h>

//
// NOT RECOMMEND to use __CONSOLE_PRINT directly
//
#define __CONSOLE_PRINT_FUNC(Format, ...)                               \
  do {                                                                  \
    ConsolePrint (Format, ##__VA_ARGS__);                               \
  } while (FALSE)

#define __CONSOLE_PRINT_UNICODE_FUNC(Format, ...)                       \
  do {                                                                  \
    ConsolePrintUnicode (Format, ##__VA_ARGS__);                        \
  } while (FALSE)

#define __CONSOLE_PRINT_CONDITION_FUNC(PrintLevel, Format, ...)         \
  do {                                                                  \
    ConsolePrint (Format, ##__VA_ARGS__);                               \
  } while (FALSE)

#define __CONSOLE_PRINT_UNICODE_CONDITION_FUNC(PrintLevel, Format, ...) \
  do {                                                                  \
    ConsolePrintUnicode (Format, ##__VA_ARGS__);                        \
  } while (FALSE)

#define __CONSOLE_PRINT_CONDITION(Expression)                           \
  __CONSOLE_PRINT_CONDITION_FUNC          Expression

#define __CONSOLE_PRINT_UNICODE_CONDITION(Expression)                   \
  __CONSOLE_PRINT_UNICODE_CONDITION_FUNC  Expression
//
// NOT RECOMMEND to use __CONSOLE_PRINT directly
//

//
// CONSOLE_PRINT will redirect log messages to consoles
// Expression does not have PrintLevel(ErrorLevel).
//
#define CONSOLE_PRINT(Expression)                                       \
  __CONSOLE_PRINT_FUNC           Expression

#define CONSOLE_PRINT_UNICODE(Expression)                               \
  __CONSOLE_PRINT_UNICODE_FUNC   Expression

//
// ErrorLevel is ignored when log message is redirect to consoles.
// Valid only at DEBUG().
//
#define CONSOLE_PRINT_CONDITION(ConsoleOut, Expression)                 \
  do {                                                                  \
    if (ConsoleOut) {                                                   \
      __CONSOLE_PRINT_CONDITION(Expression);                            \
    } else {                                                            \
      DEBUG(Expression);                                                \
    }                                                                   \
  } while (FALSE)

#define CONSOLE_PRINT_UNICODE_CONDITION(ConsoleOut, Expression)         \
  do {                                                                  \
    if (ConsoleOut) {                                                   \
      __CONSOLE_PRINT_UNICODE_CONDITION(Expression);                    \
    } else {                                                            \
      DEBUG(Expression);                                                \
    }                                                                   \
  } while (FALSE)

typedef enum {
  ConsoleOutSerialPort  = BIT0,
  ConsoleOutFrameBuffer = BIT1,
  ConsoleOutDebugPort   = BIT2,
  ConsoleOutAll         = 0xFFFFFFFF,
} CONSOLE_OUT_DEVICE_TYPE;

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
  );

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
  );

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
  );

#endif
