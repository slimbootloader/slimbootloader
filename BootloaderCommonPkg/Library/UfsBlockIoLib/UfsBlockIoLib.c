/** @file

  Copyright (c) 2014 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/UfsBlockIoLib.h>
#include <IndustryStandard/Scsi.h>
#include "UfsInternal.h"

#define MSG_UFS_DP                0x19

//
// Template for UFS HC Peim Private Data.
//
UFS_PEIM_HC_PRIVATE_DATA   gUfsHcTemplate = {
  UFS_PEIM_HC_SIG,                // Signature
  NULL,                           // Controller
  NULL,                           // Pool
  {
    // Media
    {
      MSG_UFS_DP,
      FALSE,
      TRUE,
      FALSE,
      0x1000,
      0
    },
    {
      MSG_UFS_DP,
      FALSE,
      TRUE,
      FALSE,
      0x1000,
      0
    },
    {
      MSG_UFS_DP,
      FALSE,
      TRUE,
      FALSE,
      0x1000,
      0
    },
    {
      MSG_UFS_DP,
      FALSE,
      TRUE,
      FALSE,
      0x1000,
      0
    },
    {
      MSG_UFS_DP,
      FALSE,
      TRUE,
      FALSE,
      0x1000,
      0
    },
    {
      MSG_UFS_DP,
      FALSE,
      TRUE,
      FALSE,
      0x1000,
      0
    },
    {
      MSG_UFS_DP,
      FALSE,
      TRUE,
      FALSE,
      0x1000,
      0
    },
    {
      MSG_UFS_DP,
      FALSE,
      TRUE,
      FALSE,
      0x1000,
      0
    }
  },
  0,                              // UfsHcBase
  0,                              // Capabilities
  0,                              // Version
  0,                              // TaskTag
  0,                              // UtpTrlBase
  0,                              // Nutrs
  NULL,                           // TrlMapping
  0,                              // UtpTmrlBase
  0,                              // Nutmrs
  NULL,                           // TmrlMapping
  {                               // Luns
    {
      UFS_LUN_0,                      // Ufs Common Lun 0
      UFS_LUN_1,                      // Ufs Common Lun 1
      UFS_LUN_2,                      // Ufs Common Lun 2
      UFS_LUN_3,                      // Ufs Common Lun 3
      UFS_LUN_4,                      // Ufs Common Lun 4
      UFS_LUN_5,                      // Ufs Common Lun 5
      UFS_LUN_6,                      // Ufs Common Lun 6
      UFS_LUN_7,                      // Ufs Common Lun 7
      UFS_WLUN_REPORT_LUNS,           // Ufs Reports Luns Well Known Lun
      UFS_WLUN_UFS_DEV,               // Ufs Device Well Known Lun
      UFS_WLUN_BOOT,                  // Ufs Boot Well Known Lun
      UFS_WLUN_RPMB                   // RPMB Well Known Lun
    },
    0x0000,                           // By default exposing all Luns.
    0x0
  }
};

UFS_PEIM_HC_PRIVATE_DATA         *gPrivate = NULL;


/**
Returns UFS PEIM Private Data.
**/
UFS_PEIM_HC_PRIVATE_DATA *
UfsGetPrivateData (
  VOID
  )
{
  return gPrivate;
}


/**
  Execute Request Sense SCSI command on a specific UFS device.
  @param[in]  Private              A pointer to the UFS_PEIM_HC_PRIVATE_DATA instance.
  @param[in]  Lun                  The lun on which the SCSI cmd executed.
  @param[out] DataBuffer           A pointer to output sense data.
  @param[out] DataBufferLength     The length of output sense data.

  @retval EFI_SUCCESS              The command executed successfully.
  @retval EFI_DEVICE_ERROR         A device error occurred while attempting to send SCSI Request Packet.
  @retval EFI_TIMEOUT              A timeout occurred while waiting for the SCSI Request Packet to execute.

**/
EFI_STATUS
UfsRequestSense (
  IN     UFS_PEIM_HC_PRIVATE_DATA        *Private,
  IN     UINTN                           Lun,
  OUT    VOID                            *DataBuffer,
  OUT    UINT32                          *DataBufferLength
  )
{
  UFS_SCSI_REQUEST_PACKET                Packet;
  UINT8                                  Cdb[UFS_SCSI_OP_LENGTH_SIX];
  EFI_STATUS                             Status;

  ZeroMem (&Packet, sizeof (UFS_SCSI_REQUEST_PACKET));
  ZeroMem (Cdb, sizeof (Cdb));

  Cdb[0]  = EFI_SCSI_OP_REQUEST_SENSE;

  Packet.Timeout          = UFS_TIMEOUT;
  Packet.Cdb              = Cdb;
  Packet.CdbLength        = sizeof (Cdb);
  Packet.DataDirection    = UfsDataIn;
  Packet.InDataBuffer     = DataBuffer;
  Packet.InTransferLength = *DataBufferLength;
  Packet.SenseData        = NULL;
  Packet.SenseDataLength  = 0;

  Status = UfsExecScsiCmds (Private, (UINT8)Lun, &Packet);

  if (!EFI_ERROR (Status)) {
    *DataBufferLength = Packet.InTransferLength;
  }

  return Status;
}

/**
  Execute TEST UNITY READY SCSI command on a specific UFS device.

  @param[in]  Private              A pointer to UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  Lun                  The lun on which the SCSI cmd executed.
  @param[out] SenseData            A pointer to output sense data.
  @param[out] SenseDataLength      The length of output sense data.

  @retval EFI_SUCCESS              The command executed successfully.
  @retval EFI_DEVICE_ERROR         A device error occurred while attempting to send SCSI Request Packet.
  @retval EFI_TIMEOUT              A timeout occurred while waiting for the SCSI Request Packet to execute.

**/
EFI_STATUS
UfsTestUnitReady (
  IN     UFS_PEIM_HC_PRIVATE_DATA        *Private,
  IN     UINTN                           Lun,
  OUT    VOID                            *SenseData,  OPTIONAL
  OUT    UINT8                           *SenseDataLength
  )
{
  UFS_SCSI_REQUEST_PACKET                Packet;
  UINT8                                  Cdb[UFS_SCSI_OP_LENGTH_SIX];
  EFI_STATUS                             Status;

  ZeroMem (&Packet, sizeof (UFS_SCSI_REQUEST_PACKET));
  ZeroMem (Cdb, sizeof (Cdb));

  Cdb[0]  = EFI_SCSI_OP_TEST_UNIT_READY;

  Packet.Timeout         = UFS_TIMEOUT;
  Packet.Cdb             = Cdb;
  Packet.CdbLength       = sizeof (Cdb);
  Packet.DataDirection   = UfsNoData;
  Packet.SenseData       = SenseData;
  Packet.SenseDataLength = *SenseDataLength;

  Status = UfsExecScsiCmds (Private, (UINT8)Lun, &Packet);

  if (*SenseDataLength != 0) {
    *SenseDataLength = Packet.SenseDataLength;
  }

  return Status;
}

