/** @file
  Shell command `perf` to display system performance data.

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/DebugLib.h>
#include <Guid/CsmePerformanceInfoGuid.h>
#include <Guid/PerformanceInfoGuid.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/LoaderPerformanceLib.h>
#include <Library/TimeStampLib.h>

/**
  Display performance data.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandPerfFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandPerf = {
  L"perf",
  L"Display performance data",
  &ShellCommandPerfFunc
};

/**
  Print loader PERFORMANCE_INFO data.

  @param[in]  PerfData         pointer to performance data
  @param[in]  InMicroSecond    Show host performance time in MicroSecond when it is TRUE

**/
VOID
EFIAPI
PrintPerformanceInfo (
  IN PERFORMANCE_INFO *PerfData,
  IN BOOLEAN          InMicroSecond
  )
{
  UINT32 Idx;
  UINT64 Time;
  UINT64 PrevTime;
  UINT16 Id;
  UINT64 Tsc;

  PrevTime = 0;

  ShellPrint (L" Id   |   Time (ms)   | Delta (ms)  | Description\n");
  ShellPrint (L"------+---------------+-------------+---------------------------\n");
  for (Idx = 0; Idx < PerfData->Count; Idx++) {
    Tsc  = PerfData->TimeStamp[Idx] & 0x0000FFFFFFFFFFFFULL;
    Id   = (RShiftU64 (PerfData->TimeStamp[Idx], 48)) & 0xFFFF;
    if (InMicroSecond) {
      Time = TimeStampTickToMicroSecond (Tsc);
    } else {
      Time = DivU64x32 (Tsc, PerfData->Frequency);
    }
    ShellPrint (L" %4x | %10d ms | %8d ms | %a\n", Id, (UINT32)(UINTN)Time, (UINT32)(UINTN)(Time - PrevTime), PerfIdToStr (Id, NULL));
    PrevTime = Time;
  }
  ShellPrint (L"------+---------------+-------------+---------------------------\n");
}

/**
  Print CSME PERFORMANCE_INFO data.

  @param[in]  PerfData    pointer to performance data

**/
STATIC
VOID
EFIAPI
PrintCsmePerformanceInfo (
  VOID
  )
{
  UINT8                 Index;
  UINT32                PrevTS;
  EFI_HOB_GUID_TYPE     *GuidHob;
  CSME_PERFORMANCE_INFO  *CsmePerformanceInfo;

  GuidHob = GetNextGuidHob (&gCsmePerformanceInfoGuid, GetHobList());
  if (GuidHob == NULL) {
    return;
  }

  CsmePerformanceInfo = (CSME_PERFORMANCE_INFO *)GET_GUID_HOB_DATA (GuidHob);

  ShellPrint (L"CSME Performance Info\n");
  ShellPrint (L"=======================\n\n");

  ShellPrint (L" Id   | Time (ms)  | Delta (ms) | Description\n");
  ShellPrint (L"------+------------+------------+---------------------------\n");
  //
  // Initialize previous time stamp to CSME ROM Start execution TS
  //
  PrevTS = CsmePerformanceInfo->BootPerformanceData[1];
  for (Index = 1; Index < CsmePerformanceInfo->BootDataLength; Index++) {
    //
    // Ignore timestamps with value as 0
    //
    if (CsmePerformanceInfo->BootPerformanceData[Index] == 0) {
      continue;
    }
    ShellPrint (L" %4x | %7d ms | %7d ms | %a\n",
          Index,\
          CsmePerformanceInfo->BootPerformanceData[Index],\
          CsmePerformanceInfo->BootPerformanceData[Index] - PrevTS,\
          PerfIdToStr (Index, CsmePerfIdToStr));
    PrevTS = CsmePerformanceInfo->BootPerformanceData[Index];
  }
}

/**
  Display performance data.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandPerfFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  VOID             *GuidHob;
  PERFORMANCE_INFO *PerfData;
  BOOLEAN          InMicroSecond;

  InMicroSecond = FALSE;
  if (Argc == 2) {
    if (StrCmp (Argv[1], L"-d") == 0) {
      InMicroSecond = TRUE;
    } else {
      goto usage;
    }
  } else if (Argc > 2) {
    goto usage;
  }

  GuidHob = GetNextGuidHob (&gLoaderPerformanceInfoGuid, GetHobList());
  if (GuidHob == NULL) {
    ASSERT (FALSE);
    return RETURN_NOT_FOUND;
  }

  PerfData = (PERFORMANCE_INFO *)GET_GUID_HOB_DATA (GuidHob);

  ShellPrint (L"Loader Performance Info\n");
  ShellPrint (L"=======================\n\n");
  PrintPerformanceInfo (PerfData, InMicroSecond);

  // Print CSME boot performance
  if ((PcdGet32 (PcdBootPerformanceMask) & BIT2) != 0) {
    PrintCsmePerformanceInfo ();
  }

  return EFI_SUCCESS;

  usage:
  ShellPrint (L"Usage: %s [-d]\n", Argv[0]);
  ShellPrint (L"\n"
              L"Flags:\n"
              L"  -d     Show host performance time in MicroSecond, by default in MilliSecond\n");
  return EFI_ABORTED;
}
