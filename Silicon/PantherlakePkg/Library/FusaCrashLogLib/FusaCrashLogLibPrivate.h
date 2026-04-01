/** @file
  This file contains the CrashLog feature support library private definitions.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Uefi.h>
#include <Base.h>
#include <PchAccess.h>
#include <PiPei.h>
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

#define EFI_ACPI_BERT_OEM_ID              "INTEL "        // OEMID 6 bytes long
#define EFI_ACPI_BERT_OEM_TABLE_ID        SIGNATURE_64('E','D','K','2',' ',' ',' ',' ') // OEM table id 8 bytes long
#define EFI_ACPI_BERT_OEM_REVISION        0x00000001
#define EFI_ACPI_BERT_CREATOR_ID          SIGNATURE_32('I','N','T','L')
#define EFI_ACPI_BERT_CREATOR_REVISION    0x00000001

#define EFI_2_7_FIRMWARE_ERROR_RECORD_REVISION        2  // Header Revision. Will be removed when Cper.h has defined it.

#pragma pack(push,1)

typedef struct {
  EFI_ACPI_6_4_GENERIC_ERROR_DATA_ENTRY_STRUCTURE   GenericErrorDataEntry;
  EFI_FIRMWARE_ERROR_DATA                           EfiFirmwareErrorData;
} FIRMWARE_GENERIC_ERROR;

typedef struct {
  EFI_ACPI_6_5_BOOT_ERROR_RECORD_TABLE_HEADER    *Bert;
  CRASHLOG_HOB_DATA                              **HobDataPtr;
  UINT32                                         HobCount;
  UINT32                                         PayloadSize;
  UINT32                                         PayloadCount;
} CRASHLOG_DXE_PRIVATE_DATA;

#pragma pack(pop)
