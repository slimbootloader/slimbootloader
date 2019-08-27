/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OsLoader.h"

CONST CHAR16  *mConfigFileName[2] = {
  L"config.cfg",
  L"boot/grub/grub.cfg"
};

/**
  Get hardware partition handle from boot option info

  This function will initialize boot device, and get hardware partition
  handle based on boot option.

  @param[in]  BootOption      Current boot option
  @param[out] HwPartHandle    Hardware partition handle for boot image

  @retval  RETURN_SUCCESS     If partition was found successfully
  @retval  Others             If partition was not found
**/
EFI_STATUS
FindBootPartition (
  IN  OS_BOOT_OPTION         *BootOption,
  OUT EFI_HANDLE             *HwPartHandle
  )
{
  RETURN_STATUS              Status;
  UINTN                      BootMediumPciBase;

  AddMeasurePoint (0x4040);
  //
  // Get OS boot device address
  //
  BootMediumPciBase = GetDeviceAddr (BootOption->DevType, BootOption->DevInstance);
  DEBUG ((DEBUG_INFO, "BootMediumPciBase(0x%x)\n", BootMediumPciBase));
  BootMediumPciBase = TO_MM_PCI_ADDRESS (BootMediumPciBase);

  //
  // Init Boot device functions
  //
  Status = MediaSetInterfaceType (BootOption->DevType);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to set media interface - %r\n", Status));
    return RETURN_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "Getting boot image from %a\n", GetBootDeviceNameString(BootOption->DevType)));
  Status = MediaInitialize (BootMediumPciBase, DevInitAll);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to init media - %r\n", Status));
    return Status;
  }

  AddMeasurePoint (0x4050);
  MediaTuning (BootMediumPciBase);
  AddMeasurePoint (0x4055);

  DEBUG ((DEBUG_INFO, "Try to find boot partition\n"));
  Status = FindPartitions (BootOption->HwPart, HwPartHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to find partition - %r\n", Status));
    ClosePartitions (*HwPartHandle);
    return Status;
  }
  AddMeasurePoint (0x4060);

  DEBUG ((DEBUG_INFO, "Find partition success\n"));
  return RETURN_SUCCESS;
}


