/** @file
  Register names for iCLK device

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

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_ICLK_H_
#define _PCH_REGS_ICLK_H_

#define R_ICLK_PCR_CMU_ONE_DWORD25                      0x22E4
#define R_ICLK_PCR_CMU_ONE_DWORD26                      0x22E8
#define S_ICLK_PCR_MUXSEL_BITS_PER_FIELD                4      // CLKREQ number is encoded in 4 bits
#define B_ICLK_PCR_MUXSEL_BITS_PER_FIELD                0xF    // CLKREQ number is encoded in 4 bits
#define S_ICLK_PCR_MUXSEL_FIELDS_PER_REG                8      // each CMU_ONE_DWORD register contains bitfields for 8 CLK_SRC

#define R_ICLK_PCR_CAMERA1                              0x8000
#define B_ICLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0            (BIT0 | BIT1)
#define B_ICLK_PCR_CAMERA1_CAM1_CLK_EN                  BIT2
#define R_ICLK_PCR_BIOS_BUFFEN                          0x8080
#define R_ICLK_PCR_BIOS_BUFFEN_H                        0x8084
#endif // _PCH_REGS_ICLK_H_
