/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/GpioLib.h>
#include <GpioPinsCmlvH.h>
#include <GpioPinsCmlvLp.h>

//
// GPIO_PAD Fileds
//
typedef struct {
  UINT32    PadNum:16;
  UINT32    GrpIdx:8;
  UINT32    ChipsetId:4;
  UINT32    Rsvd:4;
} GPIO_PAD_FIELD;

//
// GPIO_CFG_DATA DW1 fields
//
typedef struct {
  UINT32    Rsvd1:16;
  UINT32    PadNum:8;
  UINT32    GrpIdx:5;
  UINT32    Rsvd2:3;
} GPIO_CFG_DATA_DW1;

#define CNL_LP_CHIPSET_ID   0x4
#define CNL_H_CHIPSET_ID    0x3
