/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/TimeStampLib.h>
#include <Library/BootloaderCommonLib.h>

/**
  Add a given performance measure point timestamp.

  @param[in]  Id        Measure point Id
  @param[in]  Value     Timestamp value

**/
VOID
AddMeasurePointTimestamp (
  IN  UINT16         Id,
  IN  UINT64         Value
  )
{
  BL_PERF_DATA   *PerfData;

  PerfData = GetPerfDataPtr();
  if (PerfData->PerfIndex >= MAX_TS_NUM) {
    return;
  }
  ((UINT16 *)&Value)[3] = Id;
  PerfData->TimeStamp[PerfData->PerfIndex++] = Value;
}

/**
  Add current performance measure point timestamp.

  @param[in]  Id          Measure point Id

**/
VOID
AddMeasurePoint (
  IN  UINT16         Id
  )
{
  AddMeasurePointTimestamp (Id, ReadTimeStamp());
}
