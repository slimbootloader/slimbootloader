/** @file
  Register names for PCIe SIP specific registers

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PCIE_SIP_RP_REGS_H_
#define _PCIE_SIP_RP_REGS_H_

#include <PcieRegs.h>
#define R_PCIE_CFG_CLIST                              0x40
#define R_PCIE_CFG_LCAP                               (R_PCIE_CFG_CLIST + R_PCIE_LCAP_OFFSET)
#define N_PCIE_CFG_LCAP_PN                            24
#define R_PCIE_CFG_LSTS                               (R_PCIE_CFG_CLIST + R_PCIE_LSTS_OFFSET)
#define R_PCIE_CFG_CCFG                               0xD0
#define B_PCIE_CFG_CCFG_UNRS                          (BIT6 | BIT5 | BIT4)
#define N_PCIE_CFG_CCFG_UNRS                          4
#define R_PCIE_CFG_LPCR                               0xC8C
#define B_PCIE_CFG_LPCR_LTRCFGLOCK                    BIT16
#endif
