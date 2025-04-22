/** @file
  Heci platform specific library

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/HeciLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <IndustryStandard/Pci.h>
#include <CseMsg.h>
#include <MeBiosPayloadData.h>

STATIC CONST HECI_SERVICE   mHeciService = {
  .Header.Signature   = HECI_SERVICE_SIGNATURE,
  .Header.Version     = HECI_SERVICE_VERSION,
  .HeciSend           = HeciServiceSend,
  .HeciReceive        = HeciServiceReceive,
  .HeciResetInterface = HeciServiceResetInterface,
  .SimpleHeciCommand  = SimpleHeciCommand,
  .HeciUserCommand    = HeciSendUserCommand,
};

/**
  This function provides a PCI config space memory address to access a HECI
  device and its register.

  @param[in] HeciDev              The HECI device to be accessed
  @param[in] Register             A register offset

  @retval HeciMmPciAddress        PCI config space memory address

**/
UINTN
EFIAPI
MeGetHeciMmPciAddress (
  IN HECI_DEVICE                  HeciDev,
  IN UINTN                        Register
  )
{
  UINTN   HeciMmPciAddress;

  switch (HeciDev) {
    case HECI1_DEVICE:
      HeciMmPciAddress = (UINTN)MM_PCI_ADDRESS (
        ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER + HeciDev, Register);
      break;
    default:
      HeciMmPciAddress = 0;
      DEBUG ((DEBUG_ERROR, "[HeciInfo] Invalid HECI%d device!", HeciDev));
      ASSERT (FALSE);
      break;
  }

  return HeciMmPciAddress;
}

/**
  Whether HECI timeouts are enabled or not.

  @retval TRUE             Timeout is enabled
  @retval FALSE            Timeout is disabled

**/
BOOLEAN
EFIAPI
MeHeciTimeoutsEnabled (
  VOID
  )
{
  return TRUE;
}

/**
  Ish HECI device

  @retval Ish HECI device
  @retval 0xFF if Ish HECI device is not supported

**/
HECI_DEVICE
EFIAPI
MeGetIshHeciDevice (
  VOID
  )
{
  return HECI_INVALID_DEVICE;
}

/**
  Get ME BIOS Payload data pointer

  @return MbpHeader             A pointer to MBP data header.
  @return NULL                  NULL in case MBP not found.

 **/
VOID *
EFIAPI
MeGetMeBiosPayloadHob (
  VOID
  )
{
  UINT32                    Length;
  VOID                     *MbpHeader;
  VOID                     *FspHobList;

  MbpHeader  = NULL;
  Length     = 0;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    MbpHeader  = GetGuidHobData (FspHobList, &Length, &gEfiHeciMbpDataHobGuid);
  }
  if ((MbpHeader == NULL) || (Length == 0)){
    DEBUG ((DEBUG_ERROR, "MBP Data Hob not Found!\n"));
    MbpHeader = NULL;
  }

  return MbpHeader;
}

