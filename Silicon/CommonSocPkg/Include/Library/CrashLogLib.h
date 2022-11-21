/** @file

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __CRASH_LOG_LIB_H__
#define __CRASH_LOG_LIB_H__

//
// Structure of CrashLog Hob data
//
typedef struct {
  UINT64  AllocateAddress;
  UINT32  Size;
} CRASHLOG_HOB;

//
// Structure of CPU CrashLog Hob data
//
typedef struct {
  CRASHLOG_HOB  Main;
  CRASHLOG_HOB  Trace;
  CRASHLOG_HOB  Telemetry;
} CPU_CRASHLOG_HOB;

/**
  Collect Crashlog Data for creating BERT table later in boot.

**/
VOID
EFIAPI
GetCrashLogData (
  VOID
);

/**
  Update BERT Table with Crash Log data

  @param Bert - ACPI BERT table pointer
  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
UpdateCrashLogBertTable (
  EFI_ACPI_6_4_BOOT_ERROR_RECORD_TABLE_HEADER *Bert
);

#endif