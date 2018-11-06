/** @file

  Copyright (c) 2016 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Stage2.h"

UINT8   mFspPhaseMask;

// Create a platform service
const PLATFORM_SERVICE   mPlatformService = {
  .Header.Signature = PLATFORM_SERVICE_SIGNATURE,
  .Header.Version   = PLATFORM_SERVICE_VERSION,
  .NotifyPhase      = BoardNotifyPhase
};

/**
  Platform notify service.

  @param[in] Phase          Boot phase to notify.

**/
VOID
BoardNotifyPhase (
  IN BOARD_INIT_PHASE   Phase
  )
{
  FSP_INIT_PHASE  FspPhase;
  UINT8           FspPhaseMask;

  // This is board notification from payload
  FspPhaseMask = 0;
  if (Phase == PostPciEnumeration) {
    FspPhase     = EnumInitPhaseAfterPciEnumeration;
    FspPhaseMask = BIT0;
  } else if (Phase == ReadyToBoot) {
    FspPhase     = EnumInitPhaseReadyToBoot;
    FspPhaseMask = BIT1;
  } else if (Phase == EndOfFirmware) {
    FspPhase = EnumInitPhaseEndOfFirmware;
    FspPhaseMask = BIT2;
  }

  if (FspPhaseMask != 0) {
    if ((FspPhaseMask & mFspPhaseMask) == 0) {
      // Only call FSP notify once
      CallFspNotifyPhase (FspPhase);
      mFspPhaseMask |= FspPhaseMask;
    }

    if (FspPhaseMask & (BIT1 | BIT2)) {
      // Only call board init for ReadyToBoot & EndOfFirmware
      BoardInit (Phase);
    }

    if (FspPhaseMask & BIT2) {
      InitializeDebugAgent (DEBUG_AGENT_INIT_DXE_UNLOAD, NULL, NULL);
    }
  }

}

/**
  Display graphical splash screen

  @retval EFI_SUCCESS     Splash screen was successfully displayed
  @retval EFI_UNSUPPORTED Frame buffer access not supported
  @retval EFI_UNSUPPORTED BmpImage is not a valid *.BMP image

**/
EFI_STATUS
EFIAPI
DisplaySplash (
  VOID
  )
{
  EFI_STATUS                          Status;
  VOID                                *SplashLogoBmp;
  FRAME_BUFFER_INFO                   *FrameBuffer;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL_UNION *GopBlt;
  UINTN                               GopBltSize;
  UINTN                               Width, Height;
  INTN                                OffX, OffY;

  // Get framebuffer info
  FrameBuffer = (FRAME_BUFFER_INFO *)GetGuidHobData (NULL, NULL, &gLoaderFrameBufferInfoGuid);
  if (FrameBuffer == NULL) {
    return EFI_UNSUPPORTED;
  }

  // Convert image from BMP format
  GopBlt = NULL;
  SplashLogoBmp = (VOID *) PCD_GET32_WITH_ADJUST (PcdSplashLogoAddress);
  ASSERT (SplashLogoBmp != NULL);
  Status = ConvertBmpToGopBlt (SplashLogoBmp,
                               (VOID **)&GopBlt, &GopBltSize,
                               &Height, &Width);
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  ASSERT (GopBlt != NULL);

  // Check image size
  if ((Width > FrameBuffer->HorizontalResolution)
      || (Height > FrameBuffer->VerticalResolution)) {
    return EFI_BUFFER_TOO_SMALL;
  }

  // Copy image to center of framebuffer
  OffX = (FrameBuffer->HorizontalResolution - Width) / 2;
  OffY = (FrameBuffer->VerticalResolution - Height) / 2;
  Status = BltToFrameBuffer (FrameBuffer, GopBlt, Width, Height, OffX, OffY);

  return Status;
}

/**
  Print out the current memory map information

  @param  MemoryMapInfo    MEMORY_MAP_ENTRY array containing the system momory map.

**/
VOID
PrintMemoryMap (
  IN MEMORY_MAP_INFO      *MemoryMapInfo
  )
{
  UINTN  Idx;

  for (Idx = 0; Idx < MemoryMapInfo->Count; Idx++) {
    DEBUG ((DEBUG_INFO, "MEM: %016lX %016lX %02X %d\n", \
            MemoryMapInfo->Entry[Idx].Base,  MemoryMapInfo->Entry[Idx].Size, \
            MemoryMapInfo->Entry[Idx].Flag,  MemoryMapInfo->Entry[Idx].Type));
  }
}

