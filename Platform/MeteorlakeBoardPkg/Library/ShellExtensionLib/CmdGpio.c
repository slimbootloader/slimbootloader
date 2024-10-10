/** @file
  Shell command `gpio` to read and write the required GPIO Pin

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/GpioV2Lib.h>
#include <Library/GpioV2SiLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Register/GpioV2ChipsetId.h>
#include <Library/PrintLib.h>

CONST CHAR8 *Title = "PadName   PadMode  SwOwn  Direction  Output  InterruptConfig  ResetConfig  TermConfig  CfgLock  TxLock";
#define     MaxLen  120
CHAR8       Buffer[MaxLen];

/**
  GPIO Read and write commands

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandGpioFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );


CONST SHELL_COMMAND mShellCommandGpio = {
  L"gpio",
  L"Read or write GPIO pin",
  &ShellCommandGpioFunc
};


/**
  Write GPIO PAD output state to low or high.

  @param[in]  Id             GPIO controller ID
  @param[in]  CommunityIdx   GPIO community index
  @param[in]  GroupIdx       GPIO group index
  @param[in]  PadIdx         GPIO pad index
  @param[in]  Value          The value write to GPIO output state

  @retval EFI_SUCCESS        The function completed successfully
  @retval EFI_NOT_FOUND      Invalid parameter (chipsetId, community, group or pad number)
**/
EFI_STATUS
GpioWrite (
  IN UINT32                  Id,
  IN UINT32                  CommunityIdx,
  IN UINT32                  GroupIdx,
  IN UINT32                  PadIdx,
  IN UINT32                  Value
  )
{
  GPIOV2_CONTROLLER          *Controller;
  UINT32                     ChipsetId;
  UINT32                     CommunityIndex;
  UINT32                     GroupIndex;
  UINT32                     PadIndex;
  GPIOV2_COMMUNITY           *Community;
  GPIOV2_GROUP               *Group;
  GPIOV2_PAD                 GpioPad;
  UINT32                     BitIndex;

  ChipsetId  = GpioGetThisChipsetId ();
  ShellPrint (L"Supported ChipsetId: 0x%x\n", ChipsetId);
  if ((ChipsetId & Id) == 0) {
    ShellPrint (L"The chipsetId 0x%x is not supported\n", Id);
    return EFI_NOT_FOUND;
  }
  ChipsetId &= Id;

  for (BitIndex = 0; BitIndex < 8; BitIndex++) {
    if ( (( 1 << BitIndex) & ChipsetId) == 0) {
      continue;
    }

    GpioPad    = GPIOV2_PAD_ID(0, 1 << BitIndex, 0, 0, 0, 0);
    Controller = GpioGetController (GpioPad);
    for (CommunityIndex = 0; CommunityIndex < Controller->CommunityNum; CommunityIndex++) {
      if (CommunityIdx != CommunityIndex) {
        continue;
      }
      Community = &Controller->Communities[CommunityIndex];
      ShellPrint (L"\nCommunity=%d, Total Groups=%d, PID=0x%x\n", CommunityIndex, Community->GroupsNum, Community->Pid);
      for (GroupIndex = 0; GroupIndex < Community->GroupsNum; GroupIndex++) {
        if (GroupIdx != GroupIndex) {
          continue;
        }
        Group = &Community->Groups[GroupIndex];
        ShellPrint (L"\n%a Group=%d tatal Pads=%d\n", Group->Name, GroupIndex, Group->PadsNum);
        for (PadIndex = 0; PadIndex < Group->PadsNum; PadIndex++) {
          if (PadIdx != PadIndex) {
            continue;
          }
          GpioPad = Group->Pads[PadIndex];
          SetTx (GpioPad, (GPIOV2_PAD_STATE) Value);
          return EFI_SUCCESS;
        }
      }
    }
  }

  return EFI_NOT_FOUND;
}


