/** @file

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/MpServiceLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/BootloaderCommonLib.h>

STATIC  SYS_CPU_TASK        *mSysCpuTask;
STATIC  SYS_CPU_INFO        *mSysCpuInfo;

/**
  Initial global data used by MP service library.

  @retval TRUE    Global data was successfully initialized.
          FALSE   Failed to initial global data.

**/
STATIC
BOOLEAN
InitCpuGlobal (
  VOID
)
{
  UINT8               *GuidHob;
  SYS_CPU_TASK_HOB    *SysCpuTaskHob;

  GuidHob = GetNextGuidHob (&gLoaderMpCpuTaskInfoGuid, GetHobListPtr());
  if (GuidHob != NULL) {
    SysCpuTaskHob = (SYS_CPU_TASK_HOB *) GET_GUID_HOB_DATA (GuidHob);
    mSysCpuTask   = (SYS_CPU_TASK *)(UINTN)SysCpuTaskHob->SysCpuTask;
    mSysCpuInfo   = (SYS_CPU_INFO *)(UINTN)SysCpuTaskHob->SysCpuInfo;
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Get processor info pointer for all CPUs.

  @retval    Pointer to SYS_CPU_INFO structure.

**/
SYS_CPU_INFO *
EFIAPI
GetCpuInfo (
  VOID
  )
{
  if (mSysCpuInfo == NULL) {
    InitCpuGlobal ();
  }

  return mSysCpuInfo;
}

/**
  Get processor task pointer for all CPUs.

  @retval    Pointer to SYS_CPU_TASK structure.

**/
SYS_CPU_TASK *
EFIAPI
GetCpuTask (
  VOID
  )
{
  if (mSysCpuInfo == NULL) {
    InitCpuGlobal ();
  }

  return mSysCpuTask;
}

/**
  Dump processor task running state

  This is a debug function to dump current task running state for each
  processor.

**/
VOID
EFIAPI
DumpCpuTask (
  VOID
  )
{
  UINT32            Index;
  SYS_CPU_TASK     *SysCpuTask;

  SysCpuTask = GetCpuTask ();
  if (SysCpuTask == NULL) {
    return;
  }

  for (Index = 0; Index < SysCpuTask->CpuCount; Index++) {
    DEBUG ((DEBUG_INFO, "CPU%02X: %02X %010lX %010lX %010lX\n",
            Index,
            SysCpuTask->CpuTask[Index].State,
            SysCpuTask->CpuTask[Index].TaskFunc,
            SysCpuTask->CpuTask[Index].Argument,
            SysCpuTask->CpuTask[Index].Result));
  }
  DEBUG ((DEBUG_INFO, "\n"));
}

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
  )
{
  SYS_CPU_TASK     *SysCpuTask;

  SysCpuTask = GetCpuTask ();
  if (SysCpuTask == NULL) {
    return EFI_NOT_FOUND;
  }

  if ((Index >= SysCpuTask->CpuCount) || (Index == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  if (SysCpuTask->CpuTask[Index].State != EnumCpuReady) {
    return EFI_NOT_READY;
  }

  SysCpuTask->CpuTask[Index].TaskFunc   = (UINT64)(UINTN)TaskFunc;
  SysCpuTask->CpuTask[Index].Argument   = Argument;
  SysCpuTask->CpuTask[Index].State      = EnumCpuStart;

  return EFI_SUCCESS;
}

