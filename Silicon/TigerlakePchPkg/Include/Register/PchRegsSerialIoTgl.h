/** @file
  Device IDs for PCH Serial IO Controllers for TGL PCH

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

  Copyright (c) 2017 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_SERIAL_IO_TGL_
#define _PCH_REGS_SERIAL_IO_TGL_


//
//  Serial IO I2C0 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C0            21
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C0          0


//
//  Serial IO I2C1 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C1            21
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C1          1


//
//  Serial IO I2C2 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C2            21
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C2          2


//
//  Serial IO I2C3 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C3            21
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C3          3


//
//  Serial IO I2C4 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C4            25
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C4          0


//
//  Serial IO I2C5 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C5            25
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C5          1


//
//  Serial IO I2C6 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C6            17
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C6          3


//
//  Serial IO I2C7 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C7            16
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C7          1


//
//  Serial IO SPI0 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI0            30
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI0          2


//
//  Serial IO SPI1 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI1            30
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI1          3


//
//  Serial IO SPI2 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI2            18
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI2          6


//
//  Serial IO SPI3 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI3            19
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI3          0


//
//  Serial IO SPI4 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI4            19
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI4          1


//
//  Serial IO SPI5 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI5            19
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI5          2


//
//  Serial IO SPI6 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI6            19
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI6          3


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


//
//  Serial IO UART3 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART3           17
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART3         0


//
//  Serial IO UART4 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART4           17
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART4         1


//
//  Serial IO UART5 Controller Registers
//
#define PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART5           17
#define PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART5         2

#endif
