/** @file
  This file contains the implementation of FirmwareUpdateLib library.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <PiPei.h>
#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/FileSystemLib.h>
#include <Guid/OsBootOptionGuid.h>
#include <Library/HobLib.h>
#include <Library/MediaAccessLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/FirmwareUpdateLib.h>
#include <Library/CryptoLib.h>

#define MM_PCI_BASE(Bus, Device, Function) \
  ( (UINTN)PcdGet64(PcdPciExpressBaseAddress) + \
    (UINTN)(Bus << 20) +    \
    (UINTN)(Device << 15) + \
    (UINTN)(Function << 12) \
  )

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

  //
  // Get OS boot device address
  //
  BootMediumPciBase = GetDeviceAddr (BootOption->DevType, BootOption->DevInstance);
  BootMediumPciBase = TO_MM_PCI_ADDRESS (BootMediumPciBase);
  DEBUG ((DEBUG_INFO, "BootMediumPciBase(0x%x)\n", BootMediumPciBase));

  //
  // Init Boot device functions
  //
  Status = MediaSetInterfaceType (BootOption->DevType);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Invalid Boot device configured\n"));
    return RETURN_UNSUPPORTED;
  }

  Status = MediaInitialize (BootMediumPciBase, DevInitAll);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "find boot partition\n"));
  Status = FindPartitions (BootOption->HwPart, HwPartHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "Find partion success\n"));
  return RETURN_SUCCESS;
}

/**
  Get Capsule image from raw partition

  Using boot option info, this function will read capsule image from raw
  partition based on hardware partition info saved in LoadedImage.
  After capsule image is loaded into memory, information will be saved
  to LoadedImage.

  @param[in]      HwPartHandle    Partition handler
  @param[out]     FileBuffer      Pointer to the buffer address for the
                                  loaded image.
  @param[out]     FileSize        Pointer to the length of the loaded image.

  @retval  EFI_OUT_OF_RESOURCES   Out of memory resource
  @retval  EFI_NOT_FOUND          Cannot find the capsule image
  @retval  EFI_LOAD_ERROR         Image loading error
  @retval  EFI_SUCCESS            Image was loaded successfully

**/
EFI_STATUS
GetCapsuleFromRawPartition (
  IN    EFI_HANDLE  HwPartHandle,
  OUT   VOID        **FileBuffer,
  OUT   UINTN       *FileSize
  )
{
  EFI_STATUS                Status;
  DEVICE_BLOCK_INFO         BlockInfo;
  VOID                      *Buffer;
  UINTN                     ImageSize;
  LOGICAL_BLOCK_DEVICE      LogicBlkDev;
  UINTN                     AlginedHeaderSize;
  UINTN                     AlginedImageSize;
  UINT32                    BlockSize;
  UINT8                     BlockData[4096];
  FIRMWARE_UPDATE_HEADER    *FwUpdHeader;

  DEBUG ((DEBUG_INFO, "Load image from SwPart (0x%x), LbaAddr(0x%x)\n", 0, 0));
  Status = GetLogicalPartitionInfo (0, HwPartHandle, &LogicBlkDev);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Get logical partition error, Status = %r\n", Status));
    return Status;
  }

  Status = MediaGetMediaInfo (0, &BlockInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "GetMediaInfo Error %r\n", Status));
    return Status;
  }

  //
  // Read the capsule Header first to get total size of the capsule image.
  // Make sure to round the Header size to be block aligned in bytes.
  //
  BlockSize = BlockInfo.BlockSize;
  AlginedHeaderSize = ((sizeof (FIRMWARE_UPDATE_HEADER) % BlockSize) == 0) ? \
                      sizeof (FIRMWARE_UPDATE_HEADER) : \
                      ((sizeof (FIRMWARE_UPDATE_HEADER) / BlockSize) + 1) * BlockSize;
  Status = MediaReadBlocks (
             0,
             LogicBlkDev.StartBlock,
             AlginedHeaderSize,
             BlockData
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Read firmware update header error, Status = %r\n", Status));
    return Status;
  }

  //
  // Firmware update header
  //
  FwUpdHeader = (FIRMWARE_UPDATE_HEADER *) BlockData;

  //
  // Make sure this is the capsule image
  //
  if (!CompareGuid (&gFirmwareUpdateImageFileGuid, &FwUpdHeader->FileGuid)) {
    DEBUG ((DEBUG_INFO, "Invalid Capsule image found, Guid mismatch\n"));
    return EFI_NOT_FOUND;
  }

  if (FwUpdHeader->HeaderSize != sizeof (FIRMWARE_UPDATE_HEADER)) {
    DEBUG ((DEBUG_INFO, "Invalid Capsule image found, Header size mismatch\n"));
    return EFI_NOT_FOUND;
  }

  if (FwUpdHeader->PubKeySize != RSA_MOD_SIZE + RSA_E_SIZE + sizeof (UINT32)) {
    DEBUG ((DEBUG_INFO, "Invalid Capsule image found, Public Key size mismatch\n"));
    return EFI_NOT_FOUND;
  }

  if (FwUpdHeader->SignatureSize != RSA2048NUMBYTES) {
    DEBUG ((DEBUG_INFO, "Invalid Capsule image found, Signature size mismatch\n"));
    return EFI_NOT_FOUND;
  }

  //
  // Make sure to round the image size to be block aligned in bytes.
  //
  ImageSize        = CAPSULE_IMAGE_SIZE ((FIRMWARE_UPDATE_HEADER *) BlockData);
  AlginedImageSize = ((ImageSize % BlockSize) == 0) ? \
                     ImageSize : \
                     ((ImageSize / BlockSize) + 1) * BlockSize;

  Buffer = (UINT8 *) AllocatePages (EFI_SIZE_TO_PAGES (AlginedImageSize));
  if (Buffer == NULL) {
    DEBUG ((DEBUG_INFO, "Allocate memory (size:0x%x) fail.\n", AlginedImageSize));
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Read the capsule image into the buffer
  //
  Status = MediaReadBlocks (
             0,
             LogicBlkDev.StartBlock,
             AlginedImageSize,
             Buffer
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Read capsule image error, Status = %r\n", Status));
    return  Status;
  }

  if ((Buffer == NULL) || (ImageSize == 0)) {
    return EFI_LOAD_ERROR;
  }

  *FileBuffer = Buffer;
  *FileSize = ImageSize;
  return EFI_SUCCESS;
}


