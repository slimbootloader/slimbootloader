/** @file

  Copyright (c) 2016 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2.h"

#define EFI_PCI_ATTRIBUTE_ISA_MOTHERBOARD_IO          0x0001
#define EFI_PCI_ATTRIBUTE_ISA_IO                      0x0002
#define EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO              0x0004
#define EFI_PCI_ATTRIBUTE_VGA_MEMORY                  0x0008
#define EFI_PCI_ATTRIBUTE_VGA_IO                      0x0010
#define EFI_PCI_ATTRIBUTE_IDE_PRIMARY_IO              0x0020
#define EFI_PCI_ATTRIBUTE_IDE_SECONDARY_IO            0x0040
#define EFI_PCI_ATTRIBUTE_MEMORY_WRITE_COMBINE        0x0080
#define EFI_PCI_ATTRIBUTE_MEMORY_CACHED               0x0800
#define EFI_PCI_ATTRIBUTE_MEMORY_DISABLE              0x1000
#define EFI_PCI_ATTRIBUTE_DUAL_ADDRESS_CYCLE          0x8000
#define EFI_PCI_ATTRIBUTE_ISA_IO_16                   0x10000
#define EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO_16           0x20000
#define EFI_PCI_ATTRIBUTE_VGA_IO_16                   0x40000

#define EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM          1
#define EFI_PCI_HOST_BRIDGE_MEM64_DECODE              2

#define EISA_ID(_Name, _Num)      ((UINT32)((_Name) | (_Num) << 16))
#define EISA_PNP_ID(_PNPId)       (EISA_ID(PNP_EISA_ID_CONST, (_PNPId)))


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
  The function is called by PerformQuickSort to sort memory map by its base.

  @param[in] Buffer1         The pointer to first buffer.
  @param[in] Buffer2         The pointer to second buffer.

  @retval 0                  Buffer1 base is less than Buffer2 base.
  @retval 1                  Buffer1 base is greater than or equal to Buffer2 base.

**/
INTN
EFIAPI
CompareMemoryMap (
  IN CONST VOID                 *Buffer1,
  IN CONST VOID                 *Buffer2
  )
{
  if (((MEMORY_MAP_ENTRY *)Buffer1)->Base < ((MEMORY_MAP_ENTRY *)Buffer2)->Base) {
    return  0;
  } else {
    return  1;
  }
}

/**
  Carve out a bootloader reserved, AcpiNvs, AcpiRecalim and payload reserved memory range
  from the system memory map.

  @param MemoryMapInfo     Array of MemoryMapInfo structure containing memory map entry.

  @retval EFI_SUCCESS           Memory ranges have been splitted successfully.
  @retval EFI_NOT_FOUND         Could not find proper entry to split.
  @retval EFI_UNSUPPORTED       Required memory range size is not page aligned.
  @retval EFI_OUT_OF_RESOURCES  The system memory map top entry is too small.
  @retval EFI_ABORTED           Insufficant memory map entry number.
**/
EFI_STATUS
SplitMemroyMap (
  IN OUT  MEMORY_MAP_INFO      *MemoryMapInfo
  )
{
  UINT32           Idx;
  UINT32           NewIdx;
  UINT32           Loop;
  UINT8            Flag;
  UINT8            Type;
  UINT32           Adjust;
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
          break;
        default:
          Adjust = 0;
          break;
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

  // Add a flash map entry
  if (NewIdx < PcdGet32 (PcdMemoryMapEntryNumber)) {
    MemoryMapInfo->Entry[NewIdx].Base = PcdGet32(PcdFlashBaseAddress);
    MemoryMapInfo->Entry[NewIdx].Size = PcdGet32(PcdFlashSize);
    MemoryMapInfo->Entry[NewIdx].Type = MEM_MAP_TYPE_RESERVED;
    MemoryMapInfo->Entry[NewIdx].Flag = 0;
    NewIdx++;
  }
  MemoryMapInfo->Count = NewIdx;

  // Always keep one free entry
  if (NewIdx >= PcdGet32 (PcdMemoryMapEntryNumber)) {
    return EFI_ABORTED;
  }

  return EFI_SUCCESS;
}

