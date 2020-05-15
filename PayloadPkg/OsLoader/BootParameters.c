/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OsLoader.h"

CONST CHAR8 *mMmcDllStr = "MMCDLL";

CONST CHAR8 *mAppendCmdLineParams = NULL;

/**
  Get command line arguments from the config file.

  This function will read command line arguments and put
  it into Buffer.

  @param[out] Buffer      Buffer to get command line arguments
  @param[in]  BufferLen   Buffer size
  @param[in]  File        Pointer to the config file
  @param[in]  FileLen     File size

  @retval Number of bytes read from the config file.
**/
UINTN
GetFromConfigFile (
  OUT UINT8                  *Buffer,
  IN  UINT32                  BufferLen,
  IN  UINT8                  *File,
  IN  UINT32                  FileLen
  )
{
  UINTN                     Index;
  UINTN                     Count;

  if (BufferLen == 0) {
    return 0;
  }

  Count = 0;
  for (Index = 0; Index < FileLen ; Index++) {
    if ((CompareMem (EOF, &File[Index], 5) == 0) || ((CHAR8)File[Index] == '\0')) {
      DEBUG ((DEBUG_INFO, "cmd Count = [0x%x]\n", Count));
      break;
    } else {
      if (((CHAR8)File[Index] != '\n') && ((CHAR8)File[Index] != '\r')) {
        if (Count + 1 < BufferLen) {
          Buffer[Count++] = File[Index];
        }
      }
    }
  }

  Buffer[Count] = '\0';
  return Count;
}

/**
  Update OS memory size info

  Multiboot mmap info might be changed since slimboot might reserve
  memory for different OS image. so re-calculate the amount of lower
  and upper memory size (in KB).

  @param[in,out] LoadedImage       Loaded boot image info

**/
VOID
UpdateOsMemSize (
  IN OUT  LOADED_IMAGE       *LoadedImage
  )
{
  UINTN                      Index;
  UINT32                     Count;
  MULTIBOOT_MMAP             *MbMmap;
  MULTIBOOT_INFO             *MbInfo;
  UINT64                     MemLowerSize;
  UINT64                     MemUpperSize;

  if ((LoadedImage->Flags & LOADED_IMAGE_MULTIBOOT) == 0) {
    return;
  }

  MemLowerSize = 0;
  MemUpperSize = 0;

  MbInfo = &LoadedImage->Image.MultiBoot.MbInfo;
  MbMmap = (MULTIBOOT_MMAP *) MbInfo->MmapAddr;
  Count  = MbInfo->MmapLength / sizeof (MULTIBOOT_MMAP);
  for (Index = 0; Index < Count; Index++) {
    if (MbMmap[Index].Type != MEM_MAP_TYPE_RAM) {
      continue;
    }
    if (MbMmap[Index].BaseAddr == 0) {
      MemLowerSize = MbMmap[Index].Length;
    } else if (MbMmap[Index].BaseAddr >= MB_ (1)) {
      MemUpperSize = MbMmap[Index].Length - MB_ (1);
    }
  }

  if ((MemLowerSize != 0) || (MemUpperSize != 0)) {
    MbInfo->MemLower = (UINT32) (MemLowerSize / KB_ (1));
    MbInfo->MemUpper = (UINT32) (MemUpperSize / KB_ (1));
    MbInfo->Flags   |= MULTIBOOT_INFO_HAS_MEMORY;
  }

}