/**
  Get IAS image from raw partition

  Using boot option info, this function will read IAS image from raw
  partition based on hardware partition info saved in LoadedImage.
  After IAS image is loaded into memory, its information will be saved
  to LoadedImage.

  @param[in]      BootOption      Current boot option
  @param[in, out] LoadedImage     Loaded Image information.

  @retval  RETURN_SUCCESS     If IAS image was loaded successfully
  @retval  Others             If IAS image was not loaded.
**/
EFI_STATUS
GetIasImageFromRawPartition (
  IN     OS_BOOT_OPTION      *BootOption,
  IN OUT LOADED_IMAGE        *LoadedImage
  )
{
  RETURN_STATUS              Status;
  DEVICE_BLOCK_INFO          BlockInfo;
  VOID                       *Buffer;
  UINTN                      ImageSize;
  LOGICAL_BLOCK_DEVICE       LogicBlkDev;
  UINTN                      AlginedHeaderSize;
  UINTN                      AlginedImageSize;
  UINT32                     BlockSize;
  UINT8                      BlockData[4096];
  EFI_LBA                    LbaAddr;
  UINT8                      SwPart;

  SwPart  = BootOption->Image[LoadedImage->LoadImageType].LbaImage.SwPart;
  LbaAddr = BootOption->Image[LoadedImage->LoadImageType].LbaImage.LbaAddr;

  //
  // The image_B partition number, is image_A partition number + 1
  // They share same LBA offset address.
  //
  if ((BootOption->BootFlags & LOAD_IMAGE_FROM_BACKUP) != 0) {
    if ((LoadedImage->LoadImageType == LOAD_IMAGE_TRUSTY) || (LoadedImage->LoadImageType == LOAD_IMAGE_NORMAL)) {
      SwPart++;
    }
  }

  DEBUG ((DEBUG_INFO, "Load image from SwPart (0x%x), LbaAddr(0x%llx)\n", SwPart, LbaAddr));
  Status = GetLogicalPartitionInfo (SwPart, LoadedImage->HwPartHandle, &LogicBlkDev);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Get logical partition error, Status = %r\n", Status));
    return Status;
  }

  Status = MediaGetMediaInfo (BootOption->HwPart, &BlockInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "GetMediaInfo Error %r\n", Status));
    return Status;
  }

  //
  // Read the IAS Header first to get total size of the IAS image.
  // Make sure to round the Header size to be block aligned in bytes.
  //
  BlockSize = BlockInfo.BlockSize;
  AlginedHeaderSize = ((sizeof (IAS_HEADER) % BlockSize) == 0) ? \
                      sizeof (IAS_HEADER) : \
                      ((sizeof (IAS_HEADER) / BlockSize) + 1) * BlockSize;
  Status = MediaReadBlocks (
             BootOption->HwPart,
             LogicBlkDev.StartBlock + LbaAddr,
             AlginedHeaderSize,
             BlockData
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Read IAS image error, Status = %r\n", Status));
    return Status;
  }

  //
  // Make sure to round the image size to be block aligned in bytes.
  //
  ImageSize        = IAS_IMAGE_SIZE ((IAS_HEADER *) BlockData);
  AlginedImageSize = ((ImageSize % BlockSize) == 0) ? \
                     ImageSize : \
                     ((ImageSize / BlockSize) + 1) * BlockSize;
  if (AlginedImageSize > MAX_IAS_IMAGE_SIZE) {
    DEBUG ((DEBUG_INFO, "IAS image is bigger than limitation (0x%x). ImageSize=0x%x\n",
            MAX_IAS_IMAGE_SIZE, AlginedImageSize));
    return EFI_LOAD_ERROR;
  }

  Buffer = (UINT8 *) AllocatePages (EFI_SIZE_TO_PAGES (AlginedImageSize));
  if (Buffer == NULL) {
    DEBUG ((DEBUG_INFO, "Allocate memory (size:0x%x) fail.\n", AlginedImageSize));
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (Buffer, BlockData, AlginedHeaderSize);

  //
  // Read the rest of the IAS image into the buffer
  //
  Status = MediaReadBlocks (
             BootOption->HwPart,
             LogicBlkDev.StartBlock + LbaAddr + (AlginedHeaderSize / BlockSize),
             AlginedImageSize,
             & (((UINT8 *)Buffer)[AlginedHeaderSize])
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Read rest of IAS image error, Status = %r\n", Status));
    return  Status;
  }

  if ((Buffer == NULL) || (ImageSize == 0)) {
    return EFI_LOAD_ERROR;
  }

  LoadedImage->IasImage.Addr = Buffer;
  LoadedImage->IasImage.Size = ImageSize;
  LoadedImage->Flags        |= LOADED_IMAGE_IAS;
  return EFI_SUCCESS;
}


