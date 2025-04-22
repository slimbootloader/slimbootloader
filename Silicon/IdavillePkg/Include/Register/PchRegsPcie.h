/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_REGS_PCIE_CDF_H_
#define _PCH_REGS_PCIE_CDF_H_

//
// Number of PCIe ports per PCIe controller
//
#define PCH_PCIE_CONTROLLER_PORTS                 4u


#define R_PCH_PCIE_CFG_LINKCTL                          0x50
#define R_PCH_PCIE_CFG_LINKSTS                          0x52
#define R_PCH_PCIE_CFG_SLOTSTS                          0x5A
#define R_PCH_PCIE_CFG_ROOTSTS                          0x60


#define R_PCH_PCIE_CFG_RPPMCSR                          0xA8


#define R_PCH_PCIE_CFG_SCICSR                         0xCC


#define R_PCH_PCIE_CFG_ACSCAPHDR                         0x148 ///< ACS Extended Capability Header
#define R_PCH_PCIE_CFG_ACSCAP                           (R_PCH_PCIE_CFG_ACSCAPHDR + R_PCIE_EX_ACSCAPR_OFFSET)


#define R_PCH_PCIE_CFG_SPAD                            0x8C4  // Scratchpad register


#endif // _PCH_REGS_PCIE_CDF_H_
