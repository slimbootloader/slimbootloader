/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OsLoader.h"


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
  UINT32      EntryNum;
  BOOLEAN     FoundEntry;
  BOOLEAN     IsKernel;
  BOOLEAN     IsInitRd;

  MenuEntry = LinuxBootCfg->MenuEntry;

  FoundEntry = FALSE;
  EntryNum = 0;
  CurrLine = CfgBuffer;

  while ((CurrLine != NULL) && (EntryNum < MAX_BOOT_MENU_ENTRY)) {
    IsKernel = FALSE;
    IsInitRd = FALSE;
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
          LinuxBootCfg->Settings.Timeout = (UINT32)AsciiStrDecimalToUintn (CurrLine);
        } else if (MatchAssignment (StartLine, "default") > 0) {
          if (CurrLine[0] == '"') {
            CurrLine++;
          }
          // Support boot option number string only
          LinuxBootCfg->Settings.Default = (UINT32)AsciiStrDecimalToUintn (CurrLine);
        }
      }
    } else if (MatchKeyWord (CurrLine, "menuentry") > 0) {
      // Mark boot option name
      if (!FoundEntry) {
        FoundEntry = TRUE;
      } else {
        EntryNum++;
      }
      CurrLine += 9;
      CurrLine  = TrimLeft (CurrLine);
      for (Idx = 0; Idx < 2; Idx++) {
        while ((CurrLine[0] != 0) && (CurrLine[0] != '"') && (CurrLine[0] != '\'') && (CurrLine < EndLine)) {
          CurrLine++;
        }
        if ((CurrLine[0] == '"') || (CurrLine[0] == '\'')) {
          if (Idx == 0) {
            MenuEntry[EntryNum].Name.Pos = (UINT32)(CurrLine - CfgBuffer + 1);
          } else {
            MenuEntry[EntryNum].Name.Len = (UINT32)(CurrLine - CfgBuffer - MenuEntry[EntryNum].Name.Pos);
          }
          CurrLine++;
        }
      }
    } else if (MatchKeyWord (CurrLine, "linux") > 0) {
      CurrLine += 5;
      IsKernel = TRUE;
    } else if (MatchKeyWord (CurrLine, "linuxefi") > 0) {
      CurrLine += 8;
      IsKernel = TRUE;
    } else if (MatchKeyWord (CurrLine, "initrd") > 0) {
      CurrLine += 6;
      IsInitRd = TRUE;
    } else if (MatchKeyWord (CurrLine, "initrdefi") > 0) {
      CurrLine += 9;
      IsInitRd = TRUE;
    }

    if (IsKernel) {
      // Mark kernel path
      CurrLine  = TrimLeft (CurrLine);
      MenuEntry[EntryNum].Kernel.Pos = (UINT32)(CurrLine - CfgBuffer);
      CurrLine  = GetNextSpace (CurrLine, EndLine);
      MenuEntry[EntryNum].Kernel.Len = (UINT32)(CurrLine - CfgBuffer - MenuEntry[EntryNum].Kernel.Pos);

      // Mark command line
      CurrLine = TrimLeft (CurrLine);
      MenuEntry[EntryNum].Command.Pos = (UINT32)(CurrLine - CfgBuffer);
      EndLine  = TrimRight (EndLine);
      MenuEntry[EntryNum].Command.Len = (UINT32)(EndLine - CfgBuffer - MenuEntry[EntryNum].Command.Pos + 1);
    }

    if (IsInitRd) {
      // Mark initrd path
      CurrLine = TrimLeft (CurrLine);
      MenuEntry[EntryNum].InitRd.Pos = (UINT32)(CurrLine - CfgBuffer);
      EndLine  = TrimRight (EndLine);
      MenuEntry[EntryNum].InitRd.Len = (UINT32)(EndLine - CfgBuffer - MenuEntry[EntryNum].InitRd.Pos + 1);
    }

    CurrLine = NextLine;
  }

  // Make sure the settings are reasonable
  if (FoundEntry) {
    EntryNum += 1;
  }

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

