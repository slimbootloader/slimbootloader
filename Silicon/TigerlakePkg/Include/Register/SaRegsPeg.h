/** @file
  Register names for PEG block
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
#ifndef _SA_REGS_PEG_H_
#define _SA_REGS_PEG_H_

//
// Device 1 Memory Mapped IO Register Offset Equates
//
#define SA_PEG_BUS_NUM     0x00
#define SA_PEG_DEV_NUM     0x01
#define SA_PEG0_DEV_NUM    SA_PEG_DEV_NUM
#define SA_PEG0_FUN_NUM    0x00
#define SA_PEG1_DEV_NUM    SA_PEG_DEV_NUM
#define SA_PEG1_FUN_NUM    0x01
#define SA_PEG2_DEV_NUM    SA_PEG_DEV_NUM
#define SA_PEG2_FUN_NUM    0x02
#define SA_PEG3_DEV_NUM    0x06
#define SA_PEG3_FUN_NUM    0x00

#define V_SA_PEG_VID       0x8086

#define V_PH3_FS_CR_OVR       0x3E
#define B_PH3_FS_CR_OVR_EN    BIT8
#define V_PH3_LF_CR_OVR       0x14
#define B_PH3_LF_CR_OVR_EN    BIT16

//
// Temporary Device & Function Number used for Switchable Graphics DGPU
//
#define SA_TEMP_DGPU_DEV   0x00
#define SA_TEMP_DGPU_FUN   0x00

//
// SA PCI Express* Port configuration
//
#define SA_PEG_MAX_FUN     0x04
#define SA_PEG_MAX_LANE    0x14
#define SA_PEG_MAX_BUNDLE  0x0A

//
// Silicon and SKU- specific MAX defines
//
#define SA_PEG_CNL_H_MAX_FUN           SA_PEG_MAX_FUN      // CNL-H- SKU supports 4 controllers with 20 PEG lanes and 10 bundles
#define SA_PEG_CNL_H_MAX_LANE          SA_PEG_MAX_LANE
#define SA_PEG_CNL_H_MAX_BUNDLE        SA_PEG_MAX_BUNDLE
#define SA_PEG_NON_CNL_H_MAX_FUN       0x03                // All non-CNL-H- SKU supports 3 controllers with 16 PEG lanes and 8 bundles
#define SA_PEG_NON_CNL_H_MAX_LANE      0x10
#define SA_PEG_NON_CNL_H_MAX_BUNDLE    0x08


#endif
