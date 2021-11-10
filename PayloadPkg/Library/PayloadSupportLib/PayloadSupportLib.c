/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/PayloadLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Acpi.h>
#include <Mcfg.h>
#include <FspEas/FspApi.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/FspHeaderFile.h>
#include <Guid/BootLoaderServiceGuid.h>
#include <Guid/LoaderPlatformInfoGuid.h>

/**
  Returns the System table info HOB data.

  @retval   Pointer to the system table info hob

**/
SYSTEM_TABLE_INFO *
GetSystemTableInfo (
  VOID
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;

  GuidHob = GetNextGuidHob (&gLoaderSystemTableInfoGuid, (VOID *)(UINTN)PcdGet32 (PcdPayloadHobList));
  if (GuidHob == NULL) {
    ASSERT (GuidHob);
    return NULL;
  }

  return (SYSTEM_TABLE_INFO *)GET_GUID_HOB_DATA (GuidHob);
}

/**
  Returns the pointer to the Loader Platform Information.

  If the pointer to the HOB list is NULL, then ASSERT().

  @return The pointer to the HOB list.

**/
VOID *
EFIAPI
GetLoaderPlatformInfoPtr (
  VOID
  )
{
  LOADER_PLATFORM_INFO  *LoaderInfo;
  EFI_HOB_GUID_TYPE     *GuidHob;

  GuidHob = GetNextGuidHob (&gLoaderPlatformInfoGuid, (VOID *)(UINTN)PcdGet32 (PcdPayloadHobList));
  if (GuidHob == NULL) {
    return NULL;
  }

  LoaderInfo = (LOADER_PLATFORM_INFO *)GET_GUID_HOB_DATA (GuidHob);
  if (LoaderInfo == NULL) {
    return NULL;
  }

  return LoaderInfo;
}

/**
  function to parse ACPI table.

  @param    AcpiBase   Acpi Base adress

**/
VOID
ParseAcpiTableInfo (
  UINT32  AcpiBase
  )
{
  EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp;
  EFI_ACPI_DESCRIPTION_HEADER                   *Xsdt;
  UINT64                                        *Entry64;
  EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE     *Fadt;
  EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE *Mcfg;
  UINTN                                         Entry64Num;
  UINTN                                         Idx;
  EFI_STATUS                                    Status;

  Rsdp = (EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER *) (UINTN)AcpiBase;
  if (Rsdp != NULL) {
    Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *) (UINTN) (Rsdp->XsdtAddress);
  } else {
    Xsdt = NULL;
  }
  ASSERT (Xsdt != NULL);
  if (Xsdt == NULL) {
    return;
  }

  Fadt = NULL;
  Mcfg = NULL;

  //
  // Search Xsdt only, assume Xsdt is always used
  //
  Status = EFI_SUCCESS;
  Entry64 = (UINT64 *) (Xsdt + 1);
  Entry64Num = (Xsdt->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) >> 3;
  for (Idx = 0; Idx < Entry64Num; Idx++) {
    if (* (UINT32 *) (UINTN) (Entry64[Idx]) == EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
      Fadt = (EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE *) (UINTN) (Entry64[Idx]);
      Status = PcdSet16S (PcdAcpiPmTimerBase, (UINT16)Fadt->PmTmrBlk);
      DEBUG ((DEBUG_INFO, "ACPI PmTimer Base: 0x%x\n", PcdGet16 (PcdAcpiPmTimerBase)));
    } else if (* (UINT32 *) (UINTN) (Entry64[Idx]) ==
               EFI_ACPI_5_0_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE) {
      Mcfg = (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE *) (UINTN) (Entry64[Idx]);
      Status = PcdSet64S (PcdPciExpressBaseAddress, Mcfg->Segment.BaseAddress);
      DEBUG ((DEBUG_INFO, "PCI Express  Base: 0x%x\n", (UINT32)PcdGet64 (PcdPciExpressBaseAddress)));
    }
  }

  ASSERT_EFI_ERROR (Status);
  ASSERT (Fadt || Mcfg);
}


/**
  Returns the memory map info HOB data.

  @retval   Pointer to the memory map info hob

**/
MEMORY_MAP_INFO *
EFIAPI
GetMemoryMapInfo (
  VOID
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;

  GuidHob = GetNextGuidHob (&gLoaderMemoryMapInfoGuid, (VOID *)(UINTN)PcdGet32 (PcdPayloadHobList));
  if (GuidHob == NULL) {
    ASSERT (GuidHob);
    return NULL;
  }
  return (MEMORY_MAP_INFO *)GET_GUID_HOB_DATA (GuidHob);
}

