/** @file
This driver is to update firmware in boot media.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Guid/SystemTableInfoGuid.h>
#include <Guid/BootLoaderVersionGuid.h>
#include <Library/HobLib.h>
#include <Library/CryptoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PayloadLib.h>
#include <Library/PayloadMemoryAllocationLib.h>
#include <Guid/MemoryMapInfoGuid.h>
#include <Guid/LoaderPlatformInfoGuid.h>
#include <Library/ResetSystemLib.h>
#include <Library/SecureBootLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/LiteFvLib.h>
#include <Library/FirmwareUpdateLib.h>
#include <Guid/SystemResourceTable.h>

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
UpdateRegionLessthan4k (
  IN  UINT64    Address,
  IN  VOID      *Buffer,
  IN  UINT32    Length
  )
{
  EFI_STATUS    Status;
  UINT8         ReadBuffer[0x1000];
  UINT32        Index;
  UINT8         *Src;

  if (Length == 0) {
    return EFI_SUCCESS;
  }

  Src = (UINT8 *)Buffer;

  //
  // Read, compare, write, read, compare
  //
  Status = BootMediaRead (Address, Length, ReadBuffer);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "BootMediaRead.  readaddr: 0x%llx, Status = 0x%x\n", Address, Status));
    return Status;
  }
  Index = CompareMem (Src, ReadBuffer, Length);
  if (Index == 0) {
    DEBUG ((DEBUG_INIT, "."));
    return EFI_SUCCESS;
  }

  //
  // Write to the boot media
  //
  Status = BootMediaWrite ((UINT32) (Address),  Length, Src);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR: in BootDeviceWrite. Status = 0x%x\n", Status));
    return Status;
  }

  //
  // Verify the written data
  //
  Status = BootMediaRead (Address, Length, ReadBuffer);
  Index = CompareMem (Src, ReadBuffer, Length);
  if (Index != 0) {
    DEBUG ((DEBUG_ERROR, "Verify Error at [%x], org=0x%x, read=0x%x\n", Index, Src[Index],
            ReadBuffer[Index]));
    return EFI_DEVICE_ERROR;
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
  UINT8         ReadBuffer[0x1000];
  UINT32        Count;
  UINT32        Index;
  UINT32        BlockLen;
  UINT8         *Src;

  if (Length == 0) {
    return EFI_SUCCESS;
  }

  BlockLen   = 0x1000;
  if (((Address & (BlockLen - 1)) > 0) || ((Length & (BlockLen - 1)) > 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Src = (UINT8 *)Buffer;

  //
  // Read, compare, erase, write, read, compare
  //
  for (Count = 0; Count < Length; Count += BlockLen) {
    Status = BootMediaRead (Address + Count, BlockLen, ReadBuffer);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "BootMediaRead.  readaddr: 0x%llx, Status = 0x%x\n", Address + Count, Status));
      return Status;
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
      return Status;
    }

    //
    // Write to the boot media
    //
    Status = BootMediaWrite ((UINT32) (Address + Count),  BlockLen, Src + Count);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "ERROR: in BootDeviceWrite. Status = 0x%x\n", Status));
      return Status;
    }

    //
    // Verify the written data
    //
    Status = BootMediaRead (Address + Count, BlockLen, ReadBuffer);
    Index = CompareMem (Src + Count, ReadBuffer, BlockLen);
    if (Index != 0) {
      DEBUG ((DEBUG_ERROR, "Verify Error at [%x], org=0x%x, read=0x%x\n", Count + Index, Src[Count + Index],
              ReadBuffer[Index]));
      return EFI_DEVICE_ERROR;
    }
  }

  return EFI_SUCCESS;
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
    if (UpdateRegion->UpdateSize < SIZE_4KB) {
      Status = UpdateRegionLessthan4k (UpdateAddress, Buffer, UpdateBlockSize);
    } else {
      Status = UpdateRegionBlock (UpdateAddress, Buffer, UpdateBlockSize);
    }
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
  Verify the firmware version to make sure it is no less than current firmware version.

  @param[in]  Stage1ABase   Pointer to stage 1A base.
  @param[out] Version       Pointer to version of the firmware

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
GetVersionfromFv (
  IN  UINT32              *Stage1ABase,
  OUT BOOT_LOADER_VERSION **Version
  )
{
  EFI_STATUS                  Status;
  EFI_FFS_FILE_HEADER         *FfsFile;
  EFI_FIRMWARE_VOLUME_HEADER  *FvHeader;

  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)(*Stage1ABase);
  //
  // Stage 1A FD has FSPT FV first, so move on to the next FV
  //
  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)((UINT32)FvHeader + (UINT32)FvHeader->FvLength);

  //
  // Get version info FFS from FV
  //
  Status = GetFfsFileByName(FvHeader,&gBootLoaderVersionFileGuid,&FfsFile);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "GetFfsFileByName: %r\n", Status));
    return Status;
  }

  //
  // Raw section in version info FFS has version information
  //
  Status = GetSectionByType(FfsFile, EFI_SECTION_RAW, 0, (VOID *)Version);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "GetSectionByType: %r\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Verify the firmware version to make sure it is no less than current firmware version.

  @param[in] ImageHdr     Pointer to the fw mgmt capsule image header
  @param[in] FwPolicy     Firmware update policy.

  @retval  EFI_SUCCESS    The operation completed successfully.
  @retval  others         There is error happening.
**/
EFI_STATUS
VerifyFwVersion (
  IN  EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr,
  IN  FIRMWARE_UPDATE_POLICY  FwPolicy
  )
{
  UINT32                CompBase;
  UINT32                CompSize;
  BOOT_LOADER_VERSION   *CurrentBlVersion;
  BOOT_LOADER_VERSION   *CapsuleBlVersion;
  EFI_STATUS            Status;

  CurrentBlVersion = NULL;
  CapsuleBlVersion = NULL;
  //
  // Get base address of Stage 1A from current firmware
  //
  Status = EFI_INVALID_PARAMETER;
  if (FwPolicy.Fields.UpdatePartitionB == 0x1) {
    Status = GetComponentInfoByPartition(FLASH_MAP_SIG_STAGE1A, FALSE, &CompBase, &CompSize);
  } else if (FwPolicy.Fields.UpdatePartitionA == 0x1) {
    Status = GetComponentInfoByPartition(FLASH_MAP_SIG_STAGE1A, TRUE, &CompBase, &CompSize);
  }

  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "GetComponentInfoByPartition: %r\n", Status));
    return Status;
  }

  Status = GetVersionfromFv (&CompBase, &CurrentBlVersion);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "GetVersionfromFv: %r\n", Status));
    return Status;
  }

  //
  // Get base address of Stage 1A in capsule Image
  //
  if (FwPolicy.Fields.UpdatePartitionB == 0x1) {
    Status = PlatformGetStage1AOffset(ImageHdr, FALSE, &CompBase, &CompSize);
  } else if (FwPolicy.Fields.UpdatePartitionA == 0x1) {
    Status = PlatformGetStage1AOffset(ImageHdr, TRUE, &CompBase, &CompSize);
  }
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "PlatformGetStage1AOffset: %r\n", Status));
    return Status;
  }

  Status = GetVersionfromFv (&CompBase, &CapsuleBlVersion);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "GetVersionfromFv: %r\n", Status));
    return Status;
  }

  //
  // Update last update version to the version we are about to update
  //
  Status = UpdateStatus(&ImageHdr->UpdateImageTypeId, \
                        (CapsuleBlVersion->ImageVersion.ProjMajorVersion << 8) | CapsuleBlVersion->ImageVersion.ProjMinorVersion, \
                        0xFFFFFFFF);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Updating status to reserved region failed: %r\n", Status));
    return Status;
  }

  if (CapsuleBlVersion->ImageVersion.SecureVerNum >= CurrentBlVersion->ImageVersion.SecureVerNum) {
    return EFI_SUCCESS;
  }

  DEBUG((DEBUG_ERROR, "Antirollback - Could not rollback to version %x from current version: %x\n", \
         CapsuleBlVersion->ImageVersion.SecureVerNum, CurrentBlVersion->ImageVersion.SecureVerNum));

  return EFI_INCOMPATIBLE_VERSION;
}

