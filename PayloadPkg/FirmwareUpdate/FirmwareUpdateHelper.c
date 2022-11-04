/** @file
  Internal functions to update firmware in boot media.

  Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/PayloadMemoryAllocationLib.h>
#include <Library/FirmwareUpdateLib.h>
#include <Library/ContainerLib.h>
#include <Library/DecompressLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/LiteFvLib.h>
#include <Library/ConsoleOutLib.h>
#include <Library/TimerLib.h>
#include "FirmwareUpdateHelper.h"
#include <Service/SpiFlashService.h>
#include <Library/BootGuardLib.h>

SPI_FLASH_SERVICE   *mFwuSpiService = NULL;

/**
  This function initialized boot media.

  It initializes SPI services and SPI Flash size information.

**/
VOID
EFIAPI
InitializeBootMedia(
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
  Get the SPI region base and size, based on the enum type

  @param[in] FlashRegionType      The Flash Region type for for the base address which is listed in the Descriptor.
  @param[out] BaseAddress         The Flash Linear Address for the Region 'n' Base
  @param[out] RegionSize          The size for the Region 'n'

  @retval EFI_SUCCESS             Read success
  @retval EFI_INVALID_PARAMETER   Invalid region type given
  @retval EFI_DEVICE_ERROR        The region is not used
**/
EFI_STATUS
EFIAPI
BootMediaGetRegion (
  IN     FLASH_REGION_TYPE  FlashRegionType,
  OUT    UINT32             *BaseAddress, OPTIONAL
  OUT    UINT32             *RegionSize OPTIONAL
  )
{
  return mFwuSpiService->SpiGetRegion (FlashRegionType, BaseAddress, RegionSize);
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
  IN     UINT64   Address,
  IN     UINT32   ByteCount,
  OUT    UINT8    *Buffer
  )
{
  return mFwuSpiService->SpiRead (FlashRegionBios, (UINT32)Address, ByteCount, Buffer);
}

/**
  This function reads blocks from the SPI device.

  @param[in] FlashRegionType      The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in]  Address             The block address in the FlashRegionAll to read from on the SPI.
  @param[in]  ByteCount           Size of the Buffer in bytes.
  @param[out] Buffer              Pointer to caller-allocated buffer containing the data received during the SPI cycle.

  @retval EFI_SUCCESS             Read completes successfully.
  @retval others                  Device error, the command aborts abnormally.

**/
EFI_STATUS
EFIAPI
BootMediaReadByType (
  IN     FLASH_REGION_TYPE  FlashRegionType,
  IN     UINT64             Address,
  IN     UINT32             ByteCount,
  OUT    UINT8              *Buffer
  )
{
  return mFwuSpiService->SpiRead (FlashRegionType, (UINT32)Address, ByteCount, Buffer);
}

/**
  This function writes blocks to the SPI device.

  @param[in]   Address            The block address in the FlashRegionAll to read from on the SPI.
  @param[in]   ByteCount          Size of the Buffer in bytes.
  @param[out]  Buffer             Pointer to the data to write.

  @retval EFI_SUCCESS             Write completes successfully.
  @retval others                  Device error, the command aborts abnormally.

**/
EFI_STATUS
EFIAPI
BootMediaWrite (
  IN     UINT64   Address,
  IN     UINT32   ByteCount,
  OUT    UINT8    *Buffer
  )
{
  return mFwuSpiService->SpiWrite (FlashRegionBios, (UINT32)Address, ByteCount, Buffer);
}

/**
  This function writes blocks to the SPI device based on flash region type.

  @param[in] FlashRegionType      The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in]  Address             The block address in the FlashRegionAll to read from on the SPI.
  @param[in]  ByteCount           Size of the Buffer in bytes.
  @param[out] Buffer              Pointer to caller-allocated buffer containing the data received during the SPI cycle.

  @retval EFI_SUCCESS             Write completes successfully.
  @retval others                  Device error, the command aborts abnormally.

**/
EFI_STATUS
EFIAPI
BootMediaWriteByType (
  IN     FLASH_REGION_TYPE  FlashRegionType,
  IN     UINT64             Address,
  IN     UINT32             ByteCount,
  OUT    UINT8              *Buffer
  )
{
  return mFwuSpiService->SpiWrite (FlashRegionType, (UINT32)Address, ByteCount, Buffer);
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
  IN     UINT64   Address,
  IN     UINT32   ByteCount
  )
{
  return mFwuSpiService->SpiErase (FlashRegionBios, (UINT32)Address, ByteCount);
}

/**
  Get SVN from existing firmware

  This routine get SPI base address and read first four bytes
  to get STAGE1A FV base address, using this base address
  this routine can calculate the offset to the SVN structure.

  @param[in]  Stage1AFvPointer    Pointer to Stage1A fv base.
  @param[out] BlVersion           Pointer to SBL version struct.

  @retval EFI_SUCCESS             Read SVN success
  @retval EFI_INVALID_PARAMETER   Invalid parameter
**/
EFI_STATUS
EFIAPI
GetSvn (
  IN  UINT32                Stage1AFvPointer,
  OUT BOOT_LOADER_VERSION   **BlVersion
  )
{
  UINT32                Stage1AFvBase;
  UINT32                TopSwapRegionSize;
  EFI_STATUS            Status;

  //
  // When updating BP1 - read version from backup partition
  // When updatint BP0 - since top swap bit is set, we need
  //                     to substract the top swap region size
  //                     because when top swap bit is set
  //                     address lines will be inverted and point
  //                     to stage 1A from primary partition.
  //
  Status = GetRegionInfo (&TopSwapRegionSize, NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Error getting top swap region size, failed with status: %r\n", Status));
    return Status;
  }
  Stage1AFvPointer = Stage1AFvPointer - TopSwapRegionSize;

  Stage1AFvBase = (UINT32)(*(UINT32 *)(UINTN)Stage1AFvPointer);
  Stage1AFvBase = Stage1AFvPointer - (~Stage1AFvBase + 1) + sizeof(UINT32);

  Status = GetVersionfromFv (Stage1AFvBase, FALSE, BlVersion);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Getting firmware version failed with status: %r\n", Status));
    return Status;
  }

  return Status;
}

