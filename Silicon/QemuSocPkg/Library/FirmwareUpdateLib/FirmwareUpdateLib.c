/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <PiPei.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/IoLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/PcdLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/BootloaderCoreLib.h>
#include <RegAccess.h>
#include <Library/FatLib.h>
#include <Library/GpioLib.h>
#include <Library/FirmwareUpdateLib.h>

SPI_FLASH_SERVICE             *mFwuSpiService = NULL;
UINT32                         mFlashSize;

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
  return EFI_UNSUPPORTED;
}

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
  if (mFwuSpiService != NULL) {
    mFwuSpiService->SpiGetRegion (FlashRegionAll, NULL, &mFlashSize);
  }
  ASSERT ((mFwuSpiService != NULL) && (mFlashSize > 0));
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
  EFI_STATUS                     Status;

  Status = mFwuSpiService->SpiRead (FlashRegionAll, (UINT32)Address, ByteCount, Buffer);

  return Status;
}

/**
  This function writes blocks from the SPI device.

  @param[in]  Address             The block address in the FlashRegionAll to read from on the SPI.
  @param[in]  ByteCount           Size of the Buffer in bytes.
  @param[in]  Buffer              Pointer to the data to write.

  @retval EFI_SUCCESS             Write completes successfully.
  @retval others                  Device error, the command aborts abnormally.

**/
EFI_STATUS
EFIAPI
BootMediaWrite (
  IN     UINT64                  Address,
  IN     UINT32                  ByteCount,
  IN     UINT8                   *Buffer
  )
{
  EFI_STATUS                     Status;

  Status = mFwuSpiService->SpiWrite (FlashRegionAll, (UINT32)Address, ByteCount, Buffer);

  return Status;
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
  EFI_STATUS                     Status;

  Status = mFwuSpiService->SpiErase (FlashRegionAll, (UINT32)Address, ByteCount);

  return Status;
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
  EFI_STATUS                Status;
  FLASH_MAP                 *FlashMap;
  FIRMWARE_UPDATE_HEADER    *FwUpdHeader;

  if ((Base == NULL) || (Size == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  FlashMap = GetFlashMapPtr();
  if (FlashMap == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Get stage 1A base and size
  //
  Status = GetComponentInfoByPartition (FLASH_MAP_SIG_STAGE1A, IsBackupPartition, Base, Size);
  if (IsBackupPartition && (Status == EFI_NOT_FOUND)) {
    Status = GetComponentInfoByPartition (FLASH_MAP_SIG_STAGE1A, FALSE, Base, Size);
  }

  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Could not get component information from flash map \n"));
    return Status;
  }

  //
  // Convert base address to offset in the BIOS region
  //
  *Base = (UINT32)(FlashMap->RomSize - (0x100000000ULL - *Base));

  FwUpdHeader = (FIRMWARE_UPDATE_HEADER *)FwImage;
  //
  // Calculate base address of the component in the capsule image
  // Capsule image address + bios region offset + offset of the component
  //
  *Base  = (UINT32)(FwImage + FwUpdHeader->ImageOffset + *Base);

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
  @param[out] PartitionInfo   The detail information on the partition to update

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
  UINT32                         RegionNumber;
  UINT32                         AllocateSize;
  UINT32                         TopSwapRegionSize;
  UINT32                         RedundantRegionSize;
  UINT32                         NonRedundantRegionSize;
  UINT32                         TopSwapRegionOffset;
  UINT32                         RedundantRegionOffset;
  UINT32                         NonRedundantRegionOffset;
  UINT32                         CfgDataBase;
  UINT32                         CfgDataSize;
  UINT8                          BootPartition;
  FIRMWARE_UPDATE_HEADER         *Header;
  FIRMWARE_UPDATE_PARTITION      *UpdatePartition;
  FIRMWARE_UPDATE_REGION         *UpdateRegion;
  UINT8                          Idx;
  FLASH_MAP                     *FlashMap;
  BOOLEAN                        IsBackup;
  EFI_STATUS                     Status;

  // Get current boot partition - Primary or backup blocks
  FlashMap      = GetFlashMapPtr();
  BootPartition = GetCurrentBootPartition ();

  // Maximum update regions
  RegionNumber = MAX_UPDATE_REGIONS;
  AllocateSize    = sizeof (FIRMWARE_UPDATE_PARTITION) + (RegionNumber - 1) * sizeof (FIRMWARE_UPDATE_REGION);
  UpdatePartition = (FIRMWARE_UPDATE_PARTITION *) AllocatePool (AllocateSize);
  ASSERT (UpdatePartition != NULL);
  UpdatePartition->RegionCount   = RegionNumber;

  Header = (FIRMWARE_UPDATE_HEADER *)FwImage;
  if (Header->CapsuleFlags & CAPSULE_FLAGS_CFG_DATA) {
    //
    // Update CfgData
    //

    // Get CFGDATA component
    IsBackup = (BootPartition == 0) ? TRUE : FALSE;
    Status = GetComponentInfoByPartition (FLASH_MAP_SIG_CFGDATA, IsBackup, &CfgDataBase, &CfgDataSize);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "No CFGDATA component found !"));
      return Status;
    }

    if (Header->ImageSize & 0xFFF) {
      DEBUG ((DEBUG_INFO, "CFGDATA capsule payload size is not block aligned!"));
      return EFI_UNSUPPORTED;
    }

    if (Header->ImageSize > CfgDataSize) {
      DEBUG ((DEBUG_INFO, "CFGDATA capsule payload size is too big for the region on flash!"));
      return EFI_UNSUPPORTED;
    }

    UpdateRegion                  = &UpdatePartition->FwRegion[0];
    UpdateRegion->ToUpdateAddress = FlashMap->RomSize + CfgDataBase;
    UpdateRegion->UpdateSize      = Header->ImageSize;
    UpdateRegion->SourceAddress   = FwImage + Header->ImageOffset;
    UpdatePartition->RegionCount  = 1;

  } else {
    //
    // Update Bootloader
    //

    // Get Region sizes from flash map
    GetRegionInfo (&TopSwapRegionSize, &RedundantRegionSize, &NonRedundantRegionSize);


    // BIOS region layout is as below.
    //
    //  +-------------------------+
    //  +        Top Swap         +
    //  +-------------------------+
    //  +     Top Swap Backup     +
    //  +-------------------------+
    //  +    Redundant Region     +
    //  +-------------------------+
    //  + Redundant Region Backup +
    //  +-------------------------+
    //  +  Non Redundant Region   +
    //  +-------------------------+
    NonRedundantRegionOffset = \
                               FlashMap->RomSize - (TopSwapRegionSize + RedundantRegionSize) * 2 - NonRedundantRegionSize;

    TopSwapRegionOffset   = FlashMap->RomSize - TopSwapRegionSize;
    RedundantRegionOffset = TopSwapRegionOffset - TopSwapRegionSize - RedundantRegionSize;
    if (BootPartition == 0) {
      TopSwapRegionOffset   -= TopSwapRegionSize;
      RedundantRegionOffset -= RedundantRegionSize;
    }

    DEBUG ((DEBUG_INFO, "TopSwapRegion      Offset/Size = 0x%08X/0x%X\n", TopSwapRegionOffset, TopSwapRegionSize));
    DEBUG ((DEBUG_INFO, "RedundantRegion    Offset/Size = 0x%08X/0x%X\n", RedundantRegionOffset, RedundantRegionSize));
    DEBUG ((DEBUG_INFO, "NonRedundantRegion Offset/Size = 0x%08X/0x%X\n", NonRedundantRegionOffset,
            NonRedundantRegionSize));

    // Top Swap region
    UpdateRegion                  = &UpdatePartition->FwRegion[0];
    if (BootPartition == 0) {
      UpdateRegion->ToUpdateAddress = TopSwapRegionOffset;
    } else {
      UpdateRegion->ToUpdateAddress = TopSwapRegionOffset - TopSwapRegionSize;
    }
    UpdateRegion->UpdateSize      = TopSwapRegionSize;
    UpdateRegion->SourceAddress   = FwImage + Header->ImageOffset + TopSwapRegionOffset;

    // Redundant region
    UpdateRegion                  = &UpdatePartition->FwRegion[1];
    UpdateRegion->ToUpdateAddress = RedundantRegionOffset;
    UpdateRegion->UpdateSize      = RedundantRegionSize;
    UpdateRegion->SourceAddress   = FwImage + Header->ImageOffset + RedundantRegionOffset;

    // Non-redundant region
    UpdateRegion                  = &UpdatePartition->FwRegion[2];
    UpdateRegion->ToUpdateAddress = NonRedundantRegionOffset;
    UpdateRegion->UpdateSize      = NonRedundantRegionSize;
    UpdateRegion->SourceAddress   = FwImage + Header->ImageOffset + NonRedundantRegionOffset;

    if (BootPartition == 0) {
      UpdatePartition->RegionCount   = 3;
    } else {
      UpdatePartition->RegionCount   = 2;
    }
  }

  DEBUG ((DEBUG_INFO, "Boot from partition %c, update partition %c ...\n",
                      'A' + BootPartition,
                      'A' + 1 - BootPartition));

  *PartitionInfo = UpdatePartition;
  for (Idx = 0; Idx < UpdatePartition->RegionCount; Idx++) {
    UpdateRegion = &UpdatePartition->FwRegion[Idx];
    DEBUG((DEBUG_INFO, "Region %d\n", Idx));
    DEBUG((DEBUG_INFO, "   ToUpdateAddress: = 0x%08x\n", UpdateRegion->ToUpdateAddress));
    DEBUG((DEBUG_INFO, "   UpdateSize:      = 0x%08x\n", UpdateRegion->UpdateSize));
    DEBUG((DEBUG_INFO, "   SourceAddress:   = 0x%08x\n", UpdateRegion->SourceAddress));
  }

  return EFI_SUCCESS;
}

