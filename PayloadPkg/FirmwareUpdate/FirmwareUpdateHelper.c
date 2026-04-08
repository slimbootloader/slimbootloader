/** @file
  Internal functions to update firmware in boot media.

  Copyright (c) 2020 - 2023, Intel Corporation. All rights reserved.<BR>
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
#include <Library/SocInfoLib.h>

#define MSR_IA32_BIOS_SIGN_ID 0x0000008B

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
  if (mFwuSpiService == NULL) {
    DEBUG ((DEBUG_ERROR, "BootMediaGetRegion service not initialized\n"));
    return EFI_NOT_READY;
  }

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
  UINT64  EndAddress;

  if (mFwuSpiService == NULL) {
    DEBUG ((DEBUG_ERROR, "BootMediaRead service not initialized\n"));
    return EFI_NOT_READY;
  }

  if ((ByteCount != 0) && (Buffer == NULL)) {
    DEBUG ((DEBUG_ERROR, "BootMediaRead invalid buffer for non-zero ByteCount\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (Address > MAX_UINT32) {
    DEBUG ((DEBUG_ERROR, "BootMediaRead address out of range: 0x%llx\n", Address));
    return EFI_INVALID_PARAMETER;
  }

  if (ByteCount != 0) {
    EndAddress = Address + (UINT64)ByteCount - 1;
    if ((EndAddress < Address) || (EndAddress > MAX_UINT32)) {
      DEBUG ((DEBUG_ERROR, "BootMediaRead range out of bounds: 0x%llx + 0x%x\n", Address, ByteCount));
      return EFI_INVALID_PARAMETER;
    }
  }

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
  UINT64  EndAddress;

  if (mFwuSpiService == NULL) {
    DEBUG ((DEBUG_ERROR, "BootMediaReadByType service not initialized\n"));
    return EFI_NOT_READY;
  }

  if ((ByteCount != 0) && (Buffer == NULL)) {
    DEBUG ((DEBUG_ERROR, "BootMediaReadByType invalid buffer for non-zero ByteCount\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (Address > MAX_UINT32) {
    DEBUG ((DEBUG_ERROR, "BootMediaReadByType address out of range: 0x%llx\n", Address));
    return EFI_INVALID_PARAMETER;
  }

  if (ByteCount != 0) {
    EndAddress = Address + (UINT64)ByteCount - 1;
    if ((EndAddress < Address) || (EndAddress > MAX_UINT32)) {
      DEBUG ((DEBUG_ERROR, "BootMediaReadByType range out of bounds: 0x%llx + 0x%x\n", Address, ByteCount));
      return EFI_INVALID_PARAMETER;
    }
  }

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
  UINT64  EndAddress;

  if (mFwuSpiService == NULL) {
    DEBUG ((DEBUG_ERROR, "BootMediaWrite service not initialized\n"));
    return EFI_NOT_READY;
  }

  if ((ByteCount != 0) && (Buffer == NULL)) {
    DEBUG ((DEBUG_ERROR, "BootMediaWrite invalid buffer for non-zero ByteCount\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (Address > MAX_UINT32) {
    DEBUG ((DEBUG_ERROR, "BootMediaWrite address out of range: 0x%llx\n", Address));
    return EFI_INVALID_PARAMETER;
  }

  if (ByteCount != 0) {
    EndAddress = Address + (UINT64)ByteCount - 1;
    if ((EndAddress < Address) || (EndAddress > MAX_UINT32)) {
      DEBUG ((DEBUG_ERROR, "BootMediaWrite range out of bounds: 0x%llx + 0x%x\n", Address, ByteCount));
      return EFI_INVALID_PARAMETER;
    }
  }

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
  UINT64  EndAddress;

  if (mFwuSpiService == NULL) {
    DEBUG ((DEBUG_ERROR, "BootMediaWriteByType service not initialized\n"));
    return EFI_NOT_READY;
  }

  if ((ByteCount != 0) && (Buffer == NULL)) {
    DEBUG ((DEBUG_ERROR, "BootMediaWriteByType invalid buffer for non-zero ByteCount\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (Address > MAX_UINT32) {
    DEBUG ((DEBUG_ERROR, "BootMediaWriteByType address out of range: 0x%llx\n", Address));
    return EFI_INVALID_PARAMETER;
  }

  if (ByteCount != 0) {
    EndAddress = Address + (UINT64)ByteCount - 1;
    if ((EndAddress < Address) || (EndAddress > MAX_UINT32)) {
      DEBUG ((DEBUG_ERROR, "BootMediaWriteByType range out of bounds: 0x%llx + 0x%x\n", Address, ByteCount));
      return EFI_INVALID_PARAMETER;
    }
  }

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
  UINT64  EndAddress;

  if (mFwuSpiService == NULL) {
    DEBUG ((DEBUG_ERROR, "BootMediaErase service not initialized\n"));
    return EFI_NOT_READY;
  }

  if (Address > MAX_UINT32) {
    DEBUG ((DEBUG_ERROR, "BootMediaErase address out of range: 0x%llx\n", Address));
    return EFI_INVALID_PARAMETER;
  }

  if (ByteCount != 0) {
    EndAddress = Address + (UINT64)ByteCount - 1;
    if ((EndAddress < Address) || (EndAddress > MAX_UINT32)) {
      DEBUG ((DEBUG_ERROR, "BootMediaErase range out of bounds: 0x%llx + 0x%x\n", Address, ByteCount));
      return EFI_INVALID_PARAMETER;
    }
  }

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
  INT32                 FvBaseOffset;
  INT64                 Stage1AFvBase64;
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

  if (Stage1AFvPointer < TopSwapRegionSize) {
    DEBUG((DEBUG_ERROR, "Invalid Stage1A FV pointer 0x%08x below top swap region size 0x%08x\n",
          Stage1AFvPointer, TopSwapRegionSize));
    return EFI_COMPROMISED_DATA;
  }
  Stage1AFvPointer = Stage1AFvPointer - TopSwapRegionSize;

  FvBaseOffset = (INT32)(*(UINT32 *)(UINTN)Stage1AFvPointer);
  Stage1AFvBase64 = (INT64)(UINT64)Stage1AFvPointer + (INT64)FvBaseOffset + (INT64)sizeof (UINT32);
  if ((Stage1AFvBase64 < 0) || (Stage1AFvBase64 > MAX_UINT32)) {
    DEBUG((DEBUG_ERROR, "Stage1A FV base computation out of range: Ptr=0x%08x Offset=0x%08x\n",
          Stage1AFvPointer, (UINT32)FvBaseOffset));
    return EFI_COMPROMISED_DATA;
  }
  Stage1AFvBase = (UINT32)Stage1AFvBase64;

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
EFIAPI
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
  UINT8         *VerifyBuffer;
  UINT32        Count;
  UINT32        BlockLen;
  UINT32        WriteSize;
  UINT8         *WriteBuffer;
  UINT8         *Src;

  ReadBuffer = NULL;
  VerifyBuffer = NULL;

  if (Length == 0) {
    return EFI_SUCCESS;
  }

  if ((Address & (SIZE_4KB - 1)) != 0) {
    DEBUG ((DEBUG_ERROR, "UpdateRegionBlock requires 4KB-aligned address: 0x%llx\n", Address));
    return EFI_INVALID_PARAMETER;
  }

  ReadBuffer = AllocatePages (EFI_SIZE_TO_PAGES (SIZE_4KB));
  if (ReadBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  VerifyBuffer = AllocatePages (EFI_SIZE_TO_PAGES (SIZE_4KB));
  if (VerifyBuffer == NULL) {
    FreePages (ReadBuffer, EFI_SIZE_TO_PAGES (SIZE_4KB));
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

    // Partial tail update still needs full 4KB erase granularity.
    // Preserve trailing bytes in the same erase block to avoid corrupting
    // adjacent data outside the requested update range.
    if (BlockLen < SIZE_4KB) {
      Status = BootMediaRead(Address + Count, SIZE_4KB, ReadBuffer);
    } else {
      Status = BootMediaRead(Address + Count, BlockLen, ReadBuffer);
    }
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "BootMediaRead.  readaddr: 0x%llx, Status = 0x%x\n", Address + Count, Status));
      goto End;
    }

    if (CompareMem (Src + Count, ReadBuffer, BlockLen) == 0) {
      DEBUG ((DEBUG_INIT, "."));
      continue;
    }

    if (BlockLen < SIZE_4KB) {
      CopyMem (ReadBuffer, Src + Count, BlockLen);
      WriteBuffer = ReadBuffer;
      WriteSize   = SIZE_4KB;
    } else {
      WriteBuffer = Src + Count;
      WriteSize   = BlockLen;
    }

    //
    // Erase the boot media
    // Block length for erase is always 4K bytes
    //
    DEBUG ((DEBUG_INIT, "x"));
    Status = BootMediaErase (Address + Count,  SIZE_4KB);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "ERROR: in BootMediaErase. Status = 0x%x\n", Status));
      goto End;
    }

    //
    // Write to the boot media
    //
    Status = BootMediaWrite (Address + Count, WriteSize, WriteBuffer);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "ERROR: in BootDeviceWrite. Status = 0x%x\n", Status));
      goto End;
    }

    //
    // Verify the written data
    //
    Status = BootMediaRead (Address + Count, WriteSize, VerifyBuffer);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Verify BootMediaRead failed.  readaddr: 0x%llx, Status = 0x%x\n", Address + Count, Status));
      goto End;
    }

    if (CompareMem (WriteBuffer, VerifyBuffer, WriteSize) != 0) {
      DEBUG ((DEBUG_ERROR, "Verify Error !\n"));
      Status = EFI_DEVICE_ERROR;
      goto End;
    }
  }

End:
  if (VerifyBuffer != NULL) {
    FreePages (VerifyBuffer, EFI_SIZE_TO_PAGES (SIZE_4KB));
  }

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
  Preserve a specific component inside a container during BIOS region update.

  This function locates a component within a container on flash and overlays
  only that component's data onto the capsule image buffer, so that after the
  BIOS region is written, the component data remains unchanged while other
  components in the same container can still be updated.

  @param[in] ContainerSig    Container signature in the flash map (e.g. 'IPFW').
  @param[in] ComponentName   Component name inside the container (e.g. 'SMBS').
  @param[in] ImageHdr        Pointer to fw mgmt capsule Image header.

  @retval  EFI_SUCCESS       Component data preserved successfully.
  @retval  EFI_NOT_FOUND     Container or component not found.
  @retval  others            Error reading from boot media.
**/
STATIC
EFI_STATUS
PreserveContainerComponent (
  IN  UINT32                          ContainerSig,
  IN  UINT32                          ComponentName,
  IN  EFI_FW_MGMT_CAP_IMAGE_HEADER   *ImageHdr
  )
{
  EFI_STATUS              Status;
  CONTAINER_HDR           *ContainerHdr;
  CONTAINER_ENTRY         *ContainerEntry;
  COMPONENT_ENTRY         *CompEntry;
  FLASH_MAP               *FlashMapPtr;
  UINT8                   *CapsuleBase;
  UINT32                  BiosRgnOffset;
  UINT32                  RomBase;
  UINT32                  CompRomOffset;

  FlashMapPtr = GetFlashMapPtr ();
  if (FlashMapPtr == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = LocateComponentEntry (ContainerSig, ComponentName, &ContainerEntry, &CompEntry);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "PreserveContainerComponent: %08X:%08X not found (%r)\n", ContainerSig, ComponentName, Status));
    return Status;
  }

  ContainerHdr  = (CONTAINER_HDR *)(UINTN)ContainerEntry->HeaderCache;
  RomBase       = (UINT32)(0x100000000ULL - FlashMapPtr->RomSize);
  CompRomOffset = (ContainerEntry->Base - RomBase) + ContainerHdr->DataOffset + CompEntry->Offset;

  //
  // Overlay the component's current flash data onto the capsule buffer.
  //
  CapsuleBase   = (UINT8 *)((UINTN)ImageHdr + sizeof (EFI_FW_MGMT_CAP_IMAGE_HEADER));
  if ((CompRomOffset + CompEntry->Size) > ImageHdr->UpdateImageSize) {
    DEBUG ((DEBUG_ERROR, "Component at 0x%X size 0x%X exceeds capsule\n", CompRomOffset, CompEntry->Size));
    return EFI_INVALID_PARAMETER;
  }
  DEBUG ((DEBUG_INFO, "Preserving %08X:%08X (Offset=0x%X, Size=0x%X)\n", ContainerSig, ComponentName, CompRomOffset, CompEntry->Size));

  BiosRgnOffset = GetRomImageOffsetInBiosRegion ();
  Status = BootMediaRead (BiosRgnOffset + CompRomOffset, CompEntry->Size, CapsuleBase + CompRomOffset);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "PreserveContainerComponent: Failed to read component: %r\n", Status));
  }

  return Status;
}