/**
  Execute INQUIRY SCSI command on a specific UFS device.

  @param[in]  Private              A pointer to UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  Lun                  The lun on which the SCSI cmd executed.
  @param[out] Inquiry              A pointer to Inquiry data buffer.
  @param[out] InquiryLengths       The length of output Inquiry data.
  @param[out] SenseData            A pointer to output sense data.
  @param[out] SenseDataLength      The length of output sense data.

  @retval EFI_SUCCESS              The command executed successfully.
  @retval EFI_DEVICE_ERROR         A device error occurred while attempting to send SCSI Request Packet.
  @retval EFI_TIMEOUT              A timeout occurred while waiting for the SCSI Request Packet to execute.

**/
EFI_STATUS
UfsInquiry (
  IN     UFS_PEIM_HC_PRIVATE_DATA     *Private,
  IN     UINTN                        Lun,
  OUT VOID                         *Inquiry,
  OUT UINT32                       *InquiryLength,
  OUT VOID                         *SenseData,  OPTIONAL
  OUT UINT8                        *SenseDataLength
  )
{
  UFS_SCSI_REQUEST_PACKET             Packet;
  UINT8                               Cdb[UFS_SCSI_OP_LENGTH_SIX];
  EFI_STATUS                          Status;

  ZeroMem (&Packet, sizeof (UFS_SCSI_REQUEST_PACKET));
  ZeroMem (Cdb, sizeof (Cdb));

  Cdb[0]  = EFI_SCSI_OP_INQUIRY;
  Cdb[4]  = sizeof (EFI_SCSI_INQUIRY_DATA);

  Packet.Timeout          = UFS_TIMEOUT;
  Packet.Cdb              = Cdb;
  Packet.CdbLength        = sizeof (Cdb);
  Packet.InDataBuffer     = Inquiry;
  Packet.InTransferLength = *InquiryLength;
  Packet.DataDirection    = UfsDataIn;
  Packet.SenseData        = SenseData;
  Packet.SenseDataLength  = *SenseDataLength;

  Status = UfsExecScsiCmds (Private, (UINT8)Lun, &Packet);

  if (*SenseDataLength != 0) {
    *SenseDataLength = Packet.SenseDataLength;
  }

  if (!EFI_ERROR (Status)) {
    *InquiryLength = Packet.InTransferLength;
  }

  return Status;
}

/**
  Execute READ CAPACITY(10) SCSI command on a specific UFS device.

  @param[in]  Private              A pointer to UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  Lun                  The lun on which the SCSI cmd executed.
  @param[out] DataBuffer           A pointer to READ_CAPACITY data buffer.
  @param[out] DataLength           The length of output READ_CAPACITY data.
  @param[out] SenseData            A pointer to output sense data.
  @param[out] SenseDataLength      The length of output sense data.

  @retval EFI_SUCCESS              The command executed successfully.
  @retval EFI_DEVICE_ERROR         A device error occurred while attempting to send SCSI Request Packet.
  @retval EFI_TIMEOUT              A timeout occurred while waiting for the SCSI Request Packet to execute.

**/
EFI_STATUS
UfsReadCapacity (
  IN     UFS_PEIM_HC_PRIVATE_DATA     *Private,
  IN     UINTN                        Lun,
  OUT VOID                         *DataBuffer,
  OUT UINT32                       *DataLength,
  OUT VOID                         *SenseData,  OPTIONAL
  OUT UINT8                        *SenseDataLength
  )
{
  UFS_SCSI_REQUEST_PACKET             Packet;
  UINT8                               Cdb[UFS_SCSI_OP_LENGTH_TEN];
  EFI_STATUS                          Status;

  ZeroMem (&Packet, sizeof (UFS_SCSI_REQUEST_PACKET));
  ZeroMem (Cdb, sizeof (Cdb));

  Cdb[0] = EFI_SCSI_OP_READ_CAPACITY;

  Packet.Timeout          = UFS_TIMEOUT;
  Packet.Cdb              = Cdb;
  Packet.CdbLength        = sizeof (Cdb);
  Packet.InDataBuffer     = DataBuffer;
  Packet.InTransferLength = *DataLength;
  Packet.DataDirection    = UfsDataIn;
  Packet.SenseData        = SenseData;
  Packet.SenseDataLength  = *SenseDataLength;

  Status = UfsExecScsiCmds (Private, (UINT8)Lun, &Packet);

  if (*SenseDataLength != 0) {
    *SenseDataLength = Packet.SenseDataLength;
  }

  if (!EFI_ERROR (Status)) {
    *DataLength = Packet.InTransferLength;
  }

  return Status;
}

/**
  Execute READ CAPACITY(16) SCSI command on a specific UFS device.

  @param[in]  Private              A pointer to UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  Lun                  The lun on which the SCSI cmd executed.
  @param[out] DataBuffer           A pointer to READ_CAPACITY data buffer.
  @param[out] DataLength           The length of output READ_CAPACITY data.
  @param[out] SenseData            A pointer to output sense data.
  @param[out] SenseDataLength      The length of output sense data.

  @retval EFI_SUCCESS              The command executed successfully.
  @retval EFI_DEVICE_ERROR         A device error occurred while attempting to send SCSI Request Packet.
  @retval EFI_TIMEOUT              A timeout occurred while waiting for the SCSI Request Packet to execute.

**/
EFI_STATUS
UfsReadCapacity16 (
  IN     UFS_PEIM_HC_PRIVATE_DATA     *Private,
  IN     UINTN                        Lun,
  OUT VOID                         *DataBuffer,
  OUT UINT32                       *DataLength,
  OUT VOID                         *SenseData,  OPTIONAL
  OUT UINT8                        *SenseDataLength
  )
{
  UFS_SCSI_REQUEST_PACKET             Packet;
  UINT8                               Cdb[UFS_SCSI_OP_LENGTH_SIXTEEN];
  EFI_STATUS                          Status;

  ZeroMem (&Packet, sizeof (UFS_SCSI_REQUEST_PACKET));
  ZeroMem (Cdb, sizeof (Cdb));

  Cdb[0]  = EFI_SCSI_OP_READ_CAPACITY16;
  Cdb[1]  = 0x10;          // Service Action should be 0x10 for UFS device.
  Cdb[13] = 0x20;          // The maximum number of bytes for returned data.

  Packet.Timeout          = UFS_TIMEOUT;
  Packet.Cdb              = Cdb;
  Packet.CdbLength        = sizeof (Cdb);
  Packet.InDataBuffer     = DataBuffer;
  Packet.InTransferLength = *DataLength;
  Packet.DataDirection    = UfsDataIn;
  Packet.SenseData        = SenseData;
  Packet.SenseDataLength  = *SenseDataLength;

  Status = UfsExecScsiCmds (Private, (UINT8)Lun, &Packet);

  if (*SenseDataLength != 0) {
    *SenseDataLength = Packet.SenseDataLength;
  }

  if (!EFI_ERROR (Status)) {
    *DataLength = Packet.InTransferLength;
  }

  return Status;
}

