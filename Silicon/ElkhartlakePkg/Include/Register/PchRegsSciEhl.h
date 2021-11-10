/** @file
  Register names for Sycamore Island (SCI)

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

  Copyright (c) 2014 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_SCI_H_
#define _PCH_REGS_SCI_H_

//
// SCI Registers
//
// EHL-PCH D26:F3
#define PCI_DEVICE_NUMBER_PCH_SCI             26
#define PCI_FUNCTION_NUMBER_PCH_SCI           3


// PCI Configuration Space Registers
#define R_SCI_CFG_DEVICE_ID                   0x0

// SCI boot status success
#define V_SCI_MEM_BOOT_STATUS         0x42544F4B    // "BTOK"

//
// SCI MMIO space registers
//
#define R_SCI_MEM_SCRATCHPAD          0x0

#endif