/**
  Get state machine flag from flash.

  This function will get state machine flag from the bootloader reserved region
  First byte in the booloader reserved region is state machine flag

  @param[in, out] StateMachine  Pointer to state machine flag byte.

**/
VOID
GetStateMachineFlag (
  IN OUT UINT8    *StateMachine
)
{
  EFI_STATUS        Status;
  UINT32            FwUpdStatusOffset;
  FW_UPDATE_STATUS  FwUpdStatus;

  //
  // Read from the reserved region and return state machine
  //
  FwUpdStatusOffset = PcdGet32(PcdFwUpdStatusBase);
  Status = BootMediaRead (FwUpdStatusOffset, sizeof(FW_UPDATE_STATUS), (UINT8 *)&FwUpdStatus);
  if (EFI_ERROR (Status)) {
    *StateMachine = FW_UPDATE_SM_INIT;
  } else {
    *StateMachine = FwUpdStatus.StateMachine;
  }
}

/**
  Set state machine flag in flash.

  This function will set state machine flag in the bootloader reserved region
  First byte in the booloader reserved region is state machine flag

  ----------------------------------------------------------------------------------------
  |  SM                            |             Operation                               |
  ----------------------------------------------------------------------------------------
  |  FW_UPDATE_SM_INIT             | Clear whole region, Preserve sig, lenth and version |
  |                                  Also preserve update status of all firmwares        |
  |  FW_UPDATE_SM_CAP_PROCESSING   | Clear whole region, Preserve FW_UPDATE_STATUS and   |
  |                                  update state machine to FW_UPDATE_SM_CAP_PROCESSING |
  |  Anything else                 | update state machine                                |
  ----------------------------------------------------------------------------------------

  @param[in] StateMachine     State machine flag byte.

  @retval  EFI_SUCCESS        State machine flag set.
  @retval  others             Error while setting state machine flag.
**/
EFI_STATUS
SetStateMachineFlag (
  IN UINT8    StateMachine
)
{
  EFI_STATUS              Status;
  UINT32                  UpdateSize;
  UINT32                  FwUpdStatusOffset;
  FW_UPDATE_STATUS        FwUpdStatus;
  FW_UPDATE_COMP_STATUS   FwUpdCompStatus[MAX_FW_COMPONENTS];

  DEBUG((DEBUG_INIT, "Set next FWU state: 0x%02X\n", StateMachine));

  FwUpdStatusOffset = PcdGet32(PcdFwUpdStatusBase);

  //
  // Read the reserved region structure to process state machine
  //
  Status = BootMediaRead (FwUpdStatusOffset, sizeof(FW_UPDATE_STATUS), (UINT8 *)&FwUpdStatus);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BootMediaRead.  FwUpdStatusOffset: 0x%llx, Status = 0x%x\n", FwUpdStatusOffset, Status));
    return Status;
  }

  //
  // If we are to clear state machine, Wipe the whole region and preserve
  // the last update status and version for all firmwares
  //
  // If we are to set the capsule to processing, then this is start of the firmware update
  // in this case, clear the whole region.
  //
  if ((StateMachine == FW_UPDATE_SM_INIT) || (StateMachine == FW_UPDATE_SM_CAP_PROCESSING)){
    if (StateMachine == FW_UPDATE_SM_INIT) {
      Status = BootMediaRead((FwUpdStatusOffset + sizeof(FW_UPDATE_STATUS)),\
                              MAX_FW_COMPONENTS * sizeof(FW_UPDATE_COMP_STATUS), (UINT8 *)&FwUpdCompStatus);
      if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "BootMediaRead failed with status %r\n", Status));
        return Status;
      }
    }

    Status = BootMediaErase (FwUpdStatusOffset, EFI_PAGE_SIZE);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "BootMediaErase failed with status %r\n", Status));
      return Status;
    }
  }

  //
  // Set desired state machine
  //
  FwUpdStatus.StateMachine = StateMachine;

  if (StateMachine == FW_UPDATE_SM_INIT) {
    //
    // If we are resetting state machine, just preserve signature, length and version
    // last update status and version for all the firmwares
    //
    UpdateSize = sizeof(UINT64);

    //
    // Write back the firware update status
    //
    Status = BootMediaWrite((FwUpdStatusOffset + sizeof(FW_UPDATE_STATUS)),\
                            MAX_FW_COMPONENTS * sizeof(FW_UPDATE_COMP_STATUS), (UINT8 *)&FwUpdCompStatus);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "BootMediaWrite failed with status %r\n", Status));
      return Status;
    }

  } else {
    UpdateSize = sizeof(FW_UPDATE_STATUS);
  }

  Status = BootMediaWrite (FwUpdStatusOffset, UpdateSize, (UINT8 *)&FwUpdStatus);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BootMediaWrite.  readaddr: 0x%llx, Status = 0x%x\n", FwUpdStatusOffset, Status));
    return Status;
  }

  return Status;
}

