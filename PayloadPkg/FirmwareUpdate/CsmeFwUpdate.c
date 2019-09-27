/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <PiPei.h>
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/LitePeCoffLib.h>
#include <Library/FirmwareUpdateLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/PayloadMemoryAllocationLib.h>
#include <CsmeUpdateDriver.h>

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
  );

/**
  Main routine to update CSME firmware.

  This function has the logic to perform CSME firmware update.

  @param[in] Buffer         Pointer to csme update binary
  @param[in] BufferLength   Csme Update binary length
  @param[in] UpdateApi      Pointer to update service API's

  @retval  EFI_SUCCESS      Update successful
  @retval  other            error status from the update routine
**/
EFI_STATUS
StartCsmeUpdate (
  IN  UINT8     *Buffer,
  IN  UINTN     BufferLength,
  IN  CSME_UPDATE_DRIVER_OUTPUT *UpdateApi
  );

/**
  Display send image status.

  A callback function that reports the progress of sending 
  the update image buffer to FW (not the progress of the update itself).

  @param[in] bytesSentToFw         The number of bytes of the buffer, that were already sent to FW.
  @param[in] totalBytesToSendToFw  The total number of bytes of the buffer.
**/
static void
DisplaySendStatus (
  IN UINT32 bytesSentToFw,
  IN UINT32 totalBytesToSendToFw
  );

/**
  Check if the update image has the same version as the flash image.

  @param[in]  buffer         Buffer of Update Image.
  @param[in]  bufferLength   Length of the buffer in bytes. 
  @param[in]  UpdateApi      Pointer to update service API's
  @param[out] isSameVersion  TRUE if same version, FALSE if not. Caller allocated.

  @retval  EFI_SUCCESS      Update successful
  @retval  other            error status from the routine
**/
static UINT32 
IsUpdateToSameVersion(
  IN  UINT8   *Buffer,
  IN  UINT32  BufferLength,
  IN  CSME_UPDATE_DRIVER_OUTPUT *UpdateApi,
  OUT BOOLEAN *IsSameVersion
  )
{
  UINT32   Status;
  UINT16   flashMajor   = 0;
  UINT16   flashMinor   = 0;
  UINT16   flashHotfix  = 0;
  UINT16   flashBuild   = 0;
  UINT16   bufferMajor  = 0;
  UINT16   bufferMinor  = 0;
  UINT16   bufferHotfix = 0;
  UINT16   bufferBuild  = 0;

  if (Buffer == NULL || BufferLength == 0 || IsSameVersion == NULL) {
    Status = INTERNAL_ERROR;
    return Status;
  }

  *IsSameVersion = FALSE;

  Status = UpdateApi->FwuPartitionVersionFromFlash(FPT_PARTITION_NAME_FTPR, &flashMajor, &flashMinor, &flashHotfix, &flashBuild);
  if (Status != SUCCESS) {
    return Status;
  }

  Status = UpdateApi->FwuPartitionVersionFromBuffer(Buffer, BufferLength, FPT_PARTITION_NAME_FTPR, &bufferMajor, &bufferMinor, &bufferHotfix, &bufferBuild);
  if (Status != SUCCESS) {
    return Status;
  }

  if (flashMajor == bufferMajor &&
      flashMinor == bufferMinor &&
      flashHotfix == bufferHotfix &&
      flashBuild == bufferBuild)
  {
    *IsSameVersion = TRUE;
  }

  DEBUG((DEBUG_ERROR, "OldVer %d.%d.%d.%d\n", flashMajor, flashMinor, flashHotfix, flashBuild));
  DEBUG((DEBUG_ERROR, "NewVer %d.%d.%d.%d\n", bufferMajor, bufferMinor, bufferHotfix, bufferBuild));

  return Status;
}

/**
  Display send image status.

  A callback function that reports the progress of sending 
  the update image buffer to FW (not the progress of the update itself).

  @param[in] bytesSentToFw         The number of bytes of the buffer, that were already sent to FW.
  @param[in] totalBytesToSendToFw  The total number of bytes of the buffer.
**/
static void 
DisplaySendStatus (
  IN UINT32 bytesSentToFw, 
  IN UINT32 totalBytesToSendToFw
  )
{
  UINT32 value = bytesSentToFw * 100 / totalBytesToSendToFw;

  if (value != 100) {
    DEBUG((DEBUG_ERROR, " Sending the update image to FW for verification:  [ %u%% ] \r \n", value));
  } else {
    DEBUG((DEBUG_ERROR, " Sending the update image to FW for verification:  [ COMPLETE ] \n"));
  }
}

