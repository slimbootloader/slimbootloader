/** @file
  GPIO pins for MTL-SoC-M.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

@par Specification Reference:
**/
#ifndef _GPIO_PINS_VER6_SOC_M_H_
#define _GPIO_PINS_VER6_SOC_M_H_

///
/// Unique ID used in GpioPad defines
///
#define GPIO_VER6_SOC_M_CHIPSET_ID     0xA

///
/// MTL SoC-M GPIO Groups
/// Use below for functions from PCH GPIO Lib which
/// require GpioGroup as argument
///
// COM0
#define GPIO_VER6_SOC_M_GROUP_CPU             ((GPIO_VER6_SOC_M_CHIPSET_ID << 8) | 0x00)
#define GPIO_VER6_SOC_M_GROUP_GPP_V           ((GPIO_VER6_SOC_M_CHIPSET_ID << 8) | 0x01)
#define GPIO_VER6_SOC_M_GROUP_GPP_C           ((GPIO_VER6_SOC_M_CHIPSET_ID << 8) | 0x02)
// COM1
#define GPIO_VER6_SOC_M_GROUP_GPP_A           ((GPIO_VER6_SOC_M_CHIPSET_ID << 8) | 0x03)
#define GPIO_VER6_SOC_M_GROUP_GPP_E           ((GPIO_VER6_SOC_M_CHIPSET_ID << 8) | 0x04)
// COM2
// COM3
#define GPIO_VER6_SOC_M_GROUP_GPP_H           ((GPIO_VER6_SOC_M_CHIPSET_ID << 8) | 0x05)
#define GPIO_VER6_SOC_M_GROUP_GPP_F           ((GPIO_VER6_SOC_M_CHIPSET_ID << 8) | 0x06)
#define GPIO_VER6_SOC_M_GROUP_SPI_SYS         ((GPIO_VER6_SOC_M_CHIPSET_ID << 8) | 0x07)
#define GPIO_VER6_SOC_M_GROUP_USB_THC         ((GPIO_VER6_SOC_M_CHIPSET_ID << 8) | 0x08)
// COM4
#define GPIO_VER6_SOC_M_GROUP_GPP_S           ((GPIO_VER6_SOC_M_CHIPSET_ID << 8) | 0x09)
#define GPIO_VER6_SOC_M_GROUP_JTAG            ((GPIO_VER6_SOC_M_CHIPSET_ID << 8) | 0x0A)
// COM5
#define GPIO_VER6_SOC_M_GROUP_GPP_B           ((GPIO_VER6_SOC_M_CHIPSET_ID << 8) | 0x0B)
#define GPIO_VER6_SOC_M_GROUP_GPP_D           ((GPIO_VER6_SOC_M_CHIPSET_ID << 8) | 0x0C)
#define GPIO_VER6_SOC_M_GROUP_VGPIO           ((GPIO_VER6_SOC_M_CHIPSET_ID << 8) | 0x0D)

///
/// MTL SOC GPIO pins
/// Use below for functions from PCH GPIO Lib which
/// require GpioPad as argument. Encoding used here
/// has all information required by library functions
///
// COM0
#define GPIO_VER6_SOC_M_CPU_PECI              ((GPIO_VER6_SOC_M_GROUP_CPU << 16 ) | 0x00)
#define GPIO_VER6_SOC_M_CPU_RESET_B           ((GPIO_VER6_SOC_M_GROUP_CPU << 16 ) | 0x01)
#define GPIO_VER6_SOC_M_CPU_SOUT              ((GPIO_VER6_SOC_M_GROUP_CPU << 16 ) | 0x02)
#define GPIO_VER6_SOC_M_CPU_SCK               ((GPIO_VER6_SOC_M_GROUP_CPU << 16 ) | 0x03)
#define GPIO_VER6_SOC_M_CPU_ALERT_B           ((GPIO_VER6_SOC_M_GROUP_CPU << 16 ) | 0x04)