/**
  Execute READ (10) SCSI command on a specific UFS device.

  @param[in]  Private              A pointer to UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  Lun                  The lun on which the SCSI cmd executed.
  @param[in]  StartLba             The start LBA.
  @param[in]  SectorNum            The sector number to be read.
  @param[out] DataBuffer           A pointer to data buffer.
  @param[out] DataLength           The length of output data.
  @param[out] SenseData            A pointer to output sense data.
  @param[out] SenseDataLength      The length of output sense data.

  @retval EFI_SUCCESS              The command executed successfully.
  @retval EFI_DEVICE_ERROR         A device error occurred while attempting to send SCSI Request Packet.
  @retval EFI_TIMEOUT              A timeout occurred while waiting for the SCSI Request Packet to execute.

**/
EFI_STATUS
UfsRead10 (
  IN     UFS_PEIM_HC_PRIVATE_DATA     *Private,
  IN     UINTN                        Lun,
  IN     UINTN                        StartLba,
  IN     UINT32                       SectorNum,
  OUT VOID                         *DataBuffer,
  OUT UINT32                       *DataLength,
  OUT VOID                         *SenseData,  OPTIONAL
  OUT UINT8                        *SenseDataLength
  )
{
  UFS_SCSI_REQUEST_PACKET             Packet;
  UINT8                               Cdb[UFS_SCSI_OP_LENGTH_TEN];
  EFI_STATUS                          Status;

  ZeroMem (&Packet, sizeof (UFS_SCSI_REQUEST_PACKET));
  ZeroMem (Cdb, sizeof (Cdb));

  Cdb[0] = EFI_SCSI_OP_READ10;
  WriteUnaligned32 ((UINT32 *)&Cdb[2], SwapBytes32 ((UINT32) StartLba));
  WriteUnaligned16 ((UINT16 *)&Cdb[7], SwapBytes16 ((UINT16) SectorNum));

  Packet.Timeout          = UFS_TIMEOUT;
  Packet.Cdb              = Cdb;
  Packet.CdbLength        = sizeof (Cdb);
  Packet.InDataBuffer     = DataBuffer;
  Packet.InTransferLength = *DataLength;
  Packet.DataDirection    = UfsDataIn;
  Packet.SenseData        = SenseData;
  Packet.SenseDataLength  = *SenseDataLength;

  Status = UfsExecScsiCmds (Private, (UINT8)Lun, &Packet);

  if (*SenseDataLength != 0) {
    *SenseDataLength = Packet.SenseDataLength;
  }

  if (!EFI_ERROR (Status)) {
    *DataLength = Packet.InTransferLength;
  }

  return Status;
}

/**
  Execute WRITE (10) SCSI command on a specific UFS device.

  @param[in]  Private              A pointer to UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  Lun                  The lun on which the SCSI cmd executed.
  @param[in]  StartLba             The start LBA.
  @param[in]  SectorNum            The sector number to be written.
  @param[out] DataBuffer           A pointer to data buffer.
  @param[out] DataLength           The length of output data.
  @param[out] SenseData            A pointer to output sense data.
  @param[out] SenseDataLength      The length of output sense data.

  @retval EFI_SUCCESS              The command executed successfully.
  @retval EFI_DEVICE_ERROR         A device error occurred while attempting to send SCSI Request Packet.
  @retval EFI_TIMEOUT              A timeout occurred while waiting for the SCSI Request Packet to execute.

**/
EFI_STATUS
UfsWrite10 (
  IN  UFS_PEIM_HC_PRIVATE_DATA     *Private,
  IN  UINTN                        Lun,
  IN  UINTN                        StartLba,
  IN  UINT32                       SectorNum,
  OUT VOID                         *DataBuffer,
  OUT UINT32                       *DataLength,
  OUT VOID                         *SenseData, OPTIONAL
  OUT UINT8                        *SenseDataLength
  )
{
  UFS_SCSI_REQUEST_PACKET             Packet;
  UINT8                               Cdb[UFS_SCSI_OP_LENGTH_TEN];
  EFI_STATUS                          Status;

  ZeroMem (&Packet, sizeof (UFS_SCSI_REQUEST_PACKET));
  ZeroMem (Cdb, sizeof (Cdb));

  Cdb[0] = EFI_SCSI_OP_WRITE10;
  WriteUnaligned32 ((UINT32 *)&Cdb[2], SwapBytes32 ((UINT32)StartLba));
  WriteUnaligned16 ((UINT16 *)&Cdb[7], SwapBytes16 ((UINT16)SectorNum));

  Packet.Timeout = UFS_TIMEOUT;
  Packet.Cdb = Cdb;
  Packet.CdbLength = sizeof (Cdb);
  Packet.OutDataBuffer = DataBuffer;
  Packet.OutTransferLength = *DataLength;
  Packet.DataDirection = UfsDataOut;
  Packet.SenseData = SenseData;
  Packet.SenseDataLength = *SenseDataLength;

  Status = UfsExecScsiCmds (Private, (UINT8)Lun, &Packet);

  if (*SenseDataLength != 0) {
    *SenseDataLength = Packet.SenseDataLength;
  }

  if (!EFI_ERROR (Status)) {
    *DataLength = Packet.OutTransferLength;
  }

  return Status;
}

/**
  Execute READ (16) SCSI command on a specific UFS device.

  @param[in]  Private              A pointer to UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  Lun                  The lun on which the SCSI cmd executed.
  @param[in]  StartLba             The start LBA.
  @param[in]  SectorNum            The sector number to be read.
  @param[out] DataBuffer           A pointer to data buffer.
  @param[out] DataLength           The length of output data.
  @param[out] SenseData            A pointer to output sense data.
  @param[out] SenseDataLength      The length of output sense data.

  @retval EFI_SUCCESS              The command executed successfully.
  @retval EFI_DEVICE_ERROR         A device error occurred while attempting to send SCSI Request Packet.
  @retval EFI_TIMEOUT              A timeout occurred while waiting for the SCSI Request Packet to execute.

**/
EFI_STATUS
UfsRead16 (
  IN     UFS_PEIM_HC_PRIVATE_DATA     *Private,
  IN     UINTN                        Lun,
  IN     UINTN                        StartLba,
  IN     UINT32                       SectorNum,
  OUT VOID                         *DataBuffer,
  OUT UINT32                       *DataLength,
  OUT VOID                         *SenseData,  OPTIONAL
  OUT UINT8                        *SenseDataLength
  )
{
  UFS_SCSI_REQUEST_PACKET             Packet;
  UINT8                               Cdb[UFS_SCSI_OP_LENGTH_SIXTEEN];
  EFI_STATUS                          Status;

  ZeroMem (&Packet, sizeof (UFS_SCSI_REQUEST_PACKET));
  ZeroMem (Cdb, sizeof (Cdb));

  Cdb[0] = EFI_SCSI_OP_READ16;
  WriteUnaligned64 ((UINT64 *)&Cdb[2], SwapBytes64 (StartLba));
  WriteUnaligned32 ((UINT32 *)&Cdb[10], SwapBytes32 (SectorNum));

  Packet.Timeout          = UFS_TIMEOUT;
  Packet.Cdb              = Cdb;
  Packet.CdbLength        = sizeof (Cdb);
  Packet.InDataBuffer     = DataBuffer;
  Packet.InTransferLength = *DataLength;
  Packet.DataDirection    = UfsDataIn;
  Packet.SenseData        = SenseData;
  Packet.SenseDataLength  = *SenseDataLength;

  Status = UfsExecScsiCmds (Private, (UINT8)Lun, &Packet);

  if (*SenseDataLength != 0) {
    *SenseDataLength = Packet.SenseDataLength;
  }

  if (!EFI_ERROR (Status)) {
    *DataLength = Packet.InTransferLength;
  }

  return Status;
}

