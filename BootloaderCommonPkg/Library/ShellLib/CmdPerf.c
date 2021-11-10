/** @file
  Shell command `perf` to display system performance data.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/DebugLib.h>
#include <Guid/PerformanceInfoGuid.h>
#include <Library/HobLib.h>

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

  @param[in]  PerfData    pointer to performance data

**/
STATIC
VOID
EFIAPI
PrintPerformanceInfo (
  IN PERFORMANCE_INFO *PerfData
  )
{
  UINT32 Idx, Time, PrevTime;
  UINT16  Id;
  UINT64 Tsc;

  PrevTime = 0;

  ShellPrint (L" Id   | Time (ms)  | Delta (ms) \n");
  ShellPrint (L"------+------------+------------\n");
  for (Idx = 0; Idx < PerfData->Count; Idx++) {
    Tsc  = PerfData->TimeStamp[Idx] & 0x0000FFFFFFFFFFFFULL;
    Id   = (RShiftU64 (PerfData->TimeStamp[Idx], 48)) & 0xFFFF;
    Time = (UINT32)DivU64x32 (Tsc, PerfData->Frequency);
    ShellPrint (L" %4x | %7d ms | %7d ms\n", Id, Time, Time - PrevTime);
    PrevTime = Time;
  }
  ShellPrint (L"------+------------+------------\n");
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

  GuidHob = GetNextGuidHob (&gLoaderPerformanceInfoGuid, GetHobList());
  if (GuidHob == NULL) {
    ASSERT (FALSE);
    return RETURN_NOT_FOUND;
  }

  PerfData = (PERFORMANCE_INFO *)GET_GUID_HOB_DATA (GuidHob);

  ShellPrint (L"Loader Performance Info\n");
  ShellPrint (L"=======================\n\n");
  PrintPerformanceInfo (PerfData);

  return EFI_SUCCESS;
}