#define GPIO_VER6_SOC_M_GPP_V0                ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x00)
#define GPIO_VER6_SOC_M_GPP_V1                ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x01)
#define GPIO_VER6_SOC_M_GPP_V2                ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x02)
#define GPIO_VER6_SOC_M_GPP_V3                ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x03)
#define GPIO_VER6_SOC_M_GPP_V4                ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x04)
#define GPIO_VER6_SOC_M_GPP_V5                ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x05)
#define GPIO_VER6_SOC_M_GPP_V6                ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x06)
#define GPIO_VER6_SOC_M_GPP_V7                ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x07)
#define GPIO_VER6_SOC_M_GPP_V8                ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x08)
#define GPIO_VER6_SOC_M_GPP_V9                ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x09)
#define GPIO_VER6_SOC_M_GPP_V10               ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x0A)
#define GPIO_VER6_SOC_M_GPP_V11               ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x0B)
#define GPIO_VER6_SOC_M_GPP_V12               ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x0C)
#define GPIO_VER6_SOC_M_GPP_V13               ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x0D)
#define GPIO_VER6_SOC_M_GPP_V14               ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x0E)
#define GPIO_VER6_SOC_M_GPP_V15               ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x0F)
#define GPIO_VER6_SOC_M_GPP_V16               ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x10)
#define GPIO_VER6_SOC_M_GPP_V17               ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x11)
#define GPIO_VER6_SOC_M_GPP_V18               ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x12)
#define GPIO_VER6_SOC_M_GPP_V19               ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x13)
#define GPIO_VER6_SOC_M_GPP_V20               ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x14)
#define GPIO_VER6_SOC_M_GPP_V21               ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x15)
#define GPIO_VER6_SOC_M_GPP_V22               ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x16)
#define GPIO_VER6_SOC_M_GPP_V23               ((GPIO_VER6_SOC_M_GROUP_GPP_V << 16 ) | 0x17)

#define GPIO_VER6_SOC_M_GPP_C0                ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x00)
#define GPIO_VER6_SOC_M_GPP_C1                ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x01)
#define GPIO_VER6_SOC_M_GPP_C2                ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x02)
#define GPIO_VER6_SOC_M_GPP_C3                ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x03)
#define GPIO_VER6_SOC_M_GPP_C4                ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x04)
#define GPIO_VER6_SOC_M_GPP_C5                ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x05)
#define GPIO_VER6_SOC_M_GPP_C6                ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x06)
#define GPIO_VER6_SOC_M_GPP_C7                ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x07)
#define GPIO_VER6_SOC_M_GPP_C8                ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x08)
#define GPIO_VER6_SOC_M_GPP_C9                ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x09)
#define GPIO_VER6_SOC_M_GPP_C10               ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x0A)
#define GPIO_VER6_SOC_M_GPP_C11               ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x0B)
#define GPIO_VER6_SOC_M_GPP_C12               ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x0C)
#define GPIO_VER6_SOC_M_GPP_C13               ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x0D)
#define GPIO_VER6_SOC_M_GPP_C14               ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x0E)
#define GPIO_VER6_SOC_M_GPP_C15               ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x0F)
#define GPIO_VER6_SOC_M_GPP_C16               ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x10)
#define GPIO_VER6_SOC_M_GPP_C17               ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x11)
#define GPIO_VER6_SOC_M_GPP_C18               ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x12)
#define GPIO_VER6_SOC_M_GPP_C19               ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x13)
#define GPIO_VER6_SOC_M_GPP_C20               ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x14)
#define GPIO_VER6_SOC_M_GPP_C21               ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x15)
#define GPIO_VER6_SOC_M_GPP_C22               ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x16)
#define GPIO_VER6_SOC_M_GPP_C23               ((GPIO_VER6_SOC_M_GROUP_GPP_C << 16 ) | 0x17)

// COM1
#define GPIO_VER6_SOC_M_GPP_A0                ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x00)
#define GPIO_VER6_SOC_M_GPP_A1                ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x01)
#define GPIO_VER6_SOC_M_GPP_A2                ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x02)
#define GPIO_VER6_SOC_M_GPP_A3                ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x03)
#define GPIO_VER6_SOC_M_GPP_A4                ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x04)
#define GPIO_VER6_SOC_M_GPP_A5                ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x05)
#define GPIO_VER6_SOC_M_GPP_A6                ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x06)
#define GPIO_VER6_SOC_M_GPP_A7                ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x07)
#define GPIO_VER6_SOC_M_GPP_A8                ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x08)
#define GPIO_VER6_SOC_M_GPP_A9                ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x09)
#define GPIO_VER6_SOC_M_GPP_A10               ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x0A)
#define GPIO_VER6_SOC_M_GPP_A11               ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x0B)
#define GPIO_VER6_SOC_M_GPP_A12               ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x0C)
#define GPIO_VER6_SOC_M_GPP_A13               ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x0D)
#define GPIO_VER6_SOC_M_GPP_A14               ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x0E)
#define GPIO_VER6_SOC_M_GPP_A15               ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x0F)
#define GPIO_VER6_SOC_M_GPP_A16               ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x10)
#define GPIO_VER6_SOC_M_GPP_A17               ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x11)
#define GPIO_VER6_SOC_M_GPP_A18               ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x12)
#define GPIO_VER6_SOC_M_GPP_A19               ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x13)
#define GPIO_VER6_SOC_M_GPP_A20               ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x14)
#define GPIO_VER6_SOC_M_GPP_A21               ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x15)
#define GPIO_VER6_SOC_M_GPP_A22               ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x16)
#define GPIO_VER6_SOC_M_GPP_A23               ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x17)
#define GPIO_VER6_SOC_M_ESPI_CLK_LOOPBK       ((GPIO_VER6_SOC_M_GROUP_GPP_A << 16 ) | 0x18)

