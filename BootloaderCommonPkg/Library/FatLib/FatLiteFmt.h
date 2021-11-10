/** @file
  FAT format data structures

Copyright (c) 2006 - 2019, Intel Corporation. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _FAT_FMT_H_
#define _FAT_FMT_H_

//
// Definitions
//
#define FAT_ATTR_READ_ONLY                0x01
#define FAT_ATTR_HIDDEN                   0x02
#define FAT_ATTR_SYSTEM                   0x04
#define FAT_ATTR_VOLUME_ID                0x08
#define FAT_ATTR_DIRECTORY                0x10
#define FAT_ATTR_ARCHIVE                  0x20
#define FAT_ATTR_LFN                      (FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID)

#define FAT_CLUSTER_SPECIAL               ((MAX_UINT32 &~0xF) | 0x7)
#define FAT_CLUSTER_FREE                  0
#define FAT_CLUSTER_RESERVED              (FAT_CLUSTER_SPECIAL)
#define FAT_CLUSTER_BAD                   (FAT_CLUSTER_SPECIAL)
#define FAT_CLUSTER_LAST                  (-1)

#define DELETE_ENTRY_MARK                 0xE5
#define EMPTY_ENTRY_MARK                  0x00

//
// Some 8.3 File Name definitions
//
#define FAT_MAIN_NAME_LEN                 8
#define FAT_EXTEND_NAME_LEN               3
#define FAT_NAME_LEN                      (FAT_MAIN_NAME_LEN + FAT_EXTEND_NAME_LEN)

//
// Some Long File Name definitions
//
#define FAT_LFN_LAST                      0x40  // Ordinal field
#define MAX_LFN_ENTRIES                   20
#define LFN_CHAR1_LEN                     5
#define LFN_CHAR2_LEN                     6
#define LFN_CHAR3_LEN                     2
#define LFN_CHAR_TOTAL                    (LFN_CHAR1_LEN + LFN_CHAR2_LEN + LFN_CHAR3_LEN)
#define LFN_ENTRY_NUMBER(a)               (((a) + LFN_CHAR_TOTAL - 1) / LFN_CHAR_TOTAL)
#define FAT_LONG_NAME_LEN                 (MAX_LFN_ENTRIES * LFN_CHAR_TOTAL)

#define FAT_CLUSTER_FUNCTIONAL(Cluster)   (((Cluster) == 0) || ((Cluster) >= FAT_CLUSTER_SPECIAL))
#define FAT_CLUSTER_END_OF_CHAIN(Cluster) ((Cluster) > (FAT_CLUSTER_SPECIAL))

//
// Directory Entry
//
#pragma pack(1)

typedef struct {
  UINT16  Day : 5;
  UINT16  Month : 4;
  UINT16  Year : 7;                 // From 1980
} FAT_DATE;

typedef struct {
  UINT16  DoubleSecond : 5;
  UINT16  Minute : 6;
  UINT16  Hour : 5;
} FAT_TIME;

typedef struct {
  FAT_TIME  Time;
  FAT_DATE  Date;
} FAT_DATE_TIME;

typedef struct {
  CHAR8         FileName[11];       // 8.3 filename
  UINT8         Attributes;
  UINT8         CaseFlag;
  UINT8         CreateMillisecond;  // (creation milliseconds - ignored)
  FAT_DATE_TIME FileCreateTime;
  FAT_DATE      FileLastAccess;
  UINT16        FileClusterHigh;    // >= FAT32
  FAT_DATE_TIME FileModificationTime;
  UINT16        FileCluster;
  UINT32        FileSize;
} FAT_DIRECTORY_ENTRY;

typedef struct {
  UINT8         Ordinal;
  CHAR8         Name1[10];                // (Really 5 chars, but not WCHAR aligned)
  UINT8         Attributes;
  UINT8         Type;
  UINT8         Checksum;
  CHAR16        Name2[6];
  UINT16        MustBeZero;
  CHAR16        Name3[2];
} FAT_DIRECTORY_LFN;

#pragma pack()
//
// Boot Sector
//
#pragma pack(1)

typedef struct {

  UINT8   Ia32Jump[3];
  CHAR8   OemId[8];

  UINT16  SectorSize;
  UINT8   SectorsPerCluster;
  UINT16  ReservedSectors;
  UINT8   NoFats;
  UINT16  RootEntries;          // < FAT32, root dir is fixed size
  UINT16  Sectors;
  UINT8   Media;                // (ignored)
  UINT16  SectorsPerFat;        // < FAT32
  UINT16  SectorsPerTrack;      // (ignored)
  UINT16  Heads;                // (ignored)
  UINT32  HiddenSectors;        // (ignored)
  UINT32  LargeSectors;         // => FAT32
  UINT8   PhysicalDriveNumber;  // (ignored)
  UINT8   CurrentHead;          // holds boot_sector_dirty bit
  UINT8   Signature;            // (ignored)
  CHAR8   Id[4];
  CHAR8   FatLabel[11];
  CHAR8   SystemId[8];

} PEI_FAT_BOOT_SECTOR;

typedef struct {

  UINT8   Ia32Jump[3];
  CHAR8   OemId[8];

  UINT16  SectorSize;
  UINT8   SectorsPerCluster;
  UINT16  ReservedSectors;
  UINT8   NoFats;
  UINT16  RootEntries;          // < FAT32, root dir is fixed size
  UINT16  Sectors;
  UINT8   Media;                // (ignored)
  UINT16  SectorsPerFat;        // < FAT32
  UINT16  SectorsPerTrack;      // (ignored)
  UINT16  Heads;                // (ignored)
  UINT32  HiddenSectors;        // (ignored)
  UINT32  LargeSectors;         // Used if Sectors==0
  UINT32  LargeSectorsPerFat;   // FAT32
  UINT16  ExtendedFlags;        // FAT32 (ignored)
  UINT16  FsVersion;            // FAT32 (ignored)
  UINT32  RootDirFirstCluster;  // FAT32
  UINT16  FsInfoSector;         // FAT32
  UINT16  BackupBootSector;     // FAT32
  UINT8   Reserved[12];         // FAT32 (ignored)
  UINT8   PhysicalDriveNumber;  // (ignored)
  UINT8   CurrentHead;          // holds boot_sector_dirty bit
  UINT8   Signature;            // (ignored)
  CHAR8   Id[4];
  CHAR8   FatLabel[11];
  CHAR8   SystemId[8];

} PEI_FAT_BOOT_SECTOR_EX;

#pragma pack()

#endif
