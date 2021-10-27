/** @file
  Register names for System Agent (SA) registers
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

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _SA_REGS_H_
#define _SA_REGS_H_

#include <Register/SaRegsIgd.h>

#define SA_MC_BUS                                   0x00
#define SA_MC_DEV                                   0x00
#define SA_MC_FUN                                   0x00

//
// CometLake CPU Desktop SA Device IDs B0:D0:F0
//
//
// CometLake CPU Halo SA Device IDs B0:D0:F0
//
typedef struct {
  UINT32 RegEax;
  UINT32 RegEbx;
  UINT32 RegEcx;
  UINT32 RegEdx;
} EFI_CPUID_REGISTER;

#define  TSEG                                       0xB8
#define  BGSM                                       0xB4

//  This Register contains the size of physical memory.
#define R_SA_TOM         (0xa0)
#define B_SA_TOM_MASK    (0x7ffff00000ULL)

//  This 64 bit register defines the Top of Upper Usable DRAM.
#define R_SA_TOUUD       (0xa8)
#define B_SA_TOUUD_MASK  (0x7ffff00000ULL)


//  This register contains the Top of low memory address.
#define R_SA_TOLUD       (0xbc)
#define B_SA_TOLUD_MASK  (0xfff00000)

#endif
