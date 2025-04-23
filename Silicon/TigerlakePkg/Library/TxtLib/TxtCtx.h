/** @file
  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _TXT_CTX_H_
#define _TXT_CTX_H_

#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <TxtInfoHob.h>
#include <Library/TimerLib.h>
#include <Register/Cpuid.h>


///
/// The following switch is used in EfiScriptLib.h file. If enabled -
/// forces linking to EfiScriptLib library
///
#define BASE_ADDR_MASK                      0xFFFFF000
#define TEST_PATTERN                        0x5A5A5A5A5A5A5A5A
#define MP_TIMEOUT_FOR_STARTUP_ALL_APS      0           ///< Set 0 for BSP always wait for APs

///
/// Chispet register
///
#define TXT_OPT_IN_VMX_AND_SMX_MSR_VALUE    0xFF03

#define LOCAL_APIC_THERMAL_DEF              0x330

#pragma pack(push, 1)
typedef struct _TXT_LIB_CONTEXT_ {
  EFI_HANDLE               ImageHandle;
  UINTN                    CpuCount;
  TXT_INFO_DATA            *TxtInfoData;
} TXT_LIB_CONTEXT;
#pragma pack(pop)

/**
  Sets up the system and then launches the TXT BIOS ACM to run the function
  requested by AcmFunction.

  @param[in]  AcmBase      Base address of BIOS ACM location
  @param[in]  Funct        Function number of BIOS ACM to be executed

  @retval     EFI_SUCCESS  Always.
**/
VOID
LaunchBiosAcm (
  IN UINT64 AcmBase,
  IN UINT64 Funct
  );

#endif

