/** @file
  TCC Config Sub Region's Struct Data

  Copyright (c) 2019 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _TCC_CONFIG_SUB_REGION_H_
#define _TCC_CONFIG_SUB_REGION_H_

#define MAX_TCC_BUFFERS   (9)
#define MAX_TCC_STREAMS   (8)
#define MAX_TCC_REGISTERS (550)
#define MAX_L2_INSTANCES (4)
#define MAX_IA_L2_CLOS (8)
#define MAX_IA_L3_CLOS (4)
#define MAX_GT_CLOS (4)
#define MAX_WRC_CLOS (1)

#pragma pack(1)
/*
 * TCC Cache Configuration
 */
typedef struct {
  UINT32 InfoFormat; /* determines which struct format to use */
  union{
    struct{
      UINT32 L2Masks[MAX_L2_INSTANCES][MAX_IA_L2_CLOS];
      UINT32 L3Masks[MAX_IA_L3_CLOS];
      UINT32 GtMasks[MAX_GT_CLOS];
      UINT32 WrcMasks[MAX_WRC_CLOS];
    } Format1;
  } Info;
}TCC_CACHE_CONFIGURATION;

/*
 * TCC Buffer Configuration
 */
#define MAX_TCC_BUFFER_APIC_IDS (16)

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
  UINT16 EpType;
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
      STREAM_ENDPOINT EpA;
      STREAM_ENDPOINT EpB;
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
  UINT8 HwpEn;
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
      UINT32               ValidRegs;                  /* number of valid entries in */
      TCC_REGISTER         TccRegs[MAX_TCC_REGISTERS]; /* number of valid entries in u32_APIC_ID[] */
    } Format1;
  } Info;
} TCC_STREAM_CONFIGURATION;

#pragma pack()

#endif
