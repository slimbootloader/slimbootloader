/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BOOTLOADER_CORE_LIB_H_
#define _BOOTLOADER_CORE_LIB_H_
#include <Library/BootloaderCommonLib.h>
#include <Guid/FlashMapInfoGuid.h>

#define  MPLD_SIGNATURE               SIGNATURE_32 ('$', 'P', 'L', 'D')
#define  IS_MULTI_PAYLOAD(x)          (*(UINT32 *)(x) == MPLD_SIGNATURE)

#define  IS_FLASH_SPACE(x)            (((x) >= PcdGet32 (PcdFlashBaseAddress)) && \
                                      ((x) <= PcdGet32 (PcdFlashBaseAddress) + PcdGet32 (PcdFlashSize) - 1))

#define  GET_STAGE_MODULE_ENTRY(x)       (STAGE_ENTRY) ((STAGE_HDR *)(x))->Entry
#define  GET_STAGE_MODULE_BASE(x)        (STAGE_ENTRY) ((STAGE_HDR *)(x))->Base

#define  PCD_GET32_WITH_ADJUST(x)     GetAdjustedPcdBase (PcdGet32 (x))

typedef  VOID   (*STAGE_ENTRY) (VOID *Params);

typedef  VOID   (*PAYLOAD_ENTRY) (VOID *HobList, VOID *Params);

typedef  VOID   (*KERNEL_ENTRY) (UINT32 Zero, UINT32 Arch, UINT32 Params);

#pragma pack(1)

#define  STAGE_IDT_ENTRY_COUNT        34

#define  PLATFORM_NAME_SIZE           8

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
} STAGE1A_ASM_HOB;

typedef struct {
  UINT32        CarBase;
  UINT32        CarTop;
  UINT32        HashStoreBase;
  UINT32        VerInfoBase;
  UINT32        AllocDataBase;
  UINT32        AllocDataLen;
  UINT32        Stage1BBase;
} STAGE1A_HOB;

typedef struct {
  UINT32        CarBase;
  UINT32        CarTop;
  UINT32        PayloadBase;
  UINT8         ConfigDataHashValid;
  UINT8         ConfigDataHash[32];
} STAGE1B_HOB;

typedef struct {
  UINT32        PayloadBase;
  UINT32        PayloadId;
  UINT32        PayloadOffset;
  UINT32        PayloadLength;
  UINT32        PayloadActualLength;
  UINT32        Stage2ExeBase;
} STAGE2_HOB;

typedef struct {
  UINT32        Signature;
  UINT32        CompressedSize;
  UINT32        Size;
  UINT32        Reserved;
  UINT8         Data[];
} LOADER_COMPRESSED_HEADER;

typedef struct {
  UINT32        Signature;
  UINT32        EntryNum;
  UINT32        EntrySize;
  UINT32        Reserved;
} MULTI_PAYLOAD_HEADER;

typedef struct {
  UINT32        Name;
  UINT32        Offset;
  UINT32        Size;
  UINT32        Reserved;
  UINT8         Hash[32];
} MULTI_PAYLOAD_ENTRY;

typedef struct {
  UINT32        AcpiTop;
  UINT32        AcpiBase;
  UINT32        AcpiGnvs;
  UINT8         BootMediaType;
  UINT8         BootPartition;
} S3_DATA;

#pragma pack()

#define ACPI_FEATURE_ENABLED()    (GetFeatureCfg() & FEATURE_ACPI)
#define MEASURED_BOOT_ENABLED()   (ACPI_FEATURE_ENABLED() && FeaturePcdGet (PcdMeasuredBootEnabled) && (GetFeatureCfg() & FEATURE_MEASURED_BOOT))

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

        Loader Reserved MEM
  +------------------------------+  StackTop
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
  UINT32            PayloadId;
  UINT32            DebugPrintErrorLevel;
  UINT8             DebugPortIdx;
  UINT8             Padding[3];
  VOID             *FspHobList;
  VOID             *LdrHobList;
  VOID             *VerInfoPtr;
  VOID             *HashStorePtr;
  VOID             *ConfDataPtr;
  VOID             *S3DataPtr;
  VOID             *DebugDataPtr;
  VOID             *PlatDataPtr;
  VOID             *LibDataPtr;
  VOID             *FlashMapPtr;
  VOID             *ServicePtr;
  VOID             *PcdDataPtr;
  VOID             *LogBufPtr;
  VOID             *DeviceTable;
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

#endif
