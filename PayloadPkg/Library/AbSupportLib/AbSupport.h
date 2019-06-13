/** @file
  Payload implements one instance of Paltform Hook Library.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/Crc32Lib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/DebugLib.h>
#include <Library/PartitionLib.h>
#include <Library/MediaAccessLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/OsBootOptionGuid.h>
#include <BlockDevice.h>

// Magic for the A/B struct
#define AB_MAGIC_SIGNATURE       SIGNATURE_32 ('\0', 'A', 'B', '0')

typedef struct {
  ///
  /// Slot priority. Range from 0 to AB_MAX_PRIORITY
  /// 0 indicates this slot is unbootable.
  /// 1 is the lowest priority and AB_MAX_PRIORITY is highest
  ///
  UINT8                   Priority;

  ///
  /// number of times left to attempting to boot this slot.
  /// Range from 0 to AB_MAX_TRIES
  ///
  UINT8                   TriesRemaining;

  ///
  /// 0 indicates successful boot, other values means boot failure
  ///
  UINT8                   SuccessBoot;
  UINT8                   Reserved;
} AB_SLOT_DATA;

typedef struct {
  UINT32                  Magic;
  UINT8                   Major;
  UINT8                   Minor;
  UINT8                   Reserved1[2];
  AB_SLOT_DATA            SlotData[2];
  UINT8                   Reserved2[12];
  UINT32                  Crc32;
} AB_BOOT_INFO;

typedef struct {
  UINT8                   LegacyData[2048];
  AB_BOOT_INFO            AbBootInfo;
} MISC_PARTITION_DATA;

