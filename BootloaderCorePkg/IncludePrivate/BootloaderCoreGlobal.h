/** @file
  Common header file for Bootloader Core global data structures.

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BOOTLOADER_CORE_GLOBAL_H_
#define _BOOTLOADER_CORE_GLOBAL_H_

#include <Library/BootloaderCoreLib.h>

#define  MPLD_SIGNATURE               SIGNATURE_32 ('$', 'P', 'L', 'D')
#define  IS_MULTI_PAYLOAD(x)          (*(UINT32 *)(x) == MPLD_SIGNATURE)

#define  IS_FLASH_SPACE(x)            (((x) >= PcdGet32 (PcdFlashBaseAddress)) && \
                                      ((x) <= PcdGet32 (PcdFlashBaseAddress) + PcdGet32 (PcdFlashSize) - 1))

#define  GET_STAGE_MODULE_ENTRY(x)    (STAGE_ENTRY) (UINTN)((STAGE_HDR *)(UINTN)(x))->Entry
#define  GET_STAGE_MODULE_BASE(x)     (STAGE_ENTRY) (UINTN)((STAGE_HDR *)(UINTN)(x))->Base

typedef  VOID   (EFIAPI *STAGE_ENTRY)   (VOID *Params);

typedef  VOID   (EFIAPI *PAYLOAD_ENTRY) (VOID *HobList, VOID *Params);

#pragma pack(1)

#define  STAGE_IDT_ENTRY_COUNT        34
#define  STAGE_GDT_ENTRY_COUNT        7

#define  PLATFORM_NAME_SIZE           8

typedef enum {
  EnumBufFlashMap,
  EnumBufVerInfo,
  EnumBufHashStore,
  EnumBufLibData,
  EnumBufService,
  EnumBufPcdData,
  EnumBufPlatData,
  EnumBufCfgData,
  EnumBufCtnList,
  EnumBufLogBuf,
  EnumBufMax
} BUF_INFO_ID;

typedef struct {
  UINT64        LdrGlobal;
  IA32_IDT_GATE_DESCRIPTOR  IdtTable[STAGE_IDT_ENTRY_COUNT];
} STAGE_IDT_TABLE;

typedef struct {
  IA32_SEGMENT_DESCRIPTOR   GdtTable[STAGE_GDT_ENTRY_COUNT];
} STAGE_GDT_TABLE;

typedef struct {
  UINT32        Entry;
  UINT32        Base;
} STAGE_HDR;

typedef struct {
  UINT32        CarBase;
  UINT32        CarTop;
  UINT64        TimeStamp;
  struct _STATUS {
    UINT32      CpuBist           :  1;
    UINT32      StackOutOfRange   :  1;
    UINT32      RsvdBits          : 30;
    UINT32      RsvdBits2         : 32;
  } Status;
} STAGE1A_ASM_PARAM;

typedef struct {
  VOID         *SrcBase;
  VOID         *DstBase;
  UINT32        AllocLen;
  UINT32        CopyLen;
} BUF_INFO;

typedef struct {
  UINT32        CarBase;
  UINT32        CarTop;
  UINT32        Stage1BBase;
  UINT32        AllocDataBase;
  UINT32        AllocDataLen;
  BUF_INFO      BufInfo[EnumBufMax];
} STAGE1A_PARAM;

typedef struct {
  UINT32        PayloadBase;
  UINT32        CfgDataAddr;
  UINT8         ConfigDataHashValid;
  UINT8         ConfigDataHash[HASH_DIGEST_MAX];
  UINT8         KeyHashManifestHashValid;
  UINT8         KeyHashManifestHash[HASH_DIGEST_MAX];
} STAGE1B_PARAM;

typedef struct {
  UINT32        PayloadBase;
  UINT32        PayloadActualLength;
  UINT32        Stage2ExeBase;
} STAGE2_PARAM;

typedef struct {
  UINT32        AcpiTop;
  UINT32        AcpiBase;
  UINT32        AcpiGnvs;
  UINT8         BootMediaType;
  UINT8         BootPartition;
} S3_DATA;

#pragma pack()

/*
          Reserved MEM
  +------------------------------+  Top of Low MEM
  |       SOC Reserved MEM       |
  +------------------------------+  Top of usable MEM Base
  |       FSP Reserved MEM       |
  +------------------------------+  FSP Reserved MEM Base
  |       LDR Reserved MEM       |
  +------------------------------+  LDR Reserved MEM Base
  |         ACPI NVS MEM         |
  +------------------------------+  ACPI NVS MEM Base
  |       ACPI Reclaim MEM       |
  +------------------------------+  ACPI Reclaim MEM Base
  |       PLD Reserved MEM       |
  +------------------------------+  PLD Reserved MEM Base
  |         DMA Buffer           |
  +------------------------------+  DMA Buffer MEM Base
  |          PLD Heap            |
  +------------------------------+  PLD Heap MEM Base
  |          PLD Stack           |
  +------------------------------+  PLD Stack MEM Base

        Loader Reserved MEM
  +------------------------------+  FSP Reserved MEM Base
  |       LDR Stack (Down)       |
  |                              |
  |         LDR HOB (Up)         |
  +------------------------------+  MemPoolEnd (Fixed)
  |                              |
  |   Permanant MEM Pool (Down)  |
  |                              |
  +------------------------------+  MemPoolCurrTop (Moving down)
  |                              |
  +------------------------------+  MemPoolCurrBottom (Moving up)
  |                              |
  |   Temporary MEM Pool (Up)    |
  |                              |
  +------------------------------+  MemPoolStart (Fixed)
*/

