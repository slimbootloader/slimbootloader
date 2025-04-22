/** @file

  Copyright (c) 2021 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __STAGE_COMMON_BOARD_INIT_LIB_H__
#define __STAGE_COMMON_BOARD_INIT_LIB_H__

#include <Library/VariableLib.h>

#define VAR_NAME_VAR_STS    L"VarSts"
#define VAR_LENGTH_VAR_STS  16

#define INITIAL_CODE_REGION_LENGTH  ALIGN_UP (FixedPcdGet32(PcdStage1ASize), SIZE_64KB)
#define INTTIAL_CODE_REGION_BASE    (UINT32)(BASE_4GB - INITIAL_CODE_REGION_LENGTH)

/**
  Initialize Variable.

**/
VOID
VariableInitialize (
  VOID
  );

/**
  Initial CodeRegion covers only TopSwap region only.
  This will enlarge CodeRegion cache after TempRamInit and TempRamExit.

**/
VOID
EFIAPI
EnlargeCodeRegionCache (
  VOID
  );

#endif // __STAGE_COMMON_BOARD_INIT_LIB_H__
