/** @file
  Register names for PCH P2SB device

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

  Copyright (c) 2013 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _P2SB_REGS_H_
#define _P2SB_REGS_H_

//
// PCI to P2SB Bridge Registers (D31:F1)
//
#define PCI_DEVICE_NUMBER_PCH_P2SB                 31
#define PCI_FUNCTION_NUMBER_PCH_P2SB               1


#define R_IO_APIC_MEM_INDEX_OFFSET                 0x00
#define R_IO_APIC_MEM_DATA_OFFSET                  0x10
#define V_P2SB_CFG_IBDF_BUS                        0
#define V_P2SB_CFG_IBDF_DEV                        30
#define V_P2SB_CFG_IBDF_FUNC                       7
#define V_P2SB_CFG_HBDF_BUS                        0
#define V_P2SB_CFG_HBDF_DEV                        30
#define V_P2SB_CFG_HBDF_FUNC                       6


#endif