/**
  Sort memory map in order and check the gap in between.

  @param MemoryMapInfo     Array of MemoryMapInfo structure containing memory map entry.

  @retval EFI_SUCCESS           Memory ranges have been splitted successfully.
  @retval EFI_UNSUPPORTED       Required memory range size is not page aligned.
  @retval EFI_OUT_OF_RESOURCES  The system memory map top entry is too small.
  @retval EFI_ABORTED           No memory range has been splitted.

**/
EFI_STATUS
SortMemroyMap (
  IN OUT  MEMORY_MAP_INFO      *MemoryMapInfo
  )
{
  UINT32               Idx;
  UINT32               NewIdx;
  MEMORY_MAP_ENTRY     TempMemoryMap;
  UINT64               Current;
  UINT32               Tolum;

  NewIdx = MemoryMapInfo->Count;
  PerformQuickSort (MemoryMapInfo->Entry, NewIdx, sizeof (MEMORY_MAP_ENTRY), CompareMemoryMap, &TempMemoryMap);

  // Verify if there is any gap for memory map entries below top of low memory
  Current = 0;
  Tolum   = (UINT32)GetMemoryInfo (EnumMemInfoTolum);
  for (Idx = 0; Idx < MemoryMapInfo->Count; Idx++) {
    if (MemoryMapInfo->Entry[Idx].Base >= Tolum) {
      break;
    }
    if ((Current < MemoryMapInfo->Entry[Idx].Base) && (NewIdx < PcdGet32 (PcdMemoryMapEntryNumber))) {
      DEBUG ((DEBUG_INFO, "Found a gap in memory map: %08X - %08X\n", Current,  (UINT32)MemoryMapInfo->Entry[Idx].Base - 1));
      MemoryMapInfo->Entry[NewIdx].Base = Current;
      MemoryMapInfo->Entry[NewIdx].Size = MemoryMapInfo->Entry[Idx].Base - Current;
      MemoryMapInfo->Entry[NewIdx].Type = MEM_MAP_TYPE_RESERVED;
      MemoryMapInfo->Entry[NewIdx].Flag = 0;
      NewIdx++;
    }
    Current = MemoryMapInfo->Entry[Idx].Base + MemoryMapInfo->Entry[Idx].Size;
  }

  // Check if it has enough space to cover all memory map
  if (NewIdx >= PcdGet32 (PcdMemoryMapEntryNumber)) {
    return EFI_ABORTED;
  }

  // New entry were added, need to sort again
  if (NewIdx != MemoryMapInfo->Count) {
    MemoryMapInfo->Count = NewIdx;
    PerformQuickSort (MemoryMapInfo->Entry, NewIdx, sizeof (MEMORY_MAP_ENTRY), CompareMemoryMap, &TempMemoryMap);
  }

  return EFI_SUCCESS;
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
  Build some basic HOBs

  After silicon initialization, the information is available
  to build some basic HOBs. These HOBs could be used/updated
  by stage2 code, or used by payload.

  @param Stage2Param         Stage2 Param pointer.

  @return                  The HOB list pointer.
**/
VOID *
EFIAPI
BuildBaseInfoHob (
  IN  STAGE2_PARAM                     *Stage2Param
  )
{
  SERIAL_PORT_INFO                     *SerialPortInfo;
  LOADER_FSP_INFO                      *LoaderFspInfo;
  MEMORY_MAP_INFO                      *MemoryMapInfo;
  UINT32                               Length;
  LOADER_GLOBAL_DATA                   *LdrGlobal;
  EFI_PEI_GRAPHICS_INFO_HOB            *FspGfxHob;
  EFI_PEI_GRAPHICS_INFO_HOB            *BlGfxHob;
  EFI_PEI_GRAPHICS_DEVICE_INFO_HOB     *BlGfxDeviceInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *GfxMode;
  EFI_STATUS                           Status;
  UINT8*                               *HashHob;
  UINT32                               HobDataSize;
  EXT_BOOT_LOADER_VERSION              *VersionHob;
  SEED_LIST_INFO_HOB                   *SeedListInfoHob;
  HASH_STORE_TABLE                     *HashStorePtr;

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
    SerialPortInfo->Revision    = LOADER_SERIAL_PORT_INFO_REVISION;
    SerialPortInfo->Type        = 1;
    SerialPortInfo->BaseAddr64  = 0x3F8;
    SerialPortInfo->BaseAddr    = 0x3F8;
    SerialPortInfo->Baud        = 115200;
    SerialPortInfo->RegWidth    = 1;
    SerialPortInfo->InputHertz  = 1843200;
    SerialPortInfo->UartPciAddr = 0;
  }

  // Build graphic info hob
  BlGfxHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GetGuidHobData (NULL, &Length, &gEfiGraphicsInfoHobGuid);
  if (BlGfxHob == NULL) {
    FspGfxHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GetGuidHobData (LdrGlobal->FspHobList, &Length,
                  &gEfiGraphicsInfoHobGuid);
    if (FspGfxHob != NULL) {
      BlGfxHob = BuildGuidHob (&gEfiGraphicsInfoHobGuid, sizeof (EFI_PEI_GRAPHICS_INFO_HOB));
      if (BlGfxHob != NULL) {
        CopyMem (BlGfxHob, FspGfxHob, sizeof (EFI_PEI_GRAPHICS_INFO_HOB));
        GfxMode = &BlGfxHob->GraphicsMode;
        DEBUG ((DEBUG_INFO, "Graphics Info: %d x %d x 32 @ 0x%08X\n",GfxMode->HorizontalResolution,\
          GfxMode->VerticalResolution, BlGfxHob->FrameBufferBase));
        if ((GfxMode->PixelFormat != PixelRedGreenBlueReserved8BitPerColor) &&
            (GfxMode->PixelFormat != PixelBlueGreenRedReserved8BitPerColor)) {
          DEBUG ((DEBUG_ERROR, "Graphics PixelFormat NOT expected (0x%x)\n", GfxMode->PixelFormat));
        }
      }
    } else {
      DEBUG ((DEBUG_INFO, "Failed to get GFX HOB from FSP\n"));
    }
  } else {
    DEBUG ((DEBUG_INFO, "Use existing GFX HOB from bootloader\n"));
  }

  // Build graphic device info hob
  BlGfxDeviceInfo = BuildGuidHob (&gEfiGraphicsDeviceInfoHobGuid, sizeof (EFI_PEI_GRAPHICS_DEVICE_INFO_HOB));
  if (BlGfxDeviceInfo != NULL) {
    SetMem (BlGfxDeviceInfo, sizeof (EFI_PEI_GRAPHICS_DEVICE_INFO_HOB), 0xFF);
  }

  // Build Memory Map hob
  Length = sizeof (MEMORY_MAP_INFO) + sizeof (MEMORY_MAP_ENTRY) * PcdGet32 (PcdMemoryMapEntryNumber);
  MemoryMapInfo = BuildGuidHob (&gLoaderMemoryMapInfoGuid, Length);
  if (MemoryMapInfo != NULL) {
    SetMem (MemoryMapInfo, Length, 0);
    MemoryMapInfo->Revision = 1;
    TraverseMemoryResourceHob (LdrGlobal->FspHobList, MemResHobCallback, MemoryMapInfo);
    Status = SplitMemroyMap (MemoryMapInfo);
    if (!EFI_ERROR (Status)) {
      Status = SortMemroyMap (MemoryMapInfo);
    }
    PrintMemoryMap (MemoryMapInfo);
    if (EFI_ERROR (Status)) {
      CpuHaltWithStatus ("Memory map failure !", Status);
    }
  }

  // Build key hash Hob for Payload
  if (LdrGlobal->HashStorePtr != NULL) {

    HashStorePtr = (HASH_STORE_TABLE *)LdrGlobal->HashStorePtr;
    HashHob      = BuildGuidHob (&gPayloadKeyHashGuid, HashStorePtr->UsedLength);

    //Copy the hash store to key hash Hob
    CopyMem (HashHob, (UINT8 *) HashStorePtr, HashStorePtr->UsedLength);

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
  Carve out a bootloader reserved, AcpiNvs, AcpiRecalim and payload reserved memory range
  from the system memory map.

  @param MemoryMapInfo     Array of MemoryMapInfo structure containing memory map entry.

  @retval EFI_SUCCESS           Memory ranges have been splitted successfully.
  @retval EFI_UNSUPPORTED       Required memory range size is not page aligned.
  @retval EFI_OUT_OF_RESOURCES  The system memory map top entry is too small.
  @retval EFI_ABORTED           No memory range has been splitted.

**/
EFI_STATUS
BuildUpldMemroyHobs (
  IN   MEMORY_MAP_INFO         *MemoryMapInfo
  )
{
  UINTN                        Idx;
  MEMORY_MAP_ENTRY             *MapEntry;
  EFI_RESOURCE_ATTRIBUTE_TYPE  Attribue;
  EFI_RESOURCE_TYPE            ResourceType;

  Attribue = EFI_RESOURCE_ATTRIBUTE_PRESENT |
             EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
             EFI_RESOURCE_ATTRIBUTE_TESTED |
             EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
             EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
             EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
             EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE;

  for (Idx = 0; Idx < MemoryMapInfo->Count; Idx++) {
    MapEntry     = &MemoryMapInfo->Entry[Idx];
    ResourceType = EFI_RESOURCE_SYSTEM_MEMORY;
    switch (MapEntry->Type) {
    case MEM_MAP_TYPE_ACPI_RECLAIM:
      BuildMemoryAllocationHob (MapEntry->Base, MapEntry->Size, EfiACPIReclaimMemory);
      break;
    case MEM_MAP_TYPE_ACPI_NVS:
      BuildMemoryAllocationHob (MapEntry->Base, MapEntry->Size, EfiACPIMemoryNVS);
      break;
    case MEM_MAP_TYPE_RESERVED:
      ResourceType = EFI_RESOURCE_MEMORY_RESERVED;
      break;
    default:
      break;
    }
    BuildResourceDescriptorHob (ResourceType, Attribue, MapEntry->Base, MapEntry->Size);
  }

  return EFI_SUCCESS;
}

/**
  Build PCI Root Bridge HOBs required by Universal Payload

**/
VOID
EFIAPI
BuildUpldPciRootBridgeHob (
  VOID
)
{
  UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES  *UpldRootBridges;
  PCI_ROOT_BRIDGE_INFO_HOB            *RootBridgeInfoHob;
  UINT8                                Count;
  UINT8                                Index;
  UINT8                                BarType;
  UINT32                               Length;

  RootBridgeInfoHob = (PCI_ROOT_BRIDGE_INFO_HOB *) GetGuidHobData (NULL, NULL,  &gLoaderPciRootBridgeInfoGuid);
  if (RootBridgeInfoHob != NULL) {

    Count  = RootBridgeInfoHob->Count;
    Length = sizeof (UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES) + sizeof (UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE) * Count;
    UpldRootBridges = (UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES *)BuildGuidHob (&gUniversalPayloadPciRootBridgeInfoGuid, Length);
    if (UpldRootBridges != NULL) {
      ZeroMem (UpldRootBridges, Length);
      UpldRootBridges->Header.Revision  = UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES_REVISION;
      UpldRootBridges->Header.Length    = (UINT16)Length;
      UpldRootBridges->ResourceAssigned = TRUE;
      UpldRootBridges->Count = Count;
      for (Index = 0; Index < Count; Index ++) {
        UpldRootBridges->RootBridge[Index].HID = EISA_PNP_ID(0x0A03);
        UpldRootBridges->RootBridge[Index].UID = Index;
        UpldRootBridges->RootBridge[Index].Segment   = 0;
        UpldRootBridges->RootBridge[Index].Supports  = EFI_PCI_ATTRIBUTE_IDE_PRIMARY_IO |
                                                       EFI_PCI_ATTRIBUTE_IDE_SECONDARY_IO |
                                                       EFI_PCI_ATTRIBUTE_ISA_IO_16 |
                                                       EFI_PCI_ATTRIBUTE_ISA_MOTHERBOARD_IO |
                                                       EFI_PCI_ATTRIBUTE_VGA_MEMORY |
                                                       EFI_PCI_ATTRIBUTE_VGA_IO_16 |
                                                       EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO_16;
        UpldRootBridges->RootBridge[Index].Attributes = UpldRootBridges->RootBridge[Index].Supports;
        UpldRootBridges->RootBridge[Index].AllocationAttributes = EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM |
                                                                  EFI_PCI_HOST_BRIDGE_MEM64_DECODE;

        UpldRootBridges->RootBridge[Index].Bus.Base  = RootBridgeInfoHob->Entry[Index].BusBase;
        UpldRootBridges->RootBridge[Index].Bus.Limit = RootBridgeInfoHob->Entry[Index].BusLimit;

        BarType = PciBarTypeIo16 - 1;
        if (RootBridgeInfoHob->Entry[Index].Resource[BarType].ResLength > 0) {
          UpldRootBridges->RootBridge[Index].Io.Base  = RootBridgeInfoHob->Entry[Index].Resource[BarType].ResBase;
          UpldRootBridges->RootBridge[Index].Io.Limit = RootBridgeInfoHob->Entry[Index].Resource[BarType].ResBase + RootBridgeInfoHob->Entry[Index].Resource[BarType].ResLength - 1;
        } else {
          UpldRootBridges->RootBridge[Index].Io.Base = MAX_UINT16;
        }

        BarType = PciBarTypeMem32 - 1;
        if (RootBridgeInfoHob->Entry[Index].Resource[BarType].ResLength > 0) {
          UpldRootBridges->RootBridge[Index].Mem.Base  = RootBridgeInfoHob->Entry[Index].Resource[BarType].ResBase;
          UpldRootBridges->RootBridge[Index].Mem.Limit = RootBridgeInfoHob->Entry[Index].Resource[BarType].ResBase + RootBridgeInfoHob->Entry[Index].Resource[BarType].ResLength - 1;
        } else {
          UpldRootBridges->RootBridge[Index].Mem.Base = MAX_UINT32;
        }

        BarType = PciBarTypeMem64 - 1;
        if (RootBridgeInfoHob->Entry[Index].Resource[BarType].ResLength > 0) {
          UpldRootBridges->RootBridge[Index].MemAbove4G.Base  = RootBridgeInfoHob->Entry[Index].Resource[BarType].ResBase;
          UpldRootBridges->RootBridge[Index].MemAbove4G.Limit = RootBridgeInfoHob->Entry[Index].Resource[BarType].ResBase + RootBridgeInfoHob->Entry[Index].Resource[BarType].ResLength - 1;
        } else {
          UpldRootBridges->RootBridge[Index].MemAbove4G.Base = MAX_UINT64;
        }

        BarType = PciBarTypePMem32 - 1;
        if (RootBridgeInfoHob->Entry[Index].Resource[BarType].ResLength > 0) {
          UpldRootBridges->RootBridge[Index].PMem.Base  = RootBridgeInfoHob->Entry[Index].Resource[BarType].ResBase;
          UpldRootBridges->RootBridge[Index].PMem.Limit = RootBridgeInfoHob->Entry[Index].Resource[BarType].ResBase + RootBridgeInfoHob->Entry[Index].Resource[BarType].ResLength - 1;
        } else {
          UpldRootBridges->RootBridge[Index].PMem.Base = MAX_UINT32;
        }

        BarType = PciBarTypePMem64 - 1;
        if (RootBridgeInfoHob->Entry[Index].Resource[BarType].ResLength > 0) {
          UpldRootBridges->RootBridge[Index].PMemAbove4G.Base  = RootBridgeInfoHob->Entry[Index].Resource[BarType].ResBase;
          UpldRootBridges->RootBridge[Index].PMemAbove4G.Limit = RootBridgeInfoHob->Entry[Index].Resource[BarType].ResBase + RootBridgeInfoHob->Entry[Index].Resource[BarType].ResLength - 1;
        } else {
          UpldRootBridges->RootBridge[Index].PMemAbove4G.Base = MAX_UINT64;
        }

      }
    }
  }
}

/**
  Build HOBs required by Universal Payload

**/
VOID
EFIAPI
BuildUniversalPayloadHob (
  VOID
)
{
  SERIAL_PORT_INFO                    *SerialPortInfo;
  MEMORY_MAP_INFO                     *MemoryMapInfo;
  UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO  *PldSerialPortInfo;
  UNIVERSAL_PAYLOAD_ACPI_TABLE        *AcpiHob;
  UNIVERSAL_PAYLOAD_SMBIOS_TABLE      *SmbiosHob;
  UINT8                                PhysicalAddressBits;

  // Memory resource hob
  MemoryMapInfo = (MEMORY_MAP_INFO *) GetGuidHobData (NULL, NULL, &gLoaderMemoryMapInfoGuid);
  if (MemoryMapInfo != NULL) {
    BuildUpldMemroyHobs (MemoryMapInfo);
  }

  // ACPI table HOB
  AcpiHob = BuildGuidHob (&gUniversalPayloadAcpiTableGuid, sizeof (UNIVERSAL_PAYLOAD_ACPI_TABLE));
  if (AcpiHob != NULL) {
    ZeroMem (AcpiHob, sizeof (UNIVERSAL_PAYLOAD_ACPI_TABLE));
    AcpiHob->Header.Revision = UNIVERSAL_PAYLOAD_ACPI_TABLE_REVISION;
    AcpiHob->Header.Length   = sizeof (UNIVERSAL_PAYLOAD_ACPI_TABLE);
    AcpiHob->Rsdp            = PcdGet32 (PcdAcpiTablesRsdp);
  }

  // SMBIOS table HOB
  SmbiosHob = BuildGuidHob (&gUniversalPayloadSmbiosTableGuid, sizeof (UNIVERSAL_PAYLOAD_SMBIOS_TABLE));
  if (SmbiosHob != NULL) {
    ZeroMem (SmbiosHob, sizeof (UNIVERSAL_PAYLOAD_SMBIOS_TABLE));
    SmbiosHob->Header.Revision  = UNIVERSAL_PAYLOAD_SMBIOS_TABLE_REVISION;
    SmbiosHob->Header.Length    = sizeof (UNIVERSAL_PAYLOAD_SMBIOS_TABLE);
    SmbiosHob->SmBiosEntryPoint = PcdGet32 (PcdSmbiosTablesBase);
  }

  // Update serial port hob
  SerialPortInfo = (SERIAL_PORT_INFO *)GetGuidHobData (NULL, NULL, &gLoaderSerialPortInfoGuid);
  if (SerialPortInfo != NULL) {
    PldSerialPortInfo = BuildGuidHob (&gUniversalPayloadSerialPortInfoGuid, sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO));
    if (PldSerialPortInfo != NULL) {
      ZeroMem (PldSerialPortInfo, sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO));
      PldSerialPortInfo->Header.Revision = UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO_REVISION;
      PldSerialPortInfo->Header.Length   = sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO);
      PldSerialPortInfo->UseMmio         = (SerialPortInfo->Type == 2) ? TRUE : FALSE;
      PldSerialPortInfo->RegisterBase    = SerialPortInfo->BaseAddr64;
      PldSerialPortInfo->BaudRate        = SerialPortInfo->Baud;
      PldSerialPortInfo->RegisterStride  = (UINT8) SerialPortInfo->RegWidth;
    }
  }

  // Build CPU memory space and IO space hob
  PhysicalAddressBits = GetPhysicalAddressBits ();
  BuildCpuHob (PhysicalAddressBits, 16);

  // Build PCI root bridge hob
  BuildUpldPciRootBridgeHob ();
}

