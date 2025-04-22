/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_PCR_H_
#define _PCH_REGS_PCR_H_

#include <PchReservedResources.h>

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
/*#define PCH_PCR_BOOT_SCRIPT_WRITE(Width, Pid, Offset, Count, Buffer) \
          S3BootScriptSaveMemWrite (Width, PCH_PCR_ADDRESS (Pid, Offset), Count, Buffer); \

#define PCH_PCR_BOOT_SCRIPT_READ_WRITE(Width, Pid, Offset, DataOr, DataAnd) \
          S3BootScriptSaveMemReadWrite (Width, PCH_PCR_ADDRESS (Pid, Offset), DataOr, DataAnd); \

#define PCH_PCR_BOOT_SCRIPT_READ(Width, Pid, Offset, BitMask, BitValue) \
          S3BootScriptSaveMemPoll (Width, PCH_PCR_ADDRESS (Pid, Offset), BitMask, BitValue, 1, 1);
*/
/**
  Definition for SBI PID
  The PCH_SBI_PID defines the PID for PCR MMIO programming and PCH SBI programming as well.
**/

#define PID_CNVI       0x73
#define PID_ICLK       0xAD
#define PID_DMI        0x88
#define PID_SPG        0x86  // ADPS only
#define PID_SPF        0x85
#define PID_SPE        0x84
#define PID_SPD        0x83
#define PID_SPC        0x82
#define PID_SPB        0x81
#define PID_SPA        0x80
#define PID_ITSS       0xC4
#define PID_RTC_HOST   0xC3
#define PID_GPIOCOM0   0x6E
#define PID_GPIOCOM1   0x6D
#define PID_GPIOCOM2   0x6C
#define PID_GPIOCOM3   0x6B
#define PID_GPIOCOM4   0x6A
#define PID_GPIOCOM5   0x69

#endif
