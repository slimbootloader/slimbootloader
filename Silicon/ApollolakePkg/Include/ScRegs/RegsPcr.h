/** @file
  Register names for SC private chipset register.

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, SC registers are denoted by "_SC_" in register names
  - Registers / bits that are different between SC generations are denoted by
    "_SC_<generation_name>_" in register/bit names.
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SC generation will be just named
    as "_SC_" without <generation_name> inserted.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_REGS_PCR_H_
#define _SC_REGS_PCR_H_

/**
  Definition for PCR address
  The PCR address is used to the PCR MMIO programming
**/

#define SC_PCR_ADDRESS(Pid, Offset)    (SC_PCR_BASE_ADDRESS | ((UINT8) (Pid) << 16) | (UINT16) (Offset))

typedef enum {
  PID0                               = 0xD1,
  PID1                               = 0xD0,
  PID2                               = 0xC6,
  PID3                               = 0xB0,
  PID4                               = 0xB3,
  PID5                               = 0xB4,
  PID6                               = 0xB6,
  PID7                               = 0xA9,
} SC_SBI_PID;

#endif

