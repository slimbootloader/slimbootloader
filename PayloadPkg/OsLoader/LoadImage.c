/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "OsLoader.h"

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
  if ((BootOption->DevAddr & 0xFF000000) == 0) {
    BootMediumPciBase = (UINTN) MM_PCI_BASE (                   \
                        ((BootOption->DevAddr >> 16) & 0xFF), \
                        ((BootOption->DevAddr >> 8)  & 0xFF), \
                        (BootOption->DevAddr & 0xFF)          \
                        );
  } else {
    BootMediumPciBase = BootOption->DevAddr;
  }
  DEBUG ((DEBUG_INFO, "BootMediumPciBase(0x%x)\n", BootMediumPciBase));

  //
  // Init Boot device functions
  //
  Status = MediaSetInterfaceType (BootOption->DevType);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Invalid Boot device configured\n"));
    return RETURN_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "Getting boot image from... %a\n", GetBootDeviceNameString(BootOption->DevType)));

  Status = MediaInitialize (BootMediumPciBase, DevInitAll);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  AddMeasurePoint (0x4050);
  MediaTuning (BootMediumPciBase);
  AddMeasurePoint (0x4055);

  DEBUG ((DEBUG_INFO, "Try to find boot partition\n"));
  Status = FindPartitions (BootOption->HwPart, HwPartHandle);
  if (EFI_ERROR (Status)) {
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
  UINT32                     LbaAddr;
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

  DEBUG ((DEBUG_INFO, "Load image from SwPart (0x%x), LbaAddr(0x%x)\n", SwPart, LbaAddr));
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

  if (FsHandle == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  FileName = (CONST CHAR8 *)&BootOption->Image[LoadedImage->LoadImageType].FileName[0];

  // Load IAS Image from file system
  AsciiStrToUnicodeStrS (FileName, FilePath, sizeof (FilePath) / sizeof (CHAR16));
  Image     = NULL;
  ImageSize = 0;
  Status = GetFileByName (FsHandle, FilePath, &Image, &ImageSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Get file '%a' failed, Status = %r\n", FileName, Status));
    return Status;
  }
  DEBUG ((DEBUG_INFO, "Get file '%s' (size:0x%x) success.\n", FilePath, ImageSize));

  if ((Image == NULL) || (ImageSize == 0)) {
    return EFI_LOAD_ERROR;
  }

  LoadedImage->IasImage.Addr = Image;
  LoadedImage->IasImage.Size = ImageSize;
  LoadedImage->Flags        |= LOADED_IMAGE_IAS;


  return EFI_SUCCESS;
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
  VOID                       *Kernel;
  UINTN                      KernelSize;
  UINTN                      InitrdSize;
  VOID                       *InitrdData;
  UINTN                      ConfigFileSize;
  VOID                       *ConfigFile;

  // Todo: use OS verification Hash flag to controll support tradition linux or not.
  DEBUG ((DEBUG_INFO, "Try booting Linux from vmlinuz...\n"));
  KernelSize = 0;
  Kernel     = NULL;
  Status = GetFileByName (FsHandle, L"vmlinuz", &Kernel, &KernelSize);
  DEBUG ((DEBUG_INFO, "load vmlinuz size (0x%x): %r\n", KernelSize, Status));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Not found Linux Images!\n"));
    return RETURN_NOT_FOUND;
  }
  LinuxImage->BootFile.Addr = Kernel;
  LinuxImage->BootFile.Size = KernelSize;

  ConfigFileSize = 0;
  ConfigFile     = NULL;
  Status = GetFileByName (FsHandle, L"config.cfg", &ConfigFile, &ConfigFileSize);
  if (!EFI_ERROR (Status) && (ConfigFileSize > 0)) {
    LinuxImage->CmdFile.Addr = ConfigFile;
    LinuxImage->CmdFile.Size = ConfigFileSize;
  }
  DEBUG ((DEBUG_INFO, "load config.cfg size (0x%x): %r\n", ConfigFileSize, Status));

  InitrdSize = 0;
  InitrdData = NULL;
  Status = GetFileByName (FsHandle, L"initrd", &InitrdData, &InitrdSize);
  if (!EFI_ERROR (Status) && (InitrdSize > 0)) {
    LinuxImage->InitrdFile.Addr = InitrdData;
    LinuxImage->InitrdFile.Size = InitrdSize;
  }
  DEBUG ((DEBUG_INFO, "load initrd size (0x%x): %r\n", InitrdSize, Status));

  LinuxImage->ExtraBlobNumber = 0;
  return EFI_SUCCESS;
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

