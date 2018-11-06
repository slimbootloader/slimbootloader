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

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootMediaLib.h>
#include <Library/BpdtLib.h>
#include <Library/SpiFlashLib.h>  //todo: should not let this lib depends on specific spi lib.
#include <Library/FlashDescriptorLib.h>
#include <Library/IoLib.h>
#include <Library/FatLib.h>
#include <Library/HeciLib.h>
#include <Library/PcdLib.h>
#include <FwuBootMode.h>
#include <Library/FirmwareUpdateLib.h>
#include <Library/MediaAccessLib.h>
#include <Library/PartitionLib.h>
#include <Library/CryptoLib.h>
#include <Library/ConfigDataLib.h>
#include <ConfigDataDefs.h>
#include <Library/FileSystemLib.h>
#include <Library/PayloadLib.h>
#include <Library/ConfigDataLib.h>
#include <Service/PlatformService.h>
#include <Service/HeciService.h>

#define FLASH_MAP_IN_FV_OFFSET   0xA4

SPI_FLASH_SERVICE   *mFwuSpiService = NULL;


/**
  This function initialized boot media.

  It initializes SPI services and SPI Flash size information.

**/
VOID
EFIAPI
InitializeBootMedia (
  VOID
  )
{
  mFwuSpiService = (SPI_FLASH_SERVICE *)GetServiceBySignature (SPI_FLASH_SERVICE_SIGNATURE);
  if (mFwuSpiService == NULL) {
    return;
  }

  mFwuSpiService->SpiInit ();
}

/**
  This function reads blocks from the SPI device.

  @param[in]  Address             The block address in the FlashRegionAll to read from on the SPI.
  @param[in]  ByteCount           Size of the Buffer in bytes.
  @param[out] Buffer              Pointer to caller-allocated buffer containing the data received during the SPI cycle.

  @retval EFI_SUCCESS             Read completes successfully.
  @retval others                  Device error, the command aborts abnormally.

**/
EFI_STATUS
EFIAPI
BootMediaRead (
  IN     UINT64                  Address,
  IN     UINT32                  ByteCount,
  OUT    UINT8                   *Buffer
  )
{
  return mFwuSpiService->SpiRead (FlashRegionBios, (UINT32)Address, ByteCount, Buffer);
}

/**
  This function writes blocks from the SPI device.

  @param[in]   Address            The block address in the FlashRegionAll to read from on the SPI.
  @param[in]   ByteCount          Size of the Buffer in bytes.
  @param[out]  Buffer             Pointer to the data to write.

  @retval EFI_SUCCESS             Write completes successfully.
  @retval others                  Device error, the command aborts abnormally.

**/
EFI_STATUS
EFIAPI
BootMediaWrite (
  IN     UINT64                  Address,
  IN     UINT32                  ByteCount,
  OUT    UINT8                   *Buffer
  )
{
  return mFwuSpiService->SpiWrite (FlashRegionBios, (UINT32)Address, ByteCount, Buffer);
}

/**
  This function erases blocks from the SPI device.

  @param[in]  Address             The block address in the FlashRegionAll to read from on the SPI.
  @param[in]  ByteCount           Size of the region to erase in bytes.

  @retval EFI_SUCCESS             Erase completes successfully.
  @retval others                  Device error, the command aborts abnormally.

**/
EFI_STATUS
EFIAPI
BootMediaErase (
  IN     UINT64                  Address,
  IN     UINT32                  ByteCount
  )
{
  return mFwuSpiService->SpiErase (FlashRegionBios, (UINT32)Address, ByteCount);
}