/**
  Execute WRITE (16) SCSI command on a specific UFS device.

  @param[in]  Private              A pointer to UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  Lun                  The lun on which the SCSI cmd executed.
  @param[in]  StartLba             The start LBA.
  @param[in]  SectorNum            The sector number to be written.
  @param[out] DataBuffer           A pointer to data buffer.
  @param[out] DataLength           The length of output data.
  @param[out] SenseData            A pointer to output sense data.
  @param[out] SenseDataLength      The length of output sense data.

  @retval EFI_SUCCESS              The command executed successfully.
  @retval EFI_DEVICE_ERROR         A device error occurred while attempting to send SCSI Request Packet.
  @retval EFI_TIMEOUT              A timeout occurred while waiting for the SCSI Request Packet to execute.

**/
EFI_STATUS
UfsWrite16 (
  IN  UFS_PEIM_HC_PRIVATE_DATA     *Private,
  IN  UINTN                        Lun,
  IN  UINTN                        StartLba,
  IN  UINT32                       SectorNum,
  OUT VOID                         *DataBuffer,
  OUT UINT32                       *DataLength,
  OUT VOID                         *SenseData, OPTIONAL
  OUT UINT8                        *SenseDataLength
  )
{
  UFS_SCSI_REQUEST_PACKET             Packet;
  UINT8                               Cdb[UFS_SCSI_OP_LENGTH_SIXTEEN];
  EFI_STATUS                          Status;

  ZeroMem (&Packet, sizeof (UFS_SCSI_REQUEST_PACKET));
  ZeroMem (Cdb, sizeof (Cdb));

  Cdb[0] = EFI_SCSI_OP_WRITE16;
  WriteUnaligned64 ((UINT64 *)&Cdb[2], SwapBytes64 (StartLba));
  WriteUnaligned32 ((UINT32 *)&Cdb[10], SwapBytes32 (SectorNum));

  Packet.Timeout = UFS_TIMEOUT;
  Packet.Cdb = Cdb;
  Packet.CdbLength = sizeof (Cdb);
  Packet.OutDataBuffer = DataBuffer;
  Packet.OutTransferLength = *DataLength;
  Packet.DataDirection = UfsDataOut;
  Packet.SenseData = SenseData;
  Packet.SenseDataLength = *SenseDataLength;

  Status = UfsExecScsiCmds (Private, (UINT8)Lun, &Packet);

  if (*SenseDataLength != 0) {
    *SenseDataLength = Packet.SenseDataLength;
  }

  if (!EFI_ERROR (Status)) {
    *DataLength = Packet.OutTransferLength;
  }

  return Status;
}

/**
  Parsing Sense Keys from sense data.

  @param  Media              The pointer of EFI_PEI_BLOCK_IO2_MEDIA
  @param  SenseData          The pointer of EFI_SCSI_SENSE_DATA
  @param  NeedRetry          The pointer of action which indicates what is need to retry

  @retval EFI_DEVICE_ERROR   Indicates that error occurs
  @retval EFI_SUCCESS        Successfully to complete the parsing

**/
EFI_STATUS
UfsParsingSenseKeys (
  IN     EFI_PEI_BLOCK_IO2_MEDIA   *Media,
  IN     EFI_SCSI_SENSE_DATA       *SenseData,
  OUT BOOLEAN                   *NeedRetry
  )
{
  if ((SenseData->Sense_Key == EFI_SCSI_SK_NOT_READY) &&
      (SenseData->Addnl_Sense_Code == EFI_SCSI_ASC_NO_MEDIA)) {
    Media->MediaPresent = FALSE;
    *NeedRetry = FALSE;
    DEBUG ((DEBUG_VERBOSE, "UfsBlockIoPei: Is No Media\n"));
    return EFI_DEVICE_ERROR;
  }

  if ((SenseData->Sense_Key == EFI_SCSI_SK_UNIT_ATTENTION) &&
      (SenseData->Addnl_Sense_Code == EFI_SCSI_ASC_MEDIA_CHANGE)) {
    *NeedRetry = TRUE;
    DEBUG ((DEBUG_VERBOSE, "UfsBlockIoPei: Is Media Change\n"));
    return EFI_SUCCESS;
  }

  if ((SenseData->Sense_Key == EFI_SCSI_SK_UNIT_ATTENTION) &&
      (SenseData->Addnl_Sense_Code == EFI_SCSI_ASC_RESET)) {
    *NeedRetry = TRUE;
    DEBUG ((DEBUG_VERBOSE, "UfsBlockIoPei: Was Reset Before\n"));
    return EFI_SUCCESS;
  }

  if ((SenseData->Sense_Key == EFI_SCSI_SK_MEDIUM_ERROR) ||
      ((SenseData->Sense_Key == EFI_SCSI_SK_NOT_READY) &&
       (SenseData->Addnl_Sense_Code == EFI_SCSI_ASC_MEDIA_UPSIDE_DOWN))) {
    *NeedRetry = FALSE;
    DEBUG ((DEBUG_VERBOSE, "UfsBlockIoPei: Media Error\n"));
    return EFI_DEVICE_ERROR;
  }

  if (SenseData->Sense_Key == EFI_SCSI_SK_HARDWARE_ERROR) {
    *NeedRetry = FALSE;
    DEBUG ((DEBUG_VERBOSE, "UfsBlockIoPei: Hardware Error\n"));
    return EFI_DEVICE_ERROR;
  }

  if ((SenseData->Sense_Key == EFI_SCSI_SK_NOT_READY) &&
      (SenseData->Addnl_Sense_Code == EFI_SCSI_ASC_NOT_READY) &&
      (SenseData->Addnl_Sense_Code_Qualifier == EFI_SCSI_ASCQ_IN_PROGRESS)) {
    *NeedRetry = TRUE;
    DEBUG ((DEBUG_VERBOSE, "UfsBlockIoPei: Was Reset Before\n"));
    return EFI_SUCCESS;
  }

  *NeedRetry = FALSE;
  DEBUG ((DEBUG_VERBOSE, "UfsBlockIoPei: Sense Key = 0x%x ASC = 0x%x!\n", SenseData->Sense_Key,
          SenseData->Addnl_Sense_Code));
  return EFI_DEVICE_ERROR;
}