/**
  Verify the firmware version to make sure it is no less than current firmware version.

  @param[in]  Stage1ABase   Stage 1A base address.
  @param[in]  IsFd          Does Stage1ABase point to Stage1A FD
                            or SBL Stage1A FV ?
  @param[out] Version       Pointer to version of the firmware

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
GetVersionfromFv (
  IN  UINT32              Stage1ABase,
  IN  BOOLEAN             IsFd,
  OUT BOOT_LOADER_VERSION **Version
  )
{
  EFI_STATUS                  Status;
  EFI_FFS_FILE_HEADER         *FfsFile;
  EFI_FIRMWARE_VOLUME_HEADER  *FvHeader;

  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)Stage1ABase;
  //
  // Stage 1A FD has FSPT FV first, so move on to the next FV
  //
  if (IsFd) {
    FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)((UINTN)FvHeader + (UINTN)FvHeader->FvLength);
  }

  //
  // Get version info FFS from FV
  //
  Status = GetFfsFileByName (FvHeader, &gBootLoaderVersionFileGuid, &FfsFile);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "GetFfsFileByName: %r\n", Status));
    return Status;
  }

  //
  // Raw section in version info FFS has version information
  //
  Status = GetSectionByType (FfsFile, EFI_SECTION_RAW, 0, (VOID *)Version);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "GetSectionByType: %r\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Update a region block.

  This is the acture function to update boot meia. It will erase boot device,
  write new data to boot device, and verify the written data.

  @param[in] Address          The boot media address to be update.
  @param[in] Buffer           The source buffer to write to the boot media.
  @param[in] Length           The length of data to write to boot media.

  @retval  EFI_SUCCESS        Update successfully.
  @retval  others             Error happening when updating.
**/
EFI_STATUS
EFIAPI
UpdateRegionBlock (
  IN  UINT64    Address,
  IN  VOID      *Buffer,
  IN  UINT32    Length
  )
{
  EFI_STATUS    Status;
  UINT8         *ReadBuffer;
  UINT32        Count;
  UINT32        BlockLen;
  UINT8         *Src;

  ReadBuffer = NULL;

  if (Length == 0) {
    return EFI_SUCCESS;
  }

  ReadBuffer = AllocatePages (EFI_SIZE_TO_PAGES (SIZE_4KB));
  if (ReadBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  BlockLen   = SIZE_4KB;
  Src = (UINT8 *)Buffer;

  //
  // Read, compare, erase, write, read, compare
  //
  for (Count = 0; Count < Length; Count += BlockLen) {
    //
    // If updating region less than 4K bytes,
    // adjust the block length to size remaining, i.e less than 4k
    //
    if (Count + BlockLen > Length) {
      BlockLen = Length - Count;
    }
    Status = BootMediaRead(Address + Count, BlockLen, ReadBuffer);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "BootMediaRead.  readaddr: 0x%llx, Status = 0x%x\n", Address + Count, Status));
      goto End;
    }

    if (CompareMem (Src + Count, ReadBuffer, BlockLen) == 0) {
      DEBUG ((DEBUG_INIT, "."));
      continue;
    }

    //
    // Erase the boot media
    // Block length for erase is always 4K bytes
    //
    DEBUG ((DEBUG_INIT, "x"));
    Status = BootMediaErase ((UINT32) (Address + Count),  SIZE_4KB);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "ERROR: in BootMediaErase. Status = 0x%x\n", Status));
      goto End;
    }

    //
    // Write to the boot media
    //
    Status = BootMediaWrite ((UINT32) (Address + Count),  BlockLen, Src + Count);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "ERROR: in BootDeviceWrite. Status = 0x%x\n", Status));
      goto End;
    }

    //
    // Verify the written data
    //
    Status = BootMediaRead (Address + Count, BlockLen, ReadBuffer);
    if (CompareMem (Src + Count, ReadBuffer, BlockLen) != 0) {
      DEBUG ((DEBUG_ERROR, "Verify Error !\n"));
      Status = EFI_DEVICE_ERROR;
      goto End;
    }
  }