/**
  Get capsule image for firmware update.

  This function read firmware update capsule image starting with firmware update
  header. It could be read from EMMC, UFS, USB, SATA, etc. block device. Often the
  Capsule image could be saved in the root directory of a FAT system.

  @param[in]  OsImageInfo      It includes the boot device info for capsule image.
  @param[out] CapsuleImage     The firmware update capsule image.
  @param[out] CapsuleImageSize The capsule image size.

  @retval  EFI_SUCCESS         Get the capsule image successfully.
  @retval  others              Error happening when getting capsule image.
**/
EFI_STATUS
LoadCapsuleImage (
  IN   OS_BOOT_OPTION            *OsImageInfo,
  OUT  VOID                      **CapsuleImage,
  OUT  UINT32                    *CapsuleImageSize
  )
{
  EFI_STATUS                     Status;
  UINTN                          HardwareDeviceBlockIndex;
  DEVICE_BLOCK_INFO              BlockInfo;
  EFI_HANDLE                     FsHandle;
  EFI_HANDLE                     HwPartHandle;

  HwPartHandle = NULL;
  Status = FindBootPartition (OsImageInfo, &HwPartHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // If we do not have file system, try reading capsule from raw partition
  //
  if (OsImageInfo->FsType >= EnumFileSystemMax) {
    Status = GetCapsuleFromRawPartition (HwPartHandle, CapsuleImage, CapsuleImageSize);
    return Status;
  }

  DEBUG ((DEBUG_ERROR, "Find partition\n"));
  HardwareDeviceBlockIndex = OsImageInfo->HwPart;
  Status = MediaGetMediaInfo (HardwareDeviceBlockIndex, &BlockInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "GetInfo Error %r\n", Status));
    return Status;
  }

  FsHandle = NULL;
  Status = InitFileSystem (OsImageInfo->SwPart, EnumFileSystemTypeFat, HwPartHandle, &FsHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "No partitions found, Status = %r\n", Status));
    goto Done;
  }

  //
  // get capsule image size.
  //
  *CapsuleImageSize = 0x1000000;
  *CapsuleImage     = NULL;
  // get the capsule image
  Status = GetFileByName (FsHandle, L"FwuImage.bin", CapsuleImage, CapsuleImageSize);