/**
  Mark the boot partition Green or Red.

  When BDPT header is marked Green, CSE considers it to be a valid partition
  and try to boot from that partition. If the header is marked RED, CSE considers
  that the partition is corrupted or compromised and does not boot from that partition

  This function updates BPDT to a Green or Red state.

  @param[in]  SourceAddress    Pointer to source address to be used for update.
  @param[in]  HeaderState      Value to set to the BPDT header signature.
  @param[out] HeaderSize       The BPDT table size.

  @retval  EFI_SUCCESS         Set the data successfully.
  @retval  others              Error happening when setting the data.
**/
STATIC
EFI_STATUS
MarkBootParitionBpdt (
  IN  UINT8     *SourceAddress,
  IN  UINT32    HeaderState,
  OUT UINT32    *HeaderSize OPTIONAL
  )
{
  BPDT_HEADER     *BpdtHeader;

  if (SourceAddress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BpdtHeader = (BPDT_HEADER *)SourceAddress;
  if (((BpdtHeader->Signature != BPDT_SIGN_GREEN) && (BpdtHeader->Signature != BPDT_SIGN_RED)) || (BpdtHeader->DscCount > MAX_PARTITION_NUM)) {
    DEBUG ((EFI_D_ERROR, "BPDT header Signature or partition number verification failed.\n"));
    return EFI_UNSUPPORTED;
  }

  BpdtHeader->Signature = HeaderState;
  if (HeaderSize != 0) {
    *HeaderSize           = BPDT_SIZE;
  }
  return EFI_SUCCESS;
}

/**
  Get hardware partition handle from boot option info

  This function will initialize boot device, and get hardware partition
  handle based on boot option.

  @param[in]  UserCfgData     Pointer to user configuration data
  @param[out] HwPartHandle    Hardware partition handle for boot image

  @retval  RETURN_SUCCESS     If partition was found successfully
  @retval  Others             If partition was not found
**/
EFI_STATUS
AplFindBootPartition (
  IN  FW_UPD_USER_CFG_DATA   *UserCfgData,
  OUT EFI_HANDLE             *HwPartHandle
  )
{
  RETURN_STATUS   Status;
  UINTN           BootMediumPciBase;


  //
  // Get device address for capsule image
  //
  BootMediumPciBase = GetDeviceAddr (UserCfgData->DevType, UserCfgData->DevInstance);
  BootMediumPciBase = TO_MM_PCI_ADDRESS (BootMediumPciBase);
  DEBUG ((DEBUG_INFO, "BootMediumPciBase(0x%x)\n", BootMediumPciBase));

  //
  // Init Boot device functions
  //
  Status = MediaSetInterfaceType (UserCfgData->DevType);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Invalid Boot device configured\n"));
    return RETURN_UNSUPPORTED;
  }

  Status = MediaInitialize (BootMediumPciBase, DevInitAll);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "find boot partition\n"));
  Status = FindPartitions (UserCfgData->HwPart, HwPartHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "Find partion success\n"));
  return RETURN_SUCCESS;
}

