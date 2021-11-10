/** @file
  Register names for PCH LPC/eSPI device

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

  Copyright (c) 1999 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_LPC_H_
#define _PCH_REGS_LPC_H_


//
// PCI to LPC Bridge Registers (D31:F0)
//
#define PCI_DEVICE_NUMBER_PCH_LPC       31
#define PCI_FUNCTION_NUMBER_PCH_LPC     0
#define V_LPC_CFG_DID_EHL_LP            0x4B00

typedef UINT8 PCH_STEPPING;
#define PCH_A0                0x00
#define PCH_STEPPING_MAX      0xFF


#define R_LPC_CFG_IOD                             0x80
#define N_LPC_CFG_IOD_COMB                        4
#define V_LPC_CFG_IOD_COMB_3E8                    7
#define N_LPC_CFG_IOD_COMA                        0
#define V_LPC_CFG_IOD_COMA_3F8                    0
#define R_LPC_CFG_IOE                             0x82
#define B_LPC_CFG_IOE_CBE                         BIT1            ///< Com Port B Enable, Enables decoding of the COMB range to LPC. Range is selected LIOD.CB.
#define B_LPC_CFG_IOE_CAE                         BIT0            ///< Com Port A Enable, Enables decoding of the COMA range to LPC. Range is selected LIOD.CA.


#define R_LPC_CFG_BC                              0xDC            ///< Bios Control

#define R_PCH_LPC_IOD                             0x80
#define R_PCH_LPC_IOE                             0x82
#define R_PCH_LPC_GEN1_DEC                        0x84
#define B_PCH_LPC_GENX_DEC_IODRA                  0x00FC0000
#define B_PCH_LPC_GENX_DEC_IOBAR                  0x0000FFFC
#define B_PCH_LPC_GENX_DEC_EN                     0x00000001
#define R_PCH_ESPI_CS1GIR1                        0xA4

//
// APM Registers
//

#define R_PCH_ESPI_PCBC                           0xDC            ///< Peripheral Channel BIOS Control
#define B_PCH_ESPI_PCBC_ESPI_EN                   BIT2            ///< eSPI Enable Pin Strap


#endif
