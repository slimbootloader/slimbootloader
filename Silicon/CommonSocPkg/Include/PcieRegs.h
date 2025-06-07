/**@file
  Register names for PCIE standard register

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PCIE_REGS_H_
#define _PCIE_REGS_H_

#include <IndustryStandard/Pci30.h>


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


#define R_PCI_BAR0_OFFSET                         0x10

//
// PCI type 1 Header
//
#define B_PCI_BRIDGE_BNUM_SBBN                    0x00FF0000 ///< Subordinate Bus Number
#define B_PCI_BRIDGE_BNUM_SCBN                    0x0000FF00 ///< Secondary Bus Number
#define B_PCI_BRIDGE_BNUM_SBBN_SCBN               (B_PCI_BRIDGE_BNUM_SBBN | B_PCI_BRIDGE_BNUM_SCBN)

//
// PCI Express Capability List Register (CAPID:10h)
//
#define R_PCIE_LCAP_OFFSET                        0x0C ///< Link Capabilities Register (Offset 0Ch)
#define R_PCIE_LCTL_OFFSET                        0x10 ///< Link Control Register (Offset 10h)
#define B_PCIE_LCTL_CCC                           BIT6 ///< Common Clock Configuration
#define B_PCIE_LCTL_RL                            BIT5 ///< Retrain Link

#define R_PCIE_LSTS_OFFSET                        0x12 ///< Link Status Register (Offset 12h)
#define B_PCIE_LSTS_SCC                           BIT12 ///< Slot Clock Configuration
#define B_PCIE_LSTS_LT                            BIT11 ///< Link Training

#define B_PCIE_DCTL2_LTREN                        BIT10 ///< LTR Mechanism Enable


#define  V_PCIE_SVCAP_CID                        0x0D
#define  V_PCIE_MID_CID                          0x05
#define  V_PCIE_XCAP_CV                          0x02
#define  V_PCIE_CLIST_CID                        0x10


//
// L1 Sub-States Extended Capability Register (CAPID:001Eh)
//
#define V_PCIE_EX_L1S_CID                        0x001E ///< Capability ID
#define B_PCIE_EX_L1SCAP_AL1SS                   BIT3 ///< ASPM L1.1 supported
#define B_PCIE_EX_L1SCAP_AL12S                   BIT2 ///< ASPM L1.2 supported
#define B_PCIE_EX_L1SCAP_PPL11S                  BIT1 ///< PCI-PM L1.1 supported
#define B_PCIE_EX_L1SCAP_PPL12S                  BIT0 ///< PCI-PM L1.2 supported
#define R_PCIE_EX_L1SCTL1_OFFSET                 0x08 ///< L1 Sub-States Control 1

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


//
// PCI Express Extended Capability Header
//
#define R_PCIE_CFG_EXCAP_OFFSET                   0x100

#endif
