/** @file
  Register names for PCH Serial IO Controllers

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_SERIAL_IO_
#define _PCH_REGS_SERIAL_IO_

#define R_SERIAL_IO_CFG_BAR0_LOW                        0x10
#define R_SERIAL_IO_CFG_BAR0_HIGH                       0x14
#define R_SERIAL_IO_CFG_BAR1_LOW                        0x18
#define R_SERIAL_IO_CFG_BAR1_HIGH                       0x1C

#define R_SERIAL_IO_CFG_PME_CTRL_STS                    0x84

#define R_SERIAL_IO_CFG_D0I3MAXDEVPG                    0xA0

#define R_SERIAL_IO_MEM_PPR_CLK                         0x200
#define B_SERIAL_IO_MEM_PPR_CLK_EN                      BIT0
#define B_SERIAL_IO_MEM_PPR_CLK_UPDATE                  BIT31
#define V_SERIAL_IO_MEM_PPR_CLK_M_DIV                   0x30
#define V_SERIAL_IO_MEM_PPR_CLK_N_DIV                   0xC35

#define R_SERIAL_IO_MEM_PPR_RESETS                      0x204
#define B_SERIAL_IO_MEM_PPR_RESETS_FUNC                 BIT0
#define B_SERIAL_IO_MEM_PPR_RESETS_APB                  BIT1
#define B_SERIAL_IO_MEM_PPR_RESETS_IDMA                 BIT2

#endif