/**
  Get the GPIO configuration string from the GPIO pad.

  @param[in]  GpioPad         GPIO pad

  @retval                     The generated GPIO configration string.
**/
CHAR8 *
GetGpioCfgStr (
  IN UINT32                   GpioPad
)
{
  EFI_STATUS                  Status;
  GPIOV2_PAD_MODE             PadMode;
  GPIOV2_HOSTSW_OWN           HostSwOwn;
  GPIOV2_PAD_STATE            OutputState;
  GPIOV2_DIRECTION            Direction;
  GPIOV2_INT_CONFIG           InterruptConfig;
  GPIOV2_RESET_CONFIG         ResetConfig;
  GPIOV2_TERMINATION_CONFIG   TermConfig;
  GPIOV2_PAD_LOCK             ConfigLock;
  GPIOV2_PAD_LOCK             TxLock;
  CHAR8                       PadModeStr[10];

  Status = GetPadMode (GpioPad, &PadMode);
  if (EFI_ERROR (Status)) {
    return " ";
  }

  GetPadName (GpioPad, MaxLen, Buffer);
  if (AsciiStrLen (Buffer) < 8) {
    AsciiStrCatS (Buffer, MaxLen, "  ");
  }
  if ((PadMode >> 1) == 0) {
    AsciiStrCatS (Buffer, MaxLen, "  Gpio   ");
  } else {
    AsciiSPrint (PadModeStr,10, "  %a%d", "Native", PadMode >> 1);
    AsciiStrCatS (Buffer, MaxLen, PadModeStr);
  }

  Status = GetHostSwOwnership (GpioPad, &HostSwOwn);
  switch (HostSwOwn) {
  case GpioV2HostOwnAcpi:
    AsciiStrCatS (Buffer, MaxLen, "  Acpi  ");
    break;
  case GpioV2HostOwnGpio:
    AsciiStrCatS (Buffer, MaxLen, "  Gpio  ");
    break;
  default:
    break;
  }

  // Get direction str
  Status = GetDirection (GpioPad, &Direction);
  switch (Direction) {
  case GpioV2DirInOut:
    AsciiStrCatS (Buffer, MaxLen,  " InOut    ");
    break;
  case GpioV2DirInInvOut:
    AsciiStrCatS (Buffer, MaxLen, " InInvOut ");
    break;
  case GpioV2DirIn:
    AsciiStrCatS (Buffer, MaxLen,  " In       ");
    break;
  case GpioV2DirInInv:
    AsciiStrCatS (Buffer, MaxLen,  " InInv    ");
    break;
  case GpioV2DirOut:
    AsciiStrCatS (Buffer, MaxLen,  " Out      ");
    break;
  case GpioV2DirNone:
    AsciiStrCatS (Buffer, MaxLen,  " None     ");
    break;
  default:
    break;
  }

  // Output State
  Status = GetTx (GpioPad, &OutputState);
  switch (OutputState) {
  case GpioV2StateLow:
    AsciiStrCatS (Buffer, MaxLen,  "  Low   ");
    break;
  case GpioV2StateHigh:
    AsciiStrCatS (Buffer, MaxLen,  "  High  ");
    break;
  default:
    break;
  }

  Status = GetInterrupt (GpioPad, &InterruptConfig);
  if ((InterruptConfig >> 1) == 0) {
    AsciiStrCatS (Buffer, MaxLen,  "  Disable       ");
  }
  if ((InterruptConfig & GpioV2IntNmi) >> 1 != 0) {
    AsciiStrCatS (Buffer, MaxLen,  "  Nmi ");
  }
  if ((InterruptConfig & GpioV2IntSmi) >> 1 != 0) {
     AsciiStrCatS (Buffer, MaxLen, "  Smi ");
  }
  if ((InterruptConfig & GpioV2IntSci) >> 1 != 0) {
    AsciiStrCatS (Buffer, MaxLen,  "  Sci ");
  }
  if ((InterruptConfig & GpioV2IntApic) >> 1 != 0) {
    AsciiStrCatS (Buffer, MaxLen,  "  Apic");
  }

  if ((InterruptConfig & GpioV2IntLevel) >> 1 != 0) {
    AsciiStrCatS (Buffer, MaxLen,  "|Level    ");
  }
  if ((InterruptConfig & GpioV2IntEdge) >> 1 != 0) {
    AsciiStrCatS (Buffer, MaxLen,  "|Edge     ");
  }
  if ((InterruptConfig & GpioV2IntLvlEdgDis) >> 1 != 0) {
    AsciiStrCatS (Buffer, MaxLen,  "  DisTrigger    ");
  }
  if ((InterruptConfig & GpioV2IntBothEdge) >> 1 != 0) {
    AsciiStrCatS (Buffer, MaxLen,  "|BothEdge ");
  }

  Status = GetResetConfig (GpioPad, &ResetConfig);
  switch (ResetConfig) {
  case GpioV2ResetResume:
    AsciiStrCatS (Buffer, MaxLen,  "   Resume     ");
    break;
  case GpioV2ResetHostDeep:
    AsciiStrCatS (Buffer, MaxLen,  "   HostDeep   ");
    break;
  case GpioV2ResetHost:
    AsciiStrCatS (Buffer, MaxLen,  "   Host       ");
    break;
  case GpioV2DswReset:
    AsciiStrCatS (Buffer, MaxLen,  "   DswReset   ");
    break;
  case GpioV2ResetGlobal:
    AsciiStrCatS (Buffer, MaxLen,  "   Global     ");
    break;
  default:
    break;
  }

  Status = GetTerminationConfig (GpioPad, &TermConfig);
  switch (TermConfig) {
  case GpioV2TermNone:
    AsciiStrCatS (Buffer, MaxLen,  "  None      ");
    break;
  case GpioV2TermWpd5K:
    AsciiStrCatS (Buffer, MaxLen,  "  Wpd5K     ");
    break;
  case GpioV2TermWpd20K:
    AsciiStrCatS (Buffer, MaxLen,  "  Wpd20K    ");
    break;
  case GpioV2TermWpu1K:
    AsciiStrCatS (Buffer, MaxLen,  "  Wpu1K     ");
    break;
  case GpioV2TermWpu2K:
    AsciiStrCatS (Buffer, MaxLen,  "  Wpu2K     ");
    break;
  case GpioV2TermWpu5K:
    AsciiStrCatS (Buffer, MaxLen,  "  Wpu5K     ");
    break;
  case GpioV2TermWpu20K:
    AsciiStrCatS (Buffer, MaxLen,  "  Wpu20K    ");
    break;
  case GpioV2TermWpu1K2K:
    AsciiStrCatS (Buffer, MaxLen,  "  Wpu1K2K   ");
    break;
  case GpioV2TermNative:
    AsciiStrCatS (Buffer, MaxLen,  "  Native    ");
    break;
  default:
    break;
  }

  Status = GetPadConfigLock (GpioPad, &ConfigLock);
  switch (ConfigLock) {
  case GpioV2Unlock:
    AsciiStrCatS (Buffer, MaxLen, "  Unlock ");
    break;
  case GpioV2Lock:
    AsciiStrCatS (Buffer, MaxLen,  "  Lock   ");
    break;
  default:
    break;
  }

  Status = GetTxLock (GpioPad, &TxLock);
  switch (TxLock) {
  case GpioV2Unlock:
    AsciiStrCatS (Buffer, MaxLen,  "  Unlock");
    break;
  case GpioV2Lock:
    AsciiStrCatS (Buffer, MaxLen,  "  Lock  ");
    break;
  default:
    break;
  }

  return Buffer;
}


