/** @file
  TCC Config Sub Region's Struct Data

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _TCC_LIB_H_
#define _TCC_LIB_H_

#include <IndustryStandard/Acpi.h>

#define MAX_TCC_BUFFERS   (9)
#define MAX_TCC_STREAMS   (8)
#define MAX_TCC_REGISTERS (550)
#define MAX_L2_INSTANCES (1)
#define MAX_IA_L2_CLOS (16)
#define MAX_IA_L3_CLOS (4)
#define MAX_GT_CLOS (4)        /* Max GT L3 CLOS Policy */
#define MAX_WRC_CLOS (0)       /* Max WRC VcRt L3 CLOS Policy */
#define MAX_TCC_BUFFER_APIC_IDS (16)

#define PTCT_WRC_L3_WAY_MASKS_VERSION         1
#define PTCT_PTCM_BINARY_VERSION              1
#define PTCT_BUFFER_VERSION                   1
#define PTCT_GT_L3_WAY_MASKS_VERSION          1
#define MAX_GT_L3_WAY_MASKS                   4
#define PTCT_MEMORY_HIERARCHY_LATENCY_VERSION 1
#define MAX_MEMORY_HIERARCHY_LATENCY_ENTRIES  3
#define MAX_MEMORY_HIERARCHY_LATENCY_APIC_ID  8

#define EFI_ACPI_PTCT_SIGNATURE           SIGNATURE_32('P', 'T', 'C', 'T')  // 'PTCT'

#pragma pack(1)
extern EFI_GUID gTccInfoHobGuid;

/*
 * TCC PtcmBinary Configuration
 */
typedef struct {
  UINT8  PtcmBinaryImage[0]; /* Image Buffer */
} TCC_PTCM_BINARY_SUB_REGION;

/*
 * TCC Stream Configuration
 */
typedef enum {
  CPU_CORE_TYPE      = 0x00000001,
  PCI_DEVICE_TYPE    = 0x00000002,
  SYSTEM_MEMORY_TYPE = 0x00000003,
} STREAM_ENDPOINT_TYPE;

typedef struct {
  UINT16 EndPointType;
  union {
    struct {
      UINT32    ApicId;         /* APIC ID of CPU core endpoint */
    } CpuCore;
    struct {
      UINT32    DidVid;         /* DID:VID of PCI device endpoint */
    } PciDevice;
    struct {
      UINT32    TccBufferIndex;
    } TccBuffer;
  } Info;
} STREAM_ENDPOINT;

typedef struct {
  UINT32          InfoFormat;       /* determines which struct format to use */
  union {
    struct {
      UINT32          TransferSize;     /* Size (in bytes) of data moved from EPA to EPB */
      UINT32          TransferLatency;  /* Maximum latency (in nanoseconds) for data moved from EPA to EPB */
      UINT8           TrafficClassMask; /* one-hot bit mask indicating which TC(s) to use */
      UINT8           Priority;         /* lower numbers are higher priority */
      STREAM_ENDPOINT EndPointA;
      STREAM_ENDPOINT EndPointB;
    } Format1;
  } Info;
} TCC_STREAM;

/*
 * TCC BIOS Configuration
 */
typedef struct {
  UINT8 Sstates;
  UINT8 Cstates;
  UINT8 Pstates;
  UINT8 Dstates;
  UINT8 CoreRapl;
  UINT8 MemoryRapl;
  UINT8 Turbo;
  UINT8 SaGv;
  UINT8 GtPm;
  UINT8 MemPm;
  UINT8 FabricPm;
  UINT8 GtRstRc6;
  UINT8 PcieAspm;
  UINT8 PcieClkGate;
  UINT8 PchPwrClkGate;
  UINT8 DmiAspm;
  UINT8 PcieRpL1;
  UINT8 HyperThreading;
  UINT8 FastGv;
  UINT8 VtdEn;
  UINT8 HwpEn; /* speed shift */
  UINT8 DelayEnDmiAspm;
} BIOS_SETTINGS;

typedef enum {
  TCC_PRE_MEM_PHASE   = 0,
  TCC_POST_MEM_PHASE  = 1,
  TCC_LATE_INIT_PHASE = 2,
  TCC_PHASE_INVALID   = 0xFFFFFFFF
} TCC_REG_PHASE;

typedef enum {
  MMIO32          = 0,
  MMIO64          = 1,
  MSR             = 2,
  IOSFSB          = 3,
  MAILBOX         = 4,
  TCC_REG_INVALID = 0xFFFFFFFF
} TCC_REG_TYPE;

typedef enum {
  SABOX   = 0,
  MSRBOX  = 1
} MAILBOX_TARGET;

typedef struct {
  UINT32 U32Base; /* ECAM format B:D:F:R of BAR */
  UINT32 U32Addr; /* offset from BAR */
  UINT32 U32Mask; /* data bit-mask */
  UINT32 U32Data; /* data value */
} MMIO32_INFO;

typedef struct {
  union {
    UINT64 U64Value;
    UINTN  Value;
  } Base;         /* ECAM format B:D:F:R of BAR */
  union {
    UINT64 U64Value;
    UINTN  Value;
  } Addr;         /* offset from BAR */
  UINT64 U64Mask; /* data bit-mask */
  UINT64 U64Data; /* data value */
} MMIO64_INFO;

typedef struct {
  UINT32 U32Addr; /* ECX value */
  UINT64 U64Mask; /* EDX:EAX data bit-mask */
  UINT64 U64Data; /* EDX:EAX data value */
} MSR_INFO;

