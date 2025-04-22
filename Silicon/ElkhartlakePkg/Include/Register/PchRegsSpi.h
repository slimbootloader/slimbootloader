/** @file
  Register names for PCH SPI device.

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

  Copyright (c) 1999 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_SPI_H_
#define _PCH_REGS_SPI_H_


#define PCI_DEVICE_NUMBER_PCH_SPI           31
#define PCI_FUNCTION_NUMBER_PCH_SPI         5

#define R_SPI_CFG_BAR0                      0x10

// Flash Descriptpr Master section
#define B_FLASH_FMBA                     0x0080    ///< Flash Master Base Address
#define R_FLMSTR1_BIOS                     0x00   //Flash Master Host CPU/BIOS
#define R_FLMSTR2_ME                       0x04   //Flash Master Me
#define R_FLMSTR3_GBE                      0x08   //Flash Master Gbe
#define R_FLMSTR4_RSVD                     0x0C   //Flash Master Reserved
#define R_FLMSTR5_EC                       0x10   //Flash Master EC

#define B_READ_ACCESS_EX_REGION_12          BIT0
#define B_READ_ACCESS_EX_REGION_13          BIT1
#define B_READ_ACCESS_EX_REGION_14          BIT2
#define B_READ_ACCESS_EX_REGION_15          BIT3
#define B_WRITE_ACCESS_EX_REGION_12         BIT4
#define B_WRITE_ACCESS_EX_REGION_13         BIT5
#define B_WRITE_ACCESS_EX_REGION_14         BIT6
#define B_WRITE_ACCESS_EX_REGION_15         BIT7
#define B_READ_ACCESS_EX_REGION_0           BIT8
#define B_READ_ACCESS_EX_REGION_1           BIT9
#define B_READ_ACCESS_EX_REGION_2           BIT10
#define B_READ_ACCESS_EX_REGION_3           BIT11
#define B_READ_ACCESS_EX_REGION_4           BIT12
#define B_READ_ACCESS_EX_REGION_5           BIT13
#define B_READ_ACCESS_EX_REGION_6           BIT14
#define B_READ_ACCESS_EX_REGION_7           BIT15
#define B_READ_ACCESS_EX_REGION_8           BIT16
#define B_READ_ACCESS_EX_REGION_9           BIT17
#define B_READ_ACCESS_EX_REGION_10          BIT18
#define B_READ_ACCESS_EX_REGION_11          BIT19
#define B_WRITE_ACCESS_EX_REGION_0          BIT20
#define B_WRITE_ACCESS_EX_REGION_1          BIT21
#define B_WRITE_ACCESS_EX_REGION_2          BIT22
#define B_WRITE_ACCESS_EX_REGION_3          BIT23
#define B_WRITE_ACCESS_EX_REGION_4          BIT24
#define B_WRITE_ACCESS_EX_REGION_5          BIT25
#define B_WRITE_ACCESS_EX_REGION_6          BIT26
#define B_WRITE_ACCESS_EX_REGION_7          BIT27
#define B_WRITE_ACCESS_EX_REGION_8          BIT28
#define B_WRITE_ACCESS_EX_REGION_9          BIT29
#define B_WRITE_ACCESS_EX_REGION_10         BIT30
#define B_WRITE_ACCESS_EX_REGION_11         BIT31
#endif