/**
  Sort memory map entries based upon PhysicalStart, from low to high.

  @param  BufferToSort   A pointer to the MEMORY_MAP_ENTRY array buffer in which firmware
                         places the current memory map.

  @param  Count          Entry count of memory map in the MEMORY_MAP_ENTRY array.

  @param  Buffer         Temporary buffer used for sorting. The required buffer size
                         is MEMORY_MAP_ENTRY.
**/
STATIC
VOID
SortMemoryMap (
  IN OUT   MEMORY_MAP_ENTRY            *BufferToSort,
  IN CONST UINTN                        Count,
  IN       MEMORY_MAP_ENTRY            *Buffer
  )
{
  MEMORY_MAP_ENTRY        *Pivot;
  UINTN                    LoopCount;
  UINTN                    NextSwapLocation;

  if (Count < 2) {
    return;
  }

  NextSwapLocation = 0;

  //
  // pick a pivot (we choose last element)
  //
  Pivot = &BufferToSort[Count - 1];

  //
  // Now get the pivot such that all on "left" are below it
  // and everything "right" are above it
  //
  for ( LoopCount = 0
                    ; LoopCount < Count - 1
        ; LoopCount++
        ) {
    //
    // if the element is less than the pivot
    //
    if (BufferToSort[LoopCount].Base <= Pivot[0].Base) {
      //
      // swap
      //
      CopyMem (Buffer, &BufferToSort[NextSwapLocation], sizeof (MEMORY_MAP_ENTRY));
      CopyMem (&BufferToSort[NextSwapLocation], &BufferToSort[LoopCount], sizeof (MEMORY_MAP_ENTRY));
      CopyMem (&BufferToSort[LoopCount], Buffer, sizeof (MEMORY_MAP_ENTRY));

      //
      // increment NextSwapLocation
      //
      NextSwapLocation++;
    }
  }
  //
  // swap pivot to it's final position (NextSwapLocaiton)
  //
  CopyMem (Buffer, Pivot, sizeof (MEMORY_MAP_ENTRY));
  CopyMem (Pivot, &BufferToSort[NextSwapLocation], sizeof (MEMORY_MAP_ENTRY));
  CopyMem (&BufferToSort[NextSwapLocation], Buffer, sizeof (MEMORY_MAP_ENTRY));

  //
  // Now recurse on 2 paritial lists.  neither of these will have the 'pivot' element
  // IE list is sorted left half, pivot element, sorted right half...
  //
  if (NextSwapLocation >= 2) {
    SortMemoryMap (
      BufferToSort,
      NextSwapLocation,
      Buffer);
  }

  if ((Count - NextSwapLocation - 1) >= 2) {
    SortMemoryMap (
      BufferToSort + (NextSwapLocation + 1),
      Count - NextSwapLocation - 1,
      Buffer);
  }

  return;
}

/**
  Translate the resource HOB into memory map data structure.

  @param ResourceDescriptor     Pointer to a resource HOB.

  @param Param                  Array of MemoryMapInfo structure containing memory map entry.
**/
VOID
EFIAPI
MemResHobCallback (
  IN EFI_HOB_RESOURCE_DESCRIPTOR  *ResourceDescriptor,
  IN VOID                         *Param
  )
{
  MEMORY_MAP_INFO      *MemoryMapInfo;
  UINTN                 Idx;

  MemoryMapInfo = (MEMORY_MAP_INFO *)Param;
  Idx           = MemoryMapInfo->Count;
  if (Idx < PcdGet32 (PcdMemoryMapEntryNumber)) {
    MemoryMapInfo->Entry[Idx].Base = ResourceDescriptor->PhysicalStart;
    MemoryMapInfo->Entry[Idx].Size = ResourceDescriptor->ResourceLength;
    MemoryMapInfo->Entry[Idx].Type = (ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) ? 1 : 2;
    MemoryMapInfo->Entry[Idx].Flag = 0;
    MemoryMapInfo->Count++;
  } else {
    ASSERT (FALSE);
  }
}

