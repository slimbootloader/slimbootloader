/** @file
  Register names for PCH GPIO

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_[generation_name]_" in register/bit names.
  - Registers / bits that are specific to PCH-H denoted by "_H_" in register/bit names.
    Registers / bits that are specific to PCH-LP denoted by "_LP_" in register/bit names.
    e.g., "_PCH_H_", "_PCH_LP_"
    Registers / bits names without _H_ or _LP_ apply for both H and LP.
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without [generation_name] inserted.

  Copyright (c) 2013 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_GPIO_H_
#define _PCH_REGS_GPIO_H_

//
// GPIO native features pins data
//
#define PCH_GPIO_HDA_LINK_NUMBER_OF_PINS     6
#define PCH_GPIO_HDA_DMIC_NUMBER_OF_PINS     2
#define PCH_GPIO_HDA_SSP_NUMBER_OF_PINS      4
#define PCH_GPIO_HDA_SNDW_NUMBER_OF_PINS     2
#define PCH_GPIO_SMBUS_NUMBER_OF_PINS        2
#define PCH_GPIO_CPU_GP_NUMBER_OF_PINS       4
#define PCH_GPIO_DDSP_HPD_NUMBER_OF_PINS     4
#define PCH_GPIO_DDP_NUMBER_OF_INTERFACES    4
#define PCH_GPIO_DDP_NUMBER_OF_PINS          2
#define PCH_GPIO_CNVI_UART_NUMBER_OF_PINS    4
#define PCH_GPIO_CNVI_SSP_NUMBER_OF_PINS     4
#define PCH_GPIO_CNVI_BRI_RGI_NUMBER_OF_PINS 4


///
/// GPIO SMI data used for EFI_SMM_GPI_DISPATCH2_PROTOCOL
///
/// Groups mapped to 2-tier General Purpose Event will all be under
/// one master GPE_111 (0x6F)
///
#define PCH_GPIO_2_TIER_MASTER_GPE_NUMBER  0x6F

#endif
