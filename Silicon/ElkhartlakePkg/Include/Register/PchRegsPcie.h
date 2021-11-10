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

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_PCIE_H_
#define _PCH_REGS_PCIE_H_


//
// PCH PCI Express Root Ports (D28:F0..7, D29:F0..7, D27:F0..7)
//
#define PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS     28
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


#define R_PCH_PCIE_CFG_MPC                            0xD8

#define R_PCH_PCIE_CFG_SMSCS                          0xDC

#define R_PCH_PCIE_CFG_SPR                            0xE0


#define R_PCH_PCIE_CFG_RPPGEN                         0xE2


#define R_PCH_PCIE_CFG_EX_ACSECH                      0x140 ///< ACS Extended Capability Header


#define R_PCH_PCIE_CFG_EX_SPEECH                      0x220 ///< Secondary PCI Express Extended Capability Header

#define R_PCH_PCIE_CFG_EX_DPCECH                      0x250 ///< Downstream Port Containment


#define R_PCH_PCIE_CFG_PCIEPMECTL                     0x420


//
// PCIE PCRs (PID:SPA SPB SPC SPD SPE SPF)
//
#define R_SPX_PCR_PCD                         0                       ///< Port configuration and disable
#define B_SPX_PCR_PCD_RP1FN                   (BIT2 | BIT1 | BIT0)    ///< Port 1 Function Number
#define S_SPX_PCR_PCD_RP_FIELD                4                       ///< 4 bits for each RP FN


#endif
