/** @file
  A minimal command-line shell.

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/ConsoleInLib.h>
#include <Library/ConsoleOutLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/MemoryAllocationLib.h>
#include "Shell.h"
#include "History.h"
#include "Parsing.h"
#include "ShellCmds.h"

#define ESC   '\x1b'

STATIC LIST_ENTRY mShellCommandEntryList =
  INITIALIZE_LIST_HEAD_VARIABLE (mShellCommandEntryList);

/**
  Prompt user for command, receive command, run command.

  @param[in]  Shell        shell instance

  @retval EFI_SUCCESS
  @retval RETURN_ABORTED

**/
STATIC
EFI_STATUS
ShellPrompt (
  IN SHELL *Shell
  );

/**
  Find shell command by name.

  @param[in]  Shell        shell instance
  @param[in]  Name         name of command
  @param[in]  Ptr          pointer to string pointer

  @retval EFI_SUCCESS
  @retval EFI_NOT_FOUND

**/
STATIC
EFI_STATUS
FindShellCommand (
  IN       SHELL         *Shell,
  IN CONST CHAR16        *Name,
  IN CONST SHELL_COMMAND **Ptr
  );

/**
  Process and run a command line.

  @param[in] Shell        shell instance
  @param[in] CmdLine      command line to parse

  @retval EFI_SUCCESS     command was completed

**/
STATIC
EFI_STATUS
RunShellCommand (
  IN       SHELL  *Shell,
  IN CONST CHAR16 *CmdLine
  );

/**
  Wait for command line input from the serial port.

  @param[in]  Shell        shell instance
  @param[out] Buffer       buffer to receive command line
  @param[in]  BufferSize   size (in bytes) of the buffer

  @retval EFI_SUCCESS
  @retval EFI_BUFFER_TOO_SMALL
  @retval EFI_TIMEOUT

**/
STATIC
EFI_STATUS
ReadShellCommand (
  IN        SHELL  *Shell,
  OUT       CHAR16 *Buffer,
  IN  CONST UINTN   BufferSize
  );

/**
  Begin a run-time interactive shell.

  @param[in]  Commands      command list (may be NULL for default commands)
  @param[in]  Timeout       seconds to wait for input before returning (0 for no timeout)

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
Shell (
  IN CONST SHELL_COMMAND **Commands,
  IN       UINTN           Timeout
  )
{
  BOOLEAN Start;
  UINTN   Index, Index1;
  SHELL   Shell;
  CONST SHELL_COMMAND **Iter;
  UINT8   Buffer;

  InitializeListHead (&mShellCommandEntryList);

  LoadShellCommands ();
  for (Iter = Commands; *Iter != NULL; Iter++) {
    ShellCommandRegister (*Iter);
  }
  Shell.ShouldExit = FALSE;

  if (Timeout != 0) {
    ShellPrint (L"\n");
    while (ConsolePoll ()) {
      ConsoleRead (&Buffer, 1);
    }
    for (Index = Timeout; Index > 0; Index--) {
      ShellPrint (L"Press any key within %d second(s) to enter the command shell", Index);
      for (Index1 = 0; Index1 < 10; Index1++) {
        Start = ConsolePoll ();
        if (Start) {
          break;
        }
        MicroSecondDelay (100 * 1000);
      }
      ShellPrint(L"\r");
      if (Start) {
        break;
      }
    }
    ShellPrint (L"\n");
    if (!Start) {
      return EFI_ABORTED;
    }
  }

  HistoryInit (TRUE);

  while (! (Shell.ShouldExit)) {
    ShellPrompt (&Shell);
  }

  HistoryInit (FALSE);

  return EFI_SUCCESS;
}

/**
  Prompt user for command, receive command, run command.

  @param[in]  Shell        shell instance

  @retval EFI_SUCCESS
  @retval RETURN_ABORTED

**/
STATIC
EFI_STATUS
ShellPrompt (
  IN SHELL *Shell
  )
{
  CHAR16     CommandLine[MAX_COMMAND_LINE_LEN];
  EFI_STATUS Status;

  ASSERT (Shell != NULL);
  ShellPrint (L"\nShell> ");

  Status = ReadShellCommand (Shell, CommandLine, sizeof (CommandLine));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HistoryAdd (CommandLine);

  return RunShellCommand (Shell, CommandLine);
}