/**
  Preserve a flash component during BIOS region update.

  This function reads the current content of a flash component from boot media
  and overlays it onto the capsule image buffer at the same offset, so that
  after the BIOS region is written, the component data remains unchanged.

  @param[in] Signature       Flash map signature of the component to preserve.
  @param[in] ImageHdr        Pointer to fw mgmt capsule Image header.
                             The capsule image buffer starts at ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER).

  @retval  EFI_SUCCESS       Component data preserved successfully.
  @retval  EFI_NOT_FOUND     Component not found in flash map.
  @retval  others            Error reading from boot media.
**/
EFI_STATUS
PreserveFlashRegion (
  IN  UINT32                          Signature,
  IN  EFI_FW_MGMT_CAP_IMAGE_HEADER   *ImageHdr
  )
{
  EFI_STATUS              Status;
  FLASH_MAP_ENTRY_DESC    *Entry;
  FLASH_MAP               *FlashMap;
  UINT8                   *CapsuleBase;
  UINT32                  BiosRgnOffset;

  FlashMap = GetFlashMapPtr ();
  if (FlashMap == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Non-volatile components (MRCD, VARS, UVAR) are in the non-redundant region
  // Look for the component in the primary partition.
  //
  Entry = GetComponentEntryByPartition (Signature, FALSE);
  if (Entry == NULL) {
    DEBUG ((DEBUG_INFO, "PreserveFlashRegion: Component %08X not found, skipping\n", Signature));
    return EFI_NOT_FOUND;
  }

  //
  // CapsuleBase points to the start of the BIOS image in the capsule.
  // Entry->Offset is the component's offset from the start of the ROM image.
  // Ensure the component falls within the capsule image boundary.
  //
  CapsuleBase = (UINT8 *)((UINTN)ImageHdr + sizeof (EFI_FW_MGMT_CAP_IMAGE_HEADER));
  if ((UINT32)(Entry->Offset + Entry->Size) > ImageHdr->UpdateImageSize) {
    DEBUG ((DEBUG_ERROR, "Component %08X at offset 0x%X size 0x%X exceeds capsule image\n", Signature, Entry->Offset, Entry->Size));
    return EFI_INVALID_PARAMETER;
  }
  DEBUG ((DEBUG_INFO, "Preserving %08X (Offset=0x%X, Size=0x%X)\n", Signature, Entry->Offset, Entry->Size));

  //
  // Read current flash content and overlay onto the capsule buffer.
  // The BIOS region offset accounts for when the SBL ROM image does not
  // occupy the entire BIOS region.
  //
  BiosRgnOffset = GetRomImageOffsetInBiosRegion ();
  Status = BootMediaRead (BiosRgnOffset + Entry->Offset, Entry->Size, CapsuleBase + Entry->Offset);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "PreserveFlashRegion: Failed to read %08X from flash, Status = %r\n", Signature, Status));
  }

  return Status;
}

