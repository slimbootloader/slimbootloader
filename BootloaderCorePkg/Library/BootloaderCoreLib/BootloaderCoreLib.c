/** @file

  Copyright (c) 2016 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Guid/FlashMapInfoGuid.h>
#include <BootloaderCoreGlobal.h>

/**
  This function retrieves current boot mode.

  @retval    The current boot mode.

**/
UINT8
EFIAPI
GetBootMode (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->BootMode;
}

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
  )
{
  UINT32               RedundentBase;
  UINT32               RedundentSize;

  RedundentSize  = PcdGet32 (PcdRedundantRegionSize);
  if (RedundentSize > 0) {
    if (GetCurrentBootPartition () == 1) {
      RedundentBase  = 0 - (PcdGet32 (PcdTopSwapRegionSize) * 2 + RedundentSize);
      if ((PcdValue >= RedundentBase) && (PcdValue < RedundentBase + RedundentSize)) {
        return PcdValue - RedundentSize;
      }
    }
  }
  return PcdValue;
}

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
  )
{
  UINT32        PcdBase;
  UINT32        PcdSize;
  EFI_STATUS    Status;

  Status = EFI_SUCCESS;

  switch (Signature) {
  case FLASH_MAP_SIG_STAGE1A:
    PcdBase = PCD_GET32_WITH_ADJUST (PcdStage1ABase);
    PcdSize = PcdGet32 (PcdStage1ASize);
    break;
  case FLASH_MAP_SIG_STAGE1B:
    PcdBase = PCD_GET32_WITH_ADJUST (PcdStage1BBase);
    PcdSize = PcdGet32 (PcdStage1BSize);
    break;
  case FLASH_MAP_SIG_STAGE2:
    PcdBase = PCD_GET32_WITH_ADJUST (PcdStage2Base);
    PcdSize = PcdGet32 (PcdStage2Size);
    break;
  case FLASH_MAP_SIG_ACM:
    PcdBase = 0xFFFFFFFF;
    PcdSize = 0xFFFFFFFF;
    Status = EFI_NOT_FOUND;
    break;
  case FLASH_MAP_SIG_UCODE:
    PcdBase = PCD_GET32_WITH_ADJUST (PcdUcodeBase);
    PcdSize = PcdGet32 (PcdUcodeSize);
    break;
  case FLASH_MAP_SIG_MRCDATA:
    PcdBase = PCD_GET32_WITH_ADJUST (PcdMrcDataBase);
    PcdSize = PcdGet32 (PcdMrcDataSize);
    break;
  case FLASH_MAP_SIG_PAYLOAD:
    PcdBase = PCD_GET32_WITH_ADJUST (PcdPayloadBase);
    PcdSize = PcdGet32 (PcdPayloadSize);
    break;
  case FLASH_MAP_SIG_SPI_IAS1:
    PcdBase = PCD_GET32_WITH_ADJUST (PcdSpiIasImage1RegionBase);
    PcdSize = PcdGet32 (PcdSpiIasImage1RegionSize);
    break;
  case FLASH_MAP_SIG_SPI_IAS2:
    PcdBase = PCD_GET32_WITH_ADJUST (PcdSpiIasImage2RegionBase);
    PcdSize = PcdGet32 (PcdSpiIasImage2RegionSize);
    break;
  case FLASH_MAP_SIG_FWUPDATE:
    PcdBase = PCD_GET32_WITH_ADJUST (PcdFwuPayloadBase);
    PcdSize = PcdGet32 (PcdFwuPayloadSize);
    break;
  case FLASH_MAP_SIG_CFGDATA:
    PcdBase = PCD_GET32_WITH_ADJUST (PcdCfgDataBase);
    PcdSize = PcdGet32 (PcdCfgDataSize);
    break;
  default:
    PcdBase = 0xFFFFFFFF;
    PcdSize = 0xFFFFFFFF;
    Status = EFI_NOT_FOUND;
  }

  //
  // If base and pcdbase are not 0, fill and return the value
  //
  if ((Base != NULL) && (PcdBase != 0)) {
    *Base = PcdBase;
  }
  if ((Size != NULL) && (PcdSize != 0)) {
    *Size = PcdSize;
  }

  return Status;
}

