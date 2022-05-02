/**@file
  GPIO pins for ADL-PCH-LP,

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
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
/*
  Following group is used only for PCH_N
  Other GPIO Group Defines are same keeping
  Macro in sync with earlier PCH in case there
  is usage outside IP Block
*/
#define GPIO_VER2_LP_GROUP_GPP_I     0x0909

///
/// TGL LP GPIO pins
/// Use below for functions from PCH GPIO Lib which
/// require GpioPad as argument. Encoding used here
/// has all information required by library functions
///
#define GPIO_VER2_LP_GPP_B1                  0x09000001
#define GPIO_VER2_LP_GPP_B2                  0x09000002
#define GPIO_VER2_LP_GPP_B14                 0x0900000E
#define GPIO_VER2_LP_GPP_B18                 0x09000012
#define GPIO_VER2_LP_GPP_B23                 0x09000017

#define GPIO_VER2_LP_GPP_T0                  0x09010000
#define GPIO_VER2_LP_GPP_T1                  0x09010001
#define GPIO_VER2_LP_GPP_T2                  0x09010002
#define GPIO_VER2_LP_GPP_T3                  0x09010003
#define GPIO_VER2_LP_GPP_T4                  0x09010004
#define GPIO_VER2_LP_GPP_T5                  0x09010005
#define GPIO_VER2_LP_GPP_T6                  0x09010006
#define GPIO_VER2_LP_GPP_T7                  0x09010007
#define GPIO_VER2_LP_GPP_T8                  0x09010008
#define GPIO_VER2_LP_GPP_T9                  0x09010009
#define GPIO_VER2_LP_GPP_T10                 0x0901000A
#define GPIO_VER2_LP_GPP_T11                 0x0901000B
#define GPIO_VER2_LP_GPP_T12                 0x0901000C
#define GPIO_VER2_LP_GPP_T13                 0x0901000D
#define GPIO_VER2_LP_GPP_T14                 0x0901000E
#define GPIO_VER2_LP_GPP_T15                 0x0901000F

#define GPIO_VER2_LP_GPP_A1                  0x09020001
#define GPIO_VER2_LP_GPP_A2                  0x09020002
#define GPIO_VER2_LP_GPP_A6                  0x09020006
#define GPIO_VER2_LP_GPP_A7                  0x09020007
#define GPIO_VER2_LP_GPP_A8                  0x09020008
#define GPIO_VER2_LP_GPP_A11                 0x0902000B
#define GPIO_VER2_LP_GPP_A13                 0x0902000D
#define GPIO_VER2_LP_GPP_A14                 0x0902000E
#define GPIO_VER2_LP_GPP_A15                 0x0902000F
#define GPIO_VER2_LP_GPP_A17                 0x09020011
#define GPIO_VER2_LP_GPP_A19                 0x09020013
#define GPIO_VER2_LP_GPP_A20                 0x09020014
#define GPIO_VER2_LP_GPP_A21                 0x09020015
#define GPIO_VER2_LP_GPP_A22                 0x09020016

#define GPIO_VER2_LP_GPP_R0                  0x09030000
#define GPIO_VER2_LP_GPP_R1                  0x09030001
#define GPIO_VER2_LP_GPP_R2                  0x09030002
#define GPIO_VER2_LP_GPP_R3                  0x09030003
#define GPIO_VER2_LP_GPP_R4                  0x09030004
#define GPIO_VER2_LP_GPP_R5                  0x09030005
#define GPIO_VER2_LP_GPP_R6                  0x09030006
#define GPIO_VER2_LP_GPP_R7                  0x09030007

#define GPIO_VER2_LP_GPD2                    0x09050002
#define GPIO_VER2_LP_GPD7                    0x09050007

#define GPIO_VER2_LP_GPP_S0                  0x09060000
#define GPIO_VER2_LP_GPP_S1                  0x09060001
#define GPIO_VER2_LP_GPP_S2                  0x09060002
#define GPIO_VER2_LP_GPP_S3                  0x09060003
#define GPIO_VER2_LP_GPP_S4                  0x09060004
#define GPIO_VER2_LP_GPP_S5                  0x09060005
#define GPIO_VER2_LP_GPP_S6                  0x09060006
#define GPIO_VER2_LP_GPP_S7                  0x09060007

