/** @file
This driver is to update firmware in boot media.

Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
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
#include <Library/SecureBootLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/FirmwareUpdateLib.h>
#include <Guid/SystemResourceTable.h>
#include <Library/GraphicsLib.h>
#include <Library/ConsoleOutLib.h>
#include <Guid/OsBootOptionGuid.h>
#include "FirmwareUpdateHelper.h"

UINT32   mSblImageBiosRgnOffset;

/**
  Retrieve the SBL rom image offset within BIOS region.

  @retval  The SBL rom image offset within BIOS region
**/
UINT32
GetRomImageOffsetInBiosRegion (
  VOID
  )
{
  return mSblImageBiosRgnOffset;
}

/**
  Verify the firmware version to make sure it is no less than current firmware version.

  @param[in] ImageHdr     Pointer to the fw mgmt capsule image header
  @param[in] FwPolicy     Firmware update policy.

  @retval  EFI_SUCCESS    The operation completed successfully.
  @retval  others         There is error happening.
**/
EFI_STATUS
VerifySblVersion (
  IN  EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr,
  IN  FIRMWARE_UPDATE_POLICY  FwPolicy
  )
{
  UINT32                Stage1ABase;
  UINT32                Stage1ASize;
  UINT32                Stage1AFvBase;
  BOOT_LOADER_VERSION   *CapsuleBlVersion;
  BOOT_LOADER_VERSION   *CurrentBlVersion;
  VOID                  *GuidHob;
  EFI_STATUS            Status;
  EXT_BOOT_LOADER_VERSION  *ExtBlVersion;

  //
  // SVN check for BIOS
  //
  CapsuleBlVersion = NULL;
  CurrentBlVersion = NULL;

  //
  // Get SBL version from current system firmware
  //
  GuidHob = GetNextGuidHob (&gBootLoaderVersionGuid, GetHobListPtr());
  if (GuidHob == NULL) {
    DEBUG((DEBUG_ERROR, "Failed to get current firmware version\n"));
    return EFI_NOT_FOUND;
  }
  ExtBlVersion = (EXT_BOOT_LOADER_VERSION *)GET_GUID_HOB_DATA (GuidHob);
  CurrentBlVersion = &ExtBlVersion->Version;

  //
  // Get base address of Stage 1A in capsule Image
  //
  if (FwPolicy.Fields.UpdatePartitionB == 0x1) {
    Status = PlatformGetStage1AOffset (ImageHdr, FALSE, &Stage1ABase, &Stage1ASize);
  } else if (FwPolicy.Fields.UpdatePartitionA == 0x1) {
    Status = PlatformGetStage1AOffset (ImageHdr, TRUE, &Stage1ABase, &Stage1ASize);
  } else {
    Status = EFI_UNSUPPORTED;
  }

  //
  // If we can get Stage 1A base from platform code, try and get SBL version
  //
  if (Status == EFI_SUCCESS) {
    Status = GetVersionfromFv (Stage1ABase, TRUE, &CapsuleBlVersion);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "Getting SBL version from capsule failed with status: %r\n", Status));
      return Status;
    }
  } else if (EFI_ERROR(Status)) {
    // This check is introduced to handle platform specific implementation
    // all platforms that DOES NOT require special handling will return
    // EFI_UNSUPPORTED from PlatformGetStage1AOffset and getting stage1Abase
    // will be handled in common way using the below implementation.
    if (Status == EFI_UNSUPPORTED) {
      // Last 4 bytes of the BIOS region contain Stage 1A FV base.
      Stage1AFvBase = (UINT32)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER) + ImageHdr->UpdateImageSize - 4);
      Status = GetSvn (Stage1AFvBase, &CapsuleBlVersion);
      if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "Getting SBL version from Stage1AFvBase failed with status: %r\n", Status));
        return Status;
      }
    } else {
      DEBUG((DEBUG_ERROR, "Getting Stage 1A offset from capsule failed with status: %r\n", Status));
      return Status;
    }
  }

  //
  // Update last update version to the version we are about to update
  //
  Status = UpdateStatus(ImageHdr->UpdateHardwareInstance, \
                        (CapsuleBlVersion->ImageVersion.ProjMajorVersion << 8) | CapsuleBlVersion->ImageVersion.ProjMinorVersion, \
                        0xFFFFFFFF);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Updating status to reserved region failed: %r\n", Status));
    return Status;
  }

  //
  // Check for Antirollback
  //
  if (CapsuleBlVersion->ImageVersion.SecureVerNum >= CurrentBlVersion->ImageVersion.SecureVerNum) {
    DEBUG((DEBUG_INIT, " Updating Slim Bootloader from version %x to version %x \n", \
           CurrentBlVersion->ImageVersion.SecureVerNum, CapsuleBlVersion->ImageVersion.SecureVerNum));
    return EFI_SUCCESS;
  }

  DEBUG((DEBUG_ERROR, "Antirollback - Could not rollback to version %x from current version: %x\n", \
         CapsuleBlVersion->ImageVersion.SecureVerNum, CurrentBlVersion->ImageVersion.SecureVerNum));

  return EFI_INCOMPATIBLE_VERSION;
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
  UINT8                 SvnStatus;
  EFI_STATUS            Status;

  //
  // Check SVN for CFGDATA update
  //
  if (((UINT32) ImageHdr->UpdateHardwareInstance) == FLASH_MAP_SIG_CFGDATA){
    DEBUG((DEBUG_INFO, "Capsule update is for Config data region!!\n"));
    Status = CheckSblConfigDataSvn (ImageHdr, FwPolicy, &SvnStatus);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    if (SvnStatus == 0) {
      DEBUG((DEBUG_INFO, "Config blob update Svn check failed!!\n"));
      return EFI_INCOMPATIBLE_VERSION;
    } else {
      return EFI_SUCCESS;
    }
  }

  if ((UINT32)ImageHdr->UpdateHardwareInstance == FW_UPDATE_COMP_BIOS_REGION) {
    Status = VerifySblVersion (ImageHdr, FwPolicy);
    return Status;
  }

  return EFI_INVALID_PARAMETER;
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
  UINT32                  FwUpdStatusOffset;

  DEBUG((DEBUG_INIT, "Set next FWU state: 0x%02X\n", StateMachine));

  FwUpdStatusOffset = PcdGet32(PcdFwUpdStatusBase);

  FwUpdStatusOffset += OFFSET_OF(FW_UPDATE_STATUS, StateMachine);
  Status = BootMediaWrite (FwUpdStatusOffset, sizeof(UINT8), (UINT8 *)&(StateMachine));
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BootMediaWrite.  readaddr: 0x%llx, Status = 0x%x\n", FwUpdStatusOffset, Status));
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
      FwPolicy->Fields.Reboot             = 0;
    }
    break;

  case FW_UPDATE_SM_PART_B:
    if (LoaderInfo->BootPartition == FW_UPDATE_PARTITION_A){
      FwPolicy->Fields.StateMachine       = FW_UPDATE_SM_PART_AB;
      FwPolicy->Fields.UpdatePartitionB   = 0x1;
      FwPolicy->Fields.SwitchtoBackupPart = 0;
      FwPolicy->Fields.Reboot             = 0;
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
    Reboot (EfiResetCold);
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
    Reboot (EfiResetWarm);
  }

  //
  // When we are setting SM as FW_UPDATE_SM_PART_AB, this indicates
  // SBL update is done, One more reset is not required, end the
  // firmware update and reset the system.
  //
  if (FwPolicy.Fields.StateMachine == FW_UPDATE_SM_PART_AB) {
    return EFI_END_OF_FILE;
  }

  return EFI_SUCCESS;
}