/**
  Carve out a bootloader reserved, AcpiNvs, AcpiRecalim and payload reserved memory range
  from the system memory map.

  @param MemoryMapInfo     Array of MemoryMapInfo structure containing memory map entry.

  @retval EFI_SUCCESS           Memory ranges have been splitted successfully.
  @retval EFI_UNSUPPORTED       Required memory range size is not page aligned.
  @retval EFI_OUT_OF_RESOURCES  The system memory map top entry is too small.
  @retval EFI_ABORTED           No memory range has been splitted.

**/
EFI_STATUS
SplitMemroyMap (
  IN OUT  MEMORY_MAP_INFO      *MemoryMapInfo
  )
{
  UINTN            Idx;
  UINTN            NewIdx;
  UINTN            Loop;
  UINT8            Flag;
  UINT8            Type;
  UINT32           Adjust;
  MEMORY_MAP_ENTRY     TempMemoryMap;
  LOADER_GLOBAL_DATA  *LdrGlobal;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();
  NewIdx = MemoryMapInfo->Count;

  for (Idx = 0; Idx < MemoryMapInfo->Count; Idx++) {
    if ((UINT64)LdrGlobal->StackTop == (MemoryMapInfo->Entry[Idx].Base + MemoryMapInfo->Entry[Idx].Size)) {
      //
      // Add bootloader and payload reserved memory map entry
      //
      for (Loop = 0; (Loop < 4) && (NewIdx < PcdGet32 (PcdMemoryMapEntryNumber)); Loop++) {
        Flag   = 0;
        Type   = MEM_MAP_TYPE_RESERVED;
        switch (Loop) {
        case 0:
          // Bootloader reserved memory
          Adjust = LdrGlobal->StackTop - LdrGlobal->MemPoolStart;
          break;
        case 1:
          // ACPI NVS memory
          Adjust = PcdGet32 (PcdLoaderAcpiNvsSize);
          Type   = MEM_MAP_TYPE_ACPI_NVS;
          break;
        case 2:
          // ACPI Reclaim memory
          Adjust = PcdGet32 (PcdLoaderAcpiReclaimSize);
          Type   = MEM_MAP_TYPE_ACPI_RECLAIM;
          break;
        case 3:
          // Payload reserved memory
          Adjust = PcdGet32 (PcdPayloadReservedMemSize);
          Flag   = MEM_MAP_FLAG_PAYLOAD;
        }
        if (Adjust == 0) {
          continue;
        }
        // Adjust size must be page aligned
        if ((Adjust & (EFI_PAGE_SIZE - 1)) != 0) {
          return EFI_UNSUPPORTED;
        }
        if (MemoryMapInfo->Entry[Idx].Size < Adjust) {
          return EFI_OUT_OF_RESOURCES;
        }
        MemoryMapInfo->Entry[Idx].Size   -= Adjust;
        MemoryMapInfo->Entry[NewIdx].Base = MemoryMapInfo->Entry[Idx].Base + MemoryMapInfo->Entry[Idx].Size;
        MemoryMapInfo->Entry[NewIdx].Size = Adjust;
        MemoryMapInfo->Entry[NewIdx].Type = Type;
        MemoryMapInfo->Entry[NewIdx].Flag = Flag;
        NewIdx++;
      }
      break;
    }
  }

  MemoryMapInfo->Entry[NewIdx].Base = PcdGet32(PcdFlashBaseAddress);
  MemoryMapInfo->Entry[NewIdx].Size = PcdGet32(PcdFlashSize);
  MemoryMapInfo->Entry[NewIdx].Type = MEM_MAP_TYPE_RESERVED;
  MemoryMapInfo->Entry[NewIdx].Flag = 0;
  NewIdx++;

  if (NewIdx <= MemoryMapInfo->Count) {
    return EFI_ABORTED;
  }
  MemoryMapInfo->Count = NewIdx;

  SortMemoryMap (MemoryMapInfo->Entry, NewIdx, &TempMemoryMap);

  return EFI_SUCCESS;
}