End:
  if (ReadBuffer != NULL) {
    FreePages (ReadBuffer, EFI_SIZE_TO_PAGES (SIZE_4KB));
  }

  return Status;
}

/**
  Update a boot region.

  This function also output the update process info.

  @param[in] UpdateRegion     The detail information for this region to update.
  @param[in] WrittenSize      The data size has been written before this region.
  @param[in] TotalSize        The total size need to write for the partition.

  @retval  EFI_SUCCESS        Update this boot region successfully.
  @retval  others             Error happening when updating boot region.
**/
EFI_STATUS
UpdateBootRegion (
  IN  FIRMWARE_UPDATE_REGION     *UpdateRegion,
  IN  UINT32                     WrittenSize,
  IN  UINT32                     TotalSize
  )
{
  EFI_STATUS    Status;
  UINT32        UpdateBlockSize;
  UINT32        UpdatedSize;
  UINT64        UpdateAddress;
  UINT8         *Buffer;

  //
  // Here write 64KB every time in order to show update process.
  //
  UpdateAddress   = UpdateRegion->ToUpdateAddress;
  Buffer          = UpdateRegion->SourceAddress;

  UpdatedSize = 0;
  while (UpdatedSize < UpdateRegion->UpdateSize) {
    if (UpdateRegion->UpdateSize < SIZE_4KB) {
      UpdateBlockSize = UpdateRegion->UpdateSize;
    } else {
      if (UpdatedSize + SIZE_64KB > UpdateRegion->UpdateSize) {
        UpdateBlockSize = UpdateRegion->UpdateSize - UpdatedSize;
      } else {
        UpdateBlockSize = SIZE_64KB;
      }
    }
    ConsolePrint ("Updating 0x%08llx, Size:0x%06x\n", UpdateAddress, UpdateBlockSize);
    Status = UpdateRegionBlock (UpdateAddress, Buffer, UpdateBlockSize);
    if (EFI_ERROR (Status)) {
      ConsolePrint ("\nFailed at address 0x%08llx, status: %r\n", UpdateAddress, Status);
      return Status;
    }
    UpdateAddress += UpdateBlockSize;
    Buffer        += UpdateBlockSize;
    UpdatedSize   += UpdateBlockSize;
    ConsolePrint ("\nFinished   %3d%%\n", (WrittenSize + UpdatedSize) * 100 / TotalSize);
  }

  return EFI_SUCCESS;
}

/**
  Update all the regions in this boot partition.

  @param[in] UpdatePartition  The detail information for this update.

  @retval  EFI_SUCCESS        Update this boot partition successfully.
  @retval  others             Error happening when updating boot partition.
**/
EFI_STATUS
UpdateBootPartition (
  IN FIRMWARE_UPDATE_PARTITION   *UpdatePartition
  )
{
  EFI_STATUS                     Status;
  UINT32                         Index;
  FIRMWARE_UPDATE_REGION        *UpdateRegion;
  FIRMWARE_UPDATE_REGION         TempRegion;
  UINT32                         TotalUpdateSize;
  UINT32                         WrittenSize;

  Status = PrepareRegionsUpdate (UpdatePartition);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "PrepareRegionsUpdate, Status = 0x%x\n", Status));
    return Status;
  }

  TotalUpdateSize = 0;
  for (Index = 0; Index < UpdatePartition->RegionCount; Index++) {
    UpdateRegion     = &UpdatePartition->FwRegion[Index];
    TotalUpdateSize += UpdateRegion->UpdateSize;
  }

  WrittenSize = 0;
  for (Index = 0; Index < UpdatePartition->RegionCount; Index++) {
    // Adjust the offset to be relative to BIOS region start
    CopyMem (&TempRegion, &UpdatePartition->FwRegion[Index], sizeof(FIRMWARE_UPDATE_REGION));
    TempRegion.ToUpdateAddress += GetRomImageOffsetInBiosRegion ();
    Status = UpdateBootRegion (&TempRegion, WrittenSize, TotalUpdateSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "UpdateBootRegion failed! Status = 0x%x\n", Status));
      return Status;
    }
    WrittenSize += TempRegion.UpdateSize;
  }

  return Status;
}

