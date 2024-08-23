/**@file
  GPIO pins for ADL-PCH-LP,

  Copyright (c) 2022 - 2024, Intel Corporation. All rights reserved.<BR>
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
#define GPIO_VER2_LP_GPP_B0                  0x09000000
#define GPIO_VER2_LP_GPP_B1                  0x09000001
#define GPIO_VER2_LP_GPP_B2                  0x09000002
#define GPIO_VER2_LP_GPP_B3                  0x09000003
#define GPIO_VER2_LP_GPP_B4                  0x09000004
#define GPIO_VER2_LP_GPP_B5                  0x09000005
#define GPIO_VER2_LP_GPP_B6                  0x09000006
#define GPIO_VER2_LP_GPP_B7                  0x09000007
#define GPIO_VER2_LP_GPP_B8                  0x09000008
#define GPIO_VER2_LP_GPP_B11                 0x0900000B
#define GPIO_VER2_LP_GPP_B12                 0x0900000C
#define GPIO_VER2_LP_GPP_B13                 0x0900000D
#define GPIO_VER2_LP_GPP_B14                 0x0900000E
#define GPIO_VER2_LP_GPP_B15                 0x0900000F
#define GPIO_VER2_LP_GPP_B16                 0x09000010
#define GPIO_VER2_LP_GPP_B17                 0x09000011
#define GPIO_VER2_LP_GPP_B18                 0x09000012
#define GPIO_VER2_LP_GPP_B23                 0x09000017


#define GPIO_VER2_LP_GPP_A0                  0x09020000
#define GPIO_VER2_LP_GPP_A1                  0x09020001
#define GPIO_VER2_LP_GPP_A2                  0x09020002
#define GPIO_VER2_LP_GPP_A3                  0x09020003
#define GPIO_VER2_LP_GPP_A4                  0x09020004
#define GPIO_VER2_LP_GPP_A5                  0x09020005
#define GPIO_VER2_LP_GPP_A6                  0x09020006
#define GPIO_VER2_LP_GPP_A7                  0x09020007
#define GPIO_VER2_LP_GPP_A8                  0x09020008
#define GPIO_VER2_LP_GPP_A9                  0x09020009
#define GPIO_VER2_LP_GPP_A10                 0x0902000A
#define GPIO_VER2_LP_GPP_A11                 0x0902000B
#define GPIO_VER2_LP_GPP_A12                 0x0902000C
#define GPIO_VER2_LP_GPP_A13                 0x0902000D
#define GPIO_VER2_LP_GPP_A14                 0x0902000E
#define GPIO_VER2_LP_GPP_A15                 0x0902000F
#define GPIO_VER2_LP_GPP_A16                 0x09020010
#define GPIO_VER2_LP_GPP_A17                 0x09020011
#define GPIO_VER2_LP_GPP_A18                 0x09020012
#define GPIO_VER2_LP_GPP_A19                 0x09020013
#define GPIO_VER2_LP_GPP_A20                 0x09020014
#define GPIO_VER2_LP_GPP_A21                 0x09020015
#define GPIO_VER2_LP_GPP_A22                 0x09020016
#define GPIO_VER2_LP_GPP_A23                 0x09020017

#define GPIO_VER2_LP_GPP_R0                  0x09030000
#define GPIO_VER2_LP_GPP_R1                  0x09030001
#define GPIO_VER2_LP_GPP_R2                  0x09030002
#define GPIO_VER2_LP_GPP_R3                  0x09030003
#define GPIO_VER2_LP_GPP_R4                  0x09030004
#define GPIO_VER2_LP_GPP_R5                  0x09030005
#define GPIO_VER2_LP_GPP_R6                  0x09030006
#define GPIO_VER2_LP_GPP_R7                  0x09030007

#define GPIO_VER2_LP_GPD0                    0x09050000
#define GPIO_VER2_LP_GPD1                    0x09050001
#define GPIO_VER2_LP_GPD2                    0x09050002
#define GPIO_VER2_LP_GPD3                    0x09050003
#define GPIO_VER2_LP_GPD4                    0x09050004
#define GPIO_VER2_LP_GPD5                    0x09050005
#define GPIO_VER2_LP_GPD6                    0x09050006
#define GPIO_VER2_LP_GPD7                    0x09050007
#define GPIO_VER2_LP_GPD8                    0x09050008
#define GPIO_VER2_LP_GPD9                    0x09050009
#define GPIO_VER2_LP_GPD10                   0x0905000A
#define GPIO_VER2_LP_GPD11                   0x0905000B

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
#define GPIO_VER2_LP_GPP_H4                  0x09070004
#define GPIO_VER2_LP_GPP_H5                  0x09070005
#define GPIO_VER2_LP_GPP_H6                  0x09070006
#define GPIO_VER2_LP_GPP_H7                  0x09070007
#define GPIO_VER2_LP_GPP_H8                  0x09070008
#define GPIO_VER2_LP_GPP_H9                  0x09070009
#define GPIO_VER2_LP_GPP_H10                 0x0907000A
#define GPIO_VER2_LP_GPP_H11                 0x0907000B
#define GPIO_VER2_LP_GPP_H12                 0x0907000C
#define GPIO_VER2_LP_GPP_H13                 0x0907000D
#define GPIO_VER2_LP_GPP_H15                 0x0907000E
#define GPIO_VER2_LP_GPP_H17                 0x09070011
#define GPIO_VER2_LP_GPP_H18                 0x09070012
#define GPIO_VER2_LP_GPP_H19                 0x09070013
#define GPIO_VER2_LP_GPP_H20                 0x09070014
#define GPIO_VER2_LP_GPP_H21                 0x09070015
#define GPIO_VER2_LP_GPP_H22                 0x09070016
#define GPIO_VER2_LP_GPP_H23                 0x09070017

#define GPIO_VER2_LP_GPP_D0                  0x09080000
#define GPIO_VER2_LP_GPP_D1                  0x09080001
#define GPIO_VER2_LP_GPP_D2                  0x09080002
#define GPIO_VER2_LP_GPP_D3                  0x09080003
#define GPIO_VER2_LP_GPP_D4                  0x09080004
#define GPIO_VER2_LP_GPP_D5                  0x09080005
#define GPIO_VER2_LP_GPP_D6                  0x09080006
#define GPIO_VER2_LP_GPP_D7                  0x09080007
#define GPIO_VER2_LP_GPP_D8                  0x09080008
#define GPIO_VER2_LP_GPP_D9                  0x09080009
#define GPIO_VER2_LP_GPP_D10                 0x0908000A
#define GPIO_VER2_LP_GPP_D11                 0x0908000B
#define GPIO_VER2_LP_GPP_D12                 0x0908000C
#define GPIO_VER2_LP_GPP_D13                 0x0908000D
#define GPIO_VER2_LP_GPP_D14                 0x0908000E
#define GPIO_VER2_LP_GPP_D15                 0x0908000F
#define GPIO_VER2_LP_GPP_D16                 0x09080010
#define GPIO_VER2_LP_GPP_D17                 0x09080011
#define GPIO_VER2_LP_GPP_D18                 0x09080012
#define GPIO_VER2_LP_GPP_D19                 0x09080013


#define GPIO_VER2_LP_GPP_C0                  0x090B0000
#define GPIO_VER2_LP_GPP_C1                  0x090B0001
#define GPIO_VER2_LP_GPP_C2                  0x090B0002
#define GPIO_VER2_LP_GPP_C3                  0x090B0003
#define GPIO_VER2_LP_GPP_C4                  0x090B0004
#define GPIO_VER2_LP_GPP_C5                  0x090B0005
#define GPIO_VER2_LP_GPP_C6                  0x090B0006
#define GPIO_VER2_LP_GPP_C7                  0x090B0007


#define GPIO_VER2_LP_GPP_F0                  0x090C0000
#define GPIO_VER2_LP_GPP_F1                  0x090C0001
#define GPIO_VER2_LP_GPP_F2                  0x090C0002
#define GPIO_VER2_LP_GPP_F3                  0x090C0003
#define GPIO_VER2_LP_GPP_F4                  0x090C0004
#define GPIO_VER2_LP_GPP_F5                  0x090C0005
#define GPIO_VER2_LP_GPP_F6                  0x090C0006
#define GPIO_VER2_LP_GPP_F7                  0x090C0007
#define GPIO_VER2_LP_GPP_F8                  0x090C0008
#define GPIO_VER2_LP_GPP_F9                  0x090C0009
#define GPIO_VER2_LP_GPP_F10                 0x090C000A
#define GPIO_VER2_LP_GPP_F11                 0x090C000B
#define GPIO_VER2_LP_GPP_F12                 0x090C000C
#define GPIO_VER2_LP_GPP_F13                 0x090C000D
#define GPIO_VER2_LP_GPP_F14                 0x090C000E
#define GPIO_VER2_LP_GPP_F15                 0x090C000F
#define GPIO_VER2_LP_GPP_F16                 0x090C0010
#define GPIO_VER2_LP_GPP_F17                 0x090C0011
#define GPIO_VER2_LP_GPP_F18                 0x090C0012
#define GPIO_VER2_LP_GPP_F19                 0x090C0013
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
#define GPIO_VER2_LP_GPP_E6                  0x090E0006
#define GPIO_VER2_LP_GPP_E7                  0x090E0007
#define GPIO_VER2_LP_GPP_E8                  0x090E0008
#define GPIO_VER2_LP_GPP_E9                  0x090E0009
#define GPIO_VER2_LP_GPP_E10                 0x090E000A
#define GPIO_VER2_LP_GPP_E11                 0x090E000B
#define GPIO_VER2_LP_GPP_E12                 0x090E000C
#define GPIO_VER2_LP_GPP_E13                 0x090E000D
#define GPIO_VER2_LP_GPP_E14                 0x090E000E
#define GPIO_VER2_LP_GPP_E15                 0x090E000F
#define GPIO_VER2_LP_GPP_E16                 0x090E0010
#define GPIO_VER2_LP_GPP_E17                 0x090E0011
#define GPIO_VER2_LP_GPP_E18                 0x090E0012
#define GPIO_VER2_LP_GPP_E19                 0x090E0013
#define GPIO_VER2_LP_GPP_E20                 0x090E0014
#define GPIO_VER2_LP_GPP_E21                 0x090E0015
#define GPIO_VER2_LP_GPP_E22                 0x090E0016
#define GPIO_VER2_LP_GPP_E23                 0x090E0017

/*
  Following group is used only for PCH_N
  Other GPIO Group Defines are same keeping
  Macro in sync with earlier PCH in case there
  is usage outside IP Block
*/
#define GPIO_VER2_LP_GPP_I0                   0x09090000
#define GPIO_VER2_LP_GPP_I1                   0x09090001
#define GPIO_VER2_LP_GPP_I2                   0x09090002
#define GPIO_VER2_LP_GPP_I3                   0x09090003
#define GPIO_VER2_LP_GPP_I4                   0x09090004
#define GPIO_VER2_LP_GPP_I5                   0x09090005
#define GPIO_VER2_LP_GPP_I6                   0x09090006
#define GPIO_VER2_LP_GPP_I7                   0x09090007
#define GPIO_VER2_LP_GPP_I8                   0x09090008
#define GPIO_VER2_LP_GPP_I9                   0x09090009
#define GPIO_VER2_LP_GPP_I10                  0x0909000A
#define GPIO_VER2_LP_GPP_I11                  0x0909000B
#define GPIO_VER2_LP_GPP_I12                  0x0909000C
#define GPIO_VER2_LP_GPP_I13                  0x0909000D
#define GPIO_VER2_LP_GPP_I14                  0x0909000E
#define GPIO_VER2_LP_GPP_I15                  0x0909000F
#define GPIO_VER2_LP_GPP_I16                  0x09090010
#define GPIO_VER2_LP_GPP_I17                  0x09090011
#define GPIO_VER2_LP_GPP_I18                  0x09090012
#define GPIO_VER2_LP_GPP_I19                  0x09090013