/**
  Get IAS image from file

  This function will read IAS image from file based on FsHandle.
  After IAS image is loaded into memory, its information will be saved
  to LoadedImage.

  @param[in]  FsHandle        File system handle used to read file
  @param[in]  BootOption      Current boot option
  @param[out] LoadedImage     Loaded Image information.

  @retval  RETURN_SUCCESS     If IAS image was loaded successfully
  @retval  Others             If IAS image was not loaded.
**/
EFI_STATUS
GetIasImageFromFs (
  IN  EFI_HANDLE             FsHandle,
  IN  OS_BOOT_OPTION         *BootOption,
  OUT LOADED_IMAGE           *LoadedImage
  )
{
  RETURN_STATUS              Status;
  CHAR16                     FilePath[MAX_FILE_PATH_LEN];
  VOID                       *Image;
  UINTN                      ImageSize;
  CONST CHAR8                *FileName;
  EFI_HANDLE                 FileHandle;

  if (FsHandle == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  FileName = (CONST CHAR8 *)&BootOption->Image[LoadedImage->LoadImageType].FileName[0];

  // Load IAS Image from file system
  AsciiStrToUnicodeStrS (FileName, FilePath, sizeof (FilePath) / sizeof (CHAR16));

  FileHandle = NULL;
  Status = OpenFile (FsHandle, FilePath, &FileHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Open file '%a' failed, Status = %r\n", FileName, Status));
    goto Done;
  }

  Status = GetFileSize (FileHandle, &ImageSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Get file size failed, Status = %r\n", Status));
    goto Done;
  }
  DEBUG ((DEBUG_INFO, "File '%a' size %d\n", FileName, ImageSize));

  if (ImageSize == 0) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  Image = AllocatePool (ImageSize);
  if (Image == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  Status = ReadFile (FileHandle, &Image, &ImageSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Read file '%a' failed, Status = %r\n", FileName, Status));
    if (Image != NULL) {
      FreePool (Image);
    }
    goto Done;
  }
  DEBUG ((DEBUG_INFO, "Get file '%s' (size:0x%x) success.\n", FilePath, ImageSize));

  LoadedImage->IasImage.Addr = Image;
  LoadedImage->IasImage.Size = ImageSize;
  LoadedImage->Flags        |= LOADED_IMAGE_IAS;

Done:
  if (FileHandle != NULL) {
    CloseFile (FileHandle);
  }

  return Status;
}


/**
  Load a file from media and fill in the loaded file information.

  @param[in]  FsHandle        File system handle used to read file
  @param[in]  ConfigFile      Configuration file buffer.
  @param[in]  FileInfo        Pointer to the file informatino in buffer.
  @param[out] ImageData       Pointer to receive the loaded file address and size.

  @retval  RETURN_SUCCESS     If image was loaded successfully
  @retval  Others             If image was not loaded.
**/
STATIC
EFI_STATUS
LoadLinuxFile (
  IN  EFI_HANDLE             FsHandle,
  IN  CHAR8                 *ConfigFile,
  IN  STR_SLICE             *FileInfo,
  OUT IMAGE_DATA            *ImageData
  )
{
  EFI_STATUS  Status;
  VOID       *FileBuffer;
  UINTN       FileSize;
  CHAR8      *Ptr;
  CHAR16      FileName[256];
  EFI_HANDLE  FileHandle;

  if (FileInfo->Len == 0) {
    return EFI_NOT_FOUND;
  }

  if ((FileInfo->Buf[0] == 0) && (ConfigFile != NULL)) {
    Ptr = ConfigFile + FileInfo->Pos;
  } else {
    Ptr = FileInfo->Buf + FileInfo->Pos;
  }

  Ptr[FileInfo->Len] = 0;
  AsciiStrToUnicodeStrS (Ptr, FileName, sizeof(FileName) / sizeof(CHAR16));
  FileSize   = 0;
  FileBuffer = NULL;
  FileHandle = NULL;
  Status = OpenFile (FsHandle, FileName, &FileHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Open file '%s' failed, Status = %r\n", FileName, Status));
    goto Done;
  }

  Status = GetFileSize (FileHandle, &FileSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Get file '%s' size failed, Status = %r\n", FileName, Status));
    goto Done;
  }

  FileBuffer = AllocatePool (FileSize);
  if (FileBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  Status = ReadFile (FileHandle, &FileBuffer, &FileSize);
  DEBUG ((DEBUG_INFO, "Load file %a [size %d bytes]: %r\n", Ptr, FileSize, Status));
  if (!EFI_ERROR (Status)) {
    ImageData->Addr = FileBuffer;
    ImageData->Size = FileSize;
  } else {
    if (FileBuffer != NULL) {
      FreePool (FileBuffer);
    }
  }

Done:
  if (FileHandle != NULL) {
    CloseFile (FileHandle);
  }
  return Status;
}

/**
  Get traditional linux image from file

  This function will read traditional linux files (vmlinuz, config.cfg
  and initrd) from file system into memory. The loaded file info will
  be saved to LinuxImage.

  @param[in]  FsHandle        File system handle used to read file
  @param[out] LinuxImage      Used to save loaded Image information.

  @retval  RETURN_SUCCESS     If IAS image was loaded successfully
  @retval  Others             If IAS image was not loaded.
**/
EFI_STATUS
GetTraditionalLinux (
  IN  EFI_HANDLE             FsHandle,
  OUT LINUX_IMAGE            *LinuxImage
  )
{
  RETURN_STATUS              Status;
  UINTN                      ConfigFileSize;
  VOID                       *ConfigFile;
  LINUX_BOOT_CFG             LinuxBootCfg;
  UINT32                     Index;
  UINT32                     EntryIdx;
  CHAR8                      *Ptr;
  MENU_ENTRY                 *MenuEntry;
  EFI_HANDLE                 FileHandle;
  BOOLEAN                    DefBootOption;

  ConfigFile     = NULL;
  ConfigFileSize = 0;
  DefBootOption  = FALSE;
  Status = RETURN_NOT_FOUND;

  DEBUG ((DEBUG_INFO, "Try booting Linux from config file ...\n"));

  for (Index = 0; Index < (UINTN)(FeaturePcdGet (PcdGrubBootCfgEnabled) ? 2 : 1); Index++) {
    DEBUG ((DEBUG_INFO, "Checking %s\n",mConfigFileName[Index]));
    ConfigFile     = NULL;
    ConfigFileSize = 0;

    Status = OpenFile (FsHandle, (CHAR16 *)mConfigFileName[Index], &FileHandle);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Open file '%s' failed, Status = %r\n", (CHAR16 *)mConfigFileName[Index], Status));
      continue;
    }

    Status = GetFileSize (FileHandle, &ConfigFileSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Get file '%s' size failed, Status = %r\n", (CHAR16 *)mConfigFileName[Index], Status));
      CloseFile (FileHandle);
      continue;
    }

    ConfigFile = AllocatePool (ConfigFileSize);
    if (ConfigFile == NULL) {
      CloseFile (FileHandle);
      return EFI_OUT_OF_RESOURCES;
    }

    Status = ReadFile (FileHandle, &ConfigFile, &ConfigFileSize);
    CloseFile (FileHandle);
    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Load file %s [size 0x%x]: %r\n", (CHAR16 *)mConfigFileName[Index], ConfigFileSize, Status));
      break;
    }
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Could not find configuration file!\n"));
    // No any config was found, try to load vmlinuz/initrd directly.
    if (ConfigFile != NULL) {
      FreePool (ConfigFile);
      ConfigFile = NULL;
    }
    ConfigFileSize = 0;
  }

  EntryIdx = 0;
  ZeroMem (&LinuxBootCfg, sizeof (LINUX_BOOT_CFG));
  if (FeaturePcdGet (PcdGrubBootCfgEnabled)) {
    // Process the config file and
    // Get boot option from user if timeout is non-zero
    if (ConfigFileSize > 0) {
      ParseLinuxBootConfig (ConfigFile, &LinuxBootCfg);
      PrintLinuxBootConfig (ConfigFile, &LinuxBootCfg);
      EntryIdx = GetLinuxBootOption (ConfigFile, &LinuxBootCfg);
    }
  }

  if (LinuxBootCfg.EntryNum == 0) {
    // Build a default boot option
    DefBootOption = TRUE;
    LinuxBootCfg.EntryNum   = 1;
    MenuEntry = LinuxBootCfg.MenuEntry;
    MenuEntry[0].Name.Pos    = 0;
    MenuEntry[0].Name.Len    = 5;
    AsciiStrCpyS (MenuEntry[0].Name.Buf, sizeof(MenuEntry[0].Name.Buf), "Linux");
    MenuEntry[0].InitRd.Pos  = 0;
    MenuEntry[0].InitRd.Len  = 6;
    AsciiStrCpyS (MenuEntry[0].InitRd.Buf, sizeof(MenuEntry[0].InitRd.Buf), "initrd");
    MenuEntry[0].Kernel.Pos  = 0;
    MenuEntry[0].Kernel.Len  = 7;
    AsciiStrCpyS (MenuEntry[0].Kernel.Buf, sizeof(MenuEntry[0].Kernel.Buf), "vmlinuz");
    MenuEntry[0].Command.Pos = 0;
    MenuEntry[0].Command.Len = ConfigFileSize;
    EntryIdx = 0;
  }

  // Load kernel image
  Status = LoadLinuxFile (FsHandle, ConfigFile, &LinuxBootCfg.MenuEntry[EntryIdx].Kernel, &LinuxImage->BootFile);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Load kernel failed!\n"));
    Status = RETURN_LOAD_ERROR;
    goto Done;
  }

  // Update command line
  LinuxImage->CmdFile.Size = LinuxBootCfg.MenuEntry[EntryIdx].Command.Len;
  if ((LinuxImage->CmdFile.Size > 0) && (ConfigFile != NULL)) {
    Ptr = (CHAR8 *)ConfigFile + LinuxBootCfg.MenuEntry[EntryIdx].Command.Pos;
    Ptr[LinuxImage->CmdFile.Size] = 0;
    LinuxImage->CmdFile.Addr = Ptr;
  } else {
    LinuxImage->CmdFile.Addr = 0;
  }

  // Load InitRd, optional
  Status = LoadLinuxFile (FsHandle, ConfigFile, &LinuxBootCfg.MenuEntry[EntryIdx].InitRd, &LinuxImage->InitrdFile);
  if (EFI_ERROR (Status)) {
    if ((Status == EFI_NOT_FOUND) && ((LinuxBootCfg.MenuEntry[EntryIdx].InitRd.Len == 0) || DefBootOption)) {
      return RETURN_SUCCESS;
    } else {
      DEBUG ((DEBUG_ERROR, "Load initrd failed!\n"));
      Status = RETURN_LOAD_ERROR;
    }
  }

  LinuxImage->ExtraBlobNumber = 0;

Done:
  if (ConfigFile != NULL) {
    FreePool (ConfigFile);
  }
  return Status;
}