/**
  Update graphics hobs.

**/
VOID
UpdateGraphicsHob (
  VOID
  )
{
  EFI_PEI_GRAPHICS_INFO_HOB        *GfxInfoHob;
  EFI_PEI_GRAPHICS_DEVICE_INFO_HOB *GfxDeviceInfoHob;

  // Update graphic info HOB
  GfxInfoHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GetGuidHobData (NULL, NULL, &gEfiGraphicsInfoHobGuid);
  if (GfxInfoHob != NULL) {
    PlatformUpdateHobInfo (&gEfiGraphicsInfoHobGuid, GfxInfoHob);
  }

  // Update graphic device info HOB
  GfxDeviceInfoHob = (EFI_PEI_GRAPHICS_DEVICE_INFO_HOB *)GetGuidHobData (NULL, NULL, &gEfiGraphicsDeviceInfoHobGuid);
  if (GfxDeviceInfoHob != NULL) {
    PlatformUpdateHobInfo (&gEfiGraphicsDeviceInfoHobGuid, GfxDeviceInfoHob);
  }

}

/**
  Build SMM variable related HOBs for Universal Payload

**/
VOID
BuildSmmVariableHobs (
  VOID
)
{
  EFI_STATUS                           Status;
  LDR_SMM_INFO                         *SmmInfoHob;
  UINT32                               Length;
  PLD_SMM_REGISTERS                    *SmmRegisterHob;
  PLD_GENERIC_REGISTER                 *Reg;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK       *SmmMemoryHob;
  SPI_FLASH_INFO                       *SpiFlashInfoHob;
  LOADER_PLATFORM_INFO                 *PlatformInfo;
  NV_VARIABLE_INFO                     *NvVariableHob;
  UINT32                               Base;
  PLD_S3_COMMUNICATION                 *S3CommunicationHob;

  SmmInfoHob   = NULL;
  PlatformInfo = NULL;
  if ((PcdGet8(PcdBuildSmmHobs) & BIT0) != 0) {
    SmmInfoHob   = (LDR_SMM_INFO *)GetGuidHobData (NULL, NULL, &gSmmInformationGuid);
    PlatformInfo = (LOADER_PLATFORM_INFO *) GetGuidHobData (NULL, NULL, &gLoaderPlatformInfoGuid);
  }

  // Build SMM register HOB for universal payload
  Length = sizeof (PLD_SMM_REGISTERS) + 5 * sizeof (PLD_GENERIC_REGISTER);
  SmmRegisterHob = BuildGuidHob (&gPldSmmRegisterInfoGuid, Length);
  if (SmmRegisterHob != NULL) {
    ZeroMem (SmmRegisterHob, Length);
    PlatformUpdateHobInfo (&gPldSmmRegisterInfoGuid, SmmRegisterHob);
    if (((PcdGet8(PcdBuildSmmHobs) & BIT0) != 0) && (SmmInfoHob != NULL)) {
      // Initialize the HOB using other HOBs
      SmmRegisterHob->Revision = 0;
      SmmRegisterHob->Count    = 5;
      Reg = &SmmRegisterHob->Registers[0];
      Reg->Id                        = REGISTER_ID_SMI_GBL_EN;
      Reg->Value                     = 1;
      Reg->Address.AddressSpaceId    = (SmmInfoHob->SmiCtrlReg.RegType == REG_TYPE_IO)?EFI_ACPI_3_0_SYSTEM_IO:EFI_ACPI_3_0_SYSTEM_MEMORY;
      Reg->Address.RegisterBitWidth  = 1;
      Reg->Address.RegisterBitOffset = SmmInfoHob->SmiCtrlReg.SmiGblPos;
      Reg->Address.AccessSize        = EFI_ACPI_3_0_DWORD;
      Reg->Address.Address           = SmmInfoHob->SmiCtrlReg.Address;

      Reg++;
      Reg->Id                        = REGISTER_ID_SMI_GBL_EN_LOCK;
      Reg->Value                     = 1;
      Reg->Address.AddressSpaceId    = (SmmInfoHob->SmiLockReg.RegType == REG_TYPE_IO)?EFI_ACPI_3_0_SYSTEM_IO:EFI_ACPI_3_0_SYSTEM_MEMORY;
      Reg->Address.RegisterBitWidth  = 1;
      Reg->Address.RegisterBitOffset = SmmInfoHob->SmiLockReg.SmiLockPos;
      Reg->Address.AccessSize        = EFI_ACPI_3_0_DWORD;
      Reg->Address.Address           = SmmInfoHob->SmiLockReg.Address;

      Reg++;
      Reg->Id                        = REGISTER_ID_SMI_EOS;
      Reg->Value                     = 1;
      Reg->Address.AddressSpaceId    = (SmmInfoHob->SmiCtrlReg.RegType == REG_TYPE_IO)?EFI_ACPI_3_0_SYSTEM_IO:EFI_ACPI_3_0_SYSTEM_MEMORY;
      Reg->Address.RegisterBitWidth  = 1;
      Reg->Address.RegisterBitOffset = SmmInfoHob->SmiCtrlReg.SmiEosPos ;
      Reg->Address.AccessSize        = EFI_ACPI_3_0_DWORD;
      Reg->Address.Address           = SmmInfoHob->SmiCtrlReg.Address;

      Reg++;
      Reg->Id                        = REGISTER_ID_SMI_APM_EN;
      Reg->Value                     = 1;
      Reg->Address.AddressSpaceId    = (SmmInfoHob->SmiCtrlReg.RegType == REG_TYPE_IO)?EFI_ACPI_3_0_SYSTEM_IO:EFI_ACPI_3_0_SYSTEM_MEMORY;
      Reg->Address.RegisterBitWidth  = 1;
      Reg->Address.RegisterBitOffset = SmmInfoHob->SmiCtrlReg.SmiApmPos;
      Reg->Address.AccessSize        = EFI_ACPI_3_0_DWORD;
      Reg->Address.Address           = SmmInfoHob->SmiCtrlReg.Address;

      Reg++;
      Reg->Id                        = REGISTER_ID_SMI_APM_STS;
      Reg->Value                     = 1;
      Reg->Address.AddressSpaceId    = (SmmInfoHob->SmiStsReg.RegType == REG_TYPE_IO)?EFI_ACPI_3_0_SYSTEM_IO:EFI_ACPI_3_0_SYSTEM_MEMORY;
      Reg->Address.RegisterBitWidth  = 1;
      Reg->Address.RegisterBitOffset = SmmInfoHob->SmiStsReg.SmiApmPos;
      Reg->Address.AccessSize        = EFI_ACPI_3_0_DWORD;
      Reg->Address.Address           = SmmInfoHob->SmiStsReg.Address;
    }
  }

  // SMM memory HOB
  Length = sizeof (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK) + sizeof (EFI_SMRAM_DESCRIPTOR);
  SmmMemoryHob = BuildGuidHob (&gEfiSmmSmramMemoryGuid, Length);
  if (SmmMemoryHob != NULL) {
    ZeroMem (SmmMemoryHob, Length);
    PlatformUpdateHobInfo (&gEfiSmmSmramMemoryGuid, SmmMemoryHob);
    if (((PcdGet8(PcdBuildSmmHobs) & BIT0) != 0) && (SmmInfoHob != NULL)) {
      // Initialize the HOB using other HOBs
      SmmMemoryHob->NumberOfSmmReservedRegions  = 1;
      if ((SmmInfoHob->Flags & SMM_FLAGS_4KB_COMMUNICATION) != 0){
        SmmMemoryHob->NumberOfSmmReservedRegions  += 1;
        SmmMemoryHob->Descriptor[0].CpuStart      = SmmInfoHob->SmmBase;
        SmmMemoryHob->Descriptor[0].PhysicalStart = SmmInfoHob->SmmBase;
        SmmMemoryHob->Descriptor[0].PhysicalSize  = SIZE_4KB;
        SmmMemoryHob->Descriptor[0].RegionState   = EFI_ALLOCATED;
        SmmMemoryHob->Descriptor[1].CpuStart      = SmmInfoHob->SmmBase + SIZE_4KB;
        SmmMemoryHob->Descriptor[1].PhysicalStart = SmmInfoHob->SmmBase + SIZE_4KB;
        SmmMemoryHob->Descriptor[1].PhysicalSize  = SmmInfoHob->SmmSize - SIZE_4KB;
        SmmMemoryHob->Descriptor[1].RegionState   = 0;
      } else {
        SmmMemoryHob->Descriptor[0].CpuStart      = SmmInfoHob->SmmBase;
        SmmMemoryHob->Descriptor[0].PhysicalStart = SmmInfoHob->SmmBase;
        SmmMemoryHob->Descriptor[0].PhysicalSize  = SmmInfoHob->SmmSize;
        SmmMemoryHob->Descriptor[0].RegionState   = 0;
      }
    }
  }

  // SPI Flash HOB
  SpiFlashInfoHob = BuildGuidHob (&gSpiFlashInfoGuid, sizeof (SPI_FLASH_INFO));
  if (SpiFlashInfoHob != NULL) {
    ZeroMem (SpiFlashInfoHob,  sizeof (SPI_FLASH_INFO));
    PlatformUpdateHobInfo (&gSpiFlashInfoGuid, SpiFlashInfoHob);
    if (((PcdGet8(PcdBuildSmmHobs) & BIT0) != 0) && (PlatformInfo != NULL)) {
      // Initialize the HOB using other HOBs
      SpiFlashInfoHob->Revision                     = 0;
      SpiFlashInfoHob->Flags                        = PlatformInfo->Flags;
      SpiFlashInfoHob->SpiAddress.AddressSpaceId    = SPACE_ID_PCI_CONFIGURATION;
      SpiFlashInfoHob->SpiAddress.RegisterBitWidth  = 32;
      SpiFlashInfoHob->SpiAddress.RegisterBitOffset = 0;
      SpiFlashInfoHob->SpiAddress.AccessSize        = REGISTER_BIT_WIDTH_DWORD;
      SpiFlashInfoHob->SpiAddress.Address           = TO_MM_PCI_ADDRESS (GetDeviceAddr (4, 0));
    }
  }

  // SPI NV variable HOB
  NvVariableHob = BuildGuidHob (&gNvVariableInfoGuid, sizeof (NV_VARIABLE_INFO));
  if (NvVariableHob != NULL) {
    ZeroMem (NvVariableHob,  sizeof (NV_VARIABLE_INFO));
    Status = GetComponentInfo (FLASH_MAP_SIG_UEFIVARIABLE, &Base, &Length);
    if (!EFI_ERROR (Status)) {
      NvVariableHob->Revision           = 0;
      NvVariableHob->VariableStoreBase  = Base;
      NvVariableHob->VariableStoreSize  = Length;
    }
  }

  // S3 communication HOB
  S3CommunicationHob = BuildGuidHob (&gPldS3CommunicationGuid, sizeof (PLD_S3_COMMUNICATION));
  if (S3CommunicationHob != NULL) {
    ZeroMem (S3CommunicationHob,  sizeof (PLD_S3_COMMUNICATION));
    PlatformUpdateHobInfo (&gPldS3CommunicationGuid, S3CommunicationHob);
    if (((PcdGet8(PcdBuildSmmHobs) & BIT0) != 0) && (SmmInfoHob != NULL)) {
      // Initialize the HOB using other HOBs
      if ((SmmInfoHob->Flags & SMM_FLAGS_4KB_COMMUNICATION) != 0){
        S3CommunicationHob->CommBuffer.PhysicalStart = SmmInfoHob->SmmBase;
        S3CommunicationHob->CommBuffer.CpuStart      = SmmInfoHob->SmmBase;
        S3CommunicationHob->CommBuffer.PhysicalSize  = SIZE_4KB;
        S3CommunicationHob->PldAcpiS3Enable          = FALSE;
      }
    }
  }
}


