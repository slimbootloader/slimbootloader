/** @file
  Register names for IBL PMC device (ACPI PM Service)

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
    Register that is specific to -LP denoted by "_PCH_" in component name.
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

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PMC_REGS_IBL_H_
#define _PMC_REGS_IBL_H_

//
// PWRM Registers
//
#define V_IBL_PCH_PMC_PWRM_GPIO_CFG_GPP_C     0x0
#define V_IBL_PCH_PMC_PWRM_GPIO_CFG_GPP_D     0x1
#define V_IBL_PCH_PMC_PWRM_GPIO_CFG_GPP_E     0x2
#define V_IBL_PCH_PMC_PWRM_VGPIO_CFG_00_31    0x3
#define V_IBL_PCH_PMC_PWRM_VGPIO_CFG_31_63    0x4
#define V_IBL_PCH_PMC_PWRM_VGPIO_CFG_64_95    0x5

#endif // _PMC_REGS_IBL_H_