/**
  This function will enforce firmware update policy.

  Firmware update policy

  ----------------------------------------------------------
  |  SM   |   TS   |             Operation                 |
  ----------------------------------------------------------
  |  FF   |    0   | Set SM to FE, Set TS and reboot       |
  |  FF   |    1   | Set SM to FD, clear TS and reboot     |
  |  FE   |    0   | Set TS and reboot                     |
  |  FE   |    1   | Set SM to FC, clear TS and reboot     |
  |  FD   |    0   | Set SM to FC, reboot                  |
  |  FD   |    1   | clear TS and reboot                   |
  |  FC   |    0   | Clear IBB signal,Set SM to FF, reboot |
  |  FC   |    1   | Clear IBB signal,Set SM to FF, reboot |
  ----------------------------------------------------------

  @param[in][out] FwPolicy    Pointer to Firmware update policy.

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
EnforceFwUpdatePolicy (
  IN FIRMWARE_UPDATE_POLICY   *FwPolicy
 )
{
  UINT8                   StateMachine;
  BOOLEAN                 ResetRequired;
  LOADER_PLATFORM_INFO    *LoaderInfo;

  ResetRequired = FALSE;

  //
  // Get boot partition based on top swap flag
  //
  LoaderInfo = GetLoaderPlatformInfoPtr ();
  if (LoaderInfo == NULL) {
    DEBUG((DEBUG_ERROR, "Could not get loader platform information \n"));
    return EFI_NOT_FOUND;
  }

  //
  // Get State machine flag
  //
  GetStateMachineFlag (&StateMachine);
  DEBUG((DEBUG_INIT, "Get current FWU state: 0x%02X\n", StateMachine));

  FwPolicy->Data = 0;

  switch (StateMachine) {
  case FW_UPDATE_SM_CAP_PROCESSING:
    if (LoaderInfo->BootPartition == FW_UPDATE_PARTITION_A) {
      FwPolicy->Fields.StateMachine       = FW_UPDATE_SM_PART_A;
      FwPolicy->Fields.UpdatePartitionB   = 0x1;
      FwPolicy->Fields.SwitchtoBackupPart = 0x1;
      FwPolicy->Fields.Reboot             = 0x1;
    } else if (LoaderInfo->BootPartition == FW_UPDATE_PARTITION_B) {
      FwPolicy->Fields.StateMachine       = FW_UPDATE_SM_PART_B;
      FwPolicy->Fields.UpdatePartitionA   = 0x1;
      FwPolicy->Fields.SwitchtoBackupPart = 0;
      FwPolicy->Fields.Reboot             = 0x1;
    }
    break;

  case FW_UPDATE_SM_PART_A:
    if (LoaderInfo->BootPartition == FW_UPDATE_PARTITION_A){
      SetBootPartition (BackupPartition);
      ResetRequired = TRUE;
    } else if (LoaderInfo->BootPartition == FW_UPDATE_PARTITION_B){
      FwPolicy->Fields.StateMachine       = FW_UPDATE_SM_PART_AB;
      FwPolicy->Fields.UpdatePartitionA   = 0x1;
      FwPolicy->Fields.SwitchtoBackupPart = 0;
      FwPolicy->Fields.Reboot             = 0x1;
    }
    break;

  case FW_UPDATE_SM_PART_B:
    if (LoaderInfo->BootPartition == FW_UPDATE_PARTITION_A){
      FwPolicy->Fields.StateMachine       = FW_UPDATE_SM_PART_AB;
      FwPolicy->Fields.UpdatePartitionB   = 0x1;
      FwPolicy->Fields.SwitchtoBackupPart = 0;
      FwPolicy->Fields.Reboot             = 0x1;
    } else if (LoaderInfo->BootPartition == FW_UPDATE_PARTITION_B){
      SetBootPartition (PrimaryPartition);
      ResetRequired = TRUE;
    }
    break;

  case FW_UPDATE_SM_PART_AB:
    if (LoaderInfo->BootPartition == FW_UPDATE_PARTITION_A){
      //
      // This return would end the firmware update
      // but before retuning would update status and version
      // in the reserved region.
      //
      return EFI_ALREADY_STARTED;
    } else if (LoaderInfo->BootPartition == FW_UPDATE_PARTITION_B) {
      SetBootPartition (PrimaryPartition);
      ResetRequired = TRUE;
    }
    break;
  }

  if (ResetRequired) {
    DEBUG((DEBUG_ERROR, "Reset required to proceed with the firmware update.\n"));
    ResetSystem (EfiResetCold);
    CpuDeadLoop ();
  }

  return EFI_SUCCESS;
}

/**
  This function will enforce firmware update policy after
  partition update is successful.

  After update firmware update policy

  @param[in] FwPolicy         Firmware update policy.

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
AfterUpdateEnforceFwUpdatePolicy (
  IN FIRMWARE_UPDATE_POLICY   FwPolicy
 )
{
  EFI_STATUS    Status;

  if ((FwPolicy.Fields.StateMachine & 0xFF) >= FW_UPDATE_SM_PART_AB) {
    Status = SetStateMachineFlag ((UINT8)FwPolicy.Fields.StateMachine);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "Set state machine flag failed with status: %r\n", Status));
      return Status;
    }
  }

  if (FwPolicy.Fields.SwitchtoBackupPart == 1) {
    Status = SetBootPartition (BackupPartition);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "Set top swap bit failed with status: %r\n", Status));
      return Status;
    }
  } else if (FwPolicy.Fields.SwitchtoBackupPart == 0) {
    Status = SetBootPartition (PrimaryPartition);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "Clearing top swap bit failed with status: %r\n", Status));
      return Status;
    }
  }

  if (FwPolicy.Fields.Reboot == 1) {
    DEBUG((DEBUG_ERROR, "Reset required to proceed with the firmware update.\n"));
    ResetSystem (EfiResetWarm);
    CpuDeadLoop ();
  }

  return EFI_SUCCESS;
}

/**
  This function will be called after the firmware update is complete.
  This function will update firmware update status structure in reserved region

  @param[in] ImageHdr           Pointer to Fw update image guid
  @param[in] LastAttemptVersion Version of last firmware update attempted.
  @param[in] LastAttemptStatus  Status of last firmware update attempted.

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
UpdateStatus (
  IN EFI_GUID   *ImageId,
  IN UINT16     LastAttemptVersion,
  IN EFI_STATUS LastAttemptStatus
 )
{
  UINT8                   Count;
  EFI_STATUS              Status;
  UINT32                  FwUpdStatusOffset;
  UINT32                  ByteOffset;
  FW_UPDATE_COMP_STATUS   FwUpdCompStatus[MAX_FW_COMPONENTS];

  FwUpdStatusOffset = PcdGet32(PcdFwUpdStatusBase);

  //
  // Read all the component structures
  //
  Status = BootMediaRead ((FwUpdStatusOffset + sizeof(FW_UPDATE_STATUS)), \
                          MAX_FW_COMPONENTS * sizeof(FW_UPDATE_COMP_STATUS), (UINT8 *)&FwUpdCompStatus);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BootMediaRead. offset: 0x%llx, Status = 0x%x\n", (FwUpdStatusOffset + sizeof(FW_UPDATE_STATUS)), Status));
    return Status;
  }

  //
  // Find the required component to update status
  //
  for (Count = 0; Count < MAX_FW_COMPONENTS; Count ++) {
    if (CompareGuid(&(FwUpdCompStatus[Count].FirmwareId), ImageId) == TRUE) {
      DEBUG((DEBUG_VERBOSE, "FOund the component to update status\n"));
      break;
    }
  }

  if (Count == MAX_FW_COMPONENTS) {
    DEBUG ((DEBUG_ERROR, "Could not find the component to update status\n"));
    return EFI_NOT_FOUND;
  }

  if (LastAttemptVersion != 0) {
    FwUpdCompStatus[Count].LastAttemptVersion = LastAttemptVersion;
  }

  if (LastAttemptStatus != 0xFFFFFFFF) {
    FwUpdCompStatus[Count].UpdatePending = FW_UPDATE_IMAGE_UPDATE_DONE;
    if (LastAttemptStatus == EFI_SUCCESS) {
      FwUpdCompStatus[Count].LastAttemptStatus = LAST_ATTEMPT_STATUS_SUCCESS;
    } else if (LastAttemptStatus == EFI_INCOMPATIBLE_VERSION) {
      FwUpdCompStatus[Count].LastAttemptStatus = LAST_ATTEMPT_STATUS_ERROR_INCORRECT_VERSION;
    } else if (LastAttemptStatus == EFI_OUT_OF_RESOURCES) {
      FwUpdCompStatus[Count].LastAttemptStatus = LAST_ATTEMPT_STATUS_ERROR_INSUFFICIENT_RESOURCES;
    } else if ((LastAttemptStatus == EFI_SECURITY_VIOLATION) || (LastAttemptStatus == EFI_COMPROMISED_DATA)){
      //
      // If we get security violation, at this point we do not know
      // version from capsule, instead of keeping the existing version
      // reset it back to 0
      //
      FwUpdCompStatus[Count].LastAttemptVersion = 0;
      FwUpdCompStatus[Count].LastAttemptStatus = LAST_ATTEMPT_STATUS_ERROR_AUTH_ERROR;
    } else {
      FwUpdCompStatus[Count].LastAttemptVersion = 0;
      FwUpdCompStatus[Count].LastAttemptStatus = LAST_ATTEMPT_STATUS_ERROR_UNSUCCESSFUL;
    }
  }

  //
  // Update a field at a time, if we loose power in between, we can still have control
  //
  ByteOffset = COMP_STATUS_OFFSET(FwUpdStatusOffset, Count) + OFFSET_OF(FW_UPDATE_COMP_STATUS, LastAttemptVersion);
  Status = BootMediaWrite(ByteOffset, sizeof(UINT32), (UINT8 *)&(FwUpdCompStatus[Count].LastAttemptVersion));
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Updating last attempt version failed with status: %r\n", Status));
    return Status;
  }

  ByteOffset = COMP_STATUS_OFFSET(FwUpdStatusOffset, Count) + OFFSET_OF(FW_UPDATE_COMP_STATUS, LastAttemptStatus);
  Status = BootMediaWrite(ByteOffset, sizeof(UINT32), (UINT8 *)&(FwUpdCompStatus[Count].LastAttemptStatus));
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Updating last attempt status failed with status: %r\n", Status));
    return Status;
  }

  ByteOffset = COMP_STATUS_OFFSET(FwUpdStatusOffset, Count) + OFFSET_OF(FW_UPDATE_COMP_STATUS, UpdatePending);
  Status = BootMediaWrite(ByteOffset, sizeof(UINT8), (UINT8 *)&(FwUpdCompStatus[Count].UpdatePending));
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Updating updatepending failed with status: %r\n", Status));
    return Status;
  }

  DEBUG((DEBUG_INIT, "Firmware Update status updated to reserved region \n"));

  return EFI_SUCCESS;
}

/**
  Verify the capsule image against its signature.

  This function first gets the hash of the processed public key, then compare it
  with the saved one from hash store. If they are same, the pre-processed key is
  verified.
  Then using the pre-process key to verify capsule image, if it passed verification,
  the capsule image can be trusted.

  @param[in] FwImage          The pointer to the firmware update capsule image.
  @param[in] FwSize           The size of capsule image in bytes.

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
AuthenticateCapsule (
  IN  UINT8     *FwImage,
  IN  UINT32    FwSize
  )
{
  EFI_STATUS                Status;
  FIRMWARE_UPDATE_HEADER    *Header;
  UINT8                     *Key;
  UINT8                     *Signature;
  FW_UPDATE_STATUS          FwUpdStatus;
  UINT32                    FwUpdStatusOffset;

  FwUpdStatusOffset = PcdGet32(PcdFwUpdStatusBase);

  Header = (FIRMWARE_UPDATE_HEADER *)FwImage;
  if (!CompareGuid (&Header->FileGuid, &gFirmwareUpdateImageFileGuid)) {
    DEBUG ((DEBUG_ERROR, "Invalid capsule: Image file guid is not expected. guid=%g\n", &Header->FileGuid));
    return EFI_INVALID_PARAMETER;
  }

  if (Header->SignatureOffset >= FwSize || Header->SignatureOffset + Header->SignatureSize >= FwSize) {
    DEBUG ((DEBUG_ERROR, "Invalid capsule: SignatureOffset=0x%x, SignatureSize=0x%x\n", Header->SignatureOffset,
            Header->SignatureSize));
    return EFI_INVALID_PARAMETER;
  }

  if (Header->PubKeyOffset >= FwSize || Header->PubKeyOffset + Header->PubKeySize != FwSize) {
    DEBUG ((DEBUG_ERROR, "Invalid capsule: PubKeyOffset=0x%x, PubKeySize=0x%x\n", Header->PubKeyOffset, Header->PubKeySize));
    return EFI_INVALID_PARAMETER;
  }

  if (Header->ImageOffset >= FwSize || Header->ImageOffset + Header->ImageSize >= FwSize) {
    DEBUG ((DEBUG_ERROR, "Invalid capsule: ImageOffset=0x%x, ImageOffset=0x%x\n", Header->ImageOffset, Header->ImageOffset));
    return EFI_INVALID_PARAMETER;
  }

  Key       = FwImage + Header->PubKeyOffset;
  Signature = FwImage + Header->SignatureOffset;

  //
  // Copy fw update status structure to memory
  //
  Status = BootMediaRead (FwUpdStatusOffset, sizeof(FW_UPDATE_STATUS), (UINT8 *)&FwUpdStatus);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BootMediaRead failed with status: %r\n", Status));
    return Status;
  }

  //
  // If capsule signature in flash does not match with capsule signature,
  // it indicates that capsule image is modified in between firmware update.
  //
  if (*(UINT32 *)(FwUpdStatus.CapsuleSig) != 0xFFFFFFFF) {
    if (CompareMem(&FwUpdStatus.CapsuleSig, Signature, FW_UPDATE_SIG_LENGTH) != 0) {
      return EFI_COMPROMISED_DATA;
    }
  }

  Status    = DoRsaVerify (FwImage, Header->SignatureOffset, COMP_TYPE_PUBKEY_FWU, Signature, Key, NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Image verification failed, %r!\n", Status));
    return EFI_SECURITY_VIOLATION;
  }

  //
  // If this is first time processing the capsule, save the capsule signature in flash until
  // the end of firmware update
  //
  if (*(UINT32 *)(FwUpdStatus.CapsuleSig) != 0xFFFFFFFF) {

    CopyMem((VOID *)&FwUpdStatus.CapsuleSig, (VOID *)Signature, sizeof(FW_UPDATE_SIG_LENGTH));

    Status = BootMediaWrite (FwUpdStatusOffset, sizeof(FW_UPDATE_STATUS), (UINT8 *)&FwUpdStatus);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "BootMediaWrite failed with status %r\n", Status));
      return Status;
    }
  }
  return EFI_SUCCESS;
}

/**
  Process capsule image.

  This function will abstract firmware images from the capsule image. for each
  of the firmware image found, FW_UPDATE_COMP_STATUS structure will be created
  in the reserved region and update pending will be set to pending state.

  This function will create component structures in the order in which they are
  found in the capsule image.

  @param[in] FwImage          The pointer to the firmware update capsule image.
  @param[in] FwSize           The size of capsule image in bytes.

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
ProcessCapsule (
  IN  UINT8     *FwImage,
  IN  UINT32    FwSize
  )
{
  UINT8                         Count;
  UINT8                         TotalPayloadCount;
  EFI_STATUS                    Status;
  UINT32                        FwUpdStatusOffset;
  FW_UPDATE_STATUS              FwUpdStatus;
  FW_UPDATE_COMP_STATUS         FwUpdCompStatus[MAX_FW_COMPONENTS];
  FIRMWARE_UPDATE_HEADER        *FwUpdHeader;
  EFI_FW_MGMT_CAP_HEADER        *CapHeader;
  EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImgHeader;

  FwUpdStatusOffset = PcdGet32(PcdFwUpdStatusBase);

  //
  // Read reserved region structure
  //
  Status = BootMediaRead (FwUpdStatusOffset, sizeof(FW_UPDATE_STATUS), (UINT8 *)&FwUpdStatus);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BootMediaRead. offset: 0x%llx, Status = 0x%x\n", FwUpdStatusOffset, Status));
    return Status;
  }

  //
  // If state machine is 0xFF, Capsule is not processed yet
  // set SM to capsule processing stage, this will reset back to
  // init at the end of firmware update
  //
  if (FwUpdStatus.StateMachine == FW_UPDATE_SM_INIT) {
    SetStateMachineFlag(FW_UPDATE_SM_CAP_PROCESSING);
  } else {
    return EFI_SUCCESS;
  }

  FwUpdHeader = (FIRMWARE_UPDATE_HEADER *)FwImage;
  CapHeader = (EFI_FW_MGMT_CAP_HEADER *)((UINTN)FwUpdHeader + FwUpdHeader->ImageOffset);

  //
  // If capsule header is NULL or no payloads found in the capsule
  // return EFI_NOT_FOUND;
  //
  if ((CapHeader == NULL) || (CapHeader->PayloadItemCount == 0)) {
    ImgHeader = NULL;
    return EFI_NOT_FOUND;
  }

  TotalPayloadCount = (UINT8)(CapHeader->EmbeddedDriverCount + CapHeader->PayloadItemCount);

  //
  // Zero out memory for component structures
  //
  ZeroMem((VOID *)&FwUpdCompStatus, MAX_FW_COMPONENTS * sizeof(FW_UPDATE_COMP_STATUS));

  //
  // Uninitialized update structure in flash
  // initialize it
  //
  if (FwUpdStatus.Signature == 0xFFFFFFFF) {
    FwUpdStatus.Signature = FW_UPDATE_STATUS_SIGNATURE;
    FwUpdStatus.Version = FW_UPDATE_STATUS_VERSION;
    FwUpdStatus.Length = sizeof(FW_UPDATE_STATUS);
  }

  ImgHeader = (EFI_FW_MGMT_CAP_IMAGE_HEADER *)((UINTN)CapHeader + sizeof(EFI_FW_MGMT_CAP_HEADER) + TotalPayloadCount * sizeof (UINT64));

  //
  // Loop through all the payloads
  // if matching guid found, return the payload header
  // Boundary check to see if image header address exceeds the capsule region
  //
  for (Count = 0; Count < CapHeader->PayloadItemCount; Count++) {
    //
    // Create new structure
    //
    CopyMem((VOID *)&(FwUpdCompStatus[Count].FirmwareId), (VOID *)&(ImgHeader->UpdateImageTypeId), sizeof(EFI_GUID));
    FwUpdCompStatus[Count].LastAttemptVersion = 0xFFFFFFFF;
    FwUpdCompStatus[Count].LastAttemptStatus = 0xFFFFFFFF;
    FwUpdCompStatus[Count].UpdatePending = FW_UPDATE_IMAGE_UPDATE_PENDING;

    ImgHeader = (EFI_FW_MGMT_CAP_IMAGE_HEADER *)((UINTN)ImgHeader + ImgHeader->UpdateImageSize + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));
    if ((UINT32)ImgHeader > (UINTN)((UINTN)CapHeader + FwUpdHeader->ImageSize)) {
      return EFI_NOT_FOUND;
    }
  }

  //
  // Update the component information to the reserved region
  //
  Status = BootMediaWrite ((FwUpdStatusOffset + sizeof(FW_UPDATE_STATUS)),\
                           (MAX_FW_COMPONENTS * sizeof(FW_UPDATE_COMP_STATUS)), (UINT8 *)&FwUpdCompStatus);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BootMediaWrite. offset: 0x%llx, Status = 0x%x\n", (FwUpdStatusOffset + sizeof(FW_UPDATE_STATUS)), Status));
    return Status;
  }

  return Status;
}

/**
  Find payload in the capsule image.

  This function will parse through the capsule image to find the payload
  matching the input guid.

  This function if provided with an empty guid will return the first payload
  found

  @param[in] ImageId        Guid to identify payload in the capsule image
  @param[in] CapImage       Pointer to the capsule Image
  @param[in] CapImageSize   Size of the capsule image in bytes
  @param[in] ImageHdr       Pointer to the capsule Image header

  @retval  EFI_SUCCESS      Found matching payload in the capsule.
  @retval  EFI_NOT_FOUND    No matching payload found in the capsule.
**/
EFI_STATUS
FindImage (
  IN  EFI_GUID                      *ImageId,
  IN  UINT8                         *CapImage,
  IN  UINT32                        CapImageSize,
  OUT EFI_FW_MGMT_CAP_IMAGE_HEADER  **ImageHdr
  )
{
  UINT8                   Count;
  UINT8                   TotalPayloadCount;
  EFI_FW_MGMT_CAP_HEADER  *CapHeader;
  FIRMWARE_UPDATE_HEADER  *FwUpdHeader;
  EFI_FW_MGMT_CAP_IMAGE_HEADER  *CapImageHdr;

  *ImageHdr = NULL;
  CapImageHdr = NULL;

  FwUpdHeader = (FIRMWARE_UPDATE_HEADER *)CapImage;
  CapHeader = (EFI_FW_MGMT_CAP_HEADER *)((UINTN)FwUpdHeader + FwUpdHeader->ImageOffset);

  //
  // If capsule header is NULL or no payloads found in the capsule
  // return EFI_NOT_FOUND;
  //
  if ((CapHeader == NULL) || (CapHeader->PayloadItemCount == 0)) {
    return EFI_NOT_FOUND;
  }

  TotalPayloadCount = (UINT8)(CapHeader->EmbeddedDriverCount + CapHeader->PayloadItemCount);

  CapImageHdr = (EFI_FW_MGMT_CAP_IMAGE_HEADER *)((UINTN)CapHeader + sizeof(EFI_FW_MGMT_CAP_HEADER) + TotalPayloadCount * sizeof(UINT64));

  //
  // If input guid is NULL, return the first payload header
  //
  if (ImageId == NULL) {
    *ImageHdr = CapImageHdr;
    return EFI_SUCCESS;
  }

  //
  // Loop through all the payloads
  // if matching guid, return the payload header
  // Boundary check to see if image header address exceeds the capsule region
  //
  for (Count = 0; Count < CapHeader->PayloadItemCount; Count++) {
    if (CompareGuid(&CapImageHdr->UpdateImageTypeId, ImageId) == TRUE) {
      *ImageHdr = CapImageHdr;
      return EFI_SUCCESS;
    }
    CapImageHdr = (EFI_FW_MGMT_CAP_IMAGE_HEADER *)((UINTN)CapImageHdr + CapImageHdr->UpdateImageSize + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));
    if ((UINTN)CapImageHdr > (UINTN)((UINTN)CapHeader + FwUpdHeader->ImageSize)) {
      return EFI_NOT_FOUND;
    }
  }

  return EFI_NOT_FOUND;
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
    DEBUG ((DEBUG_ERROR, "AfterUpdateEnforceFwUpdatePolicy failed! Status = %r\n", Status));
    return Status;
  }

  return Status;
}

