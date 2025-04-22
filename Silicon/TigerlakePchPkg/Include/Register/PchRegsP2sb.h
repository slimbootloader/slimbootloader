/** @file
  Register names for PCH P2SB device

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

  Copyright (c) 2013 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_P2SB_H_
#define _PCH_REGS_P2SB_H_

//
// PCI to P2SB Bridge Registers (D31:F1)
//
#define PCI_DEVICE_NUMBER_PCH_P2SB                 31
#define PCI_FUNCTION_NUMBER_PCH_P2SB               1


#define R_IO_APIC_MEM_INDEX_OFFSET                 0x00
#define R_IO_APIC_MEM_DATA_OFFSET                  0x10
#define V_P2SB_CFG_IBDF_BUS                        0
#define V_P2SB_CFG_IBDF_DEV                        30
#define V_P2SB_CFG_IBDF_FUNC                       7
#define V_P2SB_CFG_HBDF_BUS                        0
#define V_P2SB_CFG_HBDF_DEV                        30
#define V_P2SB_CFG_HBDF_FUNC                       6

//
// Definition for SBI
//
#define R_P2SB_CFG_SBIADDR                         0xD0
#define R_P2SB_CFG_SBIDATA                         0xD4
#define R_P2SB_CFG_SBISTAT                         0xD8
#define B_P2SB_CFG_SBISTAT_OPCODE                  0xFF00
#define B_P2SB_CFG_SBISTAT_POSTED                  BIT7
#define B_P2SB_CFG_SBISTAT_RESPONSE                0x0006
#define N_P2SB_CFG_SBISTAT_RESPONSE                1
#define B_P2SB_CFG_SBISTAT_INITRDY                 BIT0
#define R_P2SB_CFG_SBIRID                          0xDA
#define R_P2SB_CFG_SBIEXTADDR                      0xDC

//
// Others
//
#define R_P2SB_CFG_E0                              0xE0


#endif