/**
  Dump GPIO PAD in SBL configuration data format.

  @param[in]  Id             GPIO controller ID
  @param[in]  CommunityIdx   GPIO community index
  @param[in]  GroupIdx       GPIO group index
  @param[in]  PadIdx         GPIO pad index

**/
VOID
DumpPadsCfg (
  IN UINT32                  Id,
  IN UINT32                  CommunityIdx,
  IN UINT32                  GroupIdx,
  IN UINT32                  PadIdx
  )
{
  GPIOV2_CONTROLLER          *Controller;
  UINT32                     ChipsetId;
  UINT32                     CommunityIndex;
  UINT32                     GroupIndex;
  UINT32                     PadIndex;
  GPIOV2_COMMUNITY           *Community;
  GPIOV2_GROUP               *Group;
  GPIOV2_PAD                 GpioPad;
  UINT32                     BitIndex;

  ChipsetId  = GpioGetThisChipsetId ();
  ShellPrint (L"Supported ChipsetId: 0x%x\n", ChipsetId);
  if ((ChipsetId & Id) == 0) {
    ShellPrint (L"The chipsetId 0x%x is not supported\n", Id);
    return;
  }
  ChipsetId &= Id;

  for (BitIndex = 0; BitIndex < 8; BitIndex++) {
    if ( (( 1 << BitIndex) & ChipsetId) == 0) {
      continue;
    }

    GpioPad    = GPIOV2_PAD_ID(0, 1 << BitIndex, 0, 0, 0, 0);
    Controller = GpioGetController (GpioPad);
    //ShellPrint (L"Id=0x%x, Total Communities=%d, P2sbBase=0x%llx, SbRegBar=0x%x\n", Controller->ChipsetId, Controller->CommunityNum, Controller->P2sbBase, Controller->SbRegBar);
    for (CommunityIndex = 0; CommunityIndex < Controller->CommunityNum; CommunityIndex++) {
      if ((CommunityIdx != 0xFFFF) && (CommunityIdx != CommunityIndex)) {
        continue;
      }
      Community = &Controller->Communities[CommunityIndex];
      //ShellPrint (L"\nCommunity=%d, Total Groups=%d, PID=0x%x\n", CommunityIndex, Community->GroupsNum, Community->Pid);
      for (GroupIndex = 0; GroupIndex < Community->GroupsNum; GroupIndex++) {
        if ((GroupIdx != 0xFFFF) && (GroupIdx != GroupIndex)) {
          continue;
        }
        Group = &Community->Groups[GroupIndex];
        //ShellPrint (L"\n%a Group=%d tatal Pads=%d\n", Group->Name, GroupIndex, Group->PadsNum);
        ShellPrint (L"\n%a\n", Title);
        for (PadIndex = 0; PadIndex < Group->PadsNum; PadIndex++) {
          if ((PadIdx != 0xFFFF) && (PadIdx != PadIndex)) {
            continue;
          }
          GpioPad = Group->Pads[PadIndex];
          ShellPrint (L"%a\n", GetGpioCfgStr (GpioPad));
        }
      }
    }
  }
}