/**
  Perform Firmware update.

  This function based on the image type id guid from the image header will
  call the respective functions to perform capsule update.
*
  @param[in] CapImage       Pointer to the capsule Image
  @param[in] CapImageSize   Size of the capsule image in bytes
* @param[in] ImageHdr       Pointer to fw mgmt capsule Image header
* @param[in] ResetRequired  Pointer to boolean reset required

  @retval  EFI_SUCCESS      Update successful.
  @retval  other            error status from the update routine
**/
EFI_STATUS
ApplyFwImage (
  IN    UINT8                        *CapImage,
  IN    UINT32                       CapImageSize,
  IN    EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr,
    OUT BOOLEAN                       *ResetRequired
  )
{
  EFI_STATUS  Status;
  VOID        *CsmeUpdateInData;

  Status = EFI_SUCCESS;
  *ResetRequired = FALSE;

  if (CompareGuid(&ImageHdr->UpdateImageTypeId, &gSblFWUpdateImageFileGuid) == TRUE) {
    Status = UpdateSystemFirmware(ImageHdr);
    *ResetRequired = TRUE;
  } else if (CompareGuid(&ImageHdr->UpdateImageTypeId, &gCfgFWUpdateImageFileGuid) == TRUE) {
    Status = UpdateSystemFirmware(ImageHdr);
    *ResetRequired = TRUE;
  } else if (CompareGuid(&ImageHdr->UpdateImageTypeId, &gCsmeFWUpdateImageFileGuid) == TRUE) {
    if (FeaturePcdGet(PcdCsmeUpdateEnabled) == 1) {
      CsmeUpdateInData = InitCsmeUpdInputData();
      if (CsmeUpdateInData != NULL) {
        Status = UpdateCsme(CapImage, CapImageSize, CsmeUpdateInData, ImageHdr);
        *ResetRequired = TRUE;
      }
    }
  }

  return Status;
}