/**
  Perform full BIOS region update.

  @param[in] ImageHdr       Pointer to fw mgmt capsule Image header

  @retval  EFI_SUCCESS      Update successful.
  @retval  other            error occurred during firmware update
**/
EFI_STATUS
UpdateFullBiosRegion (
  IN EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr
  )
{
  EFI_STATUS               Status;
  UINT32                   BiosRgnBase;
  UINT32                   BiosRgnSize;
  FIRMWARE_UPDATE_REGION   UpdateRegion;

  DEBUG((DEBUG_INFO, "Update full BIOS region\n"));
  Status = BootMediaGetRegion (FlashRegionBios, &BiosRgnBase, &BiosRgnSize);
  if (!EFI_ERROR (Status)) {
    if (ImageHdr->UpdateImageSize > BiosRgnSize) {
      DEBUG((DEBUG_ERROR, "BIOS image in capsule is bigger than BIOS region on flash\n"));
      Status = EFI_UNSUPPORTED;
    }
  }
  if (ALIGN_DOWN(ImageHdr->UpdateImageSize, SIZE_4KB) != ImageHdr->UpdateImageSize) {
    DEBUG((DEBUG_ERROR, "BIOS image size in capsule is not 4KB aligned\n"));
    Status = EFI_UNSUPPORTED;
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (&UpdateRegion, sizeof(UpdateRegion));
  UpdateRegion.ToUpdateAddress = BiosRgnSize - ImageHdr->UpdateImageSize;
  UpdateRegion.UpdateSize      = ImageHdr->UpdateImageSize;
  UpdateRegion.SourceAddress   = (UINT8 *)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));
  Status = UpdateBootRegion (&UpdateRegion, 0, UpdateRegion.UpdateSize);
  return Status;
}

/**
  Perform system Firmware update.

  This function will update SBL or Configuration data alone.

  @param[in] ImageHdr       Pointer to fw mgmt capsule Image header
  @param[in] FwPolicy       Fw update policy

  @retval  EFI_SUCCESS      Update successful.
  @retval  other            error occurred during firmware update
**/
EFI_STATUS
UpdateSystemFirmware (
  IN EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr,
  IN FIRMWARE_UPDATE_POLICY        FwPolicy
  )
{
  EFI_STATUS                  Status;
  FIRMWARE_UPDATE_PARTITION   *UpdatePartition;

  //
  // Check firmware version.
  //
  Status = VerifyFwVersion (ImageHdr, FwPolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, " VerifyFwVersion failed with Status = 0x%x\n", Status));
    return Status;
  }

  //
  // Get firmware update required information.
  //
  Status = GetFirmwareUpdateInfo (ImageHdr, FwPolicy, &UpdatePartition);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "GetFirmwareUpdateInfo, Status = 0x%x\n", Status));
    return Status;
  }

  //
  // Check firmware structure.
  //
  Status = VerifyFwStruct (ImageHdr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, " VerifyFwStruct failed with Status = 0x%x\n", Status));
    return Status;
  }

  //
  // Do boot partition update.
  //
  Status = UpdateBootPartition (UpdatePartition);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UpdateBootPartition, Status = 0x%x\n", Status));
    return Status;
  }

  return Status;
}

/**
  Perform single component update.

  This function will create update partition region for a single
  non reundant component.

  @param[in] CompBase       Base address of the component.
  @param[in] CompSize       Size of the component.
  @param[in] ImageHdr       Pointer to fw mgmt capsule Image header

  @retval  EFI_SUCCESS      Update successful.
  @retval  other            error occurred during firmware update
**/
EFI_STATUS
UpdateSingleComponent (
  IN UINT32                         CompBase,
  IN UINT32                         CompSize,
  IN EFI_FW_MGMT_CAP_IMAGE_HEADER   *ImageHdr
  )
{
  EFI_STATUS                Status;
  UINT32                    CompName;
  FLASH_MAP                 *FlashMap;
  UINT32                    AllocateSize;
  FIRMWARE_UPDATE_PARTITION *UpdatePartition;
  FIRMWARE_UPDATE_REGION    *UpdateRegion;

  if (ImageHdr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CompName = (UINT32)ImageHdr->UpdateHardwareInstance;

  if (ImageHdr->UpdateImageSize > CompSize) {
    DEBUG ((DEBUG_INFO, "%4a component capsule payload size is too big for the region on flash! \n", (CHAR8 *)&CompName));
    return EFI_UNSUPPORTED;
  }

  // Get current boot partition - Primary or backup blocks
  FlashMap = GetFlashMapPtr();
  if (FlashMap == NULL) {
    return EFI_NOT_FOUND;
  }

  AllocateSize    = sizeof (FIRMWARE_UPDATE_PARTITION) + sizeof (FIRMWARE_UPDATE_REGION);
  UpdatePartition = (FIRMWARE_UPDATE_PARTITION *) AllocatePool (AllocateSize);
  ASSERT (UpdatePartition != NULL);

  UpdateRegion                  = &UpdatePartition->FwRegion[0];
  UpdateRegion->ToUpdateAddress = FlashMap->RomSize + CompBase;
  UpdateRegion->UpdateSize      = ImageHdr->UpdateImageSize;
  UpdateRegion->SourceAddress   = (UINT8 *)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));
  UpdatePartition->RegionCount  = 1;

  //
  // Do boot partition update.
  //
  Status = UpdateBootPartition (UpdatePartition);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Updating component %4a failed with status = %r\n", (CHAR8 *)&CompName, Status));
  }

  return Status;
}