/**
  Create interface HOBs required by payloads.

  @param Stage2Hob         Stage2 HOB pointer.

  @return                  Bootloader HOB list pointer.
**/
VOID *
EFIAPI
BuildBaseInfoHob (
  IN  STAGE2_HOB  *Stage2Hob
  )
{
  SERIAL_PORT_INFO          *SerialPortInfo;
  FRAME_BUFFER_INFO         *FrameBufferInfo;
  LOADER_FSP_INFO           *LoaderFspInfo;
  MEMORY_MAP_INFO           *MemoryMapInfo;
  FRAME_BUFFER_INFO          FrameBuffer;
  UINT32                     Length;
  LOADER_GLOBAL_DATA        *LdrGlobal;
  EFI_PEI_GRAPHICS_INFO_HOB *FspGfxHob;
  EFI_STATUS                 Status;
  PAYLOAD_KEY_HASH          *HashHob;
  UINT32                     HobDataSize;
  CONST UINT8                *PubKeyHash;
  UINT8                      Index;
  UINT8                      CompType;
  EXT_BOOT_LOADER_VERSION   *VersionHob;
  SEED_LIST_INFO_HOB        *SeedListInfoHob;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();

  // Build Loader FSP info hob
  LoaderFspInfo = BuildGuidHob (&gLoaderFspInfoGuid, sizeof (LOADER_FSP_INFO));
  if (LoaderFspInfo != NULL) {
    LoaderFspInfo->FspsBase   = PCD_GET32_WITH_ADJUST (PcdFSPSBase);
    LoaderFspInfo->FspHobList = LdrGlobal->FspHobList;
  }

  // Build serial port hob
  SerialPortInfo = BuildGuidHob (&gLoaderSerialPortInfoGuid, sizeof (SERIAL_PORT_INFO));
  if (SerialPortInfo != NULL) {
    SerialPortInfo->Type        = 1;
    SerialPortInfo->BaseAddr    = 0x3F8;
    SerialPortInfo->Baud        = 115200;
    SerialPortInfo->RegWidth    = 1;
    SerialPortInfo->InputHertz  = 1843200;
    SerialPortInfo->UartPciAddr = 0;
  }

  // Build Frame Buffer hob
  FspGfxHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GetGuidHobData (LdrGlobal->FspHobList, &Length,
              &gEfiGraphicsInfoHobGuid);
  if (FspGfxHob != NULL) {
    FrameBuffer.LinearFrameBuffer    = FspGfxHob->FrameBufferBase;
    FrameBuffer.HorizontalResolution = FspGfxHob->GraphicsMode.HorizontalResolution;
    FrameBuffer.VerticalResolution   = FspGfxHob->GraphicsMode.VerticalResolution;
    FrameBuffer.BitsPerPixel         = 32;
    FrameBuffer.BytesPerScanLine     = (UINT16) (FspGfxHob->GraphicsMode.PixelsPerScanLine * 4);
    FrameBuffer.Red.Position         = 0x10;
    FrameBuffer.Red.Mask             = 0xFF;
    FrameBuffer.Green.Position       = 0x08;
    FrameBuffer.Green.Mask           = 0xFF;
    FrameBuffer.Blue.Position        = 0x00;
    FrameBuffer.Blue.Mask            = 0xFF;
    FrameBuffer.Reserved.Position    = 0x18;
    FrameBuffer.Reserved.Mask        = 0xFF;
  } else {
    FrameBuffer.LinearFrameBuffer    = 0;
  }

  if (FrameBuffer.LinearFrameBuffer) {
    FrameBufferInfo = BuildGuidHob (&gLoaderFrameBufferInfoGuid, sizeof (FRAME_BUFFER_INFO));
    CopyMem (FrameBufferInfo, &FrameBuffer, sizeof (FRAME_BUFFER_INFO));
    DEBUG ((DEBUG_INFO, "Framebuffer Info: %d x %d x 32 @ 0x%08X\n", \
            FrameBuffer.HorizontalResolution, FrameBuffer.VerticalResolution, FrameBuffer.LinearFrameBuffer));
  }

  // Build Memory Map hob
  Length = sizeof (MEMORY_MAP_INFO) + sizeof (MEMORY_MAP_ENTRY) * PcdGet32 (PcdMemoryMapEntryNumber);
  MemoryMapInfo = BuildGuidHob (&gLoaderMemoryMapInfoGuid, Length);
  if (MemoryMapInfo != NULL) {
    SetMem (MemoryMapInfo, Length, 0);
    MemoryMapInfo->Revision = 1;
    TraverseMemoryResourceHob (LdrGlobal->FspHobList, MemResHobCallback, MemoryMapInfo);
    Status = SplitMemroyMap (MemoryMapInfo);
    PrintMemoryMap (MemoryMapInfo);
    if (EFI_ERROR (Status)) {
      CpuHaltWithStatus ("Memory map failure !", Status);
    }
  }

  // Build key hash Hob for Payload
  if (LdrGlobal->HashStorePtr != NULL) {
    HobDataSize = sizeof (PAYLOAD_KEY_HASH) + sizeof (KEY_HASH_ITEM) * MAX_KEY_DIGEST_COUNT;
    HashHob     = BuildGuidHob (&gPayloadKeyHashGuid, HobDataSize);
    if (HashHob != NULL) {
      HashHob->DigestCount = MAX_KEY_DIGEST_COUNT;
      for (Index = 0; Index < MAX_KEY_DIGEST_COUNT; Index++) {
        CompType = COMP_TYPE_PUBKEY_CFG_DATA + Index;
        Status   = GetComponentHash (CompType, &PubKeyHash);
        if (EFI_ERROR (Status)) {
          HashHob->KeyHash[Index].ComponentType = HASH_INDEX_MAX_NUM;
        } else {
          HashHob->KeyHash[Index].ComponentType = CompType;
          CopyMem (&HashHob->KeyHash[Index].Digest, PubKeyHash, SHA256_DIGEST_SIZE);
        }
      }
    }
  }

  // Build boot loader version Hob for Payload
  if (LdrGlobal->VerInfoPtr != NULL) {
    HobDataSize = sizeof (EXT_BOOT_LOADER_VERSION);
    VersionHob  = BuildGuidHob (&gBootLoaderVersionGuid, HobDataSize);
    if (VersionHob != NULL) {
      VersionHob->Revision = 1;
      ZeroMem (&VersionHob->Reserved0, sizeof (VersionHob->Reserved0));
      CopyMem (&VersionHob->Version, LdrGlobal->VerInfoPtr, sizeof (BOOT_LOADER_VERSION));
    }
  }

  // Build Seed List Info hob for Payload
  if (PcdGetBool (PcdSeedListEnabled)) {
    Length = PcdGet32 (PcdSeedListBufferSize);
    SeedListInfoHob = BuildGuidHob (&gSeedListInfoHobGuid, Length);
    if (SeedListInfoHob != NULL) {
      ZeroMem (SeedListInfoHob, Length);
      SeedListInfoHob->Revision       = 1;
      SeedListInfoHob->TotalSeedCount = 0;
      SeedListInfoHob->BufferSize     = Length;
    }
  }
  return LdrGlobal->LdrHobList;
}

