/** @file
  Register names for CPU registers

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _CPU_REGS_H_
#define _CPU_REGS_H_

#define EFI_CACHE_UNCACHEABLE                  0
#define EFI_CACHE_WRITECOMBINING               1
#define EFI_CACHE_WRITETHROUGH                 4
#define EFI_CACHE_WRITEPROTECTED               5
#define EFI_CACHE_WRITEBACK                    6

#define MSR_CACHE_VARIABLE_MTRR_BASE                                  0x200
#define B_CACHE_MTRR_VALID                                            BIT11
#define B_CACHE_FIXED_MTRR_VALID                                      BIT10

#define MSR_BOOT_GUARD_SACM_INFO                                      0x13A
#define V_TPM_PRESENT_MASK                                            0x06
#define B_BOOT_GUARD_SACM_INFO_NEM_ENABLED                            BIT0
#define B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS                            BIT3
#define B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT                          BIT5
#define B_BOOT_GUARD_SACM_INFO_VERIFIED_BOOT                          BIT6
#define B_BOOT_GUARD_SACM_INFO_CAPABILITY                             BIT32

#define MSR_PLATFORM_INFO                                             0x000000CE
#define N_PLATFORM_INFO_MIN_RATIO                                     40
#define B_PLATFORM_INFO_RATIO_MASK                                    0xFF
#define N_PLATFORM_INFO_MAX_RATIO                                     8
#define B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL                           BIT35
#define N_MSR_PLATFORM_INFO_CONFIG_TDP_NUM_LEVELS_OFFSET              33
#define V_CONFIG_TDP_NUM_LEVELS_MASK                                  (BIT34 | BIT33)
#define B_PLATFORM_INFO_TDC_TDP_LIMIT                                 BIT29
#define N_PLATFORM_INFO_RATIO_LIMIT                                   28
#define B_PLATFORM_INFO_RATIO_LIMIT                                   BIT28
#define B_PLATFORM_INFO_SAMPLE_PART                                   BIT27
#define B_PLATFORM_INFO_SMM_SAVE_CONTROL                              BIT16
#define N_PLATFORM_INFO_PROG_TCC_ACTIVATION_OFFSET                    30
#define B_PLATFORM_INFO_PROG_TCC_ACTIVATION_OFFSET                    BIT30
#define B_PLATFORM_INFO_TIMED_MWAIT_SUPPORTED                         BIT37
#define B_PLATFORM_INFO_EDRAM_EN                                      BIT57
#define B_BOOT_GUARD_ACM_ERRORCODE_MASK                               0x00007FF0

#define R_CPU_BOOT_GUARD_ACM_STATUS                                   0x328
#define R_CPU_BOOT_GUARD_BOOTSTATUS                                   0xA0

#define TXT_PUBLIC_BASE                                               0xFED30000

#define MSR_IA32_MTRRCAP                                              0xFE
#define MSR_PRMRR_VALID_CONFIG                                        0x1FB
#define MSR_IA32_FEATURE_CONTROL                                      0x3A
#define MSR_PRMRR_PHYS_BASE                                           0x1F4
#define MSR_PRMRR_PHYS_MASK                                           0x1F5

#define MSR_TURBO_RATIO_LIMIT                                         0x000001AD
#define MSR_PACKAGE_POWER_SKU_UNIT                                    0x00000606
#define MSR_PACKAGE_POWER_LIMIT                                       0x00000610

#endif
