/** @file
  This file contains the implementation of FirmwareUpdateLib library.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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
#include <Library/HeciLib.h>
#include <Library/PcdLib.h>
#include <FwuBootMode.h>
#include <Library/FirmwareUpdateLib.h>
#include <Library/MediaAccessLib.h>
#include <Library/PartitionLib.h>
#include <Library/CryptoLib.h>
#include <Library/ConfigDataLib.h>
#include <ConfigDataCommonStruct.h>
#include <Library/FileSystemLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/ConfigDataLib.h>
#include <Service/PlatformService.h>
#include <Service/HeciService.h>
#include <Library/ResetSystemLib.h>
#include <ScRegs/RegsPmc.h>

#define FLASH_MAP_IN_FV_OFFSET   0xA4

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
    DEBUG ((DEBUG_ERROR, "BPDT header Signature or partition number verification failed.\n"));
    return EFI_UNSUPPORTED;
  }

  BpdtHeader->Signature = HeaderState;
  if (HeaderSize != 0) {
    *HeaderSize           = BPDT_SIZE;
  }
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
  EFI_STATUS                   Status;
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

  Status = BootMediaGetRegion (FlashRegionBios, &RgnBase, &RgnSize);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_AVAILABLE_YET;
  }

  // Search for Flash Map signature in image
  FlashMapPtr = NULL;
  Ptr         = (UINT8 *)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));
  BpOffset    = IsBackupPartition ? (RgnSize >> 1) : 0;
  for (Offset = BpOffset; Offset < ImageHdr->UpdateImageSize; Offset += SIZE_4KB) {
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
        *Base  = (UINT32)(UINTN)(Ptr + EntryDesc.Offset);
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

  @param[in]  ImageHdr        Pointer to Fw Mgmt capsule Image header
  @param[in]  FwPolicy        Firmware update policy.
  @param[out] PartitionInfo   The detail informaion on the partition to update

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
  EFI_STATUS                     Status;
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

  Status = BootMediaGetRegion (FlashRegionBios, &RgnBase, &RgnSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "GetPartitionSize Status = 0x%x\n", Status));
    return Status;
  }

  if (FwPolicy.Fields.UpdatePartitionA == 1) {
    ToUpdateAddress = 0;
  } else {
    ToUpdateAddress = RgnSize >> 1;
  }
  BiosAddress  = (UINT8 *)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER) + ToUpdateAddress);

  //
  // Mark BPDT header to RED, so if update fails in between, we will have a corrupted partition
  //
  Status = MarkBootParitionBpdt (BiosAddress, BPDT_SIGN_RED, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SetBootPartitionInvalid failed! Status = 0x%x\n", Status));
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
      DEBUG ((DEBUG_ERROR, "Could not get reserved region base from flash map\n"));
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
    DEBUG ((DEBUG_ERROR, "SetBootPartitionInvalid failed! Status = 0x%x\n", Status));
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
  UINT8         StateMachine;

  if (Partition == PrimaryPartition) {
    //
    // HECI command to clear MBP data is ignored when booting from BP2
    // reset here so that we can boot from BP1
    //
    GetStateMachineFlag(&StateMachine);
    if (StateMachine == FW_UPDATE_SM_PART_AB) {
      ResetSystem (EfiResetWarm);
      CpuDeadLoop ();
    }
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
    DEBUG ((DEBUG_ERROR, "Could not flip boot partition\n", Status));
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
PlatformEndFirmwareUpdate (
  VOID
  )
{
  EFI_STATUS        Status;
  HECI_SERVICE      *HeciService;
  PLATFORM_SERVICE  *PlatformService;

  DEBUG ((DEBUG_INFO, "Firmware update Done! clear CSE flag to normal boot mode.\n"));

  HeciService = (HECI_SERVICE *) GetServiceBySignature (HECI_SERVICE_SIGNATURE);
  if ((HeciService == NULL) || (HeciService->SimpleHeciCommand == NULL)) {
    DEBUG ((DEBUG_INFO, "Heci service unable, Could not exit firmware update mode.\n"));
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
    DEBUG ((DEBUG_INIT, "Rebooting ...\n"));
    PlatformService->ResetSystem(EfiResetCold);
  }

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
  // Clear platform firmware update trigger.
  MmioAnd32 (PMC_BASE_ADDRESS + R_PMC_BIOS_SCRATCHPAD, 0xFF00FFFF);
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
