/** @file
  Header file for PTCT.

  Copyright (c) 2020 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PTCT_H_
#define _PTCT_H_
#include <IndustryStandard/Acpi.h>


#define MAX_MEMORY_HIERARCHY_LATENCY_ENTRIES  3
#define MAX_MEMORY_HIERARCHY_LATENCY_APIC_ID  8

typedef enum {
  PtctNoneType,
  PtctPtcdLimitsType,
  PtctPtcmBinaryType,
  PtctWrcL3WayMasksType,
  PtctGtL3WayMasksType,
  PtctBufferType,
  PtctStreamDataPathType,
  PtctTimeAwareSubSystemsType,
  PtctRealTimeIommuType,
  PtctMemoryHierarchyLatencyType
} PtctEntryTypes;

#pragma pack(1)
/*
 * TCC Buffer Configuration
 */

#define EFI_ACPI_PTCT_SIGNATURE           SIGNATURE_32('P', 'T', 'C', 'T')  // 'PTCT'


#define R_SA_MCHBAR_WRC_VC_ENABLE                  0x6F10
#define B_SA_MCHBAR_WRC_VC_ENABLE_MASK             0x00040000

#define R_SA_MCHBAR_WRC_VC_CLOS                    0x6F08
#define N_SA_MCHBAR_WRC_VC_CLOS_OFFSET             15
#define B_SA_MCHBAR_WRC_VC_CLOS_MASK               0x00038000


#define TGL_MSR_WRC_COS_WAY_MASK_0_0 0x000018D0
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32 CosWaysMask : 20;
    /* Bits[19:0], Access Type=RW, default=0x000FFFFF*/
    /* available ways vectors for class of service of
       IA core restrictions: Only contiguous 1s are allowed.
    */
    UINT32 Rsvd20 : 12;
    /* Bits[31:20], Access Type=RO, default=None*/
    /* Reserved */
  } Bits;

  UINT32 Uint32;
  UINT64 Uint64;
} TGL_MSR_WRC_COS_WAY_MASK_0_0_REGISTER;

#define TGL_MSR_WRC_COS_WAY_MASK_1_1 0x000018D1
#define TGL_MSR_WRC_COS_WAY_MASK_2_2 0x000018D2
#define TGL_MSR_WRC_COS_WAY_MASK_3_3 0x000018D3

#define PTCT_WRC_L3_WAY_MASKS_VERSION 1

#define TGL_MSR_GT_COS_WAY_MASK_0_0 0x000018B0

typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32 CosWaysMask : 20;
    /* Bits[19:0], Access Type=RW, default=0x000FFFFF*/
    /* available ways vectors for class of service of
       IA core restrictions: Only contiguous 1s are allowed.
    */
    UINT32 Rsvd20 : 12;
    /* Bits[31:20], Access Type=RO, default=None*/
    /* Reserved */
  } Bits;

  UINT32 Uint32;
  UINT64 Uint64;
} TGL_MSR_GT_COS_WAY_MASK_0_0_REGISTER;

#define TGL_MSR_GT_COS_WAY_MASK_1_1 0x000018B1
#define TGL_MSR_GT_COS_WAY_MASK_2_2 0x000018B2
#define TGL_MSR_GT_COS_WAY_MASK_3_3 0x000018B3

typedef union {
  struct {
    UINT32  Low;
    UINT32  High;
  } Data32;
  UINT64 Data;
} UINT64_STRUCT;

#pragma pack (push,1)

typedef struct {
  UINT16  Size;
  UINT16  Version;
  UINT32  Type;
} PTCT_HEADER_ENTRY;

#define PTCT_PTCD_LIMITS_VERSION 1

typedef struct {
  PTCT_HEADER_ENTRY Header;
  UINT32            MaxIaL2Cos;
  UINT32            MaxIaL3Cos;
  UINT32            MaxGtCos;
  UINT32            MaxWrcCos;
  UINT32            MaxL2Instances;
  UINT32            MaxTccBuffers;
  UINT32            MaxTccBufferApicIds;
  UINT32            MaxTccStreams;
  UINT32            MaxTccRegisters;
} PTCT_PTCD_LIMITS;

typedef struct {
  PTCT_HEADER_ENTRY Header;
  UINT32            WrcL3WayMasks;
} PTCT_WRC_L3_WAY_MASKS;

#define PTCT_PTCM_BINARY_VERSION 1

typedef struct {
  PTCT_HEADER_ENTRY Header;
  UINT64            Address;
  UINT32            Size;
} PTCT_PTCM_BINARY;

#define PTCT_BUFFER_VERSION 1

// Size can larger. if more than 1 ApicId
typedef struct {
  PTCT_HEADER_ENTRY Header;
  UINT32            CacheLevel;
  UINT64            Base;
  UINT32            WayMasks;
  UINT32            Size;
  UINT32            ApicId[1];
} PTCT_BUFFER;

typedef struct {
  PTCT_HEADER_ENTRY Header;
  UINT32            CacheLevel;
  UINT64            Base;
  UINT32            WayMasks;
  UINT32            Size;
} PTCT_PSEUDO_SRAM2;

typedef struct {
  PTCT_HEADER_ENTRY Header;

} PTCT_PSEUDO_SRAM3;

typedef struct {
  PTCT_HEADER_ENTRY Header;
  UINT32            CacheLevel;
  UINT64            Base;
} PTCT_PSEUDO_SRAM4;

#define PTCT_GT_L3_WAY_MASKS_VERSION 1
#define MAX_GT_L3_WAY_MASKS 4