Done:
  if (EFI_ERROR (Status)) {
    if (FsHandle != NULL) {
      FreePool (FsHandle);
    }
    if (HwPartHandle != NULL) {
      FreePool (HwPartHandle);
    }
  }

  return Status;
}


/**
  Get OS boot option list.

  This function will get OS boot options from HOB, and return boot list
  pointer that point to HOB data.

  @retval Boot option list   Return OS boot option list.
  @retval NULL               Return NULL if there is error.
**/
OS_BOOT_OPTION_LIST *
GetOsImageList (
  VOID
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;

  GuidHob = GetNextGuidHob (&gOsBootOptionGuid, GetHobListPtr());
  if (GuidHob == NULL) {
    ASSERT (GuidHob);
    return NULL;
  }
  return (OS_BOOT_OPTION_LIST *)GET_GUID_HOB_DATA (GuidHob);
}


/**
  Get capsule image for firmware update.

  This function read firmware update capsule image starting with firmware update
  header. It could be read from EMMC, UFS, USB, SATA, etc. block device. Often the
  Capsule image could be saved in the root directory of a FAT system.

  @param[out] FwBuffer        The firmware update capsule image.
  @param[out] FwSize          The capsule image size.

  @retval  EFI_SUCCESS        Get the capsule image successfully.
  @retval  others             Error happening when getting capsule image.
**/
EFI_STATUS
EFIAPI
GetCapsuleImage (
  OUT  VOID                      **FwBuffer,
  OUT  UINT32                    *FwSize
  )
{
  EFI_STATUS                     Status;
  VOID                           *CapsuleImage;
  UINTN                          CapsuleImageSize;
  OS_BOOT_OPTION_LIST            *OsImageInfoList;
  UINT32                         Index;

  Status = EFI_UNSUPPORTED;

  DEBUG ((DEBUG_INFO, "\n=================Read Capsule Image==============\n"));

  Status = PlatformGetCapsuleImage (&CapsuleImage, &CapsuleImageSize);
  DEBUG((DEBUG_ERROR, " PlatformGetCapsuleImage: Status : %r\n", Status));
  if ((Status == EFI_SUCCESS) || (Status == EFI_NOT_FOUND)) {
    *FwSize   = CapsuleImageSize;
    *FwBuffer = CapsuleImage;
    return Status;
  }

  //
  // Get Boot Image Info
  //
  OsImageInfoList = GetOsImageList ();
  if (OsImageInfoList == NULL) {
    DEBUG ((DEBUG_ERROR, "Could not obtain OS image list\n"));
    return EFI_NOT_FOUND;
  }

  //
  // Load and run Image in order from OsImageInfoList
  //
  for (Index = 0; Index < OsImageInfoList->OsBootOptionCount; Index++) {
    Status = LoadCapsuleImage (&OsImageInfoList->OsBootOption[Index], &CapsuleImage, &CapsuleImageSize);
    if (!EFI_ERROR (Status)) {
      break;
    }
  }

  if (Index == OsImageInfoList->OsBootOptionCount) {
    DEBUG ((DEBUG_ERROR, "Get firmware update capsule image failure!\n"));
    return Status;
  }

  *FwSize   = CapsuleImageSize;
  *FwBuffer = CapsuleImage;
  DEBUG ((DEBUG_INFO, "Capsule Image found, ImageSize=0x%x\n", *FwSize));
  DEBUG ((DEBUG_INFO, "First 256Bytes of capsule image\n"));
  DumpHex (2, 0, 256, (VOID *)CapsuleImage);

  return Status;
}