/**
  This is generic function for firmware update.

  This function will get capsule image file, verify the image, and update
  current firmware using new firmware.

  @retval  EFI_SUCCESS           The operation completed successfully.
  @retval  others                There is error happening.
**/
EFI_STATUS
InitFirmwareUpdate (
  VOID
)
{
  EFI_STATUS                  Status;
  UINT8                       Count;
  VOID                        *CapsuleImage;
  UINT32                      CapsuleSize;
  UINT32                      FwUpdStatusOffset;
  UINT32                      ByteOffset;
  BOOLEAN                     ResetRequired;
  FW_UPDATE_COMP_STATUS       FwUpdCompStatus[MAX_FW_COMPONENTS];
  EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImgHdr;

  ImgHdr = NULL;
  FwUpdStatusOffset = PcdGet32(PcdFwUpdStatusBase);
  ResetRequired = FALSE;

  //
  // 1. Get capsule image.
  //
  Status = GetCapsuleImage (&CapsuleImage, &CapsuleSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "GetCapsuleImage failed with status = %r\n", Status));
    return Status;
  }
  DEBUG ((DEBUG_INFO, "CapsuleImage: 0x%p, CapsuleSize: 0x%X\n", CapsuleImage, CapsuleSize));

  //
  // 2. Authenticate capsule image.
  //
  Status = AuthenticateCapsule (CapsuleImage, CapsuleSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AuthenticateCapsule, Status = 0x%x\n", Status));
    return Status;
  }

  //
  // 3. Process capsule image.
  //
  Status = ProcessCapsule (CapsuleImage, CapsuleSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ProcessCapsule, Status = 0x%x\n", Status));
    return Status;
  }

  //
  // Read the component structure in the reserved region
  //
  Status = BootMediaRead ((FwUpdStatusOffset + sizeof(FW_UPDATE_STATUS)), \
                          MAX_FW_COMPONENTS * sizeof(FW_UPDATE_COMP_STATUS), (UINT8 *)&FwUpdCompStatus);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BootMediaRead. offset: 0x%llx, Status = 0x%x\n", (FwUpdStatusOffset + sizeof(FW_UPDATE_STATUS)), Status));
    return Status;
  }

  //
  // Loop through the components to perform update
  //
  for (Count = 0; Count < MAX_FW_COMPONENTS; Count ++) {
    //
    // If the component has pending or processing state
    //
    if (FwUpdCompStatus[Count].UpdatePending & (BIT0 | BIT1 | BIT2)) {
      if (FwUpdCompStatus[Count].UpdatePending == FW_UPDATE_IMAGE_UPDATE_PENDING) {
        //
        // Set the component to processing state before the update
        //
        FwUpdCompStatus[Count].UpdatePending = FW_UPDATE_IMAGE_UPDATE_PROCESSING;

        //
        // Update component state in the reserved region
        //
        ByteOffset = COMP_STATUS_OFFSET(FwUpdStatusOffset, Count) + OFFSET_OF(FW_UPDATE_COMP_STATUS, UpdatePending);
        Status = BootMediaWrite(ByteOffset, sizeof(UINT8), (UINT8 *)&(FwUpdCompStatus[Count].UpdatePending));
        if (EFI_ERROR (Status)) {
          DEBUG((DEBUG_ERROR, "BootMediaWrite. offset: 0x%llx, Status = 0x%x\n", (FwUpdStatusOffset + sizeof(FW_UPDATE_STATUS)), Status));
          return Status;
        }
      }

      //
      // Find payload associated with component in the capsule image
      //
      Status = FindImage(&(FwUpdCompStatus[Count].FirmwareId), CapsuleImage, CapsuleSize, &ImgHdr);
      if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "FindImage failed with Status = %r\n", Status));
        return Status;
      }

      //
      // Start firmware udpate for the component
      //
      Status = ApplyFwImage(CapsuleImage, CapsuleSize, ImgHdr, &ResetRequired);
      if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "ApplyFwImage failed with Status = %r\n", Status));
      }

      //
      // Update firmware update status of the component in reserved region
      //
      Status = UpdateStatus(&(ImgHdr->UpdateImageTypeId), (UINT16)ImgHdr->Version, Status);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "UpdateStatus failed! Status = %r\n", Status));
        DEBUG((DEBUG_ERROR, "Reset required to proceed with the firmware update.\n"));
      }

      //
      // Reset system if required
      //
      if (ResetRequired == TRUE) {
        ResetSystem (EfiResetCold);
        CpuDeadLoop ();
      }
    }
  }

  //
  // At this point, all the firmware images in the capsule are processed
  // Clear the state machine and exit
  //
  if (Count == MAX_FW_COMPONENTS) {
    SetStateMachineFlag(FW_UPDATE_SM_INIT);
  } else {
    return EFI_ABORTED;
  }

  return EFI_SUCCESS;
}

