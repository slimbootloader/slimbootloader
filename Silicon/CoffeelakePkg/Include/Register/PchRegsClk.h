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

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_ICLK_H_
#define _PCH_REGS_ICLK_H_

#define R_ICLK_PCR_XTAL_DWORD0                          0x2100
#define B_ICLK_PCR_XTAL_DWORD0_WARM_STAB                (BIT6 | BIT5)
#define V_ICLK_PCR_XTAL_DWORD0_WARM_STAB_90US           BIT6
#define R_ICLK_PCR_CMU_ONE_DWORD0                       0x2280
#define B_ICLK_PCR_CMU_ONE_DWORD0_MUXSEL_CPUBCLK        (BIT3 | BIT2)
#define N_ICLK_PCR_CMU_ONE_DWORD0_MUXSEL_CPUBCLK        2
#define V_ICLK_PCR_CMU_ONE_DWORD0_MUXSEL_CPUBCLK_OCPLL  1
#define B_ICLK_PCR_CMU_ONE_DWORD0_MUXSEL_ITPXDP         (BIT5 | BIT4)
#define N_ICLK_PCR_CMU_ONE_DWORD0_MUXSEL_ITPXDP         4

#define R_ICLK_PCR_CMU_ONE_DWORD3                       0x228C
#define B_ICLK_PCR_CMU_ONE_DWORD3_CGCTRL_CLKGATEDEF     BIT9
#define R_ICLK_PCR_CMU_ONE_DWORD16                      0x22C0
#define B_ICLK_PCR_CMU_ONE_DWORD16_CK_HDAPLLBMC_MODECTRL BIT28
#define R_ICLK_PCR_CMU_ONE_DWORD25                      0x22E4
#define R_ICLK_PCR_CMU_ONE_DWORD26                      0x22E8
#define S_ICLK_PCR_MUXSEL_BITS_PER_FIELD                4      // CLKREQ number is encoded in 4 bits
#define B_ICLK_PCR_MUXSEL_BITS_PER_FIELD                0xF    // CLKREQ number is encoded in 4 bits
#define S_ICLK_PCR_MUXSEL_FIELDS_PER_REG                8      // each CMU_ONE_DWORD register contains bitfields for 8 CLK_SRC

#define R_ICLK_PCR_DBUFF_SRC0                           0x6280 // DBUFF register for CLKSRC0
#define R_ICLK_PCR_DBUFF_SRC1                           0x6300 // DBUFF register for CLKSRC1
#define R_ICLK_PCR_DBUFF_SRC2                           0x6380 // DBUFF register for CLKSRC2
#define R_ICLK_PCR_DBUFF_SRC3                           0x6400 // DBUFF register for CLKSRC3
#define R_ICLK_PCR_DBUFF_SRC4                           0x6480 // DBUFF register for CLKSRC4
#define R_ICLK_PCR_DBUFF_SRC5                           0x6500 // DBUFF register for CLKSRC5
#define R_ICLK_PCR_DBUFF_SRC6                           0x6580 // DBUFF register for CLKSRC6
#define R_ICLK_PCR_DBUFF_SRC7                           0x6600 // DBUFF register for CLKSRC7
#define R_ICLK_PCR_DBUFF_SRC8                           0x6680 // DBUFF register for CLKSRC8
#define R_ICLK_PCR_DBUFF_SRC9                           0x6700 // DBUFF register for CLKSRC9
#define R_ICLK_PCR_DBUFF_SRC10                          0x6780 // DBUFF register for CLKSRC10
#define R_ICLK_PCR_DBUFF_SRC11                          0x6800 // DBUFF register for CLKSRC11
#define R_ICLK_PCR_DBUFF_SRC12                          0x6880 // DBUFF register for CLKSRC12
#define R_ICLK_PCR_DBUFF_SRC13                          0x6900 // DBUFF register for CLKSRC13
#define R_ICLK_PCR_DBUFF_SRC14                          0x6980 // DBUFF register for CLKSRC14
#define R_ICLK_PCR_DBUFF_SRC15                          0x6A00 // DBUFF register for CLKSRC15
#define R_ICLK_PCR_DBUFF_SRC_EN                         BIT29  // Enable bit for clock source

#define R_ICLK_PCR_CAMERA1                              0x8000
#define R_ICLK_PCR_CAMERA2                              0x8080
#define B_ICLK_PCR_FREQUENCY                            BIT0
#define B_ICLK_PCR_REQUEST                              BIT1

#endif // _PCH_REGS_ICLK_H_
