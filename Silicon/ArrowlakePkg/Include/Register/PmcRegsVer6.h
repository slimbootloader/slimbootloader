/** @file
  Register names for Ver6 PCH PMC device

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

@par Specification
**/
#ifndef _PCH_REGS_PMC_MTL_H_
#define _PCH_REGS_PMC_MTL_H_

//
// PWRM Registers
//
#define V_MTL_SOC_M_PMC_PWRM_GPIO_CFG_GPP_V      0x0
#define V_MTL_SOC_M_PMC_PWRM_GPIO_CFG_GPP_C      0x1
#define V_MTL_SOC_M_PMC_PWRM_GPIO_CFG_GPP_A      0x2
#define V_MTL_SOC_M_PMC_PWRM_GPIO_CFG_GPP_E      0x3
#define V_MTL_SOC_M_PMC_PWRM_GPIO_CFG_GPP_H      0x4
#define V_MTL_SOC_M_PMC_PWRM_GPIO_CFG_GPP_F      0x5
#define V_MTL_SOC_M_PMC_PWRM_GPIO_CFG_USB        0x6
#define V_MTL_SOC_M_PMC_PWRM_GPIO_CFG_GPP_S      0x7
#define V_MTL_SOC_M_PMC_PWRM_GPIO_CFG_GPP_B      0x8
#define V_MTL_SOC_M_PMC_PWRM_GPIO_CFG_GPP_D      0x9
#define V_MTL_SOC_M_PMC_PWRM_GPIO_CFG_VGPIO      0xA
#define V_MTL_SOC_M_PMC_PWRM_GPIO_CFG_CPU        0xF   // GPE not supported by this group
#define V_MTL_SOC_M_PMC_PWRM_GPIO_SPI_SYS        0xF   // GPE not supported by this group
#define V_MTL_SOC_M_PMC_PWRM_GPIO_JTAG           0xF   // GPE not supported by this group

#define V_MTL_SOC_S_PMC_PWRM_GPIO_CFG_GPP_A      0x0
#define V_MTL_SOC_S_PMC_PWRM_GPIO_CFG_GPP_C      0x1
#define V_MTL_SOC_S_PMC_PWRM_GPIO_CFG_GPP_B      0x2
#define V_MTL_SOC_S_PMC_PWRM_GPIO_CFG_VGPIO_3    0x3
#define V_MTL_SOC_S_PMC_PWRM_GPIO_CFG_VGPIO_4    0x4
#define V_MTL_SOC_S_PMC_PWRM_GPIO_CFG_VGPIO_0    0x5
#define V_MTL_SOC_S_PMC_PWRM_GPIO_CFG_GPP_D      0x7
#define V_MTL_SOC_S_PMC_PWRM_GPIO_CFG_JTAG       0xF   // GPE not supported by this group

#endif // _PCH_REGS_PMC_MTL_H_