/**
  Gets the count of block I/O devices that one specific block driver detects.

  This function is used for getting the count of block I/O devices that one
  specific block driver detects.  To the PEI ATAPI driver, it returns the number
  of all the detected ATAPI devices it detects during the enumeration process.
  To the PEI legacy floppy driver, it returns the number of all the legacy
  devices it finds during its enumeration process. If no device is detected,
  then the function will return zero.

  @param[out] NumberBlockDevices   The number of block I/O devices discovered.

  @retval     EFI_SUCCESS          The operation performed successfully.

**/
EFI_STATUS
EFIAPI
UfsGetDeviceNo (
  OUT UINTN                          *NumberBlockDevices
  )
{
  //
  // For Ufs device, it has up to 8 normal Luns plus some well-known Luns.
  // At PEI phase, we will only expose normal Luns to user.
  // For those disabled Lun, when user try to access it, the operation would fail.
  //
  *NumberBlockDevices = UFS_PEIM_MAX_LUNS;
  return EFI_SUCCESS;
}

/**
  Gets a block device's media information.

  This function will provide the caller with the specified block device's media
  information. If the media changes, calling this function will update the media
  information accordingly.

  @param[in]  Private       A pointer to UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk. Because the driver that implements Block I/O
                            PPIs will manage multiple block devices, the PPIs that
                            want to talk to a single device must specify the
                            device index that was assigned during the enumeration
                            process. This index is a number from one to
                            NumberBlockDevices.
  @param[out] MediaInfo     The media information of the specified block media.
                            The caller is responsible for the ownership of this
                            data structure.

  @par Note:
      The MediaInfo structure describes an enumeration of possible block device
      types.  This enumeration exists because no device paths are actually passed
      across interfaces that describe the type or class of hardware that is publishing
      the block I/O interface. This enumeration will allow for policy decisions
      in the Recovery PEIM, such as "Try to recover from legacy floppy first,
      LS-120 second, CD-ROM third." If there are multiple partitions abstracted
      by a given device type, they should be reported in ascending order; this
      order also applies to nested partitions, such as legacy MBR, where the
      outermost partitions would have precedence in the reporting order. The
      same logic applies to systems such as IDE that have precedence relationships
      like "Master/Slave" or "Primary/Secondary". The master device should be
      reported first, the slave second.

  @retval EFI_SUCCESS        Media information about the specified block device
                             was obtained successfully.
  @retval EFI_DEVICE_ERROR   Cannot get the media information due to a hardware
                             error.
  @retval EFI_OUT_OF_RESOURCES  Insufficant memory resource pool.
**/
EFI_STATUS
UfsGetMediaInfoInternal (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private,
  IN  UINTN                          DeviceIndex,
  OUT EFI_PEI_BLOCK_IO2_MEDIA        **MediaInfo
  )
{
  EFI_STATUS                         Status;
  EFI_SCSI_SENSE_DATA                SenseData;
  UINT8                              SenseDataLength;
  EFI_SCSI_DISK_CAPACITY_DATA        *Capacity;
  EFI_SCSI_DISK_CAPACITY_DATA16      *Capacity16;
  UINT32                             DataLength;
  BOOLEAN                            NeedRetry;

  NeedRetry = TRUE;

  if (DeviceIndex >= UFS_PEIM_MAX_LUNS) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Private->Luns.BitMask & (BIT0 << DeviceIndex)) == 0) {
    return EFI_ACCESS_DENIED;
  }

  //
  // The structure for Capacity and Capacity16 is byte-align in defination.
  // To make sure their address are 4-byte aligned (required by UfsExecScsiCmds)
  // Here allocate memory for them instead of directly using structure variable.
  //
  Capacity   = AllocateZeroPool (sizeof (EFI_SCSI_DISK_CAPACITY_DATA));
  Capacity16 = AllocateZeroPool (sizeof (EFI_SCSI_DISK_CAPACITY_DATA16));
  if ((Capacity == NULL) || (Capacity16 == NULL)) {
    if (Capacity != NULL) {
      FreePool (Capacity);
    }
    if (Capacity16 != NULL) {
      FreePool (Capacity16);
    }
    return EFI_OUT_OF_RESOURCES;
  }

  SenseDataLength = sizeof (SenseData);
  ZeroMem (&SenseData, SenseDataLength);

  //
  // First test unit ready
  //
  do {
    Status = UfsTestUnitReady (
               Private,
               DeviceIndex,
               &SenseData,
               &SenseDataLength
               );
    if (!EFI_ERROR (Status)) {
      break;
    }

    if (SenseDataLength == 0) {
      continue;
    }

    Status = UfsParsingSenseKeys (& (Private->Media[DeviceIndex]), &SenseData, &NeedRetry);
    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Done;
    }

  } while (NeedRetry);

  DataLength      = sizeof (EFI_SCSI_DISK_CAPACITY_DATA);
  SenseDataLength = 0;
  Status = UfsReadCapacity (Private, DeviceIndex, Capacity, &DataLength, NULL, &SenseDataLength);
  if (EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;
    goto Done;
  }

  if ((Capacity->LastLba3 == 0xff) && (Capacity->LastLba2 == 0xff) &&
      (Capacity->LastLba1 == 0xff) && (Capacity->LastLba0 == 0xff)) {
    DataLength      = sizeof (EFI_SCSI_DISK_CAPACITY_DATA16);
    SenseDataLength = 0;
    Status = UfsReadCapacity16 (Private, DeviceIndex, Capacity16, &DataLength, NULL, &SenseDataLength);
    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Done;
    }
    Private->Media[DeviceIndex].LastBlock  = ((UINT32)Capacity16->LastLba3 << 24) | (Capacity16->LastLba2 << 16) |
        (Capacity16->LastLba1 << 8) | Capacity16->LastLba0;
    Private->Media[DeviceIndex].LastBlock |= LShiftU64 ((UINT64)Capacity16->LastLba7,
        56) | LShiftU64 ((UINT64)Capacity16->LastLba6, 48) | LShiftU64 ((UINT64)Capacity16->LastLba5,
            40) | LShiftU64 ((UINT64)Capacity16->LastLba4, 32);
    Private->Media[DeviceIndex].BlockSize  = (Capacity16->BlockSize3 << 24) | (Capacity16->BlockSize2 << 16) |
        (Capacity16->BlockSize1 << 8) | Capacity16->BlockSize0;
  } else {
    Private->Media[DeviceIndex].LastBlock  = ((UINT32)Capacity->LastLba3 << 24) | (Capacity->LastLba2 << 16) |
        (Capacity->LastLba1 << 8) | Capacity->LastLba0;
    Private->Media[DeviceIndex].BlockSize  = (Capacity->BlockSize3 << 24) | (Capacity->BlockSize2 << 16) |
        (Capacity->BlockSize1 << 8) | Capacity->BlockSize0;
  }

  *MediaInfo = &Private->Media[DeviceIndex];

Done:
  FreePool (Capacity);
  FreePool (Capacity16);

  return Status;
}