/**
  Platform hook point before starting to write a boot partition.

  This function will do some platform specific things before starting to write
  data to boot media. e.g. Send other component message it is going to do boot
  media update.

  @param[in] UpdatePartition   Pointer to the detail informaion on the partition
                               to update

  @retval   EFI_SUCCESS        Already return success for this platform.

**/
EFI_STATUS
EFIAPI
PrepareRegionsUpdate (
  IN FIRMWARE_UPDATE_PARTITION   *UpdatePartition
  )
{
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
  UINT8                   Data;

  //
  // Flip the boot partition.
  //
  if (Partition == BackupPartition) {
    //
    // Switch to back up parition - Set Top Swap
    //
    Data = 0x80;
    DEBUG ((DEBUG_INFO, "Flip Top Swap to 1\n"));
  } else if (Partition == PrimaryPartition) {
    //
    // Switch to primary parition - Clear Top Swap
    //
    Data = 0x00;
    DEBUG ((DEBUG_INFO, "Flip Top Swap to 0\n"));
  }

  //
  // This is platform specific method.
  // Use 0xFFFFFFF5 to request TS change.
  // It needs to work with QemuFwu script
  //
  BootMediaWrite (0xFFFFFFF5 + mFlashSize, 1, &Data);

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
  UINT8    Data;

  DEBUG ((EFI_D_INFO, "Firmware update Done! clear FWU flag to normal boot mode.\n"));

  //
  // This is platform specific method.
  // Use 0xFFFFFFF6 to request clear FWU flag here.
  // It needs to work with QemuFwu script
  //
  Data   = 0x80;
  BootMediaWrite (0xFFFFFFF6 + mFlashSize, 1, &Data);

  return EFI_SUCCESS;
}


