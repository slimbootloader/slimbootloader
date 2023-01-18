/** @file
  GPIO pins for EHL-PCH,

  Copyright (c) 2019 - 2023 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _GPIO_PINS_VER3_H_
#define _GPIO_PINS_VER3_H_

///
/// Unique ID used in GpioPad defines
///
#define GPIO_VER3_CHIPSET_ID     0xB

///
/// EHL GPIO Groups
/// Use below for functions from PCH GPIO Lib which
/// require GpioGroup as argument
///
#define GPIO_VER3_GROUP_GPP_B    0x0B00
#define GPIO_VER3_GROUP_GPP_T    0x0B01
#define GPIO_VER3_GROUP_GPP_G    0x0B02
#define GPIO_VER3_GROUP_GPP_V    0x0B03
#define GPIO_VER3_GROUP_GPP_H    0x0B04
#define GPIO_VER3_GROUP_GPP_D    0x0B05
#define GPIO_VER3_GROUP_GPP_U    0x0B06
#define GPIO_VER3_GROUP_VGPIO    0x0B07
#define GPIO_VER3_GROUP_GPD      0x0B08
#define GPIO_VER3_GROUP_GPP_S    0x0B0A
#define GPIO_VER3_GROUP_GPP_A    0x0B0B
#define GPIO_VER3_GROUP_GPP_C    0x0B0D
#define GPIO_VER3_GROUP_GPP_F    0x0B0E
#define GPIO_VER3_GROUP_GPP_E    0x0B10
#define GPIO_VER3_GROUP_GPP_R    0x0B12

///
/// EHL GPIO pins
/// Use below for functions from PCH GPIO Lib which
/// require GpioPad as argument. Encoding used here
/// has all information required by library functions
///
#define GPIO_VER3_GPP_A0                  0x0B0B0000
#define GPIO_VER3_GPP_A1                  0x0B0B0001
#define GPIO_VER3_GPP_A2                  0x0B0B0002
#define GPIO_VER3_GPP_A3                  0x0B0B0003
#define GPIO_VER3_GPP_A4                  0x0B0B0004
#define GPIO_VER3_GPP_A5                  0x0B0B0005
#define GPIO_VER3_GPP_A6                  0x0B0B0006
#define GPIO_VER3_GPP_A7                  0x0B0B0007
#define GPIO_VER3_GPP_A8                  0x0B0B0008
#define GPIO_VER3_GPP_A9                  0x0B0B0009
#define GPIO_VER3_GPP_A10                 0x0B0B000A
#define GPIO_VER3_GPP_A11                 0x0B0B000B
#define GPIO_VER3_GPP_A12                 0x0B0B000C
#define GPIO_VER3_GPP_A13                 0x0B0B000D
#define GPIO_VER3_GPP_A14                 0x0B0B000E
#define GPIO_VER3_GPP_A15                 0x0B0B000F
#define GPIO_VER3_GPP_A16                 0x0B0B0010
#define GPIO_VER3_GPP_A17                 0x0B0B0011
#define GPIO_VER3_GPP_A18                 0x0B0B0012
#define GPIO_VER3_GPP_A19                 0x0B0B0013
#define GPIO_VER3_GPP_A20                 0x0B0B0014
#define GPIO_VER3_GPP_A21                 0x0B0B0015
#define GPIO_VER3_GPP_A22                 0x0B0B0016
#define GPIO_VER3_GPP_A23                 0x0B0B0017

#define GPIO_VER3_GPP_B0                  0x0B000000
#define GPIO_VER3_GPP_B1                  0x0B000001
#define GPIO_VER3_GPP_B2                  0x0B000002
#define GPIO_VER3_GPP_B3                  0x0B000003
#define GPIO_VER3_GPP_B4                  0x0B000004
#define GPIO_VER3_GPP_B5                  0x0B000005
#define GPIO_VER3_GPP_B6                  0x0B000006
#define GPIO_VER3_GPP_B7                  0x0B000007
#define GPIO_VER3_GPP_B8                  0x0B000008
#define GPIO_VER3_GPP_B9                  0x0B000009
#define GPIO_VER3_GPP_B10                 0x0B00000A
#define GPIO_VER3_GPP_B11                 0x0B00000B
#define GPIO_VER3_GPP_B12                 0x0B00000C
#define GPIO_VER3_GPP_B13                 0x0B00000D
#define GPIO_VER3_GPP_B14                 0x0B00000E
#define GPIO_VER3_GPP_B15                 0x0B00000F
#define GPIO_VER3_GPP_B16                 0x0B000010
#define GPIO_VER3_GPP_B17                 0x0B000011
#define GPIO_VER3_GPP_B18                 0x0B000012
#define GPIO_VER3_GPP_B19                 0x0B000013
#define GPIO_VER3_GPP_B20                 0x0B000014
#define GPIO_VER3_GPP_B21                 0x0B000015
#define GPIO_VER3_GPP_B22                 0x0B000016
#define GPIO_VER3_GPP_B23                 0x0B000017

#define GPIO_VER3_GPP_T4                  0x0B010004
#define GPIO_VER3_GPP_T5                  0x0B010005
#define GPIO_VER3_GPP_T6                  0x0B010006
#define GPIO_VER3_GPP_T7                  0x0B010007

#define GPIO_VER3_GPP_C0                  0x0B0D0000
#define GPIO_VER3_GPP_C2                  0x0B0D0002
#define GPIO_VER3_GPP_C3                  0x0B0D0003
#define GPIO_VER3_GPP_C4                  0x0B0D0004
#define GPIO_VER3_GPP_C5                  0x0B0D0005
#define GPIO_VER3_GPP_C6                  0x0B0D0006
#define GPIO_VER3_GPP_C7                  0x0B0D0007
#define GPIO_VER3_GPP_C8                  0x0B0D0008
#define GPIO_VER3_GPP_C9                  0x0B0D0009
#define GPIO_VER3_GPP_C10                 0x0B0D000A
#define GPIO_VER3_GPP_C11                 0x0B0D000B
#define GPIO_VER3_GPP_C12                 0x0B0D000C
#define GPIO_VER3_GPP_C13                 0x0B0D000D
#define GPIO_VER3_GPP_C14                 0x0B0D000E
#define GPIO_VER3_GPP_C15                 0x0B0D000F
#define GPIO_VER3_GPP_C16                 0x0B0D0010
#define GPIO_VER3_GPP_C17                 0x0B0D0011
#define GPIO_VER3_GPP_C18                 0x0B0D0012
#define GPIO_VER3_GPP_C19                 0x0B0D0013
#define GPIO_VER3_GPP_C20                 0x0B0D0014
#define GPIO_VER3_GPP_C21                 0x0B0D0015
#define GPIO_VER3_GPP_C22                 0x0B0D0016
#define GPIO_VER3_GPP_C23                 0x0B0D0017

#define GPIO_VER3_GPP_G1                  0x0B020001
#define GPIO_VER3_GPP_G12                 0x0B02000C
#define GPIO_VER3_GPP_G19                 0x0B020013

#define GPIO_VER3_GPP_V1                  0x0B030001
#define GPIO_VER3_GPP_V13                 0x0B03000D
#define GPIO_VER3_GPP_V14                 0x0B03000E
#define GPIO_VER3_GPP_V15                 0x0B03000F

#define GPIO_VER3_GPP_D1                  0x0B050001
#define GPIO_VER3_GPP_D13                 0x0B05000D
#define GPIO_VER3_GPP_D17                 0x0B050011
#define GPIO_VER3_GPP_D18                 0x0B050012

#define GPIO_VER3_GPP_H0                  0x0B040000
#define GPIO_VER3_GPP_H1                  0x0B040001
#define GPIO_VER3_GPP_H2                  0x0B040002
#define GPIO_VER3_GPP_H3                  0x0B040003

#define GPIO_VER3_GPP_U0                  0x0B060000
#define GPIO_VER3_GPP_U1                  0x0B060001
#define GPIO_VER3_GPP_U2                  0x0B060002
#define GPIO_VER3_GPP_U3                  0x0B060003
#define GPIO_VER3_GPP_U4                  0x0B060004
#define GPIO_VER3_GPP_U5                  0x0B060005
#define GPIO_VER3_GPP_U6                  0x0B060006
#define GPIO_VER3_GPP_U7                  0x0B060007
#define GPIO_VER3_GPP_U8                  0x0B060008
#define GPIO_VER3_GPP_U9                  0x0B060009
#define GPIO_VER3_GPP_U10                 0x0B06000A
#define GPIO_VER3_GPP_U11                 0x0B06000B
#define GPIO_VER3_GPP_U12                 0x0B06000C
#define GPIO_VER3_GPP_U13                 0x0B06000D
#define GPIO_VER3_GPP_U14                 0x0B06000E
#define GPIO_VER3_GPP_U15                 0x0B06000F
#define GPIO_VER3_GPP_U16                 0x0B060010
#define GPIO_VER3_GPP_U17                 0x0B060011
#define GPIO_VER3_GPP_U18                 0x0B060012
#define GPIO_VER3_GPP_U19                 0x0B06001

#define GPIO_VER3_VGPIO0                  0x0B070000
#define GPIO_VER3_VGPIO39                 0x0B07001B

#define GPIO_VER3_GPD1                    0x0B080001
#define GPIO_VER3_GPD2                    0x0B080002
#define GPIO_VER3_GPD7                    0x0B080007
#define GPIO_VER3_GPD11                   0x0B08000B

#define GPIO_VER3_UFS_RESETB              0x0B09000B


#define GPIO_VER3_GPP_C1                  0x0B0D0001
#define GPIO_VER3_GPP_C2                  0x0B0D0002
#define GPIO_VER3_GPP_C8                  0x0B0D0008
#define GPIO_VER3_GPP_C12                 0x0B0D000C
#define GPIO_VER3_GPP_C13                 0x0B0D000D
#define GPIO_VER3_GPP_C20                 0x0B0D0014
#define GPIO_VER3_GPP_C21                 0x0B0D0015
#define GPIO_VER3_GPP_C22                 0x0B0D0016
#define GPIO_VER3_GPP_C23                 0x0B0D0017

#define GPIO_VER3_GPP_F1                  0x0B0E0001
#define GPIO_VER3_GPP_F2                  0x0B0E0002
#define GPIO_VER3_GPP_F4                  0x0B0E0004
#define GPIO_VER3_GPP_F5                  0x0B0E0005
#define GPIO_VER3_GPP_F17                 0x0B0E0011
#define GPIO_VER3_GPP_F20                 0x0B0E0014
#define GPIO_VER3_GPP_F21                 0x0B0E0015


#define GPIO_VER3_GPP_E0                  0x0B100000
#define GPIO_VER3_GPP_E1                  0x0B100001
#define GPIO_VER3_GPP_E2                  0x0B100002
#define GPIO_VER3_GPP_E6                  0x0B100006
#define GPIO_VER3_GPP_E8                  0x0B100008
#define GPIO_VER3_GPP_E10                 0x0B10000A
#define GPIO_VER3_GPP_E11                 0x0B10000B
#define GPIO_VER3_GPP_E12                 0x0B10000C
#define GPIO_VER3_GPP_E13                 0x0B10000D
#define GPIO_VER3_GPP_E17                 0x0B100011


#define GPIO_VER3_MUXING_SERIALIO_UART0_RXD_GPP_F1        0x2B0E0201
#define GPIO_VER3_MUXING_SERIALIO_UART0_TXD_GPP_F2        0x2B0E1202
#define GPIO_VER3_MUXING_SERIALIO_UART0_RTS_GPP_F0        0x2B0E2200
#define GPIO_VER3_MUXING_SERIALIO_UART0_CTS_GPP_F3        0x2B0E3203

#define GPIO_VER3_MUXING_SERIALIO_I2C4_SDA_GPP_H8         0x1B44CC08
#define GPIO_VER3_MUXING_SERIALIO_I2C4_SCL_GPP_H9         0x1B44AC09

#define GPIO_VER3_MUXING_DMIC0_CLKA_GPP_R6                0x5B520C06
#define GPIO_VER3_MUXING_DMIC0_CLKB_GPP_G13               0x4B42140D
#define GPIO_VER3_MUXING_DMIC0_DATA_GPP_R7                0x5B520407
#define GPIO_VER3_MUXING_DMIC1_CLKA_GPP_R4                0x5B520E04
#define GPIO_VER3_MUXING_DMIC1_CLKB_GPP_G14               0x4B42160E
#define GPIO_VER3_MUXING_DMIC1_DATA_GPP_R5                0x5B520605

#define GPIO_VER3_MUXING_CNVI_RF_RESET_GPP_G8             0x2B42E408

#define GPIO_VER3_MUXING_CNVI_CRF_XTAL_CLKREQ_GPP_G9      0x2B42E609

#define GPIO_VER3_MUXING_PSE_I2S0_RXD_GPP_E15             0x1B70160F
#define GPIO_VER3_MUXING_PSE_I2S0_TXD_GPP_E16             0x1B702610
#define GPIO_VER3_MUXING_PSE_I2S0_SCLK_GPP_E20            0x1B703615
#define GPIO_VER3_MUXING_PSE_I2S0_SFRM_GPP_E21            0x1B704614

#define GPIO_VER3_MUXING_PSE_PWM_08_GPP_E4                0x7B706604
#define GPIO_VER3_MUXING_PSE_PWM_09_GPP_E5                0x7B706805
#define GPIO_VER3_MUXING_PSE_PWM_10_GPP_E6                0x7B706A06
#define GPIO_VER3_MUXING_PSE_PWM_11_GPP_E17               0x7B706C11
#define GPIO_VER3_MUXING_PSE_PWM_12_GPP_E18               0x7B706E12
#define GPIO_VER3_MUXING_PSE_PWM_13_GPP_E19               0x7B707013
#define GPIO_VER3_MUXING_PSE_PWM_14_GPP_H10               0x4B64720A
#define GPIO_VER3_MUXING_PSE_PWM_15_GPP_H11               0x4B64740B

#define GPIO_VER3_MUXING_PSE_SPI1_MOSI_GPP_D3             0x4B852003
#define GPIO_VER3_MUXING_PSE_SPI1_MISO_GPP_D2             0x4B853002
#define GPIO_VER3_MUXING_PSE_SPI1_SCLK_GPP_D1             0x4B854001
#define GPIO_VER3_MUXING_PSE_SPI1_CSB0_GPP_D0             0x4B855000

#define GPIO_VER3_MUXING_PSE_TGPIO6_GPP_T3                0x8B81A203

#define GPIO_VER3_MUXING_PSE_TGPIO7_GPP_G11               0x8B82A40B

#define GPIO_VER3_MUXING_PSE_TGPIO8_GPP_B7                0x8B80A607

#define GPIO_VER3_MUXING_PSE_TGPIO9_GPP_B8                0x8B80A808

#define GPIO_VER3_MUXING_PSE_TGPIO10_GPP_U7               0x8B86AA07

#define GPIO_VER3_MUXING_PSE_TGPIO11_GPP_U11              0x8B86AC0B

#define GPIO_VER3_MUXING_PSE_TGPIO12_GPP_U19              0x8B86AE13

#define GPIO_VER3_MUXING_PSE_TGPIO13_GPP_D12              0x8B85B00C

#define GPIO_VER3_MUXING_PSE_TGPIO14_GPP_E20              0x8B90B214

#define GPIO_VER3_MUXING_PSE_TGPIO15_GPP_E3               0x8B90B403

#define GPIO_VER3_MUXING_PSE_TGPIO16_GPP_E7               0x8B90B607

#define GPIO_VER3_MUXING_PSE_TGPIO17_GPP_E15              0x8B90B80F

#define GPIO_VER3_MUXING_PSE_TGPIO18_GPP_E6               0x8B90BA06

#define GPIO_VER3_MUXING_PSE_TGPIO19_GPP_C1               0x8B8DBC01
#endif // _GPIO_PINS_VER3_H_
