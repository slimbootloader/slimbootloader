/** @file
  GPIO pins for TGL-PCH-LP,

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _GPIO_PINS_VER2_LP_H_
#define _GPIO_PINS_VER2_LP_H_
///
/// This header file should be used together with
/// PCH GPIO lib in C and ASL. All defines used
/// must match both ASL/C syntax
///

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
#define GPIO_VER2_LP_GROUP_SPI      0x0904
#define GPIO_VER2_LP_GROUP_GPD      0x0905
#define GPIO_VER2_LP_GROUP_GPP_S    0x0906
#define GPIO_VER2_LP_GROUP_GPP_H    0x0907
#define GPIO_VER2_LP_GROUP_GPP_D    0x0908
#define GPIO_VER2_LP_GROUP_GPP_U    0x0909
#define GPIO_VER2_LP_GROUP_VGPIO    0x090A
#define GPIO_VER2_LP_GROUP_GPP_C    0x090B
#define GPIO_VER2_LP_GROUP_GPP_F    0x090C
#define GPIO_VER2_LP_GROUP_HVCMOS   0x090D
#define GPIO_VER2_LP_GROUP_GPP_E    0x090E
#define GPIO_VER2_LP_GROUP_JTAG     0x090F
#define GPIO_VER2_LP_GROUP_CPU      0x0910
#define GPIO_VER2_LP_GROUP_VGPIO_3  0x0911


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
#define GPIO_VER2_LP_GPP_B9                  0x09000009
#define GPIO_VER2_LP_GPP_B10                 0x0900000A
#define GPIO_VER2_LP_GPP_B11                 0x0900000B
#define GPIO_VER2_LP_GPP_B12                 0x0900000C
#define GPIO_VER2_LP_GPP_B13                 0x0900000D
#define GPIO_VER2_LP_GPP_B14                 0x0900000E
#define GPIO_VER2_LP_GPP_B15                 0x0900000F
#define GPIO_VER2_LP_GPP_B16                 0x09000010
#define GPIO_VER2_LP_GPP_B17                 0x09000011
#define GPIO_VER2_LP_GPP_B18                 0x09000012
#define GPIO_VER2_LP_GPP_B19                 0x09000013
#define GPIO_VER2_LP_GPP_B20                 0x09000014
#define GPIO_VER2_LP_GPP_B21                 0x09000015
#define GPIO_VER2_LP_GPP_B22                 0x09000016
#define GPIO_VER2_LP_GPP_B23                 0x09000017
#define GPIO_VER2_LP_GSPI0_CLK_LOOPBK        0x09000018
#define GPIO_VER2_LP_GSPI1_CLK_LOOPBK        0x09000019

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
#define GPIO_VER2_LP_ESPI_CLK_LOOPBK         0x09020018

#define GPIO_VER2_LP_GPP_R0                  0x09030000
#define GPIO_VER2_LP_GPP_R1                  0x09030001
#define GPIO_VER2_LP_GPP_R2                  0x09030002
#define GPIO_VER2_LP_GPP_R3                  0x09030003
#define GPIO_VER2_LP_GPP_R4                  0x09030004
#define GPIO_VER2_LP_GPP_R5                  0x09030005
#define GPIO_VER2_LP_GPP_R6                  0x09030006
#define GPIO_VER2_LP_GPP_R7                  0x09030007

#define GPIO_VER2_LP_SPI0_IO_2               0x09040000
#define GPIO_VER2_LP_SPI0_IO_3               0x09040001
#define GPIO_VER2_LP_SPI0_MOSI_IO_0          0x09040002
#define GPIO_VER2_LP_SPI0_MISO_IO_1          0x09040003
#define GPIO_VER2_LP_SPI0_TPM_CSB            0x09040004
#define GPIO_VER2_LP_SPI0_FLASH_0_CSB        0x09040005
#define GPIO_VER2_LP_SPI0_FLASH_1_CSB        0x09040006
#define GPIO_VER2_LP_SPI0_CLK                0x09040007
#define GPIO_VER2_LP_SPI0_CLK_LOOPBK         0x09040008

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
#define GPIO_VER2_LP_INPUT3VSEL              0x0905000C
#define GPIO_VER2_LP_SLP_LANB                0x0905000D
#define GPIO_VER2_LP_SLP_SUSB                0x0905000E
#define GPIO_VER2_LP_WAKEB                   0x0905000F
#define GPIO_VER2_LP_DRAM_RESETB             0x09050010

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
#define GPIO_VER2_LP_GPP_H14                 0x0907000E
#define GPIO_VER2_LP_GPP_H15                 0x0907000F
#define GPIO_VER2_LP_GPP_H16                 0x09070010
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
#define GPIO_VER2_LP_GSPI2_CLK_LOOPBK        0x09080014

#define GPIO_VER2_LP_GPP_U0                  0x09090000
#define GPIO_VER2_LP_GPP_U1                  0x09090001
#define GPIO_VER2_LP_GPP_U2                  0x09090002
#define GPIO_VER2_LP_GPP_U3                  0x09090003
#define GPIO_VER2_LP_GPP_U4                  0x09090004
#define GPIO_VER2_LP_GPP_U5                  0x09090005
#define GPIO_VER2_LP_GPP_U6                  0x09090006
#define GPIO_VER2_LP_GPP_U7                  0x09090007
#define GPIO_VER2_LP_GPP_U8                  0x09090008
#define GPIO_VER2_LP_GPP_U9                  0x09090009
#define GPIO_VER2_LP_GPP_U10                 0x0909000A
#define GPIO_VER2_LP_GPP_U11                 0x0909000B
#define GPIO_VER2_LP_GPP_U12                 0x0909000C
#define GPIO_VER2_LP_GPP_U13                 0x0909000D
#define GPIO_VER2_LP_GPP_U14                 0x0909000E
#define GPIO_VER2_LP_GPP_U15                 0x0909000F
#define GPIO_VER2_LP_GPP_U16                 0x09090010
#define GPIO_VER2_LP_GPP_U17                 0x09090011
#define GPIO_VER2_LP_GPP_U18                 0x09090012
#define GPIO_VER2_LP_GPP_U19                 0x09090013
#define GPIO_VER2_LP_GSPI3_CLK_LOOPBK        0x09090014
#define GPIO_VER2_LP_GSPI4_CLK_LOOPBK        0x09090015
#define GPIO_VER2_LP_GSPI5_CLK_LOOPBK        0x09090016
#define GPIO_VER2_LP_GSPI6_CLK_LOOPBK        0x09090017

#define GPIO_VER2_LP_VGPIO0                  0x090A0000
#define GPIO_VER2_LP_VGPIO4                  0x090A0001
#define GPIO_VER2_LP_VGPIO5                  0x090A0002
#define GPIO_VER2_LP_VGPIO6                  0x090A0003
#define GPIO_VER2_LP_VGPIO7                  0x090A0004
#define GPIO_VER2_LP_VGPIO8                  0x090A0005
#define GPIO_VER2_LP_VGPIO9                  0x090A0006
#define GPIO_VER2_LP_VGPIO10                 0x090A0007
#define GPIO_VER2_LP_VGPIO11                 0x090A0008
#define GPIO_VER2_LP_VGPIO12                 0x090A0009
#define GPIO_VER2_LP_VGPIO13                 0x090A000A
#define GPIO_VER2_LP_VGPIO18                 0x090A000B
#define GPIO_VER2_LP_VGPIO19                 0x090A000C
#define GPIO_VER2_LP_VGPIO20                 0x090A000D
#define GPIO_VER2_LP_VGPIO21                 0x090A000E
#define GPIO_VER2_LP_VGPIO22                 0x090A000F
#define GPIO_VER2_LP_VGPIO23                 0x090A0010
#define GPIO_VER2_LP_VGPIO24                 0x090A0011
#define GPIO_VER2_LP_VGPIO25                 0x090A0012
#define GPIO_VER2_LP_VGPIO30                 0x090A0013
#define GPIO_VER2_LP_VGPIO31                 0x090A0014
#define GPIO_VER2_LP_VGPIO32                 0x090A0015
#define GPIO_VER2_LP_VGPIO33                 0x090A0016
#define GPIO_VER2_LP_VGPIO34                 0x090A0017
#define GPIO_VER2_LP_VGPIO35                 0x090A0018
#define GPIO_VER2_LP_VGPIO36                 0x090A0019
#define GPIO_VER2_LP_VGPIO37                 0x090A001A

#define GPIO_VER2_LP_GPP_C0                  0x090B0000
#define GPIO_VER2_LP_GPP_C1                  0x090B0001
#define GPIO_VER2_LP_GPP_C2                  0x090B0002
#define GPIO_VER2_LP_GPP_C3                  0x090B0003
#define GPIO_VER2_LP_GPP_C4                  0x090B0004
#define GPIO_VER2_LP_GPP_C5                  0x090B0005
#define GPIO_VER2_LP_GPP_C6                  0x090B0006
#define GPIO_VER2_LP_GPP_C7                  0x090B0007
#define GPIO_VER2_LP_GPP_C8                  0x090B0008
#define GPIO_VER2_LP_GPP_C9                  0x090B0009
#define GPIO_VER2_LP_GPP_C10                 0x090B000A
#define GPIO_VER2_LP_GPP_C11                 0x090B000B
#define GPIO_VER2_LP_GPP_C12                 0x090B000C
#define GPIO_VER2_LP_GPP_C13                 0x090B000D
#define GPIO_VER2_LP_GPP_C14                 0x090B000E
#define GPIO_VER2_LP_GPP_C15                 0x090B000F
#define GPIO_VER2_LP_GPP_C16                 0x090B0010
#define GPIO_VER2_LP_GPP_C17                 0x090B0011
#define GPIO_VER2_LP_GPP_C18                 0x090B0012
#define GPIO_VER2_LP_GPP_C19                 0x090B0013
#define GPIO_VER2_LP_GPP_C20                 0x090B0014
#define GPIO_VER2_LP_GPP_C21                 0x090B0015
#define GPIO_VER2_LP_GPP_C22                 0x090B0016
#define GPIO_VER2_LP_GPP_C23                 0x090B0017


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
#define GPIO_VER2_LP_GPPF_CLK_LOOPBK         0x090C0018

#define GPIO_VER2_LP_L_BKLTEN                0x090D0000
#define GPIO_VER2_LP_L_BKLTCTL               0x090D0001
#define GPIO_VER2_LP_L_VDDEN                 0x090D0002
#define GPIO_VER2_LP_SYS_PWROK               0x090D0003
#define GPIO_VER2_LP_SYS_RESETB              0x090D0004
#define GPIO_VER2_LP_MLK_RSTB                0x090D0005

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
#define GPIO_VER2_LP_GPPE_CLK_LOOPBK         0x090E0018

#define GPIO_VER2_LP_JTAG_TDO                0x090F0000
#define GPIO_VER2_LP_JTAGX                   0x090F0001
#define GPIO_VER2_LP_PRDYB                   0x090F0002
#define GPIO_VER2_LP_PREQB                   0x090F0003
#define GPIO_VER2_LP_CPU_TRSTB               0x090F0004
#define GPIO_VER2_LP_JTAG_TDI                0x090F0005
#define GPIO_VER2_LP_JTAG_TMS                0x090F0006
#define GPIO_VER2_LP_JTAG_TCK                0x090F0007
#define GPIO_VER2_LP_DBG_PMODE               0x090F0008
#define GPIO_VER2_LP_MLK                     0x090F0009

#define GPIO_VER2_LP_HDACPU_SDI              0x09100000
#define GPIO_VER2_LP_HDACPU_SDO              0x09100001
#define GPIO_VER2_LP_HDACPU_SCLK             0x09100002
#define GPIO_VER2_LP_PM_SYNC                 0x09100003
#define GPIO_VER2_LP_PECI                    0x09100004
#define GPIO_VER2_LP_CPUPWRGD                0x09100005
#define GPIO_VER2_LP_THRMTRIPB               0x09100006
#define GPIO_VER2_LP_PLTRST_CPUB             0x09100007
#define GPIO_VER2_LP_PM_DOWN                 0x09100008
#define GPIO_VER2_LP_TRIGGER_IN              0x09100009
#define GPIO_VER2_LP_TRIGGER_OUT             0x0910000A
#define GPIO_VER2_LP_UFS_RESETB              0x0910000B
#define GPIO_VER2_LP_CLKOUT_CPURTC           0x0910000C
#define GPIO_VER2_LP_VCCST_OVERRIDE          0x0910000D
#define GPIO_VER2_LP_C10_WAKE                0x0910000E

#define GPIO_VER2_LP_VGPIO_PCIE_0            0x09110000
#define GPIO_VER2_LP_VGPIO_PCIE_1            0x09110001
#define GPIO_VER2_LP_VGPIO_PCIE_2            0x09110002
#define GPIO_VER2_LP_VGPIO_PCIE_3            0x09110003
#define GPIO_VER2_LP_VGPIO_PCIE_4            0x09110004
#define GPIO_VER2_LP_VGPIO_PCIE_5            0x09110005
#define GPIO_VER2_LP_VGPIO_PCIE_6            0x09110006
#define GPIO_VER2_LP_VGPIO_PCIE_7            0x09110007
#define GPIO_VER2_LP_VGPIO_PCIE_8            0x09110008
#define GPIO_VER2_LP_VGPIO_PCIE_9            0x09110009
#define GPIO_VER2_LP_VGPIO_PCIE_10           0x0911000A
#define GPIO_VER2_LP_VGPIO_PCIE_11           0x0911000B
#define GPIO_VER2_LP_VGPIO_PCIE_12           0x0911000C
#define GPIO_VER2_LP_VGPIO_PCIE_13           0x0911000D
#define GPIO_VER2_LP_VGPIO_PCIE_14           0x0911000E
#define GPIO_VER2_LP_VGPIO_PCIE_15           0x0911000F
#define GPIO_VER2_LP_VGPIO_PCIE_16           0x09110010
#define GPIO_VER2_LP_VGPIO_PCIE_17           0x09110011
#define GPIO_VER2_LP_VGPIO_PCIE_18           0x09110012
#define GPIO_VER2_LP_VGPIO_PCIE_19           0x09110013
#define GPIO_VER2_LP_VGPIO_PCIE_20           0x09110014
#define GPIO_VER2_LP_VGPIO_PCIE_21           0x09110015
#define GPIO_VER2_LP_VGPIO_PCIE_22           0x09110016
#define GPIO_VER2_LP_VGPIO_PCIE_23           0x09110017
#define GPIO_VER2_LP_VGPIO_PCIE_24           0x09110018
#define GPIO_VER2_LP_VGPIO_PCIE_25           0x09110019
#define GPIO_VER2_LP_VGPIO_PCIE_26           0x0911001A
#define GPIO_VER2_LP_VGPIO_PCIE_27           0x0911001B
#define GPIO_VER2_LP_VGPIO_PCIE_28           0x0911001C
#define GPIO_VER2_LP_VGPIO_PCIE_29           0x0911001D
#define GPIO_VER2_LP_VGPIO_PCIE_30           0x0911001E
#define GPIO_VER2_LP_VGPIO_PCIE_31           0x0911001F
#define GPIO_VER2_LP_VGPIO_PCIE_32           0x09110020
#define GPIO_VER2_LP_VGPIO_PCIE_33           0x09110021
#define GPIO_VER2_LP_VGPIO_PCIE_34           0x09110022
#define GPIO_VER2_LP_VGPIO_PCIE_35           0x09110023
#define GPIO_VER2_LP_VGPIO_PCIE_36           0x09110024
#define GPIO_VER2_LP_VGPIO_PCIE_37           0x09110025
#define GPIO_VER2_LP_VGPIO_PCIE_38           0x09110026
#define GPIO_VER2_LP_VGPIO_PCIE_39           0x09110027
#define GPIO_VER2_LP_VGPIO_PCIE_40           0x09110028
#define GPIO_VER2_LP_VGPIO_PCIE_41           0x09110029
#define GPIO_VER2_LP_VGPIO_PCIE_42           0x0911002A
#define GPIO_VER2_LP_VGPIO_PCIE_43           0x0911002B
#define GPIO_VER2_LP_VGPIO_PCIE_44           0x0911002C
#define GPIO_VER2_LP_VGPIO_PCIE_45           0x0911002D
#define GPIO_VER2_LP_VGPIO_PCIE_46           0x0911002E
#define GPIO_VER2_LP_VGPIO_PCIE_47           0x0911002F
#define GPIO_VER2_LP_VGPIO_PCIE_48           0x09110030
#define GPIO_VER2_LP_VGPIO_PCIE_49           0x09110031
#define GPIO_VER2_LP_VGPIO_PCIE_50           0x09110032
#define GPIO_VER2_LP_VGPIO_PCIE_51           0x09110033
#define GPIO_VER2_LP_VGPIO_PCIE_52           0x09110034
#define GPIO_VER2_LP_VGPIO_PCIE_53           0x09110035
#define GPIO_VER2_LP_VGPIO_PCIE_54           0x09110036
#define GPIO_VER2_LP_VGPIO_PCIE_55           0x09110037
#define GPIO_VER2_LP_VGPIO_PCIE_56           0x09110038
#define GPIO_VER2_LP_VGPIO_PCIE_57           0x09110039
#define GPIO_VER2_LP_VGPIO_PCIE_58           0x0911003A
#define GPIO_VER2_LP_VGPIO_PCIE_59           0x0911003B
#define GPIO_VER2_LP_VGPIO_PCIE_60           0x0911003C
#define GPIO_VER2_LP_VGPIO_PCIE_61           0x0911003D
#define GPIO_VER2_LP_VGPIO_PCIE_62           0x0911003E
#define GPIO_VER2_LP_VGPIO_PCIE_63           0x0911003F
#define GPIO_VER2_LP_VGPIO_PCIE_64           0x09110040
#define GPIO_VER2_LP_VGPIO_PCIE_65           0x09110041
#define GPIO_VER2_LP_VGPIO_PCIE_66           0x09110042
#define GPIO_VER2_LP_VGPIO_PCIE_67           0x09110043
#define GPIO_VER2_LP_VGPIO_PCIE_68           0x09110044
#define GPIO_VER2_LP_VGPIO_PCIE_69           0x09110045
#define GPIO_VER2_LP_VGPIO_PCIE_70           0x09110046
#define GPIO_VER2_LP_VGPIO_PCIE_71           0x09110047
#define GPIO_VER2_LP_VGPIO_PCIE_72           0x09110048
#define GPIO_VER2_LP_VGPIO_PCIE_73           0x09110049
#define GPIO_VER2_LP_VGPIO_PCIE_74           0x0911004A
#define GPIO_VER2_LP_VGPIO_PCIE_75           0x0911004B
#define GPIO_VER2_LP_VGPIO_PCIE_76           0x0911004C
#define GPIO_VER2_LP_VGPIO_PCIE_77           0x0911004D
#define GPIO_VER2_LP_VGPIO_PCIE_78           0x0911004E
#define GPIO_VER2_LP_VGPIO_PCIE_79           0x0911004F
#define GPIO_VER2_LP_VGPIO_USB_0             0x09110050
#define GPIO_VER2_LP_VGPIO_USB_1             0x09110051
#define GPIO_VER2_LP_VGPIO_USB_2             0x09110052
#define GPIO_VER2_LP_VGPIO_USB_3             0x09110053
#define GPIO_VER2_LP_VGPIO_USB_4             0x09110054
#define GPIO_VER2_LP_VGPIO_USB_5             0x09110055
#define GPIO_VER2_LP_VGPIO_USB_6             0x09110056
#define GPIO_VER2_LP_VGPIO_USB_7             0x09110057
#define GPIO_VER2_LP_VGPIO_PCIE_80           0x09110058
#define GPIO_VER2_LP_VGPIO_PCIE_81           0x09110059
#define GPIO_VER2_LP_VGPIO_PCIE_82           0x0911005A
#define GPIO_VER2_LP_VGPIO_PCIE_83           0x0911005B

//
// GPIO Pin Muxing
// Determines a selection of physical pad for a given signal.
// Please refer to GPIO_NATIVE_PAD type.
// If certain signal is not listed below it means that it can be enabled
// only on a single pad and muxing setting is not needed.
//
#define GPIO_VER2_LP_MUXING_SERIALIO_UART0_RXD_GPP_C8        0x190B0208
#define GPIO_VER2_LP_MUXING_SERIALIO_UART0_RXD_GPP_F1        0x290C0201
#define GPIO_VER2_LP_MUXING_SERIALIO_UART0_TXD_GPP_C9        0x190B1209
#define GPIO_VER2_LP_MUXING_SERIALIO_UART0_TXD_GPP_F2        0x290C1202
#define GPIO_VER2_LP_MUXING_SERIALIO_UART0_RTS_GPP_C10       0x190B220A
#define GPIO_VER2_LP_MUXING_SERIALIO_UART0_RTS_GPP_F0        0x290C2200
#define GPIO_VER2_LP_MUXING_SERIALIO_UART0_CTS_GPP_C11       0x190B320B
#define GPIO_VER2_LP_MUXING_SERIALIO_UART0_CTS_GPP_F3        0x290C3203

#define GPIO_VER2_LP_MUXING_SERIALIO_I2C4_SDA_GPP_H8         0x1947CC08
#define GPIO_VER2_LP_MUXING_SERIALIO_I2C4_SDA_GPP_D13        0x3948CC0D
#define GPIO_VER2_LP_MUXING_SERIALIO_I2C4_SCL_GPP_H9         0x1947AC09
#define GPIO_VER2_LP_MUXING_SERIALIO_I2C4_SCL_GPP_D14        0x3948AC0E

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

#define GPIO_VER2_LP_MUXING_CNVI_RF_RESET_GPP_A8             0x2942E408
#define GPIO_VER2_LP_MUXING_CNVI_RF_RESET_GPP_F4             0x194CE404

#define GPIO_VER2_LP_MUXING_CNVI_MODEM_CLKREQ_GPP_A9         0x2942E609
#define GPIO_VER2_LP_MUXING_CNVI_CRF_XTAL_CLKREQ_GPP_A9      0x3942E609
#define GPIO_VER2_LP_MUXING_CNVI_MODEM_CLKREQ_GPP_F5         0x294CE605
#define GPIO_VER2_LP_MUXING_CNVI_CRF_XTAL_CLKREQ_GPP_F5      0x394CE605

#endif // _GPIO_PINS_VER2_LP_H_