/**
  Reads the requested number of blocks from the specified block device.

  The function reads the requested number of blocks from the device. All the
  blocks are read, or an error is returned. If there is no media in the device,
  the function returns EFI_NO_MEDIA.

  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk. Because the driver that implements Block I/O
                            PPIs will manage multiple block devices, PPIs that
                            want to talk to a single device must specify the device
                            index that was assigned during the enumeration process.
                            This index is a number from one to NumberBlockDevices.
  @param[in]  StartLBA      The starting logical block address (LBA) to read from
                            on the device
  @param[in]  BufferSize    The size of the Buffer in bytes. This number must be
                            a multiple of the intrinsic block size of the device.
  @param[out] Buffer        A pointer to the destination buffer for the data.
                            The caller is responsible for the ownership of the
                            buffer.

  @retval EFI_SUCCESS             The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR        The device reported an error while attempting
                                  to perform the read operation.
  @retval EFI_INVALID_PARAMETER   The read request contains LBAs that are not
                                  valid, or the buffer is not properly aligned.
  @retval EFI_NO_MEDIA            There is no media in the device.
  @retval EFI_BAD_BUFFER_SIZE     The BufferSize parameter is not a multiple of
                                  the intrinsic block size of the device.

**/
EFI_STATUS
UfsReadBlocksInternal (
  IN  UINTN                          DeviceIndex,
  IN  EFI_LBA                        StartLBA,
  IN  UINTN                          BufferSize,
  OUT VOID                           *Buffer
  )
{
  EFI_STATUS                         Status;
  UINTN                              BlockSize;
  UINTN                              NumberOfBlocks;
  UFS_PEIM_HC_PRIVATE_DATA           *Private;
  EFI_SCSI_SENSE_DATA                SenseData;
  UINT8                              SenseDataLength;
  BOOLEAN                            NeedRetry;
  UINT32                             BufferSize32;

  Private = UfsGetPrivateData();
  if (Private == NULL) {
    return EFI_NOT_FOUND;
  }

  Status    = EFI_SUCCESS;
  NeedRetry = TRUE;

  ZeroMem (&SenseData, sizeof (SenseData));
  SenseDataLength = sizeof (SenseData);

  //
  // Check parameters
  //
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize == 0) {
    return EFI_SUCCESS;
  }

  if (DeviceIndex >= UFS_PEIM_MAX_LUNS) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Private->Luns.BitMask & (BIT0 << DeviceIndex)) == 0) {
    return EFI_ACCESS_DENIED;
  }

  BlockSize = Private->Media[DeviceIndex].BlockSize;

  if (BufferSize % BlockSize != 0) {
    Status = EFI_BAD_BUFFER_SIZE;
  }

  if (StartLBA > Private->Media[DeviceIndex].LastBlock) {
    Status = EFI_INVALID_PARAMETER;
  }

  NumberOfBlocks = BufferSize / BlockSize;

  do {
    Status = UfsTestUnitReady (
               Private,
               DeviceIndex,
               &SenseData,
               &SenseDataLength
               );
    if (!EFI_ERROR (Status)) {
      break;
    }

    if (SenseDataLength == 0) {
      continue;
    }

    Status = UfsParsingSenseKeys (& (Private->Media[DeviceIndex]), &SenseData, &NeedRetry);
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }

  } while (NeedRetry);

  SenseDataLength = 0;
  BufferSize32 = (UINT32)BufferSize;
  if (Private->Media[DeviceIndex].LastBlock < 0xfffffffful) {
    Status = UfsRead10 (
               Private,
               DeviceIndex,
               (UINT32)StartLBA,
               (UINT32)NumberOfBlocks,
               Buffer,
               &BufferSize32,
               NULL,
               &SenseDataLength
               );
  } else {
    Status = UfsRead16 (
               Private,
               DeviceIndex,
               (UINT32)StartLBA,
               (UINT32)NumberOfBlocks,
               Buffer,
               &BufferSize32,
               NULL,
               &SenseDataLength
               );
  }
  return Status;
}


/**
  Reads the requested number of blocks from the specified block device.

  The function reads the requested number of blocks from the device. All the
  blocks are read, or an error is returned. If there is no media in the device,
  the function returns EFI_NO_MEDIA.

  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk. Because the driver that implements Block I/O
                            PPIs will manage multiple block devices, PPIs that
                            want to talk to a single device must specify the device
                            index that was assigned during the enumeration process.
                            This index is a number from one to NumberBlockDevices.
  @param[in]  StartLBA      The starting logical block address (LBA) to read from
                            on the device
  @param[in]  BufferSize    The size of the Buffer in bytes. This number must be
                            a multiple of the intrinsic block size of the device.
  @param[out] Buffer        A pointer to the destination buffer for the data.
                            The caller is responsible for the ownership of the
                            buffer.

  @retval EFI_SUCCESS             The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR        The device reported an error while attempting
                                  to perform the read operation.
  @retval EFI_INVALID_PARAMETER   The read request contains LBAs that are not
                                  valid, or the buffer is not properly aligned.
  @retval EFI_NO_MEDIA            There is no media in the device.
  @retval EFI_BAD_BUFFER_SIZE     The BufferSize parameter is not a multiple of
                                  the intrinsic block size of the device.

**/
EFI_STATUS
EFIAPI
UfsReadBlocks (
  IN  UINTN                          DeviceIndex,
  IN  EFI_LBA                        StartLba,
  IN  UINTN                          BufferSize,
  OUT VOID                           *Buffer
  )
{
  EFI_STATUS                         Status;
  UINT32                             ReadSize;
  UINT32                             ReadBlockSize;
  EFI_LBA                            LbaAddress;

  Status     = EFI_SUCCESS;
  ReadSize   = 0;
  LbaAddress = StartLba;
  while (ReadSize < BufferSize) {
    if (ReadSize + SIZE_64KB > BufferSize) {
      ReadBlockSize = (UINT32)BufferSize - ReadSize;
    } else {
      ReadBlockSize = SIZE_64KB;
    }

    Status = UfsReadBlocksInternal (DeviceIndex, LbaAddress, ReadBlockSize, (UINT8 *)Buffer + ReadSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "    UfsReadBlocks_internal: Status = %r\n", Status));
      break;
    }
    ReadSize   += ReadBlockSize;
    LbaAddress += ReadBlockSize / SIZE_4KB;
  }

  return Status;
}