/**
  This function will be called after the firmware update is complete.
  This function will update firmware update status structure in reserved region

  @param[in] Signature          Signature of component to update.
  @param[in] LastAttemptVersion Version of last firmware update attempted.
  @param[in] LastAttemptStatus  Status of last firmware update attempted.

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
UpdateStatus (
  IN UINT64     Signature,
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
    if (FwUpdCompStatus[Count].HardwareInstance == Signature) {
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
  PUB_KEY_HDR               *PubKeyHdr;
  SIGNATURE_HDR             *SignatureHdr;

  Header = (FIRMWARE_UPDATE_HEADER *)FwImage;
  if (FwSize < sizeof (FIRMWARE_UPDATE_HEADER)) {
    DEBUG ((DEBUG_ERROR, "Invalid capsule: file is too small. file size=%d\n", FwSize));
    return EFI_INVALID_PARAMETER;
  }

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

  PubKeyHdr       = (PUB_KEY_HDR *) (FwImage + Header->PubKeyOffset);
  SignatureHdr    = (SIGNATURE_HDR *) (FwImage + Header->SignatureOffset);
  Status = DoRsaVerify (FwImage, Header->SignatureOffset, HASH_USAGE_PUBKEY_FWU, SignatureHdr, PubKeyHdr, PcdGet8(PcdCompSignHashAlg), NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Image verification failed, %r!\n", Status));
    return EFI_SECURITY_VIOLATION;
  }

  return EFI_SUCCESS;
}

/**
  Get Firmware update Image loading preference order.

  This function will return image loading order based on hardware instance id.

  @param[in] HardwareInstance    Firmware update component id.

  @retval  ImageOrder        preference order for image loading.
**/
UINT32
GetFwImageOrder (
  IN  UINT64    HardwareInstance
  )
{
  UINT32 ImageOrder;

  if (HardwareInstance == FW_UPDATE_COMP_CSME_REGION) {
    ImageOrder = FW_UPDATE_COMP_CSME_REGION_ORDER;
  } else if (HardwareInstance == FW_UPDATE_COMP_CSME_DRIVER) {
    ImageOrder = FW_UPDATE_COMP_CSME_DRIVER_ORDER;
  } else if (HardwareInstance == FW_UPDATE_COMP_BIOS_REGION) {
    ImageOrder = FW_UPDATE_COMP_BIOS_REGION_ORDER;
  } else {
    ImageOrder = FW_UPDATE_COMP_DEFAULT_ORDER;
  }

  return ImageOrder;
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
  UINT8                         i;
  UINT8                         TotalPayloadCount;
  EFI_STATUS                    Status;
  UINT32                        FwUpdStatusOffset;
  FW_UPDATE_STATUS              FwUpdStatus;
  FW_UPDATE_COMP_STATUS         FwUpdCompStatus[MAX_FW_COMPONENTS];
  FW_UPDATE_COMP_STATUS         FwUpdCompStatusTemp;
  FIRMWARE_UPDATE_HEADER        *FwUpdHeader;
  EFI_FW_MGMT_CAP_HEADER        *CapHeader;
  EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImgHeader;
  SIGNATURE_HDR                 *SignatureHdr;
  UINT32                        SigLen;

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
  // If Bit 3 is 0, SM is set to Done.
  // When SM is DONE, we are performing new firmware
  // update, clear whole reserved region and start fresh.
  //
  if ((FwUpdStatus.StateMachine & BIT3) == 0) {
    Status = BootMediaErase(FwUpdStatusOffset, EFI_PAGE_SIZE);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "BootMediaErase failed with status %r\n", Status));
      return Status;
    }

    //
    // Since we cleared the whole region, lets read it one more time.
    //
    Status = BootMediaRead (FwUpdStatusOffset, sizeof(FW_UPDATE_STATUS), (UINT8 *)&FwUpdStatus);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "BootMediaRead. offset: 0x%llx, Status = 0x%x\n", FwUpdStatusOffset, Status));
      return Status;
    }
  }

  //
  // If state machine is 0xFF, Capsule is not processed yet
  // set SM to capsule processing stage, this will reset back to
  // init at the end of firmware update
  //
  SignatureHdr = (SIGNATURE_HDR *) (FwImage + ((FIRMWARE_UPDATE_HEADER *)FwImage)->SignatureOffset);
  SigLen       = MIN (SignatureHdr->SigSize, sizeof(FwUpdStatus.CapsuleSig));
  if (FwUpdStatus.StateMachine == FW_UPDATE_SM_INIT) {
    //
    // Initialize reserved region structure
    //
    FwUpdStatus.Signature = FW_UPDATE_STATUS_SIGNATURE;
    FwUpdStatus.Version = FW_UPDATE_STATUS_VERSION;
    FwUpdStatus.Length = sizeof(FW_UPDATE_STATUS);

    //
    // Save the current capsule signature into flash
    //
    CopyMem (FwUpdStatus.CapsuleSig, SignatureHdr->Signature, SigLen);

    Status = BootMediaWrite(FwUpdStatusOffset, sizeof(FW_UPDATE_STATUS), (UINT8 *)&FwUpdStatus);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = SetStateMachineFlag (FW_UPDATE_SM_CAP_PROCESSING);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    //
    // Clear firmware update trigger
    //
    ClearFwUpdateTrigger();
  } else {

    //
    // If capsule signature in flash does not match with capsule signature,
    // it indicates that capsule image is modified in between firmware update.
    //
    if (CompareMem (FwUpdStatus.CapsuleSig, SignatureHdr->Signature, SigLen) != 0) {
      return EFI_COMPROMISED_DATA;
    }

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
    FwUpdCompStatus[Count].HardwareInstance = ImgHeader->UpdateHardwareInstance;
    FwUpdCompStatus[Count].LastAttemptVersion = 0xFFFFFFFF;
    FwUpdCompStatus[Count].LastAttemptStatus = 0xFFFFFFFF;
    FwUpdCompStatus[Count].UpdatePending = FW_UPDATE_IMAGE_UPDATE_PENDING;

    ImgHeader = (EFI_FW_MGMT_CAP_IMAGE_HEADER *)((UINTN)ImgHeader + ImgHeader->UpdateImageSize + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));
    if ((UINTN)ImgHeader > ((UINTN)CapHeader + FwUpdHeader->ImageSize)) {
      return EFI_NOT_FOUND;
    }
  }

  //
  // Sort FwUpdCompStatus as per the Firmware update image loading order when multiple updates are in capsule update
  // CSME - 1, CSMD - 2, BIOS - 3, remaining components as containers would be updated at last.
  //
  for (Count = 0; Count < CapHeader->PayloadItemCount; Count++) {
    for (i = Count+1; i < CapHeader->PayloadItemCount; i++) {
      if (GetFwImageOrder (FwUpdCompStatus[Count].HardwareInstance) > GetFwImageOrder (FwUpdCompStatus[i].HardwareInstance)) {
           CopyMem((VOID *)&FwUpdCompStatusTemp, (VOID *)&FwUpdCompStatus[Count], sizeof(FW_UPDATE_COMP_STATUS));
           CopyMem((VOID *)&FwUpdCompStatus[Count], (VOID *)&FwUpdCompStatus[i], sizeof(FW_UPDATE_COMP_STATUS));
           CopyMem((VOID *)&FwUpdCompStatus[i], (VOID *)&FwUpdCompStatusTemp, sizeof(FW_UPDATE_COMP_STATUS));
      }
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

  @param[in] Signature      Signature of component to update.
  @param[in] CapImage       Pointer to the capsule Image
  @param[in] CapImageSize   Size of the capsule image in bytes
  @param[in] ImageHdr       Pointer to the capsule Image header

  @retval  EFI_SUCCESS      Found matching payload in the capsule.
  @retval  EFI_NOT_FOUND    No matching payload found in the capsule.
**/
EFI_STATUS
FindImage (
  IN  UINT64                        Signature,
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
  if (FwUpdHeader == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // If capsule header is NULL or no payloads found in the capsule
  // return EFI_NOT_FOUND;
  //
  CapHeader = (EFI_FW_MGMT_CAP_HEADER *)((UINTN)FwUpdHeader + FwUpdHeader->ImageOffset);
  if ((CapHeader == NULL) || (CapHeader->PayloadItemCount == 0)) {
    return EFI_NOT_FOUND;
  }

  TotalPayloadCount = (UINT8)(CapHeader->EmbeddedDriverCount + CapHeader->PayloadItemCount);

  CapImageHdr = (EFI_FW_MGMT_CAP_IMAGE_HEADER *)((UINTN)CapHeader + sizeof(EFI_FW_MGMT_CAP_HEADER) + TotalPayloadCount * sizeof(UINT64));

  //
  // Loop through all the payloads
  // if matching guid, return the payload header
  // Boundary check to see if image header address exceeds the capsule region
  //
  for (Count = 0; Count < CapHeader->PayloadItemCount; Count++) {
    if (CapImageHdr->UpdateHardwareInstance == Signature) {
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
  Perform Firmware update.

  This function based on the image type id guid from the image header will
  call the respective functions to perform capsule update.

  @param[in] CapImage       Pointer to the capsule Image
  @param[in] CapImageSize   Size of the capsule image in bytes
  @param[in] ImageHdr       Pointer to fw mgmt capsule Image header
  @param[in] ResetRequired  Pointer to boolean reset required

  @retval  EFI_SUCCESS      Update successful.
  @retval  other            error status from the update routine
**/
EFI_STATUS
ApplyFwImage (
  IN    UINT8                         *CapImage,
  IN    UINT32                         CapImageSize,
  IN    EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr,
  OUT BOOLEAN                         *ResetRequired
  )
{
  EFI_STATUS              Status;
  UINT32                  Signature;
  BOOT_PARTITION          Partition;
  VOID                    *CsmeUpdateInData;
  FIRMWARE_UPDATE_HEADER  *CapHdr;

  CapHdr = (FIRMWARE_UPDATE_HEADER *)CapImage;

  Status = EFI_SUCCESS;
  *ResetRequired = FALSE;

  if (ImageHdr == NULL) {
    return EFI_NOT_FOUND;
  }

  Signature = (UINT32)ImageHdr->UpdateHardwareInstance;

  switch (Signature) {
  case FW_UPDATE_COMP_BIOS_REGION:
    if ((CapHdr->CapsuleFlags & CAPSULE_FLAG_FORCE_BIOS_UPDATE) != 0) {
      Status = UpdateFullBiosRegion (ImageHdr);
      *ResetRequired = TRUE;
    } else {
      Status = UpdateSystemFirmware (ImageHdr);
    }
    break;
  case FW_UPDATE_COMP_CSME_REGION:
    Status = EFI_UNSUPPORTED;
    if (FeaturePcdGet(PcdCsmeUpdateEnabled) == 1) {
      //
      // CSME update will only work when updated from primary partition
      //
      Partition = (BOOT_PARTITION)GetCurrentBootPartition ();
      if (Partition == BackupPartition) {
        SetBootPartition (PrimaryPartition);
        Reboot (EfiResetCold);
      }
      CsmeUpdateInData = InitCsmeUpdInputData();
      if (CsmeUpdateInData != NULL) {
        Status = UpdateCsme(CapImage, CapImageSize, CsmeUpdateInData, ImageHdr);
        *ResetRequired = FALSE;
      }
    }
    break;
  case FW_UPDATE_COMP_CMD_REQUEST:
    Status = FwCmdUpdateProcess (ImageHdr);
    break;
  default:
    Status = UpdateSblComponent (ImageHdr);
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
  FIRMWARE_UPDATE_HEADER        *CapHdr;

  ImgHdr = NULL;
  FwUpdStatusOffset = PcdGet32(PcdFwUpdStatusBase);
  ResetRequired = FALSE;

  //
  // 1. Get capsule image.
  //
  Status = GetCapsuleImage (&CapsuleImage, &CapsuleSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "GetCapsuleImage failed with status = %r\n", Status));
  }

  //
  // 2. Authenticate capsule image.
  //
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "CapsuleImage: 0x%p, CapsuleSize: 0x%X\n", CapsuleImage, CapsuleSize));
    Status = AuthenticateCapsule (CapsuleImage, CapsuleSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "AuthenticateCapsule, Status = 0x%x\n", Status));
    }
  }

  //
  // 3. Process capsule image.
  //
  if (!EFI_ERROR (Status)) {
    Status = ProcessCapsule (CapsuleImage, CapsuleSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "ProcessCapsule, Status = 0x%x\n", Status));
    }
  }

  //
  // Read the component structure in the reserved region
  //
  if (!EFI_ERROR (Status)) {
    Status = BootMediaRead ((FwUpdStatusOffset + sizeof(FW_UPDATE_STATUS)), \
                            MAX_FW_COMPONENTS * sizeof(FW_UPDATE_COMP_STATUS), (UINT8 *)&FwUpdCompStatus);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "BootMediaRead. offset: 0x%llx, Status = 0x%x\n", (FwUpdStatusOffset + sizeof(FW_UPDATE_STATUS)), Status));
    }
  }

  if (EFI_ERROR (Status)) {
    //
    // Error condition
    // Clear state machine anyway to prevent FWU loop.
    //
    SetStateMachineFlag (FW_UPDATE_SM_DONE);
    return Status;
  }

  //
  // Handle full BIOS region update separately.
  // It needs to consider cases that the user updates firmware from SBL FW to UEFI FW.
  // SBL FWU state machine and status stored in flash should not be updated in a successful
  // update since the new FW might have used this region for other purpose.
  //
  CapHdr = (FIRMWARE_UPDATE_HEADER *)CapsuleImage;
  if ((CapHdr->CapsuleFlags & CAPSULE_FLAG_FORCE_BIOS_UPDATE) != 0) {
    // Only expect a single BIOS component update.
    Status = FindImage (FwUpdCompStatus[0].HardwareInstance, CapsuleImage, CapsuleSize, &ImgHdr);
    if (!EFI_ERROR (Status)) {
      Status = ApplyFwImage(CapsuleImage, CapsuleSize, ImgHdr, &ResetRequired);
    }
    DEBUG ((DEBUG_INFO, "Full BIOS region update status: %r\n", Status));
    if (EFI_ERROR (Status)) {
      // Clear state machine anyway to prevent FWU loop.
      SetStateMachineFlag (FW_UPDATE_SM_DONE);
    }
    // Always reboot since full BIOS was updated
    Reboot (EfiResetCold);
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
        }
      }

      //
      // Find payload associated with component in the capsule image
      //
      Status = FindImage(FwUpdCompStatus[Count].HardwareInstance, CapsuleImage, CapsuleSize, &ImgHdr);
      if (!EFI_ERROR (Status)) {
        //
        // Start firmware udpate for the component, exclude CSME driver (CSMD)
        //
        if ((UINT32)ImgHdr->UpdateHardwareInstance != FW_UPDATE_COMP_CSME_DRIVER) {
          Status = ApplyFwImage(CapsuleImage, CapsuleSize, ImgHdr, &ResetRequired);
          if (EFI_ERROR (Status)) {
            DEBUG((DEBUG_ERROR, "ApplyFwImage failed with Status = %r\n", Status));
          }
        }
      } else {
        DEBUG((DEBUG_ERROR, "FindImage failed with Status = %r\n", Status));
        continue;
      }

      //
      // Update firmware update status of the component in reserved region
      //
      Status = UpdateStatus(ImgHdr->UpdateHardwareInstance, (UINT16)ImgHdr->Version, Status);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "UpdateStatus failed! Status = %r\n", Status));
      }

      //
      // Reset system if required
      //
      if (ResetRequired == TRUE) {
        Reboot (EfiResetCold);
      }
    }
  }

  //
  // At this point, all the firmware images in the capsule are processed
  // Clear the state machine and exit
  //
  if (Count == MAX_FW_COMPONENTS) {
    SetStateMachineFlag(FW_UPDATE_SM_DONE);
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

  if (TopSwapRegionSize != NULL) {
    *TopSwapRegionSize = GetRegionOffsetSize(FlashMap, FLASH_MAP_FLAGS_TOP_SWAP, NULL);
  }
  if (RedundantRegionSize != NULL) {
    *RedundantRegionSize = GetRegionOffsetSize (FlashMap, FLASH_MAP_FLAGS_REDUNDANT_REGION, NULL);
  }
  if (NonRedundantRegionSize != NULL) {
    *NonRedundantRegionSize = GetRegionOffsetSize (FlashMap, FLASH_MAP_FLAGS_NON_REDUNDANT_REGION, NULL);
  }
  return EFI_SUCCESS;
}