/**
  Create interface HOBs required by payloads.

  @param Stage2Hob         Stage2 HOB pointer.

  @return                  Bootloader HOB list pointer.
**/
VOID *
EFIAPI
BuildExtraInfoHob (
  IN  STAGE2_HOB  *Stage2Hob
  )
{
  LOADER_GLOBAL_DATA        *LdrGlobal;
  S3_DATA                   *S3Data;
  SERIAL_PORT_INFO          *SerialPortInfo;
  SYSTEM_TABLE_INFO         *SystemTableInfo;
  SYS_CPU_INFO              *SysCpuInfo;
  PERFORMANCE_INFO          *PerformanceInfo;
  OS_BOOT_OPTION_LIST       *OsBootOptionInfo;
  FRAME_BUFFER_INFO         *FrameBufferInfo;
  LOADER_PLATFORM_INFO      *LoaderPlatformInfo;
  LOADER_PLATFORM_DATA      *LoaderPlatformData;
  LOADER_LIBRARY_DATA       *LoaderLibData;
  UINT32                     Count;
  UINT32                     Length;
  BOOT_LOADER_SERVICES_LIST *HobServicesList;
  SERVICES_LIST             *PldServicesList;
  SERVICES_LIST             *CoreServicesList;
  FLASH_MAP                 *FlashMapPtr;
  FLASH_MAP                 *FlashMapHob;
  UINTN                      Index;
  SEED_LIST_INFO_HOB        *SeedListInfoHob;
  PLT_DEVICE_TABLE          *DeviceTable;
  VOID                      *DeviceTableHob;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();
  S3Data    = (S3_DATA *)LdrGlobal->S3DataPtr;

  // Build library data hob
  LoaderLibData = BuildGuidHob (&gLoaderLibraryDataGuid, sizeof (LOADER_LIBRARY_DATA));
  if (LoaderLibData != NULL) {
    LoaderLibData->Count = (UINT16)PcdGet32 (PcdMaxLibraryDataEntry);
    LoaderLibData->Flags = 0;
    LoaderLibData->Data  = LdrGlobal->LibDataPtr;
  }

  // Update Framebuffer
  FrameBufferInfo = (FRAME_BUFFER_INFO *)GetGuidHobData (NULL, NULL, &gLoaderFrameBufferInfoGuid);
  if (FrameBufferInfo != NULL) {
    PlatformUpdateHobInfo (&gLoaderFrameBufferInfoGuid, FrameBufferInfo);
  }

  // Update serial port hob
  SerialPortInfo = (SERIAL_PORT_INFO *)GetGuidHobData (NULL, NULL, &gLoaderSerialPortInfoGuid);
  if (SerialPortInfo != NULL) {
    PlatformUpdateHobInfo (&gLoaderSerialPortInfoGuid, SerialPortInfo);
  }

  // Build ACPI Hob
  SystemTableInfo = BuildGuidHob (&gLoaderSystemTableInfoGuid, sizeof (SYSTEM_TABLE_INFO));
  if (SystemTableInfo != NULL) {
    SystemTableInfo->AcpiTableBase = S3Data->AcpiBase;
    SystemTableInfo->AcpiTableSize = S3Data->AcpiTop - S3Data->AcpiBase;
    SystemTableInfo->SmbiosTableBase = 0;
    SystemTableInfo->SmbiosTableSize = 0;
    PlatformUpdateHobInfo (&gLoaderSystemTableInfoGuid, SystemTableInfo);
  }

  // Build Loader Platform Data Hob
  LoaderPlatformData = BuildGuidHob (&gLoaderPlatformDataGuid, sizeof (LOADER_PLATFORM_DATA));
  if (LoaderPlatformData != NULL) {
    LoaderPlatformData->Revision = 1;
    LoaderPlatformData->DebugLogBuffer = (DEBUG_LOG_BUFFER_HEADER *) GetDebugLogBufferPtr ();
    LoaderPlatformData->ConfigDataPtr  = GetConfigDataPtr ();
  }

  // Build flash map info hob
  if (FeaturePcdGet (PcdFlashMapEnabled) == TRUE) {
    FlashMapPtr = LdrGlobal->FlashMapPtr;
    FlashMapHob = BuildGuidHob (&gFlashMapInfoGuid, FlashMapPtr->Length);
    if (FlashMapHob != NULL) {
      CopyMem (FlashMapHob, FlashMapPtr, FlashMapPtr->Length);
    }
  }

  // Build OS boot medium info hob
  Length = sizeof (OS_BOOT_OPTION_LIST) + sizeof (OS_BOOT_OPTION) * PcdGet32 (PcdOsBootOptionNumber);
  OsBootOptionInfo = BuildGuidHob (&gOsBootOptionGuid, Length);
  if (OsBootOptionInfo != NULL) {
    ZeroMem (OsBootOptionInfo, Length);
    OsBootOptionInfo->Revision    = 1;
    OsBootOptionInfo->ResetReason = GetResetReason ();
    PlatformUpdateHobInfo (&gOsBootOptionGuid, OsBootOptionInfo);
  }

  // Update seed list hob
  SeedListInfoHob = (SEED_LIST_INFO_HOB *)GetGuidHobData (NULL, NULL, &gSeedListInfoHobGuid);
  if (SeedListInfoHob != NULL) {
    PlatformUpdateHobInfo (&gSeedListInfoHobGuid, SeedListInfoHob);
  }

  // Build boot loader services hob
  Length          = sizeof (BOOT_LOADER_SERVICES_LIST) + sizeof (SERVICE_COMMON_HEADER *) * PcdGet32 (PcdServiceNumber);
  HobServicesList = BuildGuidHob (&gBootLoaderServiceGuid, Length);
  if (HobServicesList != NULL) {
    ZeroMem (HobServicesList, Length);
    HobServicesList->Revision = 1;
    PldServicesList  = &HobServicesList->ServiceList;
    CoreServicesList = (SERVICES_LIST *)GetServiceListPtr ();
    for (Index = 0; Index < CoreServicesList->Count; Index++) {
      if (PldServicesList->Count < PcdGet32 (PcdServiceNumber)) {
        if (CoreServicesList->Header[Index] != NULL) {
          PldServicesList->Header[PldServicesList->Count] = CoreServicesList->Header[Index];
          PldServicesList->Count++;
        }
      }
    }
  }

  // Build device table Hob
  DeviceTable  = (PLT_DEVICE_TABLE *)LdrGlobal->DeviceTable;
  Length = sizeof (PLT_DEVICE_TABLE) + sizeof (PLT_DEVICE) * DeviceTable->DeviceNumber;
  DeviceTableHob = BuildGuidHob (&gDeviceTableHobGuid, Length);
  if (DeviceTableHob != NULL) {
    CopyMem (DeviceTableHob, DeviceTable, Length);
  }

  // Build Performance Hob
  Count  = LdrGlobal->PerfData.PerfIndex;
  Length = sizeof (PERFORMANCE_INFO) + sizeof (UINT64) * Count;
  PerformanceInfo = BuildGuidHob (&gLoaderPerformanceInfoGuid, Length);
  if (PerformanceInfo != NULL) {
    PerformanceInfo->Revision = 1;
    PerformanceInfo->Count = (UINT16)LdrGlobal->PerfData.PerfIndex;
    PerformanceInfo->Frequency = LdrGlobal->PerfData.FreqKhz;
    PerformanceInfo->Flags = 0;
    CopyMem (PerformanceInfo->TimeStamp, LdrGlobal->PerfData.TimeStamp, sizeof (UINT64) * Count);
  }

  // Build Loader Platform info Hob
  Length       = sizeof (LOADER_PLATFORM_INFO);
  LoaderPlatformInfo = BuildGuidHob (&gLoaderPlatformInfoGuid, Length);
  if (LoaderPlatformInfo != NULL) {
    LoaderPlatformInfo->Revision = 1;
    LoaderPlatformInfo->BootPartition = GetCurrentBootPartition ();
    LoaderPlatformInfo->BootMode = GetBootMode ();
    LoaderPlatformInfo->PlatformId = GetPlatformId ();
    LoaderPlatformInfo->LdrFeatures = LdrGlobal->LdrFeatures;
    SysCpuInfo = MpGetInfo ();
    if (SysCpuInfo != NULL) {
      LoaderPlatformInfo->CpuCount = SysCpuInfo->CpuCount;
    } else {
      LoaderPlatformInfo->CpuCount = 1;
    }
    PlatformUpdateHobInfo (&gLoaderPlatformInfoGuid, LoaderPlatformInfo);
  }

  return LdrGlobal->LdrHobList;
}

