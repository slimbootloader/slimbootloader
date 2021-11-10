/** @file
  Register names for PCH private chipset register

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

   Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>
   SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_PCR_H_
#define _PCH_REGS_PCR_H_

///
/// Definition for PCR base address (defined in PchReservedResources.h)
///
//#define PCH_PCR_BASE_ADDRESS            0xFD000000
//#define PCH_PCR_MMIO_SIZE               0x01000000
/**
  Definition for PCR address
  The PCR address is used to the PCR MMIO programming
**/
#define PCH_PCR_ADDRESS(Pid, Offset)    (PCH_PCR_BASE_ADDRESS | ((UINT8)(Pid) << 16) | (UINT16)(Offset))

/**
  Definition for SBI PID
  The PCH_SBI_PID defines the PID for PCR MMIO programming and PCH SBI programming as well.
**/
//CNL
#define  PID_BROADCAST1                        0xFF // @todo CNL PCH, check CNL cspec
#define  PID_BROADCAST2                        0xFE // @todo CNL PCH, check CNL cspec
#define  PID_ICLK                              0xAD
#define  PID_DMI                               0x88
#define  PID_SPD                               0x83
#define  PID_SPC                               0x82
#define  PID_SPB                               0x81
#define  PID_SPA                               0x80
#define  PID_UFSX2                             0x50
#define  PID_RTC_HOST                          0xC3
#define  PID_GPIOCOM0                          0x6E
#define  PID_GPIOCOM1                          0x6D
#define  PID_GPIOCOM2                          0x6C
#define  PID_GPIOCOM3                          0x6B
#define  PID_GPIOCOM4                          0x6A
#define  PID_GPIOCOM5                          0x69
#define  PID_EMMC                              0x51

typedef  UINT8                            PCH_SBI_PID;

#endif
