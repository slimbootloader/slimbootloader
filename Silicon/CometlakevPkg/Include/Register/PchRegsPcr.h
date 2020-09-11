/** @file
  Register names for PCH private chipset register

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_PCR_H_
#define _PCH_REGS_PCR_H_

#define PID_ICLK       0xFF  //need to be removed as ACPI compilation fails
#define PID_DMI        0xEF
#define PID_SPE        0xE4
#define PID_SPD        0xE3
#define PID_SPC        0xE2
#define PID_SPB        0xE1
#define PID_SPA        0xE0
#define PID_SDX        0xFF  //need to be removed as ACPI compilation fails
#define PID_EMMC       0xFF  //need to be removed as ACPI compilation fails
#define PID_RTC_HOST   0xC3
#define PID_GPIOCOM0   0xAF
#define PID_GPIOCOM1   0xAE
#define PID_GPIOCOM2   0xAD
#define PID_GPIOCOM3   0xAC

#define PID_PSF3        0xBC
#define PID_SERIALIO    0xCB

#define R_PCH_H_PCR_PSF3_T0_SHDW_UART2_REG_BASE         0x0F00
#define R_PCH_PCR_SERIAL_IO_PCICFGCTRL14                0x228 //UART2
#define B_PCH_PCR_SERIAL_IO_PCICFGCTRL_PCI_CFG_DIS      BIT0
#define B_PCH_PCR_SERIAL_IO_PCICFGCTRL_ACPI_INTR_EN     BIT1
#define B_PCH_PCR_SERIAL_IO_PCICFGCTRL_BAR1_DIS         BIT7

#endif
