/** @file
  Register names for IPU block
  <b>Conventions</b>:
  - Prefixes:
    - Definitions beginning with "R_" are registers
    - Definitions beginning with "B_" are bits within registers
    - Definitions beginning with "V_" are meaningful values of bits within the registers
    - Definitions beginning with "S_" are register sizes
    - Definitions beginning with "N_" are the bit position
  - In general, SA registers are denoted by "_SA_" in register names
  - Registers / bits that are different between SA generations are denoted by
    "_SA_[generation_name]_" in register/bit names. e.g., "_SA_HSW_"
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SA generation will be just named
    as "_SA_" without [generation_name] inserted.

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _SA_REGS_IPU_H_
#define _SA_REGS_IPU_H_

//
// Device 5 Equates
//
#define SA_IPU_BUS_NUM    0x00
#define SA_IPU_DEV_NUM    0x05
#define SA_IPU_FUN_NUM    0x00

//
// GPIO native features pins data
//
#define SA_GPIO_IMGUCLK_NUMBER_OF_PINS     2
#endif