/**
  End firmware update.

  This function will clear firmware update trigger and end firmware update.

  @retval  EFI_SUCCESS        Update successfully.
  @retval  others             Error happened during end firmware update.

**/
EFI_STATUS
EFIAPI
EndFirmwareUpdate (
  VOID
  )
{
  EFI_STATUS  Status;

  ClearFwUpdateTrigger ();

  Status = PlatformEndFirmwareUpdate ();
  return Status;
}

/**
  Initialize platform console.

  @retval  EFI_NOT_FOUND    No additional console was found.
  @retval  EFI_SUCCESS      Console has been initialized successfully.
  @retval  Others           There is error during console initialization.
**/
EFI_STATUS
InitConsole (
  VOID
)
{
  EFI_STATUS                Status;
  UINT32                    Height;
  UINT32                    Width;
  UINT32                    OffX;
  UINT32                    OffY;
  EFI_PEI_GRAPHICS_INFO_HOB *GfxInfoHob;

  Status = EFI_NOT_FOUND;

  if (PcdGet32 (PcdConsoleOutDeviceMask) & ConsoleOutFrameBuffer) {
    GfxInfoHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GetGuidHobData (NULL, NULL, &gEfiGraphicsInfoHobGuid);
    if (GfxInfoHob != NULL) {
      Width  = GfxInfoHob->GraphicsMode.HorizontalResolution;
      Height = GfxInfoHob->GraphicsMode.VerticalResolution;
      if ((PcdGet32 (PcdFrameBufferMaxConsoleWidth) > 0) && (Width > PcdGet32 (PcdFrameBufferMaxConsoleWidth))) {
        Width = PcdGet32 (PcdFrameBufferMaxConsoleWidth);
      }
      if ((PcdGet32 (PcdFrameBufferMaxConsoleHeight) > 0) && (Height > PcdGet32 (PcdFrameBufferMaxConsoleHeight))) {
        Height = PcdGet32 (PcdFrameBufferMaxConsoleHeight);
      }
      OffX = (GfxInfoHob->GraphicsMode.HorizontalResolution - Width) / 2;
      OffY = (GfxInfoHob->GraphicsMode.VerticalResolution - Height) / 2;
      Status = InitFrameBufferConsole (GfxInfoHob, Width, Height, OffX, OffY);
    }
  }

  return Status;
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
  UINT32        BiosRgnSize;

  //
  // Prepare Console Print
  //
  InitConsole ();
  ConsolePrint ("Starting Firmware Update\n");

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
    Status = EFI_NO_MAPPING;
    goto EndOfFwu;
  }

  //
  // Get bootloader reserved region base and size
  //
  Status = GetComponentInfoByPartition (FLASH_MAP_SIG_BLRESERVED, FALSE, &RsvdBase, &RsvdSize);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Could not get component information for bootloader reserved region\n"));
    Status = EFI_NO_MAPPING;
    goto EndOfFwu;
  }

  //
  // Get SBL rom image start offset in BIOS region
  // Specially, if SBL rom image occupies the whole BIOS region, this offset is 0
  //
  mSblImageBiosRgnOffset = 0;
  Status = BootMediaGetRegion (FlashRegionBios, NULL, &BiosRgnSize);
  if (!EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "BIOS Region Size: 0x%08X\n", BiosRgnSize));
    DEBUG((DEBUG_INFO, "SBL  ROM    Size: 0x%08X\n", FlashMap->RomSize));
    if (BiosRgnSize < FlashMap->RomSize) {
      Status = EFI_ABORTED;
    } else {
      mSblImageBiosRgnOffset = BiosRgnSize - FlashMap->RomSize;
    }
  }
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Could not fit image into BIOS region\n"));
    goto EndOfFwu;
  }


  //
  // Set PCD for Firmware Update status structure offset within BIOS region
  //
  (VOID) PcdSet32S (PcdFwUpdStatusBase, mSblImageBiosRgnOffset + (FlashMap->RomSize - (~RsvdBase + 1)));

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

EndOfFwu:
  //
  // Terminate firmware update
  //
  ConsolePrint ("Exiting Firmware Update (Status: %r)\n", Status);
  EndFirmwareUpdate ();

  Reboot (EfiResetCold);
}
