/** @file
  Register names for CPU registers

  <b>Conventions</b>
  - Definitions beginning with "MSR_" are MSRs
  - Definitions beginning with "R_" are registers
  - Definitions beginning with "B_" are bits within registers
  - Definitions beginning with "V_" are meaningful values of bits within the registers
  - Definitions beginning with "S_" are register sizes
  - Definitions beginning with "N_" are the bit position

  Copyright (c) 2004 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _CPU_REGS_H_
#define _CPU_REGS_H_

#define MSR_BOOT_GUARD_SACM_INFO                                      0x0000013A
#define B_BOOT_GUARD_SACM_INFO_NEM_ENABLED                            BIT0

#define CACHE_WRITEPROTECTED            5

#define MSR_CACHE_VARIABLE_MTRR_BASE                                  0x200
#define B_CACHE_MTRR_VALID                                            BIT11

#endif
