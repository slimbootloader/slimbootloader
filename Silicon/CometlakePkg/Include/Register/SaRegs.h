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
#define V_SA_MC_VID                                 0x8086
#define R_SA_MC_DEVICE_ID                           0x02
#define R_SA_MC_CAPID0_B                            0xE8

#define CPUID_VERSION_INFO                          0x01
#define CPUID_FULL_FAMILY_MODEL                     0x0FFF0FF0
#define CPUID_FULL_FAMILY_MODEL_COMETLAKE_ULT_ULX  0x000806E0
#define CPUID_FULL_FAMILY_MODEL_COMETLAKE_DT_HALO  0x000906E0
//
// CometLake CPU Desktop SA Device IDs B0:D0:F0
//
#define V_SA_DEVICE_ID_CML_DT_1        0x9B53  ///< CometLake Desktop (6+2) SA DID
#define V_SA_DEVICE_ID_CML_DT_2        0x9B33  ///< CometLake Desktop (10+2) SA DID
#define V_SA_DEVICE_ID_CML_DT_3        0x9B43  ///< CometLake Desktop (8+2) SA DID
//
// CometLake CPU Halo SA Device IDs B0:D0:F0
//
#define V_SA_DEVICE_ID_CML_HALO_2      0x9B44  ///< CometLake Halo (8+2) SA DID
#define V_SA_DEVICE_ID_CML_HALO_1      0x9B54  ///< CometLake Halo (6+2) SA DID
#define V_SA_DEVICE_ID_CML_HALO_3      0x9B64  ///< CometLake Halo (4+2) SA DID
typedef struct {
  UINT32 RegEax;
  UINT32 RegEbx;
  UINT32 RegEcx;
  UINT32 RegEdx;
} EFI_CPUID_REGISTER;

#define  TSEG                                       0xB8
#define  BGSM                                       0xB4

#endif
