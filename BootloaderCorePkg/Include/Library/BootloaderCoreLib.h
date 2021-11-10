/** @file

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BOOTLOADER_CORE_LIB_H_
#define _BOOTLOADER_CORE_LIB_H_

#include <Library/BootloaderCommonLib.h>

//
// These MACRO definations should be used only after feature config data is set in stage1b.
//
#define ACPI_FEATURE_ENABLED()   (GetFeatureCfg() & FEATURE_ACPI)
#define ACPI_ENABLED()           (FeaturePcdGet (PcdAcpiEnabled) && ACPI_FEATURE_ENABLED())
#define MEASURED_BOOT_ENABLED()  (FeaturePcdGet (PcdMeasuredBootEnabled) && (GetFeatureCfg() & FEATURE_MEASURED_BOOT) && ACPI_FEATURE_ENABLED())

#define PCD_GET32_WITH_ADJUST(x)  GetAdjustedPcdBase (PcdGet32 (x))

typedef enum {
  EnumMemInfoTom,     // Total system memory size
  EnumMemInfoTolum,   // Top of low usable memory
  EnumMemInfoTouum,   // Top of upper usable memory
  EnumMemInfoTodplm,  // Top of DMA protected low memory
  EnumMemInfoMax
} MEM_INFO_TYPE;

/**
  Returns the pointer to the Fsp HOB list.

  If the pointer to the Fsp HOB list is NULL, then ASSERT().

  @return The pointer to the Fsp HOB list.

**/
VOID *
EFIAPI
GetFspHobListPtr (
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
  This function sets current SOC SKU info.

  @param Sku    The value of current SOC SKU.

**/
VOID
EFIAPI
SetSocSku (
  IN UINT32 Sku
  );

/**
  This function retrieves current SOC SKU info.

  @retval    The current SOC SKU.

**/
UINT32
EFIAPI
GetSocSku (
  VOID
  );

/**
  Returns the debug print error level mask for the current module.

  @return  Debug print error level mask for the current module.

**/
UINT32
EFIAPI
GetDebugErrorLevel (
  VOID
  );


/**
  Sets the global debug print error level mask fpr the entire platform.

  @param   ErrorLevel     Global debug print error level.

**/
VOID
EFIAPI
SetDebugErrorLevel (
  UINT32  ErrorLevel
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
  This function retrieves system memory info the given type.

  @param[in] MemInfoType   Memory info type to retrieve.

  @retval    Value of the required memory info type.
             It returns 0 if the required type is invalid.

**/
UINT64
EFIAPI
GetMemoryInfo (
  IN  MEM_INFO_TYPE   MemInfoType
  );

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
  );

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
  );

/**
  This function retrieves the address of S3Data.

  @retval    S3Data pointer address.

**/
VOID *
EFIAPI
GetS3DataPtr (
  VOID
  );

/**
  This function sets features configuration.

  @param Features   The features the platform supports.

**/
VOID
EFIAPI
SetFeatureCfg (
  IN  UINT32  Features
  );

/**
  This function retrieves the Version Information pointer.

  @retval    Version Information pointer address.

**/
VOID *
EFIAPI
GetVerInfoPtr (
  VOID
  );

/**
  This function clears FSP Hob Data and pointer.

**/
VOID
EFIAPI
ClearFspHob (
  VOID
  );

#endif
