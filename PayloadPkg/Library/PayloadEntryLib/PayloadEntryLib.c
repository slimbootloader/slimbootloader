/** @file
  This file provides payload common library interfaces.

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PayloadLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/LoaderPerformanceLib.h>
#include <Library/PayloadMemoryAllocationLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/ContainerLib.h>
#include <Library/TimeStampLib.h>
#include <Library/GraphicsLib.h>
#include <Library/PagingLib.h>
#include <Guid/BootLoaderVersionGuid.h>
#include <Guid/LoaderPlatformInfoGuid.h>
#include <Guid/PciRootBridgeInfoGuid.h>

/**
  Initialize critical payload global data.

  @param[in]      HobList         The HOB list pointer for payload.
  @param[in, out] PldHeapBase     Payload heap base pointer.
  @param[in, out] PldStatckBase   Payload stack base pointer.
  @param[in, out] PldStatckSize   Payload stack size pointer.

**/
STATIC
VOID
PayloadInit (
  IN     VOID                   *HobList,
  IN OUT UINT32                 *PldHeapBase,
  IN OUT UINT32                 *PldStatckBase,
  IN OUT UINT32                 *PldStatckSize
  )
{
  PAYLOAD_GLOBAL_DATA       *GlobalDataPtr;
  EFI_HOB_GUID_TYPE         *GuidHob;
  UINT8                     *BufPtr;
  DEBUG_LOG_BUFFER_HEADER   *DebugLogBufferHdr;
  UINT32                    HeapBase;
  UINT32                    HeapSize;
  UINT64                    RsvdBase;
  UINT64                    RsvdSize;
  UINT32                    DmaBase;
  UINT32                    DmaSize;
  UINT32                    StackBase;
  UINT32                    StackSize;
  LOADER_PLATFORM_DATA      *LoaderPlatformData;
  EFI_STATUS                PcdStatus1;
  EFI_STATUS                PcdStatus2;
  CONTAINER_LIST            *ContainerList;
  EFI_MEMORY_RANGE_ENTRY    MemoryRanges[3];

  PcdStatus1 = PcdSet32S (PcdPayloadHobList, (UINT32)(UINTN)HobList);

  //
  // Payload Memmap
  // +--------------------------------------------+ TOLUM
  // |   Reserved memory for Slimboot core        |
  // +--------------------------------------------+ RsvdBase + RsvdSize
  // |   Reserved memory for Payload              |
  // +--------------------------------------------+ RsvdBase
  // |   + DMA buffer                             |
  // +--------------------------------------------+ DmaBase
  // |   + Payload heap                           |
  // +--------------------------------------------+ HeapBase
  // |   + Payload stack                          |
  // +--------------------------------------------+ StackBase
  // |   Free memory                              |
  // +--------------------------------------------+ 0
  //

  GetPayloadReservedRamRegion (&RsvdBase, &RsvdSize);
  ASSERT ((RsvdBase & EFI_PAGE_MASK) == 0);

  if (FeaturePcdGet (PcdDmaProtectionEnabled)) {
    DmaSize = ALIGN_UP (PcdGet32 (PcdDmaBufferSize), EFI_PAGE_SIZE);
  } else {
    DmaSize = 0;
  }
  DmaBase  = (UINT32)RsvdBase - DmaSize;
  DmaBase  = ALIGN_DOWN(DmaBase, PcdGet32 (PcdDmaBufferAlignment));

  HeapSize  = ALIGN_UP (PcdGet32 (PcdPayloadHeapSize), EFI_PAGE_SIZE);
  HeapBase = DmaBase - HeapSize;

  StackSize = ALIGN_UP (PcdGet32 (PcdPayloadStackSize), EFI_PAGE_SIZE);
  StackBase = HeapBase - StackSize;

  // Add payload reserved memory region and free memory region
  // Use EfiRuntimeServicesData as DMA memory pool
  MemoryRanges[0].BaseAddress   = HeapBase;
  MemoryRanges[0].NumberOfPages = EFI_SIZE_TO_PAGES (HeapSize);
  MemoryRanges[0].Type          = EfiBootServicesData;
  MemoryRanges[1].BaseAddress   = RsvdBase;
  MemoryRanges[1].NumberOfPages = EFI_SIZE_TO_PAGES ((UINT32)RsvdSize);
  MemoryRanges[1].Type          = EfiReservedMemoryType;
  MemoryRanges[2].BaseAddress   = DmaBase;
  MemoryRanges[2].NumberOfPages = EFI_SIZE_TO_PAGES (DmaSize);
  MemoryRanges[2].Type          = EfiRuntimeServicesData;
  AddMemoryResourceRange (MemoryRanges, 3);

  GlobalDataPtr = AllocateZeroPool (sizeof (PAYLOAD_GLOBAL_DATA));
  ASSERT (GlobalDataPtr != NULL);
  GlobalDataPtr->Signature = PLD_GDATA_SIGNATURE;
  PcdStatus2 = PcdSet32S (PcdGlobalDataAddress, (UINT32) (UINTN)GlobalDataPtr);
  ASSERT_EFI_ERROR (PcdStatus1 | PcdStatus2);

  // Create Debug Log Buffer and init configuration data
  GuidHob = GetNextGuidHob (&gLoaderPlatformDataGuid, (VOID *)(UINTN)PcdGet32 (PcdPayloadHobList));
  if (GuidHob != NULL) {
    LoaderPlatformData = (LOADER_PLATFORM_DATA *) GET_GUID_HOB_DATA (GuidHob);

    if (LoaderPlatformData->DmaBufferPtr != NULL) {
      // Verify the DMA buffer is aligned with core allocation
      ASSERT ((UINT32)(UINTN)LoaderPlatformData->DmaBufferPtr == DmaBase);
    }

    GlobalDataPtr->CfgDataPtr = LoaderPlatformData->ConfigDataPtr;

    DebugLogBufferHdr  = LoaderPlatformData->DebugLogBuffer;
    if (DebugLogBufferHdr->Signature == DEBUG_LOG_BUFFER_SIGNATURE) {
      BufPtr = AllocatePool (DebugLogBufferHdr->TotalLength);
      CopyMem (BufPtr, DebugLogBufferHdr, DebugLogBufferHdr->UsedLength);
      GlobalDataPtr->LogBufPtr = BufPtr;
    }

    ContainerList = LoaderPlatformData->ContainerList;
    if (ContainerList != NULL) {
      BufPtr = AllocatePool (ContainerList->TotalLength);
      CopyMem (BufPtr, ContainerList, ContainerList->TotalLength);
      GlobalDataPtr->ContainerList = ContainerList;
    }

  }

  PlatformHookSerialPortInitialize ();
  SerialPortInitialize ();

  if (PldHeapBase != NULL) {
    *PldHeapBase = HeapBase;
  }

  if (PldStatckBase != NULL) {
    *PldStatckBase = StackBase;
  }

  if (PldStatckSize != NULL) {
    *PldStatckSize = StackSize;
  }
}

