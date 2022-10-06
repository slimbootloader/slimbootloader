/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __CPU_DATA_STRUCT_H__
#define __CPU_DATA_STRUCT_H__

#define CPUID_VERSION_INFO                      0x01

#define CPUID_EXTENDED_TOPOLOGY                 0x0B
#define CPUID_VIR_PHY_ADDRESS_SIZE              0x80000008

typedef struct {
  UINT32  RegEax;
  UINT32  RegEbx;
  UINT32  RegEcx;
  UINT32  RegEdx;
} EFI_CPUID_REGISTER;

#endif // __CPU_DATA_STRUCT_H__
