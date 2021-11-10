/** @file
  Register names for CPU registers

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _CPU_REGS_H_
#define _CPU_REGS_H_

#define EFI_CACHE_WRITEPROTECTED               5

#define MSR_CACHE_VARIABLE_MTRR_BASE                                  0x200
#define B_CACHE_MTRR_VALID                                            BIT11

#define MSR_BOOT_GUARD_SACM_INFO                                      0x13A
#define B_BOOT_GUARD_SACM_INFO_NEM_ENABLED                            BIT0

#define MSR_PLATFORM_INFO                                             0x000000CE


#define MSR_IA32_MTRRCAP                                              0xFE
#define MSR_PRMRR_VALID_CONFIG                                        0x1FB
#define MSR_IA32_FEATURE_CONTROL                                      0x3A
#define MSR_PRMRR_PHYS_BASE                                           0x1F4
#define MSR_PRMRR_PHYS_MASK                                           0x1F5

#endif