/**
  Perform non redundant component update.

  This function will update single non redundant component update.

  @param[in] ImageHdr       Pointer to fw mgmt capsule Image header

  @retval  EFI_SUCCESS      Update successful.
  @retval  other            error occurred during firmware update
**/
EFI_STATUS
UpdateNonRedundantComp (
  IN EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr
  )
{
  EFI_STATUS                    Status;
  UINT32                        CompBase;
  UINT32                        CompSize;

  Status = GetComponentInfoByPartition ((UINT32)ImageHdr->UpdateHardwareInstance, FALSE, &CompBase, &CompSize);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_INFO, "No component with signature 0x%x found ! \n", (UINT32)ImageHdr->UpdateHardwareInstance));
    return Status;
  }

  //
  // Update the component
  //
  Status = UpdateSingleComponent (CompBase, CompSize, ImageHdr);

  return Status;
}

/**
  Perform container component update.

  This function will try to locate component inside the container
  and if found, will update the container component.

  @param[in] ImageHdr       Pointer to fw mgmt capsule Image header

  @retval  EFI_SUCCESS      Update successful.
  @retval  other            error occurred during firmware update
**/
EFI_STATUS
UpdateContainerComp (
  IN EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr
  )
{
  EFI_STATUS        Status;
  UINT32            ContainerName;
  UINT32            ComponentName;
  UINT32            ComponentBase;
  CONTAINER_ENTRY   *ContainerEntryPtr;
  COMPONENT_ENTRY   *ComponentEntryPtr;
  CONTAINER_HDR     *ContainerHdr;
  LOADER_COMPRESSED_HEADER *FlashCompLzHeader;
  LOADER_COMPRESSED_HEADER *CapCompLzHeader;

  ComponentName = (UINT32)RShiftU64 (ImageHdr->UpdateHardwareInstance, 32);
  ContainerName = (UINT32)ImageHdr->UpdateHardwareInstance;

  Status = LocateComponentEntry (ContainerName, ComponentName, &ContainerEntryPtr, &ComponentEntryPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Container component update failed with status: %r \n", Status));
    return Status;
  }

  if ((ContainerEntryPtr == NULL) || (ComponentEntryPtr == NULL)) {
    return EFI_NOT_FOUND;
  }

  //
  // Update the component
  //
  ContainerHdr = (CONTAINER_HDR *)(UINTN)ContainerEntryPtr->HeaderCache;
  //
  // Component base = Container base + data offset from container base + offset of component inside container
  //
  ComponentBase = ContainerEntryPtr->Base + ContainerHdr->DataOffset + ComponentEntryPtr->Offset;

  // Current implementation only supports compressed header.
  // Exception: Signature is zero as a mark for previously detected bad region, e.g., TCCT
  FlashCompLzHeader = (LOADER_COMPRESSED_HEADER *) (UINTN) ComponentBase;
  CapCompLzHeader   = (LOADER_COMPRESSED_HEADER *) ((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));
  if (((IS_COMPRESSED (FlashCompLzHeader) == FALSE) && (FlashCompLzHeader->Signature != 0)) ||
      (IS_COMPRESSED (CapCompLzHeader) == FALSE)) {
    return EFI_UNSUPPORTED;
  }

  // Check Svn for container component
  if (CapCompLzHeader->Svn < FlashCompLzHeader->Svn) {
    DEBUG((DEBUG_INFO, "Container Component svn did not met!"));
    return EFI_UNSUPPORTED;
  }

  Status = UpdateSingleComponent (ComponentBase, ComponentEntryPtr->Size, ImageHdr);

  return Status;
}