#define GPIO_VER6_SOC_M_GPP_E0                ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x00)
#define GPIO_VER6_SOC_M_GPP_E1                ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x01)
#define GPIO_VER6_SOC_M_GPP_E2                ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x02)
#define GPIO_VER6_SOC_M_GPP_E3                ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x03)
#define GPIO_VER6_SOC_M_GPP_E4                ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x04)
#define GPIO_VER6_SOC_M_GPP_E5                ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x05)
#define GPIO_VER6_SOC_M_GPP_E6                ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x06)
#define GPIO_VER6_SOC_M_GPP_E7                ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x07)
#define GPIO_VER6_SOC_M_GPP_E8                ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x08)
#define GPIO_VER6_SOC_M_GPP_E9                ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x09)
#define GPIO_VER6_SOC_M_GPP_E10               ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x0A)
#define GPIO_VER6_SOC_M_GPP_E11               ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x0B)
#define GPIO_VER6_SOC_M_GPP_E12               ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x0C)
#define GPIO_VER6_SOC_M_GPP_E13               ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x0D)
#define GPIO_VER6_SOC_M_GPP_E14               ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x0E)
#define GPIO_VER6_SOC_M_GPP_E15               ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x0F)
#define GPIO_VER6_SOC_M_GPP_E16               ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x10)
#define GPIO_VER6_SOC_M_GPP_E17               ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x11)
#define GPIO_VER6_SOC_M_GPP_E18               ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x12)
#define GPIO_VER6_SOC_M_GPP_E19               ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x13)
#define GPIO_VER6_SOC_M_GPP_E20               ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x14)
#define GPIO_VER6_SOC_M_GPP_E21               ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x15)
#define GPIO_VER6_SOC_M_GPP_E22               ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x16)
#define GPIO_VER6_SOC_M_GPP_E23               ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x17)
#define GPIO_VER6_SOC_M_THC0_GPSI0_CLK_LOOPBK ((GPIO_VER6_SOC_M_GROUP_GPP_E << 16 ) | 0x18)

// COM2
// COM3
#define GPIO_VER6_SOC_M_GPP_H0                ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x00)
#define GPIO_VER6_SOC_M_GPP_H1                ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x01)
#define GPIO_VER6_SOC_M_GPP_H2                ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x02)
#define GPIO_VER6_SOC_M_GPP_H3                ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x03)
#define GPIO_VER6_SOC_M_GPP_H4                ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x04)
#define GPIO_VER6_SOC_M_GPP_H5                ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x05)
#define GPIO_VER6_SOC_M_GPP_H6                ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x06)
#define GPIO_VER6_SOC_M_GPP_H7                ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x07)
#define GPIO_VER6_SOC_M_GPP_H8                ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x08)
#define GPIO_VER6_SOC_M_GPP_H9                ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x09)
#define GPIO_VER6_SOC_M_GPP_H10               ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x0A)
#define GPIO_VER6_SOC_M_GPP_H11               ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x0B)
#define GPIO_VER6_SOC_M_GPP_H12               ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x0C)
#define GPIO_VER6_SOC_M_GPP_H13               ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x0D)
#define GPIO_VER6_SOC_M_GPP_H14               ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x0E)
#define GPIO_VER6_SOC_M_GPP_H15               ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x0F)
#define GPIO_VER6_SOC_M_GPP_H16               ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x10)
#define GPIO_VER6_SOC_M_GPP_H17               ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x11)
#define GPIO_VER6_SOC_M_GPP_H18               ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x12)
#define GPIO_VER6_SOC_M_GPP_H19               ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x13)
#define GPIO_VER6_SOC_M_GPP_H20               ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x14)
#define GPIO_VER6_SOC_M_GPP_H21               ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x15)
#define GPIO_VER6_SOC_M_GPP_H22               ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x16)
#define GPIO_VER6_SOC_M_GPP_H23               ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x17)
#define GPIO_VER6_SOC_M_LPI3C1_CLK_LOOPBK     ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x18)
#define GPIO_VER6_SOC_M_LPI3C0_CLK_LOOPBK     ((GPIO_VER6_SOC_M_GROUP_GPP_H << 16 ) | 0x19)

