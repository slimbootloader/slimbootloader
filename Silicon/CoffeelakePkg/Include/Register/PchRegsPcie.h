/** @file
  Register names for PCH PCI-E root port devices

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PCH_REGS_PCIE_H_
#define _PCH_REGS_PCIE_H_

#define PCH_PCIE_CONTROLLER_PORTS                 4u

#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1       28
#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2       29
#define PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3       27

#define R_PCH_PCIE_CFG_MPC                        0xD8
#define R_PCH_PCIE_CFG_SMSCS                      0xDC
#define R_PCH_PCIE_CFG_SPR                        0xE0
#define R_PCH_PCIE_CFG_RPPGEN                     0xE2
#define R_PCH_PCIE_CFG_PCIEPMECTL                 0x420


#define R_SPX_PCR_PCD                             0
#define B_SPX_PCR_PCD_RP1FN                       (BIT2 | BIT1 | BIT0)
#define S_SPX_PCR_PCD_RP_FIELD                    4

#endif
