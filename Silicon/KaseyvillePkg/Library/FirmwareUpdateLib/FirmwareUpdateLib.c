/** @file
  This file contains the implementation of FirmwareUpdateLib library.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/FirmwareUpdateLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/IoLib.h>
#include <IndustryStandard/Pci22.h>
#include <Register/UBOX_MISC.h>



/**
  Initializes input structure for csme update driver.

  This function will initialize input structure for csme
  update driver. Since HECI functionality is avaiable to access
  only in silicon driver, this init is done in silicon package.

  @retval Pointer to intialized input data structure.
  @retval NULL if csme update is not supported.

**/
VOID *
EFIAPI
InitCsmeUpdInputData (
  VOID
  )
{
  //returned NULL since GNRD does not have CSME support
  return NULL;
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
EFIAPI
SetBootPartition (
  IN BOOT_PARTITION  Partition
  )
{
  // Returned Success since GNRD does not Top swap feature which will be
  // used to switch between boot partition
  return EFI_SUCCESS;
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
  FLASH_MAP_ENTRY_DESC  *Entry;

  if ((ImageHdr == NULL) || (Base == NULL) || (Size == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // GNRD has no redundant/backup partitions - all regions are non-redundant.
  // Find Stage1A (SG1A) entry from the flash map. The entry offset gives the
  // position of the Stage1A FD within the BIOS region, which maps directly
  // to the capsule image payload.
  //
  Entry = GetComponentEntryByPartition (FLASH_MAP_SIG_STAGE1A, FALSE);
  if (Entry == NULL) {
    DEBUG ((DEBUG_ERROR, "Could not find Stage1A in flash map!\n"));
    return EFI_NOT_FOUND;
  }

  *Base = (UINT32)((UINTN)ImageHdr + sizeof (EFI_FW_MGMT_CAP_IMAGE_HEADER) + Entry->Offset);
  *Size = Entry->Size;

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
  UINT32                        AllocateSize;
  UINT32                        NonRedundantRegionSize;
  UINT32                        NonRedundantRegionOffset;
  UINT32                        CompBase;
  UINT32                        CompSize;
  FIRMWARE_UPDATE_PARTITION     *UpdatePartition;
  FIRMWARE_UPDATE_REGION        *UpdateRegion;
  UINT8                         Idx;
  FLASH_MAP                     *FlashMap;
  EFI_STATUS                    Status;

  FlashMap = GetFlashMapPtr ();
  if (FlashMap == NULL) {
    return EFI_NOT_FOUND;
  }

  AllocateSize    = sizeof (FIRMWARE_UPDATE_PARTITION) + (MAX_UPDATE_REGIONS - 1) * sizeof (FIRMWARE_UPDATE_REGION);
  UpdatePartition = (FIRMWARE_UPDATE_PARTITION *) AllocateZeroPool (AllocateSize);
  if (UpdatePartition == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  UpdatePartition->RegionCount = 0;

  if (CompareGuid (&ImageHdr->UpdateImageTypeId, &gSblCompFWUpdateImageFileGuid)) {
    //
    // Update individual SBL component
    //
    Status = GetComponentInfoByPartition ((UINT32)ImageHdr->UpdateHardwareInstance, FALSE, &CompBase, &CompSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "No SBL component found!\n"));
      FreePool (UpdatePartition);
      return Status;
    }

    if (ImageHdr->UpdateImageSize > CompSize) {
      DEBUG ((DEBUG_INFO, "Capsule payload size is too big for the region on flash!\n"));
      FreePool (UpdatePartition);
      return EFI_UNSUPPORTED;
    }

    UpdateRegion                  = &UpdatePartition->FwRegion[0];
    UpdateRegion->ToUpdateAddress = FlashMap->RomSize + CompBase;
    UpdateRegion->UpdateSize      = ImageHdr->UpdateImageSize;
    UpdateRegion->SourceAddress   = (UINT8 *)((UINTN)ImageHdr + sizeof (EFI_FW_MGMT_CAP_IMAGE_HEADER));
    UpdatePartition->RegionCount  = 1;

  } else {
    //
    // Update full bootloader.
    // GNRD has no top-swap or redundant regions - all regions are non-redundant.
    // Update the entire non-redundant region.
    //
    GetRegionInfo (NULL, NULL, &NonRedundantRegionSize);

    NonRedundantRegionOffset = FlashMap->RomSize - NonRedundantRegionSize;

    DEBUG ((DEBUG_INFO, "NonRedundantRegion Offset/Size = 0x%08X/0x%X\n",
            NonRedundantRegionOffset, NonRedundantRegionSize));

    UpdateRegion                  = &UpdatePartition->FwRegion[0];
    UpdateRegion->ToUpdateAddress = NonRedundantRegionOffset;
    UpdateRegion->UpdateSize      = NonRedundantRegionSize;
    UpdateRegion->SourceAddress   = (UINT8 *)((UINTN)ImageHdr + sizeof (EFI_FW_MGMT_CAP_IMAGE_HEADER) + NonRedundantRegionOffset);
    UpdatePartition->RegionCount  = 1;
  }

  for (Idx = 0; Idx < UpdatePartition->RegionCount; Idx++) {
    UpdateRegion = &UpdatePartition->FwRegion[Idx];
    DEBUG ((DEBUG_INFO, "Region %d\n", Idx));
    DEBUG ((DEBUG_INFO, "   ToUpdateAddress: = 0x%08x\n", UpdateRegion->ToUpdateAddress));
    DEBUG ((DEBUG_INFO, "   UpdateSize:      = 0x%08x\n", UpdateRegion->UpdateSize));
    DEBUG ((DEBUG_INFO, "   SourceAddress:   = 0x%08x\n", UpdateRegion->SourceAddress));
  }

  *PartitionInfo = UpdatePartition;
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
  UINTN                 Address;

  //
  // BIOSSCRATCHPAD3_CFG (Bus PCI_MAX_BUS-1, Dev 0, Func 2 PCI_Config 0xAC)
  // If BIT0 is set, indicates FWUpdate is triggered.
  //
  Address = MM_PCI_ADDRESS (PCI_MAX_BUS-1, UBOX_DECS_DEV_NUM, UBOX_DECS_FUNC_NUM, 0);
  MmioAnd32 (Address + R_BIOSSCRATCHPAD3_UBOX_DECS, (UINT32)~BIT0);
}


/**
  Flash descriptor region lock

  This function will do some command buffer parsing and check
  for additional parameters

  @param[in]  CmdDataBuf    Pointer to command buffer.
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