#define GPIO_VER6_SOC_M_GPP_F0                ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x00)
#define GPIO_VER6_SOC_M_GPP_F1                ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x01)
#define GPIO_VER6_SOC_M_GPP_F2                ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x02)
#define GPIO_VER6_SOC_M_GPP_F3                ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x03)
#define GPIO_VER6_SOC_M_GPP_F4                ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x04)
#define GPIO_VER6_SOC_M_GPP_F5                ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x05)
#define GPIO_VER6_SOC_M_GPP_F6                ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x06)
#define GPIO_VER6_SOC_M_GPP_F7                ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x07)
#define GPIO_VER6_SOC_M_GPP_F8                ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x08)
#define GPIO_VER6_SOC_M_GPP_F9                ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x09)
#define GPIO_VER6_SOC_M_GPP_F10               ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x0A)
#define GPIO_VER6_SOC_M_GPP_F11               ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x0B)
#define GPIO_VER6_SOC_M_GPP_F12               ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x0C)
#define GPIO_VER6_SOC_M_GPP_F13               ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x0D)
#define GPIO_VER6_SOC_M_GPP_F14               ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x0E)
#define GPIO_VER6_SOC_M_GPP_F15               ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x0F)
#define GPIO_VER6_SOC_M_GPP_F16               ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x10)
#define GPIO_VER6_SOC_M_GPP_F17               ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x11)
#define GPIO_VER6_SOC_M_GPP_F18               ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x12)
#define GPIO_VER6_SOC_M_GPP_F19               ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x13)
#define GPIO_VER6_SOC_M_GPP_F20               ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x14)
#define GPIO_VER6_SOC_M_GPP_F21               ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x15)
#define GPIO_VER6_SOC_M_GPP_F22               ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x16)
#define GPIO_VER6_SOC_M_GPP_F23               ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x17)
#define GPIO_VER6_SOC_M_THC1_GSPI1_CLK_LOOPBK ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x18)
#define GPIO_VER6_SOC_M_GSPI0A_CLK_LOOPBK     ((GPIO_VER6_SOC_M_GROUP_GPP_F << 16 ) | 0x19)

#define GPIO_VER6_SOC_M_SPI0_IO2              ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x00)
#define GPIO_VER6_SOC_M_SPI0_IO3              ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x01)
#define GPIO_VER6_SOC_M_SPI0_MOSI_IO_0        ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x02)
#define GPIO_VER6_SOC_M_SPI0_MISO_IO_0        ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x03)
#define GPIO_VER6_SOC_M_SPI0_TPM_CS_B         ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x04)
#define GPIO_VER6_SOC_M_SPI0_FLASH_0_CS_B     ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x05)
#define GPIO_VER6_SOC_M_SPI0_FLASH_1_CS_B     ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x06)
#define GPIO_VER6_SOC_M_SPI0_CLK              ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x07)
#define GPIO_VER6_SOC_M_BKLTEN                ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x08)
#define GPIO_VER6_SOC_M_BKLTCTL               ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x09)
#define GPIO_VER6_SOC_M_VDDEN                 ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x0A)
#define GPIO_VER6_SOC_M_SYS_PWROK             ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x0B)
#define GPIO_VER6_SOC_M_SYS_RESET_B           ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x0C)
#define GPIO_VER6_SOC_M_MLK_RST_B             ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x0D)
#define GPIO_VER6_SOC_M_SPI0_CLK_LOOPBK       ((GPIO_VER6_SOC_M_GROUP_SPI_SYS << 16 ) | 0x0E)

