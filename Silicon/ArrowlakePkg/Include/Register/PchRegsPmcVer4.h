/** @file
  Register names for Ver4 PCH PMC device

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_PMC_ADL_H_
#define _PCH_REGS_PMC_ADL_H_


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

#endif // _PCH_REGS_PMC_ADL_H_