#define  LDR_GDATA_SIGNATURE     SIGNATURE_32('L', 'D', 'R', 'G')

typedef struct {
  UINT32            Signature;
  UINT16            PlatformId;
  UINT16            PlatformBomId;
  UINT32            SocSku;
  UINT8             BootMode;
  UINT8             LoaderStage;
  UINT8             CurrentBootPartition;
  UINT8             ResetReason;
  UINT32            StackTop;
  UINT32            MemPoolEnd;
  UINT32            MemPoolStart;
  UINT32            MemPoolCurrTop;
  UINT32            MemPoolCurrBottom;
  UINT32            PayloadId;
  UINT32            DebugPrintErrorLevel;
  UINT8             DebugPortIdx;
  UINT8             Padding[3];
  UINT64            MemoryInfo[EnumMemInfoMax];
  VOID             *FspHobList;
  VOID             *LdrHobList;
  VOID             *FlashMapPtr;
  VOID             *VerInfoPtr;
  VOID             *HashStorePtr;
  VOID             *LibDataPtr;
  VOID             *ServicePtr;
  VOID             *PcdDataPtr;
  VOID             *PlatDataPtr;
  VOID             *CfgDataPtr;
  VOID             *LogBufPtr;
  VOID             *DeviceTable;
  VOID             *ContainerList;
  VOID             *S3DataPtr;
  VOID             *DebugDataPtr;
  VOID             *DmaBufferPtr;
  UINT8             PlatformName[PLATFORM_NAME_SIZE];
  UINT32            LdrFeatures;
  BL_PERF_DATA      PerfData;
  UINT32            CarBase;
  UINT32            CarSize;
  UINT32            MemPoolMaxUsed;
} LOADER_GLOBAL_DATA;

/**
  This function sets the Loader global data pointer.

  @param[in] LoaderData    Fsp global data pointer.

**/
VOID
EFIAPI
SetLoaderGlobalDataPointer (
  IN LOADER_GLOBAL_DATA   *LoaderData
  );

/**
  This function gets the Loader global data pointer.

**/
LOADER_GLOBAL_DATA *
EFIAPI
GetLoaderGlobalDataPointer (
  VOID
  );

#endif
