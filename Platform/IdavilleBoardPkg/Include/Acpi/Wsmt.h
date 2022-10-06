/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef _WSMT_H_
#define _WSMT_H_


#include <IndustryStandard/Acpi.h>


#define ACPI_WSMT_SIGNATURE                       0x544D5357
#define ACPI_WSMT_LENGTH                          40
#define ACPI_OEM_WSMT_REVISION                    0x00000000
#define EFI_ACPI_WSMT_REVISION                    0x00000001
#define WSMT_PROTECTION_FLAG                      (BIT0 | BIT1 | BIT2 )


#pragma pack(1)

typedef union {
  struct {
    //
    //BIT0:
    //If set, expresses that for all synchronous SMM entries,
    //SMM will validate that input and output buffers lie entirely within the expected fixed memory regions.
    //
    UINT32   FixedCommBuffers:1;
    //
    //BIT1: COMM_BUFFER_NESTED_PTR_PROTECTION
    //If set, expresses that for all synchronous SMM entries, SMM will validate that input and
    //output pointers embedded within the fixed communication buffer only refer to address ranges
    //that lie entirely within the expected fixed memory regions.
    //
    UINT32   CommBufferNestedPointerProtection:1;
    //
    //BIT2: SYSTEM_RESOURCE_PROTECTION
    //Firmware setting this bit is an indication that it will not allow reconfiguration of system resources via non-architectural mechanisms.
    //
    UINT32   SystemResourceProtection:1;
    UINT32   Reserved:29;
  } Bits;
  UINT32 Flags;
} EFI_ACPI_WSMT_PROTECTION_FLAGS;

//
//  WSMT ACPI table
//
typedef struct _ACPI_WINDOWS_SMM_SECURITY_MITIGATIONS_TABLE {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  EFI_ACPI_WSMT_PROTECTION_FLAGS          ProtectionFlags;
} ACPI_WINDOWS_SMM_SECURITY_MITIGATIONS_TABLE;

#pragma pack()

#endif //_WSMT_H_
