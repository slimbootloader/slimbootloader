/** @file
  Register names for PCIe SIP specific registers

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

#ifndef _PCIE_SIP_RP_REGS_H_
#define _PCIE_SIP_RP_REGS_H_

#include <PcieRegs.h>


#define N_PCIE_MEM_LPHYCP4_LGCLKSQEXITDBTIMERS 0
#define N_PCIE_MEM_LPHYCP4_OSCCLKSQEXITDBTIMERS 3


#define N_PCIE_MEM_PIPEPDCTL2_L1D1UPUPGPDCTL  4
#define N_PCIE_MEM_PIPEPDCTL2_L1D1PGNOPGPDCTL 8

#define N_PCIE_MEM_PIPEPDCTL3_L1DLOWNOPGPDCTL 0


#define N_PCIE_MEM_PX32EQCFG1_PX32GMEQSMMFLNTL 1
#define N_PCIE_MEM_PX32EQCFG1_PX32GEQTS2IRRC  7


#define R_PCIE_CFG_LCAP                       0x0000004cU      ///< Link Capabilities
#define N_PCIE_CFG_LCAP_PN                    24


#define R_PCIE_CFG_CCFG                       0x000000d0U      ///< Channel Configuration
#define B_PCIE_CFG_CCFG_UNRS                  ( BIT6 | BIT5 | BIT4 ) ///< Upstream Non-Posted Request Size
#define N_PCIE_CFG_CCFG_UNRS                  4


#define B_PCIE_CFG_LTROVR2_LTRSOVREN          BIT0             ///< LTR Snoop Override Enable
#define B_PCIE_CFG_LTROVR2_LTRNSOVREN         BIT1             ///< LTR Non-Snoop Override Enable


#define R_PCIE_CFG_LPCR                       0x00000c8cU      ///< Lock Policy Control Register
#define B_PCIE_CFG_LPCR_LTRCFGLOCK            BIT16            ///< LTR Configuration Lock


#endif