typedef struct {
  UINT8  Port;       /* IOSFSB Port ID */
  UINT8  IosfSbType; /* IOSFSB Register Type (command) */
  UINT32 U32Addr;    /* register address */
  UINT32 U32Mask;    /* data bit-mask */
  UINT32 U32Data;    /* data value */
} IOSFSB_INFO;

typedef struct {
  MAILBOX_TARGET MailboxType; /* Mailbox Register Type */
  UINT32         U32Cmd;      /* register address */
  UINT32         U32Mask;     /* data bit-mask */
  UINT32         U32Data;     /* data value */
} MAILBOX_INFO;

typedef struct
{
  UINT32  TccRegPhase;
  UINT32  TccRegType;
  union {
      MMIO32_INFO  Mmio32;
      MMIO64_INFO  Mmio64;
      MSR_INFO     Msr;
      IOSFSB_INFO  IosfSb;
      MAILBOX_INFO Mailbox;
  } Info;
} TCC_REGISTER;

typedef struct  {
  UINT32 InfoFormat; /* determines which struct format to use */
  union {
    struct {
      BIOS_SETTINGS        BiosSettings;
      UINT32               ValidStreams;               /* number of valid entries in Streams */
      TCC_STREAM           Streams[MAX_TCC_STREAMS];   /* desired cache ways (one-hot) */
      UINT32               ValidRegs;                  /* number of valid entries in TccRegs*/
      TCC_REGISTER         TccRegs[MAX_TCC_REGISTERS]; /* number of valid entries in u32_APIC_ID[] */
    } Format1;
  } Info;
} TCC_STREAM_CONFIGURATION;

typedef struct  {
  VOID          *BufferBase;
  UINT32         BufferSize;
  VOID          *TuningBase;
  UINT32         TuningSize;
  VOID          *CacheconfigBase;
  UINT32         CacheconfigSize;
  VOID          *PtcmBase;
  UINT32         PtcmSize;
} TCC_BINARIES_BASE;

#pragma pack()

/*
 * TCC Buffer Configuration
 */

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

typedef struct  {
  UINT32 InfoFormat; /* determines which struct format to use */
  union {
    struct {
      UINT32 Size; /* size in bytes */
      UINT32 CacheWays; /* desired cache ways (one-hot) */
      UINT32 CacheLevel; /* desired cache level (e.g. 2 or 3) */
      UINT32 ValidApicIds; /* number of valid entries in u32_APIC_ID[] */
      UINT32 ApicId[MAX_TCC_BUFFER_APIC_IDS]; /* APIC_IDs associated with this buffer */
    } Format;
  } Info;
}TCC_BUFFER;

typedef struct  {
  UINT32 InfoFormat; /* determines which struct format to use */
  union {
    struct {
      UINT32 ValidBuffers; /* number of valid entris in s_buffers[] */
      TCC_BUFFER SBuffers[MAX_TCC_BUFFERS];
    } Format;
  } Info;
}TCC_BUFFER_CONFIGURATION;

typedef union {
  struct {
    UINT32  Low;
    UINT32  High;
  } Data32;
  UINT64 Data;
} UINT64_STRUCT;

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
} MSR_WRC_COS_WAY_MASK_0_0_REGISTER;

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
} MSR_GT_COS_WAY_MASK_0_0_REGISTER;

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

typedef struct {
  PTCT_HEADER_ENTRY Header;
  UINT64            Address;
  UINT32            Size;
} PTCT_PTCM_BINARY;

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

typedef struct {
  PTCT_HEADER_ENTRY Header;
  UINT32            GtL3WayMasks[MAX_GT_L3_WAY_MASKS];
} PTCT_GT_L3_WAY_MASKS;

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
  UINT32            ApicId[MAX_MEMORY_HIERARCHY_LATENCY_APIC_ID];
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

/*
  Get PTCT PTCD Limits entry data

  @param[in]  PtctIndex   PTCT entry Index
  @param[out] PtctEntry   PTCT PTCD Limits entry

  @retval EFI_SUCCESS             Successful get PTCT PTCD Limits entry
  @retval EFI_INVALID_PARAMETER   Invalid Parameter
  @retval EFI_OUT_OF_RESOURCES    Fail allocate memory
*/
EFI_STATUS
EFIAPI
GetPtctPtcdLimits (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
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
EFIAPI
GetPtctWrcL3WayMasks (
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
EFIAPI
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
EFIAPI
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
EFIAPI
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
EFIAPI
GetPtctPtcmBinary (
  IN  UINT32            PtctIndex,
  OUT PTCT_HEADER_ENTRY **PtctEntry
  );

/**
  Update PTCT ACPI Table
**/
EFI_STATUS
EFIAPI
UpdateAcpiPtctTable (
    IN EFI_ACPI_DESCRIPTION_HEADER *PtctTable
  );

/**
  Update FSP-M UPD config data for TCC tuning, load TCC buffer & tuning data binaries from TempRam
**/
EFI_STATUS
EFIAPI
TccModePreMemConfig (
  OUT BIOS_SETTINGS*     PreMemPolicyConfig,
  OUT TCC_BINARIES_BASE* PreMemTccBinaries
);

/**
  Update FSP-S UPD config data for TCC tuning, load TCC related binaries into FSP
**/
EFI_STATUS
EFIAPI
TccModePostMemConfig (
  VOID
  );

#endif