typedef struct {
  PTCT_HEADER_ENTRY Header;
  UINT32            GtL3WayMasks[MAX_GT_L3_WAY_MASKS];
} PTCT_GT_L3_WAY_MASKS;

#define PTCT_MEMORY_HIERARCHY_LATENCY_VERSION 1
#define MAX_APIC_ID                  8
#define MAX_MEMORY_HIERARCHY_LATENCY 3

typedef enum {
  MemHeirL2CacheType       = 0x2,
  MemHeirL3CacheType       = 0x3,
  MemHeirDramCacheType     = 0x100
} MEMORY_HIERARCHY_TYPES;
//
// Memory Hierarchy Latency
//
typedef struct {
  UINT32            Hierarchy;
  UINT32            ClockCycles;
  UINT32            NumOfValidApic;
  UINT32            ApicId[MAX_APIC_ID];
} MEMORY_HIERARCHY_LATENCY;

// Size can larger. if more than 1 ApicId
typedef struct {
  PTCT_HEADER_ENTRY Header;
  UINT32            Hierarchy;
  UINT32            ClockCycles;
  UINT32            ApicId[1];
} PTCT_MEMORY_HIERARCHY_LATENCY;

#pragma pack (pop)

typedef struct {
  EFI_STATUS (*ptct_entry_func) (UINT32 PtctIndex, PTCT_HEADER_ENTRY **PtctEntry); // Ptr to function with parameter list.
  UINT32     NumEntries;
} PtctCallEntry;

/**
  Gets WRC COS policy MSR Address

  @param[in] CosPolicy          Wrc COS Policy
  @param[out] MsrAddress        Cos Ways Mask

  @retval EFI_SUCCESS           Found WRC COS Address successful
  @retval EFI_INVALID_PARAMETER Invalid Cos Policy
  @retval EFI_UNSUPPORTED       Unsupported MSR
**/
EFI_STATUS
MsrGetWrcCosAddress (
  IN UINT8 CosPolicy,
  IN UINT32 *MsrAddress
  );

/*
  Get PTCT WRC L3 Way entry Data

  @param[in]  PtctIndex   PTCT entry Index
  @param[out] PTCT_HEADER_ENTRY   PTCT WRC L3 Way entry

  @retval EFI_SUCCESS             Successful get PTCT WRC L3 Way entry
  @retval EFI_INVALID_PARAMETER   Invalid Parameter
  @retval EFI_OUT_OF_RESOURCES    Fail allocate memory
*/
EFI_STATUS
GetPtctWrcL3WayMasks (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
  );

/*
  Get PTCT PTCD Limits entry data

  @param[in]  PtctIndex   PTCT entry Index
  @param[out] PtctEntry   PTCT PTCD Limits entry

  @retval EFI_SUCCESS             Successful get PTCT PTCD Limits entry
  @retval EFI_INVALID_PARAMETER   Invalid Parameter
  @retval EFI_OUT_OF_RESOURCES    Fail allocate memory
*/
EFI_STATUS
GetPtctPtcdLimits (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
  );

/*
  Get PTCT GT L3 Way entry Data

  @param[in]  PtctIndex   PTCT entry Index
  @param[out] PTCT_HEADER_ENTRY   PTCT GT L3 Way entry

  @retval EFI_SUCCESS             Successful get PTCT WRC L3 Way entry
  @retval EFI_INVALID_PARAMETER   Invalid Parameter
  @retval EFI_OUT_OF_RESOURCES    Fail allocate memory
*/
EFI_STATUS
GetPtctGtL3WayMasks (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
  );

/*
  Get PTCT Buffer entry Data

  @param[in]  PtctIndex   PTCT entry Index
  @param[out] PtctEntry   PTCT Buffer entry

  @retval EFI_SUCCESS             Successful get PTCT Buffer entry
  @retval EFI_INVALID_PARAMETER   Input invalid
  @retval EFI_OUT_OF_RESOURCES    Fail allocate memory
  @retval EFI_LOAD_ERROR          TCC Buffer Subregion is null
  @retval EFI_NOT_FOUND           Invalid Buffer IndexS
*/
EFI_STATUS
GetPtctBuffer (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
  );

/*
  Get PTCT Memory Hierarchy Latency entry Data

  @param[in]  PtctIndex   PTCT entry Index
  @param[out] PtctEntry   PTCT Memory Hierarchy Latency entry

  @retval EFI_SUCCESS             Successful get PTCT memory Hierarchy Latency entry
  @retval EFI_INVALID_PARAMETER   Invalid Parameter
  @retval EFI_OUT_OF_RESOURCES    Fail allocate memory
*/
EFI_STATUS
GetPtctMemHierarchyLatency (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
  );

/*
  Get PTCT PTCM Binary entry data

  @param[out] PTCT_HEADER_ENTRY   PTCT PTCM Binary entry
  @param[in]  UINT32              PTCT entry Index

  @retval EFI_SUCCESS             Successful get PTCT PTCM Binary entry
  @retval EFI_INVALID_PARAMETER   Invalid Parameter
  @retval EFI_OUT_OF_RESOURCES    Fail allocate memory
*/
EFI_STATUS
GetPtctPtcmBinary (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
  );

/**
  Update PTCT ACPI Table
**/
EFI_STATUS
UpdateAcpiPtctTable (
    IN EFI_ACPI_DESCRIPTION_HEADER *PtctTable
  );

#endif //_PTCT_H_