/**
  Get all region sizes from flash map.

  This function will get topswap, redundant and non redundant region sizes from
  flash map.

  @param[out] TopSwapRegionSize          The boot media address to be update.
  @param[out] RedundantRegionSize        The source buffer to write to the boot media.
  @param[out] NonRedundantRegionSize     The length of data to write to boot media.

  @retval  EFI_SUCCESS        Update successfully.
  @retval  others             Error happening when updating.
**/
EFI_STATUS
EFIAPI
GetRegionInfo (
  OUT UINT32     *TopSwapRegionSize,
  OUT UINT32     *RedundantRegionSize,
  OUT UINT32     *NonRedundantRegionSize
  )
{
  FLASH_MAP             *FlashMap;

  FlashMap = GetFlashMapPtr();
  if (FlashMap == NULL) {
    return EFI_NOT_FOUND;
  }

  *TopSwapRegionSize = GetRegionOffsetSize (FlashMap, FLASH_MAP_FLAGS_TOP_SWAP, NULL);
  *RedundantRegionSize = GetRegionOffsetSize (FlashMap, FLASH_MAP_FLAGS_REDUNDANT_REGION, NULL);
  *NonRedundantRegionSize = GetRegionOffsetSize (FlashMap, FLASH_MAP_FLAGS_NON_REDUNDANT_REGION, NULL);

  return EFI_SUCCESS;
}

