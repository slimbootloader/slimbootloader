/** @file
  Payload implements one instance of Paltform Hook Library.

  Copyright (c) 2015 - 2023, Intel Corporation. All rights reserved.<BR>
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

// Magic for the A/B struct, 0x42414342
#define AB_MAGIC_SIGNATURE       SIGNATURE_32('B', 'C', 'A', 'B')

#pragma pack(1)

typedef struct {
  // Slot priority with 15 meaning highest priority, 1 lowest
  // priority and 0 the slot is unbootable.
  UINT8                   Priority:4;
  // Number of times left attempting to boot this slot.
  UINT8                   TriesRemaining:3;
  // 1 if this slot has booted successfully, 0 otherwise.
  UINT8                   SuccessBoot:1;
  // 1 if this slot is corrupted from a dm-verity corruption, 0
  // otherwise.
  UINT8                   VerityCorrupted:1;
  // Reserved for further use.
  UINT8                   Reserved:7;
} AB_SLOT_DATA;

typedef struct {
  // NUL terminated active slot suffix.
  UINT8                   SlotSuffix[4];
  // Bootloader Control AB magic number (see AB_MAGIC_SIGNATURE).
  UINT32                  Magic;
  // Version of struct being used (see AB_VERSION).
  UINT8                   Major;
  // Number of slots being managed.
  UINT8                   NbSlot:3;
  // Number of times left attempting to boot recovery.
  UINT8                   RecoveryTriesRemaining:3;
  // Status of any pending snapshot merge of dynamic partitions.
  UINT8                   MergeStatus:3;
  // Ensure 4-bytes alignment for slot_info field.
  UINT8                   Reserved1[1];
  // Per-slot information.  Up to 4 slots.
  AB_SLOT_DATA            SlotData[4];
  // Reserved for further use.
  UINT8                   Reserved2[8];
  // CRC32 of all 28 bytes preceding this field (little endian
  // format).
  UINT32                  Crc32;
} AB_BOOT_INFO;

typedef struct {
  UINT8                   LegacyData[2048];
  AB_BOOT_INFO            AbBootInfo;
} MISC_PARTITION_DATA;

#pragma pack()