/**
  The payload common Entry Point for C code.

  @param[in] Params         The HOB list pointer for payload.
  @param[in] PldBase        Address of the payload image base.

**/
VOID
EFIAPI
SecStartup (
  IN VOID                   *Params,
  IN VOID                   *PldBase
  )
{
  PAYLOAD_GLOBAL_DATA       *GlobalDataPtr;
  SYSTEM_TABLE_INFO         *SystemTableInfo;
  UINT64                    TimeStamp;
  RETURN_STATUS             Status;
  EFI_HOB_GUID_TYPE         *GuidHob;
  LOADER_LIBRARY_DATA       *LoaderLibData;
  UINT32                    AllocateLen;
  UINT32                    Idx;
  UINT8                     *BufPtr;
  LIBRARY_DATA              *LibData;
  VOID                      *PcdDataPtr;
  BOOT_LOADER_SERVICES_LIST *BldServicesList;
  UINT32                    HeapBase;
  UINT32                    StackBase;
  UINT32                    StackSize;
  LOADER_PLATFORM_INFO      *LoaderPlatformInfo;

  TimeStamp = ReadTimeStamp ();

  PayloadInit (Params, &HeapBase, &StackBase, &StackSize);
  GlobalDataPtr = (PAYLOAD_GLOBAL_DATA *)(UINTN)PcdGet32 (PcdGlobalDataAddress);

  // DEBUG will be available after PayloadInit ()
  DEBUG ((DEBUG_INIT, "\nPayload startup\n"));

  // Copy libraries data
  GuidHob = GetNextGuidHob (&gLoaderLibraryDataGuid, (VOID *)(UINTN)PcdGet32 (PcdPayloadHobList));
  if (GuidHob != NULL) {
    LoaderLibData = (LOADER_LIBRARY_DATA *)GET_GUID_HOB_DATA (GuidHob);
    LibData       = (LIBRARY_DATA *) LoaderLibData->Data;
    AllocateLen   = 0;
    for (Idx = 0; Idx < LoaderLibData->Count; Idx++) {
      AllocateLen += ALIGN_UP (LibData[Idx].BufSize, sizeof (UINTN));
    }

    AllocateLen += LoaderLibData->Count * sizeof (LIBRARY_DATA);
    BufPtr = AllocatePool (AllocateLen);
    ASSERT (BufPtr != NULL);

    CopyMem (BufPtr, LibData, LoaderLibData->Count * sizeof (LIBRARY_DATA));
    LibData = (LIBRARY_DATA *) BufPtr;
    BufPtr += LoaderLibData->Count * sizeof (LIBRARY_DATA);

    if (AllocateLen > 0) {
      for (Idx = 0; Idx < LoaderLibData->Count; Idx++) {
        if (LibData[Idx].BufBase != 0) {
          CopyMem (BufPtr, (VOID *)(UINTN)LibData[Idx].BufBase, LibData[Idx].BufSize);
          LibData[Idx].BufBase = (UINT32)(UINTN)BufPtr;
          BufPtr += ALIGN_UP (LibData[Idx].BufSize, sizeof (UINTN));
        }
      }
    }
    GlobalDataPtr->LibDataPtr = (VOID *) LibData;
  }

  //
  // Get PCD data base
  //
  Status = GetLibraryData (PcdGet8 (PcdPcdLibId), (VOID **)&PcdDataPtr);
  if (!EFI_ERROR (Status)) {
    GlobalDataPtr->PcdDataPtr = PcdDataPtr;
  }

  //
  // Init boot loader service
  //
  GuidHob = GetNextGuidHob (&gBootLoaderServiceGuid, GetHobListPtr());
  if (GuidHob != NULL) {
    BldServicesList = (BOOT_LOADER_SERVICES_LIST *)GET_GUID_HOB_DATA (GuidHob);
    GlobalDataPtr->ServiceList = &BldServicesList->ServiceList;
  }

  // Get public key hash from HOB
  GuidHob = GetNextGuidHob (&gPayloadKeyHashGuid, GetHobListPtr());
  if (GuidHob != NULL) {
    GlobalDataPtr->HashStorePtr = GET_GUID_HOB_DATA (GuidHob);
  }

  // Init features
  LoaderPlatformInfo = (LOADER_PLATFORM_INFO  *) GetGuidHobData (NULL, NULL, &gLoaderPlatformInfoGuid);
  if (LoaderPlatformInfo != NULL) {
    GlobalDataPtr->LdrFeatures = LoaderPlatformInfo->LdrFeatures;
  }

  // Init device table
  GlobalDataPtr->DeviceTable =  GetGuidHobData (NULL, NULL, &gDeviceTableHobGuid);

  //
  // GetLoaderPerformanceInfo() function
  //
  GetLoaderPerformanceInfo (&GlobalDataPtr->PerfData);
  AddMeasurePointTimestamp (0x4000, TimeStamp);

  // ACPI table
  SystemTableInfo = GetSystemTableInfo ();
  if ((SystemTableInfo != NULL) && (SystemTableInfo->AcpiTableBase != 0)) {
    ParseAcpiTableInfo ((UINT32)SystemTableInfo->AcpiTableBase);
  }

  DEBUG_CODE_BEGIN ();
  // Initialize HOB/Stack region with known pattern so that the usage can be detected
  SetMem32 (
    (VOID *)(UINTN)StackBase,
    StackSize,
    STACK_DEBUG_FILL_PATTERN
    );
  DEBUG_CODE_END ();

  SwitchStack ((SWITCH_STACK_ENTRY_POINT)PayloadMain, &Params, PldBase, (VOID *) (UINTN)HeapBase);
}
