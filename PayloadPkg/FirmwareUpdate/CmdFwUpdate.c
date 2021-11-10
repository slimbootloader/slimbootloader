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
#include <Library/StringSupportLib.h>
#include "FirmwareUpdateHelper.h"


#define FLASH_DESCRIPTOR_LOCK_STR     "FLASHDESCLOCK"
#define ARB_SVN_COMMIT_STR            "ARBSVNCOMMIT"
#define OEM_KEY_REVOCATION_STR        "OEMKEYREVOCATION"


typedef UINT32 CMDI_TYPE;
#define CMDI_TYPE_SPI_DESCRIPTOR_LOCK       BIT0
#define CMDI_TYPE_ARB_SVN_COMMIT            BIT1
#define CMDI_TYPE_OEM_KEY_REVOCATIION       BIT2


/*
  Firmware update command handler

  This function would parse the command and execute the
  functionality requested.

  @param[in]  CmdBuf            Command buffer.
  @param[in]  BufLen            Command buffer length
  @param[out] CmdProcessed      Command type processed

  @retval  EFI_SUCCESS      Update successful
  @retval  other            error status from the update routine
*/
EFI_STATUS
FwUpdateCmdHandler (
  IN  CHAR8      *CmdBuf,
  IN  UINTN      BufLen,
  OUT UINT32     *CmdProcessed
   )
{
  EFI_STATUS Status;

  Status = EFI_SUCCESS;
  *CmdProcessed = 0;

  if (AsciiStrnCmp (CmdBuf, FLASH_DESCRIPTOR_LOCK_STR, AsciiStrLen(FLASH_DESCRIPTOR_LOCK_STR)) == 0) {
      *CmdProcessed  =  CMDI_TYPE_SPI_DESCRIPTOR_LOCK;
      Status = SetFlashDescriptorLock (CmdBuf, BufLen);
  } else if (AsciiStrnCmp (CmdBuf, ARB_SVN_COMMIT_STR, AsciiStrLen(ARB_SVN_COMMIT_STR)) == 0) {
      *CmdProcessed  =  CMDI_TYPE_ARB_SVN_COMMIT;
      Status = SetArbSvnCommit (CmdBuf, BufLen);
  } else if (AsciiStrnCmp (CmdBuf, OEM_KEY_REVOCATION_STR, AsciiStrLen(OEM_KEY_REVOCATION_STR)) == 0) {
      *CmdProcessed  =  CMDI_TYPE_OEM_KEY_REVOCATIION;
      Status = SetOemKeyRevocation (CmdBuf, BufLen);
  }

  return Status;
}


/*
 Parse firmware update command data buffer

 Command format:
 {FLASHDESCLOCK}
 {ARBSVNCOMMIT}
 {Command3}

  @param[in]  Buffer            Command buffer.
  @param[in]  BufLength         Command buffer length
  @param[out] CmdsProcessed     Commands processed

  @retval  EFI_SUCCESS      Update successful
  @retval  other            error status from the update routine
*/
EFI_STATUS
ParseCommandStringBuf(
  IN  UINT8       *Buffer,
  IN  UINTN       BufLength,
  OUT CMDI_TYPE   *CmdsProcessed
  )
{
  CHAR8      *CurrLine;
  CHAR8      *NextLine;
  CHAR8      *EndString;
  UINT32      LineLen;
  UINT32      CommandType;
  EFI_STATUS  Status;

  *CmdsProcessed = 0;

  Status = EFI_SUCCESS;
  CurrLine = (CHAR8 *) Buffer;

  while ((CurrLine != NULL) && (CurrLine < ((CHAR8 *) Buffer + BufLength))) {
    NextLine = GetNextLine (CurrLine, &LineLen);
    CurrLine = TrimLeft (CurrLine);

    if (CurrLine[0] == '{') {
      // String format found in line
      CurrLine += 1;
      EndString = AsciiStrStr(CurrLine, "}");

      if(EndString != NULL) {
        // Invoke Fw command handler
        Status = FwUpdateCmdHandler (CurrLine, EndString - CurrLine, &CommandType);
        *CmdsProcessed |= CommandType;
      }
    }
    CurrLine = NextLine;
  }

  return Status;
}


/**
  Main routine for firmware command updates.

  This function has the logic to perform command updates.

  @param[in] CapImageHdr    Pointer to fw mgmt capsule Image header

  @retval  EFI_SUCCESS      Update successful
  @retval  other            error status from the update routine
**/
EFI_STATUS
FwCmdUpdateProcess (
  EFI_FW_MGMT_CAP_IMAGE_HEADER  *CapImageHdr
  )
{
  CMDI_TYPE                       CmdsProcessed;
  EFI_STATUS                      Status;

  if (CapImageHdr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //Parse command data buffer
  Status = ParseCommandStringBuf ((UINT8 *)CapImageHdr + sizeof(EFI_FW_MGMT_CAP_IMAGE_HEADER), CapImageHdr->UpdateImageSize, &CmdsProcessed);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Error in command parse Status = %r\n", Status));
    return Status;
  }

  DEBUG((DEBUG_INFO, "Commands processed = 0x%x\n", CmdsProcessed));

  return Status;
}
