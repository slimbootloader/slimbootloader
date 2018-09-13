/** @file
  Register names for SC private chipset register

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

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_REGS_GPIO_H_
#define _SC_REGS_GPIO_H_

//
// GPIO Common Private Configuration Registers
//
#define R_PCR_GPIO_PADCFGLOCK_0   0x60
#define R_PCR_GPIO_PADCFGLOCKTX_0 0x64
#define R_PCR_GPIO_PADCFGLOCK_1   0x68
#define R_PCR_GPIO_PADCFGLOCKTX_1 0x6C
#define R_PCR_GPIO_PADCFGLOCK_2   0x70
#define R_PCR_GPIO_PADCFGLOCKTX_2 0x74

#define R_PCR_GPIO_SMI_STS_0      0x140
#define R_PCR_GPIO_SMI_STS_1      0x144
#define R_PCR_GPIO_SMI_STS_2      0x148
#define R_PCR_GPIO_SMI_EN_0       0x150
#define R_PCR_GPIO_SMI_EN_1       0x154
#define R_PCR_GPIO_SMI_EN_2       0x158
#define S_GPIO_GP_SMI_EN          4
#define S_GPIO_GP_SMI_STS         4
#define V_GPIO_NUM_SUPPORTED_GPIS 248

#define B_GPIO_IOSSTATE           (BIT17 | BIT16 | BIT15 | BIT14) ///< IO Standby State
#define N_GPIO_IOSSTATE           14
#define B_GPIO_IOSTERM            (BIT9 | BIT8)                   ///< IO Standby Termination
#define N_GPIO_IOSTERM            8

#endif