/**
  Dump GPIO PAD DW0 and DW1 registers for the specified GPIO PADs

  @param[in]  Id             GPIO controller ID
  @param[in]  CommunityIdx   GPIO community index
  @param[in]  GroupIdx       GPIO group index
  @param[in]  PadIdx         GPIO pad index

**/
VOID
DumpPads (
  IN UINT32                  Id,
  IN UINT32                  CommunityIdx,
  IN UINT32                  GroupIdx,
  IN UINT32                  PadIdx
  )
{
  GPIOV2_CONTROLLER          *Controller;
  UINT32                     ChipsetId;
  UINT32                     CommunityIndex;
  UINT32                     GroupIndex;
  UINT32                     PadIndex;
  GPIOV2_COMMUNITY           *Community;
  GPIOV2_GROUP               *Group;
  GPIOV2_PAD                 GpioPad;
  UINT32                     RegOffset;
  UINT32                     Dw0;
  UINT32                     Dw1;
  UINT32                     BitIndex;

  ChipsetId  = GpioGetThisChipsetId ();
  ShellPrint (L"Supported ChipsetId: 0x%x\n", ChipsetId);
  if ((ChipsetId & Id) == 0) {
    ShellPrint (L"The chipsetId 0x%x is not supported\n", Id);
    return;
  }
  ChipsetId &= Id;

  for (BitIndex = 0; BitIndex < 8; BitIndex++) {
    if ( (( 1 << BitIndex) & ChipsetId) == 0) {
      continue;
    }

    GpioPad    = GPIOV2_PAD_ID(0, 1 << BitIndex, 0, 0, 0, 0);
    Controller = GpioGetController (GpioPad);
    ShellPrint (L"Id=0x%x, Total Communities=%d, P2sbBase=0x%llx, SbRegBar=0x%llx\n", Controller->ChipsetId, Controller->CommunityNum, Controller->P2sbBase, Controller->SbRegBar);
    for (CommunityIndex = 0; CommunityIndex < Controller->CommunityNum; CommunityIndex++) {
      if ((CommunityIdx != 0xFFFF) && (CommunityIdx != CommunityIndex)) {
        continue;
      }
      Community = &Controller->Communities[CommunityIndex];
      ShellPrint (L"\nCommunity=%d, Total Groups=%d, PID=0x%x\n", CommunityIndex, Community->GroupsNum, Community->Pid);
      for (GroupIndex = 0; GroupIndex < Community->GroupsNum; GroupIndex++) {
        if ((GroupIdx != 0xFFFF) && (GroupIdx != GroupIndex)) {
          continue;
        }
        Group = &Community->Groups[GroupIndex];
        ShellPrint (L"\n%a Group=%d tatal Pads=%d\n", Group->Name, GroupIndex, Group->PadsNum);
        for (PadIndex = 0; PadIndex < Group->PadsNum; PadIndex++) {
          if ((PadIdx != 0xFFFF) && (PadIdx != PadIndex)) {
            continue;
          }
          GpioPad = Group->Pads[PadIndex];
          RegOffset = GetRegisterOffset (GpioPad, GpioV2Dw0Reg);
          Dw0 = GpioRead32 (GpioPad, RegOffset);
          Dw1 = GpioRead32 (GpioPad, RegOffset + 4);
          ShellPrint (L"  PAD%02d: DW0:0x%08x, DW1:0x%08x\n", PadIndex, Dw0, Dw1);
        }
      }
    }
  }
}


