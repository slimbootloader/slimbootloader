/** @file
  Register names for Serial IO UART Controllers

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _SERIAL_IO_UART_REGS_H_
#define _SERIAL_IO_UART_REGS_H_


//
//  MMIO Convergance Layer
//
#define R_SERIAL_IO_MEM_PPR_RESETS_8BIT      0x81
#define R_SERIAL_IO_MEM_PPR_CLK              0x200
#define B_SERIAL_IO_MEM_PPR_CLK_EN           BIT0
#define B_SERIAL_IO_MEM_PPR_CLK_UPDATE       BIT31
#define V_SERIAL_IO_MEM_PPR_CLK_M_DIV        0x25A
#define V_SERIAL_IO_MEM_PPR_CLK_N_DIV        0x7FFF


#endif // _SERIAL_IO_UART_REGS_H_