/**
  Perform container and component svn checks

  This function will try to locate container and components in the flash map
  and next from capsule. Container and components svn checks are performed.

  @param[in]  ImageHdr       Pointer to fw mgmt capsule Image header
  @param[out] SvnStatus      Svn compare status

  @retval  EFI_SUCCESS      SVN check successful.
  @retval  other            error occurred during firmware update
**/
EFI_STATUS
CheckSblContainerSvn (
  IN   EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr,
  OUT  UINT8                         *SvnStatus
  )
{
  CONTAINER_HDR             *FlashContainerHdr;
  CONTAINER_ENTRY           *ContainerEntry;
  LOADER_COMPRESSED_HEADER  *FlashLzHdr;
  CONTAINER_HDR             *CapContainerAddr;
  COMPONENT_ENTRY           *CapCompEntry;
  LOADER_COMPRESSED_HEADER  *CapLzHdr;
  UINT64                    FlashComponentName;
  UINT8                     ContainerSvnCheck;
  UINT8                     ComponentSvnCheck;
  EFI_STATUS                Status;

  Status = EFI_SUCCESS;
  ContainerSvnCheck = 0;
  ComponentSvnCheck = 0;

  // Locate container entry info from flash
  Status = LocateComponentEntry ((UINT32)ImageHdr->UpdateHardwareInstance, 0, &ContainerEntry, NULL);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  // Locate container header currently in flash
  FlashContainerHdr = (CONTAINER_HDR *)(UINTN)ContainerEntry->HeaderCache;

  // Locate the container header info from capsule image.
  CapContainerAddr   = (CONTAINER_HDR *)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));

  //Check capsule container SVN with container avaiable in flash
  if (CapContainerAddr->Svn >= FlashContainerHdr->Svn) {
    // Container SVN  met.
    ContainerSvnCheck = 1;
  } else {
    DEBUG((DEBUG_INFO, "Container Svn check failed!!\n"));
    // Container SVN not met.
    ContainerSvnCheck = 0;
    return EFI_UNSUPPORTED;
  }

  // Enumerate all components in container
  FlashComponentName = 0;
  do {
    // Get next avaiable component in flash container
    Status = GetNextAvailableComponent (FlashContainerHdr->Signature, (UINT32 *)&FlashComponentName);
    if (EFI_ERROR (Status)) {
      break;
    }
    DEBUG ((DEBUG_INFO, "Flash COMP:%a\n", &FlashComponentName));
    // Locate flash component header and image info
    Status = LocateComponent (FlashContainerHdr->Signature, (UINT32) FlashComponentName, (VOID **)&FlashLzHdr, NULL);
    if (EFI_ERROR (Status)) {
      break;
    }

    // Retrieve componenent address details from capsule container
    CapCompEntry = NULL;
    CapCompEntry = LocateComponentEntryFromContainer ((CONTAINER_HDR *) CapContainerAddr, (UINT32 ) FlashComponentName);
    if (CapCompEntry != NULL) {
      CapLzHdr = (LOADER_COMPRESSED_HEADER *)((UINT8 *)CapContainerAddr + CapContainerAddr->DataOffset + CapCompEntry->Offset);

      if ((IS_COMPRESSED (CapLzHdr) == FALSE) || (IS_COMPRESSED (FlashLzHdr) == FALSE)) {
        DEBUG ((DEBUG_INFO, "Component compressed header signature mismatch!!\n"));
        return EFI_UNSUPPORTED;
      }

      //Check capsule component SVN with flash componenet svn
      if (CapLzHdr->Svn >= FlashLzHdr->Svn) {
        // Component SVN  met.
        ComponentSvnCheck = 1;
      } else {
        // Component SVN not met. No need to check remaining components
        ComponentSvnCheck = 0;
        DEBUG((DEBUG_INFO, "Component Svn check failed for %a!!\n", &FlashComponentName));
        break;
      }
    }
  } while ((Status == EFI_SUCCESS));

  // Check validatity of container and component svn checks
  if ((ContainerSvnCheck == 1) && (ComponentSvnCheck == 1)){
    *SvnStatus = 1;
    Status = EFI_SUCCESS;
  }

  return Status;
}

/**
  This function would check if update is for container Image.

  @param[in]  Signature   Container signature

  @retval  BOOLEAN        Container True/False

**/
BOOLEAN
IsUpdateComponentForContainer (
  IN  UINT32   Signature
  )
{
  CONTAINER_ENTRY       *ContainerEntryPtr;
  EFI_STATUS             Status;

  ContainerEntryPtr = NULL;

  Status = LocateComponentEntry (Signature, 0, &ContainerEntryPtr, NULL);
  if (EFI_ERROR (Status)) {
      return FALSE;
  }

  if (ContainerEntryPtr == NULL) {
   return FALSE;
  }

   return TRUE;
}

/**
  Perform Config data svn check

  This function will perform svn checks for cfgdata in flash and
  config data in capsule.

  @param[in]  ImageHdr       Pointer to fw mgmt capsule Image header
  @param[in]  FwPolicy       Firmware update policy
  @param[out] SvnStatus      Svn compare status

  @retval  EFI_SUCCESS      SVN check successful.
  @retval  other            error occurred during firmware update
**/
EFI_STATUS
CheckSblConfigDataSvn (
  IN   EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr,
  IN   FIRMWARE_UPDATE_POLICY        FwPolicy,
  OUT  UINT8                         *SvnStatus
  )
{
  CDATA_BLOB                *CfgBlobCapAddr;
  CDATA_BLOB                *CfgBlobFlashDataPtr;
  UINT32                     CfgBlobFlashAddr;
  UINT32                     CfgBlobSize;
  EFI_STATUS                 Status;

  Status = EFI_INVALID_PARAMETER;

  if ((ImageHdr == NULL) || (SvnStatus == NULL)) {
    return Status;
  }

  //
  // Get base address of CFGDATA from  firmware block to update
  //

  if (FwPolicy.Fields.UpdatePartitionB == 0x1) {
    Status = GetComponentInfoByPartition(FLASH_MAP_SIG_CFGDATA, TRUE, (UINT32 *) &CfgBlobFlashAddr, &CfgBlobSize);
  } else if (FwPolicy.Fields.UpdatePartitionA == 0x1) {
    Status = GetComponentInfoByPartition(FLASH_MAP_SIG_CFGDATA, FALSE, (UINT32 *) &CfgBlobFlashAddr, &CfgBlobSize);
  }

  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Error in retrieving config data on flash: %r\n", Status));
    return Status;
  }

  CfgBlobFlashDataPtr = (CDATA_BLOB *) (UINTN) CfgBlobFlashAddr;

  // Locate config data blob header info from capsule image
  CfgBlobCapAddr = (CDATA_BLOB *)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));
  if (CfgBlobCapAddr->Signature != CFG_DATA_SIGNATURE){
    DEBUG((DEBUG_INFO, "Config data signature in capsule image does not match\n"));
    return EFI_NOT_FOUND;
  }

  //Check SVN
  if (CfgBlobCapAddr->ExtraInfo.Svn >= CfgBlobFlashDataPtr->ExtraInfo.Svn){
    *SvnStatus = 1;
  } else {
    *SvnStatus = 0;
  }

  return EFI_SUCCESS;
}

