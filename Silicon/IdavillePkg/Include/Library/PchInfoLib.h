/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_INFO_LIB_H_
#define _PCH_INFO_LIB_H_

typedef UINT8 PCH_STEPPING;
#define PCH_STEPPING_MAX        0xFF

typedef UINT8 PCH_SERIES;
#define PCH_H                   1
#define PCH_LP                  2
#define PCH_N                   3

typedef UINT8 PCH_GENERATION;
#define CDF_PCH                 0x80

/**
  Return Pch Series

  @retval PCH_SERIES            Pch Series
**/
PCH_SERIES
PchSeries (
  VOID
  );

/**
  Return Pch Generation

  @retval PCH_GENERATION            Pch Generation
**/
PCH_GENERATION
PchGeneration (
  VOID
  );

/**
  Return Pch stepping type

  @retval PCH_STEPPING            Pch stepping type
**/
PCH_STEPPING
PchStepping (
  VOID
  );

/**
  Get Pch Maximum Pcie Root Port Number

  @retval Pch Maximum Pcie Root Port Number
**/
UINT8
GetPchMaxPciePortNum (
  VOID
  );

/**
  Get Pch Maximum Sata Port Number

  @retval Pch Maximum Sata Port Number
**/
UINT8
GetPchMaxSataPortNum (
  VOID
  );

#endif // _PCH_INFO_LIB_H_