#define GPIO_VER2_LP_GPP_H0                  0x09070000
#define GPIO_VER2_LP_GPP_H1                  0x09070001
#define GPIO_VER2_LP_GPP_H2                  0x09070002
#define GPIO_VER2_LP_GPP_H3                  0x09070003
#define GPIO_VER2_LP_GPP_H9                  0x09070009
#define GPIO_VER2_LP_GPP_H10                 0x0907000A
#define GPIO_VER2_LP_GPP_H11                 0x0907000B
#define GPIO_VER2_LP_GPP_H17                 0x09070011
#define GPIO_VER2_LP_GPP_H19                 0x09070013
#define GPIO_VER2_LP_GPP_H23                 0x09070017

#define GPIO_VER2_LP_GPP_D1                  0x09080001
#define GPIO_VER2_LP_GPP_D11                 0x0908000B
#define GPIO_VER2_LP_GPP_D13                 0x0908000D
#define GPIO_VER2_LP_GPP_D14                 0x0908000E
#define GPIO_VER2_LP_GPP_D15                 0x0908000F
#define GPIO_VER2_LP_GPP_D16                 0x09080010
#define GPIO_VER2_LP_GPP_D17                 0x09080011
#define GPIO_VER2_LP_GPP_D18                 0x09080012


#define GPIO_VER2_LP_GPP_C2                  0x090B0002
#define GPIO_VER2_LP_GPP_C5                  0x090B0005


#define GPIO_VER2_LP_GPP_F1                  0x090C0001
#define GPIO_VER2_LP_GPP_F2                  0x090C0002
#define GPIO_VER2_LP_GPP_F4                  0x090C0004
#define GPIO_VER2_LP_GPP_F5                  0x090C0005
#define GPIO_VER2_LP_GPP_F6                  0x090C0006
#define GPIO_VER2_LP_GPP_F7                  0x090C0007
#define GPIO_VER2_LP_GPP_F9                  0x090C0009
#define GPIO_VER2_LP_GPP_F10                 0x090C000A
#define GPIO_VER2_LP_GPP_F11                 0x090C000B
#define GPIO_VER2_LP_GPP_F13                 0x090C000D
#define GPIO_VER2_LP_GPP_F14                 0x090C000E
#define GPIO_VER2_LP_GPP_F15                 0x090C000F
#define GPIO_VER2_LP_GPP_F16                 0x090C0010
#define GPIO_VER2_LP_GPP_F17                 0x090C0011
#define GPIO_VER2_LP_GPP_F18                 0x090C0012
#define GPIO_VER2_LP_GPP_F20                 0x090C0014
#define GPIO_VER2_LP_GPP_F21                 0x090C0015
#define GPIO_VER2_LP_GPP_F22                 0x090C0016
#define GPIO_VER2_LP_GPP_F23                 0x090C0017


#define GPIO_VER2_LP_GPP_E0                  0x090E0000
#define GPIO_VER2_LP_GPP_E1                  0x090E0001
#define GPIO_VER2_LP_GPP_E2                  0x090E0002
#define GPIO_VER2_LP_GPP_E3                  0x090E0003
#define GPIO_VER2_LP_GPP_E4                  0x090E0004
#define GPIO_VER2_LP_GPP_E5                  0x090E0005
#define GPIO_VER2_LP_GPP_E7                  0x090E0007
#define GPIO_VER2_LP_GPP_E15                 0x090E000F
#define GPIO_VER2_LP_GPP_E16                 0x090E0010
#define GPIO_VER2_LP_GPP_E20                 0x090E0014
#define GPIO_VER2_LP_GPP_E21                 0x090E0015
#define GPIO_VER2_LP_GPP_E22                 0x090E0016
#define GPIO_VER2_LP_GPP_E23                 0x090E0017


#define GPIO_VER2_LP_MUXING_DMIC0_CLKA_GPP_S2                0x29460c02
#define GPIO_VER2_LP_MUXING_DMIC1_CLKA_GPP_S6                0x29460e06
#define GPIO_VER2_LP_MUXING_DMIC0_CLKB_GPP_S4                0x29461404
#define GPIO_VER2_LP_MUXING_DMIC1_CLKB_GPP_S5                0x29461605
#define GPIO_VER2_LP_MUXING_DMIC0_DATA_GPP_S3                0x29460403
#define GPIO_VER2_LP_MUXING_DMIC1_DATA_GPP_S7                0x29460607

#define GPIO_VER2_LP_GPD11                   0x0905000B

#endif // _GPIO_PINS_VER2_LP_H_