/**
  Payload main entry.

  This function will continue Payload execution with a new memory based stack.

  @param  Param           parameter passed from SwitchStack().
  @param  PldBase         payload base passed from SwitchStack().

**/
VOID
EFIAPI
PayloadMain (
  IN  VOID  *Param,
  IN  VOID  *PldBase
  )
{
  UINT32        RsvdBase;
  UINT32        RsvdSize;
  FLASH_MAP     *FlashMap;
  EFI_STATUS    Status;

  DEBUG ((DEBUG_INFO, "Starting Firmware Update\n"));
  //
  // Initialize boot media to look for the capsule image
  //
  InitializeBootMedia ();

  //
  // Get flash map pointer
  //
  FlashMap = GetFlashMapPtr();
  if (FlashMap == NULL) {
    DEBUG((DEBUG_ERROR, "Could not get flash map\n"));
    return;
  }

  //
  // Get bootloader reserved region base and size
  //
  Status = GetComponentInfoByPartition (FLASH_MAP_SIG_BLRESERVED, FALSE, &RsvdBase, &RsvdSize);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Could not get component information for bootloader reserved region\n"));
    return;
  }

  //
  // Set PCD for Firmware Update status structure base
  //
  Status = PcdSet32S (PcdFwUpdStatusBase, (FlashMap->RomSize - (~RsvdBase + 1)));
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = PcdSet32S (PcdRsvdRegionBase, RsvdBase);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Perform firmware update
  //
  Status = InitFirmwareUpdate ();
  if (EFI_ERROR (Status)) {
    if (Status != EFI_ALREADY_STARTED) {
      DEBUG((DEBUG_ERROR, "Firmware update failed with Status = %r\n", Status));
    } else {
      //
      // This case happens when firmware update is successfully completed
      //
      Status = EFI_SUCCESS;
    }
  }

  //
  // Terminate firmware update
  //
  Status = EndFirmwareUpdate ();
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "EndFirmwareUpdate, Status = 0x%x\n", Status));
  }

  DEBUG((DEBUG_ERROR, "Reset required to proceed with the firmware update.\n"));
  ResetSystem (EfiResetCold);
  CpuDeadLoop ();
}