#define GPIO_VER6_SOC_M_USB_0                 ((GPIO_VER6_SOC_M_GROUP_USB_THC << 16 ) | 0x00)
#define GPIO_VER6_SOC_M_USB_1                 ((GPIO_VER6_SOC_M_GROUP_USB_THC << 16 ) | 0x01)
#define GPIO_VER6_SOC_M_USB_2                 ((GPIO_VER6_SOC_M_GROUP_USB_THC << 16 ) | 0x02)
#define GPIO_VER6_SOC_M_USB_3                 ((GPIO_VER6_SOC_M_GROUP_USB_THC << 16 ) | 0x03)
#define GPIO_VER6_SOC_M_USB_4                 ((GPIO_VER6_SOC_M_GROUP_USB_THC << 16 ) | 0x04)
#define GPIO_VER6_SOC_M_USB_5                 ((GPIO_VER6_SOC_M_GROUP_USB_THC << 16 ) | 0x05)
#define GPIO_VER6_SOC_M_USB_6                 ((GPIO_VER6_SOC_M_GROUP_USB_THC << 16 ) | 0x06)
#define GPIO_VER6_SOC_M_USB_7                 ((GPIO_VER6_SOC_M_GROUP_USB_THC << 16 ) | 0x07)
#define GPIO_VER6_SOC_M_TS_0                  ((GPIO_VER6_SOC_M_GROUP_USB_THC << 16 ) | 0x08)
#define GPIO_VER6_SOC_M_TS_1                  ((GPIO_VER6_SOC_M_GROUP_USB_THC << 16 ) | 0x09)
#define GPIO_VER6_SOC_M_THC_0                 ((GPIO_VER6_SOC_M_GROUP_USB_THC << 16 ) | 0x0A)
#define GPIO_VER6_SOC_M_THC_1                 ((GPIO_VER6_SOC_M_GROUP_USB_THC << 16 ) | 0x0B)
#define GPIO_VER6_SOC_M_THC_2                 ((GPIO_VER6_SOC_M_GROUP_USB_THC << 16 ) | 0x0C)
#define GPIO_VER6_SOC_M_THC_3                 ((GPIO_VER6_SOC_M_GROUP_USB_THC << 16 ) | 0x0D)

// COM4
#define GPIO_VER6_SOC_M_GPP_S0                ((GPIO_VER6_SOC_M_GROUP_GPP_S << 16) | 0x00)
#define GPIO_VER6_SOC_M_GPP_S1                ((GPIO_VER6_SOC_M_GROUP_GPP_S << 16) | 0x01)
#define GPIO_VER6_SOC_M_GPP_S2                ((GPIO_VER6_SOC_M_GROUP_GPP_S << 16) | 0x02)
#define GPIO_VER6_SOC_M_GPP_S3                ((GPIO_VER6_SOC_M_GROUP_GPP_S << 16) | 0x03)
#define GPIO_VER6_SOC_M_GPP_S4                ((GPIO_VER6_SOC_M_GROUP_GPP_S << 16) | 0x04)
#define GPIO_VER6_SOC_M_GPP_S5                ((GPIO_VER6_SOC_M_GROUP_GPP_S << 16) | 0x05)
#define GPIO_VER6_SOC_M_GPP_S6                ((GPIO_VER6_SOC_M_GROUP_GPP_S << 16) | 0x06)
#define GPIO_VER6_SOC_M_GPP_S7                ((GPIO_VER6_SOC_M_GROUP_GPP_S << 16) | 0x07)

#define GPIO_VER6_SOC_M_MBPB0                 ((GPIO_VER6_SOC_M_GROUP_JTAG << 16) | 0x00)
#define GPIO_VER6_SOC_M_MBPB1                 ((GPIO_VER6_SOC_M_GROUP_JTAG << 16) | 0x01)
#define GPIO_VER6_SOC_M_MBPB2                 ((GPIO_VER6_SOC_M_GROUP_JTAG << 16) | 0x02)
#define GPIO_VER6_SOC_M_MBPB3                 ((GPIO_VER6_SOC_M_GROUP_JTAG << 16) | 0x03)
#define GPIO_VER6_SOC_M_JTAG_TDO              ((GPIO_VER6_SOC_M_GROUP_JTAG << 16) | 0x04)
#define GPIO_VER6_SOC_M_PRDY_B                ((GPIO_VER6_SOC_M_GROUP_JTAG << 16) | 0x05)
#define GPIO_VER6_SOC_M_PREQ_B                ((GPIO_VER6_SOC_M_GROUP_JTAG << 16) | 0x06)
#define GPIO_VER6_SOC_M_JTAG_TDI              ((GPIO_VER6_SOC_M_GROUP_JTAG << 16) | 0x07)
#define GPIO_VER6_SOC_M_JTAG_TMS              ((GPIO_VER6_SOC_M_GROUP_JTAG << 16) | 0x08)
#define GPIO_VER6_SOC_M_JTAG_TCK              ((GPIO_VER6_SOC_M_GROUP_JTAG << 16) | 0x09)
#define GPIO_VER6_SOC_M_DBG_PMODE             ((GPIO_VER6_SOC_M_GROUP_JTAG << 16) | 0x0A)
#define GPIO_VER6_SOC_M_JTAG_TRST_B           ((GPIO_VER6_SOC_M_GROUP_JTAG << 16) | 0x0B)

