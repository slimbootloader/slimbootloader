/** @file

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OsLoader.h"

#define LOADED_IMAGES_INFO_SIGNATURE   SIGNATURE_32 ('L', 'I', 'I', 'S')

typedef struct {
  UINTN                   Signature;
  LOADED_IMAGE           *LoadedImageList[LoadImageTypeMax];
} LOADED_IMAGES_INFO;

STATIC CONST CHAR16  *mConfigFileName[3] = {
  L"config.cfg",
  L"boot/grub/grub.cfg",
  L"EFI/BOOT/grub.cfg"
};

/**
  Get boot image from a IFWI container component

  This function will load component from a container inside the IFWI
  using the container signature and the component name from boot option.
  After Boot image is loaded into memory, its information will be saved
  to LoadedImage.

  @param[in]      BootOption      Current boot option
  @param[in, out] LoadedImage     Loaded Image information.

  @retval  RETURN_SUCCESS     If a boot image was loaded successfully
  @retval  Others             If a boot image was not loaded.
**/
EFI_STATUS
GetBootImageFromIfwiContainer (
  IN     OS_BOOT_OPTION      *BootOption,
  IN OUT LOADED_IMAGE        *LoadedImage
  )
{
  RETURN_STATUS              Status;
  VOID                       *Buffer;
  UINT32                     ImageSize;
  CONTAINER_IMAGE           *Image;

  Image = &BootOption->Image[LoadedImage->LoadImageType].ContainerImage;
  if ((Image->Indicate != '!') || (Image->BackSlash != '/')) {
    return EFI_INVALID_PARAMETER;
  }

  Buffer    = NULL;
  ImageSize = 0;
  Status = LoadComponent (Image->ContainerSig, Image->ComponentName, (VOID **)&Buffer, &ImageSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Load component (%x/%x) error - %r\n", Image->ContainerSig, Image->ComponentName, Status));
    return Status;
  }

  LoadedImage->Flags |= LOADED_IMAGE_COMPONENT;
  LoadedImage->ImageData.Addr = Buffer;
  LoadedImage->ImageData.Size = ImageSize;
  LoadedImage->ImageData.AllocType = ImageAllocateTypePage;

  if ( *((UINT32 *) Buffer) == CONTAINER_BOOT_SIGNATURE ) {
    LoadedImage->Flags      |= LOADED_IMAGE_CONTAINER;
  } else if ( *((UINT32 *) Buffer) == IAS_MAGIC_PATTERN ) {
    LoadedImage->Flags      |= LOADED_IMAGE_IAS;
  }

  DEBUG ((DEBUG_ERROR, "Loaded component (%x/%x) success.\n", Image->ContainerSig, Image->ComponentName));
  return EFI_SUCCESS;
}

