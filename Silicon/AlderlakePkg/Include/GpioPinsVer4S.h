/** @file
  GPIO pins for ADL-PCH-S,

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _GPIO_PINS_VER4_S_H_
#define _GPIO_PINS_VER4_S_H_

///
/// Unique ID used in GpioPad defines
///
#define GPIO_VER4_S_CHIPSET_ID     0x8

///
/// ADL S GPIO Groups
/// Use below for functions from PCH GPIO Lib which
/// require GpioGroup as argument
///
#define GPIO_VER4_S_GROUP_GPP_I     0x0800
#define GPIO_VER4_S_GROUP_GPP_R     0x0801
#define GPIO_VER4_S_GROUP_GPP_J     0x0802
#define GPIO_VER4_S_GROUP_VGPIO     0x0803
#define GPIO_VER4_S_GROUP_GPP_B     0x0805
#define GPIO_VER4_S_GROUP_GPP_G     0x0806
#define GPIO_VER4_S_GROUP_GPP_H     0x0807
#define GPIO_VER4_S_GROUP_GPD       0x0808
#define GPIO_VER4_S_GROUP_GPP_A     0x080A
#define GPIO_VER4_S_GROUP_GPP_C     0x080B
#define GPIO_VER4_S_GROUP_GPP_S     0x080D
#define GPIO_VER4_S_GROUP_GPP_E     0x080E
#define GPIO_VER4_S_GROUP_GPP_K     0x080F
#define GPIO_VER4_S_GROUP_GPP_F     0x0810
#define GPIO_VER4_S_GROUP_GPP_D     0x0811

///
/// ADL S GPIO pins
/// Use below for functions from PCH GPIO Lib which
/// require GpioPad as argument. Encoding used here
/// has all information required by library functions
///
#define GPIO_VER4_S_GPP_I0                   0x08000000

#define GPIO_VER4_S_GPP_R1                   0x08010001
#define GPIO_VER4_S_GPP_R2                   0x08010002
#define GPIO_VER4_S_GPP_R10                  0x0801000A
#define GPIO_VER4_S_GPP_R11                  0x0801000B
#define GPIO_VER4_S_GPP_R20                  0x08010014
#define GPIO_VER4_S_GPP_R21                  0x08010015

#define GPIO_VER4_S_GPP_J8                   0x08020008
#define GPIO_VER4_S_GPP_J9                   0x08020009


#define GPIO_VER4_S_GPP_B1                   0x08050001
#define GPIO_VER4_S_GPP_B2                   0x08050002
#define GPIO_VER4_S_GPP_B3                   0x08050003
#define GPIO_VER4_S_GPP_B4                   0x08050004
#define GPIO_VER4_S_GPP_B6                   0x08050006
#define GPIO_VER4_S_GPP_B14                  0x0805000E
#define GPIO_VER4_S_GPP_B19                  0x08050013
#define GPIO_VER4_S_GPP_B20                  0x08050014
#define GPIO_VER4_S_GPP_B21                  0x08050015
#define GPIO_VER4_S_GPP_B22                  0x08050016
#define GPIO_VER4_S_GPP_B23                  0x08050017

#define GPIO_VER4_S_GPP_G0                   0x08060000
#define GPIO_VER4_S_GPP_G1                   0x08060001
#define GPIO_VER4_S_GPP_G3                   0x08060003
#define GPIO_VER4_S_GPP_G4                   0x08060004
#define GPIO_VER4_S_GPP_G5                   0x08060005
#define GPIO_VER4_S_GPP_G6                   0x08060006
#define GPIO_VER4_S_GPP_G7                   0x08060007

#define GPIO_VER4_S_GPP_H0                   0x08070000
#define GPIO_VER4_S_GPP_H1                   0x08070001
#define GPIO_VER4_S_GPP_H2                   0x08070002
#define GPIO_VER4_S_GPP_H10                  0x0807000A
#define GPIO_VER4_S_GPP_H11                  0x0807000B
#define GPIO_VER4_S_GPP_H12                  0x0807000C
#define GPIO_VER4_S_GPP_H13                  0x0807000D
#define GPIO_VER4_S_GPP_H14                  0x0807000E
#define GPIO_VER4_S_GPP_H15                  0x0807000F
#define GPIO_VER4_S_GPP_H16                  0x08070010
#define GPIO_VER4_S_GPP_H17                  0x08070011
#define GPIO_VER4_S_GPP_H18                  0x08070012
#define GPIO_VER4_S_GPP_H23                  0x08070017

#define GPIO_VER4_S_GPD0                     0x08080000
#define GPIO_VER4_S_GPD1                     0x08080001
#define GPIO_VER4_S_GPD2                     0x08080002
#define GPIO_VER4_S_GPD7                     0x08080007
#define GPIO_VER4_S_GPD12                    0x0808000C


#define GPIO_VER4_S_GPP_A1                   0x080A0001
#define GPIO_VER4_S_GPP_A14                  0x080A000E

#define GPIO_VER4_S_GPP_C1                   0x080B0001
#define GPIO_VER4_S_GPP_C2                   0x080B0002
#define GPIO_VER4_S_GPP_C5                   0x080B0005
#define GPIO_VER4_S_GPP_C8                   0x080B0008
#define GPIO_VER4_S_GPP_C9                   0x080B0009
#define GPIO_VER4_S_GPP_C10                  0x080B000A
#define GPIO_VER4_S_GPP_C12                  0x080B000C
#define GPIO_VER4_S_GPP_C13                  0x080B000D
#define GPIO_VER4_S_GPP_C20                  0x080B0014
#define GPIO_VER4_S_GPP_C21                  0x080B0015


#define GPIO_VER4_S_GPP_S0                   0x080D0000
#define GPIO_VER4_S_GPP_S1                   0x080D0001
#define GPIO_VER4_S_GPP_S2                   0x080D0002
#define GPIO_VER4_S_GPP_S3                   0x080D0003
#define GPIO_VER4_S_GPP_S4                   0x080D0004
#define GPIO_VER4_S_GPP_S5                   0x080D0005
#define GPIO_VER4_S_GPP_S6                   0x080D0006
#define GPIO_VER4_S_GPP_S7                   0x080D0007

#define GPIO_VER4_S_GPP_E0                   0x080E0000
#define GPIO_VER4_S_GPP_E1                   0x080E0001
#define GPIO_VER4_S_GPP_E2                   0x080E0002
#define GPIO_VER4_S_GPP_E3                   0x080E0003
#define GPIO_VER4_S_GPP_E6                   0x080E0006
#define GPIO_VER4_S_GPP_E7                   0x080E0007
#define GPIO_VER4_S_GPP_E11                  0x080E000B
#define GPIO_VER4_S_GPP_E12                  0x080E000C
#define GPIO_VER4_S_GPP_E21                  0x080E0015

#define GPIO_VER4_S_GPP_K0                   0x080F0000
#define GPIO_VER4_S_GPP_K1                   0x080F0001
#define GPIO_VER4_S_GPP_K2                   0x080F0002
#define GPIO_VER4_S_GPP_K3                   0x080F0003
#define GPIO_VER4_S_GPP_K4                   0x080F0004
#define GPIO_VER4_S_GPP_K5                   0x080F0005
#define GPIO_VER4_S_GPP_K11                  0x080F000B

#define GPIO_VER4_S_GPP_F0                   0x08100000
#define GPIO_VER4_S_GPP_F1                   0x08100001
#define GPIO_VER4_S_GPP_F2                   0x08100002
#define GPIO_VER4_S_GPP_F3                   0x08100003
#define GPIO_VER4_S_GPP_F4                   0x08100004
#define GPIO_VER4_S_GPP_F5                   0x08100005
#define GPIO_VER4_S_GPP_F6                   0x08100006
#define GPIO_VER4_S_GPP_F7                   0x08100007
#define GPIO_VER4_S_GPP_F8                   0x08100008
#define GPIO_VER4_S_GPP_F9                   0x08100009
#define GPIO_VER4_S_GPP_F10                  0x0810000A
#define GPIO_VER4_S_GPP_F11                  0x0810000B
#define GPIO_VER4_S_GPP_F12                  0x0810000C
#define GPIO_VER4_S_GPP_F13                  0x0810000D
#define GPIO_VER4_S_GPP_F16                  0x08100010
#define GPIO_VER4_S_GPP_F17                  0x08100011
#define GPIO_VER4_S_GPP_F18                  0x08100012
#define GPIO_VER4_S_GPP_F22                  0x08100016
#define GPIO_VER4_S_GPP_F23                  0x08100017

#define GPIO_VER4_S_GPP_D1                   0x08110001
#define GPIO_VER4_S_GPP_D2                   0x08110002
#define GPIO_VER4_S_GPP_D3                   0x08110003
#define GPIO_VER4_S_GPP_D11                  0x0811000B
#define GPIO_VER4_S_GPP_D13                  0x0811000D
#define GPIO_VER4_S_GPP_D14                  0x0811000E
#define GPIO_VER4_S_GPP_D22                  0x08110016
#define GPIO_VER4_S_GPP_D23                  0x08110017


#endif // _GPIO_PINS_VER4_S_H_