/**
  Print all the GPIO PADs related information.

  It prints avialble GPIO controller, community, group and pad info.

**/
VOID
ListPadsSummary (
  VOID
  )
{
  GPIOV2_CONTROLLER         *Controller;
  UINT32                    ChipsetId;
  UINT32                    CommunityIndex;
  UINT32                    GroupIndex;
  GPIOV2_COMMUNITY          *Community;
  GPIOV2_GROUP              *Group;
  GPIOV2_PAD                GpioPad;
  UINT32                    BitIndex;

  ChipsetId  = GpioGetThisChipsetId ();
  ShellPrint (L"Supported ChipsetId: 0x%x\n", ChipsetId);
  for (BitIndex = 0; BitIndex < 8; BitIndex++) {
    if ( (( 1 << BitIndex) & ChipsetId) == 0) {
      continue;
    }

    GpioPad    = GPIOV2_PAD_ID(0, 1 << BitIndex, 0, 0, 0, 0);
    Controller = GpioGetController (GpioPad);
    ShellPrint (L"\nChipsetId=0x%x, Total Communities=%d\n", Controller->ChipsetId, Controller->CommunityNum);
    for (CommunityIndex = 0; CommunityIndex < Controller->CommunityNum; CommunityIndex++) {
      Community = &Controller->Communities[CommunityIndex];
      ShellPrint (L"    Community=%d, Total Groups=%d\n", CommunityIndex, Community->GroupsNum);
      for (GroupIndex = 0; GroupIndex < Community->GroupsNum; GroupIndex++) {
        Group = &Community->Groups[GroupIndex];
        ShellPrint (L"    %a Group=%d tatal Pads=%d\n", Group->Name, GroupIndex, Group->PadsNum);
      }
    }
  }
}


