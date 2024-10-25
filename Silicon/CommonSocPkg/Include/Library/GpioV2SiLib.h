
/** @file
  Header file for Gpiov2 Silicon library

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _GPIOV2_SI_LIB_H_
#define _GPIOV2_SI_LIB_H_

#include <GpioV2Config.h>

typedef struct {
  UINT8       PmcGpeDwxVal;
  UINT8       GpioGpeDwxVal;
} GPIOV2_GROUP_TO_GPE_MAPPING;

typedef struct {
  CONST CHAR8                         *Name;
  GPIOV2_PAD_GROUP                    GpioPadGroup;
  GPIOV2_GROUP_TO_GPE_MAPPING         GroupToGpeMapping;
  GPIOV2_GROUP_REGISTERS_OFFSETS      RegisterOffsets;
  UINT32                              PadsNum;
  GPIOV2_PAD                          *Pads;
  // The size from a PAD DW0 to the nexe PAD DW0 in bytes.
  UINT8                              PadDwSize;
} GPIOV2_GROUP;

typedef struct {
  GPIOV2_COMMUNITY_REGISTERS_OFFSETS  RegisterOffsets;
  UINT32                              Pid;
  UINT32                              GroupsNum;
  GPIOV2_GROUP                        *Groups;
} GPIOV2_COMMUNITY;

typedef struct {
  UINT64                              SbRegBar;
  UINT64                              P2sbBase;
  UINT32                              ChipsetId;
  UINT32                              CommunityNum;
  GPIOV2_COMMUNITY                    *Communities;
} GPIOV2_CONTROLLER;


/**
  This procedure retrieves pointer a P2SB controller where PAD belong to

  @param[in] GpioPad              GPIO PAD

  @retval                         A P2SB controller pointer
**/
GPIOV2_CONTROLLER *
EFIAPI
GpioGetController (
  IN GPIOV2_PAD             GpioPad
  );

/**
  Get GPIO Chipset ID specific to PCH generation and series
**/
UINT32
EFIAPI
GpioGetThisChipsetId (
  VOID
  );

#endif
