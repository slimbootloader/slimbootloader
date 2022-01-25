/**@file
  Device IDs for Serial IO Controllers for ADL PCH

  Conventions:

  - Register definition format:
    Prefix_[GenerationName]_[ComponentName]_SubsystemName_RegisterSpace_RegisterName
  - Prefix:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register size
    Definitions beginning with "N_" are the bit position
  - [GenerationName]:
    Three letter acronym of the generation is used (e.g. SKL,KBL,CNL etc.).
    Register name without GenerationName applies to all generations.
  - [ComponentName]:
    This field indicates the component name that the register belongs to (e.g. PCH, SA etc.)
    Register name without ComponentName applies to all components.
    Register that is specific to -H denoted by "_PCH_S_" in component name.
    Register that is specific to -N denoted by "_PCH_N_" in component name.
    Register that is specific to -LP denoted by "_PCH_LP_" in component name.
  - SubsystemName:
    This field indicates the subsystem name of the component that the register belongs to
    (e.g. PCIE, USB, SATA, GPIO, PMC etc.).
  - RegisterSpace:
    MEM - MMIO space register of subsystem.
    IO  - IO space register of subsystem.
    PCR - Private configuration register of subsystem.
    CFG - PCI configuration space register of subsystem.
  - RegisterName:
    Full register name.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _SERIAL_IO_REGS_VER4_H_
#define _SERIAL_IO_REGS_VER4_H_

#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C0     0x200
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C1     0x204
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C2     0x208
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C3     0x20C
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C4     0x210
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C5     0x214
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C6     0x218
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_I2C7     0x21C

#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_SPI0     0x23C
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_SPI1     0x240
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_SPI2     0x244
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_SPI3     0x248
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_SPI4     0x24C
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_SPI5     0x250
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_SPI6     0x254

#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART0    0x220
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART1    0x224
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART2    0x228
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART3    0x22C
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART4    0x230
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART5    0x234
#define R_VER4_SERIAL_IO_PCR_PCICFGCTRL_UART6    0x238


#endif //_SERIAL_IO_REGS_VER4_H_