// COM5
#define GPIO_VER6_SOC_M_GPP_B0                ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x00)
#define GPIO_VER6_SOC_M_GPP_B1                ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x01)
#define GPIO_VER6_SOC_M_GPP_B2                ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x02)
#define GPIO_VER6_SOC_M_GPP_B3                ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x03)
#define GPIO_VER6_SOC_M_GPP_B4                ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x04)
#define GPIO_VER6_SOC_M_GPP_B5                ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x05)
#define GPIO_VER6_SOC_M_GPP_B6                ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x06)
#define GPIO_VER6_SOC_M_GPP_B7                ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x07)
#define GPIO_VER6_SOC_M_GPP_B8                ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x08)
#define GPIO_VER6_SOC_M_GPP_B9                ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x09)
#define GPIO_VER6_SOC_M_GPP_B10               ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x0A)
#define GPIO_VER6_SOC_M_GPP_B11               ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x0B)
#define GPIO_VER6_SOC_M_GPP_B12               ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x0C)
#define GPIO_VER6_SOC_M_GPP_B13               ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x0D)
#define GPIO_VER6_SOC_M_GPP_B14               ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x0E)
#define GPIO_VER6_SOC_M_GPP_B15               ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x0F)
#define GPIO_VER6_SOC_M_GPP_B16               ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x10)
#define GPIO_VER6_SOC_M_GPP_B17               ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x11)
#define GPIO_VER6_SOC_M_GPP_B18               ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x12)
#define GPIO_VER6_SOC_M_GPP_B19               ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x13)
#define GPIO_VER6_SOC_M_GPP_B20               ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x14)
#define GPIO_VER6_SOC_M_GPP_B21               ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x15)
#define GPIO_VER6_SOC_M_GPP_B22               ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x16)
#define GPIO_VER6_SOC_M_GPP_B23               ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x17)
#define GPIO_VER6_SOC_M_ISHI3C0_CLK_LOOPBK    ((GPIO_VER6_SOC_M_GROUP_GPP_B << 16 ) | 0x18)

#define GPIO_VER6_SOC_M_GPP_D0                ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x00)
#define GPIO_VER6_SOC_M_GPP_D1                ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x01)
#define GPIO_VER6_SOC_M_GPP_D2                ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x02)
#define GPIO_VER6_SOC_M_GPP_D3                ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x03)
#define GPIO_VER6_SOC_M_GPP_D4                ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x04)
#define GPIO_VER6_SOC_M_GPP_D5                ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x05)
#define GPIO_VER6_SOC_M_GPP_D6                ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x06)
#define GPIO_VER6_SOC_M_GPP_D7                ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x07)
#define GPIO_VER6_SOC_M_GPP_D8                ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x08)
#define GPIO_VER6_SOC_M_GPP_D9                ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x09)
#define GPIO_VER6_SOC_M_GPP_D10               ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x0A)
#define GPIO_VER6_SOC_M_GPP_D11               ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x0B)
#define GPIO_VER6_SOC_M_GPP_D12               ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x0C)
#define GPIO_VER6_SOC_M_GPP_D13               ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x0D)
#define GPIO_VER6_SOC_M_GPP_D14               ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x0E)
#define GPIO_VER6_SOC_M_GPP_D15               ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x0F)
#define GPIO_VER6_SOC_M_GPP_D16               ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x10)
#define GPIO_VER6_SOC_M_GPP_D17               ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x11)
#define GPIO_VER6_SOC_M_GPP_D18               ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x12)
#define GPIO_VER6_SOC_M_GPP_D19               ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x13)
#define GPIO_VER6_SOC_M_GPP_D20               ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x14)
#define GPIO_VER6_SOC_M_GPP_D21               ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x15)
#define GPIO_VER6_SOC_M_GPP_D22               ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x16)
#define GPIO_VER6_SOC_M_GPP_D23               ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x17)
#define GPIO_VER6_SOC_M_BOOTHALT_B            ((GPIO_VER6_SOC_M_GROUP_GPP_D << 16 ) | 0x18)