/**
  This function writes data from Memory to UFS

  @param[in]  DeviceIndex   Specifies the block device to which the function wants
                            to talk.
  @param[in]  StartLBA      Target UFS block number(LBA) where data will be written
  @param[in]  BufferSize    Total data size to be written in bytes unit
  @param[out] Buffer        Pointer to the buffer in Memory to be copied to UFS

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
EFIAPI
UfsWriteBlocks (
  IN  UINTN                          DeviceIndex,
  IN  EFI_PEI_LBA                    StartLBA,
  IN  UINTN                          BufferSize,
  OUT VOID                           *Buffer
  )
{
  EFI_STATUS                         Status;
  UINTN                              BlockSize;
  UINTN                              NumberOfBlocks;
  UFS_PEIM_HC_PRIVATE_DATA           *Private;
  EFI_SCSI_SENSE_DATA                SenseData;
  UINT8                              SenseDataLength;
  BOOLEAN                            NeedRetry;
  UINT32                             BufferSize32;

  DEBUG ((DEBUG_INFO, "UfsWriteBlocks. DeviceIndex = %x\n", DeviceIndex));

  Private = UfsGetPrivateData();
  if (Private == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = EFI_SUCCESS;
  NeedRetry = TRUE;

  ZeroMem (&SenseData, sizeof (SenseData));
  SenseDataLength = sizeof (SenseData);

  //
  // Check parameters
  //
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize == 0) {
    return EFI_SUCCESS;
  }

  if (DeviceIndex >= UFS_PEIM_MAX_LUNS) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Private->Luns.BitMask & (BIT0 << DeviceIndex)) == 0) {
    return EFI_ACCESS_DENIED;
  }

  BlockSize = Private->Media[DeviceIndex].BlockSize;

  if (BufferSize % BlockSize != 0) {
    Status = EFI_BAD_BUFFER_SIZE;
  }

  if (StartLBA > Private->Media[DeviceIndex].LastBlock) {
    Status = EFI_INVALID_PARAMETER;
  }

  NumberOfBlocks = BufferSize / BlockSize;

  do {
    Status = UfsTestUnitReady (
               Private,
               DeviceIndex,
               &SenseData,
               &SenseDataLength
               );
    if (!EFI_ERROR (Status)) {
      break;
    }

    if (SenseDataLength == 0) {
      continue;
    }

    Status = UfsParsingSenseKeys (& (Private->Media[DeviceIndex]), &SenseData, &NeedRetry);
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }

  } while (NeedRetry);

  SenseDataLength = 0;
  BufferSize32 = (UINT32)BufferSize;
  if (Private->Media[DeviceIndex].LastBlock < 0xfffffffful) {
    Status = UfsWrite10 (
               Private,
               DeviceIndex,
               (UINT32)StartLBA,
               (UINT32)NumberOfBlocks,
               Buffer,
               &BufferSize32,
               NULL,
               &SenseDataLength
               );
  } else {
    Status = UfsWrite16 (
               Private,
               DeviceIndex,
               (UINT32)StartLBA,
               (UINT32)NumberOfBlocks,
               Buffer,
               &BufferSize32,
               NULL,
               &SenseDataLength
               );
  }
  return Status;
}

/**
  Gets a block device's media information.

  This function will provide the caller with the specified block device's media
  information. If the media changes, calling this function will update the media
  information accordingly.

  @param[in]  DeviceIndex    Specifies the block device to which the function wants
                             to talk.
  @param[out] DevBlockInfo   The Block Io information of the specified block partition.

  @retval EFI_SUCCESS        The Block Io information about the specified block device
                             was obtained successfully.
  @retval EFI_DEVICE_ERROR   Cannot get the media information due to a hardware
                             error.

**/
EFI_STATUS
EFIAPI
UfsGetMediaInfo (
  IN  UINTN                          DeviceIndex,
  OUT DEVICE_BLOCK_INFO              *DevBlockInfo
  )
{
  EFI_STATUS                         Status;
  UFS_PEIM_HC_PRIVATE_DATA           *Private;
  EFI_PEI_BLOCK_IO2_MEDIA            *MediaInfo;

  if (DevBlockInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Private = UfsGetPrivateData();
  if (Private == NULL) {
    return EFI_NOT_FOUND;
  }

  Status    = UfsGetMediaInfoInternal (
                Private,
                DeviceIndex,
                &MediaInfo
                );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DevBlockInfo->BlockSize = MediaInfo->BlockSize;
  DevBlockInfo->BlockNum  = MediaInfo->LastBlock + 1;

  return EFI_SUCCESS;
}

/**
  Finishes device initialization by setting fDeviceInit flag and waiting untill device responds by
  clearing it.

  @param[in] Private  Pointer to the UFS_PASS_THRU_PRIVATE_DATA.

  @retval EFI_SUCCESS  The operation succeeds.
  @retval Others       The operation fails.

**/
EFI_STATUS
UfsFinishDeviceInitialization (
  IN UFS_PEIM_HC_PRIVATE_DATA  *Private
  )
{
  EFI_STATUS  Status;
  UINT8  DeviceInitStatus;
  UINT8  Timeout;

  DeviceInitStatus = 0xFF;

  //
  // The host enables the device initialization completion by setting fDeviceInit flag.
  //
  Status = UfsSetFlag (Private, UfsFlagDevInit);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Timeout = 5;
  do {
    Status = UfsReadFlag (Private, UfsFlagDevInit, &DeviceInitStatus);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    MicroSecondDelay (1);
    Timeout--;
  } while (DeviceInitStatus != 0 && Timeout != 0);

  return EFI_SUCCESS;
}

/**
  Switches the link Power Mode and Gear.

  @param[in] Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS                Successfully switched the Power Mode and Gear
  @retval others                     Failed to switch the Power Mode and Gear

**/
EFI_STATUS
EFIAPI
VerifyUfsHcPlatformPostHceSwitchGear (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private
  )
{
  EFI_STATUS                         Status;

  DEBUG ((DEBUG_INFO, "VerifyUfsHcPlatformPostHceSwitchGear Entry\n" ));
  if (Private == NULL) {
    DEBUG((DEBUG_ERROR, "VerifyUfsHcPlatformPostHceSwitchGear Private is NULL\n"));
    return EFI_LOAD_ERROR;
  }

  Status = UfsPowerModeAndGearSwitch(Private);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "UfsPowerModeAndGearSwitch Fails, Status = %r\n", Status));
  }
  DEBUG ((DEBUG_INFO, "VerifyUfsHcPlatformPostHceSwitchGear Exit\n" ));
  return Status;
}

