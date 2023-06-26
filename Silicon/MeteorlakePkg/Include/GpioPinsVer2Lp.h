/** @file
  GPIO pins for TGL-PCH-LP,

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _GPIO_PINS_VER2_LP_H_
#define _GPIO_PINS_VER2_LP_H_

///
/// Unique ID used in GpioPad defines
///
#define GPIO_VER2_LP_CHIPSET_ID     0x9

///
/// TGL LP GPIO Groups
/// Use below for functions from PCH GPIO Lib which
/// require GpioGroup as argument
///
#define GPIO_VER2_LP_GROUP_GPP_B    0x0900
#define GPIO_VER2_LP_GROUP_GPP_T    0x0901
#define GPIO_VER2_LP_GROUP_GPP_A    0x0902
#define GPIO_VER2_LP_GROUP_GPP_R    0x0903
#define GPIO_VER2_LP_GROUP_GPD      0x0905
#define GPIO_VER2_LP_GROUP_GPP_S    0x0906
#define GPIO_VER2_LP_GROUP_GPP_H    0x0907
#define GPIO_VER2_LP_GROUP_GPP_D    0x0908
#define GPIO_VER2_LP_GROUP_GPP_U    0x0909
#define GPIO_VER2_LP_GROUP_VGPIO    0x090A
#define GPIO_VER2_LP_GROUP_GPP_C    0x090B
#define GPIO_VER2_LP_GROUP_GPP_F    0x090C
#define GPIO_VER2_LP_GROUP_GPP_E    0x090E


#endif // _GPIO_PINS_VER2_LP_H_