#define GPIO_VER6_SOC_M_VGPIO0                ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x00)
#define GPIO_VER6_SOC_M_VGPIO4                ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x01)
#define GPIO_VER6_SOC_M_VGPIO5                ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x02)
#define GPIO_VER6_SOC_M_VGPIO6                ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x03)
#define GPIO_VER6_SOC_M_VGPIO7                ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x04)
#define GPIO_VER6_SOC_M_VGPIO8                ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x05)
#define GPIO_VER6_SOC_M_VGPIO9                ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x06)
#define GPIO_VER6_SOC_M_VGPIO10               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x07)
#define GPIO_VER6_SOC_M_VGPIO11               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x08)
#define GPIO_VER6_SOC_M_VGPIO12               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x09)
#define GPIO_VER6_SOC_M_VGPIO13               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x0A)
#define GPIO_VER6_SOC_M_VGPIO18               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x0B)
#define GPIO_VER6_SOC_M_VGPIO19               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x0C)
#define GPIO_VER6_SOC_M_VGPIO20               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x0D)
#define GPIO_VER6_SOC_M_VGPIO21               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x0E)
#define GPIO_VER6_SOC_M_VGPIO22               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x0F)
#define GPIO_VER6_SOC_M_VGPIO23               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x10)
#define GPIO_VER6_SOC_M_VGPIO24               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x11)
#define GPIO_VER6_SOC_M_VGPIO25               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x12)
#define GPIO_VER6_SOC_M_VGPIO30               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x13)
#define GPIO_VER6_SOC_M_VGPIO31               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x14)
#define GPIO_VER6_SOC_M_VGPIO32               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x15)
#define GPIO_VER6_SOC_M_VGPIO33               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x16)
#define GPIO_VER6_SOC_M_VGPIO34               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x17)
#define GPIO_VER6_SOC_M_VGPIO35               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x18)
#define GPIO_VER6_SOC_M_VGPIO36               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x19)
#define GPIO_VER6_SOC_M_VGPIO37               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x1A)
#define GPIO_VER6_SOC_M_VGPIO40               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x1B)
#define GPIO_VER6_SOC_M_VGPIO41               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x1C)
#define GPIO_VER6_SOC_M_VGPIO42               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x1D)
#define GPIO_VER6_SOC_M_VGPIO43               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x1E)
#define GPIO_VER6_SOC_M_VGPIO44               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x1F)
#define GPIO_VER6_SOC_M_VGPIO45               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x20)
#define GPIO_VER6_SOC_M_VGPIO46               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x21)
#define GPIO_VER6_SOC_M_VGPIO47               ((GPIO_VER6_SOC_M_GROUP_VGPIO << 16 ) | 0x22)

//
// GPIO Pin Muxing
// Determines a selection of physical pad for a given signal.
// Please refer to GPIO_NATIVE_PAD type.
// If certain signal is not listed below it means that it can be enabled
// only on a single pad and muxing setting is not needed.
//
#define GPIO_VER6_SOC_M_MUXING_ISH_SPI0_CS_GPP_D5         0x3A2CA005 // Native Function 3
#define GPIO_VER6_SOC_M_MUXING_ISH_SPI0_CS_GPP_F17        0x4A26A011 // Native Function 4
#define GPIO_VER6_SOC_M_MUXING_ISH_SPI0_CLK_GPP_D6        0x3A2C9006 // Native Function 3
#define GPIO_VER6_SOC_M_MUXING_ISH_SPI0_CLK_GPP_F11       0x4A26900B // Native Function 4
#define GPIO_VER6_SOC_M_MUXING_ISH_SPI0_MISO_GPP_D7       0x3A2C8007 // Native Function 3
#define GPIO_VER6_SOC_M_MUXING_ISH_SPI0_MISO_GPP_F12      0x4A26800C // Native Function 4
#define GPIO_VER6_SOC_M_MUXING_ISH_SPI0_MOSI_GPP_D8       0x3A2C7008 // Native Function 3
#define GPIO_VER6_SOC_M_MUXING_ISH_SPI0_MOSI_GPP_F13      0x4A26700D // Native Function 4

#define GPIO_VER6_SOC_M_MUXING_ISH_UART1_RXD_GPP_H14      0x1A25220E // Native Function 1
#define GPIO_VER6_SOC_M_MUXING_ISH_UART1_RXD_GPP_H6       0x3A252206 // Native Function 3
#define GPIO_VER6_SOC_M_MUXING_ISH_UART1_TXD_GPP_H15      0x1A25320F // Native Function 1
#define GPIO_VER6_SOC_M_MUXING_ISH_UART1_TXD_GPP_H7       0x3A253207 // Native Function 3

