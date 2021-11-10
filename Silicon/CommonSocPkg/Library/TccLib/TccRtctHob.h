/** @file

  Copyright (c) 2021 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _TCC_RTCT_HOB_H_
#define _TCC_RTCT_HOB_H_

#include <Base.h>
#include <IndustryStandard/Acpi60.h>

extern EFI_GUID gTccRtctHobGuid;

//
// RTCT Version
//
#define RTCT_VERSION_MAJOR         2
#define RTCT_VERSION_MINOR         0

//
// RTCT Entry Types
//
typedef enum {
  RtctCompatibilityType           = 0x00000000,
  RtctRtcdLimitsType              = 0x00000001,
  RtctCrlBinaryType               = 0x00000002,
  RtctIaWayMasksType              = 0x00000003,
  RtctWrcWayMasksType             = 0x00000004,
  RtctGtWayMasksType              = 0x00000005,
  RtctSsramWayMaskType            = 0x00000006,
  RtctSoftwareSramType            = 0x00000007,
  RtctMemoryHierarchyLatencyType  = 0x00000008,
  RtctErrorLogAddressType         = 0x00000009
} RtctEntryTypes;

///
/// The TCC RTCT data HOB starts with EFI_ACPI_DESCRIPTION_HEADER
/// EFI_ACPI_DESCRIPTION_HEADER.Length has the actual data lengh for this HOB
/// including the ACPI header.
/// Following the EFI_ACPI_DESCRIPTION_HEADER, it is the various TCC entries
///

#pragma pack (push,1)

//
// RTCT Entry Header
//
typedef struct {
  UINT16  Size;
  UINT16  Version;
  UINT32  Type;
} RTCT_HEADER_ENTRY;


#define RTCT_COMPATABILITY_VERSION 1

//
// RTCT Compatability Entry
//
typedef struct {
  RTCT_HEADER_ENTRY Header;
  UINT32            RtctVersionMajor;
  UINT32            RtctVersionMinor;
  UINT32            RtcdVersionMajor;
  UINT32            RtcdVersionMinor;
} RTCT_COMPATABILITY;


#define RTCT_RTCD_LIMITS_VERSION 2

//
// RTCT RTCD Limits Entry
//
typedef struct {
  RTCT_HEADER_ENTRY Header;
  UINT32            MaxIaL2Cos;
  UINT32            MaxIaL3Cos;
  UINT32            MaxL2Instances;
  UINT32            MaxL3Instances;
  UINT32            MaxGtCos;
  UINT32            MaxWrcCos;
  UINT32            MaxTccStreams;
  UINT32            MaxTccRegisters;
} RTCT_RTCD_LIMITS;


#define RTCT_CRL_BINARY_VERSION 1

//
// RTCT CRL Binary Entry
//
typedef struct {
  RTCT_HEADER_ENTRY Header;
  UINT64            Address;
  UINT32            Size;
} RTCT_CRL_BINARY;


#define RTCT_IA_WAY_MASKS_VERSION 1
//
// RTCT IA Way Masks Entry
//
//
typedef struct {
  RTCT_HEADER_ENTRY Header;
  UINT32            CacheLevel;
  UINT32            CacheId;
  UINT32            WayMask[0];
} RTCT_IA_WAY_MASKS;


#define RTCT_WRC_WAY_MASKS_VERSION 2

//
// RTCT WRC Way Mask Entry
//
typedef struct {
  RTCT_HEADER_ENTRY Header;
  UINT32            CacheLevel;
  UINT32            CacheId;
  UINT32            WayMask;
} RTCT_WRC_WAY_MASKS;

#define RTCT_GT_WAY_MASKS_VERSION 1

//
// RTCT GT Way Mask Entry
//
typedef struct {
  RTCT_HEADER_ENTRY Header;
  UINT32            CacheLevel;
  UINT32            CacheId;
  UINT32            WayMask[0];
} RTCT_GT_WAY_MASKS;

#define RTCT_SSRAM_WAY_MASK_VERSION 1
//
// RTCT SSRAM Way Mask Entry
//
typedef struct {
  RTCT_HEADER_ENTRY Header;
  UINT32            CacheLevel;
  UINT32            CacheId;
  UINT32            WayMask;
} RTCT_SSRAM_WAY_MASK;

#define RTCT_SOFTWARE_SRAM_VERSION 2

//
// Rtct Software SRAM Entry
//
typedef struct {
  RTCT_HEADER_ENTRY Header;
  UINT32            CacheLevel;
  UINT32            CacheId;
  UINT64            Base;
  UINT32            Size;
  UINT32            Shared;
} RTCT_SOFTWARE_SRAM;

#define RTCT_MEMORY_HIERARCHY_LATENCY_VERSION 2
//
// RTCT Memory Hierarchy Latency Entry
//
typedef struct {
  RTCT_HEADER_ENTRY Header;
  UINT32            Hierarchy;
  UINT32            ClockCycles;
  UINT32            ClockCyclesVt;
  UINT32            CacheId[0];
} RTCT_MEMORY_HIERARCHY_LATENCY;

#pragma pack (pop)

#endif
