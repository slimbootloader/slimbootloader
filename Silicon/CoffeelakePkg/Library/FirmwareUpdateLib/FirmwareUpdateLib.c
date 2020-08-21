/** @file
  This file contains the implementation of FirmwareUpdateLib library.

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <PiPei.h>
#include <RegAccess.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <Service/SpiFlashService.h>
#include <Library/FirmwareUpdateLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Library/PchPcrLib.h>
#include <Library/HeciLib.h>
#include <CsmeUpdateDriver.h>

SPI_FLASH_SERVICE   *mFwuSpiService = NULL;

#define FWU_BOOT_MODE_OFFSET   0x40
#define FWU_BOOT_MODE_VALUE    0x5A

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
  IN     UINT64   Address,
  IN     UINT32   ByteCount,
  OUT    UINT8    *Buffer
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
  IN     UINT64   Address,
  IN     UINT32   ByteCount
  )
{
  return mFwuSpiService->SpiErase (FlashRegionBios, (UINT32)Address, ByteCount);
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
  CSME_UPDATE_DRIVER_INPUT    *CsmeUpdDriverInput;

  CsmeUpdDriverInput = (CSME_UPDATE_DRIVER_INPUT *)AllocateZeroPool (sizeof(CSME_UPDATE_DRIVER_INPUT));

  if (CsmeUpdDriverInput != NULL) {
    CsmeUpdDriverInput->AllocatePool     = (VOID *)((UINTN)AllocatePool);
    CsmeUpdDriverInput->AllocateZeroPool = (VOID *)((UINTN)AllocateZeroPool);
    CsmeUpdDriverInput->FreePool         = (VOID *)((UINTN)FreePool);
    CsmeUpdDriverInput->CopyMem          = (VOID *)((UINTN)CopyMem);
    CsmeUpdDriverInput->SetMem           = (VOID *)((UINTN)SetMem);
    CsmeUpdDriverInput->CompareMem       = (VOID *)((UINTN)CompareMem);
    CsmeUpdDriverInput->Stall            = (VOID *)((UINTN)MicroSecondDelay);
    CsmeUpdDriverInput->PciRead          = (VOID *)((UINTN)PciReadBuffer);
    CsmeUpdDriverInput->HeciReadMessage  = (VOID *)((UINTN)HeciReceive);
    CsmeUpdDriverInput->HeciSendMessage  = (VOID *)((UINTN)HeciSend);
    CsmeUpdDriverInput->HeciReset        = (VOID *)((UINTN)ResetHeciInterface);
  }

  return CsmeUpdDriverInput;
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
  UINT32    TopSwapReg;
  UINT32    Data32;

  TopSwapReg = PCH_PCR_ADDRESS(PID_RTC_HOST, R_RTC_PCR_BUC);
  Data32    = MmioRead32 (TopSwapReg);
  DEBUG ((DEBUG_INFO, "TopSwapReg=0x%x, Data32=0x%x\n", TopSwapReg, Data32));

  if (Partition == BackupPartition) {
    //
    // Switch to back up partition - Set Top Swap
    //
    Data32 |= BIT0;
  } else if (Partition == PrimaryPartition) {
    //
    // Switch to primary partition - Clear Top Swap
    //
    Data32 &= ~BIT0;
  }

  MmioWrite32 (TopSwapReg, Data32);
  DEBUG ((DEBUG_INFO, "write Data32=0x%x\n", Data32));
  Data32 = MmioRead32 (TopSwapReg);

  DEBUG ((DEBUG_INFO, "Read it to ensure data is written. Data32=0x%x\n", Data32));

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
  EFI_STATUS                Status;
  FLASH_MAP                 *FlashMap;

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

  //
  // Calculate base address of the component in the capsule image
  // Capsule image address + bios region offset + offset of the component
  //
  *Base  = (UINT32)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER) + *Base);

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
  UINT32                        TopSwapRegionSize;
  UINT32                        RedundantRegionSize;
  UINT32                        NonRedundantRegionSize;
  UINT32                        TopSwapRegionOffset;
  UINT32                        RedundantRegionOffset;
  UINT32                        NonRedundantRegionOffset;
  UINT32                        CompBase;
  UINT32                        CompSize;
  FIRMWARE_UPDATE_PARTITION     *UpdatePartition;
  FIRMWARE_UPDATE_REGION        *UpdateRegion;
  UINT8                         Idx;
  FLASH_MAP                     *FlashMap;
  BOOLEAN                       IsBackup;
  EFI_STATUS                    Status;

  TopSwapRegionOffset       = 0;
  RedundantRegionOffset     = 0;
  NonRedundantRegionOffset  = 0;

  FlashMap = GetFlashMapPtr();
  if (FlashMap == NULL) {
    return EFI_NOT_FOUND;
  }

  AllocateSize    = sizeof (FIRMWARE_UPDATE_PARTITION) + (MAX_UPDATE_REGIONS - 1) * sizeof(FIRMWARE_UPDATE_REGION);
  UpdatePartition = (FIRMWARE_UPDATE_PARTITION *) AllocateZeroPool(AllocateSize);
  ASSERT (UpdatePartition != NULL);

  UpdatePartition->RegionCount   = 0;

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

    if (FwPolicy.Fields.UpdatePartitionB == 1) {
      TopSwapRegionOffset   -= TopSwapRegionSize;
      RedundantRegionOffset -= RedundantRegionSize;
    }

    DEBUG ((DEBUG_INFO, "TopSwapRegion      Offset/Size = 0x%08X/0x%X\n", TopSwapRegionOffset, TopSwapRegionSize));
    DEBUG ((DEBUG_INFO, "RedundantRegion    Offset/Size = 0x%08X/0x%X\n", RedundantRegionOffset, RedundantRegionSize));
    DEBUG ((DEBUG_INFO, "NonRedundantRegion Offset/Size = 0x%08X/0x%X\n", NonRedundantRegionOffset,
            NonRedundantRegionSize));

    //
    // Top Swap region
    //
    UpdateRegion                  = &UpdatePartition->FwRegion[0];
    UpdateRegion->ToUpdateAddress = TopSwapRegionOffset;
    UpdateRegion->UpdateSize      = TopSwapRegionSize;
    UpdateRegion->SourceAddress   = (UINT8 *)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER)+ (UINTN)UpdateRegion->ToUpdateAddress);
    UpdatePartition->RegionCount += 1;

    //
    // Redundant region
    //
    UpdateRegion                  = &UpdatePartition->FwRegion[1];
    UpdateRegion->ToUpdateAddress = RedundantRegionOffset;
    UpdateRegion->UpdateSize      = RedundantRegionSize;
    UpdateRegion->SourceAddress   = (UINT8 *)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER) + (UINTN)UpdateRegion->ToUpdateAddress);
    UpdatePartition->RegionCount += 1;

    if ((FwPolicy.Fields.StateMachine == FW_UPDATE_SM_PART_A) ||
        (FwPolicy.Fields.StateMachine == FW_UPDATE_SM_PART_B)) {
      //
      // Non-redundant region
      //
      UpdateRegion                  = &UpdatePartition->FwRegion[2];
      UpdateRegion->ToUpdateAddress = NonRedundantRegionOffset;
      UpdateRegion->UpdateSize      = NonRedundantRegionSize;
      UpdateRegion->SourceAddress   = (UINT8 *)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER) + NonRedundantRegionOffset);
      UpdatePartition->RegionCount += 1;
    }
  }

  for (Idx = 0; Idx < UpdatePartition->RegionCount; Idx++) {
    UpdateRegion = &UpdatePartition->FwRegion[Idx];
    DEBUG((DEBUG_INFO, "Region %d\n", Idx));
    DEBUG((DEBUG_INFO, "   ToUpdateAddress: = 0x%08x\n", UpdateRegion->ToUpdateAddress));
    DEBUG((DEBUG_INFO, "   UpdateSize:      = 0x%08x\n", UpdateRegion->UpdateSize));
    DEBUG((DEBUG_INFO, "   SourceAddress:   = 0x%08x\n", UpdateRegion->SourceAddress));
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
  Platform hook point to clear firmware update trigger.

  This function is responsible for clearing firmware update trigger.

**/
VOID
EFIAPI
ClearFwUpdateTrigger (
  VOID
  )
{
  IoAnd32(ACPI_BASE_ADDRESS + R_ACPI_IO_OC_WDT_CTL, 0xFF00FFFF);
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
