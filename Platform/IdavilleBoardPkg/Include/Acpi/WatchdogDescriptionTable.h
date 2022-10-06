/** @file
  This file describes the contents of ACPI Watchdog Description Table.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _WATCH_DOG_DESCRIPTION_TABLE_H_
#define _WATCH_DOG_DESCRIPTION_TABLE_H_

//
// Include files
//
#include <IndustryStandard/Acpi.h>

//
// Ensure proper structure formats
//
#pragma pack(1)
//
// WDDT structure ACPI 6.2 compliant
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  UINT16                                  SpecVersion;
  UINT16                                  TableVersion;
  UINT16                                  Vid;
  EFI_ACPI_6_2_GENERIC_ADDRESS_STRUCTURE  BaseAddress;
  UINT16                                  TimerMaxCount;
  UINT16                                  TimerMinCount;
  UINT16                                  TimerCountPeriod;
  UINT16                                  Status;
  UINT16                                  Capability;
} EFI_ACPI_WATCH_DOG_DESCRIPTION_TABLE;

//
// "WDDT" Watchdog Description Table signatures
//
#define EFI_ACPI_WDDT_SIGNATURE 0x54444457

#pragma pack()


//
// WDT Status
//
#define EFI_ACPI_WDDT_STATUS_AVAILABLE             0x0001
#define EFI_ACPI_WDDT_STATUS_ACTIVE                0x0002
#define EFI_ACPI_WDDT_STATUS_OWNED_BY_BIOS         0x0000
#define EFI_ACPI_WDDT_STATUS_OWNED_BY_OS           0x0004
#define EFI_ACPI_WDDT_STATUS_USER_RESET_EVENT      0x0800
#define EFI_ACPI_WDDT_STATUS_WDT_EVENT             0x1000
#define EFI_ACPI_WDDT_STATUS_POWER_FAIL_EVENT      0x2000
#define EFI_ACPI_WDDT_STATUS_UNKNOWN_RESET_EVENT   0x4000

//
// WDT Capability
//
#define EFI_ACPI_WDDT_CAPABILITY_AUTO_RESET            0x0001
#define EFI_ACPI_WDDT_CAPABILITY_ALERT_SUPPORT         0x0002
#define EFI_ACPI_WDDT_CAPABILITY_PLATFORM_SHUTDOWN     0x0004
#define EFI_ACPI_WDDT_CAPABILITY_IMMEDIATE_SHUTDOWN    0x0008
#define EFI_ACPI_WDDT_CAPABILITY_BIOS_HANDOFF_SUPPORT  0x0010

#endif