#define GPIO_VER6_SOC_M_MUXING_ISH_I2C2_SDA_GPP_B18       0x1A2BE412 // Native Function 1
#define GPIO_VER6_SOC_M_MUXING_ISH_I2C2_SDA_GPP_D1        0x3A2CE401 // Native Function 3
#define GPIO_VER6_SOC_M_MUXING_ISH_I2C2_SCL_GPP_B19       0x1A2BD413 // Native Function 1
#define GPIO_VER6_SOC_M_MUXING_ISH_I2C2_SCL_GPP_D2        0x3A2CD402 // Native Function 3

#define GPIO_VER6_SOC_M_MUXING_ISH_GP5_GPP_B22            0x4A0BAA16 // Native Function 4
#define GPIO_VER6_SOC_M_MUXING_ISH_GP5_GPP_E15            0x8A04AA0F // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_ISH_GP6_GPP_B23            0x4A0BAC17 // Native Function 4
#define GPIO_VER6_SOC_M_MUXING_ISH_GP6_GPP_F10            0x8A06AC0A // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_ISH_GP8_GPP_B20            0x4A0BB014 // Native Function 4
#define GPIO_VER6_SOC_M_MUXING_ISH_GP8_GPP_F22            0x8A06B016 // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_ISH_GP9_GPP_B21            0x4A0BB215 // Native Function 4
#define GPIO_VER6_SOC_M_MUXING_ISH_GP9_GPP_F23            0x8A06B217 // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_ISH_GP10_GPP_E16           0x8A04B410 // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_ISH_GP10_GPP_H10           0x8A05B40A // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_ISH_GP11_GPP_F9            0x8A06B609 // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_ISH_GP11_GPP_H11           0x8A05B60B // Native Function 8

#define GPIO_VER6_SOC_M_MUXING_SERIALIO_SPI0_CS_GPP_E10   0x5A04700A // Native Function 5
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_SPI0_CS_GPP_F18   0x8A067012 // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_SPI0_CLK_GPP_E11  0x5A04600B // Native Function 5
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_SPI0_CLK_GPP_F16  0x8A066010 // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_SPI0_MISO_GPP_E12 0x5A04500C // Native Function 5
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_SPI0_MISO_GPP_F15 0x8A06500F // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_SPI0_MOSI_GPP_E13 0x5A04400D // Native Function 5
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_SPI0_MOSI_GPP_F14 0x8A06400E // Native Function 8

#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C2_SDA_GPP_H4   0x1A45C804 // Native Function 1
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C2_SDA_GPP_B2   0x8A4BC802 // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C2_SCL_GPP_H5   0x1A45A805 // Native Function 1
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C2_SCL_GPP_B3   0x8A4BA803 // Native Function 8

#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C3_SDA_GPP_H6   0x1A45CA06 // Native Function 1
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C3_SDA_GPP_D1   0x1A4CCA01 // Native Function 1
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C3_SCL_GPP_H7   0x1A45AA07 // Native Function 1
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C3_SCL_GPP_D2   0x1A4CAA02 // Native Function 1

#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C4_SDA_GPP_E12  0x8A44CC0C // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C4_SDA_GPP_B18  0x2A4BCC12 // Native Function 2
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C4_SCL_GPP_E13  0x8A44AC0D // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C4_SCL_GPP_B19  0x2A4BAC13 // Native Function 2

#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C5_SDA_GPP_F13  0x8A46CE0D // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C5_SDA_GPP_B20  0x2A4BCE14 // Native Function 2
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C5_SCL_GPP_F12  0x8A46AE0C // Native Function 8
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I2C5_SCL_GPP_B21  0x2A4BAE15 // Native Function 2

#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I3C1_SDA_GPP_H21  0x2A652015 // Native Function 2
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I3C1_SDA_GPP_H10  0x2A65200A // Native Function 2
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I3C1_SCL_GPP_H22  0x2A651C16 // Native Function 2
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I3C1_SCL_GPP_H11  0x2A651C0B // Native Function 2
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I3C1_SCL_FB       0x2A65C018 // Native Function 2
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_I3C1_SCL_FB_A     0x3A65C018 // Native Function 3

#define GPIO_VER6_SOC_M_MUXING_SERIALIO_UART1_RXD_GPP_H6  0x2A050406 // Native Function 2
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_UART1_RXD_GPP_H14 0x2A05040E // Native Function 2
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_UART1_TXD_GPP_H7  0x2A051407 // Native Function 2
#define GPIO_VER6_SOC_M_MUXING_SERIALIO_UART1_TXD_GPP_H15 0x2A05140F // Native Function 2

#endif // _GPIO_PINS_VER6_SOC_M_H_