/**
  Get capsule image from raw partition

  Using boot option info, this function will read capsule image from raw
  partition based on hardware partition info saved in LoadedImage.
  After capsule image is loaded into memory, its information will be saved
  to LoadedImage.

  @param[in]  UserCfgData      Pointer to user configuration data
  @param[in]  HwPartHandle     hardware partition handle
  @param[out] CapsuleImage     The firmware update capsule image.
  @param[out] CapsuleImageSize The capsule image size.

  @retval  RETURN_SUCCESS     If capsule image was loaded successfully
  @retval  Others             If capsule image was not loaded.
**/
EFI_STATUS
GetCapsuleImageFromRawPartition (
  IN  FW_UPD_USER_CFG_DATA   *UserCfgData,
  IN  EFI_HANDLE             HwPartHandle,
  OUT VOID                   **CapsuleImage,
  OUT UINT32                 *CapsuleImageSize
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
  FIRMWARE_UPDATE_HEADER    *FwUpdHeader;

  SwPart  = UserCfgData->SwPart;
  LbaAddr = (UINT32)UserCfgData->LbaAddr;

  DEBUG ((DEBUG_INFO, "Load image from SwPart (0x%x), LbaAddr(0x%x)\n", SwPart, LbaAddr));
  Status = GetLogicalPartitionInfo (SwPart, HwPartHandle, &LogicBlkDev);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Get logical partition error, Status = %r\n", Status));
    return Status;
  }

  Status = MediaGetMediaInfo (UserCfgData->HwPart, &BlockInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "GetMediaInfo Error %r\n", Status));
    return Status;
  }

  //
  // Read the firmware update capsule Header first to get total size of the capsule image.
  // Make sure to round the Header size to be block aligned in bytes.
  //
  BlockSize = BlockInfo.BlockSize;
  AlginedHeaderSize = ((sizeof (FIRMWARE_UPDATE_HEADER) % BlockSize) == 0) ? \
                      sizeof (FIRMWARE_UPDATE_HEADER) : \
                      ((sizeof (FIRMWARE_UPDATE_HEADER) / BlockSize) + 1) * BlockSize;
  Status = MediaReadBlocks (
             UserCfgData->HwPart,
             LogicBlkDev.StartBlock + LbaAddr,
             AlginedHeaderSize,
             BlockData
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Read capsule image error, Status = %r\n", Status));
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
  // Read the rest of the capsule image into the buffer
  //
  Status = MediaReadBlocks (
             UserCfgData->HwPart,
             LogicBlkDev.StartBlock + LbaAddr,
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

  *CapsuleImage = Buffer;
  *CapsuleImageSize = ImageSize;

  return EFI_SUCCESS;
}

/**
  Platform code to get capsule image for firmware update.

  This function is platform hook to implement specific way to detecting capsule
  firmware update

  @param[out] CapsuleImage     The firmware update capsule image.
  @param[out] CapsuleImageSize The capsule image size.

  @retval  EFI_SUCCESS         Get the capsule image successfully.
  @retval  others              Error happening when getting capsule image.
**/
EFI_STATUS
PlatformGetCapsuleImage (
  OUT  VOID       **CapsuleImage,
  OUT  UINT32     *CapsuleImageSize
  )
{
  EFI_STATUS               Status;
  FW_UPD_USER_CFG_DATA     *UserCfgData;
  UINTN                    HardwareDeviceBlockIndex;
  DEVICE_BLOCK_INFO        BlockInfo;
  EFI_HANDLE               FsHandle;
  EFI_HANDLE               HwPartHandle;
  CHAR16                   FileName[MAX_FILE_LEN];

  UserCfgData = NULL;

  //
  // Get capsule configuration data
  //
  UserCfgData = (FW_UPD_USER_CFG_DATA *) FindConfigDataByTag (CDATA_CAPSULE_TAG);

  //
  // If we do not find capsule information, return error
  //
  if (UserCfgData == NULL) {
    DEBUG((DEBUG_ERROR, " UserCfgData not found \n"));
    return EFI_NOT_FOUND;
  }

  //
  // Find boot partition using capsule configuration
  //
  HwPartHandle = NULL;
  Status = AplFindBootPartition (UserCfgData, &HwPartHandle);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, " AplFindBootPartition: Status : %r\n", Status));
    return EFI_NOT_FOUND;
  }

  //
  // If we do not have file system, try reading capsule from raw partition
  //
  if (UserCfgData->FsType >= EnumFileSystemMax) {
    Status = GetCapsuleImageFromRawPartition (UserCfgData, HwPartHandle, CapsuleImage, CapsuleImageSize);
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, " GetCapsuleImageFromRawPartition: Status : %r\n", Status));
      return EFI_NOT_FOUND;
    }
    return Status;
  }

  HardwareDeviceBlockIndex = UserCfgData->HwPart;
  Status = MediaGetMediaInfo (HardwareDeviceBlockIndex, &BlockInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "GetInfo Error %r\n", Status));
    return EFI_NOT_FOUND;
  }

  FsHandle = NULL;
  Status = InitFileSystem (UserCfgData->SwPart, EnumFileSystemTypeFat, HwPartHandle, &FsHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "No partitions found, Status = %r\n", Status));
    goto Done;
  }

  //
  // get capsule image size.
  //
  *CapsuleImageSize = 0x1000000;
  *CapsuleImage     = NULL;

  //
  // Find capsule using the name provided in configuration data
  //
  if (UserCfgData->FileName[0] != 0) {
    AsciiStrToUnicodeStr((CONST CHAR8 *)(&UserCfgData->FileName), FileName);
    Status = GetFileByName(FsHandle, FileName, CapsuleImage, CapsuleImageSize);
    DEBUG((DEBUG_ERROR, " GetFileByName: Status : %r\n", Status));
  } else {
    Status = EFI_NOT_FOUND;
  }

Done:
  if (EFI_ERROR (Status)) {
    if (FsHandle != NULL) {
      FreePool (FsHandle);
    }
    if (HwPartHandle != NULL) {
      FreePool (HwPartHandle);
    }
  }

  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  Get offset of Stage 1A in the capsule image

  This function will get stage 1A base address from flash map.
  Computes offset in the BIOS region from the base address.
  Then it calculates base address of stage1A in the capsule image.

  @param[in]  FwImage         The firmware update capsule image.
  @param[in]  IsBackupPartition TRUE for Back up copy, FALSE for primary copy
  @param[out] Base            Base address of the component
  @param[out] Size            Size of the component

  @retval  EFI_SUCCESS        Image offset returned successfully.
  @retval  EFI_NOT_FOUND      Could not get component information from flash map.
  @retval  others             Error happening when updating.
