/** @file
  TCC Config Sub Region Struct

  Copyright (c) 2019 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _TCC_CONFIG_SUB_REGION_H_
#define _TCC_CONFIG_SUB_REGION_H_


#define MAX_BUFFERS 8
#define MAX_STREAMS 8

#define TCC_REGISTERS_MAX 550

#pragma pack(1)

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

typedef struct {
  UINT32  FormatVersion;
  BIOS_SETTINGS BiosSettings;
} TCC_BIOS_POLICY_CONFIG;

typedef struct {
  UINT32 Valid;
  UINT32 SocketId;
  UINT32 ApicId;
  UINT32 BufferSize;
  UINT32 Latency;
  UINT64 ArithmeticIntensity;
  UINT8  BufferAllocType;
  UINT8  Behavior;
} BUFFER_CONFIGURATION;

typedef struct {
  UINT32 SocketId;
  UINT32 ApicId;
} CPU_CORE;

typedef struct {
  UINT32 DidVid;
} PCI_DEVICE;

typedef struct {
  UINT32 Latency;
} SYSTEM_MEMORY;

typedef enum {
  CPU_CORE_TYPE      = 0x00000001,
  PCI_DEVICE_TYPE    = 0x00000002,
  SYSTEM_MEMORY_TYPE = 0x00000003,
} STREAM_ENDPOINT_TYPE;

typedef struct {
  UINT32 EpType;
  union {
      CPU_CORE      CpuCore;
      PCI_DEVICE    PciDev;
      SYSTEM_MEMORY SystemMemory;
  } Info;
} STREAM_ENDPOINT;

typedef struct {
  UINT32          Valid;
  UINT32          Bandwidth;
  STREAM_ENDPOINT EpA;
  STREAM_ENDPOINT EpB;
} STREAM_CONFIGURATION;

typedef struct {
  UINT32 FormatVersion;
  BUFFER_CONFIGURATION Buffer[MAX_BUFFERS];
  STREAM_CONFIGURATION Streams[MAX_STREAMS];
} TCC_IO_CONFIG;

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

typedef struct
{
  UINT32       FormatVersion;
  TCC_REGISTER TccRegisters[TCC_REGISTERS_MAX];
} TCC_REG_CONFIG;

typedef struct {
  UINT32                  FormatVersion;
  TCC_BIOS_POLICY_CONFIG  BiosConfig;       // TCC BIOS Settings
  TCC_IO_CONFIG           TccIoConfig;      // TCC Configuration
  TCC_REG_CONFIG          TccRegConfig;     // TCC Tuning Register
} PLATFORM_TUNING_CONFIG_DATA;

typedef union {
  PLATFORM_TUNING_CONFIG_DATA Config;
} TCC_CONFIG_SUB_REGION;

#pragma pack()

#endif
