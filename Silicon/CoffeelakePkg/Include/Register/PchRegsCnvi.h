/** @file
  Register names for CNVi

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

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_CNVI_H_
#define _PCH_REGS_CNVI_H_

//
//  CNVi WiFi Registers (D20:F3)
//
#define PCI_DEVICE_NUMBER_PCH_CNVI_WIFI            20
#define PCI_FUNCTION_NUMBER_PCH_CNVI_WIFI          3
#define V_CNVI_CFG_WIFI_VENDOR_ID                  V_PCH_INTEL_VENDOR_ID
#define R_CNVI_CFG_WIFI_GIO_DEV_CAP                0x44      ///< Device Capabilities
#define R_CNVI_CFG_WIFI_GIO_DEV_CTRL               0x48      ///< Device Control
#define R_CNVI_CFG_WIFI_GIO_DEV_CAP_2              0x64      ///< Device Capabilities 2
#define B_CNVI_CFG_WIFI_GIO_DEV_CAP_2_LTR_MEC_SUP  BIT11     ///< LTR Mechanism Supported
#define R_CNVI_CFG_WIFI_GIO_DEV_2                  0x68      ///< Device Control 2
#define B_CNVI_CFG_WIFI_GIO_DEV_2_LTR_MEC_EN       BIT10     ///< LTR Mechanism Enable
#define R_CNVI_CFG_WIFI_PMCSR                      0xCC      ///< Power Management Status and Control Register

//
// Private registers description for CNVi WiFi
//
#define R_CNVI_PCR_WIFI_WIDGET_CONTROL           0x8114  ///< Widget Control
#define B_CNVI_PCR_WIFI_WIDGET_CONTROL_MEM_INT   0xFF    ///< IRQ number

#endif
