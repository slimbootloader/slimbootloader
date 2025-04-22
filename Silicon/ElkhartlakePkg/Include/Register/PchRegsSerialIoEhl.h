/** @file
  Device IDs for PCH Serial IO Controllers for EHL PCH

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

  Copyright (c) 2019 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_SERIAL_IO_EHL_
#define _PCH_REGS_SERIAL_IO_EHL_


//
//  Serial IO UART0 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART0           30
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART0         0


//
//  Serial IO UART1 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART1           30
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART1         1


//
//  Serial IO UART2 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART2           25
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART2         2


#endif
