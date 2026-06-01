/** @file
  Input Handler - keyboard input and escape sequence translation.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "InputHandler.h"
#include <Library/ConsoleInLib.h>
#include <Library/PcdLib.h>
#include <Library/SerialPortLib.h>

#define ESC  0x1B

STATIC BOOLEAN  mPendingByteValid = FALSE;
STATIC UINT8    mPendingByte = 0;

STATIC
VOID
TraceRaw (
  IN UINT8  Value
  )
{
  (VOID)Value;
}

STATIC
VOID
TraceKey (
  IN UINT16  Key
  )
{
  (VOID)Key;
}

/**
  Initialize input handler.
**/
EFI_STATUS
InputInit (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Read one raw byte from console (blocking).

  Uses serial port input exclusively. Polls serial port non-blockingly
  in a spin loop until data arrives.
**/
STATIC
UINT8
ReadChar (
  VOID
  )
{
  UINT8  Ch;

  if (mPendingByteValid) {
    mPendingByteValid = FALSE;
    return mPendingByte;
  }

  Ch = 0;
  for (;;) {
    if ((PcdGet32 (PcdConsoleInDeviceMask) & ConsoleInSerialPort) != 0) {
      if (SerialPortPoll ()) {
        if (SerialPortRead (&Ch, 1) == 1) {
          return Ch;
        }
      }
    }

    if (ConsolePoll () && (ConsoleRead (&Ch, 1) == 1)) {
      return Ch;
    }
  }
}

/**
  Try to read one raw byte from console within a bounded poll loop.

  @retval TRUE   A byte was read and stored in Ch.
  @retval FALSE  No byte became available before timeout.
**/
STATIC
BOOLEAN
ReadCharWithTimeout (
  IN  UINTN  SpinCount,
  OUT UINT8  *Ch
  )
{
  if (Ch == NULL) {
    return FALSE;
  }

  if (mPendingByteValid) {
    *Ch = mPendingByte;
    mPendingByteValid = FALSE;
    return TRUE;
  }

  for (; SpinCount > 0; SpinCount--) {
    if ((PcdGet32 (PcdConsoleInDeviceMask) & ConsoleInSerialPort) != 0) {
      if (SerialPortPoll () && (SerialPortRead (Ch, 1) == 1)) {
        return TRUE;
      }
    }

    if (ConsolePoll () && (ConsoleRead (Ch, 1) == 1)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Drain any trailing CR/LF bytes that a line-mode client (e.g. Windows telnet)
  appends after each keystroke (CH + CR + LF).

  Spins up to ~200k iterations (roughly 1-2 ms under QEMU TCG) waiting for
  the bytes to arrive in the UART FIFO, then discards all 0x0D / 0x0A bytes.
  Stops immediately on any non-CR/LF byte and stores it in mPendingByte so the
  next ReadCharWithTimeout() returns it.
**/
STATIC
VOID
DrainCrLf (
  VOID
  )
{
  UINT8  B;
  UINTN  SpinLeft;

  for (SpinLeft = 200000; SpinLeft > 0; SpinLeft--) {
    B = 0;
    if ((PcdGet32 (PcdConsoleInDeviceMask) & ConsoleInSerialPort) != 0 &&
        SerialPortPoll ()) {
      if (SerialPortRead (&B, 1) < 1) {
        continue;
      }
      if (B != 0x0D && B != 0x0A) {
        mPendingByte = B;
        mPendingByteValid = TRUE;
        break;
      }
      SpinLeft = 200000;
      continue;
    }
    if (ConsolePoll ()) {
      if (ConsoleRead (&B, 1) < 1) {
        continue;
      }
      if (B != 0x0D && B != 0x0A) {
        mPendingByte = B;
        mPendingByteValid = TRUE;
        break;
      }
      SpinLeft = 200000;
      continue;
    }
  }
}

/**
  Blocking read of a key, translating ANSI escape sequences.

  Escape sequences handled:
    ESC [ A  = Up
    ESC [ B  = Down
    ESC [ C  = Right
    ESC [ D  = Left
    ESC [ H  = Home
    ESC [ F  = End
    ESC [ 5~ = PgUp
    ESC [ 6~ = PgDn
    ESC O P  = F1
    ESC O Q  = F2
    ESC O R  = F3
    ESC O S  = F4
    ESC [ 1 5 ~ = F5
    ESC [ 1 7 ~ = F6
    ESC [ 1 8 ~ = F7
    ESC [ 1 9 ~ = F8
    ESC [ 2 0 ~ = F9
    ESC [ 2 1 ~ = F10

  @return Key code (UI_KEY_UP, UI_KEY_ENTER, UI_KEY_ESC, etc.)
**/
UINT16
InputGetKey (
  VOID
  )
{
  UINT8   Ch;
  UINT8   Ch2;
  UINT8   Ch3;
  UINT8   Param[8];
  UINT8   ParamLen;
  UINT8   Final;
  UINT16  Number;
  UINTN   Idx;
  UINT16  Key;

  Ch = ReadChar ();
  TraceRaw (Ch);

  if (Ch == ESC) {
    // Some terminals can emit doubled ESC before CSI/SS3 sequences.
    if (!ReadCharWithTimeout (200000, &Ch2)) {
      TraceKey (UI_KEY_ESC);
      return UI_KEY_ESC;
    }

    TraceRaw (Ch2);
    while (Ch2 == ESC) {
      if (!ReadCharWithTimeout (200000, &Ch2)) {
        TraceKey (UI_KEY_ESC);
        return UI_KEY_ESC;
      }

      TraceRaw (Ch2);
    }

    if (Ch2 == '[') {
      Ch3 = ReadChar ();
      TraceRaw (Ch3);
      switch (Ch3) {
        case 'A':
          Key = UI_KEY_UP;
          TraceKey (Key);
          DrainCrLf ();
          return Key;
        case 'B':
          Key = UI_KEY_DOWN;
          TraceKey (Key);
          DrainCrLf ();
          return Key;
        case 'C':
          Key = UI_KEY_RIGHT;
          TraceKey (Key);
          DrainCrLf ();
          return Key;
        case 'D':
          Key = UI_KEY_LEFT;
          TraceKey (Key);
          DrainCrLf ();
          return Key;
        case 'H':
          Key = UI_KEY_HOME;
          DrainCrLf ();
          return Key;
        case 'F':
          Key = UI_KEY_END;
          DrainCrLf ();
          return Key;
        default:
          // Parse CSI forms like [5~, [20~, [1;2A, etc.
          Param[0] = Ch3;
          ParamLen = 1;
          Final = 0;

          for (Idx = 0; Idx < 8; Idx++) {
            if ((Param[ParamLen - 1] >= 'A' && Param[ParamLen - 1] <= 'Z') ||
                Param[ParamLen - 1] == '~') {
              Final = Param[ParamLen - 1];
              break;
            }

            Ch = ReadChar ();
            TraceRaw (Ch);
            if (ParamLen < sizeof (Param)) {
              Param[ParamLen++] = Ch;
            }
          }

          if (Final == 'A') {
            Key = UI_KEY_UP;
            TraceKey (Key);
            DrainCrLf ();
            return Key;
          }

          if (Final == 'B') {
            Key = UI_KEY_DOWN;
            TraceKey (Key);
            DrainCrLf ();
            return Key;
          }

          if (Final == 'C') {
            Key = UI_KEY_RIGHT;
            TraceKey (Key);
            DrainCrLf ();
            return Key;
          }

          if (Final == 'D') {
            Key = UI_KEY_LEFT;
            TraceKey (Key);
            DrainCrLf ();
            return Key;
          }

          if (Final == 'H') {
            Key = UI_KEY_HOME;
            DrainCrLf ();
            return Key;
          }

          if (Final == 'F') {
            Key = UI_KEY_END;
            DrainCrLf ();
            return Key;
          }

          if (Final == '~') {
            Number = 0;
            for (Idx = 0; Idx < ParamLen; Idx++) {
              if (Param[Idx] >= '0' && Param[Idx] <= '9') {
                Number = (UINT16)(Number * 10 + (Param[Idx] - '0'));
              } else {
                break;
              }
            }

            if (Number == 5) {
              Key = UI_KEY_PGUP;
              DrainCrLf ();
              return Key;
            }

            if (Number == 6) {
              Key = UI_KEY_PGDN;
              DrainCrLf ();
              return Key;
            }

            if (Number == 20) {
              Key = UI_KEY_F9;
              TraceKey (Key);
              DrainCrLf ();
              return Key;
            }

            if (Number == 21) {
              Key = UI_KEY_F10;
              TraceKey (Key);
              DrainCrLf ();
              return Key;
            }
          }

          TraceKey (UI_KEY_NONE);
          DrainCrLf ();
          return UI_KEY_NONE;
      }
    } else if (Ch2 == 'O') {
      Ch3 = ReadChar ();
      TraceRaw (Ch3);
      switch (Ch3) {
        case 'A':
          Key = UI_KEY_UP;
          TraceKey (Key);
          DrainCrLf ();
          return Key;
        case 'B':
          Key = UI_KEY_DOWN;
          TraceKey (Key);
          DrainCrLf ();
          return Key;
        case 'C':
          Key = UI_KEY_RIGHT;
          TraceKey (Key);
          DrainCrLf ();
          return Key;
        case 'D':
          Key = UI_KEY_LEFT;
          TraceKey (Key);
          DrainCrLf ();
          return Key;
        case 'P':
          TraceKey (UI_KEY_F1);
          DrainCrLf ();
          return UI_KEY_F1;
        case 'Q':
          TraceKey (UI_KEY_F2);
          DrainCrLf ();
          return UI_KEY_F2;
        default:
          TraceKey (UI_KEY_NONE);
          DrainCrLf ();
          return UI_KEY_NONE;
      }
    } else {
      TraceKey (UI_KEY_ESC);
      DrainCrLf ();
      return UI_KEY_ESC;
    }
  }

  Key = UI_KEY_NONE;
  switch (Ch) {
    case 0x0D:
    case 0x0A:
      Key = UI_KEY_ENTER;
      break;
    case 0x09:
      Key = UI_KEY_TAB;
      break;
    case 0x08:
    case 0x7F:
      Key = UI_KEY_BKSP;
      break;
    case 'i':
      Key = UI_KEY_UP;
      break;
    case 'k':
      Key = UI_KEY_DOWN;
      break;
    case 'j':
      Key = UI_KEY_LEFT;
      break;
    case 'l':
      Key = UI_KEY_RIGHT;
      break;
    case 'q':
      Key = UI_KEY_F9;  // Restore defaults (alternative to F9 key)
      break;
    case 's':
      Key = UI_KEY_F10; // Save (alternative to F10 key)
      break;
    default:
      Key = (Ch >= 0x20 && Ch < 0x7F) ? (UINT16)Ch : UI_KEY_NONE;
      break;
  }

  if (Key != UI_KEY_ENTER && Key != UI_KEY_NONE) {
    DrainCrLf ();
  }

  if ((Key == UI_KEY_UP)   || (Key == UI_KEY_DOWN) ||
      (Key == UI_KEY_LEFT) || (Key == UI_KEY_RIGHT) ||
      (Key == UI_KEY_F9)   || (Key == UI_KEY_F10) ||
      (Key == UI_KEY_ESC)) {
    TraceKey (Key);
  }

  return Key;
}

/**
  Non-blocking poll for key availability.

  @return TRUE if a key is available.
**/
BOOLEAN
InputKeyReady (
  VOID
  )
{
  if (mPendingByteValid) {
    return TRUE;
  }

  if (((PcdGet32 (PcdConsoleInDeviceMask) & ConsoleInSerialPort) != 0) &&
      SerialPortPoll ()) {
    return TRUE;
  }

  return ConsolePoll ();
}
