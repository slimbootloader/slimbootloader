/** @file
  Internal functions to update firmware in boot media.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
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
#include "FirmwareUpdateHelper.h"

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
  INTN          Index;
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
    Status = BootMediaRead (Address + Count, BlockLen, ReadBuffer);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "BootMediaRead.  readaddr: 0x%llx, Status = 0x%x\n", Address + Count, Status));
      goto End;
    }
    Index = CompareMem (Src + Count, ReadBuffer, BlockLen);
    if (Index == 0) {
      DEBUG ((DEBUG_INIT, "."));
      continue;
    }

    //
    // Erase the boot media
    //
    DEBUG ((DEBUG_INIT, "x"));
    Status = BootMediaErase ((UINT32) (Address + Count),  BlockLen);
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
    Index = CompareMem (Src + Count, ReadBuffer, BlockLen);
    if (Index != 0) {
      DEBUG ((DEBUG_ERROR, "Verify Error at [%x], org=0x%x, read=0x%x\n", Count + Index, Src[Count + Index],
              ReadBuffer[Index]));
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
    DEBUG ((DEBUG_INIT, "Updating 0x%08llx, Size:0x%05x\n", UpdateAddress, UpdateBlockSize));
    Status = UpdateRegionBlock (UpdateAddress, Buffer, UpdateBlockSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "\nFailed! Address=0x%08llx, Status = %r\n", UpdateAddress, Status));
      return Status;
    }
    UpdateAddress += UpdateBlockSize;
    Buffer        += UpdateBlockSize;
    UpdatedSize   += UpdateBlockSize;
    DEBUG ((DEBUG_INIT, "\nFinished   %3d%%\n", (WrittenSize + UpdatedSize) * 100 / TotalSize));
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
  FIRMWARE_UPDATE_REGION         *UpdateRegion;
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
    UpdateRegion = &UpdatePartition->FwRegion[Index];
    Status = UpdateBootRegion (UpdateRegion, WrittenSize, TotalUpdateSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "UpdateBootRegion failed! Status = 0x%x\n", Status));
      return Status;
    }
    WrittenSize += UpdateRegion->UpdateSize;
  }

  return Status;
}

/**
  Perform system Firmware update.

  This function will update SBL or Configuration data alone.

  @param[in] ImageHdr       Pointer to fw mgmt capsule Image header

  @retval  EFI_SUCCESS      Update successful.
  @retval  other            error occurred during firmware update
**/
EFI_STATUS
UpdateSystemFirmware (
  IN EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr
  )
{
  EFI_STATUS              Status;
  FIRMWARE_UPDATE_PARTITION   *UpdatePartition;
  FIRMWARE_UPDATE_POLICY  FwPolicy;

  //
  // 1. Enforce firmware update policy.
  //
  Status = EnforceFwUpdatePolicy (&FwPolicy);
  if (EFI_ERROR (Status)) {
    if (Status == EFI_ALREADY_STARTED) {
      return EFI_SUCCESS;
    }
    DEBUG((DEBUG_ERROR, "EnforceFwUpdatePolicy: Status = 0x%x\n", Status));
    return Status;
  }

  //
  // 2. Check firmware version.
  //
  Status = VerifyFwVersion (ImageHdr, FwPolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, " VerifyFwVersion failed with Status = 0x%x\n", Status));
    return Status;
  }

  //
  // 3. Get firmware update required information.
  //
  Status = GetFirmwareUpdateInfo (ImageHdr, FwPolicy, &UpdatePartition);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "GetFirmwareUpdateInfo, Status = 0x%x\n", Status));
    return Status;
  }

  //
  // 4. Do boot partition update.
  //
  Status = UpdateBootPartition (UpdatePartition);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UpdateBootPartition, Status = 0x%x\n", Status));
    return Status;
  }

  //
  // 5. After update Enforce firmware update policy
  //
  Status = AfterUpdateEnforceFwUpdatePolicy(FwPolicy);
  if (EFI_ERROR (Status)) {
    //
    // If EFI_END_OF_FILE is returned, that means SBL update is successful
    // return success to end firmware update.
    //
    if (Status != EFI_END_OF_FILE) {
      DEBUG((DEBUG_ERROR, "AfterUpdateEnforceFwUpdatePolicy failed! Status = %r\n", Status));
      return Status;
    } else {
      return EFI_SUCCESS;
    }
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

  ComponentName = (UINT32)RShiftU64 (ImageHdr->UpdateHardwareInstance, 32);
  ContainerName = (UINT32)ImageHdr->UpdateHardwareInstance;

  Status = LocateComponentEntry (ContainerName, ComponentName, &ContainerEntryPtr, &ComponentEntryPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Container component update failed with status: %r \n", Status));
    return Status;
  }

  //
  // Update the component
  //
  ContainerHdr = (CONTAINER_HDR *)(UINTN)ContainerEntryPtr->HeaderCache;
  //
  // Component base = Container base + data offset from container base + offset of component inside container
  //
  ComponentBase = ContainerEntryPtr->Base + ContainerHdr->DataOffset + ComponentEntryPtr->Offset;
  Status = UpdateSingleComponent (ComponentBase, ComponentEntryPtr->Size, ImageHdr);

  return Status;
}

/**
  Perform Slim Bootloader component update.

  This function will try to locate component in the flash map,
  if found, will update the component.

  @param[in] ImageHdr       Pointer to fw mgmt capsule Image header

  @retval  EFI_SUCCESS      Update successful.
  @retval  other            error occurred during firmware update
**/
EFI_STATUS
UpdateSblComponent (
  IN EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr
  )
{
  EFI_STATUS    Status;
  FLASH_MAP_ENTRY_DESC  *Entry;

  Status = EFI_NOT_FOUND;

  DEBUG ((DEBUG_INFO, "UpdateSblComponent : %x \n", (UINT32)ImageHdr->UpdateHardwareInstance));

  if ((UINT32)RShiftU64 (ImageHdr->UpdateHardwareInstance, 32)){
    //
    // Upper 4 bytes are not null, this is a container update.
    //
    DEBUG ((DEBUG_INFO, "Container component update requested! \n"));
    Status = UpdateContainerComp (ImageHdr);
    return Status;
  }
  //
  // This is a SBL component update, check if it is a redundant component
  //
  Entry = GetComponentEntryByPartition((UINT32)ImageHdr->UpdateHardwareInstance, TRUE);
  if (Entry == NULL) {
    return EFI_NOT_FOUND;
  }

  if ((Entry->Flags & FLASH_MAP_FLAGS_NON_REDUNDANT_REGION) != 0){
    DEBUG ((DEBUG_INFO, "Non redundant component update requested! \n"));
    Status = UpdateNonRedundantComp(ImageHdr);
  } else if ((Entry->Flags & FLASH_MAP_FLAGS_REDUNDANT_REGION) != 0) {
    DEBUG ((DEBUG_INFO, "Redundant component update requested! \n"));
    Status = UpdateSystemFirmware(ImageHdr);
  }

  return Status;
}
