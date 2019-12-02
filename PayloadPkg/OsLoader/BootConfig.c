/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OsLoader.h"

/**
  Get next line start pointer.

  @param[in]   Start    The pointer for current line.
  @param[out]  Length   The UINT32 pointer to receive the length of current line.

  @retval      The pointer for next line.
               NULL if no more line is available.
**/
STATIC
CHAR8 *
GetNextLine (
  IN   CHAR8    *Start,
  OUT  UINT32   *Length
  )
{
  CHAR8 *Ptr;

  Ptr = Start;
  while ((Ptr[0] != 0) && (Ptr[0] != '\n')) {
    Ptr++;
  }

  *Length = Ptr - Start;
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
STATIC
CHAR8 *
TrimLeft (
  IN  CHAR8  *Line
  )
{
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
STATIC
CHAR8 *
GetNextSpace (
  IN  CHAR8  *LineStart,
  IN  CHAR8  *LineEnd
  )
{
  CHAR8  *Line;

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
STATIC
CHAR8 *
TrimRight (
  IN  CHAR8  *Line
  )
{
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
STATIC
UINT32
MatchKeyWord (
  IN  CHAR8  *Line,
  IN  CHAR8  *Keyword
  )
{
  UINT32 KeywordLen;

  KeywordLen = AsciiStrLen (Keyword);
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
STATIC
UINT32
MatchAssignment (
  IN  CHAR8  *Line,
  IN  CHAR8  *Variable
  )
{
  UINT32 VarLen;

  VarLen = AsciiStrLen (Variable);
  if (AsciiStrnCmp (Line, Variable, VarLen) == 0) {
    if (Line[VarLen] == '=') {
      return VarLen;
    }
  }
  return 0;
}


/**
  Parse the Linux boot configuration file.

  @param[in]    CfgBuffer     The configuration buffer.
  @param[out]   LinuxBootCfg  The pointer to hold the parse results.

**/
VOID
ParseLinuxBootConfig (
  IN   CHAR8            *CfgBuffer,
  OUT  LINUX_BOOT_CFG   *LinuxBootCfg
  )
{
  CHAR8      *CurrLine;
  CHAR8      *NextLine;
  CHAR8      *StartLine;
  CHAR8      *EndLine;
  MENU_ENTRY *MenuEntry;
  UINT32      Idx;
  UINT32      LineLen;
  INT32       EntryNum;

  MenuEntry = LinuxBootCfg->MenuEntry;

  EntryNum = -1;
  CurrLine = CfgBuffer;
  while (CurrLine != NULL) {
    NextLine = GetNextLine (CurrLine, &LineLen);
    EndLine  = CurrLine + LineLen;
    CurrLine = TrimLeft (CurrLine);
    if (MatchKeyWord (CurrLine, "set") > 0) {
      CurrLine += 3;

      // Handle "set" varaible statement
      // Only support "timeout" and "default" now
      CurrLine  = TrimLeft (CurrLine);
      StartLine = CurrLine;
      while ((CurrLine[0] != 0) && (CurrLine[0] != '=') && (CurrLine < EndLine)) {
        CurrLine++;
      }
      if (CurrLine[0] == '=') {
        CurrLine++;
        if (MatchAssignment (StartLine, "timeout") > 0) {
          LinuxBootCfg->Settings.Timeout = AsciiStrDecimalToUintn (CurrLine);
        } else if (MatchAssignment (StartLine, "default") > 0) {
          if (CurrLine[0] == '"') {
            CurrLine++;
          }
          // Support boot option number string only
          LinuxBootCfg->Settings.Default = AsciiStrDecimalToUintn (CurrLine);
        }
      }
    } else if (MatchKeyWord (CurrLine, "menuentry") > 0) {
      if (EntryNum >= MAX_BOOT_MENU_ENTRY) {
        NextLine = NULL;
        break;
      }

      // Mark boot option name
      EntryNum++;
      CurrLine += 9;
      CurrLine  = TrimLeft (CurrLine);
      for (Idx = 0; Idx < 2; Idx++) {
        while ((CurrLine[0] != 0) && (CurrLine[0] != '"') && (CurrLine[0] != '\'') && (CurrLine < EndLine)) {
          CurrLine++;
        }
        if ((CurrLine[0] == '"') || (CurrLine[0] == '\'')) {
          if (Idx == 0) {
            MenuEntry[EntryNum].Name.Pos = CurrLine - CfgBuffer + 1;
          } else {
            MenuEntry[EntryNum].Name.Len = CurrLine - CfgBuffer - MenuEntry[EntryNum].Name.Pos;
          }
          CurrLine++;
        }
      }
    } else if (MatchKeyWord (CurrLine, "linux") > 0) {
      CurrLine += 5;

      // Mark kernel path
      CurrLine  = TrimLeft (CurrLine);
      MenuEntry[EntryNum].Kernel.Pos = CurrLine - CfgBuffer;
      CurrLine  = GetNextSpace (CurrLine, EndLine);
      MenuEntry[EntryNum].Kernel.Len = CurrLine - CfgBuffer - MenuEntry[EntryNum].Kernel.Pos;

      // Mark command line
      CurrLine = TrimLeft (CurrLine);
      MenuEntry[EntryNum].Command.Pos = CurrLine - CfgBuffer;
      EndLine  = TrimRight (EndLine);
      MenuEntry[EntryNum].Command.Len = EndLine - CfgBuffer - MenuEntry[EntryNum].Command.Pos + 1;
    } else if (MatchKeyWord (CurrLine, "initrd") > 0) {
      CurrLine += 6;

      // Mark initrd path
      CurrLine = TrimLeft (CurrLine);
      MenuEntry[EntryNum].InitRd.Pos = CurrLine - CfgBuffer;
      EndLine  = TrimRight (EndLine);
      MenuEntry[EntryNum].InitRd.Len = EndLine - CfgBuffer - MenuEntry[EntryNum].InitRd.Pos + 1;
    }

    CurrLine = NextLine;
  }

  if (EntryNum >= 0) {
    EntryNum++;
  } else {
    EntryNum = 0;
  }

  // Make sure the settings are reasonable
  LinuxBootCfg->EntryNum = EntryNum;
  if (LinuxBootCfg->Settings.Default >= (UINT32)EntryNum) {
    LinuxBootCfg->Settings.Default = 0;
  }
  if (LinuxBootCfg->Settings.Timeout >= 60) {
    LinuxBootCfg->Settings.Timeout = 3;
  }
}


/**
  Print Linux boot options.

  @param[in]    CfgBuffer     The configuration buffer.
  @param[out]   LinuxBootCfg  The pointer to hold the parse results.

**/
VOID
PrintLinuxBootConfig (
  CHAR8                    *CfgBuffer,
  LINUX_BOOT_CFG           *LinuxBootCfg
  )
{
  UINTN  Idx;
  CHAR8 *CurrLine;
  UINT32 LineLen;
  CHAR8  Val;

  ShellPrint (L"\nBoot Menu:\n");
  for (Idx = 0; Idx < LinuxBootCfg->EntryNum; Idx++) {
    if (LinuxBootCfg->MenuEntry[Idx].Name.Buf[0] == 0) {
      CurrLine = CfgBuffer + LinuxBootCfg->MenuEntry[Idx].Name.Pos;
    } else {
      CurrLine = LinuxBootCfg->MenuEntry[Idx].Name.Buf;
    }
    LineLen  = LinuxBootCfg->MenuEntry[Idx].Name.Len;
    Val = CfgBuffer[LineLen];
    CurrLine[LineLen] = 0;
    ShellPrint (L"  %d - %c %a\n", Idx, (Idx == LinuxBootCfg->Settings.Default) ? '*' : ' ', CurrLine);
    CurrLine[LineLen] = Val;
  }
}


/**
  Get Linux boot option from user input.

  @param[in]    CfgBuffer     The configuration buffer.
  @param[out]   LinuxBootCfg  The pointer to hold the parse results.

  @retval       0 based boot option index.

**/
UINT32
GetLinuxBootOption (
  CHAR8                    *CfgBuffer,
  LINUX_BOOT_CFG           *LinuxBootCfg
  )
{
  UINT32     EntryIdx;
  UINT32     Timeout;
  UINT32     Index;
  UINT8      Key;

  EntryIdx = LinuxBootCfg->Settings.Default;
  Timeout  = LinuxBootCfg->Settings.Timeout;
  if ((Timeout > 0) && (LinuxBootCfg->EntryNum > 0)) {
    ShellPrint (L"Press '0' ~ '%d' to select boot option: ", LinuxBootCfg->EntryNum - 1);
    for (Index = 0; Index < Timeout * 100; Index++) {
      if (ConsolePoll ()) {
        if (ConsoleRead (&Key, 1) > 0) {
          if ((Key >= '0') && (Key < '0' + LinuxBootCfg->EntryNum)) {
            ShellPrint (L"%c", Key);
            EntryIdx = Key - '0';
            break;
          }
        }
      }
      MicroSecondDelay (10 * 1000);
    }
    ShellPrint (L"\nBoot from option %d\n", EntryIdx);
  }

  return EntryIdx;
}