/**
  Load Image from boot media.

  This function will initialize OS boot device, and load image based on
  boot option, the loaded image info will be saved in  LoadedImage.

  @param[in]  BootOption     Current boot option
  @param[out] LoadedImage    Loaded Image information.

  @retval  RETURN_SUCCESS    If image was loaded successfully
  @retval  Others            If image was not loaded.
**/
EFI_STATUS
GetImageFromMedia (
  IN  OS_BOOT_OPTION         *BootOption,
  OUT LOADED_IMAGE           *LoadedImage
  )
{
  RETURN_STATUS              Status;
  EFI_HANDLE                 FsHandle;
  EFI_HANDLE                 HwPartHandle;
  EFI_HOB_GUID_TYPE          *GuidHob;
  EXT_BOOT_LOADER_VERSION    *VersionHob;

  FsHandle     = NULL;
  HwPartHandle = LoadedImage->HwPartHandle;
  if (BootOption->FsType >= EnumFileSystemMax) {
    Status = GetIasImageFromRawPartition (BootOption, LoadedImage);
    return Status;
  }

  // Load image from file system.
  DEBUG ((DEBUG_INFO, "Init File system\n"));
  Status = InitFileSystem (BootOption->SwPart, EnumFileSystemTypeAuto, HwPartHandle, &FsHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "No partitions found, Status = %r\n", Status));
    return Status;
  }

  Status = GetIasImageFromFs (FsHandle, BootOption, LoadedImage);
  if (!EFI_ERROR (Status)) {
    return Status;
  }

  // Enable tradition linux boot only in debug build
  GuidHob = GetNextGuidHob (&gBootLoaderVersionGuid, GetHobListPtr ());
  if (GuidHob != NULL) {
    VersionHob = (EXT_BOOT_LOADER_VERSION *)GET_GUID_HOB_DATA (GuidHob);
    if ((VersionHob->Version.ImageVersion.BldDebug != 0) ||
         !FeaturePcdGet (PcdVerifiedBootEnabled)) {
      Status = GetTraditionalLinux (FsHandle, &LoadedImage->Image.Linux);
      if (!EFI_ERROR (Status)) {
        LoadedImage->Flags |= LOADED_IMAGE_LINUX;
      }
    }
  }

  return Status;
}