/**
  This function sets current boot mode.

  @param BootMode   The value of current boot mode to set.

**/
VOID
EFIAPI
SetBootMode (
  IN UINT8   BootMode
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;

  LdrGlobal = GetLoaderGlobalDataPointer();
  LdrGlobal->BootMode = (UINT8)BootMode;
}

/**
  This function sets current boot partition.

  @param[in] Partition   The value of current boot partition to set.

**/
VOID
EFIAPI
SetCurrentBootPartition (
  IN UINT8   Partition
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;

  LdrGlobal = GetLoaderGlobalDataPointer();
  LdrGlobal->CurrentBootPartition = Partition;
}

/**
  This function retrieves reset reason in last boot.

  @retval    The last boot reset reason.

**/
UINT8
EFIAPI
GetResetReason (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->ResetReason;
}

/**
  This function sets reset reason in last boot.

  @param[in] ResetReason   The value of last boot reset reason.

**/
VOID
EFIAPI
SetResetReason (
  IN UINT8   ResetReason
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;

  LdrGlobal = GetLoaderGlobalDataPointer();
  LdrGlobal->ResetReason = ResetReason;
}

/**
  This function retrieves current payload ID to boot.

  @retval    The current payload ID to boot.

**/
UINT32
EFIAPI
GetPayloadId (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->PayloadId;
}

/**
  This function sets current payload ID to boot.

  @param[in] PayloadId   The value of payload ID to boot.

**/
VOID
EFIAPI
SetPayloadId (
  IN UINT32   PayloadId
  )
{
  GetLoaderGlobalDataPointer()->PayloadId = PayloadId;
}

/**
  This function sets current platform id.

  @param PlatformId   The value of current platform id.

**/
VOID
EFIAPI
SetPlatformId (
  IN UINT16   PlatformId
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;

  LdrGlobal = GetLoaderGlobalDataPointer();
  LdrGlobal->PlatformId = PlatformId;
}


/**
  This function retrieves current platform BOM id.

  @retval    The current platform BOM id.

**/
UINT16
EFIAPI
GetPlatformBomId (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->PlatformBomId;
}

/**
  This function sets current SOC SKU info.

  @param Sku    The value of current SOC SKU.

**/
VOID
EFIAPI
SetSocSku (
  IN UINT32 Sku
  )
{
  GetLoaderGlobalDataPointer()->SocSku = Sku;
}

/**
  This function retrieves current SOC SKU info.

  @retval    The current SOC SKU.

**/
UINT32
EFIAPI
GetSocSku (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->SocSku;
}

/**
  This function sets current debug port index.

  @param DebugPortIdx   The debug port index.

**/
VOID
EFIAPI
SetDebugPort (
  IN UINT8   DebugPortIdx
  )
{
  GetLoaderGlobalDataPointer()->DebugPortIdx = DebugPortIdx;
}


/**
  This function gets current debug port index.

  @retval    The current debug port index.

**/
UINT8
EFIAPI
GetDebugPort (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->DebugPortIdx;
}


