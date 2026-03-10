/** @file
  Register names for PCH PCI-E root port devices

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
#ifndef _PCH_REGS_PCIE_RP_H_
#define _PCH_REGS_PCIE_RP_H_

#define R_PCH_PCIE_CFG_CLIST                          0x40
#define R_PCH_PCIE_CFG_LCAP                           (R_PCH_PCIE_CFG_CLIST + R_PCIE_LCAP_OFFSET)
#define N_PCH_PCIE_CFG_LCAP_PN                        24
#define R_PCH_PCIE_CFG_MPC                            0xD8
#define R_PCH_PCIE_CFG_SMSCS                          0xDC
#define R_PCH_PCIE_CFG_SPR                            0xE0
#define R_PCH_PCIE_CFG_RPPGEN                         0xE2

#define R_PCH_PCIE_CFG_LTROVR                         0x400
#define B_PCH_PCIE_CFG_LTROVR_LTRNSROVR               BIT31 ///< LTR Non-Snoop Requirement Bit Override
#define B_PCH_PCIE_CFG_LTROVR_LTRSROVR                BIT15 ///< LTR Snoop Requirement Bit Override

#define R_PCH_PCIE_CFG_LTROVR2                        0x404
#define B_PCH_PCIE_CFG_LTROVR2_FORCE_OVERRIDE         BIT3 ///< LTR Force Override Enable
#define B_PCH_PCIE_CFG_LTROVR2_LOCK                   BIT2 ///< LTR Override Lock
#define B_PCH_PCIE_CFG_LTROVR2_LTRNSOVREN             BIT1 ///< LTR Non-Snoop Override Enable
#define B_PCH_PCIE_CFG_LTROVR2_LTRSOVREN              BIT0 ///< LTR Snoop Override Enable

#define R_PCH_PCIE_CFG_TNPT                           0x418 ///< Thermal and Power Throttling
#define B_PCH_PCIE_CFG_TNPT_DRXLTE                    BIT1
#define B_PCH_PCIE_CFG_TNPT_DTXLTE                    BIT0
#define R_PCH_PCIE_CFG_PCIEPMECTL                     0x420
#define B_PCH_PCIE_CFG_PCIEPMECTL_DLSULPPGE           BIT30
#define B_PCH_PCIE_CFG_PCIEPMECTL_L1LE                BIT17
#define B_PCH_PCIE_CFG_PCIEPMECTL_L1FSOE              BIT0

//
// PCIE PCRs (PID:SPA SPB SPC SPD SPE SPF)
//
#define R_SPX_SIP16_PCR_PCD                   0x3E00                  ///< Port configuration and disable
#define B_SPX_PCR_PCD_RP1FN                   (BIT2 | BIT1 | BIT0)    ///< Port 1 Function Number
#define S_SPX_PCR_PCD_RP_FIELD                4                       ///< 4 bits for each RP FN

//
// Number of PCIe ports per PCIe controller
//
#define PCH_PCIE_CONTROLLER_PORTS                     4u
#endif
