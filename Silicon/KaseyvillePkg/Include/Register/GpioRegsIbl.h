/** @file
  Register names for IBL GPIO
  Conventions:
  Prefixes:
  Definitions beginning with "R_" are registers
  Definitions beginning with "B_" are bits within registers
  Definitions beginning with "V_" are meaningful values within the bits
  Definitions beginning with "S_" are register sizes
  Definitions beginning with "N_" are the bit position
  In general, PCH registers are denoted by "_PCH_" in register names
  Registers / bits that are different between PCH generations are denoted by
  _PCH_[generation_name]_" in register/bit names.
  Registers / bits that are specific to PCH-H denoted by "_H_" in register/bit names.
  Registers / bits that are specific to PCH-LP denoted by "_LP_" in register/bit names.
  e.g., "_PCH_", "_PCH_"
  Registers / bits names without _H_ or _LP_ apply for both H and LP.
  Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
  at the end of the register/bit names
  Registers / bits of new devices introduced in a PCH generation will be just named
  as "_PCH_" without [generation_name] inserted.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_REGS_GPIO_IBL_H_
#define _PCH_REGS_GPIO_IBL_H_

//
// GPIO all communities common offsets
//
#define R_IBL_GPIO_PCR_MISCCFG         0x0008

//
// GPIO Private Configuration Registers
// Group: C
//
#define R_IBL_GPIO_PCR_GPP_C_PADCFGLOCK               0x00000060
#define R_IBL_GPIO_PCR_GPP_C_PADCFGLOCKTX             0x00000064
#define R_IBL_GPIO_PCR_GPP_C_HOSTSW_OWN               0x00000080
#define R_IBL_GPIO_PCR_GPP_C_GPI_IS                   0x00000100
#define R_IBL_GPIO_PCR_GPP_C_GPI_IE                   0x00000110
#define R_IBL_GPIO_PCR_GPP_C_GPI_GPE_STS              0x00000120
#define R_IBL_GPIO_PCR_GPP_C_GPI_GPE_EN               0x00000130
#define R_IBL_GPIO_PCR_GPP_C_SMI_STS                  0x00000140
#define R_IBL_GPIO_PCR_GPP_C_SMI_EN                   0x00000150
#define R_IBL_GPIO_PCR_GPP_C_NMI_STS                  0x00000160
#define R_IBL_GPIO_PCR_GPP_C_NMI_EN                   0x00000170

//
// GPIO Private Configuration Registers
// Group: D
//
#define R_IBL_GPIO_PCR_GPP_D_PADCFGLOCK               0x00000068
#define R_IBL_GPIO_PCR_GPP_D_PADCFGLOCKTX             0x0000006C
#define R_IBL_GPIO_PCR_GPP_D_HOSTSW_OWN               0x00000084
#define R_IBL_GPIO_PCR_GPP_D_GPI_IS                   0x00000104
#define R_IBL_GPIO_PCR_GPP_D_GPI_IE                   0x00000114
#define R_IBL_GPIO_PCR_GPP_D_GPI_GPE_STS              0x00000124
#define R_IBL_GPIO_PCR_GPP_D_GPI_GPE_EN               0x00000134
#define R_IBL_GPIO_PCR_GPP_D_SMI_STS                  0x00000144
#define R_IBL_GPIO_PCR_GPP_D_SMI_EN                   0x00000154
#define R_IBL_GPIO_PCR_GPP_D_NMI_STS                  0x00000164
#define R_IBL_GPIO_PCR_GPP_D_NMI_EN                   0x00000174


#define R_IBL_GPIO_PCR_GPP_C_PADCFG_OFFSET              0x400
#define R_IBL_GPIO_PCR_GPP_D_PADCFG_OFFSET              0x4C0


#endif
