/** @file
  Register names for PCIE standard register

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position

  Copyright (c) 2014 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCIE_REGS_H_
#define _PCIE_REGS_H_

#include <IndustryStandard/Pci30.h>

//
// PCI type 0 Header
//

#define R_PCI_BAR0_OFFSET                         0x10


//
// PCI Express Capability List Register (CAPID:10h)
//
#define R_PCIE_SLSTS_OFFSET                       0x1A ///< Slot Status Register (Offset 1Ah)
#define B_PCIE_SLSTS_PDS                          BIT6 ///< Presence Detect State

//
// L1 Sub-States Extended Capability Register (CAPID:001Eh)
//
#define  B_PCIE_EX_L1SCAP_PTV                     0x00F80000 //< Port Tpower_on value
#define  N_PCIE_EX_L1SCAP_PTV                     19
#define  B_PCIE_EX_L1SCAP_PTPOS                   0x00030000 //< Port Tpower_on scale
#define  N_PCIE_EX_L1SCAP_PTPOS                   16
#define  B_PCIE_EX_L1SCAP_CMRT                    0x0000FF00 //< Common Mode Restore time
#define  N_PCIE_EX_L1SCAP_CMRT                    8
#define  V_PCIE_EX_L1SCAP_PTPOS_2us               0
#define  V_PCIE_EX_L1SCAP_PTPOS_10us              1
#define  V_PCIE_EX_L1SCAP_PTPOS_100us             2
#define  B_PCIE_EX_L1SCAP_L1SSES                  BIT5 ///< CLKREQ Acceleration Supported
#define  B_PCIE_EX_L1SCAP_L1PSS                   BIT4 ///< L1 PM substates supported
#define  B_PCIE_EX_L1SCAP_AL1SS                   BIT3 ///< ASPM L1.1 supported
#define  B_PCIE_EX_L1SCAP_AL12S                   BIT2 ///< ASPM L1.2 supported
#define  B_PCIE_EX_L1SCAP_PPL11S                  BIT1 ///< PCI-PM L1.1 supported
#define  B_PCIE_EX_L1SCAP_PPL12S                  BIT0 ///< PCI-PM L1.2 supported

//
// PTM Extended Capability Register (CAPID:001Fh)
//
#define  B_PCIE_EX_PTMCAP_PTMRC                   BIT2 ///< PTM Root Capable
#define  B_PCIE_EX_PTMCAP_PTMRSPC                 BIT1 ///< PTM Responder Capable

//
// Base Address Offset
//
#define B_PCI_BAR_MEMORY_TYPE_MASK                (BIT1 | BIT2)
#define B_PCI_BAR_MEMORY_TYPE_64                  BIT2


#endif
