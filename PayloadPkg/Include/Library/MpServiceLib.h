/** @file

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _MP_SERVICE_LIB_H_
#define _MP_SERVICE_LIB_H_

#include <Protocol/MpService.h>
#include <Guid/MpCpuTaskInfoHob.h>

/**
  Get processor info pointer for all CPUs.

  @retval    Pointer to SYS_CPU_INFO structure.

**/
SYS_CPU_INFO *
EFIAPI
GetCpuInfo (
  VOID
  );

/**
  Get processor task pointer for all CPUs.

  @retval    Pointer to SYS_CPU_TASK structure.

**/
SYS_CPU_TASK *
EFIAPI
GetCpuTask (
  VOID
  );

/**
  Dump processor task running state

  This is a debug function to dump current task running state for each
  processor.

**/
VOID
EFIAPI
DumpCpuTask (
  VOID
  );

/**
  Run a task function for a specific processor.

  @param[in]  Index       CPU index
  @param[in]  TaskFunc    Task function pointer
  @param[in]  Argument    Argument for the task function

  @retval EFI_NOT_FOUND           CPU task hob does not exist.
  @retval EFI_INVALID_PARAMETER   Invalid Index parameter.
  @retval EFI_NOT_READY           CPU state is not ready for new task yet.
  @retval EFI_SUCCESS             CPU accepted the new task successfully.

**/
EFI_STATUS
EFIAPI
RunCpuTask (
  IN  UINT32         Index,
  IN  CPU_TASK_FUNC  TaskFunc,
  IN  UINT64         Argument
  );

#endif