**/
EFI_STATUS
EFIAPI
PlatformGetStage1AOffset (
  IN  UINT8      *FwImage,
  IN  BOOLEAN    IsBackupPartition,
  OUT UINT32     *Base,
  OUT UINT32     *Size
  )
{
  EFI_STATUS                   Status;
  FIRMWARE_UPDATE_HEADER      *FwUpdHeader;
  UINT32                       Offset;
  UINT8                       *Ptr;
  UINT32                       Index;
  UINT32                       RgnBase;
  UINT32                       RgnSize;
  UINT32                       BpOffset;
  UINT32                       MaxEntries;
  FLASH_MAP                   *FlashMapPtr;
  FLASH_MAP_ENTRY_DESC         EntryDesc;

  if ((Base == NULL) || (Size == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = mFwuSpiService->SpiGetRegion (FlashRegionBios, &RgnBase, &RgnSize);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_AVAILABLE_YET;
  }

  // Search for Flash Map signature in image
  FlashMapPtr = NULL;
  FwUpdHeader = (FIRMWARE_UPDATE_HEADER *)FwImage;
  Ptr         = (UINT8 *)(FwImage + FwUpdHeader->ImageOffset);
  BpOffset    = IsBackupPartition ? (RgnSize >> 1) : 0;
  for (Offset = BpOffset; Offset < FwUpdHeader->ImageSize; Offset += SIZE_4KB) {
     if (*(UINT32 *)(Ptr + Offset + FLASH_MAP_IN_FV_OFFSET) == FLASH_MAP_SIG_HEADER) {
        // This is flash map
        FlashMapPtr = (FLASH_MAP *)(Ptr + Offset + FLASH_MAP_IN_FV_OFFSET);
        break;
     }
  }

  if (FlashMapPtr != NULL) {
    // Search for Stage1A in flash map
    MaxEntries = ((FlashMapPtr->Length - FLASH_MAP_HEADER_SIZE) / sizeof (FLASH_MAP_ENTRY_DESC));
    for (Index = 0; Index < MaxEntries; Index++) {
      EntryDesc = FlashMapPtr->EntryDesc[Index];
      if (EntryDesc.Signature == FLASH_MAP_SIG_STAGE1A) {
        *Base  = (UINT32)(Ptr + EntryDesc.Offset);
        *Size  = EntryDesc.Size;
        Status = EFI_SUCCESS;
        break;
      }
    }
  }

  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Could not find stage1A information from image!\n"));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Get details information on how to update a boot partition.

  Platform knows the capsule image and the boot device layout. By parsing capsule
  Image and boot media, the platform could produce a list of regions to be updated.

  Platform could set partition flag to decide if a reboot is required after a boot
  partition is updated.
  Platform could set region flag to indicate if the source image is the final image
  to write to boot media. If the flag is set, that source will be used to check if
  the source is same before doing firmware update.

  @param[in]  FwImage         The firmware update capsule image.
  @param[in]  FwPolicy        Firmware update policy.
  @param[out] PartitionInfo   The detail informaion on the partition to update

  @retval  EFI_SUCCESS        Update successfully.
  @retval  others             Error happening when updating.
**/
EFI_STATUS
EFIAPI
GetFirmwareUpdateInfo (
  IN  UINT8                      *FwImage,
  IN  FIRMWARE_UPDATE_POLICY     FwPolicy,
  OUT FIRMWARE_UPDATE_PARTITION  **PartitionInfo
  )
{
  EFI_STATUS                     Status;
  FIRMWARE_UPDATE_HEADER         *Header;
  FIRMWARE_UPDATE_PARTITION      *UpdatePartition;
  UINT32                         AllocateSize;
  UINT32                         ToUpdateAddress;
  FIRMWARE_UPDATE_REGION         *UpdateRegion;
  UINT8                          *BiosAddress;
  UINT32                         RgnBase;
  UINT32                         RgnSize;
  UINT32                         RsvdRgnBase;
  UINT32                         RsvdRgnSize;

  ToUpdateAddress = 0;
  BiosAddress     = NULL;

  AllocateSize    = sizeof (FIRMWARE_UPDATE_PARTITION) + 2 * sizeof (FIRMWARE_UPDATE_REGION);
  UpdatePartition = (FIRMWARE_UPDATE_PARTITION *) AllocatePool (AllocateSize);
  ASSERT (UpdatePartition != NULL);

  UpdatePartition->RegionCount   = 2;

  Status = mFwuSpiService->SpiGetRegion (FlashRegionBios, &RgnBase, &RgnSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "GetPartitionSize Status = 0x%x\n", Status));
    return Status;
  }

  Header = (FIRMWARE_UPDATE_HEADER *)FwImage;
  if (FwPolicy.Fields.UpdatePartitionA == 1) {
    ToUpdateAddress = 0;
  } else {
    ToUpdateAddress = RgnSize >> 1;
  }
  BiosAddress  = (UINT8 *)(FwImage + Header->ImageOffset + ToUpdateAddress);

  //
  // Mark BPDT header to RED, so if update fails in between, we will have a corrupted partition
  //
  Status = MarkBootParitionBpdt (BiosAddress, BPDT_SIGN_RED, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SetBootPartitionInvalid failed! Status = 0x%x\n", Status));
    return Status;
  }

  //
  // 2. Prepare boot partition data.
  //
  UpdateRegion                  = &UpdatePartition->FwRegion[0];
  UpdateRegion->ToUpdateAddress = ToUpdateAddress;
  UpdateRegion->UpdateSize      = RgnSize >> 1;
  if (FwPolicy.Fields.UpdatePartitionB == 1) {
    Status = GetComponentInfoByPartition (FLASH_MAP_SIG_BLRESERVED, FALSE, &RsvdRgnBase, &RsvdRgnSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Could not get reserved region base from flash map\n"));
      RsvdRgnSize = 0;
    }
    UpdateRegion->UpdateSize      = (RgnSize >> 1) - RsvdRgnSize;
  }
  UpdateRegion->SourceAddress   = BiosAddress;

  //
  // Mark the BPDT header back to green
  //
  Status = MarkBootParitionBpdt (BiosAddress, BPDT_SIGN_GREEN, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SetBootPartitionInvalid failed! Status = 0x%x\n", Status));
    return Status;
  }

  //
  // 3. Prepare boot partition header with a valid marker.
  //
  UpdateRegion                  = &UpdatePartition->FwRegion[1];
  UpdateRegion->ToUpdateAddress = ToUpdateAddress;
  UpdateRegion->UpdateSize      = 0x4;
  UpdateRegion->SourceAddress   = BiosAddress;

  *PartitionInfo = UpdatePartition;

  PrepareCseForFirmwareUpdate();

  return EFI_SUCCESS;
}

