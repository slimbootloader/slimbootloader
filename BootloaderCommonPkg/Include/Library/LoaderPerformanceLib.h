/** @file

  Copyright (c) 2016 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _LOADER_PERF_LIB_H_
#define _LOADER_PERF_LIB_H_

typedef CHAR8 * (EFIAPI *PERF_ID_TO_STR) (UINT32 Id);

/**
  Add a given performance measure point timestamp.

  @param[in]  Id        Measure point Id
  @param[in]  Value     Timestamp value

**/
VOID
AddMeasurePointTimestamp (
  IN  UINT16         Id,
  IN  UINT64         Value
  );

/**
  Add current performance measure point timestamp.

  @param[in]  Id          Measure point Id

**/
VOID
AddMeasurePoint (
  IN  UINT16         Id
  );

/**
  Print Bootloader Measure Point information.

  @param[in]  PerfData          A pointer indicating BL_PERF_DATA instance to print performance data
  @param[in]  PerfIdToStrTbl    A pointer to description table corresponding to Id

**/
VOID
EFIAPI
PrintMeasurePoint (
  IN BL_PERF_DATA   *PerfData,
  IN PERF_ID_TO_STR  PerfIdToStrTbl
  );

/**
  Provide description string corresponding to Id.

  If there is a description in default description table corresponding to Id,
  default description will be returned.
  If there is no description in default description table,
    look for description in overrided description table once if it is provided
  Otherwise,just return empty string

  @param[in]  Id                MeasurePoint Id
  @param[in]  PerfIdToStrTbl    A pointer to description table

  @retval Description string
**/
CHAR8 *
EFIAPI
PerfIdToStr (
  IN UINT32            Id,
  IN PERF_ID_TO_STR    PerfIdToStrTbl
  );

/**
  Provide description string for csme perf data corresponding to Id.

  @param[in]  Id  MeasurePoint Id

  @retval Default description string
**/
CHAR8 *
EFIAPI
CsmePerfIdToStr (
  IN UINT32          Id
  );

#endif
