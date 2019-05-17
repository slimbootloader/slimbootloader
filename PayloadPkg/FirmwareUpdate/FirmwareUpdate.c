/** @file
This driver is to update firmware in boot media.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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

  @param[in] FwImage            The pointer to the firmware update capsule image.
  @param[in] FwPolicy           Firmware update policy.

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
VerifyFwVersion (
  IN  UINT8                   *FwImage,
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
    Status = PlatformGetStage1AOffset(FwImage, FALSE, &CompBase, &CompSize);
  } else if (FwPolicy.Fields.UpdatePartitionA == 0x1) {
    Status = PlatformGetStage1AOffset(FwImage, TRUE, &CompBase, &CompSize);
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

  Status = UpdateStatus((CapsuleBlVersion->ImageVersion.ProjMajorVersion << 8) | CapsuleBlVersion->ImageVersion.ProjMinorVersion, 0xFFFFFFFF);
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
  FIRMWARE_UPDATE_STATUS  *FwUpdStatus;

  //
  // Read from the reserved region and return state machine
  //
  FwUpdStatus = (FIRMWARE_UPDATE_STATUS *)(PcdGet32(PcdRsvdRegionBase));

  *StateMachine = FwUpdStatus->StateMachine;
}

/**
  Set state machine flag in flash.

  This function will set state machine flag in the bootloader reserved region
  First byte in the booloader reserved region is state machine flag

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
  UINT32                  FwUpdStatusOffset;
  FIRMWARE_UPDATE_STATUS  FwUpdStatus;

  DEBUG((DEBUG_INIT, "Set next FWU state to 0x%02X\n", StateMachine));

  //
  // Any value less than 0xFC is invalid
  //
  if (StateMachine < FW_UPDATE_SM_PART_AB) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Any value other than 0xFC, 0xFD, 0xFE, 0xFF is invalid
  //
  if ((StateMachine & FW_UPDATE_SM_PART_AB) != FW_UPDATE_SM_PART_AB) {
    return EFI_INVALID_PARAMETER;
  }

  FwUpdStatusOffset = PcdGet32(PcdFwUpdStatusBase);

  Status = BootMediaRead (FwUpdStatusOffset, sizeof(FIRMWARE_UPDATE_STATUS), (UINT8 *)&FwUpdStatus);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BootMediaRead.  FwUpdStatusOffset: 0x%llx, Status = 0x%x\n", FwUpdStatusOffset, Status));
    return Status;
  }

  //
  // To write 0xFF, erase 4kb block
  //
  if (StateMachine == FW_UPDATE_SM_INIT) {
    Status = BootMediaErase (FwUpdStatusOffset, EFI_PAGE_SIZE);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "BootMediaErase.  RsvdBase: 0x%llx, Status = 0x%x\n", FwUpdStatusOffset, Status));
      return Status;
    }

    Status = BootMediaWrite (FwUpdStatusOffset, sizeof(FIRMWARE_UPDATE_STATUS) - sizeof(UINT32), (UINT8 *)&FwUpdStatus);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "BootMediaWrite.  readaddr: 0x%llx, Status = 0x%x\n", FwUpdStatusOffset, Status));
      return Status;
    }

    return Status;
  }

  FwUpdStatus.StateMachine = StateMachine;

  Status = BootMediaWrite (FwUpdStatusOffset, sizeof(FIRMWARE_UPDATE_STATUS), (UINT8 *)&FwUpdStatus);
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
  EFI_STATUS              Status;
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
  case FW_UPDATE_SM_INIT:
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
      Status = SetStateMachineFlag (FW_UPDATE_SM_INIT);
      if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "Error while setting state machine\n"));
        return Status;
      }
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

  @param[in] LastAttemptVersion Version of last firmware update attempted.
  @param[in] LastAttemptStatus Status of last firmware update attempted.

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
UpdateStatus (
  IN UINT16     LastAttemptVersion,
  IN EFI_STATUS LastAttemptStatus
 )
{
  EFI_STATUS              Status;
  UINT32                  FwUpdStatusOffset;
  FIRMWARE_UPDATE_STATUS  FwUpdStatus;

  FwUpdStatusOffset = PcdGet32(PcdFwUpdStatusBase);

  Status = BootMediaRead (FwUpdStatusOffset, sizeof(FIRMWARE_UPDATE_STATUS), (UINT8 *)&FwUpdStatus);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BootMediaRead.  FwUpdStatusOffset: 0x%llx, Status = 0x%x\n", FwUpdStatusOffset, Status));
    return Status;
  }

  if (FwUpdStatus.Signature == 0xFFFFFFFF) {
    FwUpdStatus.Signature = FIRMWARE_UPDATE_STATUS_SIGNATURE;
    FwUpdStatus.Version = FIRMWARE_UPDATE_STATUS_VERSION;
    FwUpdStatus.Length = sizeof(FIRMWARE_UPDATE_STATUS);
  }

  if (LastAttemptVersion != 0) {
    FwUpdStatus.LastAttemptVersion = LastAttemptVersion;
  }

  if (LastAttemptStatus != 0xFFFFFFFF) {
    if (LastAttemptStatus == EFI_SUCCESS) {
      FwUpdStatus.LastAttemptStatus = LAST_ATTEMPT_STATUS_SUCCESS;
    } else if (LastAttemptStatus == EFI_INCOMPATIBLE_VERSION) {
      FwUpdStatus.LastAttemptStatus = LAST_ATTEMPT_STATUS_ERROR_INCORRECT_VERSION;
    } else if (LastAttemptStatus == EFI_OUT_OF_RESOURCES) {
      FwUpdStatus.LastAttemptStatus = LAST_ATTEMPT_STATUS_ERROR_INSUFFICIENT_RESOURCES;
    } else if (LastAttemptStatus == EFI_SECURITY_VIOLATION) {
      //
      // If we get security violation, at this point we do not know
      // version from capsule, instead of keeping the existing version
      // reset it back to 0
      //
      FwUpdStatus.LastAttemptVersion = 0;
      FwUpdStatus.LastAttemptStatus = LAST_ATTEMPT_STATUS_ERROR_AUTH_ERROR;
    } else {
      FwUpdStatus.LastAttemptVersion = 0;
      FwUpdStatus.LastAttemptStatus = LAST_ATTEMPT_STATUS_ERROR_UNSUCCESSFUL;
    }
  }

  Status = BootMediaErase (FwUpdStatusOffset, EFI_PAGE_SIZE);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BootMediaErase.  RsvdBase: 0x%llx, Status = 0x%x\n", FwUpdStatusOffset, Status));
    return Status;
  }

  Status = BootMediaWrite(FwUpdStatusOffset, sizeof(FIRMWARE_UPDATE_STATUS), (UINT8 *)&FwUpdStatus);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BootMediaWrite.  readaddr: 0x%llx, Status = 0x%x\n", FwUpdStatusOffset, Status));
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
  CHAR8                     *Type;

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
  Status    = DoRsaVerify (FwImage, Header->SignatureOffset, COMP_TYPE_PUBKEY_FWU, Signature, Key, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Image verification failed, %r!\n", Status));
    return EFI_SECURITY_VIOLATION;
  }

  if ((Header->CapsuleFlags & CAPSULE_FLAGS_CFG_DATA) != 0) {
    Type = "CFGDATA";
  } else {
    Type = "BOOTLOADER";
  }
  DEBUG ((DEBUG_INFO, "The new %a image passed verification\n", Type));

  return EFI_SUCCESS;
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
  VOID                        *CapsuleImage;
  UINT32                      CapsuleSize;
  FIRMWARE_UPDATE_POLICY      FwPolicy;
  FIRMWARE_UPDATE_PARTITION   *UpdatePartition;

  //
  // 1. Enforce firmware update policy.
  //
  Status = EnforceFwUpdatePolicy (&FwPolicy);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "EnforceFwUpdatePolicy: Status = 0x%x\n", Status));
    return Status;
  }

  //
  // 2. Get capsule image.
  //
  Status = GetCapsuleImage (&CapsuleImage, &CapsuleSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "GetCapsuleImage failed with status = %r\n", Status));
    return Status;
  }
  DEBUG ((DEBUG_INFO, "CapsuleImage: 0x%p, CapsuleSize: 0x%X\n", CapsuleImage, CapsuleSize));

  //
  // 3. Authenticate capsule image.
  //
  Status = AuthenticateCapsule (CapsuleImage, CapsuleSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AuthenticateCapsule, Status = 0x%x\n", Status));
    return Status;
  }

  //
  // 4. Check firmware version.
  //
  Status = VerifyFwVersion (CapsuleImage, FwPolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, " VerifyFwVersion failed with Status = 0x%x\n", Status));
    return Status;
  }

  //
  // 5. Get firmware update required information.
  //
  Status = GetFirmwareUpdateInfo (CapsuleImage, FwPolicy, &UpdatePartition);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "GetFirmwareUpdateInfo, Status = 0x%x\n", Status));
    return Status;
  }

  //
  // 6. Do boot partition update.
  //
  Status = UpdateBootPartition (UpdatePartition);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UpdateBootPartition, Status = 0x%x\n", Status));
    return Status;
  }

  //
  // 7. After update Enforce firmware update policy
  //
  Status = AfterUpdateEnforceFwUpdatePolicy(FwPolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AfterUpdateEnforceFwUpdatePolicy failed! Status = %r\n", Status));
    return Status;
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
  // Control comes here after the firmware update is complete
  // Update Fw update status in reserved region
  //
  Status = UpdateStatus(0, Status);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UpdateLastAttemptStatus failed! Status = %r\n", Status));
    DEBUG((DEBUG_ERROR, "Reset required to proceed with the firmware update.\n"));
    ResetSystem (EfiResetCold);
    CpuDeadLoop ();
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


