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


#define B_PCIE_DCTL2_LTREN                        BIT10 ///< LTR Mechanism Enable


#define  V_PCIE_SVCAP_CID                        0x0D
#define  V_PCIE_MID_CID                          0x05
#define  V_PCIE_XCAP_CV                          0x02
#define  V_PCIE_CLIST_CID                        0x10


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
#endif