/**
  Main routine to update CSME firmware.

  This function has the logic to perform CSME firmware update.

  @param[in] Buffer         Pointer to csme update binary
  @param[in] BufferLength   Csme Update binary length
  @param[in] UpdateApi      Pointer to update service API's

  @retval  EFI_SUCCESS      Update successful
  @retval  other            error status from the update routine
**/
EFI_STATUS
StartCsmeUpdate (
  IN  UINT8     *Buffer,
  IN  UINTN     BufferLength,
  IN  CSME_UPDATE_DRIVER_OUTPUT *UpdateApi
  )
{
  UINT32            UpdateStatus;
  BOOLEAN           AllowSameVersion;
  UINT16            EnabledState;
  BOOLEAN           IsSameVersion;
  UINT32            FirmwareType;
  UINT32            PchSku;
  UINT32            Index;
  UINT32            IndexMod;
  CHAR8             ProgressChar;
  BOOLEAN           InProgress;
  UINT32            CurrentPercent;
  UINT32            FwUpdStatus;
  UINT32            ReqResetType;
  UINT32            Timer;
  UINT32            PreviousPercent;

  AllowSameVersion  = FALSE;
  EnabledState      = FALSE;
  IsSameVersion     = FALSE;
  InProgress        = FALSE;
  FirmwareType      = FWU_FW_TYPE_INVALID;
  PchSku            = FWU_PCH_SKU_INVALID;
  Timer             = 0;
  Index             = 0;
  IndexMod          = 0;
  CurrentPercent    = 0;
  PreviousPercent   = 0;
  FwUpdStatus       = 0;
  ReqResetType      = MFT_PART_INFO_EXT_UPDATE_ACTION_NONE;

  DEBUG((DEBUG_ERROR, "CSME Firmware Update \n"));

  //
  // Check FWU enabled state
  //
  UpdateStatus = UpdateApi->FwuEnabledState(&EnabledState);
  if (UpdateStatus != SUCCESS) {
    goto End;
  }

  if (EnabledState == FW_UPDATE_DISABLED) {
    UpdateStatus = FWU_LOCAL_DIS;
    goto End;
  }

  //
  // For full update, check if update to the same version
  //
  UpdateStatus = IsUpdateToSameVersion(Buffer, (UINT32)BufferLength, UpdateApi, &IsSameVersion);
  if (UpdateStatus != SUCCESS) {
    goto End;
  }

  //
  // Need /s option for same version
  //
  if (IsSameVersion && !AllowSameVersion) {
    UpdateStatus = FWU_ALLOWSV_RS_MISSING;
    goto End;
  }

  //
  // Display FW Type
  //
  UpdateStatus = UpdateApi->FwuFwType(&FirmwareType);
  if (UpdateStatus != SUCCESS) {
    goto End;
  }

  switch (FirmwareType) {
  case FWU_FW_TYPE_CONSUMER:
    DEBUG((DEBUG_ERROR, "Consumer type Firmware found \n"));
    break;
  case FWU_FW_TYPE_CORPORATE:
    DEBUG((DEBUG_ERROR, "Corporate type Firmware found \n"));
    break;
  default:
    DEBUG((DEBUG_ERROR, "FW type is not supported for FW Update \n"));
    break;
  }

  //
  // Display PCH SKU
  //
  UpdateStatus = UpdateApi->FwuPchSku(&PchSku);
  if (UpdateStatus != SUCCESS) {
    goto End;
  }

  switch (PchSku) {
  case FWU_PCH_SKU_H:
    DEBUG((DEBUG_ERROR, "PCH SKU is: H \n"));
    break;
  case FWU_PCH_SKU_LP:
    DEBUG((DEBUG_ERROR, "PCH SKU is: LP \n"));
    break;
  default:
    DEBUG((DEBUG_ERROR, "PCH SKU is not identified \n"));
    break;
  }

  DEBUG((DEBUG_ERROR, "\n Executing Full FW Update \n"));
  DEBUG((DEBUG_ERROR, "\n Warning: Do not exit the process or power off the machine before the firmware update is completed. \n"));
  UpdateStatus = UpdateApi->FwuFullUpdateFromBuffer(
                              Buffer,
                              (UINT32)BufferLength,
                              NULL,
                              &DisplaySendStatus
                              );
  if (UpdateStatus != SUCCESS) {
    goto End;
  }

  DEBUG((DEBUG_ERROR, "\n\n\n                        Do not Interrupt.\r"));

  //
  // Image sent to FW Update client
  // poll the FW Update progress until finished
  //
  do
  {
    //
    // we mod4 the index to determine which ascii animation frame to display for this iteration
    // loop through (|) (/) (-) (\) (|) (/) ...
    //
    IndexMod = (++Index % 4);
    switch (IndexMod) {
    case CMD_LINE_STATUS_UPDATE_1:
      ProgressChar = '|';
      break;
    case CMD_LINE_STATUS_UPDATE_2:
      ProgressChar = '/';
      break;
    case CMD_LINE_STATUS_UPDATE_3:
      ProgressChar = '-';
      break;
    case CMD_LINE_STATUS_UPDATE_4:
      ProgressChar = '\\';
      break;
    default:
      break;
    }

    UpdateStatus = UpdateApi->FwuCheckUpdateProgress(
                                &InProgress,
                                &CurrentPercent,
                                &FwUpdStatus,
                                &ReqResetType
                                );
    if (UpdateStatus != SUCCESS) {
      //
      // update failed
      //
      break;
    }

    //
    // Update finished successfully
    //
    if (!InProgress)
    {
      //
      // Print 100% for customers output consistency
      //
      DEBUG((DEBUG_ERROR, "FW Update:  [ 100%% (%c)]\r", ProgressChar));
      break;
    }

    //
    // Update in progress
    //
    DEBUG((DEBUG_ERROR, "FW Update:  [ %3u%% (%c)]\r", CurrentPercent, ProgressChar));

    //
    // Wait 250 milliseconds before polling again
    //
    MicroSecondDelay (250000);
    //
    // If 30 second passed
    //
    if (Timer >= 30000) {
      //
      // If percent didn't change in 30 seconds
      //
      if (CurrentPercent == PreviousPercent) {
        UpdateStatus = FWU_UPDATE_TIMEOUT;
        break;
      }

      //
      // Percent changed
      //
      PreviousPercent = CurrentPercent;
      Timer = 0;
    }
    else {
      Timer += 250;
    }
  } while (TRUE);

  //
  // New line after printing all the percents.
  //
  DEBUG((DEBUG_ERROR, "\n"));
  DEBUG((DEBUG_ERROR, "FW Error Code: %u.\n", FwUpdStatus));

  if (UpdateStatus != SUCCESS) {
    goto End;
  }

  switch (ReqResetType) {
  case MFT_PART_INFO_EXT_UPDATE_ACTION_NONE:
  case MFT_PART_INFO_EXT_UPDATE_ACTION_CSE_RESET:
    DEBUG((DEBUG_ERROR, "FW Update completed successfully. \n"));
    break;
  case MFT_PART_INFO_EXT_UPDATE_ACTION_HOST_RESET:
  case MFT_PART_INFO_EXT_UPDATE_ACTION_GLOBAL_RESET:
  default:
    DEBUG((DEBUG_ERROR, "FW Update completed successfully. New firmware will take effect after a reboot\n"));
    break;
  }

End:
  if (UpdateStatus != SUCCESS) {
    DEBUG((DEBUG_ERROR, "%a\n", GetErrorString (UpdateStatus)));
    return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}

/**
  Perform csme Firmware update.

  This function based on the image type id guid from the image header will
  call the respective functions to perform capsule update.

  @param[in] CapImage       The pointer to the firmware update capsule image.
  @param[in] CapImageSize   The size of capsule image in bytes.
  @param[in] CsmeUpdInputData   pointer to input data structure for CSME update
  @param[in] ImageHdr       Pointer to fw mgmt capsule Image header

  @retval  EFI_SUCCESS      Update successful.
  @retval  other            error status from the update routine
**/
EFI_STATUS
UpdateCsme (
  IN  UINT8                         *CapImage,
  IN  UINT32                        CapImageSize,
  IN  VOID                          *CsmeUpdInputData,
  IN  EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr
  )
{
  EFI_STATUS                    Status;
  UINT32                        *DriverPtr;
  DRIVER_ENTRY                  DriverEntry;
  CSME_UPDATE_DRIVER_OUTPUT     *CsmeUpdateApi;
  CSME_UPDATE_DRIVER_PARAMS     DriverParams;
  EFI_FW_MGMT_CAP_IMAGE_HEADER  *CsmeDriverImageHdr;

  DriverPtr           = NULL;
  CsmeUpdateApi       = NULL;
  CsmeDriverImageHdr  = NULL;

  if ((CapImage == NULL) || (CapImageSize == 0) || (ImageHdr == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem((VOID *)&DriverParams, sizeof(DriverParams));

  Status = FindImage(&gCsmeFWUDriverImageFileGuid, CapImage, CapImageSize, &CsmeDriverImageHdr);
  if ((EFI_ERROR (Status)) || (CsmeDriverImageHdr == NULL)) {
    DEBUG((DEBUG_ERROR, "Finding CSME update driver failed with Status = %r\n", Status));
    return Status;
  }

  DriverPtr = (UINT32 *)((UINT32)CsmeDriverImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));

  Status = PeCoffRelocateImage ((UINT32)DriverPtr);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Relocate CSME Update driver failed with status = %r\n", Status));
    return Status;
  }

  Status = PeCoffLoaderGetEntryPoint(DriverPtr, (VOID *)&DriverEntry);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Getting entry point for CSME Update driver failed with status = %r\n", Status));
    return Status;
  }

  DriverParams.CsmeUpdDriverInFunc = CsmeUpdInputData;
  *(DriverParams.OutputFunc) = NULL;

  DriverEntry((VOID *)&DriverParams);

  CsmeUpdateApi = *(DriverParams.OutputFunc);

  if ( CsmeUpdateApi == NULL ) {
    DEBUG((DEBUG_ERROR, "Unable to initialize update routines for CSME update \n"));
    return EFI_UNSUPPORTED;
  }

  DEBUG((DEBUG_ERROR, "--------------------------------------------------------\n"));
  DEBUG((DEBUG_ERROR, "--------------------CSME FW Update START ---------------\n"));
  DEBUG((DEBUG_ERROR, "--------------------------------------------------------\n"));

  DriverPtr = (UINT32 *)((UINT32)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));

  Status = StartCsmeUpdate((VOID *)DriverPtr, ImageHdr->UpdateImageSize, CsmeUpdateApi);

  return Status;
}

