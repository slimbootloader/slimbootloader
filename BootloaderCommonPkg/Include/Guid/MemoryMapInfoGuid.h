/** @file
  This file defines the hob structure for the memory map entries.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __MEMORY_MAP_INFO_GUID_H__
#define __MEMORY_MAP_INFO_GUID_H__

#define MEM_MAP_FLAG_PAYLOAD      0x01

#define MEM_MAP_TYPE_RAM          0x01
#define MEM_MAP_TYPE_RESERVED     0x02
#define MEM_MAP_TYPE_ACPI_RECLAIM 0x03
#define MEM_MAP_TYPE_ACPI_NVS     0x04

///
/// Serial Port Information GUID
///
extern EFI_GUID gLoaderMemoryMapInfoGuid;

#pragma pack(1)
typedef struct {
  UINT64 Base;
  UINT64 Size;
  UINT8  Type;
  UINT8  Flag;
  UINT8  Reserved[6];
} MEMORY_MAP_ENTRY;

typedef struct {
  UINT8  Revision;
  UINT8  Reserved0[3];
  UINT32 Count;
  MEMORY_MAP_ENTRY  Entry[0];
} MEMORY_MAP_INFO;
#pragma pack()

#endif