/**
  Wait for command line input from the serial port.

  @param[in]  Shell        shell instance
  @param[out] Buffer       buffer to receive command line
  @param[in]  BufferSize   size (in bytes) of the buffer

  @retval EFI_SUCCESS
  @retval EFI_BUFFER_TOO_SMALL
  @retval EFI_TIMEOUT

**/
STATIC
EFI_STATUS
ReadShellCommand (
  IN        SHELL  *Shell,
  OUT       CHAR16 *Buffer,
  IN  CONST UINTN   BufferSize
  )
{
  UINTN   Count;
  UINTN   CurrCount;
  UINT8   Char;
  UINTN   HistoryLineLen;
  CHAR16  HistoryLine[MAX_COMMAND_LINE_LEN];

  Count = 0;

  ASSERT (Buffer != NULL);
  ASSERT (BufferSize > sizeof (CHAR16));

  while (1) {
    // Check for buffer overflow
    if (Count >= (BufferSize / sizeof (CHAR16) - 1)) {
      Buffer[0] = CHAR_NULL; // Terminate String
      return EFI_BUFFER_TOO_SMALL;
    }

    // Get next character from serial port
    Char = 0;
    if (ConsoleRead (&Char, 1) < 1) {
      return EFI_TIMEOUT;
    }

    // Check for end of line
    if ((Char == '\r') || (Char == '\n')) {
      Buffer[Count] = CHAR_NULL; // Terminate String
      break;
    }

    // Check for backspace/delete
    if ((Char == 0x7f) || (Char == '\b')) {
      if (Count > 0) {
        // Move cursor back 1 char and clear
        ShellPrint (L"\b \b");
        Count--;
      }
      continue;
    }

    // Handle key codes of the form: ESC [ x
    if (Char == ESC) {
      // Expect '[' character
      if (ConsoleRead (&Char, 1) < 1) {
        return EFI_TIMEOUT;
      }
      if (Char == '[') {
        // Expect command
        if (ConsoleRead (&Char, 1) < 1) {
          return EFI_TIMEOUT;
        }

        HistoryLineLen = 0;
        if (Char == 'A') { // Up
          HistoryLineLen = HistoryUp (HistoryLine);
        } else if (Char == 'B') { // Down
          HistoryLineLen = HistoryDown (HistoryLine);
        } else if (Char == 'C') { // Right
        } else if (Char == 'D') { // Left
        }


        CurrCount = Count;
        while (Count-- > 0) {
          ShellPrint (L"\b");
        }
        if (HistoryLineLen > 0) {
          StrCpyS (Buffer, MAX_COMMAND_LINE_LEN, HistoryLine);
          ShellPrint (L"%s", Buffer);
        }
        if (CurrCount > HistoryLineLen) {
          CurrCount = CurrCount - HistoryLineLen;

          Count = CurrCount;
          while (Count-- > 0) {
            ShellPrint (L" ");
          }

          Count = CurrCount;
          while (Count-- > 0) {
            ShellPrint (L"\b");
          }
        }
        Count = HistoryLineLen;

        // Key code consumed, continue getting input
        continue;
      }
    }

    // Echo character back and save character in buffer
    ShellPrint (L"%c", Char);
    Buffer[Count++] = Char;
  }

  ShellPrint (L"\n");
  TrimSpaces (&Buffer);

  return EFI_SUCCESS;
}