/**
  Update OS memmap

  Based on memory usage, especially reserved memory usage, this
  function will update OS Memmap.

  @param[in,out] LoadedImage       Loaded boot image info

**/
VOID
UpdateOsMemMap (
  IN OUT  LOADED_IMAGE       *LoadedImage
  )
{
  UINTN                      Index;
  UINT64                     RsvdMemBase;
  UINT64                     RsvdMemSize;
  UINT32                     Count;
  BOOT_PARAMS                *Bp;
  E820_ENTRY                 *E820Entry;
  UINT32                     MmapDiff;
  MULTIBOOT_IMAGE            *MultiBoot;
  OS_CONFIG_DATA_HOB         *OsConfigData;
  UINT32                     TempMemorySize;

  OsConfigData = (OS_CONFIG_DATA_HOB *) GetGuidHobData (NULL, NULL, &gOsConfigDataGuid);
  if ((OsConfigData == NULL) || (OsConfigData->EnableCrashMode == 0)) {
    UpdateOsMemSize (LoadedImage);
    return;
  }

  //
  // Crash mode is enabled, make sure the memory used by payload is set to
  // reserved memory to avoid overriding OS memory in next boot.
  //
  GetPayloadReservedRamRegion (&RsvdMemBase, &RsvdMemSize);
  TempMemorySize  = ALIGN_UP (PcdGet32 (PcdPayloadStackSize), EFI_PAGE_SIZE);
  TempMemorySize += ALIGN_UP (PcdGet32 (PcdPayloadHeapSize),  EFI_PAGE_SIZE);

  //
  // Linux E820 Mmap is very similar with multiboot MMAP
  // Here find the difference to share same update logic below
  //
  if ((LoadedImage->Flags & LOADED_IMAGE_MULTIBOOT) != 0) {
    MultiBoot = &LoadedImage->Image.MultiBoot;
    E820Entry = (E820_ENTRY *) ((UINT8 *)MultiBoot->MbInfo.MmapAddr + sizeof (UINT32));
    Count     = MultiBoot->MbInfo.MmapLength / sizeof (MULTIBOOT_MMAP);
    MmapDiff  = sizeof (UINT32);
  } else {
    Bp        = (BOOT_PARAMS *) (UINTN)BOOT_PARAMS_BASE;
    E820Entry = &Bp->E820Map[0];
    Count     = Bp->E820Entries;
    if (Count >= 128) {
      Count = 128;
    }
    MmapDiff  = 0;
  }

  for (Index = 0; Index < Count; Index++) {
    // Update payload reserved memory map
    if ((E820Entry->Addr == RsvdMemBase) &&  (E820Entry->Size == RsvdMemSize)) {
      E820Entry->Addr = RsvdMemBase - TempMemorySize;
      E820Entry->Size = RsvdMemSize + TempMemorySize;
    }

    // Update free memory map adjcent to payload reserved memory
    if (E820Entry->Addr + E820Entry->Size == RsvdMemBase) {
      E820Entry->Size -= TempMemorySize;
    }

    E820Entry = (E820_ENTRY *) ((UINT8 *)E820Entry + sizeof (E820_ENTRY) + MmapDiff);
  }
  UpdateOsMemSize (LoadedImage);
}

/**
  Display OS boot parameters.

  @param[in]    LoadedImage       Loaded image info

**/
VOID
DisplayInfo (
  IN     LOADED_IMAGE        *LoadedImage
  )
{
DEBUG_CODE_BEGIN ();
  if ((LoadedImage->Flags & LOADED_IMAGE_MULTIBOOT) != 0) {
    DumpMbInfo (&LoadedImage->Image.MultiBoot.MbInfo);
    DumpMbBootState (&LoadedImage->Image.MultiBoot.BootState);
  } else if ((LoadedImage->Flags & LOADED_IMAGE_LINUX) != 0) {
    DumpLinuxBootParams (GetLinuxBootParams ());
  }
DEBUG_CODE_END ();
}

