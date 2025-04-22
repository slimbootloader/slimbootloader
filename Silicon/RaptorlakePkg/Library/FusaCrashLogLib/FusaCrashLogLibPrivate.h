/** @file
  This file contains the CrashLog feature support library private definitions.

  Copyright (c) 2022 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Uefi.h>
#include <Base.h>
#include <PchAccess.h>
#include <PiPei.h>
#include <Register/SaRegsHostBridge.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Acpi.h>
#include <Guid/Cper.h>
#include <Library/IoLib.h>
#include <Library/PchPciBdfLib.h>
#include <Library/PciLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/CrashLogLib.h>

#define CRASHLOG_SIZE_DEBUG_PURPOSE       0x030           // It will help to verify the extracted binary from BERT by matching the binary content

#define EFI_ACPI_BERT_OEM_ID              "INTEL "        // OEMID 6 bytes long
#define EFI_ACPI_BERT_OEM_TABLE_ID        SIGNATURE_64('E','D','K','2',' ',' ',' ',' ') // OEM table id 8 bytes long
#define EFI_ACPI_BERT_OEM_REVISION        0x00000001
#define EFI_ACPI_BERT_CREATOR_ID          SIGNATURE_32('I','N','T','L')
#define EFI_ACPI_BERT_CREATOR_REVISION    0x00000001

#define EFI_2_7_FIRMWARE_ERROR_RECORD_REVISION        2  // Header Revision. Will be removed when Cper.h has defined it.

#define CRASHLOG_RECORDID_GUID  \
{ 0x8f87f311, 0xc998, 0x4d9e, { 0xa0, 0xc4, 0x60, 0x65, 0x51, 0x8c, 0x4f, 0x6d } }

#define TELEMETRY_RECORDID_GUID  \
{ 0x26d769a7, 0xc31a, 0x43d0, { 0x93, 0x78, 0x3c, 0x6c, 0x87, 0x2e, 0xea, 0x4d } }

#define TRACE_RECORDID_GUID  \
{ 0x024508e0, 0xd564, 0x42ed, { 0xb2, 0x36, 0x58, 0x0d, 0x54, 0x2b, 0xc9, 0xd6 } }

#pragma pack(push,1)

typedef struct {
  EFI_ACPI_6_4_GENERIC_ERROR_DATA_ENTRY_STRUCTURE   GenericErrorDataEntry;
  EFI_FIRMWARE_ERROR_DATA                           EfiFirmwareErrorData;
} FIRMWARE_GENERIC_ERROR;

#pragma pack(pop)

static EFI_GUID mCrashLog_RecordId_Guid     = CRASHLOG_RECORDID_GUID;
static EFI_GUID mTelemetry_RecordId_Guid    = TELEMETRY_RECORDID_GUID;
static EFI_GUID mTrace_RecordId_Guid        = TRACE_RECORDID_GUID;