/**
  Read a line of input from the serial port.

  @param[in]  Shell        shell instance
  @param[out] Buffer       buffer to receive command line
  @param[in]  BufferSize   size (in bytes) of the buffer

  @retval EFI_SUCCESS
  @retval EFI_BUFFER_TOO_SMALL
  @retval EFI_TIMEOUT

**/
EFI_STATUS
ShellReadLine (
  IN        SHELL  *Shell,
  OUT       CHAR16 *Buffer,
  IN  CONST UINTN   BufferSize
  )
{
  UINTN Count;
  UINT8 Char;

  Count = 0;

  ASSERT (Buffer != NULL);
  ASSERT (BufferSize > sizeof (CHAR16));

  while (1) {
    // Check for buffer overflow
    if (Count >= (BufferSize / sizeof (CHAR16) - 1)) {
      Buffer[0] = CHAR_NULL; // Terminate String
      return EFI_BUFFER_TOO_SMALL;
    }

    // Get next character from serial port
    Char = 0;
    if (ConsoleRead (&Char, 1) < 1) {
      return EFI_TIMEOUT;
    }

    // Check for end of line
    if ((Char == '\r') || (Char == '\n')) {
      Buffer[Count] = CHAR_NULL; // Terminate String
      break;
    }

    // Check for backspace/delete
    if ((Char == 0x7f) || (Char == '\b')) {
      if (Count > 0) {
        // Move cursor back 1 char and clear
        ShellPrint (L"\b \b");
        Count--;
      }
      continue;
    }

    // Handle key codes of the form: ESC [ x
    if (Char == ESC) {
      // Expect '[' character
      if (ConsoleRead (&Char, 1) < 1) {
        return EFI_TIMEOUT;
      }
      if (Char == '[') {
        // Expect command
        if (ConsoleRead (&Char, 1) < 1) {
          return EFI_TIMEOUT;
        }

        if (Char == 'A') { // Up
        } else if (Char == 'B') { // Down
        } else if (Char == 'C') { // Right
        } else if (Char == 'D') { // Left
        }

        // Key code consumed, continue getting input
        continue;
      }
    }

    // Echo character back and save character in buffer
    ShellPrint (L"%c", Char);
    Buffer[Count++] = Char;
  }

  ShellPrint (L"\n");
  TrimSpaces (&Buffer);

  return EFI_SUCCESS;
}

/**
  Read a UINT value from the serial port.

  @param[in]  Shell        shell instance
  @param[out] Buffer       buffer to receive command line
  @param[in]  BufferSize   size (in bytes) of the buffer
  @param[out] IsHex        determine if the UINT is hex or decimal format

  @retval EFI_SUCCESS
  @retval EFI_BUFFER_TOO_SMALL
  @retval EFI_TIMEOUT

**/
EFI_STATUS
ShellReadUintn (
  IN        SHELL   *Shell,
  OUT       CHAR16  *Buffer,
  IN  CONST UINTN    BufferSize,
  OUT       BOOLEAN *IsHex
  )
{
  UINTN Count;
  UINT8 Char;

  Count = 0;

  ASSERT (Buffer != NULL);
  ASSERT (BufferSize > sizeof (CHAR16));

  *IsHex = FALSE;
  ZeroMem (Buffer, BufferSize);

  while (1) {
    // Check for buffer overflow
    if (Count >= (BufferSize / sizeof (CHAR16) - 1)) {
      return EFI_BUFFER_TOO_SMALL;
    }

    // Get next character from serial port
    Char = 0;
    if (ConsoleRead (&Char, 1) < 1) {
      return EFI_TIMEOUT;
    }

    if ((Char == '\r') || (Char == '\n')) {
      // Check for end of line
      Buffer[Count] = CHAR_NULL; // Terminate String
      break;
    } else if ((Char == 0x7f) || (Char == '\b')) {
      // Check for backspace/delete
      if (Count > 0) {
        if (Buffer[Count - 1] == 'x' || Buffer[Count - 1] == 'X') {
          *IsHex = FALSE;
        }
        // Move cursor back 1 char and clear
        ShellPrint (L"\b \b");
        Count--;
      }
      continue;
    } else if (Char == 'x' || Char == 'X') {
      if (Count != 1) {
        continue;
      }
      *IsHex = TRUE;
    } else if (Char >= '0' && Char <= '9') {
      ;
    } else if (*IsHex && ((Char >= 'a' && Char <= 'f') || (Char >= 'A' && Char <= 'F'))) {
      ;
    } else {
      continue;
    }

    // Echo character back and save character in buffer
    ShellPrint (L"%c", Char);
    Buffer[Count++] = Char;
  }

  ShellPrint (L"\n");
  TrimSpaces (&Buffer);

  return EFI_SUCCESS;
}