/**
  Read or write GPIO Pin.

  @param[in]  Shell        Shell instance
  @param[in]  Argc         Number of command line arguments
  @param[in]  Argv         Command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandGpioFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  UINT32                  Index;
  UINT32                  Value;
  UINT32                  Id;
  UINT32                  CommunityIdx;
  UINT32                  GroupIdx;
  UINT32                  PadIdx;
  BOOLEAN                 IsRaw;

  Id           = 0xFFFF;
  CommunityIdx = 0xFFFF;
  GroupIdx     = 0xFFFF;
  PadIdx       = 0xFFFF;
  IsRaw        = FALSE;

  ShellPrint (L"Argc = %d\n", Argc);

  if ((Argc > 1) && (Argc < 12)) {
    //
    // Parse optional arguments
    //
    for (Index = 2; Index < Argc; Index++) {
      if (StrCmp (Argv[Index - 1], L"write") == 0){
        Value = (UINT32)StrHexToUintn (Argv[Index]);
        Index++;
      }

      if ((Index < Argc) && (StrCmp (Argv[Index], L"-id") == 0)) {
        if ((Index + 1) < Argc) {
          Id = (UINT32)StrHexToUintn (Argv[Index + 1]);
          Index++;
        } else {
          ShellPrint (L"Error, '-id' requires a value\n");
        }
      } else if ((Index < Argc) && (StrCmp (Argv[Index], L"-c") == 0)) {
        if ((Index + 1) < Argc) {
          CommunityIdx = (UINT32)StrHexToUintn (Argv[Index + 1]);
          Index++;
        } else {
          ShellPrint (L"Error, '-c' requires a value\n");
        }
      } else if ((Index < Argc) && (StrCmp (Argv[Index], L"-g") == 0)) {
        if ((Index + 1) < Argc) {
          GroupIdx = (UINT32)StrHexToUintn (Argv[Index + 1]);
          Index++;
        } else {
          ShellPrint (L"Error, '-g' requires a value\n");
        }
      } else if ((Index < Argc) && (StrCmp (Argv[Index], L"-p") == 0)) {
        if ((Index + 1) < Argc) {
          PadIdx = (UINT32)StrHexToUintn (Argv[Index + 1]);
          Index++;
        } else {
          ShellPrint (L"Error, '-p' requires a value\n");
        }
      } else if ((Index < Argc) && (StrCmp (Argv[Index], L"-raw") == 0)) {
        IsRaw = TRUE;
      } else if (Index < Argc) {
        ShellPrint (L"Invalid parameter '%s'", Argv[Index]);
        goto Usage;
      }
    }
  } else if (Argc >= 12) {
    ShellPrint (L"Error, too many arguments\n");
    goto Usage;
  } else {
    goto Usage;
  }

  if (StrCmp (Argv[1], L"list") == 0){
    ListPadsSummary ();
    return EFI_SUCCESS;
  } else if (StrCmp (Argv[1], L"dump") == 0){
    if (IsRaw) {
      DumpPads (Id, CommunityIdx, GroupIdx, PadIdx);
    } else {
      DumpPadsCfg (Id, CommunityIdx, GroupIdx, PadIdx);
    }
  } else if ((Argc == 11) && (StrCmp (Argv[1], L"write") == 0)){
    GpioWrite (Id, CommunityIdx, GroupIdx, PadIdx, Value);
  } else {
    goto Usage;
  }

  return EFI_SUCCESS;

Usage:
  ShellPrint (L"Usage: %s list\n", Argv[0]);
  ShellPrint (L"       %s dump  [-id ChipsetId] [-c CommunityIndex] [-g GroupIndex] [-p PadIndex] [-raw]\n", Argv[0]);
  ShellPrint (L"       %s write <0 or 1> <-id ChipsetId> <-c CommunityIndex> <-g GroupIndex> <-p PadIndex>\n", Argv[0]);
  ShellPrint (L"       All input numeric values are interpreted as hexadecimal numbers\n");

  ShellPrint (L"       Example Commands:\n");
  ShellPrint (L"       # print available ChipsetId, communities, groups and GPIO pad numbers\n");
  ShellPrint (L"         gpio list \n");

  ShellPrint (L"       # Dump GPIO DW0 and DW1 register for all available GPIO\n");
  ShellPrint (L"         gpio dump -raw \n");

  ShellPrint (L"       # Dump all GPIO pads in community 0 group 0 in SBL configuration format\n");
  ShellPrint (L"         gpio dump -c 0 -g 0\n");

  ShellPrint (L"       # Write 1 to set output state as High for GPIO controller 2 community 1 group 0 pad 5 \n");
  ShellPrint (L"         gpio write 1 -id 0x02 -c 1 -g 0 -p 5\n");

  return EFI_ABORTED;
}
