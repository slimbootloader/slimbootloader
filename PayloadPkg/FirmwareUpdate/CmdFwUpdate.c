/** @file
  This file contains implementation of Firmware Command update library.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <PiPei.h>
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/LitePeCoffLib.h>
#include <Library/FirmwareUpdateLib.h>
#include <Library/StringSUpportLib.h>


#define FLASH_DESCRIPTOR_LOCK_STR     "FLASHDESCLOCK"
#define NAMED_VARIABLE_LOCK_STR       "NVARLOCK"

typedef UINT32 CMDI_TYPE;
#define CMDI_TYPE_SPI_DESCRIPTOR_LOCK       BIT0
#define CMDI_TYPE_NAMED_VARIABLE_LOCK       BIT1

typedef struct {
  // Commands requested
  CMDI_TYPE   CmdRequested;
  // Flash Descriptor Lock command Data
  CHAR8        *FlashDescLockDataBuf;
  // Flash Descriptor Lock command Data length
  UINT32      FlashDescLockDataLen;
  // NVar region Lock command Data
  CHAR8        *NVarLockDataBuf;
  // NVar region Lock command Data length
  UINT32      NavrLockDataLen;
} CMDI_DATA;


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

/*
 Parse firmware update command data buffer

 Command format:
 {FLASHDESCLOCK}
 {NVARLOCK}

  @param[in]  Buffer      Command buffer.
  @param[in]  BufLength   Command buffer length
  @param[out] CmdiData    Command data parsed
*/
EFI_STATUS
ParseCommandStringBuf(
  IN  UINT8       *Buffer,
  IN  UINTN       BufLength,
  OUT CMDI_DATA   *CmdiData
  )
{
  CHAR8      *CurrLine;
  CHAR8      *NextLine;
  CHAR8      *EndLine;
  CHAR8      *EndString;
  UINT32      LineLen;
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;
  CurrLine = (CHAR8 *) Buffer;

  while ((CurrLine != NULL) && (CurrLine < (Buffer + BufLength))) {
    NextLine = GetNextLine (CurrLine, &LineLen);
    EndLine  = CurrLine + LineLen;
    CurrLine = TrimLeft (CurrLine);

    if (CurrLine[0] == '{') {
      // String format found in line
      CurrLine += 1;
      EndString = AsciiStrStr(CurrLine, "}");

      if(EndString != NULL) {
        if(AsciiStrnCmp(CurrLine, FLASH_DESCRIPTOR_LOCK_STR, AsciiStrLen(FLASH_DESCRIPTOR_LOCK_STR)) == 0) {
          CmdiData->FlashDescLockDataBuf = CurrLine;
          CmdiData->FlashDescLockDataLen = EndString - CurrLine;
          CmdiData->CmdRequested        |=  CMDI_TYPE_SPI_DESCRIPTOR_LOCK;
        } else if (AsciiStrnCmp(CurrLine, NAMED_VARIABLE_LOCK_STR, AsciiStrLen(NAMED_VARIABLE_LOCK_STR)) == 0) {
          CmdiData->NVarLockDataBuf      = CurrLine;
          CmdiData->NavrLockDataLen      = EndString - CurrLine;
          CmdiData->CmdRequested        |=  CMDI_TYPE_NAMED_VARIABLE_LOCK;
        }
      }
    }
    CurrLine = NextLine;
  }

  return Status;
}


/**
  Main routine for firmware command updates.

  This function has the logic to perform command updates.

  @param[in] CapImage         The pointer to the firmware update capsule image.
  @param[in] CapImageSize     The size of capsule image in bytes.

  @retval  EFI_SUCCESS      Update successful
  @retval  other            error status from the update routine
**/
EFI_STATUS
FwCmdUpdateProcess (
  IN  UINT8     *CapImage,
  IN  UINTN     CapImageSize
  )
{
  CMDI_DATA                       CmdData;
  EFI_FW_MGMT_CAP_IMAGE_HEADER    *CapImageHdr;
  EFI_STATUS                      Status;

  if ((CapImage == NULL) ||  (CapImageSize == 0)){
    return EFI_INVALID_PARAMETER;
  }

  // Find the command update region
  Status = FindImage (FW_UPDATE_COMP_CMD_UPDATE, CapImage, CapImageSize, &CapImageHdr);
  if ((EFI_ERROR (Status)) || (CapImageHdr == NULL)) {
    DEBUG((DEBUG_ERROR, "Finding CMDI update failed with Status = %r\n", Status));
    return Status;
  }

  ZeroMem (&CmdData, sizeof (CmdData));

  //Parse command data buffer
  Status = ParseCommandStringBuf ((CHAR8 *)CapImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER), CapImageHdr->UpdateImageSize, &CmdData);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Error in command parse Status = %r\n", Status));
    return Status;
  }

  DEBUG((DEBUG_ERROR, "Commands Requested = 0x%x\n", CmdData.CmdRequested));

  if(CmdData.CmdRequested & CMDI_TYPE_SPI_DESCRIPTOR_LOCK) {
    DEBUG((DEBUG_ERROR, "FLASH DESCRIPTOR LOCK COMMAND RECEIVED!! \n"));
    // Process SPI Descriptor Lock
    Status = SetFlashDescriptorLock (CmdData.FlashDescLockDataBuf, CmdData.FlashDescLockDataLen);
  }
  if(CmdData.CmdRequested & CMDI_TYPE_NAMED_VARIABLE_LOCK) {
    DEBUG((DEBUG_ERROR, "NVAR REGION LOCK COMMAND RECEIVED!! \n"));
    // Process Named variable region lock
    Status = MeNvarRegionLock (CmdData.NVarLockDataBuf, CmdData.NavrLockDataLen);
  }
  return Status;
}