/**
  Returns the maximum available memory region.

  Find available memory region from memory map info hob

  @param[out] Base  Base address of the available memory region.
  @param[out] Size  Size of the available memory region.

  @retval RETURN_SUCCESS        Found the memory map entry.
  @retval RETURN_NOT_FOUND      Memory map entry not found.

**/
RETURN_STATUS
EFIAPI
GetMaxAvailableRamRegion (
  UINT64  *Base,
  UINT64  *Size
  )
{
  RETURN_STATUS      Status;
  MEMORY_MAP_INFO   *MemoryMapInfo;
  UINT8              Index;
  UINT8              MaxSizeIndex;

  Status        = RETURN_NOT_FOUND;
  Index         = 0;
  MemoryMapInfo = NULL;
  MaxSizeIndex  = 0;

  //
  // Get memory map information
  //
  MemoryMapInfo = GetMemoryMapInfo ();
  ASSERT (MemoryMapInfo);

  for (Index = 0; Index < MemoryMapInfo->Count; Index++) {
    if ((MemoryMapInfo->Entry[Index].Type == MEM_MAP_TYPE_RAM) \
        && (MemoryMapInfo->Entry[Index].Base < 0x100000000UL) \
        && (MemoryMapInfo->Entry[MaxSizeIndex].Size < (UINT32)MemoryMapInfo->Entry[Index].Size)) {
      //
      // Found available memory map entry
      //
      MaxSizeIndex = Index;
      Status = RETURN_SUCCESS;
    }
  }
  ASSERT (Status == RETURN_SUCCESS);

  *Base = MemoryMapInfo->Entry[MaxSizeIndex].Base;
  *Size = MemoryMapInfo->Entry[MaxSizeIndex].Size;

  return Status;
}

/**
  Returns the payload reserved memory region.

  Find payload reserved memory region from memory map info hob

  @param[out] Base  Base address of reserved memory region.
  @param[out] Size  Size of the reserved memory region.

  @retval RETURN_SUCCESS        Found the memory map entry.
  @retval RETURN_NOT_FOUND      Memory map entry not found.

**/
RETURN_STATUS
EFIAPI
GetPayloadReservedRamRegion (
  UINT64  *Base,
  UINT64  *Size
  )
{
  RETURN_STATUS      Status;
  MEMORY_MAP_INFO   *MemoryMapInfo;
  UINT8              Index;
  UINT64             ReservedBase;
  UINT64             ReservedSize;

  Status        = RETURN_NOT_FOUND;
  ReservedBase  = 0;
  ReservedSize  = 0;

  //
  // Get memory map information
  //
  MemoryMapInfo = GetMemoryMapInfo ();
  if (MemoryMapInfo != NULL) {
    for (Index = 0; Index < MemoryMapInfo->Count; Index++) {
      if ((MemoryMapInfo->Entry[Index].Type == MEM_MAP_TYPE_RESERVED) \
          && (MemoryMapInfo->Entry[Index].Flag & MEM_MAP_FLAG_PAYLOAD)) {
        //
        // Found payload memory map entry
        //
        ReservedBase = MemoryMapInfo->Entry[Index].Base;
        ReservedSize = MemoryMapInfo->Entry[Index].Size;
        Status = RETURN_SUCCESS;
        break;
      }
    }
  }

  if (Base != NULL) {
    *Base = ReservedBase;
  }

  if (Size != NULL) {
    *Size = ReservedSize;
  }

  return Status;
}

/**
  Returns the timestamps data.

  Finds timestamps data from loader performance info hob

  @param[out] PerfData  Pointer to loader performance data.

  @retval EFI_SUCCESS        Found the timestamps data.
  @retval RETURN_NOT_FOUND   Loader performance hob not found.

**/
EFI_STATUS
GetLoaderPerformanceInfo (
  BL_PERF_DATA  *PerfData
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;
  PERFORMANCE_INFO              *PerfInfo;

  GuidHob = GetNextGuidHob (&gLoaderPerformanceInfoGuid, (VOID *)(UINTN)PcdGet32 (PcdPayloadHobList));
  if (GuidHob == NULL) {
    return RETURN_NOT_FOUND;
  }

  PerfInfo            = (PERFORMANCE_INFO *)GET_GUID_HOB_DATA (GuidHob);
  if (PerfData != NULL) {
    PerfData->PerfIndex = PerfInfo->Count;
    PerfData->FreqKhz   = PerfInfo->Frequency;
    CopyMem ((VOID *)PerfData->TimeStamp, (VOID *)PerfInfo->TimeStamp, sizeof (UINT64) * PerfInfo->Count);
  }

  return EFI_SUCCESS;
}

