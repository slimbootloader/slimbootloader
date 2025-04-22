/** @file
  Register names for Serial IO UART Controllers

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
    Register that is specific to -H denoted by "_PCH_H_" in component name.
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

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _SERIAL_IO_UART_REGS_H_
#define _SERIAL_IO_UART_REGS_H_
//
//  MMIO Convergance Layer
//
#define R_SERIAL_IO_MEM_PPR_RESETS_8BIT      0x81
#define R_SERIAL_IO_MEM_PPR_CLK              0x200
#define B_SERIAL_IO_MEM_PPR_CLK_EN           BIT0
#define B_SERIAL_IO_MEM_PPR_CLK_UPDATE       BIT31

//
//  General Purpose
//
#define R_SERIAL_IO_PCR_GPPRVRW7                    0x618
#define B_SERIAL_IO_PCR_GPPRVRW7_UART0_BYTE_ADDR_EN BIT0

#endif // _SERIAL_IO_UART_REGS_H_
