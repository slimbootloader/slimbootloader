/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MP_INIT_LIB_INTERNAL_H_
#define _MP_INIT_LIB_INTERNAL_H_

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/LocalApicLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/MpInitLib.h>

#define   AP_BUFFER_ADDRESS        0x38000
#define   AP_BUFFER_SIZE           0x8000

#define   AP_STACK_SIZE_SHIFT_BITS 12
#define   AP_STACK_SIZE            (1<<AP_STACK_SIZE_SHIFT_BITS)
#define   AP_TASK_TIMEOUT_UNIT     15
#define   AP_TASK_TIMEOUT_CNT      1000

#define   SMM_BASE_GAP             0x1000
#define   SMM_BASE_MIN_SIZE        0x10000

#pragma pack(1)
typedef struct {
  UINT16 CSSelector;
  UINT16 DSSelector;
  UINT16 ESSelector;
  UINT16 SSSelector;
  UINT16 FSSelector;
  UINT16 GSSelector;
} MP_BSP_SELECTORS;

typedef struct {
  UINT32            BufferStart;
  UINT8             Gdtr[10];
  UINT8             Idtr[10];
  UINT32            Reserved;
  MP_BSP_SELECTORS  BspSelector;
  UINT32            StackStart;
  UINT32            CProcedure;
  UINT32            SpinLock;
  UINT32            ApCounter;
  UINT32            ApStackSize;
  UINT32            MpDataStruct;
} AP_DATA_STRUCT;

typedef struct {
  UINT8 *RendezvousFunnelAddress;
  UINT32 CodeSize;
  UINT32 MpDataSize;
  UINT32 ProtModeStartOffset;
  UINT32 ProtModeJmpPatchOffset;
} MP_ASSEMBLY_ADDRESS_MAP;

typedef struct {
  UINT32           ApDoneCounter;
  UINT32           SmmRebaseDoneCounter;
  SPIN_LOCK        SpinLock;
} MP_DATA_EXCHANGE_STRUCT;
#pragma pack()

typedef struct {
  UINT32           CpuCount;
  CPU_INFO         CpuInfo[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];
} ALL_CPU_INFO;

typedef struct {
  UINT32           CpuCount;
  CPU_TASK         CpuTask[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];
} ALL_CPU_TASK;

/**
  Assembly function to get the address map of MP.

  @param [out] AddressMap Pointer where the mapping info
                          is loaded.
 **/
VOID
EFIAPI
AsmGetAddressMap (
  OUT MP_ASSEMBLY_ADDRESS_MAP    *AddressMap
  );

/**
  Assembly function to get the BSP.

  @param [out] BspSelectors Pointer where the BspSelector info
                            is loaded.
 **/
VOID
EFIAPI
AsmGetBspSelectors (
  OUT MP_BSP_SELECTORS    *BspSelectors
  );

/**
  Assembly function to do cli and Halt.

 **/
VOID
AsmCliHlt (
  VOID
  );

#endif
