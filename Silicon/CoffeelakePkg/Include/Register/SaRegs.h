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

  Copyright (c) 2018 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _SA_REGS_H_
#define _SA_REGS_H_

#include <Register/SaRegsIgd.h>

#define SA_MC_BUS                                   0x00
#define SA_MC_DEV                                   0x00
#define SA_MC_FUN                                   0x00
#define V_SA_MC_VID                                 0x8086
#define R_SA_MC_DEVICE_ID                           0x02
#define R_SA_MCHBAR                                 0x48
#define R_SA_MC_CAPID0_B                            0xE8

#define R_SA_MCHBAR_VTD1_OFFSET                     0x5400  // HW UNIT1 for IGD
#define R_SA_MCHBAR_VTD3_OFFSET                     0x5410  // HW UNIT3 for all other - PEG, USB, SATA etc

#define CPUID_VERSION_INFO                          0x01
#define CPUID_FULL_FAMILY_MODEL                     0x0FFF0FF0
#define CPUID_FULL_FAMILY_MODEL_COFFEELAKE_ULT_ULX  0x000806E0
#define CPUID_FULL_FAMILY_MODEL_COFFEELAKE_DT_HALO  0x000906E0

#define V_SA_DEVICE_ID_CFL_ULT_1                    0x3ED0
#define V_SA_DEVICE_ID_CFL_ULT_2                    0x3ECC
#define V_SA_DEVICE_ID_CFL_ULT_3                    0x3E34
#define V_SA_DEVICE_ID_CFL_ULT_4                    0x3E35
#define V_SA_DEVICE_ID_CFL_ULT_5                    0x3E36
#define V_SA_DEVICE_ID_CFL_ULT_6                    0x3ECC

#define V_SA_DEVICE_ID_CFL_DT_1                     0x3EC2
#define V_SA_DEVICE_ID_CFL_DT_2                     0x3E1F
#define V_SA_DEVICE_ID_CFL_DT_3                     0x3E0F
#define V_SA_DEVICE_ID_CFL_DT_4                     0x3E30

#define V_SA_DEVICE_ID_CFL_HALO_1                   0x3EC4
#define V_SA_DEVICE_ID_CFL_HALO_2                   0x3E10
#define V_SA_DEVICE_ID_CFL_HALO_3                   0x3E20

#define V_SA_DEVICE_ID_CFL_HALO_IOT_1               0x3E12

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

#define R_SA_MC_CAPID0_A_OFFSET (0xE4)

#define R_SA_GGC             (0x50)
#define N_SA_GGC_GMS_OFFSET  (0x8)
#define B_SA_GGC_GMS_MASK    (0xff00)
#define N_SA_GGC_GGMS_OFFSET (0x6)
#define B_SA_GGC_GGMS_MASK   (0xc0)
#define V_SA_GGC_GGMS_8MB    3

#endif
