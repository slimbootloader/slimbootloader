/** @file
  This file defines the hob structure for the seed list entries.

  Copyright (c) 2014 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SEED_INFO_GUID_H__
#define __SEED_INFO_GUID_H__

#define BOOTLOADER_SEED_LEN                   64
#define BOOTLOADER_SEED_MAX_ENTRIES           4
#define RPMB_MAX_PARTITION_NUM                4   // eMMC has only 1 partition now, but future UFS/NVMe may have more (2~4).
#define RPMB_HECI_KEY_SIZE                    64

// Seed Type
#define SEED_ENTRY_TYPE_SVNSEED               0x1
#define SEED_ENTRY_TYPE_RPMB                  0x2

// Usage Useed/Dseed
#define SEED_ENTRY_USAGE_USEED                0x1
#define SEED_ENTRY_USAGE_DSEED                0x2

// Usage Rpmb
#define SEED_ENTRY_USAGE_BASED_ON_SERIALNO      0x0
#define SEED_ENTRY_USAGE_NOT_BASED_ON_SERIALNO  0x1

#define SEED_ENTRY_USAGE_NA                   0xF

#define SEED_ENTRY_IDX_SVNSEED_IDX1           0x0
#define SEED_ENTRY_IDX_SVNSEED_IDX2           0x1
#define SEED_ENTRY_IDX_SVNSEED_IDX3           0x2
#define SEED_ENTRY_IDX_SVNSEED_IDX4           0x3
#define SEED_ENTRY_IDX_SVNSEED_INVALID        0xF

#define SEED_ENTRY_IDX_RPMB_PART1             0x0
#define SEED_ENTRY_IDX_RPMB_PART2             0x1
#define SEED_ENTRY_IDX_RPMB_PART3             0x2
#define SEED_ENTRY_IDX_RPMB_PART4             0x3
#define SEED_ENTRY_IDX_INVALID                0xF

///
/// Seed Info GUID
///
extern EFI_GUID gSeedListInfoHobGuid;


#pragma pack(1)
typedef struct {
  UINT8 CseSvn;   // 0XFF indicates invalid entry / SVN
  UINT8 BiosSvn;  // For ICL+, Reserved for APL
  UINT8 Reserved[2];
  UINT8 Seed[BOOTLOADER_SEED_LEN];
} SVN_SEED_INFO;

typedef struct {
  UINT32         NumofSeeds;
  SVN_SEED_INFO  UseedList[BOOTLOADER_SEED_MAX_ENTRIES];
  SVN_SEED_INFO  DseedList[BOOTLOADER_SEED_MAX_ENTRIES];
  UINT8          RpmbHeciSeeds[RPMB_MAX_PARTITION_NUM][RPMB_HECI_KEY_SIZE];
} LOADER_SEED_LIST;

typedef struct {
  UINT8     Type;           // Seed info struct: svn_seed_info or rpmbseed or attestation key
  UINT8     Usage;          // If same type, is it used or dseed.
                            // For RPMB, // Bit 0 => 0 = RPMB key is based on card serial number
                            //                       1 = RPMB key is not based on card serial number
  UINT8     Idx;            // MAX_ENTRIES: 4. If Same type and Usage, which seed Idx is this: {0,1,2,3}.
  UINT8     Reserved;
  UINT16    Flags;          // Reserved for future use
  UINT16    SeedEntrySize;  // Total size: sizeof (SVN_SEED_INFO) + sizeof(SEED_ENTRY)
  UINT8     Seed[0];        // Data of the Seed struct: SVN_SEED_INFO data or RPMB key data or attestation key box
} SEED_ENTRY;

typedef struct {
  UINT8       Revision;
  UINT8       Reserved0[3];
  UINT32      BufferSize;       // Will contain the total size allocated using PCDsize
  UINT8       TotalSeedCount;   // How many Seed_Entries ( useed + dseed + rpmb)
  UINT8       Reserved[3];
} SEED_LIST_INFO_HOB;
#pragma pack()

#endif
