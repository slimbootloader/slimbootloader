/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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
#include <Library/PcdLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/BootloaderCoreLib.h>
#include <RegAccess.h>
#include <Library/FileSystemLib.h>
#include <Library/GpioLib.h>
#include <Library/FirmwareUpdateLib.h>

UINT32  mFlashSize = 0;

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
  Initializes input structure for csme update driver.

  This function will initialize input structure for csme
  update driver. Since HECI functionality is avaiable to access
  only in silicon driver, this init is done in silicon package.

  @retval Pointer to intialized input data structure.
  @retval NULL if csme update is not supported.

**/
VOID *
InitCsmeUpdInputData (
  VOID
  )
{
  return NULL;
}

/**
  Get offset of Stage 1A in the capsule image

  This function will get stage 1A base address from flash map.
  Computes offset in the BIOS region from the base address.
  Then it calculates base address of stage1A in the capsule image.

  @param[in]  ImageHdr        Pointer to Fw Mgmt capsule Image header
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
  IN  EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr,
  IN  BOOLEAN    IsBackupPartition,
  OUT UINT32     *Base,
  OUT UINT32     *Size
  )
{
  return EFI_UNSUPPORTED;
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

  @param[in]  ImageHdr        Pointer to Fw Mgmt capsule Image header
  @param[in]  FwPolicy        Firmware update policy.
  @param[out] PartitionInfo   The detail information on the partition to update

  @retval  EFI_SUCCESS        Update successfully.
  @retval  others             Error happening when updating.
**/
EFI_STATUS
EFIAPI
GetFirmwareUpdateInfo (
  IN  EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr,
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
  UINT32                         CompBase;
  UINT32                         CompSize;
  UINT8                          BootPartition;
  FIRMWARE_UPDATE_PARTITION      *UpdatePartition;
  FIRMWARE_UPDATE_REGION         *UpdateRegion;
  UINT8                          Idx;
  FLASH_MAP                     *FlashMap;
  BOOLEAN                        IsBackup;
  EFI_STATUS                     Status;

  // Get current boot partition - Primary or backup blocks
  FlashMap      = GetFlashMapPtr();
  if (FlashMap == NULL) {
    return EFI_NOT_FOUND;
  }
  BootPartition = GetCurrentBootPartition ();

  // Maximum update regions
  RegionNumber = MAX_UPDATE_REGIONS;
  AllocateSize    = sizeof (FIRMWARE_UPDATE_PARTITION) + (RegionNumber - 1) * sizeof (FIRMWARE_UPDATE_REGION);
  UpdatePartition = (FIRMWARE_UPDATE_PARTITION *) AllocatePool (AllocateSize);
  ASSERT (UpdatePartition != NULL);
  UpdatePartition->RegionCount   = RegionNumber;

  if (CompareGuid(&ImageHdr->UpdateImageTypeId, &gSblCompFWUpdateImageFileGuid)) {
    //
    // Update SblComponent
    //

    // Get SBL component
    IsBackup = (FwPolicy.Fields.UpdatePartitionB == 1) ? TRUE : FALSE;
    Status = GetComponentInfoByPartition ((UINT32)ImageHdr->UpdateHardwareInstance, IsBackup, &CompBase, &CompSize);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "No SBL component found !"));
      return Status;
    }

    if (ImageHdr->UpdateImageSize > CompSize) {
      DEBUG ((DEBUG_INFO, "capsule payload size is too big for the region on flash!"));
      return EFI_UNSUPPORTED;
    }

    UpdateRegion                  = &UpdatePartition->FwRegion[0];
    UpdateRegion->ToUpdateAddress = FlashMap->RomSize + CompBase;
    UpdateRegion->UpdateSize      = ImageHdr->UpdateImageSize;
    UpdateRegion->SourceAddress   = (UINT8 *)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));
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
    UpdateRegion->SourceAddress   = (UINT8 *)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER) + TopSwapRegionOffset);

    // Redundant region
    UpdateRegion                  = &UpdatePartition->FwRegion[1];
    UpdateRegion->ToUpdateAddress = RedundantRegionOffset;
    UpdateRegion->UpdateSize      = RedundantRegionSize;
    UpdateRegion->SourceAddress   = (UINT8 *)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER) + RedundantRegionOffset);

    // Non-redundant region
    UpdateRegion                  = &UpdatePartition->FwRegion[2];
    UpdateRegion->ToUpdateAddress = NonRedundantRegionOffset;
    UpdateRegion->UpdateSize      = NonRedundantRegionSize;
    UpdateRegion->SourceAddress   = (UINT8 *)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER) + NonRedundantRegionOffset);

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
  UINT8       Data;
  EFI_STATUS  Status;

  if (mFlashSize == 0) {
    Status = BootMediaGetRegion (FlashRegionAll, NULL, &mFlashSize);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

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
PlatformEndFirmwareUpdate (
  VOID
  )
{
  UINT8       Data;
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INIT, "Firmware update Done! clear FWU flag to normal boot mode.\n"));

  if (mFlashSize == 0) {
    Status = BootMediaGetRegion (FlashRegionAll, NULL, &mFlashSize);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //
  // This is platform specific method.
  // Use 0xFFFFFFF6 to request clear FWU flag here.
  // It needs to work with QemuFwu script
  //
  Data   = 0x80;
  BootMediaWrite (0xFFFFFFF6 + mFlashSize, 1, &Data);

  return EFI_SUCCESS;
}

/**
  Platform hook point to clear firmware update trigger.

  This function is responsible for clearing firmware update trigger.

**/
VOID
EFIAPI
ClearFwUpdateTrigger (
  VOID
  )
{
  return;
}


/**
  Flash descriptor region lock

  This function will do some command buffer parsing and check
  for additional parameters

  @param[in]  CmdDataBuf    Pointer to command buffer
  @param[in]  CmdDataSize   size of command data.

  @retval  EFI_SUCCESS      Flash descriptor lock successfully.
  @retval  others           Error happening when updating.

**/
EFI_STATUS
EFIAPI
SetFlashDescriptorLock (
  IN  CHAR8      *CmdDataBuf,
  IN  UINTN      CmdDataSize
   )
{
  return EFI_UNSUPPORTED;
}


/**
  Anti Rollback Svn Commit

  @param[in]  CmdDataBuf    Pointer to command buffer.
  @param[in]  CmdDataSize   size of command data.

  @retval  EFI_SUCCESS      Svn commit successfully.
  @retval  others           Error happening when updating.

**/
EFI_STATUS
EFIAPI
SetArbSvnCommit (
   IN  CHAR8     *CmdDataBuf,
   IN  UINTN     CmdDataSize
   )
{
  return EFI_UNSUPPORTED;
}

/**
  Oem Key Revocation

  @param[in]  CmdDataBuf    Pointer to command buffer.
  @param[in]  CmdDataSize   size of command data.

  @retval  EFI_SUCCESS      Oem Key Revocation is successful.
  @retval  others           Error happening when updating.

**/
EFI_STATUS
EFIAPI
SetOemKeyRevocation (
   IN  CHAR8     *CmdDataBuf,
   IN  UINTN     CmdDataSize
   )
{
  return EFI_UNSUPPORTED;
}