/**
  Process and run a command line.

  @param[in] Shell        shell instance
  @param[in] CmdLine      command line to parse

  @retval EFI_SUCCESS     command was completed

**/
STATIC
EFI_STATUS
RunShellCommand (
  IN       SHELL  *Shell,
  IN CONST CHAR16 *CmdLine
  )
{
  CHAR16              Buf[MAX_COMMAND_LINE_LEN];
  CHAR16              *Ptr;
  CONST CHAR16        *Walker;
  UINTN               Index, Count, BufRemaining;
  EFI_STATUS          Status;
  CONST SHELL_COMMAND *Cmd;
  CHAR16              *Argv[8]; // FIXME: Use allocator once that's available

  // Parse the command line parameters
  BufRemaining = sizeof (Buf);
  Ptr          = Buf;
  Walker       = CmdLine;

  for (Count = 0; Walker != NULL && *Walker != CHAR_NULL; Count++) {
    Status = GetNextParameter (&Walker, &Ptr, BufRemaining, TRUE);
    if (EFI_ERROR (Status)) {
      break;
    }
    BufRemaining -= StrSize (Ptr);
    Ptr          += StrLen (Ptr) + 1;
  }

  if (Count == 0) {
    return EFI_SUCCESS;
  }

  ASSERT (Count <= (sizeof (Argv) / sizeof (Argv[0])));
  for (Index = 0, Ptr = Buf; Index < Count; Index++) {
    Argv[Index]  = Ptr;
    Ptr     += StrLen (Ptr) + 1;
  }

  // Locate and execute command
  Cmd = NULL;
  Status = FindShellCommand (Shell, Argv[0], &Cmd);
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      ShellPrint (L"Command not found (try help)\n");
    }
  } else {
    ASSERT (Cmd);
    Status = Cmd->Entry (Shell, Count, Argv);
  }

  return Status;
}

/**
  Find shell command by name.

  @param[in]  Shell        shell instance
  @param[in]  Name         name of command
  @param[in]  Ptr          pointer to string pointer

  @retval EFI_SUCCESS
  @retval EFI_NOT_FOUND

**/
STATIC
EFI_STATUS
FindShellCommand (
  IN       SHELL         *Shell,
  IN CONST CHAR16        *Name,
  IN CONST SHELL_COMMAND **Ptr
  )
{
  LIST_ENTRY                *Link;
  SHELL_COMMAND_LIST_ENTRY  *Entry;

  //
  // Add '?' alias for help
  //
  if (Name[0] == '?') {
    Name = L"help";
  }

  for (Link = mShellCommandEntryList.ForwardLink; Link != &mShellCommandEntryList; Link = Link->ForwardLink) {
    Entry = CR (Link, SHELL_COMMAND_LIST_ENTRY, Link, SHELL_COMMAND_LIST_ENTRY_SIGNATURE);
    if (StrCmp (Name, Entry->ShellCommand->Name) == 0) {
      *Ptr = Entry->ShellCommand;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Register a Shell Command

  @param[in]  ShellCommand A Shell Command to be registered

  @retval EFI_SUCCESS
  @retval EFI_OUT_OF_RESOURCES
**/
EFI_STATUS
EFIAPI
ShellCommandRegister (
  IN  CONST SHELL_COMMAND   *ShellCommand
  )
{
  SHELL_COMMAND_LIST_ENTRY  *Entry;

  Entry = (SHELL_COMMAND_LIST_ENTRY *)AllocateZeroPool (sizeof (SHELL_COMMAND_LIST_ENTRY));
  ASSERT (Entry != NULL);
  if (Entry == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  InitializeListHead (&Entry->Link);
  Entry->ShellCommand = ShellCommand;
  Entry->Signature = SHELL_COMMAND_LIST_ENTRY_SIGNATURE;

  InsertTailList (&mShellCommandEntryList, &Entry->Link);

  return EFI_SUCCESS;
}

/**
  Return a Shell Command Entry List Pointer

  @retval LIST_ENTRY Pointer
**/
LIST_ENTRY *
EFIAPI
GetShellCommandEntryList (
  VOID
  )
{
  return &mShellCommandEntryList;
}