/**
  Get Boot image from raw partition

  Using boot option info, this function will read Boot image from raw
  partition based on hardware partition info saved in LoadedImage.
  After Boot image is loaded into memory, its information will be saved
  to LoadedImage.

  @param[in]      BootOption      Current boot option
  @param[in, out] LoadedImage     Loaded Image information.

  @retval  RETURN_SUCCESS     If IAS image was loaded successfully
  @retval  Others             If IAS image was not loaded.
**/
STATIC
EFI_STATUS
GetBootImageFromRawPartition (
  IN     OS_BOOT_OPTION      *BootOption,
  IN OUT LOADED_IMAGE        *LoadedImage
  )
{
  RETURN_STATUS              Status;
  DEVICE_BLOCK_INFO          BlockInfo;
  VOID                       *Buffer;
  UINTN                      ImageSize;
  LOGICAL_BLOCK_DEVICE       LogicBlkDev;
  UINTN                      AlignedHeaderSize;
  UINTN                      AlignedImageSize;
  UINTN                      AlignedHeaderBlkCnt;
  UINT32                     BlockSize;
  VOID                      *BlockData;
  EFI_LBA                    LbaAddr;
  UINT8                      SwPart;
  UINT64                     Address;
  CONTAINER_HDR             *ContainerHdr;

  SwPart   = BootOption->Image[LoadedImage->LoadImageType].LbaImage.SwPart;
  LbaAddr  = BootOption->Image[LoadedImage->LoadImageType].LbaImage.LbaAddr;

  //
  // The image_B partition number, is image_A partition number + 1
  // They share same LBA offset address.
  //
  if ((BootOption->BootFlags & LOAD_IMAGE_FROM_BACKUP) != 0) {
    if ((LoadedImage->LoadImageType == BOOT_FLAGS_PREOS)
      || (LoadedImage->LoadImageType == LoadImageTypeNormal)) {
      SwPart++;
    }
  }

  DEBUG ((DEBUG_INFO, "Load image from SwPart (0x%x), LbaAddr(0x%llx)\n", SwPart, LbaAddr));
  if (SwPart != 0xFF) {
    Status = GetLogicalPartitionInfo (SwPart, LoadedImage->HwPartHandle, &LogicBlkDev);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Get logical partition error - %r\n", Status));
      return Status;
    }
  } else {
    DEBUG ((DEBUG_INFO, "SwPart is 255, LbaAddr will be treated as an absolute LBA\n"));
    LogicBlkDev.StartBlock = 0;
  }

  Status = MediaGetMediaInfo (BootOption->HwPart, &BlockInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Get media info error - %r\n", Status));
    return Status;
  }

  //
  // Read the IAS Header first to get total size of the IAS image.
  // Make sure to round the Header size to be block aligned in bytes.
  //
  BlockSize = BlockInfo.BlockSize;
  AlignedHeaderBlkCnt = (sizeof (IAS_HEADER) + (BlockSize - 1)) / BlockSize;
  AlignedHeaderSize = AlignedHeaderBlkCnt * BlockSize;

  BlockData = AllocatePages (EFI_SIZE_TO_PAGES (AlignedHeaderSize));
  if (BlockData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Address =  LogicBlkDev.StartBlock + LbaAddr;

  Status = MediaReadBlocks (
             BootOption->HwPart,
             Address,
             AlignedHeaderSize,
             BlockData
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Read image error - %r\n", Status));
    return Status;
  }

  //
  // Make sure to round the image size to be block aligned in bytes.
  //
  ContainerHdr = (CONTAINER_HDR *)BlockData;
  if (ContainerHdr->Signature == CONTAINER_BOOT_SIGNATURE) {
    ImageSize = ContainerHdr->DataOffset + ContainerHdr->DataSize;
  } else if (ContainerHdr->Signature == IAS_MAGIC_PATTERN) {
    ImageSize = IAS_IMAGE_SIZE ((IAS_HEADER *) BlockData);
  } else {
    DEBUG ((DEBUG_INFO, "No valid image header found !\n"));
    return EFI_LOAD_ERROR;
  }

  //
  // Check image size from the image header
  //
  if (ImageSize == 0) {
    return EFI_LOAD_ERROR;
  }

  AlignedImageSize = ((ImageSize % BlockSize) == 0) ? \
                     ImageSize : \
                     ((ImageSize / BlockSize) + 1) * BlockSize;
  if (AlignedImageSize > MAX_IAS_IMAGE_SIZE) {
    DEBUG ((DEBUG_INFO, "Image is bigger than limitation (0x%x). ImageSize=0x%x\n",
            MAX_IAS_IMAGE_SIZE, AlignedImageSize));
    //
    // Free temporary pages used for image header
    //
    FreePages (BlockData, EFI_SIZE_TO_PAGES (AlignedHeaderSize));
    return EFI_LOAD_ERROR;
  }

  Buffer = (UINT8 *) AllocatePages (EFI_SIZE_TO_PAGES (AlignedImageSize));
  if (Buffer == NULL) {
    DEBUG ((DEBUG_INFO, "Allocate memory (size:0x%x) fail.\n", AlignedImageSize));
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (Buffer, BlockData, AlignedHeaderSize);

  //
  // Free temporary pages used for image header
  //
  FreePages (BlockData, EFI_SIZE_TO_PAGES (AlignedHeaderSize));

  //
  // Read the rest of the IAS image into the buffer
  //
  Address =  LogicBlkDev.StartBlock + LbaAddr + AlignedHeaderBlkCnt;

  Status = MediaReadBlocks (
             BootOption->HwPart,
             Address,
             (AlignedImageSize - AlignedHeaderSize),
             (VOID *)((UINTN)Buffer + AlignedHeaderSize)
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Read rest of image error - %r\n", Status));
    FreePages (Buffer, EFI_SIZE_TO_PAGES (AlignedImageSize));
    return Status;
  }

  LoadedImage->ImageData.Addr = Buffer;
  LoadedImage->ImageData.Size = (UINT32)ImageSize;
  LoadedImage->ImageData.AllocType = ImageAllocateTypePage;
  if ( *((UINT32 *) Buffer) == CONTAINER_BOOT_SIGNATURE ) {
    LoadedImage->Flags      |= LOADED_IMAGE_CONTAINER;
  } else if ( *((UINT32 *) Buffer) == IAS_MAGIC_PATTERN ) {
    LoadedImage->Flags      |= LOADED_IMAGE_IAS;
  }
  return EFI_SUCCESS;
}

/**
  Get Boot image from File System

  This function will read Boot image from file based on FsHandle.
  After Boot image is loaded into memory, its information will be saved
  to LoadedImage.

  @param[in]  FsHandle        File system handle used to read file
  @param[in]  BootOption      Current boot option
  @param[out] LoadedImage     Loaded Image information.

  @retval  RETURN_SUCCESS     If Boot image was loaded successfully
  @retval  Others             If Boot image was not loaded.
**/
STATIC
EFI_STATUS
GetBootImageFromFs (
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

  // Load Boot Image from file system
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

  if (LoadedImage->LoadImageType == LoadImageTypeExtra0) {
     //
     // Currently extra image is used only by RTCM which need allocate reserved memory.
     // Need revise this logic if there is other use case late.
     //
     Image = AllocateReservedPages (EFI_SIZE_TO_PAGES(ImageSize));
     LoadedImage->Flags |= LOADED_IMAGE_RUN_EXTRA;
  } else {
    Image = AllocatePages (EFI_SIZE_TO_PAGES (ImageSize));
  }
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

  LoadedImage->ImageData.Addr = Image;
  LoadedImage->ImageData.Size = (UINT32)ImageSize;
  LoadedImage->ImageData.AllocType = ImageAllocateTypePage;
  if ( *((UINT32 *) Image) == CONTAINER_BOOT_SIGNATURE ) {
    LoadedImage->Flags      |= LOADED_IMAGE_CONTAINER;
  } else if ( *((UINT32 *) Image) == IAS_MAGIC_PATTERN ) {
    LoadedImage->Flags      |= LOADED_IMAGE_IAS;
  }

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

  FileBuffer = AllocatePages (EFI_SIZE_TO_PAGES(FileSize));
  if (FileBuffer == NULL) {
    DEBUG ((DEBUG_INFO, "Unable to allocate memory to load file '%s' (size: %d) \n", FileName, FileSize));
    DEBUG ((DEBUG_INFO, "Please increase PLD_HEAP_SIZE\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  Status = ReadFile (FileHandle, &FileBuffer, &FileSize);
  DEBUG ((DEBUG_INFO, "Load file %a [size %d bytes]: %r\n", Ptr, FileSize, Status));
  if (!EFI_ERROR (Status)) {
    // Free pre-allocated memory
    FreeImageData (ImageData);
    // Re-assign new memory
    ImageData->Addr = FileBuffer;
    ImageData->Size = (UINT32)FileSize;
    ImageData->AllocType = ImageAllocateTypePage;
  } else {
    if (FileBuffer != NULL) {
      FreePages (FileBuffer, EFI_SIZE_TO_PAGES(FileSize));
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
STATIC
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
  UINT32                     Size;

  ConfigFile     = NULL;
  ConfigFileSize = 0;
  DefBootOption  = FALSE;
  Status = RETURN_NOT_FOUND;

  DEBUG ((DEBUG_INFO, "Try booting Linux from config file ...\n"));

  for (Index = 0; Index < (UINTN)(FeaturePcdGet (PcdGrubBootCfgEnabled) ? 3 : 1); Index++) {
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

    // Allocate one more space to append NULL char
    ConfigFile = AllocatePool (ConfigFileSize + 1);
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

    FreePool (ConfigFile);
    ConfigFile = NULL;
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
    if (ConfigFileSize > 0 && ConfigFile != NULL) {
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
    MenuEntry[0].Command.Len = (UINT32)ConfigFileSize;
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
  Size = LinuxBootCfg.MenuEntry[EntryIdx].Command.Len;
  FreeImageData (&LinuxImage->CmdFile);
  if ((Size > 0) && (ConfigFile != NULL)) {
    LinuxImage->CmdFile.Addr = AllocatePool (Size);
    if (LinuxImage->CmdFile.Addr == NULL) {
      FreePool (ConfigFile);
      return EFI_OUT_OF_RESOURCES;
    }
    LinuxImage->CmdFile.Size = Size;

    Ptr = (CHAR8 *)ConfigFile + LinuxBootCfg.MenuEntry[EntryIdx].Command.Pos;
    CopyMem (LinuxImage->CmdFile.Addr, Ptr, Size);
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
  Get a pointer of Loaded Image which has specific LoadImageType

  This function will return the pointer address of a Loaded Image with given
  LoadImageType.

  @param[in]  LoadedImageHandle       Loaded Image handle
  @param[in]  LoadImageType           Load Image Type Index
  @param[out] LoadedImage             Loaded Image

  @retval     EFI_SUCCESS             Found a Loaded Image from the handle successfully
  @retval     EFI_INVALID_PARAMETER   If Loaded Image handle is invalid
  @retval     EFI_NOT_FOUND           Not found a Loaded Image from the handle

**/
EFI_STATUS
EFIAPI
GetLoadedImageByType (
  IN   EFI_HANDLE         LoadedImageHandle,
  IN   LOAD_IMAGE_TYPE    LoadImageType,
  OUT  LOADED_IMAGE     **LoadedImage
  )
{
  LOADED_IMAGES_INFO  *LoadedImagesInfo;
  LOADED_IMAGE        *LoadedImagePtr;

  LoadedImagesInfo = (LOADED_IMAGES_INFO *)LoadedImageHandle;
  if ((LoadedImagesInfo == NULL) || (LoadedImagesInfo->Signature != LOADED_IMAGES_INFO_SIGNATURE)) {
    return EFI_INVALID_PARAMETER;
  }

  LoadedImagePtr = LoadedImagesInfo->LoadedImageList[LoadImageType];
  if (LoadedImagePtr == NULL) {
    *LoadedImage = NULL;
    return EFI_NOT_FOUND;
  }

  *LoadedImage = LoadedImagePtr;

  return EFI_SUCCESS;
}

/**
  Free the allocated memory in an image data

  This function free a memory allocated in IMAGE_DATA according to Allocation Type.

  @param[in]  ImageData       An image data pointer which has allocated memory address,
                              its size, and allocation type.

**/
VOID
FreeImageData (
  IN  IMAGE_DATA    *ImageData
  )
{
  if ((ImageData == NULL) || (ImageData->Addr == NULL) || (ImageData->Size == 0)) {
    return;
  }

  if (ImageData->AllocType >= ImageAllocateTypeMax) {
    return;
  } else if (ImageData->AllocType == ImageAllocateTypePool) {
    FreePool (ImageData->Addr);
  } else if (ImageData->AllocType == ImageAllocateTypePage) {
    FreePages (ImageData->Addr, EFI_SIZE_TO_PAGES (ImageData->Size));
  }
  ZeroMem (ImageData, sizeof (IMAGE_DATA));
}

/**
  Free all allocated memory in a loaded image

  This function will clean up all temporary resources used to load a single image.

  @param[in]  LoadedImage     A load image pointer which has a boot image info

**/
VOID
UnloadLoadedImage (
  IN  LOADED_IMAGE  *LoadedImage
  )
{
  COMMON_IMAGE               *CommonImage;
  LINUX_IMAGE                *LinuxImage;
  MULTIBOOT_IMAGE            *MultiBootImage;
  MULTIBOOT_MODULE_DATA      *MbModuleData;
  TRUSTY_IMAGE_DATA          *TrustyImageData;
  MULTIBOOT_INFO             *MbInfo;
  MULTIBOOT2_INFO            *Mb2Info;
  UINT32                      Index;
  UINT32                      Count;

  if (LoadedImage == NULL) {
    return;
  }

  //
  // Free Boot Image Data loaded from FS or RAW partition
  //
  FreeImageData (&LoadedImage->ImageData);

  //
  // Free Common Boot & Cmdline Data
  //
  CommonImage = &LoadedImage->Image.Common;
  FreeImageData (&CommonImage->BootFile);
  FreeImageData (&CommonImage->CmdFile);

  //
  // Free Linux Image Data
  //
  if (LoadedImage->Flags & LOADED_IMAGE_LINUX) {
    LinuxImage = &LoadedImage->Image.Linux;
    FreeImageData (&LinuxImage->InitrdFile);

    Count = ARRAY_SIZE (LinuxImage->ExtraBlob);
    for (Index = 0; Index < Count; Index++) {
      FreeImageData (&LinuxImage->ExtraBlob[Index]);
    }
  }

  //
  // Free MultiBoot Image Data
  //
  if (LoadedImage->Flags & LOADED_IMAGE_MULTIBOOT) {
    MultiBootImage = &LoadedImage->Image.MultiBoot;

    // Free Module Image Data
    MbModuleData = MultiBootImage->MbModuleData;
    for (Index = 0; Index < MultiBootImage->MbModuleNumber; Index++) {
      FreeImageData (&MbModuleData[Index].CmdFile);
      FreeImageData (&MbModuleData[Index].ImgFile);
    }

    // Free Trusty Image Data
    TrustyImageData = &MultiBootImage->TrustyImageData;
    FreeImageData (&TrustyImageData->VmmImageData.VmmRuntimeAddr);
    FreeImageData (&TrustyImageData->VmmImageData.VmmHeapAddr);
    FreeImageData (&TrustyImageData->VmmImageData.VmmBootParams);
    FreeImageData (&TrustyImageData->BootParamsData.PlatformInfo);
    FreeImageData (&TrustyImageData->BootParamsData.SeedList);
    FreeImageData (&TrustyImageData->BootParamsData.Base);

    // Free MmapAddr which is allocated in SetupMultibootInfo ()
    MbInfo = &MultiBootImage->MbInfo;
    if ((MbInfo->MmapAddr != NULL) && (MbInfo->Flags & MULTIBOOT_INFO_HAS_MMAP)) {
      FreePool (MbInfo->MmapAddr);
      MbInfo->MmapAddr = NULL;
    }
  }

  //
  // Free MultiBoot-2 Image Data
  //
  if (LoadedImage->Flags & LOADED_IMAGE_MULTIBOOT2) {
    MultiBootImage = &LoadedImage->Image.MultiBoot;

    // Free info tags which are allocated in SetupMultiboot2Info ()
    Mb2Info = &MultiBootImage->Mb2Info;
    if (Mb2Info->StartTag != NULL) {
      FreePool (Mb2Info->StartTag);
      Mb2Info->StartTag = NULL;
    }
  }

  FreePool (LoadedImage);
}

/**
  Free all temporary resources used for Boot Image

  This function will clean up all temporary resources used to load Boot Image.

  @param[in]  LoadedImageHandle   Loaded Image handle.
  @param[in]  KeepRootNode        TRUE,  do not free memory for LOADED_IMAGES_INFO root node.
                                  FALSE, free memory for LOADED_IMAGES_INFO root node.
**/
VOID
EFIAPI
UnloadBootImages (
  IN  EFI_HANDLE       LoadedImageHandle,
  IN  BOOLEAN          KeepRootNode
  )
{
  LOADED_IMAGES_INFO        *LoadedImagesInfo;
  LOADED_IMAGE              *LoadedImage;
  UINT8                      Index;

  LoadedImagesInfo = (LOADED_IMAGES_INFO *)LoadedImageHandle;
  if ((LoadedImagesInfo == NULL) || (LoadedImagesInfo->Signature != LOADED_IMAGES_INFO_SIGNATURE)) {
    DEBUG ((DEBUG_INFO, "Invalid LoadedImageHandle parameter!\n"));
    return;
  }

  for (Index = 0; Index < LoadImageTypeMax; Index++) {
    LoadedImage = (LOADED_IMAGE *)LoadedImagesInfo->LoadedImageList[Index];
    UnloadLoadedImage (LoadedImage);
    LoadedImagesInfo->LoadedImageList[Index] = NULL;
  }

  if (!KeepRootNode) {
    FreePool (LoadedImagesInfo);
  }
}

/**
  Load Image from boot media.

  This function will initialize OS boot device, and load image based on
  boot option, the loaded image info will be saved in  LoadedImage.

  @param[in]  BootOption        Current boot option
  @param[in]  HwPartHandle      Hardware partition handle
  @param[in]  FsHandle          FileSystem handle
  @param[out] LoadedImageHandle Loaded Image handle

  @retval     RETURN_SUCCESS    If image was loaded successfully
  @retval     Others            If image was not loaded.
**/
EFI_STATUS
EFIAPI
LoadBootImages (
  IN  OS_BOOT_OPTION  *OsBootOption,
  IN  EFI_HANDLE       HwPartHandle,
  IN  EFI_HANDLE       FsHandle,
  OUT EFI_HANDLE      *LoadedImageHandle
  )
{
  LOADED_IMAGES_INFO        *LoadedImagesInfo;
  LOADED_IMAGE              *LoadedImage;
  BOOT_IMAGE                *BootImage;
  UINT8                      BootFlags;
  EFI_STATUS                 Status;
  UINT8                      Index;
  CONTAINER_IMAGE           *ContainerImage;

  ASSERT (OsBootOption != NULL);

  BootImage = OsBootOption->Image;
  BootFlags = OsBootOption->BootFlags;

  LoadedImagesInfo = (LOADED_IMAGES_INFO *)AllocateZeroPool (sizeof (LOADED_IMAGES_INFO));
  if (LoadedImagesInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  LoadedImagesInfo->Signature = LOADED_IMAGES_INFO_SIGNATURE;

  for (Index = 0; Index < LoadImageTypeMax; Index++) {
    if ((Index == LoadImageTypePreOs) && ((BootFlags & BOOT_FLAGS_PREOS) == 0)) {
      continue;
    }
    if (Index == LoadImageTypeMisc) {
      continue;
    }

    if (Index >= LoadImageTypeExtra0) {
      if (((BootFlags & BOOT_FLAGS_EXTRA) == 0) || (BootImage[Index].LbaImage.Valid == 0)) {
        continue;
      }
    }

    LoadedImage = (LOADED_IMAGE *)AllocateZeroPool (sizeof (LOADED_IMAGE));
    if (LoadedImage == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    LoadedImage->HwPartHandle   = HwPartHandle;
    LoadedImage->LoadImageType  = Index;

    //
    // Load Boot Image from FS or RAW partition
    //
    ContainerImage = &BootImage[Index].ContainerImage;
    if ((ContainerImage->Indicate == '!') && (ContainerImage->BackSlash == '/')) {
      Status = GetBootImageFromIfwiContainer (OsBootOption, LoadedImage);
    } else if (FsHandle != NULL) {
      Status = GetBootImageFromFs (FsHandle, OsBootOption, LoadedImage);
    } else {
      Status = GetBootImageFromRawPartition (OsBootOption, LoadedImage);
    }

    DEBUG ((DEBUG_INFO, "LoadBootImage ImageType-%d %r\n", Index, Status));
    LoadedImagesInfo->LoadedImageList[Index] = LoadedImage;

    if (EFI_ERROR (Status)) {
      if (Index >= LoadImageTypeExtra0) {
        // Continue boot if load extra image failed.
        Status = EFI_SUCCESS;
        continue;
      }

      // UnloadBootImages () will free all unnecessary Memory
      break;
    }
  }

  //
  // Launch Traditional Linux for debugging purpose only
  //
  if (DebugCodeEnabled () || !FeaturePcdGet (PcdVerifiedBootEnabled)) {
    if (EFI_ERROR (Status) && (FsHandle != NULL)) {
      // Free loaded images previously, but keep LoadedImagesInfo structure
      UnloadBootImages ((EFI_HANDLE)(UINTN)LoadedImagesInfo, TRUE);
      LoadedImage = LoadedImagesInfo->LoadedImageList[LoadImageTypeNormal];
      if (LoadedImage == NULL) {
        LoadedImage = (LOADED_IMAGE *)AllocateZeroPool (sizeof (LOADED_IMAGE));
        if (LoadedImage == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
      }
      LoadedImage->HwPartHandle   = HwPartHandle;
      LoadedImage->LoadImageType  = LoadImageTypeNormal;
      LoadedImagesInfo->LoadedImageList[LoadImageTypeNormal] = LoadedImage;
      Status = GetTraditionalLinux (FsHandle, &LoadedImage->Image.Linux);
      if (!EFI_ERROR (Status)) {
        LoadedImage->Flags |= LOADED_IMAGE_LINUX;
        DEBUG ((DEBUG_INFO, "LoadBootImage TraditionalLinux ImageType-%d Image\n", Index));
      }
    }
  }

  *LoadedImageHandle = (EFI_HANDLE)(UINTN)LoadedImagesInfo;

  //
  // Free allocated memory if not successful
  //
  if (EFI_ERROR (Status)) {
    UnloadBootImages (*LoadedImageHandle, FALSE);
    *LoadedImageHandle = NULL;
  }

  return Status;
}
