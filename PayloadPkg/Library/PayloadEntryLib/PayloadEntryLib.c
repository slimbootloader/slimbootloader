/** @file
  This file provides payload common library interfaces.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
#include <Library/DebugLogBufferLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Guid/BootLoaderServiceGuid.h>
#include <Guid/BootLoaderVersionGuid.h>
#include <Guid/LoaderPlatformDataGuid.h>
#include <Guid/LoaderPlatformInfoGuid.h>
#include <Library/GraphicsLib.h>

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
  UINT32                    StackBase;
  UINT32                    StackSize;
  LOADER_PLATFORM_DATA      *LoaderPlatformData;

  PcdSet32 (PcdPayloadHobList, (UINT32)HobList);

  //
  // Payload Memmap
  // +--------------------------------------------+ TOLUM
  // |   Reserved memory for Slimboot core        |
  // +--------------------------------------------+ RsvdBase + RsvdSize
  // |   Reserved memory for Payload              |
  // +--------------------------------------------+ RsvdBase
  // |   + Payload heap                           |
  // +--------------------------------------------+ HeapBase
  // |   + Payload stack                          |
  // +--------------------------------------------+ StackBase
  // |   Free memory                              |
  // +--------------------------------------------+ 0
  //

  GetPayloadReservedRamRegion (&RsvdBase, &RsvdSize);
  ASSERT ((RsvdBase & EFI_PAGE_MASK) == 0);

  HeapSize  = ALIGN_UP (PcdGet32 (PcdPayloadHeapSize), EFI_PAGE_SIZE);
  HeapBase = (UINT32)RsvdBase - HeapSize;

  StackSize = ALIGN_UP (PcdGet32 (PcdPayloadStackSize), EFI_PAGE_SIZE);
  StackBase = HeapBase - StackSize;

  // Add payload reserved memory region and free memory region
  AddMemoryResourceRange (HeapBase, EFI_SIZE_TO_PAGES (HeapSize), \
                          RsvdBase, EFI_SIZE_TO_PAGES ((UINT32)RsvdSize));

  GlobalDataPtr = AllocateZeroPool (sizeof (PAYLOAD_GLOBAL_DATA));
  GlobalDataPtr->Signature = PLD_GDATA_SIGNATURE;
  PcdSet32 (PcdGlobalDataAddress, (UINT32) (UINTN)GlobalDataPtr);

  // Create Debug Log Buffer and init configuration data
  GuidHob = GetNextGuidHob (&gLoaderPlatformDataGuid, (VOID *)PcdGet32 (PcdPayloadHobList));
  if (GuidHob != NULL) {
    LoaderPlatformData = (LOADER_PLATFORM_DATA *) GET_GUID_HOB_DATA (GuidHob);
    GlobalDataPtr->CfgDataPtr = LoaderPlatformData->ConfigDataPtr;

    DebugLogBufferHdr  = LoaderPlatformData->DebugLogBuffer;
    if (DebugLogBufferHdr->Signature == DEBUG_LOG_BUFFER_SIGNATURE) {
      BufPtr = AllocatePool (DebugLogBufferHdr->TotalLength);
      CopyMem (BufPtr, DebugLogBufferHdr, DebugLogBufferHdr->UsedLength);
      GlobalDataPtr->LogBufPtr = BufPtr;
    }
  }

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

**/
VOID
EFIAPI
SecStartup (
  IN VOID                   *Params
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
  UINT32                     HeapBase;
  UINT32                     StackBase;
  UINT32                     StackSize;
  LOADER_PLATFORM_INFO       *LoaderPlatformInfo;

  TimeStamp = AsmReadTsc();

  PayloadInit (Params, &HeapBase, &StackBase, &StackSize);
  GlobalDataPtr = (PAYLOAD_GLOBAL_DATA *)PcdGet32 (PcdGlobalDataAddress);

  // DEBUG will be available after PayloadInit ()
  DEBUG ((DEBUG_INIT, "\nPayload startup\n"));

  // Copy libraries data
  GuidHob = GetNextGuidHob (&gLoaderLibraryDataGuid, (VOID *)PcdGet32 (PcdPayloadHobList));
  if (GuidHob != NULL) {
    LoaderLibData = (LOADER_LIBRARY_DATA *)GET_GUID_HOB_DATA (GuidHob);
    LibData       = (LIBRARY_DATA *) LoaderLibData->Data;
    AllocateLen   = 0;
    for (Idx = 0; Idx < LoaderLibData->Count; Idx++) {
      AllocateLen += ALIGN_UP (LibData[Idx].BufSize, sizeof (UINTN));
    }

    AllocateLen += LoaderLibData->Count * sizeof (LIBRARY_DATA);
    BufPtr = AllocatePool (AllocateLen);

    CopyMem (BufPtr, LibData, LoaderLibData->Count * sizeof (LIBRARY_DATA));
    LibData = (LIBRARY_DATA *) BufPtr;
    BufPtr += LoaderLibData->Count * sizeof (LIBRARY_DATA);

    if (AllocateLen > 0) {
      for (Idx = 0; Idx < LoaderLibData->Count; Idx++) {
        if (LibData[Idx].BufBase != 0) {
          CopyMem (BufPtr, (VOID *)LibData[Idx].BufBase, LibData[Idx].BufSize);
          LibData[Idx].BufBase = (UINT32)BufPtr;
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
  SystemTableInfo = GetSystemTableInfo();
  ParseAcpiTableInfo ((UINT32)SystemTableInfo->AcpiTableBase);

  DEBUG_CODE_BEGIN ();
  // Initialize HOB/Stack region with known pattern so that the usage can be detected
  SetMem32 (
    (VOID *)StackBase,
    StackSize,
    STACK_DEBUG_FILL_PATTERN
    );
  DEBUG_CODE_END ();

  SwitchStack ((SWITCH_STACK_ENTRY_POINT)PayloadMain, &Params, NULL, (VOID *) (UINTN)HeapBase);
}
