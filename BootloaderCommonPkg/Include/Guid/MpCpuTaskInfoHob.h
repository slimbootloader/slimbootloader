/** @file
  This file defines the hob structure for the AP task.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __MP_CPU_TASK_INFO_H__
#define __MP_CPU_TASK_INFO_H__


///
/// Boot loader MP CPU task Information GUID
///
extern EFI_GUID gLoaderMpCpuTaskInfoGuid;

typedef enum {
  EnumCpuBusy = 0,
  EnumCpuReady,
  EnumCpuStart,
  EnumCpuEnd,
} CPU_STATE;

typedef UINT64 (*CPU_TASK_PROC)          (UINT64 Arg);

typedef struct {
  // Refer CPU_STATE
  UINT8           State;

  UINT8           Reserved[7];

  // CPU task function CPU_TASK_PROC
  UINT64          CProcedure;

  // Argument for the CPU task function
  UINT64          Argument;

  // The return result when CPU task function complete
  UINT64          Result;
} CPU_TASK;

typedef struct {
  UINT32           CpuCount;
  CPU_TASK         CpuTask[0];
} SYS_CPU_TASK;

typedef struct {
  SYS_CPU_TASK     *SysCpuTask;
} SYS_CPU_TASK_HOB;

#endif