/**
  Print the stack/HOB and heap usage information.

**/
VOID
EFIAPI
PrintStackHeapInfo (
  VOID
  )
{
  LOADER_GLOBAL_DATA             *LdrGlobal;
  EFI_HOB_HANDOFF_INFO_TABLE     *HandOffHob;
  UINT32                          StackBot;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();
  HandOffHob = (EFI_HOB_HANDOFF_INFO_TABLE *)LdrGlobal->LdrHobList;

  StackBot = DetectUsedStackBottom ((UINT32)HandOffHob->EfiMemoryTop,
                                    (UINT32)HandOffHob->EfiMemoryTop - (UINT32)HandOffHob->EfiFreeMemoryBottom);
  DEBUG ((
           DEBUG_INFO,
           "Stage2 stack: 0x%X (stack used 0x%X, HOB used 0x%X, 0x%X free)\n",
           (UINT32)HandOffHob->EfiMemoryTop - (UINT32)HandOffHob->EfiMemoryBottom,
           (UINT32)HandOffHob->EfiMemoryTop - StackBot,
           (UINT32)HandOffHob->EfiFreeMemoryBottom - (UINT32)HandOffHob->EfiMemoryBottom,
           StackBot - (UINT32)HandOffHob->EfiFreeMemoryBottom
           ));

  DEBUG ((
           DEBUG_INFO,
           "Stage2 heap: 0x%X (0x%X used, 0x%X free)\n",
           LdrGlobal->MemPoolEnd - LdrGlobal->MemPoolStart,
           LdrGlobal->MemPoolEnd - LdrGlobal->MemPoolCurrTop,
           LdrGlobal->MemPoolCurrTop - LdrGlobal->MemPoolStart
           ));
}


/**
  Initialize services so that payload can consume.

**/
VOID
EFIAPI
InitializeService (
  VOID
  )
{
  LOADER_GLOBAL_DATA             *LdrGlobal;
  SERVICES_LIST                  *ServiceList;

  // Initialize HOB
  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();

  // Zero out all services installed in previous stages.
  // Those service might have been expired and stage2 needs to reinstall if required.
  ServiceList = (SERVICES_LIST *)LdrGlobal->ServicePtr;
  if (ServiceList != NULL) {
    ZeroMem (ServiceList->Header, sizeof (SERVICE_COMMON_HEADER *) * ServiceList->Count);
  }

  // Register platform service
  RegisterService ((VOID *)&mPlatformService);

}
