/** @file
  Register names for TGL PCH LPC/eSPI device

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

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_LPC_TGL_H_
#define _PCH_REGS_LPC_TGL_H_

//
// PCI to LPC Bridge Registers (D31:F0)
//
#define PCI_DEVICE_NUMBER_PCH_LPC       31
#define PCI_FUNCTION_NUMBER_PCH_LPC     0

#define V_LPC_CFG_DID_TGL_H                       0x4380
#define V_LPC_CFG_DID_TGL_LP                      0xA080


//
// TGL PCH-LP Device IDs
//
#define V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_0           0xA080        ///< PCH LP Mobile SKU
#define V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_1           0xA081        ///< PCH LP Mobile U Super SKU (SSKU)
#define V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_2           0xA082        ///< PCH LP Mobile U Premium
#define V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_3           0xA083        ///< PCH LP Mobile U Base/Mainstream
#define V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_4           0xA084        ///< PCH LP Mobile SKU
#define V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_5           0xA085        ///< PCH LP Mobile SKU
#define V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_6           0xA086        ///< PCH LP Mobile Y Super SKU (SSKU)
#define V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_7           0xA087        ///< PCH LP Mobile Y Premium


#define B_LPC_CFG_DID                             0xFFE0

#define R_LPC_CFG_IOD                             0x80
#define N_LPC_CFG_IOD_COMB                        4
#define V_LPC_CFG_IOD_COMB_3E8                    7
#define N_LPC_CFG_IOD_COMA                        0
#define V_LPC_CFG_IOD_COMA_3F8                    0
#define R_LPC_CFG_IOE                             0x82
#define B_LPC_CFG_IOE_ME1                         BIT11           ///< Microcontroller Enable #1, Enables decoding of I/O locations 62h and 66h to LPC.
#define B_LPC_CFG_IOE_CBE                         BIT1            ///< Com Port B Enable, Enables decoding of the COMB range to LPC. Range is selected LIOD.CB.
#define B_LPC_CFG_IOE_CAE                         BIT0            ///< Com Port A Enable, Enables decoding of the COMA range to LPC. Range is selected LIOD.CA.
#define R_LPC_CFG_GEN1_DEC                        0x84
#define B_LPC_CFG_GENX_DEC_IODRA                  0x00FC0000
#define B_LPC_CFG_GENX_DEC_IOBAR                  0x0000FFFC
#define B_LPC_CFG_GENX_DEC_EN                     0x00000001
#define R_LPC_CFG_LGMR                            0x98
#define B_LPC_CFG_LGMR_MA                         0xFFFF0000
#define B_LPC_CFG_LGMR_LMRD_EN                    BIT0

#define R_LPC_CFG_BDE                             0xD8                          ///< BIOS decode enable


#define R_LPC_CFG_BC                              0xDC            ///< Bios Control

#define R_ESPI_CFG_PCBC                           0xDC            ///< Peripheral Channel BIOS Control
#define B_ESPI_CFG_PCBC_ESPI_EN                   BIT2            ///< eSPI Enable Pin Strap

//
// Private Configuration Register
// RTC PCRs (PID:RTC)
//
#define R_RTC_PCR_BUC                         0x3414               ///< Backed Up Control


#endif
