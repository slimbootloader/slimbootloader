/** @file

  Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _MP_INIT_LIB_H_
#define _MP_INIT_LIB_H_

#include <Guid/MpCpuTaskInfoHob.h>

typedef enum {
  EnumMpInitNull   = 0x00,
  EnumMpInitWakeup = 0x01,
  EnumMpInitRun    = 0x02,
  EnumMpInitDone   = 0x04,
  EnumMpInitFull   = 0xFF
} MP_INIT_PHASE;

typedef VOID   (*PLATFORM_CPU_INIT_HOOK) (UINT32 CpuIndex);


/**
  BSP initialization routine.

**/
VOID
BspInit (
  VOID
  );


/**
  Multiprocessor Initialization.

  @param[in]  Phase       Initialization phase for MP.

  @retval EFI_SUCCESS     MP init has been done successfully for current phase.

**/
EFI_STATUS
EFIAPI
MpInit (
  IN MP_INIT_PHASE  Phase
  );

/**
  Get processor info for all CPUs.

  @retval    Pointer to SYS_CPU_INFO structure.

**/
SYS_CPU_INFO *
EFIAPI
MpGetInfo (
  VOID
  );

/**
  Get processor task state for all CPUs.

  @retval    Pointer to SYS_CPU_TASK structure containing task info.

**/
SYS_CPU_TASK *
EFIAPI
MpGetTask (
  VOID
  );


/**
  Run a task function for a specific processor.

  @param[in]  Index       CPU index
  @param[in]  TaskFunc    Task function pointer
  @param[in]  Argument    Argument for the task function

  @retval EFI_INVALID_PARAMETER   Invalid Index parameter.
  @retval EFI_NOT_READY           CPU state is not ready for new task yet.
  @retval EFI_SUCCESS             CPU accepted the new task successfully.

**/
EFI_STATUS
EFIAPI
MpRunTask (
  IN  UINT32         Index,
  IN  CPU_TASK_FUNC  TaskFunc,
  IN  UINT64         Argument
  );


/**
  Dump MP task state

  This is a debug function to dump current task running state for each
  processor.


**/
VOID
EFIAPI
MpDumpTask (
  VOID
  );

#endif
