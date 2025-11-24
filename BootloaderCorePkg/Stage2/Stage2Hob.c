/** @file

  Copyright (c) 2016 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2.h"


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
    if (PcdGet32(PcdFlashSize) < SIZE_16MB){
      MemoryMapInfo->Entry[NewIdx].Base = PcdGet32(PcdFlashBaseAddress);
      MemoryMapInfo->Entry[NewIdx].Size = PcdGet32(PcdFlashSize);
    } else {
      // Limit flash map entry to 16MB
      MemoryMapInfo->Entry[NewIdx].Base = (UINT32)(~SIZE_16MB + 1);
      MemoryMapInfo->Entry[NewIdx].Size = SIZE_16MB;
    }
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
    LoaderFspInfo->FspHobList = (UINT32)(UINTN)LdrGlobal->FspHobList;
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
  if (FeaturePcdGet(PcdVerifiedBootEnabled) && LdrGlobal->HashStorePtr != NULL) {

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
  Build HOBs required by Universal Payload

**/
VOID
EFIAPI
BuildUniversalPayloadHob (
  VOID
)
{
  MEMORY_MAP_INFO                     *MemoryMapInfo;
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

  // Build CPU memory space and IO space hob
  PhysicalAddressBits = GetPhysicalAddressBits ();
  BuildCpuHob (PhysicalAddressBits, 16);

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
  SMM_BASE_HOB_DATA                    *SmmBaseHob;
  SYS_CPU_INFO                         *SysCpuInfo;
  UINT32                               Index;
  UINT32                               Size;

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

  // SMM rebase HOB
  if (PcdGet8 (PcdSmmRebaseMode) == SMM_REBASE_ENABLE_NOSMRR) {
    SysCpuInfo = MpGetInfo ();
    Length = sizeof (SMM_BASE_HOB_DATA) + SysCpuInfo->CpuCount * sizeof (UINT64);
    SmmBaseHob = BuildGuidHob (&gSmmBaseHobGuid, Length);
    if (SmmBaseHob != NULL) {
      SmmBaseHob->ProcessorIndex     = 0;
      SmmBaseHob->NumberOfProcessors = SysCpuInfo->CpuCount;
      for (Index = 0; Index < SysCpuInfo->CpuCount; Index++) {
        SmmBaseHob->SmBase[Index] = PcdGet32 (PcdSmramTsegBase) + PcdGet32 (PcdSmramTsegSize) - (SMM_BASE_MIN_SIZE + Index * SMM_BASE_GAP);
        DEBUG ((DEBUG_INFO, "SmmBaseHob->SmBase[%d]=0x%x\n", Index, SmmBaseHob->SmBase[Index]));
      }
    }
  }

  // SMM memory HOB
  Length = sizeof (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK) + sizeof (EFI_SMRAM_DESCRIPTOR);
  if (PcdGet8 (PcdSmmRebaseMode) == SMM_REBASE_ENABLE_NOSMRR) {
    Length += sizeof (EFI_SMRAM_DESCRIPTOR);
  }
  SmmMemoryHob = BuildGuidHob (&gEfiSmmSmramMemoryGuid, Length);
  if ((SmmMemoryHob != NULL) && (SmmInfoHob != NULL)) {
    ZeroMem (SmmMemoryHob, Length);
    PlatformUpdateHobInfo (&gEfiSmmSmramMemoryGuid, SmmMemoryHob);
    Index  = 0;
    Size   = 0;
    Length = 0;
    if ((PcdGet8(PcdBuildSmmHobs) & BIT0) != 0) {
      // Initialize the HOB using other HOBs
      SmmMemoryHob->NumberOfSmmReservedRegions  = 0;
      if ((SmmInfoHob->Flags & SMM_FLAGS_4KB_COMMUNICATION) != 0){
        SmmMemoryHob->Descriptor[Index].CpuStart      = SmmInfoHob->SmmBase;
        SmmMemoryHob->Descriptor[Index].PhysicalStart = SmmInfoHob->SmmBase;
        SmmMemoryHob->Descriptor[Index].PhysicalSize  = SIZE_4KB;
        SmmMemoryHob->Descriptor[Index].RegionState   = EFI_ALLOCATED;
        Index++;
        Size = SIZE_4KB;
      }

      if (PcdGet8 (PcdSmmRebaseMode) == SMM_REBASE_ENABLE_NOSMRR) {
        Length = SMM_BASE_MIN_SIZE + SysCpuInfo->CpuCount * SMM_BASE_GAP;
      }
      SmmMemoryHob->Descriptor[Index].CpuStart      = SmmInfoHob->SmmBase + Size;
      SmmMemoryHob->Descriptor[Index].PhysicalStart = SmmInfoHob->SmmBase + Size;
      SmmMemoryHob->Descriptor[Index].PhysicalSize  = SmmInfoHob->SmmSize - Size - Length;
      SmmMemoryHob->Descriptor[Index].RegionState   = 0;
      Index++;

      if (PcdGet8 (PcdSmmRebaseMode) == SMM_REBASE_ENABLE_NOSMRR) {
        SmmMemoryHob->Descriptor[Index].CpuStart      = SmmInfoHob->SmmBase + SmmInfoHob->SmmSize - Length;
        SmmMemoryHob->Descriptor[Index].PhysicalStart = SmmInfoHob->SmmBase + SmmInfoHob->SmmSize - Length;
        SmmMemoryHob->Descriptor[Index].PhysicalSize  = Length;
        SmmMemoryHob->Descriptor[Index].RegionState   = EFI_ALLOCATED;
        Index++;
      }
      SmmMemoryHob->NumberOfSmmReservedRegions = Index;
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
  UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO *SerialPortInfo;
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
  CSME_PERFORMANCE_INFO            *CsmeBootTimeData;
  TPM_EVENT_LOG_INFO               *TpmEventLogHob;
  SECUREBOOT_INFO                  *SecureBootInfoHob;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer();

  // Build library data hob
  LoaderLibData = BuildGuidHob (&gLoaderLibraryDataGuid, sizeof (LOADER_LIBRARY_DATA));
  if (LoaderLibData != NULL) {
    LoaderLibData->Count = (UINT16)PcdGet32 (PcdMaxLibraryDataEntry);
    LoaderLibData->Flags = 0;
    LoaderLibData->Data  = LdrGlobal->LibDataPtr;
  }

  // Build serial port hob
  SerialPortInfo = BuildGuidHob (&gUniversalPayloadSerialPortInfoGuid, sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO));
  if (SerialPortInfo != NULL) {
    SerialPortInfo->Header.Revision = UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO_REVISION;
    SerialPortInfo->Header.Length   = sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO);
    SerialPortInfo->RegisterBase    = GetSerialPortBase ();
    SerialPortInfo->UseMmio         = (SerialPortInfo->RegisterBase > SIZE_64KB) ? TRUE : FALSE;
    SerialPortInfo->BaudRate        = 115200;
    SerialPortInfo->RegisterStride  = GetSerialPortStrideSize ();
    PlatformUpdateHobInfo (&gUniversalPayloadSerialPortInfoGuid, SerialPortInfo);
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

  //
  // Build HOB for CSME boot time performance data
  //
  if ((PcdGet32 (PcdBootPerformanceMask) & BIT2) != 0) {
    Length = sizeof (CSME_PERFORMANCE_INFO) + (64 * sizeof (UINT32));
    CsmeBootTimeData = BuildGuidHob (&gCsmePerformanceInfoGuid, Length);
    if (CsmeBootTimeData != NULL) {
      ZeroMem (CsmeBootTimeData, Length);
      CsmeBootTimeData->BootDataLength = 64;
      PlatformUpdateHobInfo (&gCsmePerformanceInfoGuid, CsmeBootTimeData);
    }
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

  // SecureBoot Info HOB
  SecureBootInfoHob = BuildGuidHob (&gSecureBootInfoGuid, sizeof (SECUREBOOT_INFO));
  if (SecureBootInfoHob != NULL) {
    ZeroMem (SecureBootInfoHob, sizeof (SECUREBOOT_INFO));
    SecureBootInfoHob->Header.Revision      = PAYLOAD_SECUREBOOT_INFO_HOB_REVISION;
    SecureBootInfoHob->Header.Length        = sizeof (SECUREBOOT_INFO);
    SecureBootInfoHob->VerifiedBootEnabled  = (LdrGlobal->LdrFeatures & FEATURE_VERIFIED_BOOT) >> 4;
    SecureBootInfoHob->MeasuredBootEnabled  = (LdrGlobal->LdrFeatures & FEATURE_MEASURED_BOOT) >> 1;

    TpmLibGetActivePcrBanks (&SecureBootInfoHob->TpmPcrActivePcrBanks);

    if (LoaderPlatformInfo != NULL) {
      SecureBootInfoHob->FirmwareDebuggerInitialized  = ((LoaderPlatformInfo->HwState >> 2) || (LoaderPlatformInfo->HwState >> 3));
    }

    // SBL supports only TPM 2.0
    if (SecureBootInfoHob->MeasuredBootEnabled) {
      SecureBootInfoHob->TpmType                      = TPM_TYPE_20;
    } else {
      SecureBootInfoHob->TpmType                      = NO_TPM;
    }
  }

  // Create the Tpm event log for bootloader events.
  CreateTpmEventLogHob ();

  // Tpm Event log Buffer Info HOB
  // This is a addon information
  TpmEventLogHob = BuildGuidHob (&gTpmEventLogInfoGuid, sizeof (TPM_EVENT_LOG_INFO));
  if (TpmEventLogHob != NULL) {
    ZeroMem (TpmEventLogHob, sizeof (TPM_EVENT_LOG_INFO));
    TpmEventLogHob->Revision  = PAYLOAD_TPM_EVENT_LOG_HOB_REVISION;
    GetTpmEventLog (&TpmEventLogHob->Tcg2Lasa, &TpmEventLogHob->Tcg2EventSize);
  }

  BuildUniversalPayloadHob ();

  if ((PcdGet8(PcdBuildSmmHobs) & BIT1) != 0) {
    BuildSmmVariableHobs ();
  }
  return LdrGlobal->LdrHobList;
}
