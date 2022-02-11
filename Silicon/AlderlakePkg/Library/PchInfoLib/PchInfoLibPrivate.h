/** @file
  Private header for PCH Info Lib.

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

typedef struct {
  UINT8 DevNum;
  UINT8 Pid;
  UINT8 RpNumBase;
} PCH_PCIE_CONTROLLER_INFO;

/**
  Structure for PCH SKU string mapping
**/
struct PCH_SKU_STRING {
  UINT16        Id;
  CHAR8         *String;
};

extern struct PCH_SKU_STRING mSkuStrs[];

/**
  Determine Pch Series based on Device Id

  @param[in] LpcDeviceId      Lpc Device Id

  @retval PCH_SERIES          Pch Series
**/
PCH_SERIES
PchSeriesFromLpcDid (
  IN UINT16 LpcDeviceId
  );

/**
Determine Pch Generation based on Device Id

@param[in] LpcDeviceId            Lpc Device Id

@retval PCH_GENERATION            Pch Generation
**/
PCH_GENERATION
PchGenerationFromDid (
  IN UINT16 LpcDeviceId
  );

/**
  Determine Pch Stepping based on Revision ID

  @param[in] RevId            Pch Revision Id

  @retval PCH_STEPPING        Pch Stepping
**/
PCH_STEPPING
PchSteppingFromRevId (
  IN UINT8 RevId
  );

/**
  Print Pch Stepping String

  @param[out] Buffer         Output buffer of string
  @param[in]  BufferSize     Buffer Size
  @param[in]  PchStep        Pch Stepping Type

  @retval VOID
**/
VOID
PchPrintSteppingStr (
  OUT CHAR8        *Buffer,
  IN  UINT32       BufferSize,
  IN  PCH_STEPPING PchStep
  );