#define GPIO_VER2_LP_MUXING_SERIALIO_UART0_RXD_GPP_F1        0x290C0201
#define GPIO_VER2_LP_MUXING_SERIALIO_UART0_TXD_GPP_F2        0x290C1202
#define GPIO_VER2_LP_MUXING_SERIALIO_UART0_RTS_GPP_F0        0x290C2200
#define GPIO_VER2_LP_MUXING_SERIALIO_UART0_CTS_GPP_F3        0x290C3203

#define GPIO_VER2_P_MUXING_ISH_UART1_RXD_GPP_D17             0x29282211
#define GPIO_VER2_P_MUXING_ISH_UART1_TXD_GPP_D18             0x29283212

#define GPIO_VER2_P_MUXING_SERIALIO_I2C0_SDA_GPP_H4          0x1947c404
#define GPIO_VER2_P_MUXING_SERIALIO_I2C0_SDA_GPP_E12         0x594ec40c
#define GPIO_VER2_P_MUXING_SERIALIO_I2C1_SDA_GPP_H6          0x1947c606
#define GPIO_VER2_P_MUXING_SERIALIO_I2C1_SDA_GPP_F13         0x594cc60d
#define GPIO_VER2_P_MUXING_SERIALIO_I2C7_SDA_GPP_H12         0x1947d20c
#define GPIO_VER2_P_MUXING_SERIALIO_I2C7_SDA_GPP_D15         0x4948d20f

