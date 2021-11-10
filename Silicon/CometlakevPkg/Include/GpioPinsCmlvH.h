/** @file
  GPIO pins for CNL-PCH-H,

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _GPIO_PINS_CNL_H_H_
#define _GPIO_PINS_CNL_H_H_
///
/// This header file should be used together with
/// PCH GPIO lib in C and ASL. All defines used
/// must match both ASL/C syntax
///

///
/// Unique ID used in GpioPad defines
///
#define GPIO_CNL_H_CHIPSET_ID       0x01

///
/// CNL H GPIO Groups
/// Use below for functions from PCH GPIO Lib which
/// require GpioGroup as argument
///
#define GPIO_CNL_H_GROUP_GPP_A  0x0100
#define GPIO_CNL_H_GROUP_GPP_B  0x0101
#define GPIO_CNL_H_GROUP_GPP_C  0x0102
#define GPIO_CNL_H_GROUP_GPP_D  0x0103
#define GPIO_CNL_H_GROUP_GPP_E  0x0104
#define GPIO_CNL_H_GROUP_GPP_F  0x0105
#define GPIO_CNL_H_GROUP_GPP_G  0x0106
#define GPIO_CNL_H_GROUP_GPP_H  0x0107
#define GPIO_CNL_H_GROUP_GPP_I  0x0108
#define GPIO_CNL_H_GROUP_GPD    0x0109

///
/// CNL H GPIO pins
/// Use below for functions from PCH GPIO Lib which
/// require GpioPad as argument. Encoding used here
/// has all information required by library functions
///
#define GPIO_CNL_H_GPP_A1               0x01000001
#define GPIO_CNL_H_GPP_A17              0x01000011

#define GPIO_CNL_H_GPP_B2               0x01010002
#define GPIO_CNL_H_GPP_B23              0x01010017

#define GPIO_CNL_H_GPP_C1               0x01020001
#define GPIO_CNL_H_GPP_C2               0x01020002
#define GPIO_CNL_H_GPP_C8               0x01020008
#define GPIO_CNL_H_GPP_C9               0x01020009
#define GPIO_CNL_H_GPP_C12              0x0102000C
#define GPIO_CNL_H_GPP_C13              0x0102000D
#define GPIO_CNL_H_GPP_C20              0x01020014
#define GPIO_CNL_H_GPP_C21              0x01020015


#define GPIO_CNL_H_GPP_E3               0x01040003

#define GPIO_CNL_H_GPP_F2               0x01050002
#define GPIO_CNL_H_GPP_F5               0x01050005
#define GPIO_CNL_H_GPP_F8               0x01050008
#define GPIO_CNL_H_GPP_F22              0x01050016

#define GPIO_CNL_H_GPP_G1               0x01060001
#define GPIO_CNL_H_GPP_G2               0x01060002
#define GPIO_CNL_H_GPP_G6               0x01060006
#define GPIO_CNL_H_GPP_G7               0x01060007
#define GPIO_CNL_H_GPP_G10              0x0106000A
#define GPIO_CNL_H_GPP_G18              0x01060012
#define GPIO_CNL_H_GPP_G22              0x01060016


#define GPIO_CNL_H_GPP_I1               0x01080001
#define GPIO_CNL_H_GPP_I3               0x01080003
#define GPIO_CNL_H_GPP_I10              0x0108000A


#endif
