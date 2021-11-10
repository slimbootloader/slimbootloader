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
#define GPIO_CNL_H_CHIPSET_ID       0x3

///
/// CNL H GPIO Groups
/// Use below for functions from PCH GPIO Lib which
/// require GpioGroup as argument
///
#define GPIO_CNL_H_GROUP_GPP_A  0x0300
#define GPIO_CNL_H_GROUP_GPP_B  0x0301
#define GPIO_CNL_H_GROUP_GPP_C  0x0302
#define GPIO_CNL_H_GROUP_GPP_D  0x0303
#define GPIO_CNL_H_GROUP_GPP_E  0x0304
#define GPIO_CNL_H_GROUP_GPP_F  0x0305
#define GPIO_CNL_H_GROUP_GPP_G  0x0306
#define GPIO_CNL_H_GROUP_GPP_H  0x0307
#define GPIO_CNL_H_GROUP_GPP_I  0x0308
#define GPIO_CNL_H_GROUP_GPP_J  0x0309
#define GPIO_CNL_H_GROUP_GPP_K  0x030A
#define GPIO_CNL_H_GROUP_GPD    0x030B
#define GPIO_CNL_H_GROUP_VGPIO  0x030C

///
/// CNL H GPIO pins
/// Use below for functions from PCH GPIO Lib which
/// require GpioPad as argument. Encoding used here
/// has all information required by library functions
///
#define GPIO_CNL_H_GPP_A1               0x03000001
#define GPIO_CNL_H_GPP_A17              0x03000011

#define GPIO_CNL_H_GPP_B2               0x03010002
#define GPIO_CNL_H_GPP_B23              0x03010017


#define GPIO_CNL_H_GPP_E3               0x03040003

#define GPIO_CNL_H_GPP_F2               0x03050002
#define GPIO_CNL_H_GPP_F5               0x03050005
#define GPIO_CNL_H_GPP_F8               0x03050008
#define GPIO_CNL_H_GPP_F22              0x03050016


#define GPIO_CNL_H_GPP_I1               0x03080001
#define GPIO_CNL_H_GPP_I3               0x03080003
#define GPIO_CNL_H_GPP_I10              0x0308000A


#define GPIO_CNL_H_GPP_K1               0x030A0001
#define GPIO_CNL_H_GPP_K2               0x030A0002
#define GPIO_CNL_H_GPP_K6               0x030A0006
#define GPIO_CNL_H_GPP_K7               0x030A0007
#define GPIO_CNL_H_GPP_K10              0x030A000A
#define GPIO_CNL_H_GPP_K18              0x030A0012
#define GPIO_CNL_H_GPP_K22              0x030A0016


#define GPIO_CNL_H_VGPIO6               0x030C0006


#endif