/**
  The function will initialize UFS device.

  Based on UfsHcPciBase, this function will initialize UFS host controller, allocate
  necessary resources, and enumarate all the LUNs.

  @param[in]  UfsHcPciBase       UFS Host Controller's PCI ConfigSpace Base address
  @param[in]  DevInitPhase       For the performance optimization,
                                 Device initialization is separated to several phases.

  @retval EFI_SUCCESS            The driver is successfully initialized.
  @retval Others                 Can't initialize the UFS device.

**/
EFI_STATUS
EFIAPI
InitializeUfs (
  IN  UINTN                     UfsHcPciBase,
  IN  DEVICE_INIT_PHASE         DevInitPhase
  )
{
  EFI_STATUS                    Status;
  UFS_PEIM_HC_PRIVATE_DATA      *Private;
  UINT32                        Index;
  UFS_UNIT_DESC                 UnitDescriptor;
  UFS_DEV_DESC                  DeviceDescriptor;
  UINTN                         MmioBase;
  UINT8                         Controller;
  UFS_HC_PEI_PRIVATE_DATA      *UfsPrivateHcData;
  UINT32                        UnitDescriptorSize;
  UINT32                        DeviceDescriptorSize;
  UFS_CARD_REF_CLK_FREQ_ATTRIBUTE  Attributes,RefClkFreq;
  UINT8                            RefClkAttr;

  RefClkFreq = UfsCardRefClkFreq19p2Mhz;
  if (DevInitPhase == DevDeinit) {
    if (gPrivate != NULL) {
      Private = gPrivate;

      if ((Private->Pool != NULL) && (Private->Pool->Head != NULL)) {
        UfsFreeMemPool (Private->Pool);
      }

      if (Private->UtpTmrlBase != NULL) {
        IoMmuFreeBuffer (
          EFI_SIZE_TO_PAGES (Private->Nutmrs * sizeof (UTP_TMRD)),
          Private->UtpTmrlBase,
          Private->TmrlMapping
          );
      }

      if (Private->UtpTrlBase != NULL) {
        IoMmuFreeBuffer (
          EFI_SIZE_TO_PAGES (Private->Nutrs * sizeof (UTP_TRD)),
          Private->UtpTrlBase,
          Private->TrlMapping
          );
      }

      UfsControllerStop (Private);

      FreePool (Private);
      gPrivate = NULL;
    }
    return EFI_SUCCESS;
  }

  // Enable Bus Master
  MmioOr16 (UfsHcPciBase + PCI_COMMAND_OFFSET,
            (UINT16)(EFI_PCI_COMMAND_IO_SPACE | EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER));

  gPrivate = (UFS_PEIM_HC_PRIVATE_DATA *)AllocatePool (sizeof (UFS_PEIM_HC_PRIVATE_DATA));
  if (gPrivate == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate memory for UFS_PEIM_HC_PRIVATE_DATA! \n"));
    return EFI_OUT_OF_RESOURCES;
  }

  Private = gPrivate;

  ZeroMem (Private, sizeof (UFS_PEIM_HC_PRIVATE_DATA));

  UfsPrivateHcData = (UFS_HC_PEI_PRIVATE_DATA *)AllocatePool (sizeof (UFS_HC_PEI_PRIVATE_DATA));
  if (UfsPrivateHcData == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate memory for UFS_HC_PEI_PRIVATE_DATA! \n"));
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Init Ufs data
  //
  InitializeUfsHcPeim (UfsPrivateHcData, UfsHcPciBase);

  Controller = 0;
  MmioBase   = 0;

  while (TRUE) {
    Status = GetUfsHcMmioBar (UfsPrivateHcData, Controller, &MmioBase);
    //
    // When status is error, meant no controller is found
    //
    if (EFI_ERROR (Status)) {
      break;
    }

    CopyMem (Private, &gUfsHcTemplate, sizeof (UFS_PEIM_HC_PRIVATE_DATA));

    Private->UfsHcBase = MmioBase;

    //
    // Initialize the memory pool which will be used in all transactions.
    //
    Status = UfsInitMemPool (Private);
    if (EFI_ERROR (Status)) {
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }

    Status = GetUfsHcInfo (Private);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to initialize UfsHcInfo\n"));
      Status = EFI_DEVICE_ERROR;
      break;
    }

    //
    // Initialize UFS Host Controller H/W.
    //
    Status = UfsControllerInit (Private);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "UfsDevicePei: Host Controller Initialization Error, Status = %r\n", Status));
      Controller++;
      continue;
    }

    //
    // UFS 2.0 spec Section 13.1.3.3:
    // At the end of the UFS Interconnect Layer initialization on both host and device side,
    // the host shall send a NOP OUT UPIU to verify that the device UTP Layer is ready.
    //
    Status = UfsExecNopCmds (Private);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Ufs Sending NOP IN command Error, Status = %r\n", Status));
      Controller++;
      continue;
    }

    Status = UfsFinishDeviceInitialization (Private);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Device failed to finish initialization, Status = %r\n", Status));
      Controller++;
      continue;
    }

    DEBUG ((DEBUG_INFO, "Setting clock frequency for UFS\n"));
    RefClkAttr = UfsAttrRefClkFreq;
    Attributes = UfsCardRefClkFreqObsolete;
    Status     = UfsRwAttributes (Private, TRUE, RefClkAttr, 0, 0, (UINT32 *)&Attributes);
    if (!EFI_ERROR (Status)) {
      if (Attributes != RefClkFreq) {
        Attributes = RefClkFreq;
        DEBUG (
          (DEBUG_INFO,
            "Setting bRefClkFreq attribute(%x) to %x\n  0 -> 19.2 Mhz\n  1 -> 26 Mhz\n  2 -> 38.4 Mhz\n  3 -> Obsolete\n",
            RefClkAttr,
            Attributes)
          );
        Status = UfsRwAttributes (Private, FALSE, RefClkAttr, 0, 0, (UINT32 *)&Attributes);
        if (EFI_ERROR (Status)) {
          DEBUG (
            (DEBUG_ERROR,
              "Failed to Change Reference Clock Attribute to %d, Status = %r \n",
              RefClkFreq,
              Status)
            );
        }
      }
    } else {
      DEBUG (
        (DEBUG_ERROR,
          "Failed to Read Reference Clock Attribute, Status = %r \n",
          Status)
        );
    }
    if (Attributes == 0){
      DEBUG (
            (DEBUG_INFO,
              "Setting bRefClkFreq attribute(%x) to 19.2 Mhz\n",
              RefClkAttr)
          );
    } else if (Attributes == 1) {
        DEBUG (
            (DEBUG_INFO,
              "Setting bRefClkFreq attribute(%x) to 26 Mhz\n",
              RefClkAttr)
        );
    } else if (Attributes == 2) {
        DEBUG (
            (DEBUG_INFO,
              "Setting bRefClkFreq attribute(%x) to 38.4 Mhz\n",
              RefClkAttr)
        );
    }

    Status = VerifyUfsHcPlatformPostHceSwitchGear(Private);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to switch power link power mode gear, status = %r\n", Status));
      return Status;
    }

    //
    // Check if 8 common luns are active and set corresponding bit mask.
    //
    UnitDescriptorSize = sizeof (UFS_UNIT_DESC);
    for (Index = 0; Index < 8; Index++) {
      Status = UfsRwDeviceDesc (Private, TRUE, UfsUnitDesc, (UINT8) Index, 0, &UnitDescriptor, &UnitDescriptorSize);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Failed to read unit descriptor, index = %X, status = %r\n", Index, Status));
        continue;
      }
      if (UnitDescriptor.LunEn == 0x1) {
        DEBUG ((DEBUG_INFO, "UFS LUN %X is enabled\n", Index));
        Private->Luns.BitMask |= (BIT0 << Index);
      }
    }

    //
    // Check if RPMB WLUN is supported and set corresponding bit mask.
    //
    DeviceDescriptorSize = sizeof (UFS_DEV_DESC);
    Status = UfsRwDeviceDesc (Private, TRUE, UfsDeviceDesc, 0, 0, &DeviceDescriptor, &DeviceDescriptorSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to read device descriptor, status = %r\n", Status));
    } else {
      if (DeviceDescriptor.SecurityLun == 0x1) {
        DEBUG ((DEBUG_INFO, "UFS WLUN RPMB is supported\n"));
        Private->Luns.BitMask |= BIT11;
      }
    }

    Controller++;
  }

  DEBUG ((DEBUG_INFO, "Init UFS done\n"));

  return EFI_SUCCESS;
}
