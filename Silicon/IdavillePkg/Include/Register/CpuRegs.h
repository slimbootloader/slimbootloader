/** @file
  Register names for CPU registers

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _CPU_REGS_H_
#define _CPU_REGS_H_

#include <Register/Intel/ArchitecturalMsr.h>


#define EFI_CACHE_WRITEPROTECTED                                      5
#define   B_EFI_CACHE_MTRR_VALID                                      BIT11

#define MSR_BOOT_GUARD_SACM_INFO                                      0x13A
#define B_BOOT_GUARD_SACM_INFO_NEM_ENABLED                            BIT0

#define MSR_PLATFORM_INFO                                             0x000000CE
#define MSR_TURBO_RATIO_LIMIT                                         0x000001AD
#define MSR_PACKAGE_POWER_SKU_UNIT                                    0x00000606
#define MSR_PACKAGE_POWER_LIMIT                                       0x00000610
#define MSR_TME_CAPABILITIES                                          0x00000981
#define MSR_TME_ACTIVATE                                              0x00000982
#define MSR_IA32_FEATURE_CONTROL                                      0x0000003A

#define R_CPU_BOOT_GUARD_ACM_STATUS                                   0x328
#define R_CPU_BOOT_GUARD_BOOTSTATUS                                   0xA0

#define TXT_PUBLIC_BASE                                               0xFED30000

#pragma pack(1)
///
/// MSR_REGISTER definition as a Union of QWORDS, DWORDS and BYTES
///
typedef union _MSR_REGISTER {
  UINT64  Qword;       ///< MSR value in 64 bit QWORD.

  ///
  /// MSR value represented in two DWORDS
  ///
  struct _DWORDS {
    UINT32  Low;       ///< Lower DWORD of the 64 bit MSR value.
    UINT32  High;      ///< Higher DWORD of the 64 bit MSR value.
  } Dwords;

  ///
  /// MSR value represented in eight bytes.
  ///
  struct _BYTES {
    UINT8 FirstByte;   ///< First byte of the 64 bit MSR value.
    UINT8 SecondByte;  ///< Second byte of the 64 bit MSR value.
    UINT8 ThirdByte;   ///< Third byte of the 64 bit MSR value.
    UINT8 FouthByte;   ///< Fourth byte of the 64 bit MSR value.
    UINT8 FifthByte;   ///< Fifth byte of the 64 bit MSR value.
    UINT8 SixthByte;   ///< Sixth byte of the 64 bit MSR value.
    UINT8 SeventhByte; ///< Seventh byte of the 64 bit MSR value.
    UINT8 EighthByte;  ///< Eigth byte of the 64 bit MSR value.
  } Bytes;
} MSR_REGISTER;
#pragma pack()

#endif
