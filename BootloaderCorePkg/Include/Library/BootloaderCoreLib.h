/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BOOTLOADER_CORE_LIB_H_
#define _BOOTLOADER_CORE_LIB_H_
#include <Library/BootloaderCommonLib.h>
#include <Library/CryptoLib.h>
#include <Guid/FlashMapInfoGuid.h>

#define  MPLD_SIGNATURE               SIGNATURE_32 ('$', 'P', 'L', 'D')
#define  MVBT_SIGNATURE               SIGNATURE_32 ('$', 'M', 'V', 'B')
#define  IS_MULTI_PAYLOAD(x)          (*(UINT32 *)(x) == MPLD_SIGNATURE)

#define  IS_FLASH_SPACE(x)            (((x) >= PcdGet32 (PcdFlashBaseAddress)) && \
                                      ((x) <= PcdGet32 (PcdFlashBaseAddress) + PcdGet32 (PcdFlashSize) - 1))

#define  GET_STAGE_MODULE_ENTRY(x)    (STAGE_ENTRY) ((STAGE_HDR *)(x))->Entry
#define  GET_STAGE_MODULE_BASE(x)     (STAGE_ENTRY) ((STAGE_HDR *)(x))->Base

#define  PCD_GET32_WITH_ADJUST(x)     GetAdjustedPcdBase (PcdGet32 (x))

typedef  VOID   (*STAGE_ENTRY) (VOID *Params);

typedef  VOID   (*PAYLOAD_ENTRY) (VOID *HobList, VOID *Params);

typedef  VOID   (*KERNEL_ENTRY) (UINT32 Zero, UINT32 Arch, UINT32 Params);

#pragma pack(1)

#define  STAGE_IDT_ENTRY_COUNT        34

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
  EnumBufLogBuf,
  EnumBufMax
} BUF_INFO_ID;

typedef struct {
  UINT32        LdrGlobal;
  UINT64        IdtTable[STAGE_IDT_ENTRY_COUNT];
} STAGE_IDT_TABLE;

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
  UINT32        Signature;
  UINT8         EntryNum;
  UINT8         Reserved[3];
} VBT_MB_HDR;

typedef struct {
  UINT32        ImageId;
  UINT32        Length;
  UINT8         Data[];
} VBT_ENTRY_HDR;

typedef struct {
  UINT32        AcpiTop;
  UINT32        AcpiBase;
  UINT32        AcpiGnvs;
  UINT8         BootMediaType;
  UINT8         BootPartition;
} S3_DATA;

#pragma pack()

//
// These MACRO definations should be used only after feature config data is set in stage1b.
//
#define ACPI_FEATURE_ENABLED()   (GetFeatureCfg() & FEATURE_ACPI)
#define ACPI_ENABLED()           (FeaturePcdGet (PcdAcpiEnabled) && ACPI_FEATURE_ENABLED())
#define MEASURED_BOOT_ENABLED()  (FeaturePcdGet (PcdMeasuredBootEnabled) && (GetFeatureCfg() & FEATURE_MEASURED_BOOT) && ACPI_FEATURE_ENABLED())

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
  UINT8             BootMode;
  UINT8             LoaderStage;
  UINT8             CurrentBootPartition;
  UINT8             ResetReason;
  UINT32            StackTop;
  UINT32            MemPoolEnd;
  UINT32            MemPoolStart;
  UINT32            MemPoolCurrTop;
  UINT32            MemPoolCurrBottom;
  UINT32            MemUsableTop;
  UINT32            PayloadId;
  UINT32            DebugPrintErrorLevel;
  UINT8             DebugPortIdx;
  UINT8             Padding[3];
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

/**
  This function retrieves current boot mode.

  @retval    The current boot mode.

**/
UINT8
EFIAPI
GetBootMode (
  VOID
  );

/**
  This function sets current boot mode.

  @param BootMode   The value of current boot mode to set.

**/
VOID
EFIAPI
SetBootMode (
  IN UINT8   BootMode
  );

/**
  This function sets current boot partition.

  @param[in] Partition   The value of current boot partition to set.

**/
VOID
EFIAPI
SetCurrentBootPartition (
  IN UINT8   Partition
  );

/**
  This function retrieves reset reason in last boot.

  @retval    The last boot reset reason.

**/
UINT8
EFIAPI
GetResetReason (
  VOID
  );

/**
  This function sets reset reason in last boot.

  @param[in] ResetReason   The value of last boot reset reason.

**/
VOID
EFIAPI
SetResetReason (
  IN UINT8   ResetReason
  );

/**
  This function retrieves current payload ID to boot.

  @retval    The current payload ID to boot.

**/
UINT32
EFIAPI
GetPayloadId (
  VOID
  );

/**
  This function sets current payload ID to boot.

  @param[in] PayloadId   The value of payload ID to boot.

**/
VOID
EFIAPI
SetPayloadId (
  IN UINT32   PayloadId
  );

/**
  This function sets current platform id.

  @param PlatformId   The value of current platform id.

**/
VOID
EFIAPI
SetPlatformId (
  IN UINT16   PlatformId
  );

/**
  This function retrieves current platform BOM id.

  @retval    The current platform BOM id.

**/
UINT16
EFIAPI
GetPlatformBomId (
  VOID
  );


/**
  This function sets current debug port index.

  @param DebugPortIdx   The debug port index.

**/
VOID
EFIAPI
SetDebugPort (
  IN UINT8   DebugPortIdx
  );


/**
  This function gets current debug port index.

  @retval    The current debug port index.
**/
UINT8
EFIAPI
GetDebugPort (
  VOID
  );

/**
  This function sets current platform BOM id.

  @param BomId   The value of current platform BOM id.

**/
VOID
EFIAPI
SetPlatformBomId (
  IN UINT16   BomId
  );

/**
  This function retrieves current platform name.

  @retval    The current platform name.

**/
VOID *
EFIAPI
GetPlatformName (
  VOID
  );

/**
  This function sets current platform name.

  @param PlatformId   The value of current platform name.

**/
VOID
EFIAPI
SetPlatformName (
  IN VOID   *PlatformName
  );

/**

  Sets the pointer to the HOB List.

  @param HobList                       Pointer to the HOB List.
**/
VOID
EFIAPI
SetHobList (
  IN VOID          *HobList
  );

/**
  Adjust the PCD value based on boot partition.

  If current boot is from 1st boot partition,  return current PCD value.
  if current boot is from 2nd boot partitoin,  adjust the address gap
  between 1st redundant region base and 2nd redundant region base.

  @param[in] PcdValue              The base address getting from PCD

  @retval    UINT32                The adjusted base address.

**/
UINT32
GetAdjustedPcdBase (
  IN UINT32                        PcdValue
  );

/**
  Gets component information from the PCD's.

  This function will look for the component based on the input signature and
  will return the base address and size of the component.

  @param[in]  Signature     Signature of the component information required
  @param[out] Base          Base address of the component
  @param[out] Size          Size of the component

**/
EFI_STATUS
EFIAPI
GetComponentPcdInfo (
  IN  UINT32     Signature,
  OUT UINT32     *Base,
  OUT UINT32     *Size
  );

/**
  This function retrieves DMA buffer base.

  @retval    DMA buffer base.

**/
VOID *
EFIAPI
GetDmaBufferPtr (
  VOID
  );

/**
  This function retrieves usable memory top.

  @retval    Usable memory top.

**/
UINT32
EFIAPI
GetUsableMemoryTop (
  VOID
  );

#endif
