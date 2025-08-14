/** @file

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <PiPei.h>
#include <Uefi/UefiBaseType.h>
#include <Library/PciLib.h>
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
  );

/**
  Reads a range of PCI configuration registers into a caller supplied buffer.

  Reads the range of PCI configuration registers specified by StartAddress and
  Size into the buffer specified by Buffer. This function only allows the PCI
  configuration registers from a single PCI function to be read. Size is
  returned. When possible 32-bit PCI configuration read cycles are used to read
  from StartAddress to StartAddress + Size. Due to alignment restrictions, 8-bit
  and 16-bit PCI configuration read cycles may be used at the beginning and the
  end of the range.

  StartAddress is in EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS format.
  - when register offset is  < 0x100, it is :    bbddffrr
  - when register offset is >= 0x100, it is : rrrbbddff00

  If StartAddress is not aligned with format defined, then ASSERT().
  If the range to be read exceeds a single PCI function, then ASSERT().
  If Buffer is NULL or Size == 0, then ASSERT().

  @param  StartAddress  The starting address that encodes the PCI Bus, Device,
                        Function and Register.
  @param  Size          The size in bytes of the transfer.
  @param  Buffer        The pointer to a buffer receiving the data read.

  @return EFI_SUCCESS        if data is read into buffer
  @return EFI_NOT_FOUND      if data is NOT read into buffer
  @return EFI_INVALID_PARAMETER  Invalid parameter
**/
EFI_STATUS
EFIAPI
CsmePciReadBuffer (
  IN      UINT64    StartAddress,
  IN      UINTN     Size,
  OUT     VOID      *Buffer
  )
{
  UINT8   Bus;
  UINT8   Device;
  UINT8   Function;
  UINT16  Register;
  UINTN   ReadCount;
  UINT32  StartAddressHi;
  UINT32  StartAddressLo;

  if ((Buffer == NULL) || (Size == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // A valid PCI address should contain 1's only in the low 32 bits or in bit range [43:8]
  //
  StartAddressHi = (UINT32)RShiftU64 (StartAddress, 32);
  StartAddressLo = (UINT32)StartAddress;

  if ((StartAddressHi & (~0xFFF)) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (((StartAddressHi & 0xFFF) != 0) && ((StartAddressLo & 0xFF) != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Register = (UINT16)(StartAddressHi & 0xFFF);
  if (Register == 0) {
    Register = (UINT16)(StartAddressLo & 0xFF);
  }

  if ((Register + Size) > 0x1000) {
      return EFI_INVALID_PARAMETER;
  }

  Bus = (UINT8)((StartAddressLo >> 24) & 0xFF);
  Device = (UINT8)((StartAddressLo >> 16) & 0xFF);
  Function = (UINT8)((StartAddressLo >> 8) & 0xFF);

  ReadCount = PciReadBuffer (
                PCI_LIB_ADDRESS(Bus, Device, Function, Register),
                Size * sizeof(UINT32),
                Buffer
                );

  if (ReadCount != 0) {
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

/**
  Display send image status.

  A callback function that reports the progress of sending
  the update image buffer to FW (not the progress of the update itself).

  @param[in] bytesSentToFw         The number of bytes of the buffer, that were already sent to FW.
  @param[in] totalBytesToSendToFw  The total number of bytes of the buffer.
**/
static void
EFIAPI
DisplaySendStatus (
  IN UINT32 bytesSentToFw,
  IN UINT32 totalBytesToSendToFw
  )
{
  UINT32 value = bytesSentToFw * 100 / totalBytesToSendToFw;

  if (value != 100) {
    DEBUG((DEBUG_ERROR, " Sending the update image to FW for verification:  [ %u%% ] \r", value));
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
  UINT16            EnabledState;
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

  EnabledState      = FALSE;
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
    DEBUG((DEBUG_ERROR, "CSME update failed. Error Code: 0x%X\n", UpdateStatus));
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

  ZeroMem ((VOID *)&DriverParams, sizeof(DriverParams));

  Status = FindImage (FW_UPDATE_COMP_CSME_DRIVER, CapImage, CapImageSize, &CsmeDriverImageHdr);
  if ((EFI_ERROR (Status)) || (CsmeDriverImageHdr == NULL)) {
    DEBUG((DEBUG_ERROR, "Finding CSME update driver failed with Status = %r\n", Status));
    return Status;
  }

  DriverPtr = (UINT32 *)((UINTN)CsmeDriverImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));

  Status = PeCoffRelocateImage ((UINT32)(UINTN)DriverPtr);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Relocate CSME Update driver failed with status = %r\n", Status));
    return Status;
  }

  Status = PeCoffLoaderGetEntryPoint (DriverPtr, (VOID *)&DriverEntry);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Getting entry point for CSME Update driver failed with status = %r\n", Status));
    return Status;
  }

  DriverParams.CsmeUpdDriverInFunc = CsmeUpdInputData;
  *(DriverParams.OutputFunc) = NULL;

  DriverEntry ((VOID *)&DriverParams);

  CsmeUpdateApi = *(DriverParams.OutputFunc);

  if ( CsmeUpdateApi == NULL ) {
    DEBUG((DEBUG_ERROR, "Unable to initialize update routines for CSME update \n"));
    return EFI_UNSUPPORTED;
  }

  DEBUG((DEBUG_ERROR, "--------------------------------------------------------\n"));
  DEBUG((DEBUG_ERROR, "--------------------CSME FW Update START ---------------\n"));
  DEBUG((DEBUG_ERROR, "--------------------------------------------------------\n"));

  DriverPtr = (UINT32 *)((UINTN)ImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER));

  Status = StartCsmeUpdate((VOID *)DriverPtr, ImageHdr->UpdateImageSize, CsmeUpdateApi);

  return Status;
}

