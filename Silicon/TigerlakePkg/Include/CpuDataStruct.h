/** @file
  This file declares various data structures used in CPU reference code.

  Copyright (c) 2004 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _CPU_DATA_STRUCT_H
#define _CPU_DATA_STRUCT_H

#define TIGERLAKE_ULT_ULX_MAX_ROOT_PORT     1
#define TIGERLAKE_DT_HALO_MAX_ROOT_PORT     4

typedef UINT32 CPU_STATE_CHANGE_CAUSE;

///
/// Structure to hold the return value of AsmCpuid instruction
///
typedef struct {
  UINT32 RegEax; ///< Value of EAX.
  UINT32 RegEbx; ///< Value of EBX.
  UINT32 RegEcx; ///< Value of ECX.
  UINT32 RegEdx; ///< Value of EDX.
} EFI_CPUID_REGISTER;

#pragma pack(1)
///
/// MSR_REGISTER definition as a Union of QWORDS, DWORDS and BYTES
///
typedef union _MSR_REGISTER {
  UINT64  Qword;       ///< MSR value in 64 bit QWORD.

  ///
  /// MSR value represented in two DWORDS
  ///
  struct {
    UINT32  Low;       ///< Lower DWORD of the 64 bit MSR value.
    UINT32  High;      ///< Higher DWORD of the 64 bit MSR value.
  } Dwords;

  ///
  /// MSR value represented in eight bytes.
  ///
  struct {
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

///
/// Store BIST data for BSP.
///
typedef struct {
  UINT32 ApicId;    ///< APIC ID
  UINT32 Health;    ///< BIST result
} BIST_HOB_DATA;

#pragma pack()

#endif
