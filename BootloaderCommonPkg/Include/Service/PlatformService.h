/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PLATFORM_SERVICE_H__
#define __PLATFORM_SERVICE_H__

#include <Guid/BootLoaderServiceGuid.h>

#define PLATFORM_SERVICE_SIGNATURE  SIGNATURE_32 ('S', 'P', 'L', 'T')
#define PLATFORM_SERVICE_VERSION    1

typedef enum {
  PreTempRamInit     = 0x10,
  PostTempRamInit    = 0x20,
  PreConfigInit      = 0x30,
  PostConfigInit     = 0x40,
  PreMemoryInit      = 0x50,
  PostMemoryInit     = 0x60,
  PreTempRamExit     = 0x70,
  PostTempRamExit    = 0x80,
  PreSiliconInit     = 0x90,
  PostSiliconInit    = 0xA0,
  PrePciEnumeration  = 0xB0,
  PostPciEnumeration = 0xC0,
  PrePayloadLoading  = 0xD0,
  PostPayloadLoading = 0xE0,
  EndOfStages        = 0xF0,
  ReadyToBoot        = 0xF8,
  EndOfFirmware      = 0xFF
} BOARD_INIT_PHASE;

/**
  Update ACPI tables using the new table provided.

  @param[in] AcpiTable            ACPI table address to update.
  @param[in] Length               ACPI table buffer length.

  @retval EFI_SUCCESS             Read success
  @retval EFI_INVALID_PARAMETER   Invalid region type given
  @retval EFI_UNSUPPORTED         Not supported
**/
typedef
EFI_STATUS
(EFIAPI *ACPI_TABLE_UPDATE) (
  IN     UINT8                      *AcpiTable,
  IN     UINT32                      Length
  );

/**
  Resets the entire platform.

  @param[in] ResetType          The type of reset to perform.

**/
typedef
VOID
(EFIAPI *RESET_SYSTEM) (
  IN EFI_RESET_TYPE   ResetType
  );

/**
  Platform notify service.

  @param[in] Phase          Boot phase to notify.

**/
typedef
VOID
(EFIAPI *NOTIFY_PHASE) (
  IN BOARD_INIT_PHASE   Phase
  );

typedef struct {
  SERVICE_COMMON_HEADER              Header;
  NOTIFY_PHASE                       NotifyPhase;
  ACPI_TABLE_UPDATE                  AcpiTableUpdate;
  RESET_SYSTEM                       ResetSystem;
} PLATFORM_SERVICE;

#endif