/**
  Returns the debug print error level mask for the current module.

  @return  Debug print error level mask for the current module.

**/
UINT32
EFIAPI
GetDebugErrorLevel (
  VOID
  )
{
  return ((LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer())->DebugPrintErrorLevel;
}


/**
  Sets the global debug print error level mask fpr the entire platform.

  @param   ErrorLevel     Global debug print error level.

**/
VOID
EFIAPI
SetDebugErrorLevel (
  UINT32  ErrorLevel
  )
{
  ((LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer())->DebugPrintErrorLevel = ErrorLevel;
}

/**
  This function sets current platform BOM id.

  @param BomId   The value of current platform BOM id.

**/
VOID
EFIAPI
SetPlatformBomId (
  IN UINT16   BomId
  )
{
  ((LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer())->PlatformBomId = BomId;
}

/**
  This function retrieves current platform name.

  @retval    The current platform name.

**/
VOID *
EFIAPI
GetPlatformName (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->PlatformName;
}

/**
  This function sets current platform name.

  @param PlatformId   The value of current platform name.

**/
VOID
EFIAPI
SetPlatformName (
  IN VOID     *PlatformName
  )
{
  CopyMem (GetLoaderGlobalDataPointer()->PlatformName, PlatformName, PLATFORM_NAME_SIZE);
}

/**

  Sets the pointer to the HOB List.

  @param HobList                       Pointer to the HOB List.

**/
VOID
EFIAPI
SetHobList (
  IN VOID          *HobList
  )
{
  GetLoaderGlobalDataPointer()->LdrHobList = HobList;
}


/**
  This function retrieves DMA buffer base.

  @retval    DMA buffer base.

**/
VOID *
EFIAPI
GetDmaBufferPtr (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->DmaBufferPtr;
}

/**
  This function retrieves system memory info the given type.

  @param[in] MemInfoType   Memory info type to retrieve.

  @retval    Value of the required memory info type.
             It returns 0 if the required type is invalid.

**/
UINT64
EFIAPI
GetMemoryInfo (
  IN  MEM_INFO_TYPE   MemInfoType
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;

  LdrGlobal = GetLoaderGlobalDataPointer();
  if ((LdrGlobal != NULL) && (MemInfoType < EnumMemInfoMax)) {
    return LdrGlobal->MemoryInfo[MemInfoType];
  } else {
    return 0;
  }
}

/**
  This function sets system memory info for the given type.

  @param[in] MemInfoType   Memory info type to retrieve.
  @param[in] Value         The value to set.

**/
VOID
EFIAPI
SetMemoryInfo (
  IN  MEM_INFO_TYPE   MemInfoType,
  IN  UINT64          Value
  )
{
  LOADER_GLOBAL_DATA  *LdrGlobal;

  LdrGlobal = GetLoaderGlobalDataPointer();
  if ((LdrGlobal != NULL) && (MemInfoType < EnumMemInfoMax)) {
    LdrGlobal->MemoryInfo[MemInfoType] = Value;
  }
}

/**
 This function retrieves TempRam Base and Size reported from FSP-T.

  @param[out] Base          Base address of TempRam
  @param[out] Size          Size of TempRam

  @retval EFI_SUCCESS       Retrieved TempRam Base and Size
  @retval EFI_UNSUPPORTED   No valid info exists

**/
EFI_STATUS
EFIAPI
GetTempRamInfo (
  OUT UINT32  *Base,
  OUT UINT32  *Size
  )
{
  UINT32  CarBase;
  UINT32  CarSize;

  CarBase = GetLoaderGlobalDataPointer()->CarBase;
  CarSize = GetLoaderGlobalDataPointer()->CarSize;

  if ((CarBase == 0) || (CarSize == 0)) {
    return EFI_UNSUPPORTED;
  }

  if (Base != NULL) {
    *Base = CarBase;
  }
  if (Size != NULL) {
    *Size = CarSize;
  }

  return EFI_SUCCESS;
}

/**
  This function retrieves the address of S3Data.

  @retval    S3Data pointer address.

**/
VOID *
EFIAPI
GetS3DataPtr (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->S3DataPtr;
}

/**
  This function sets features configuration.

  @param Features   The features the platform supports.

**/
VOID
EFIAPI
SetFeatureCfg (
  IN  UINT32  Features
  )
{
  GetLoaderGlobalDataPointer()->LdrFeatures = Features;
}

/**
  This function clears FSP Hob Data and pointer.

**/
VOID
EFIAPI
ClearFspHob (
  VOID
  )
{
  VOID                        *FspHobList;
  EFI_HOB_HANDOFF_INFO_TABLE  *HandOffHob;
  UINT32                       Length;

  FspHobList = GetLoaderGlobalDataPointer()->FspHobList;
  if (FspHobList != NULL) {
    HandOffHob = (EFI_HOB_HANDOFF_INFO_TABLE *) FspHobList;
    Length     = (UINT32)((UINTN)HandOffHob->EfiEndOfHobList - (UINTN)HandOffHob);
    ZeroMem (HandOffHob, Length);

    GetLoaderGlobalDataPointer()->FspHobList = NULL;
  }
}

/**
  This function retrieves the Version Information pointer.

  @retval    Version Information pointer address.

**/
VOID *
EFIAPI
GetVerInfoPtr (
  VOID
  )
{
  return GetLoaderGlobalDataPointer()->VerInfoPtr;
}