/**
  Perform system Firmware update.

  This function will update SBL or Configuration data alone.

  @param[in] ImageHdr       Pointer to fw mgmt capsule Image header
  @param[in] FwPolicy       Fw update policy
  @param[in] CapsuleFlags   Capsule flags from firmware update header

  @retval  EFI_SUCCESS      Update successful.
  @retval  other            error occurred during firmware update
**/
EFI_STATUS
UpdateSystemFirmware (
  IN EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr,
  IN FIRMWARE_UPDATE_POLICY        FwPolicy,
  IN UINT32                        CapsuleFlags
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
    FreePool(UpdatePartition);
    return Status;
  }

  //
  // Preserve all non-volatile data regions when
  // CAPSULE_FLAG_SKIP_ALL_NON_VOLATILE_REGION is set.
  // Each region's current flash content is read and overlaid onto the capsule
  // source buffer so that after the write, the data on flash remains unchanged.
  // This flag only applies to the normal BIOS update path (not force BIOS update).
  //
  if ((CapsuleFlags & CAPSULE_FLAG_SKIP_ALL_NON_VOLATILE_REGION) != 0) {
    Status = PreserveFlashRegion (FLASH_MAP_SIG_MRCDATA, ImageHdr);
    if (EFI_ERROR (Status) && Status != EFI_NOT_FOUND) {
      DEBUG ((DEBUG_ERROR, "Failed to preserve MRC data: %r\n", Status));
    }

    Status = PreserveFlashRegion (FLASH_MAP_SIG_VARIABLE, ImageHdr);
    if (EFI_ERROR (Status) && Status != EFI_NOT_FOUND) {
      DEBUG ((DEBUG_ERROR, "Failed to preserve SBL variable: %r\n", Status));
    }

    Status = PreserveFlashRegion (FLASH_MAP_SIG_UEFIVARIABLE, ImageHdr);
    if (EFI_ERROR (Status) && Status != EFI_NOT_FOUND) {
      DEBUG ((DEBUG_ERROR, "Failed to preserve UEFI variable: %r\n", Status));
    }

    Status = PreserveContainerComponent (SIGNATURE_32 ('I', 'P', 'F', 'W'), SIGNATURE_32 ('S', 'M', 'B', 'S'), ImageHdr);
    if (EFI_ERROR (Status) && Status != EFI_NOT_FOUND) {
      DEBUG ((DEBUG_ERROR, "Failed to preserve SMBIOS: %r\n", Status));
    }
  }

  //
  // Do boot partition update.
  //
  Status = UpdateBootPartition (UpdatePartition);
  FreePool(UpdatePartition);
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
  if (UpdatePartition == NULL) {
    DEBUG ((DEBUG_ERROR, "AllocatePool failed for update partition\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  UpdateRegion                  = &UpdatePartition->FwRegion[0];

  //
  // Guard against UINT32 overflow: RomSize + CompBase may exceed MAX_UINT32
  //
  if (((UINT64)FlashMap->RomSize + (UINT64)CompBase) > MAX_UINT32) {
    DEBUG((DEBUG_ERROR, "UpdateSingleComponent: address computation overflow: RomSize=0x%x CompBase=0x%x\n",
          FlashMap->RomSize, CompBase));
    FreePool(UpdatePartition);
    return EFI_UNSUPPORTED;
  }
  UpdateRegion->ToUpdateAddress = (UINT32)(FlashMap->RomSize + CompBase);
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

  FreePool(UpdatePartition);
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
  EFI_STATUS               Status;
  UINT32                   ContainerName;
  UINT32                   ComponentName;
  UINT32                   ComponentBase;
  CONTAINER_ENTRY          *ContainerEntryPtr;
  COMPONENT_ENTRY          *ComponentEntryPtr;
  CONTAINER_HDR            *ContainerHdr;
  LOADER_COMPRESSED_HEADER *FlashCompLzHeader;
  LOADER_COMPRESSED_HEADER *CapCompLzHeader;
  UINT8                    CompInMem[sizeof(LOADER_COMPRESSED_HEADER)];
  FLASH_MAP                *FlashMapPtr;
  UINT32                   RomBase;
  UINT64                   CompAddr;

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

  if (ContainerEntryPtr->Base >= 0xF0000000) {
    //
    // Component base = Container base + data offset from container base + offset of component inside container
    //
    CompAddr = (UINT64)ContainerEntryPtr->Base + (UINT64)ContainerHdr->DataOffset + (UINT64)ComponentEntryPtr->Offset;
    if (CompAddr > MAX_UINT32) {
      DEBUG((DEBUG_ERROR, "Container component address overflow: 0x%llx\n", CompAddr));
      return EFI_UNSUPPORTED;
    }
    ComponentBase     = (UINT32)CompAddr;
    FlashCompLzHeader = (LOADER_COMPRESSED_HEADER *) (UINTN)ComponentBase;
  } else {
    // Container base is NOT the flash address, need get its flash address
    Status = GetComponentInfo(ContainerName, &ComponentBase, NULL);
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_INFO, "Component with the matching signature not found."));
      return Status;
    }
    CompAddr = (UINT64)ComponentBase + (UINT64)ContainerHdr->DataOffset + (UINT64)ComponentEntryPtr->Offset;
    if (CompAddr > MAX_UINT32) {
      DEBUG((DEBUG_ERROR, "Container component address overflow: 0x%llx\n", CompAddr));
      return EFI_UNSUPPORTED;
    }
    ComponentBase = (UINT32)CompAddr;

    // Read compressed header since container might not be MMIO mapped.
    FlashMapPtr = GetFlashMapPtr ();
    if (FlashMapPtr == NULL) {
      DEBUG((DEBUG_ERROR, "Failed to get flash map pointer for container component update\n"));
      return EFI_NOT_FOUND;
    }
    RomBase = (UINT32) (0x100000000ULL - FlashMapPtr->RomSize);
    if (ComponentBase < RomBase) {
      DEBUG((DEBUG_ERROR, "Container component address underflow: CompBase=0x%x RomBase=0x%x\n", ComponentBase, RomBase));
      return EFI_UNSUPPORTED;
    }
    Status  = BootMediaRead(ComponentBase - RomBase, sizeof(LOADER_COMPRESSED_HEADER), CompInMem);
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_INFO, "Boot Media device error, read command aborts."));
      return Status;
    }
    FlashCompLzHeader = (LOADER_COMPRESSED_HEADER *) (UINTN) CompInMem;
  }

  // Current implementation only supports compressed header.
  // Exception: Signature is zero as a mark for previously detected bad region, e.g., TCCT
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
  UINT64                    CapDataEnd;
  UINT64                    CapCompOffset;
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

  if (ImageHdr->UpdateImageSize < sizeof (CONTAINER_HDR)) {
    DEBUG((DEBUG_ERROR, "Container payload too small for container header: 0x%x\n", ImageHdr->UpdateImageSize));
    return EFI_COMPROMISED_DATA;
  }

  if ((CapContainerAddr->DataOffset < sizeof (CONTAINER_HDR)) ||
      (CapContainerAddr->DataOffset > ImageHdr->UpdateImageSize)) {
    DEBUG((DEBUG_ERROR, "Container data offset out of bounds: 0x%x\n", CapContainerAddr->DataOffset));
    return EFI_COMPROMISED_DATA;
  }

  CapDataEnd = (UINT64)CapContainerAddr->DataOffset + (UINT64)CapContainerAddr->DataSize;
  if ((CapDataEnd < CapContainerAddr->DataOffset) || (CapDataEnd > ImageHdr->UpdateImageSize)) {
    DEBUG((DEBUG_ERROR, "Container data region out of bounds: Offset=0x%x Size=0x%x Payload=0x%x\n",
          CapContainerAddr->DataOffset, CapContainerAddr->DataSize, ImageHdr->UpdateImageSize));
    return EFI_COMPROMISED_DATA;
  }

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
      if (((UINT64)CapCompEntry->Offset > CapContainerAddr->DataSize) ||
          ((UINT64)CapCompEntry->Size > ((UINT64)CapContainerAddr->DataSize - CapCompEntry->Offset))) {
        DEBUG ((DEBUG_ERROR, "Component entry out of bounds in capsule container: Offset=0x%x Size=0x%x DataSize=0x%x\n",
                CapCompEntry->Offset, CapCompEntry->Size, CapContainerAddr->DataSize));
        return EFI_COMPROMISED_DATA;
      }

      CapCompOffset = (UINT64)CapContainerAddr->DataOffset + (UINT64)CapCompEntry->Offset;
      //
      // Defense-in-depth: reject offsets that don't fit in 32 bits or exceed payload bounds
      //
      if ((CapCompOffset < CapContainerAddr->DataOffset) ||
          (CapCompOffset > MAX_UINT32) ||
          (CapCompOffset > ((UINT64)ImageHdr->UpdateImageSize - sizeof (LOADER_COMPRESSED_HEADER)))) {
        DEBUG ((DEBUG_ERROR, "Component header out of capsule bounds: Offset=0x%llx Payload=0x%x\n",
                CapCompOffset, ImageHdr->UpdateImageSize));
        return EFI_COMPROMISED_DATA;
      }

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

  if (ImageHdr->UpdateImageSize < sizeof (CDATA_BLOB)) {
    DEBUG((DEBUG_ERROR, "Config data payload too small for CDATA_BLOB header: 0x%x\n",
          ImageHdr->UpdateImageSize));
    return EFI_COMPROMISED_DATA;
  }

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
  Get cpu ucode revision.

  @retval   cpu ucode revision
