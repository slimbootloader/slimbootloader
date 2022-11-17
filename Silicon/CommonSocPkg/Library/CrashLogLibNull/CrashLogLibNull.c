/** @file
  This file contains the CrashLog feature support library template.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Base.h>
#include <Uefi.h>
#include <IndustryStandard/Acpi.h>
#include <Library/CrashLogLib.h>

/**
  Collect CPU CrashLog data from Telemetry SRAM and store in ACPI BERT Table .

**/
VOID
EFIAPI
GetCrashLogData (
  VOID
)
{
  return;
}

/**
  Update BERT Table with Crash Log Telemetry SRAM data

  @retval  EFI_SUCCESS:              Driver initialized successfully
           EFI_LOAD_ERROR:           Failed to Initialize or to Load the driver
           EFI_OUT_OF_RESOURCES:     Could not allocate needed resources
**/
EFI_STATUS
EFIAPI
UpdateCrashLogBertTable (
  EFI_ACPI_6_4_BOOT_ERROR_RECORD_TABLE_HEADER *Bert
)
{
  return EFI_UNSUPPORTED;
}