/**
  This function return version info from MBP Hob.

  @param[in] AckData                Return FW version from MBP

  @retval    EFI_SUCCESS            Command succeeded
  @retval    EFI_INVALID_PARAMETER  No AckData pointer
  @retval    EFI_NOT_FOUND          No version info found from MBP

**/
EFI_STATUS
EFIAPI
MeGetFwVersionFromMbp (
  IN  GEN_GET_FW_VER_ACK_DATA *AckData
  )
{
  EFI_STATUS                Status;
  UINT8                     Index;
  MBP_CMD_RESP_DATA        *MbpHeader;
  MBP_ITEM_HEADER          *MbpItemHeader;
  MBP_FW_VERSION_NAME      *MbpVersion;

  if (AckData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_NOT_FOUND;
  MbpHeader = (MBP_CMD_RESP_DATA *)MeGetMeBiosPayloadHob ();
  if (MbpHeader == NULL) {
    return Status;
  }

  MbpVersion = NULL;
  MbpItemHeader = (MBP_ITEM_HEADER *)(MbpHeader + 1);
  for (Index = 0; Index < MbpHeader->ItemsNum; Index++) {
    if ((MbpItemHeader->Fields.AppId == MBP_APP_ID_KERNEL) &&
        (MbpItemHeader->Fields.ItemId == MBP_ITEM_ID_FW_VER_NAME)) {
      MbpVersion = (MBP_FW_VERSION_NAME *)(MbpItemHeader + 1);
      break;
    }
    MbpItemHeader = (MBP_ITEM_HEADER *)((UINT32 *)MbpItemHeader + MbpItemHeader->Fields.Length);
  }

  if (MbpVersion != NULL) {
    AckData->CodeMinor   = MbpVersion->MinorVersion;
    AckData->CodeMajor   = MbpVersion->MajorVersion;
    AckData->CodeBuildNo = MbpVersion->BuildVersion;
    AckData->CodeHotFix  = MbpVersion->HotfixVersion;
    DEBUG ((DEBUG_INFO, "FwVersion found from MBP!\n"));
    Status = EFI_SUCCESS;
  }

  return Status;
}

/**
  Provide Heci Service Instance

  @retval HECI_SERVICE            Return Heci Service Instance Pointer

**/
CONST
HECI_SERVICE *
EFIAPI
MeGetHeciServiceInstance (
  VOID
  )
{
  return &mHeciService;
}

/**
  Extend the PCR for measured boot support

  @param[in]  Pcr

  @return EFI_STATUS

**/
EFI_STATUS
EFIAPI
PcrExtend (
  IN  UINT8                      *Pcr
  )
{
  EFI_STATUS Status;
  UINT32 HeciSendLength;
  UINT32 HeciRecvLength;
  MKHI_BOOTLOADER_PCR_EXTEND_RESP_DATA *PCRResp;
  MKHI_BOOTLOADER_PCR_EXTEND_REQ_DATA *SendPCR;
  UINT8 DataBuffer[sizeof (MKHI_BOOTLOADER_PCR_EXTEND_REQ_DATA)];

  DEBUG ((DEBUG_INFO, "Start Send HECI Message: \n"));

  SendPCR = (MKHI_BOOTLOADER_PCR_EXTEND_REQ_DATA*)DataBuffer;
  SendPCR->MkhiHeader.Fields.GroupId = MKHI_BOOTLOADER_PCR_EXTEND_GRP_ID;
  SendPCR->MkhiHeader.Fields.Command = MKHI_BOOTLOADER_PCR_EXTEND_CMD_REQ;
  if (Pcr != NULL) {
    CopyMem ((VOID*)SendPCR->Pcr, (VOID*)Pcr, MKHI_BOOTLOADER_PCR_LEN);
  }

  HeciSendLength = sizeof (MKHI_BOOTLOADER_PCR_EXTEND_REQ_DATA);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendwAck (
               HECI1_DEVICE,
               (UINT32 *)DataBuffer,
               HeciSendLength,
               &HeciRecvLength,
               BIOS_FIXED_HOST_ADDR,
               HECI_MKHI_MESSAGE_ADDR
            );

  PCRResp = (MKHI_BOOTLOADER_PCR_EXTEND_RESP_DATA*)DataBuffer;

  DEBUG ((DEBUG_INFO, "Group    =%08x\n", PCRResp->MkhiHeader.Fields.GroupId));
  DEBUG ((DEBUG_INFO, "Command  =%08x\n", PCRResp->MkhiHeader.Fields.Command));
  DEBUG ((DEBUG_INFO, "IsRespone=%08x\n", PCRResp->MkhiHeader.Fields.IsResponse));
  DEBUG ((DEBUG_INFO, "Result   =%08x\n", PCRResp->MkhiHeader.Fields.Result));

  return Status;
}

/**
  Prepare CSE for firmware update

  @return EFI_STATUS
**/
EFI_STATUS
PrepareCseForFirmwareUpdate (
  VOID
  )
{
  UINT8   DataBuffer[sizeof (HECI_RES_IFWI_PREPARE_FOR_UPDATE)];
  UINT32  HeciSendLength;
  UINT32  HeciRecvLength;
  EFI_STATUS  Status;
  HECI_REQ_IFWI_PREPARE_FOR_UPDATE *Request;
  HECI_RES_IFWI_PREPARE_FOR_UPDATE *Response;

  ZeroMem(DataBuffer, sizeof(DataBuffer));
  Response = (HECI_RES_IFWI_PREPARE_FOR_UPDATE*)DataBuffer;

  Request = (HECI_REQ_IFWI_PREPARE_FOR_UPDATE*)DataBuffer;
  Request->MkhiHeader.Fields.GroupId = MKHI_IFWI_UPDATE_GROUP_ID;
  Request->MkhiHeader.Fields.Command = IFWI_PREPARE_FOR_UPDATE_CMD_ID;
  Request->ResetType = 1;

  HeciSendLength = sizeof (HECI_REQ_IFWI_PREPARE_FOR_UPDATE);
  HeciRecvLength = sizeof (HECI_RES_IFWI_PREPARE_FOR_UPDATE);

  Status = HeciSendwAck (
               HECI1_DEVICE,
               (UINT32 *)DataBuffer,
               HeciSendLength,
               &HeciRecvLength,
               BIOS_FIXED_HOST_ADDR,
               HECI_MKHI_MESSAGE_ADDR
            );

  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (Response->MkhiHeader.Fields.Result != 0x0) {
    DEBUG ((DEBUG_ERROR, "Rejected request IFWI prepare update \n"));
    return EFI_ACCESS_DENIED;
  }

  if (Response->Flag == 0x02) {
    return EFI_SUCCESS;
  } else {
    DEBUG ((DEBUG_ERROR, "HECI/CSE prepare for update not ready yet \n"));
    return EFI_DEVICE_ERROR;
  }

}

/**
  Set HECI to DNX mode.

  @return EFI_SUCCESS   Enter DNX mode success.
  @return Others        Enter DNX mode failed.

**/
STATIC
EFI_STATUS
EnterDnxMode (
  VOID
)
{
  UINT8  DataBuffer[sizeof (HECI_REQ_CSE_DNX_REQ)];
  UINT32 HeciSendLength;
  UINT32 HeciRecvLength;
  EFI_STATUS Status;
  HECI_REQ_CSE_DNX_REQ *Request;
  HECI_RES_CSE_DNX_REQ *Response;

  ZeroMem (DataBuffer, sizeof (DataBuffer));

  Request = (HECI_REQ_CSE_DNX_REQ*)DataBuffer;
  Request->MkhiHeader.Fields.GroupId = 0x0D;
  Request->MkhiHeader.Fields.Command = 0x01;

  HeciSendLength = sizeof (HECI_REQ_CSE_DNX_REQ);
  HeciRecvLength = sizeof (HECI_RES_CSE_DNX_REQ);

  Status = HeciSendwAck (
               HECI1_DEVICE,
               (UINT32 *)DataBuffer,
               HeciSendLength,
               &HeciRecvLength,
               BIOS_FIXED_HOST_ADDR,
               HECI_MKHI_MESSAGE_ADDR
            );

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Response = (HECI_RES_CSE_DNX_REQ*)DataBuffer;

  if (Response->MkhiHeader.Fields.Result == 0 && Response->ReqBiosAction == 0x1) {
    // Cold reset
    IoWrite8 (0xcf9, 0xE);
    CpuDeadLoop ();
  }

  return Status;
}

/**
  Dump the CSE device status.

  @retval EFI_SUCCESS         Dumping the status succeeded
  @retval EFI_NOT_FOUND       The CSE device is not found

**/
STATIC
EFI_STATUS
DumpCseStatus (
  VOID
)
{
  UINTN                   HeciMmPciAddress;
  UINT64                  HeciBar;
  UINT32                  MeFwSts1;
  UINT32                  HeciCsrHost;
  UINT32                  HeciCsrMeHra;

  HeciMmPciAddress = MeGetHeciMmPciAddress (HECI1_DEVICE, 0);
  HeciBar = (UINTN) MmioRead32 (HeciMmPciAddress + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFFFF0;
  if (IS_X64 && ((MmioRead32 (HeciMmPciAddress + PCI_BASE_ADDRESSREG_OFFSET) & 0x6) == 0x4)) {
    HeciBar += LShiftU64 (MmioRead32 (HeciMmPciAddress + (PCI_BASE_ADDRESSREG_OFFSET + 4)), 32);
  }
  if (HeciBar == 0) {
    return EFI_UNSUPPORTED;
  }

  MeFwSts1     = MmioRead32 (HeciMmPciAddress + R_ME_HFS);
  HeciCsrHost  = MmioRead32 ((UINTN)HeciBar + H_CSR);
  HeciCsrMeHra = MmioRead32 ((UINTN)HeciBar + ME_CSR_HA);

  DEBUG ((DEBUG_INFO, "HECI FW STS: 0x%08x\n", MeFwSts1));
  DEBUG ((DEBUG_INFO, "HOST CSR   : 0x%08x\n", HeciCsrHost));
  DEBUG ((DEBUG_INFO, "ME CSR     : 0x%08x\n", HeciCsrMeHra));

  return EFI_SUCCESS;
}

/**
  Wrapper function to for the HECI command without parameter.

  @param[in] SubCmd           The HECI sub-command

  @retval EFI_SUCCESS         The sub command runs success
  @retval Others              The sub command doesn't run success

**/
EFI_STATUS
EFIAPI
SimpleHeciCommand (
  IN UINT32     SubCmd
  )
{
  EFI_STATUS    Status;

  Status = EFI_UNSUPPORTED;

  switch (SubCmd) {
    case SUBCMD_ENTER_DNX_MODE:
      Status = EnterDnxMode ();
      break;
    case SUBCMD_DUMP_CSE_STATUS:
      Status = DumpCseStatus ();
      break;
    default:
      break;
  }

  return Status;
}

/**
  HeciSendUserCommand

  @param[in,out]  Data
  @param[in]      Length
  @param[in]      Force

  @return EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciSendUserCommand (
  IN OUT UINT8 *Data,
  IN     UINT8 Length,
  IN     UINT8 Force
  )
{
  UINTN  Status;
  UINT32 HeciSendLength;
  UINT32 HeciRecvLength;
  UINT32 MeMode;
  UINT8  DataBuffer[sizeof (HECI_USER_CMD_REQUEST)];
  HECI_USER_CMD_REQUEST *SendCommand;
  HECI_USER_CMD_RESPONSE *CommandResp;

  if (Length == 0 && (!Force)) {
    DEBUG ((DEBUG_INFO, "No need Sending HeciSendUserCommandClear.\n"));
    return EFI_SUCCESS;
  }
  Status = HeciGetMeMode (&MeMode);
  if (EFI_ERROR(Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_DEVICE_ERROR;
  }

  ZeroMem (DataBuffer, sizeof (DataBuffer));

  SendCommand = (HECI_USER_CMD_REQUEST*) DataBuffer;
  SendCommand->MkhiHeader.Fields.GroupId = MKHI_IFWI_UPD_SIG;
  SendCommand->MkhiHeader.Fields.Command = MKHI_IFWI_UPD_SIG_CMD;
  SendCommand->SubCommand = (Length == 0) ? SEND_CLEAR_SUB_COMMAND :
                             SEND_USR_CMD_SUB_COMMAND;

  //sanity check
  if (Length > 0) {
    if (Length > CSE_USRCMD_SIZE) {
      Length = CSE_USRCMD_SIZE;
    }

    CopyMem (SendCommand->Data, Data, Length);
  }

  HeciSendLength = sizeof (HECI_USER_CMD_REQUEST);
  HeciRecvLength = sizeof (HECI_USER_CMD_RESPONSE);

  Status = HeciSendwAck (
               HECI1_DEVICE,
               (UINT32 *)DataBuffer,
               HeciSendLength,
               &HeciRecvLength,
               BIOS_FIXED_HOST_ADDR,
               HECI_MKHI_MESSAGE_ADDR
            );

  CommandResp = (HECI_USER_CMD_RESPONSE*) DataBuffer;

  DEBUG ((DEBUG_INFO,  "Group    =%08x\n", CommandResp->Header.Fields.GroupId));
  DEBUG ((DEBUG_INFO,  "Command  =%08x\n", CommandResp->Header.Fields.Command));
  DEBUG ((DEBUG_INFO,  "IsRespone=%08x\n", CommandResp->Header.Fields.IsResponse));
  DEBUG ((DEBUG_INFO,  "Result   =%08x\n", CommandResp->Header.Fields.Result));

  if (CommandResp->Header.Fields.Result != 0) {
    Status = CommandResp->Header.Fields.Result;
    DEBUG ((DEBUG_ERROR, "Heci send user command fail!\n"));
  }

  return Status;
}
