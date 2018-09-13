/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
