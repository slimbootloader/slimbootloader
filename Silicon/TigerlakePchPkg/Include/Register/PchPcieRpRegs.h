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

  Copyright (c) 1999 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_PCIE_RP_H_
#define _PCH_REGS_PCIE_RP_H_


#define R_PCH_PCIE_CFG_MPC                            0xD8
#define R_PCH_PCIE_CFG_SMSCS                          0xDC
#define R_PCH_PCIE_CFG_SPR                            0xE0
#define R_PCH_PCIE_CFG_RPPGEN                         0xE2


//
//PCI Express Extended Capability Registers
//

#ifndef PCH_ICL
#define R_PCH_PCIE_CFG_EX_ACSECH                      0x140 ///< ACS Extended Capability Header
#else
#define R_PCH_PCIE_CFG_EX_ACSECH                      0x220 ///< ACS Extended Capability Header
#endif


#ifndef PCH_ICL
#define R_PCH_PCIE_CFG_EX_SPEECH                      0x220 ///< Secondary PCI Express Extended Capability Header
#else
#define R_PCH_PCIE_CFG_EX_SPEECH                      0xA30 ///< Secondary PCI Express Extended Capability Header
#endif

#ifndef PCH_ICL
#define R_PCH_PCIE_CFG_EX_DPCECH                      0x250 ///< Downstream Port Containment
#else
#define R_PCH_PCIE_CFG_EX_DPCECH                      0xA00 ///< Downstream Port Containment
#endif


#define R_PCH_PCIE_CFG_PCIEPMECTL                     0x420



//
// PCIE PCRs (PID:SPA SPB SPC SPD SPE SPF)
//
#define R_SPX_PCR_PCD                         0                       ///< Port configuration and disable
#define B_SPX_PCR_PCD_RP1FN                   (BIT2 | BIT1 | BIT0)    ///< Port 1 Function Number
#define B_SPX_PCR_PCD_RP1CH                   BIT3                    ///< Port 1 config hide
#define B_SPX_PCR_PCD_RP2FN                   (BIT6 | BIT5 | BIT4)    ///< Port 2 Function Number
#define B_SPX_PCR_PCD_RP2CH                   BIT7                    ///< Port 2 config hide
#define B_SPX_PCR_PCD_RP3FN                   (BIT10 | BIT9 | BIT8)   ///< Port 3 Function Number
#define B_SPX_PCR_PCD_RP3CH                   BIT11                   ///< Port 3 config hide
#define B_SPX_PCR_PCD_RP4FN                   (BIT14 | BIT13 | BIT12) ///< Port 4 Function Number
#define B_SPX_PCR_PCD_RP4CH                   BIT15                   ///< Port 4 config hide
#define S_SPX_PCR_PCD_RP_FIELD                4                       ///< 4 bits for each RP FN
#define B_SPX_PCR_PCD_P1D                     BIT16                   ///< Port 1 disable
#define B_SPX_PCR_PCD_P2D                     BIT17                   ///< Port 2 disable
#define B_SPX_PCR_PCD_P3D                     BIT18                   ///< Port 3 disable
#define B_SPX_PCR_PCD_P4D                     BIT19                   ///< Port 4 disable
#define B_SPX_PCR_PCD_SRL                     BIT31                   ///< Secured Register Lock


#define R_PCH_PCIE_CFG_MPC                            0xD8
#define R_PCH_PCIE_CFG_SPR                            0xE0
#define R_PCH_PCIE_CFG_RPPGEN                         0xE2
#define R_PCH_PCIE_CFG_PCIEPMECTL                     0x420
#define R_PCH_PCIE_CFG_SMSCS                          0xDC
#endif