/**
  Perform ACM svn check

  This function will perform svn checks for ACM in flash and
  ACM in capsule.

  @param[in]  ImageHdr       Pointer to fw mgmt capsule Image header

  @retval  EFI_SUCCESS      SVN check successful.
  @retval  other            error occurred during firmware update
**/
EFI_STATUS
CheckAcmSvn (
  IN   EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr
  )
{
  EFI_STATUS            Status;
  UINT16                ExistingAcmSvn;
  UINT16                NewAcmSvn;
  UINT32                NewAcmHdr;

  if ((ImageHdr == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetExistingAcmSvn (&ExistingAcmSvn);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Unable to get existing ACM SVN!"));
    return Status;
  }

  NewAcmHdr = (UINT32)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));

  Status = GetAcmSvnFromAcmHdr(NewAcmHdr, &NewAcmSvn);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Unable to get new ACM SVN!"));
    return Status;
  }

  if (NewAcmSvn < ExistingAcmSvn) {
    DEBUG((DEBUG_ERROR, "Acm update Svn check failed!!\n"));
    return EFI_INCOMPATIBLE_VERSION;
  }

  return EFI_SUCCESS;
}

/**
  Perform Slim Bootloader component update.

  This function will try to locate component in the flash map,
  if found, will update the component.

  @param[in] ImageHdr       Pointer to fw mgmt capsule Image header
  @param[in] FwPolicy       Fw update policy

  @retval  EFI_SUCCESS      Update successful.
  @retval  other            error occurred during firmware update
**/
EFI_STATUS
UpdateSblComponent (
  IN EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr,
  IN FIRMWARE_UPDATE_POLICY        FwPolicy
  )
{
  EFI_STATUS             Status;
  UINT8                  SvnStatus;

  Status = EFI_NOT_FOUND;
  SvnStatus = 0;

  if ((UINT32)RShiftU64 (ImageHdr->UpdateHardwareInstance, 32)){
    //
    // Upper 4 bytes are not null, this is a container component update.
    //
    DEBUG ((DEBUG_INFO, "Container component update requested! \n"));
    Status = UpdateContainerComp (ImageHdr);
    return Status;
  }

  //
  // SBL component update, check if it is a container
  //
  if (IsUpdateComponentForContainer ((UINT32) ImageHdr->UpdateHardwareInstance)){

    DEBUG((DEBUG_INFO, "Capsule update is for container region!!\n"));
    // Check  security version for container and its components to update
    Status = CheckSblContainerSvn (ImageHdr, &SvnStatus);
    if (SvnStatus != 1) {
      DEBUG((DEBUG_INFO, "Container Update Svn check failed!!\n"));
      return Status;
    }
  } else {
    DEBUG((DEBUG_INFO, "SBL component update for non-container region!\n"));
  }

  //
  // This is a SBL component update, check if it is a redundant component
  //
  if (IsRedundantComponent(ImageHdr->UpdateHardwareInstance)) {
    DEBUG ((DEBUG_INFO, "Redundant component update requested! \n"));
    Status = UpdateSystemFirmware(ImageHdr, FwPolicy);
  } else {
    DEBUG ((DEBUG_INFO, "Non redundant component update requested! \n"));
    Status = UpdateNonRedundantComp(ImageHdr);
  }
  return Status;
}

/**
  Read the value of FW_UPDATE_STATUS.CsmeNeedReset

  The CsmeNeedReset flag is used to ensure CSME update
  has taken effect before processing CMDI payload.
  This is specific to prevent {OEMKEYREVOCATION} command
  failure for the case that CSME payload contains OEM KM
  with key revocation extension.

  @retval  Value  Value of FW_UPDATE_STATUS.CsmeNeedReset
**/
UINT8
ReadCsmeNeedResetFlag (
  VOID
  )
{
  EFI_STATUS  Status;
  UINT32      FwUpdStatusOffset;
  UINT8       Value;

  FwUpdStatusOffset = PcdGet32(PcdFwUpdStatusBase);
  FwUpdStatusOffset += OFFSET_OF(FW_UPDATE_STATUS, CsmeNeedReset);

  Value = CSME_NEED_RESET_INIT;
  Status = BootMediaRead (FwUpdStatusOffset, sizeof(UINT8), (UINT8 *)&Value);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BootMediaRead CsmeNeedReset. offset: 0x%04x, Status = 0x%x\n",
          FwUpdStatusOffset, Status));
    Value = CSME_NEED_RESET_INVALID;
  }

  return Value;
}