**/
UINT32
EFIAPI
GetCpuUCodeRev (
  VOID
)
{
  UINT64 MsrValue;
  UINT32 UcodeRev;

  AsmWriteMsr64 (MSR_IA32_BIOS_SIGN_ID, 0LL);
  AsmCpuid (0, NULL, NULL, NULL, NULL);
  MsrValue = AsmReadMsr64 (MSR_IA32_BIOS_SIGN_ID);
  UcodeRev = RShiftU64 (MsrValue, 32) & 0xffffffff;

  return UcodeRev;
}

/**
  Perform UCODE revision check

  This function will perform revision checks for UCODE in flash and
  UCODE in capsule.

  @param[in]  ImageHdr       Pointer to fw mgmt capsule Image header

  @retval  EFI_SUCCESS      revision check successful.
  @retval  other            error occurred during firmware update
**/
EFI_STATUS
EFIAPI
CheckUCodeVersion (
  IN   EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr
)
{
  UINT32                ExistingUCodeRev;
  UINT32                NewUCodeRev;
  UINTN                 ImageBase;
  UINTN                 Offset;
  UINTN                 RemainingSize;
  CPU_MICROCODE_HEADER  *UCodeHdr;
  UINT8                 *ImageByte;
  UINT32                uCodeVer;
  UINT32                SlotSize;

  if (ImageHdr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ExistingUCodeRev = GetCpuUCodeRev();
  DEBUG((DEBUG_INFO, "Existing UCODE revision: %x\n", ExistingUCodeRev));

  // Update is only supported for platforms that slot their uCode
  SlotSize = PcdGet32 (PcdUcodeSlotSize);
  if (SlotSize == 0) {
    DEBUG((DEBUG_ERROR, "Existing image does not contain uCode slots!!\n"));
    return EFI_UNSUPPORTED;
  }

  if (SlotSize < sizeof (CPU_MICROCODE_HEADER)) {
    DEBUG((DEBUG_ERROR, "uCode slot size is too small for header: 0x%x\n", SlotSize));
    return EFI_UNSUPPORTED;
  }

  uCodeVer = 0;
  Offset = 0;
  ImageBase = (UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER);
  ImageByte = (UINT8*)(ImageBase + Offset);

  while ((Offset < ImageHdr->UpdateImageSize) && (*ImageByte != PAD_BYTE)) {
    RemainingSize = (UINTN)ImageHdr->UpdateImageSize - Offset;
    if (RemainingSize < sizeof (CPU_MICROCODE_HEADER)) {
      DEBUG((DEBUG_ERROR, "uCode header exceeds update payload bounds\n"));
      return EFI_COMPROMISED_DATA;
    }

    UCodeHdr = (CPU_MICROCODE_HEADER *)ImageByte;

    // Ensure uCode size from header does not exceed slot size
    if (UCodeHdr->TotalSize > SlotSize) {
      uCodeVer = 0;
      break;
    }

    if (UCodeHdr->UpdateRevision > uCodeVer) {
      uCodeVer = UCodeHdr->UpdateRevision;
    }

    if (((UINTN)ImageHdr->UpdateImageSize - Offset) < SlotSize) {
      Offset = (UINTN)ImageHdr->UpdateImageSize;
      break;
    }

    Offset   += SlotSize;
    ImageByte = (UINT8*)(ImageBase + Offset);
  }

  NewUCodeRev = uCodeVer;
  DEBUG((DEBUG_INFO, "New UCODE revision: %x\n", NewUCodeRev));

  if (NewUCodeRev < ExistingUCodeRev) {
    DEBUG((DEBUG_ERROR, "UCODE update revision check failed!!\n"));
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
    Status = UpdateSystemFirmware(ImageHdr, FwPolicy, 0);
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
  //
  // Guard against UINT32 overflow before adding the field offset
  //
  if (((UINT64)FwUpdStatusOffset + OFFSET_OF(FW_UPDATE_STATUS, CsmeNeedReset)) > MAX_UINT32) {
    DEBUG((DEBUG_ERROR, "ReadCsmeNeedResetFlag: FwUpdStatusOffset (0x%x) causes overflow\n", FwUpdStatusOffset));
    return CSME_NEED_RESET_INVALID;
  }
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
  //
  // Guard against UINT32 overflow before adding the field offset
  //
  if (((UINT64)FwUpdStatusOffset + OFFSET_OF(FW_UPDATE_STATUS, CsmeNeedReset)) > MAX_UINT32) {
    DEBUG((DEBUG_ERROR, "WriteCsmeNeedResetFlag: FwUpdStatusOffset (0x%x) causes overflow\n", FwUpdStatusOffset));
    return EFI_DEVICE_ERROR;
  }
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
  UINTN                 RemainingSize;
  CPU_MICROCODE_HEADER  *UCodeHdr;
  UINT8                 *ImageByte;
  UINT32                SlotSize;

  // Update is only supported for platforms that slot their uCode
  SlotSize = PcdGet32 (PcdUcodeSlotSize);
  if (SlotSize == 0) {
    DEBUG((DEBUG_ERROR, "Existing image does not contain uCode slots!!\n"));
    return EFI_UNSUPPORTED;
  }

  if (SlotSize < sizeof (CPU_MICROCODE_HEADER)) {
    DEBUG((DEBUG_ERROR, "uCode slot size is too small for header: 0x%x\n", SlotSize));
    return EFI_UNSUPPORTED;
  }

  ImageBase = (UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER);
  ImageOffset = 0;

  ImageByte = (UINT8*)(ImageBase + ImageOffset);
  while ((ImageOffset < ImageHdr->UpdateImageSize) && (*ImageByte != PAD_BYTE)) {
    RemainingSize = (UINTN)ImageHdr->UpdateImageSize - ImageOffset;
    if (RemainingSize < sizeof (CPU_MICROCODE_HEADER)) {
      DEBUG((DEBUG_ERROR, "uCode header exceeds update payload bounds\n"));
      return EFI_NO_MAPPING;
    }

    UCodeHdr = (CPU_MICROCODE_HEADER *)ImageByte;

    // Ensure patches in update image start at slot boundaries
    if (UCodeHdr->HeaderVersion != 1) {
      DEBUG((DEBUG_ERROR, "Existing uCode slots do not line up with new uCode slots!!\n"));
      return EFI_NO_MAPPING;
    }

    // Ensure total size from header does not exceed slot size
    if (UCodeHdr->TotalSize > SlotSize) {
      DEBUG((DEBUG_ERROR, "Total uCode size from header exceeds uCode slot size!!\n"));
      return EFI_NO_MAPPING;
    }

    if (((UINTN)ImageHdr->UpdateImageSize - ImageOffset) < SlotSize) {
      DEBUG((DEBUG_ERROR, "uCode slot exceeds update payload bounds\n"));
      return EFI_NO_MAPPING;
    }

    ImageOffset += SlotSize;
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
