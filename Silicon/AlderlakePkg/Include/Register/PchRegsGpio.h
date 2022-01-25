/** @file
  Register names for PCH GPIO

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_REGS_GPIO_H_
#define _PCH_REGS_GPIO_H_


//
// PADCFG register is split into multiple DW registers
// S_GPIO_PCR_PADCFG refers to number of bytes used by all those registers for one pad
//
#define S_GPIO_PCR_PADCFG               0x10


//Interrupt number
#define B_GPIO_PCR_INTSEL               0x7F


//
// GPIO native features pins data
//
#define PCH_GPIO_CNVI_BRI_RGI_NUMBER_OF_PINS 4


#endif // _GPIO_REGS_H_