#define GPIO_VER2_P_MUXING_SERIALIO_I2C0_SCL_GPP_H5          0x1947a405
#define GPIO_VER2_P_MUXING_SERIALIO_I2C0_SCL_GPP_E13         0x594ea40d
#define GPIO_VER2_P_MUXING_SERIALIO_I2C1_SCL_GPP_H7          0x1947a607
#define GPIO_VER2_P_MUXING_SERIALIO_I2C1_SCL_GPP_F12         0x594ca60c
#define GPIO_VER2_P_MUXING_SERIALIO_I2C7_SCL_GPP_H13         0x1947b20d
#define GPIO_VER2_P_MUXING_SERIALIO_I2C7_SCL_GPP_D16         0x4948b210

#define GPIO_VER2_LP_MUXING_DMIC0_CLKA_GPP_S6                0x29460C06
#define GPIO_VER2_LP_MUXING_DMIC0_CLKA_GPP_A7                0x59420C07
#define GPIO_VER2_LP_MUXING_DMIC0_CLKB_GPP_S2                0x29461402
#define GPIO_VER2_LP_MUXING_DMIC0_CLKB_GPP_A13               0x5942140D
#define GPIO_VER2_LP_MUXING_DMIC0_DATA_GPP_S7                0x29460407
#define GPIO_VER2_LP_MUXING_DMIC0_DATA_GPP_A8                0x59420408
#define GPIO_VER2_LP_MUXING_DMIC1_CLKA_GPP_S4                0x29460E04
#define GPIO_VER2_LP_MUXING_DMIC1_CLKA_GPP_A9                0x59420E09
#define GPIO_VER2_LP_MUXING_DMIC1_CLKB_GPP_S3                0x29461603
#define GPIO_VER2_LP_MUXING_DMIC1_CLKB_GPP_A14               0x5942160E
#define GPIO_VER2_LP_MUXING_DMIC1_DATA_GPP_S5                0x29460605
#define GPIO_VER2_LP_MUXING_DMIC1_DATA_GPP_A10               0x5942060A
#define GPIO_VER2_LP_MUXING_DMIC0_CLKA_GPP_S2                0x29460c02
#define GPIO_VER2_LP_MUXING_DMIC1_CLKA_GPP_S6                0x29460e06
#define GPIO_VER2_LP_MUXING_DMIC0_CLKB_GPP_S4                0x29461404
#define GPIO_VER2_LP_MUXING_DMIC1_CLKB_GPP_S5                0x29461605
#define GPIO_VER2_LP_MUXING_DMIC0_DATA_GPP_S3                0x29460403
#define GPIO_VER2_LP_MUXING_DMIC1_DATA_GPP_S7                0x29460607
#define GPIO_VER2_LP_MUXING_DMIC0_CLKA_GPP_R4                0x39430c04
#define GPIO_VER2_LP_MUXING_DMIC1_CLKA_GPP_R6                0x39430e06
#define GPIO_VER2_LP_MUXING_DMIC0_CLKB_GPP_R0                0x39431400
#define GPIO_VER2_LP_MUXING_DMIC1_CLKB_GPP_R1                0x39431601
#define GPIO_VER2_LP_MUXING_DMIC0_DATA_GPP_R5                0x39430405
#define GPIO_VER2_LP_MUXING_DMIC1_DATA_GPP_R7                0x39430607

