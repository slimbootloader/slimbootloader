/** @file
  Register names for PCH PCI-E root port devices

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_[generation_name]_" in register/bit names.
  - Registers / bits that are specific to PCH-H denoted by "_H_" in register/bit names.
    Registers / bits that are specific to PCH-LP denoted by "_LP_" in register/bit names.
    e.g., "_PCH_H_", "_PCH_LP_"
    Registers / bits names without _H_ or _LP_ apply for both H and LP.
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without [generation_name] inserted.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_PCIE_H_
#define _PCH_REGS_PCIE_H_

//
// PCH PCI Express Root Ports (D28:F0..7, D29:F0..7, D27:F0..7)
//
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1  0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2  1
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3  2
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4  3
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5  4
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6  5
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7  6
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8  7
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_9  0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_10 1
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_11 2
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_12 3
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_13 4
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_14 5
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_15 6
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_16 7
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_17 0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_18 1
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_19 2
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_20 3
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_21 4
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_22 5
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_23 6
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_24 7


#define R_PCH_PCIE_CLIST                          0x40
#define R_PCH_PCIE_SLSTS                          (R_PCH_PCIE_CLIST + R_PCIE_SLSTS_OFFSET)


#ifndef PCH_ICL
#define R_PCH_PCIE_EX_ACSECH                      0x140 ///< ACS Extended Capability Header
#else
#define R_PCH_PCIE_EX_ACSECH                      0x220 ///< ACS Extended Capability Header
#endif


#ifndef PCH_ICL
#define R_PCH_PCIE_EX_SPEECH                      0x220 ///< Secondary PCI Express Extended Capability Header
#else
#define R_PCH_PCIE_EX_SPEECH                      0xA30 ///< Secondary PCI Express Extended Capability Header
#endif

#ifndef PCH_ICL
#define R_PCH_PCIE_EX_DPCECH                      0x250 ///< Downstream Port Containment
#else
#define R_PCH_PCIE_EX_DPCECH                      0xA00 ///< Downstream Port Containment
#endif


#define R_SPX_PCR_PCD                         0                           ///< Port configuration and disable
#define B_SPX_PCR_PCD_RP1FN                   (BIT2 | BIT1 | BIT0)        ///< Port 1 Function Number
#define B_SPX_PCR_PCD_RP1CH                   BIT3                        ///< Port 1 config hide
#define B_SPX_PCR_PCD_RP2FN                   (BIT6 | BIT5 | BIT4)        ///< Port 2 Function Number
#define B_SPX_PCR_PCD_RP2CH                   BIT7                        ///< Port 2 config hide
#define B_SPX_PCR_PCD_RP3FN                   (BIT10 | BIT9 | BIT8)       ///< Port 3 Function Number
#define B_SPX_PCR_PCD_RP3CH                   BIT11                       ///< Port 3 config hide
#define B_SPX_PCR_PCD_RP4FN                   (BIT14 | BIT13 | BIT12)     ///< Port 4 Function Number
#define B_SPX_PCR_PCD_RP4CH                   BIT15                       ///< Port 4 config hide
#define S_SPX_PCR_PCD_RP_FIELD                4                           ///< 4 bits for each RP FN
#define B_SPX_PCR_PCD_P1D                     BIT16                       ///< Port 1 disable
#define B_SPX_PCR_PCD_P2D                     BIT17                       ///< Port 2 disable
#define B_SPX_PCR_PCD_P3D                     BIT18                       ///< Port 3 disable
#define B_SPX_PCR_PCD_P4D                     BIT19                       ///< Port 4 disable
#define B_SPX_PCR_PCD_SRL                     BIT31                       ///< Secured Register Lock
#endif