/**
  Write the value of FW_UPDATE_STATUS.CsmeNeedReset

  @param[in] Value  Value to be written to FW_UPDATE_STATUS.CsmeNeedReset

  @retval  EFI_SUCCESS            Write operation is successful
  @retval  EFI_INVALID_PARAMETER  Invalid parameter
  @retval  EFI_DEVICE_ERROR       Write operation failed
**/
EFI_STATUS
WriteCsmeNeedResetFlag (
  IN  UINT8  Value
  )
{
  EFI_STATUS  Status;
  UINT32      FwUpdStatusOffset;
  UINT8       CurrVal;

  CurrVal = ReadCsmeNeedResetFlag();
  if (Value > CurrVal) {
    DEBUG((DEBUG_ERROR, "WriteCsmeNeedResetFlag invalid parameter: %x, current value: %x\n",
          Value, CurrVal));
    return EFI_INVALID_PARAMETER;
  }

  FwUpdStatusOffset = PcdGet32(PcdFwUpdStatusBase);
  FwUpdStatusOffset += OFFSET_OF(FW_UPDATE_STATUS, CsmeNeedReset);

  Status = BootMediaWrite (FwUpdStatusOffset, sizeof(UINT8), (UINT8 *)&Value);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BootMediaWrite CsmeNeedReset=%x. offset: 0x%04x, Status = 0x%x\n",
          Value, FwUpdStatusOffset, Status));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Reboot platform.

  @param[in]  ResetType   Cold, Warm or Shutdown

**/
VOID
Reboot (
  IN  EFI_RESET_TYPE        ResetType
  )
{
  // Set FW_UPDATE_STATUS.CsmeNeedReset to DONE since the system will do a reset
  if (ReadCsmeNeedResetFlag() == CSME_NEED_RESET_PENDING) {
    WriteCsmeNeedResetFlag(CSME_NEED_RESET_DONE);
  }

  ConsolePrint("Reset required to proceed.\n\n");
  MicroSecondDelay (3000000);
  ResetSystem (ResetType);
  CpuDeadLoop ();
}

/**
  Verify uCode internal structure

  @param[in] ImageHdr     Pointer to the fw mgmt capsule image header

  @retval  EFI_SUCCESS    The operation completed successfully.
  @retval  others         There is error happening.
**/
EFI_STATUS
VerifyUcodeStruct (
  IN  EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr
  )
{
  UINTN                 ImageBase;
  UINTN                 ImageOffset;
  CPU_MICROCODE_HEADER  *UCodeHdr;
  UINT8                 *ImageByte;

  // Update is only supported for platforms that slot their uCode
  if (PcdGet32 (PcdUcodeSlotSize) == 0) {
    DEBUG((DEBUG_ERROR, "Existing image does not contain uCode slots!!\n"));
    return EFI_UNSUPPORTED;
  }

  ImageBase = (UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER);
  ImageOffset = 0;

  ImageByte = (UINT8*)(ImageBase + ImageOffset);
  while (*ImageByte != PAD_BYTE && ImageOffset < ImageHdr->UpdateImageSize) {
    UCodeHdr = (CPU_MICROCODE_HEADER *)ImageByte;

    // Ensure patches in update image start at slot boundaries
    if (UCodeHdr->HeaderVersion != 1) {
      DEBUG((DEBUG_ERROR, "Existing uCode slots do not line up with new uCode slots!!\n"));
      return EFI_NO_MAPPING;
    }

    // Ensure total size from header does not exceed slot size
    if (UCodeHdr->TotalSize > PcdGet32 (PcdUcodeSlotSize)) {
      DEBUG((DEBUG_ERROR, "Total uCode size from header exceeds uCode slot size!!\n"));
      return EFI_NO_MAPPING;
    }
    ImageOffset += PcdGet32(PcdUcodeSlotSize);
    ImageByte = (UINT8*)(ImageBase + ImageOffset);
  }

  // Check remaining bytes are unused
  while (ImageOffset < ImageHdr->UpdateImageSize) {
    if (*ImageByte != PAD_BYTE) {
      DEBUG((DEBUG_ERROR, "Existing image slots do not line up with new image slots!!\n"));
      return EFI_NO_MAPPING;
    }
    ++ImageOffset;
    ImageByte = (UINT8*)(ImageBase + ImageOffset);
  }

  return EFI_SUCCESS;
}

/**
  Verify the firmware internal structure.

  @param[in] ImageHdr     Pointer to the fw mgmt capsule image header

  @retval  EFI_SUCCESS    The operation completed successfully.
  @retval  others         There is error happening.
**/
EFI_STATUS
VerifyFwStruct (
  IN  EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr
  )
{
  EFI_STATUS  Status;

  if ((UINT32)ImageHdr->UpdateHardwareInstance == FLASH_MAP_SIG_UCODE) {
    Status = VerifyUcodeStruct (ImageHdr);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}
