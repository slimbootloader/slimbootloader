/** @file
  Shared CSME firmware health (PHAT) ACPI table definition.

  Provides the common Platform Health Assessment Table (PHAT) layout with a
  single CSME Firmware Health Data Record, plus a static initializer template,
  so every platform that enables CSME resiliency (PcdCsmeResiliencyEnabled)
  shares one definition instead of copy-pasting it. The record's AmHealthy field
  is filled in at runtime from the current CSME corruption state.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _CSME_HEALTH_PHAT_H_
#define _CSME_HEALTH_PHAT_H_

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Acpi65.h>

//
// PHAT table with one CSME Firmware Health Data Record; AmHealthy updated at runtime.
//
#pragma pack(1)
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER                              Header;
  EFI_ACPI_6_5_PHAT_FIRMWARE_HEALTH_DATA_RECORD_STRUCTURE  CsmeHealthRecord;
} CSME_HEALTH_PHAT_TABLE;
#pragma pack()

//
// Static initializer for a CSME_HEALTH_PHAT_TABLE instance.
// gAdapterInfoCsmeGuid {A30DFF09-56BF-4622-A9E7-399B0A79E7C7}
//
#define CSME_HEALTH_PHAT_TABLE_TEMPLATE                                       \
  {                                                                          \
    {                                                                        \
      EFI_ACPI_6_5_PLATFORM_HEALTH_ASSESSMENT_TABLE_SIGNATURE,               \
      sizeof (CSME_HEALTH_PHAT_TABLE),                                       \
      EFI_ACPI_6_5_PLATFORM_HEALTH_ASSESSMENT_TABLE_REVISION,                \
      0, { 0 }, 0, 0, 0, 0                                                   \
    },                                                                       \
    {                                                                        \
      EFI_ACPI_6_5_PHAT_RECORD_TYPE_FIRMWARE_HEALTH_DATA_RECORD,             \
      sizeof (EFI_ACPI_6_5_PHAT_FIRMWARE_HEALTH_DATA_RECORD_STRUCTURE),      \
      EFI_ACPI_6_5_PHAT_FIRMWARE_HEALTH_DATA_RECORD_REVISION,                \
      0,                                                                     \
      EFI_ACPI_6_5_PHAT_FIRMWARE_HEALTH_DATA_RECORD_UNKNOWN,                 \
      { 0xA30DFF09, 0x56BF, 0x4622, { 0xA9, 0xE7, 0x39, 0x9B, 0x0A, 0x79, 0xE7, 0xC7 } }, \
      0                                                                      \
    }                                                                        \
  }

#endif
