/** @file
  Header file for Gpio V2 Controller Interace library.

  Copyright (c) 2021 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIOV2_CONTROLLER_PRIVATE_H_
#define _GPIOV2_CONTROLLER_PRIVATE_H_

typedef struct _GPIOV2_PROTOCOL            GPIOV2_SERVICES;
typedef union  _GPIOV2_SIGNAL              GPIOV2_SIGNAL;
typedef struct _GPIOV2_NATIVE_SIGNAL_DATA  GPIOV2_NATIVE_SIGNAL_DATA;
typedef struct _GPIOV2_PAD_SIGNAL          GPIOV2_PAD_SIGNAL;
typedef struct _GPIOV2_PAD_DATA            GPIOV2_PAD_DATA;

#include <Include/P2SbSidebandAccessLib.h>
#include <Include/GpioV2Pad.h>

typedef struct {
  UINT8       PmcGpeDwxVal;
  UINT8       GpioGpeDwxVal;
} GPIOV2_GROUP_TO_GPE_MAPPING;

typedef struct _GPIOV2_SERVICES_PRIVATE GPIOV2_SERVICES_PRIVATE;

//
// Stores information about the unlock request for each pad in the group.
// Groups are assumed to be limited to 32 pads. Each bit in each UINT32 fields
// corresponds to single pad in the group.
//
typedef struct {
  UINT32  UnlockConfigBitMask;
  UINT32  UnlockTxBitMask;
} GPIOV2_UNLOCK_GROUP_DATA;

typedef struct {
  GPIOV2_PAD  GpioPad;
  UINT32      GpioPadMode:5;
  UINT32      Reserved:27;
} GPIOV2_PAD_MODE_INFO;

typedef union _GPIOV2_SIGNAL {
  struct {
    UINT32  InterfaceIndex: 8;
    UINT32  SignalType: 8;
    UINT32  Interface: 8;
    UINT32  Reserved: 8;
  }                               Fields;
  UINT32                          Value;
} GPIOV2_SIGNAL;

typedef struct _GPIOV2_NATIVE_SIGNAL_DATA {
  GPIOV2_SIGNAL                   Signal;
  GPIOV2_IOSTANDBY_STATE          IosState;
  GPIOV2_IOSTANDBY_TERM           IosTerm;
  BOOLEAN                         VirtualWireMessageMappingAvailable;
  UINT8                           VwIndex;
  UINT8                           BitPosition;
} GPIOV2_NATIVE_SIGNAL_DATA;

typedef struct _GPIOV2_PAD_SIGNAL {
  GPIOV2_SIGNAL                   Signal;
  GPIOV2_PAD_MODE                 PadMode;
} GPIOV2_PAD_SIGNAL;

typedef struct _GPIOV2_PAD_DATA {
  GPIOV2_PAD_SIGNAL               *PadSignals;
  UINT32                          PadSignalsNum;
} GPIOV2_PAD_DATA;

typedef struct {
  CONST CHAR8                     *Name;
  GPIOV2_PAD_GROUP                GpioPadGroup;
  GPIOV2_GROUP_TO_GPE_MAPPING     GroupToGpeMapping;
  GPIOV2_GROUP_REGISTERS_OFFSETS  RegisterOffsets;
  UINT32                          PadsNum;
  GPIOV2_PAD                      *Pads;
  GPIOV2_PAD_DATA                 *PadDataArray;
} GPIOV2_GROUP;

typedef struct GPIOV2_COMMUNITY_S {
  GPIOV2_COMMUNITY_REGISTERS_OFFSETS  RegisterOffsets;
  UINT32                              GroupsNum;
  GPIOV2_GROUP                        *Groups;
  UINT32                              Pid;
} GPIOV2_COMMUNITY;

#endif // _GPIOV2_CONTROLLER_PRIVATE_H_
