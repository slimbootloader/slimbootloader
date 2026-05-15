/** @file
  Common Register names for Serial IO Controllers

  Copyright (c) 2021 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _COMMON_SERIAL_IO_REGS_H_
#define _COMMON_SERIAL_IO_REGS_H_

#define R_SERIAL_IO_CFG_PME_CTRL_STS                        0x84

#define R_SERIAL_IO_CFG_D0I3MAXDEVPG                        0xA0

//
// Serial IO Controllers MMIO Registers
// registers accessed : BAR0 + offset
//
#define R_SERIAL_IO_MEM_PPR_RESETS                          0x204
#define B_SERIAL_IO_MEM_PPR_RESETS_FUNC                     BIT0
#define B_SERIAL_IO_MEM_PPR_RESETS_APB                      BIT1
#define B_SERIAL_IO_MEM_PPR_RESETS_IDMA                     BIT2

#endif // _COMMON_SERIAL_IO_REGS_H_
