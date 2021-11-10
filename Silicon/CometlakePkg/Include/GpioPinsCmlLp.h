/** @file
  GPIO pins for CNL-PCH-LP,

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _GPIO_PINS_CNL_LP_H_
#define _GPIO_PINS_CNL_LP_H_
///
/// This header file should be used together with
/// PCH GPIO lib in C and ASL. All defines used
/// must match both ASL/C syntax
///

///
/// Unique ID used in GpioPad defines
///
#define GPIO_CNL_LP_CHIPSET_ID      0x4

///
/// CNL LP GPIO Groups
/// Use below for functions from PCH GPIO Lib which
/// require GpioGroup as argument
///
#define GPIO_CNL_LP_GROUP_GPP_A  0x0400
#define GPIO_CNL_LP_GROUP_GPP_B  0x0401
#define GPIO_CNL_LP_GROUP_GPP_C  0x0402
#define GPIO_CNL_LP_GROUP_GPP_D  0x0403
#define GPIO_CNL_LP_GROUP_GPP_E  0x0404
#define GPIO_CNL_LP_GROUP_GPP_F  0x0405
#define GPIO_CNL_LP_GROUP_GPP_G  0x0406
#define GPIO_CNL_LP_GROUP_GPP_H  0x0407
#define GPIO_CNL_LP_GROUP_GPD    0x0408
#define GPIO_CNL_LP_GROUP_VGPIO  0x0409
#define GPIO_CNL_LP_GROUP_SPI    0x040A
#define GPIO_CNL_LP_GROUP_AZA    0x040B
#define GPIO_CNL_LP_GROUP_JTAG   0x040D

///
/// CNL LP GPIO pins
/// Use below for functions from PCH GPIO Lib which
/// require GpioPad as argument. Encoding used here
/// has all information required by library functions
///
#define GPIO_CNL_LP_GPP_A1               0x04000001
#define GPIO_CNL_LP_GPP_A17              0x04000011


#define GPIO_CNL_LP_GPP_C1               0x04020001
#define GPIO_CNL_LP_GPP_C2               0x04020002
#define GPIO_CNL_LP_GPP_C8               0x04020008
#define GPIO_CNL_LP_GPP_C9               0x04020009
#define GPIO_CNL_LP_GPP_C12              0x0402000C
#define GPIO_CNL_LP_GPP_C13              0x0402000D
#define GPIO_CNL_LP_GPP_C20              0x04020014
#define GPIO_CNL_LP_GPP_C21              0x04020015


#define GPIO_CNL_LP_GPP_F1               0x04050001
#define GPIO_CNL_LP_GPP_F2               0x04050002
#define GPIO_CNL_LP_GPP_F11              0x0405000B
#define GPIO_CNL_LP_GPP_F12              0x0405000C
#define GPIO_CNL_LP_GPP_F13              0x0405000D
#define GPIO_CNL_LP_GPP_F14              0x0405000E
#define GPIO_CNL_LP_GPP_F15              0x0405000F
#define GPIO_CNL_LP_GPP_F16              0x04050010
#define GPIO_CNL_LP_GPP_F17              0x04050011
#define GPIO_CNL_LP_GPP_F18              0x04050012
#define GPIO_CNL_LP_GPP_F19              0x04050013
#define GPIO_CNL_LP_GPP_F20              0x04050014
#define GPIO_CNL_LP_GPP_F21              0x04050015
#define GPIO_CNL_LP_GPP_F22              0x04050016


#define GPIO_CNL_LP_GPP_H2               0x04070002
#define GPIO_CNL_LP_GPP_H23              0x04070017


#define GPIO_CNL_LP_VGPIO3               0x04090003
#define GPIO_CNL_LP_VGPIO39              0x04090027


#endif