/**
  Update OS boot parameters

  This function will append required command line parameters,
  and update mem map info.

  @param[in]     CurrentBootOption Current boot option
  @param[in,out] LoadedImage       Normal OS boot image
  @param[in,out] LoadedTrustyImage Trusty OS image
  @param[in,out] LoadedExtraImages Extra OS images

  @retval   RETURN_SUCCESS         If update OS parameter success
  @retval   Others                 Failed to update OS parameters
**/
EFI_STATUS
UpdateOsParameters (
  IN     OS_BOOT_OPTION      *CurrentBootOption,
  IN OUT LOADED_IMAGE        *LoadedImage,
  IN OUT LOADED_IMAGE        *LoadedTrustyImage,
  IN OUT LOADED_IMAGE        *LoadedExtraImages
  )
{
  IMAGE_DATA                 *CmdFile;
  BOOLEAN                    SerialNumValidity;
  LOADER_SEED_LIST           *SeedList;
  EFI_STATUS                 Status;

  Status       = RETURN_SUCCESS;

  // Check storage serial number validity. eMMC only for now.
  if (PcdGetBool (PcdSeedListEnabled)) {
    SerialNumValidity = FALSE;
    if (CurrentBootOption->DevType == OsBootDeviceEmmc) {
      Status = EmmcSerialNumCheck();
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_WARN, "EmmcSerialNumCheck failed: %r\n", Status));
        SerialNumValidity = FALSE;
      } else {
        SerialNumValidity = TRUE;
      }
    }
    SeedList = (LOADER_SEED_LIST *) AllocateZeroPool (sizeof (LOADER_SEED_LIST));
    if (SeedList == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    SeedSanityCheck(CurrentBootOption, SeedList);

    if (SerialNumValidity == 1) {
      Status = RpmbKeyProvisioning (CurrentBootOption, SeedList);
      if (!EFI_ERROR(Status)) {
        DEBUG ((DEBUG_INFO, "RpmbKeyProvisioning successful\n"));
      }
    }
    FreePool(SeedList);
  }

  //
  // Multiboot and non-Multiboot share same Cmd Addr
  //
  CmdFile = &LoadedImage->Image.Common.CmdFile;

  if (mAppendCmdLineParams != NULL) {
    if ((AsciiStrSize (mAppendCmdLineParams) + CmdFile->Size) < CMDLINE_LENGTH_MAX) {
      AsciiStrCatS ((CHAR8 *)CmdFile->Addr, CMDLINE_LENGTH_MAX, mAppendCmdLineParams);
    } else {
      DEBUG ((DEBUG_INFO, "Unable to append extra cmd line parameters!\n"));
    }
  }

  CmdFile->Size = CMDLINE_LENGTH_MAX;
  Status = AddSblCommandLine (CurrentBootOption, (CHAR8 *)CmdFile->Addr, &CmdFile->Size, &LoadedImage->ReservedCmdlineData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Update Trusty image if it is loaded.
  //
  if ((CurrentBootOption->BootFlags & BOOT_FLAGS_TRUSTY) != 0) {
    LoadedImage->Image.MultiBoot.CmdBufferSize       = CMDLINE_LENGTH_MAX;
    if (LoadedTrustyImage != NULL) {
      LoadedTrustyImage->Image.MultiBoot.CmdBufferSize = CMDLINE_LENGTH_MAX;
      Status = SetupTrustyBoot (&LoadedTrustyImage->Image.MultiBoot, &LoadedImage->Image.MultiBoot);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "ERROR Setting up Trusty Boot!\n"));
        return Status;
      }
    } else {
      DEBUG ((DEBUG_ERROR, "ERROR Setting up Trusty Boot!\n"));
      return Status;
    }
    UpdateOsMemMap (LoadedTrustyImage);
    DEBUG ((DEBUG_INFO, "\nDump trusty image info:\n"));
    DisplayInfo (LoadedTrustyImage);
  }

  if ((CurrentBootOption->BootFlags & BOOT_FLAGS_EXTRA) != 0) {
    //
    // TODO:
    // Extra image is loaded in LoadedExtraImages
    // update OS boot parameter here.
    //
    DEBUG ((DEBUG_ERROR, "Warning: Extra image parameters are not handled yet.\n"));
    if (LoadedExtraImages == NULL) {
      DEBUG ((DEBUG_ERROR, "Warning: Extra image not loaded.\n"));
    }
  }

  UpdateOsMemMap (LoadedImage);
  if ((LoadedImage->Flags & LOADED_IMAGE_LINUX) != 0) {
    GetLinuxBootParams ()->Hdr.CmdlineSize = LoadedImage->Image.Linux.CmdFile.Size;
  }
  DEBUG ((DEBUG_INFO, "\nDump normal boot image info:\n"));
  DisplayInfo (LoadedImage);

  return Status;
}