/**
  Switch between the boot partitions.

  This function will use platform specific method of switching
  between primary and backup partitions.

  @param[in] Partition        Partition to select

  @retval  EFI_SUCCESS        Switched to desired partition successfully.
  @retval  others             Error happening.
**/
EFI_STATUS
SetBootPartition (
  IN BOOT_PARTITION  Partition
  )
{
  EFI_STATUS    Status;

  if (Partition == PrimaryPartition) {
    //
    // For APL platform, CSE always boot from Primary partition unless it is corrupt
    // hence, we do not need to do anything here
    //
    return EFI_SUCCESS;
  }

  //
  // Flip to boot partition 2
  // CSE will always try to boot from BP1, if BP1 is corrupt, it will boot from BP2
  //
  Status = BootMediaErase (0, BPDT_SIZE);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Could not flip boot partition\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Platform hook point before starting to write a boot partition.

  This function will do some platform specific things before starting to write
  data to boot media. e.g. Send other component message it is going to do boot
  media update.

  @param[in]  UpdatePartition   Pointer to update partition structure.

  @retval  EFI_SUCCESS        Update successfully.

**/
EFI_STATUS
EFIAPI
PrepareRegionsUpdate (
  IN FIRMWARE_UPDATE_PARTITION   *UpdatePartition
  )
{
  //
  // 1 Data clear/Data Clear lock
  // 2 Send prepare for update msg
  return EFI_SUCCESS;
}

/**
  Platform hook point after firmware update is done.

  This function will do some platform specific things after all new firmware
  is written to boot media. e.g. set boot mode to normal boot mode if platform
  support it.

  @retval  EFI_SUCCESS        Update successfully.
  @retval  others             Error happening end firmware update.

**/
EFI_STATUS
EFIAPI
EndFirmwareUpdate (
  VOID
  )
{
  EFI_STATUS        Status;
  HECI_SERVICE      *HeciService;
  PLATFORM_SERVICE  *PlatformService;

  DEBUG ((EFI_D_INFO, "Firmware update Done! clear CSE flag to normal boot mode.\n"));

  HeciService = (HECI_SERVICE *) GetServiceBySignature (HECI_SERVICE_SIGNATURE);
  if ((HeciService == NULL) || (HeciService->SimpleHeciCommand == NULL)) {
    DEBUG ((EFI_D_INFO, "Heci service unable, Could not exit firmware update mode.\n"));
    return EFI_UNSUPPORTED;
  }

  //
  // Send HECI user command to clear IBB signal data
  //
  Status = HeciService->HeciUserCommand(NULL, 0, 1);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " HeciSendUserCommand: Status : %r\n", Status));
    return Status;
  }

  //
  // Get capsule configuration data using platform service
  //
  PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
  if (PlatformService != NULL && PlatformService->ResetSystem != NULL) {
    DEBUG ((EFI_D_INIT, "Firmware Update Done. Resetting the system....\n"));
    PlatformService->ResetSystem(EfiResetCold);
  }

  return EFI_SUCCESS;
}


