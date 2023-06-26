/** @file
  Private Control Register definitions

  Copyright (c) 2021 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCR_DEFINE_H_
#define _PCR_DEFINE_H_

#include "PchReservedResources.h"

/**
  Definition for PCR address
  The PCR address is used to the PCR MMIO programming

  SBREG_BAR_20BITADDRESS is configured by SoC

  SBREG_BAR_20BITADDRESS=1, the format has included 16b addressing.
  +---------------------------------------------------------------------------------------------+
  | Addr[63:28]    | Addr[27:24]           | Addr[23:16]     | Addr[15:2]           | Addr[1:0] |
  +----------------+-----------------------+-----------------+----------------------------------+
  | REG_BAR[63:28] | TargetRegister[19:16] | TargetPort[7:0] | TargetRegister[15:2]             |
  +---------------------------------------------------------------------------------------------+

  SBREG_BAR_20BITADDRESS=0
  +---------------------------------------------------------------------------------------------+
  | Addr[63:24]    | Addr[27:24]           | Addr[23:16]     | Addr[15:2]           | Addr[1:0] |
  +----------------+-----------------------+-----------------+----------------------------------+
  | REG_BAR[63:24] | REG_BAR[27:24]        | TargetPort[7:0] | TargetRegister[15:2]             |
  +---------------------------------------------------------------------------------------------+
**/
#define PCH_PCR_ADDRESS(Pid, Offset)    (PCH_PCR_BASE_ADDRESS | (UINT32) (((Offset) & 0x0F0000) << 8) | ((UINT8)(Pid) << 16) | (UINT16) ((Offset) & 0xFFFF))

/**
  PCH PCR boot script accessing macro
  Those macros are only available for DXE phase.
**/
#define PCH_PCR_BOOT_SCRIPT_WRITE(Width, Pid, Offset, Count, Buffer) \
          S3BootScriptSaveMemWrite (Width, PCH_PCR_ADDRESS (Pid, Offset), Count, Buffer); \

#define PCH_PCR_BOOT_SCRIPT_READ_WRITE(Width, Pid, Offset, DataOr, DataAnd) \
          S3BootScriptSaveMemReadWrite (Width, PCH_PCR_ADDRESS (Pid, Offset), DataOr, DataAnd); \

#define PCH_PCR_BOOT_SCRIPT_READ(Width, Pid, Offset, BitMask, BitValue) \
          S3BootScriptSaveMemPoll (Width, PCH_PCR_ADDRESS (Pid, Offset), BitMask, BitValue, 1, 1);

typedef UINT8          P2SB_PID;

typedef union {
  UINT16 Pid16bit;
  struct {
    P2SB_PID LocalPid;
    UINT8    SegmentId;
  } PortId;
} P2SB_PORT_16_ID;

#define GET_P2SB_LOCAL_PID(id) ((id) & 0xFF)

#endif // _PCR_DEFINE_H_