#define GPIO_VER2_LP_MUXING_CNVI_RF_RESET_GPP_A8             0x2942E408
#define GPIO_VER2_LP_MUXING_CNVI_RF_RESET_GPP_F4             0x194CE404

#define GPIO_VER2_LP_MUXING_CNVI_MODEM_CLKREQ_GPP_A9         0x2942E609
#define GPIO_VER2_LP_MUXING_CNVI_CRF_XTAL_CLKREQ_GPP_A9      0x3942E609
#define GPIO_VER2_LP_MUXING_CNVI_MODEM_CLKREQ_GPP_F5         0x294CE605
#define GPIO_VER2_LP_MUXING_CNVI_CRF_XTAL_CLKREQ_GPP_F5      0x394CE605

#define GPIO_VER2_P_MUXING_SATA_DEVSLP0_GPP_E4               0x196e3e04
#define GPIO_VER2_P_MUXING_SATA_DEVSLP1_GPP_E5               0x196e4005
#define GPIO_VER2_P_MUXING_SATA_DEVSLP0_GPP_H12              0x59673e0c
#define GPIO_VER2_P_MUXING_SATA_DEVSLP1_GPP_H13              0x5967400d

#define GPIO_VER2_P_MUXING_PCIE_CLKREQ_9_E0                  0x796e9000
#define GPIO_VER2_P_MUXING_PCIE_CLKREQ_9_A12                 0x4962900c

#define GPIO_VER2_P_MUXING_ISH_GP_4_E9                       0x290ea809
#define GPIO_VER2_P_MUXING_ISH_GP_4_B3                       0x4900a803
#define GPIO_VER2_P_MUXING_ISH_GP_5_A16                      0x4902aa10
#define GPIO_VER2_P_MUXING_ISH_GP_5_B4                       0x4900aa04
#define GPIO_VER2_P_MUXING_ISH_GP_6_B14                      0x5900ac0e
#define GPIO_VER2_P_MUXING_ISH_GP_6_H12                      0x4907ac0c
#define GPIO_VER2_P_MUXING_ISH_GP_7_B15                      0x5900ae0f
#define GPIO_VER2_P_MUXING_ISH_GP_7_H13                      0x4907ae0d

#endif // _GPIO_PINS_VER2_LP_H_
