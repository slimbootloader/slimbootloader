/** @file
  Register names for Ver2 PCH PMC device

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_PMC_TGL_H_
#define _PCH_REGS_PMC_TGL_H_

//
// PWRM Registers
//
#define V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_B     0x0
#define V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_T     0x1
#define V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_A     0x2
#define V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_R     0x3
#define V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPD       0x4
#define V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_S     0x5
#define V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_H     0x6
#define V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_D     0x7
#define V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_U     0x8
#define V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_VGPIO     0x9
#define V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_F     0xA
#define V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_C     0xB
#define V_TGL_PCH_LP_PMC_PWRM_GPIO_CFG_GPP_E     0xC

/*
Following group is used only for PCH_N
Other GPIO Group Defines are same
Hence change in naming convention only for
V_GPIO_VER2_PCH_N_GPIO_PCR_MISCCFG_GPE0_GPP_X
*/
#define V_ADL_PCH_N_PMC_PWRM_GPIO_CFG_GPP_I     0x6
#define V_ADL_PCH_N_PMC_PWRM_GPIO_CFG_GPP_H     0x7
#define V_ADL_PCH_N_PMC_PWRM_GPIO_CFG_GPP_D     0x8

#endif // _PCH_REGS_PMC_TGL_H_