/**
  Build and update HOBs.

  Before jumping to payload, more information is available, so update some HOBs
  built early, and build more HOBs for payload.

  @param Stage2Param         Stage2 Param pointer.

  @return                  The HOB list pointer.
**/
VOID *
EFIAPI
BuildExtraInfoHob (
  IN  STAGE2_PARAM                 *Stage2Param
  )
{
  LOADER_GLOBAL_DATA               *LdrGlobal;
  S3_DATA                          *S3Data;
  SERIAL_PORT_INFO                 *SerialPortInfo;
  SYSTEM_TABLE_INFO                *SystemTableInfo;
  SYS_CPU_INFO                     *SysCpuInfo;
  PERFORMANCE_INFO                 *PerformanceInfo;
  OS_BOOT_OPTION_LIST              *OsBootOptionInfo;
  LOADER_PLATFORM_INFO             *LoaderPlatformInfo;
  LOADER_PLATFORM_DATA             *LoaderPlatformData;
  LOADER_LIBRARY_DATA              *LoaderLibData;
  UINT32                           Count;
  UINT32                           Length;
  BOOT_LOADER_SERVICES_LIST        *HobServicesList;
  SERVICES_LIST                    *PldServicesList;
  SERVICES_LIST                    *CoreServicesList;
  FLASH_MAP                        *FlashMapPtr;
  FLASH_MAP                        *FlashMapHob;
  UINTN                            Index;
  SEED_LIST_INFO_HOB               *SeedListInfoHob;
  PLT_DEVICE_TABLE                 *DeviceTable;
  VOID                             *DeviceTableHob;
  LDR_SMM_INFO                     *SmmInfoHob;
  SYS_CPU_TASK_HOB                 *SysCpuTaskHob;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();
  S3Data    = (S3_DATA *)LdrGlobal->S3DataPtr;

  // Build library data hob
  LoaderLibData = BuildGuidHob (&gLoaderLibraryDataGuid, sizeof (LOADER_LIBRARY_DATA));
  if (LoaderLibData != NULL) {
    LoaderLibData->Count = (UINT16)PcdGet32 (PcdMaxLibraryDataEntry);
    LoaderLibData->Flags = 0;
    LoaderLibData->Data  = LdrGlobal->LibDataPtr;
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
    SystemTableInfo->SmbiosTableBase = (UINT64)PcdGet32 (PcdSmbiosTablesBase);
    SystemTableInfo->SmbiosTableSize = (UINT32)PcdGet16 (PcdSmbiosTablesSize);
    PlatformUpdateHobInfo (&gLoaderSystemTableInfoGuid, SystemTableInfo);
  }

  // Build Loader Platform Data Hob
  LoaderPlatformData = BuildGuidHob (&gLoaderPlatformDataGuid, sizeof (LOADER_PLATFORM_DATA));
  if (LoaderPlatformData != NULL) {
    LoaderPlatformData->Revision = 1;
    LoaderPlatformData->DebugLogBuffer = (DEBUG_LOG_BUFFER_HEADER *) GetDebugLogBufferPtr ();
    LoaderPlatformData->ConfigDataPtr  = GetConfigDataPtr ();
    LoaderPlatformData->ContainerList  = GetContainerListPtr ();
    LoaderPlatformData->DmaBufferPtr   = GetDmaBufferPtr ();
  }

  // Build flash map info hob
  FlashMapPtr = LdrGlobal->FlashMapPtr;
  FlashMapHob = BuildGuidHob (&gFlashMapInfoGuid, FlashMapPtr->Length);
  if (FlashMapHob != NULL) {
    CopyMem (FlashMapHob, FlashMapPtr, FlashMapPtr->Length);
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

  // Build SMMRAM info Hob
  SmmInfoHob = BuildGuidHob (&gSmmInformationGuid, sizeof (LDR_SMM_INFO));
  if (SmmInfoHob != NULL) {
    ZeroMem (SmmInfoHob, sizeof (LDR_SMM_INFO));
    PlatformUpdateHobInfo (&gSmmInformationGuid, SmmInfoHob);
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
    ZeroMem (LoaderPlatformInfo, Length);
    LoaderPlatformInfo->Revision = 2;
    LoaderPlatformInfo->BootPartition = GetCurrentBootPartition ();
    LoaderPlatformInfo->BootMode = GetBootMode ();
    LoaderPlatformInfo->PlatformId = GetPlatformId ();
    LoaderPlatformInfo->LdrFeatures = LdrGlobal->LdrFeatures;
    SysCpuInfo = MpGetInfo ();
    LoaderPlatformInfo->CpuCount = SysCpuInfo->CpuCount;
    PlatformUpdateHobInfo (&gLoaderPlatformInfoGuid, LoaderPlatformInfo);
  }

  // Build MP CPU task info Hob
  if (GetPayloadId () == 0) {
    Length        = sizeof (SYS_CPU_TASK_HOB);
    SysCpuTaskHob = BuildGuidHob (&gLoaderMpCpuTaskInfoGuid, Length);

    if (SysCpuTaskHob != NULL) {
      SysCpuTaskHob->SysCpuTask = (UINTN) MpGetTask();
      SysCpuTaskHob->SysCpuInfo = (UINTN) MpGetInfo();
    }
  }

  BuildUniversalPayloadHob ();

  if ((PcdGet8(PcdBuildSmmHobs) & BIT1) != 0) {
    BuildSmmVariableHobs ();
  }
  return LdrGlobal->LdrHobList;
}
