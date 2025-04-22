/** @file
  This file provides RPMB helper functions which are specific for storage device.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/



#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/MmcAccessLib.h>
#include <Library/CryptoLib.h>
#include <Library/SecureBootLib.h>
#include <Library/MediaAccessLib.h>
#include <Library/RpmbLib.h>
#include "RpmbLibPrivate.h"


/**
  This function calculated the HMAC SHA-256 by taking in the Input Key and Message through RPMB data frame.
  The key used for the MAC calculation is always the 32 bytes Authentication Key stored in the storage device
  RPMB partition. Output is the 32 byte MAC that is calculated using the HMAC function.

  @param[in]       Frames               A pointer to RPMB_DATA_FRAME i.e; Message to send.
  @param[in]       BlockCnt             Number of RPMB_DATA_FRAME.
  @param[in]       Key                  Input Key to calculate the MAC.
  @param[in]       KeySize              Size of the Input Key.
  @param[out]      Mac                  Calculated MAC from HMAC SHA-256.

  @retval   EFI_SUCCESS                 Successfully calculated HMAC SHA256
  @retval   EFI_PROTOCOL_ERROR          Error caused while calculating HMAC SHA256
**/
EFI_STATUS
RpmbCalcHmacSha256(
  IN RPMB_DATA_FRAME      *Frames,
  IN UINT8                BlockCnt,
  IN UINT8                KeySize,
  IN CONST UINT8          Key[],
  OUT UINT8               Mac[]
  )
{
  //TODO: Andriod does HMAC_Update in a loop of BlockCnt. Change this later if we need BlocksCnt > 1
  return HmacSha256 ((UINT8 *)Frames->Data, HMAC_DATA_LEN, Key, KeySize, Mac, RPMB_MAC_SIZE);
}

/**
  This function checks the MAC which is calculated using HMAC SHA-256, compares the returned MAC with that of the embedded RPMB_DATA_FRAME MAC.

  @param[in]       Key                  32 bytes Input Key to calculate the MAC.
  @param[in]       Frames               A pointer to RPMB_DATA_FRAME.
  @param[in]       BlkCnt               Number of RPMB_DATA_FRAME(s).


  @retval   SUCCESS                      Returns 0.
  @retval   HMAC Mismatch occurred       Returns 1.
**/
UINT8
RpmbCheckMac(
  IN CONST UINT8        *Key,
  IN UINT8              KeySize,
  IN RPMB_DATA_FRAME    *Frames,
  IN UINT8              BlkCnt
  )
{
  UINT8 Mac[RPMB_MAC_SIZE];
  EFI_STATUS Status;

  if (BlkCnt == 0) {
    DEBUG ((DEBUG_ERROR, "RpmbCheckMac: Zero RPMB frames\n"));
    return 1;
  }

  // TODO: BlkCnt is not honored inside the Hmac function.Change this later if needed.
  Status = RpmbCalcHmacSha256(Frames, BlkCnt, KeySize, Key, Mac);
  if(EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "RpmbCheckMac failed: %r\n"));
    return 1;
  }

  if (CompareMem(Mac, Frames[BlkCnt - 1].KeyMac, RPMB_MAC_SIZE)) {
    DEBUG ((DEBUG_ERROR, "RpmbCheckMac: HMAC mismatch occured\n"));
    return 1;
  }

  return 0;
}

/**
  This function sends request type to RPMB memory partition. Request Message Types are as follows:
  0x0001 Authentication key programming request
  0x0002 Reading of the Write Counter value -request
  0x0003 Authenticated data write request
  0x0004 Authenticated data read request
  0x0005 Result read request status

  @param[in]       DeviceIndex            Specifies the block device to which the function wants
                                          to talk.
  @param[in]       Lba                    The starting logical block address to be read/written.
                                          The caller is responsible for reading/writing to only
                                          legitimate locations.
  @param[in, out]  RequestDataFrame       A pointer to RPMB Data frame for the data buffer.
  @param[in]       BufferSize             Size of the RPMB data buffer, must be a multiple of RPMB_DATA_FRAME_SIZE.

  @retval EFI_SUCCESS                     The data was read/written correctly to the device.
  @retval EFI_DEVICE_ERROR                The device reported an error while performing the read/write.
  @retval EFI_INVALID_PARAMETER           The read/write request contains LBAs that are not valid,
                                          or the buffer is not on proper alignment.

**/
EFI_STATUS
RpmbSendRequest (
  IN     UINTN                       DeviceIndex,
  IN     EFI_LBA                     Lba,
  IN OUT RPMB_DATA_FRAME             *RequestDataFrame,
  IN     UINTN                       BufferSize
  )
{
  EFI_STATUS        Status;
  BOOLEAN           IsReliableWrite;

  Status          = EFI_SUCCESS;
  IsReliableWrite = FALSE;

  // Based on the storage device spec:
  if (RequestDataFrame->ReqResp == SwapBytes16(RPMB_REQUEST_AUTH_WRITE) ||
      RequestDataFrame->ReqResp == SwapBytes16(RPMB_REQUEST_KEY_WRITE)) {
    IsReliableWrite = TRUE;
  }

  Status = MediaWriteBlocksExt (DeviceIndex, Lba, BufferSize, (VOID*)RequestDataFrame,IsReliableWrite);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "RPMBSendRequest returned error: %r\n", Status));
  }

  return Status;
}

/**
  This function requests response type from RPMB memory partition. Response Message Types are as follows:
  0x0100 Authentication key programming response
  0x0200 Reading of the Write Counter value -response
  0x0300 Authenticated data write response
  0x0400 Authenticated data read response

  @param[in]       DeviceIndex            Specifies the block device to which the function wants
                                          to talk.
  @param[in]       Lba                    The starting logical block address to be read/written.
                                          The caller is responsible for reading/writing to only
                                          legitimate locations.
  @param[in, out]  ResponseDataFrame       A pointer to RPMB Data frame for the data buffer.
  @param[in]       BufferSize             Size of the RPMB data buffer, must be a multiple of RPMB_DATA_FRAME_SIZE.

  @retval EFI_SUCCESS                     The data was read/written correctly to the device.
  @retval EFI_DEVICE_ERROR                The device reported an error while performing the read/write.
  @retval EFI_INVALID_PARAMETER           The read/write request contains LBAs that are not valid,
                                          or the buffer is not on proper alignment.

**/
EFI_STATUS
RpmbGetResponse (
  IN     UINTN                         DeviceIndex,
  IN     EFI_LBA                       Lba,
  IN OUT RPMB_DATA_FRAME               *ResponseDataFrame,
  IN     UINTN                         BufferSize
  )
{
  EFI_STATUS          Status;

  Status = MediaReadBlocks ( DeviceIndex, Lba, BufferSize, (VOID*)ResponseDataFrame);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "RpmbGetResponse returned error: %r\n", Status));
  }

  return Status;
}

/**
  This function is a combination of send request and get response types from/to RPMB memory partition.
  Request and Response Message Types that can be send to RPMB are as described in above functions.

  @param[in]       DeviceIndex            Specifies the block device to which the function wants
                                          to talk.
  @param[in]       Lba                    The starting logical block address to be read/written.
                                          The caller is responsible for reading/writing to only
                                          legitimate locations.
  @param[in]       RequestDataFrame       A pointer to RPMB Data frame for the Request Msg Type data buffer.
  @param[in, out]  ResponseDataFrame      A pointer to RPMB Data frame for the Respnse Msg Type data buffer.
  @param[in]       ExpectedResponse       This is the expected response that will be compared with ResponseDataFrame->ReqResp.
  @param[out]      ResponseResult         This is the output coming from the Response Data Frame.

  @retval EFI_SUCCESS                     The data was read/written correctly to the device.
  @retval EFI_DEVICE_ERROR                The device reported an error while performing the read/write.
  @retval EFI_INVALID_PARAMETER           The read/write request contains LBAs that are not valid,
                                          or the buffer is not on proper alignment.

**/
EFI_STATUS
EFIAPI
RpmbRequestResponse (
  IN     UINTN                          DeviceIndex,
  IN     EFI_LBA                        Lba,
  IN     RPMB_DATA_FRAME                *RequestDataFrame,
  IN OUT RPMB_DATA_FRAME                *ResponseDataFrame,
  IN     UINT16                         ExpectedResponse,
  OUT    RPMB_RESPONSE_RESULT           *ResponseResult
  )
{
  EFI_STATUS               Status;
  UINT16                   Result;

  Status  = RpmbSendRequest ( DeviceIndex, Lba, RequestDataFrame, (RPMB_DATA_FRAME_SIZE * 1));
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "RpmbRequestResponse: Failed to send request: %r\n", Status));
    return Status;
  }

  Status  = RpmbGetResponse ( DeviceIndex, Lba, ResponseDataFrame, (RPMB_DATA_FRAME_SIZE * 1));
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "RpmbRequestResponse: Failed to get response: %r\n", Status));
    return Status;
  }

  if (SwapBytes16(ResponseDataFrame->ReqResp) != ExpectedResponse) {
    DEBUG ((DEBUG_ERROR, "Error: ExpectedResponse =0x%08x, ReturnedResponse=0x%08x\n", ExpectedResponse, ResponseDataFrame->ReqResp));
    return EFI_ABORTED;
  }

  Result = SwapBytes16(ResponseDataFrame->Result);
  *ResponseResult = (RPMB_RESPONSE_RESULT)Result;
  if (Result != 0) {
    DEBUG ((DEBUG_ERROR, "RpmbRequestResponse: RPMB operation failed: ResponseResult=0x%x\n",*ResponseResult ));
    return EFI_ABORTED;
  }

  return Status;
}
/**
  This function reads the counter value of the RPMB Partition.

  @param[in]       MediumType           Current Medium Type option: eMMC, UFS, NVMe
  @param[in]       Key                  Input Key to authenticate and compare against the
                                        embbeded MAC in RPMB_DATA_FRAME.
  @param[in]       KeySize              Size of the Input Key.
  @param[out]      WriteCounter         How many times a successful write request has been done so far.
  @param[out]      ResponseResult       Output coming from CounterFrame->Result


  @retval          EFI_SUCCESS          The operation is done correctly.
  @retval          Others               The operation fails due to some error.
**/
EFI_STATUS
EFIAPI
RpmbGetCounter (
  IN  OS_BOOT_MEDIUM_TYPE       MediumType,
  IN  CONST UINT8               *Key,
  IN  UINT8                     KeySize,
  OUT UINT32                    *WriteCounter,
  OUT RPMB_RESPONSE_RESULT      *ResponseResult
  )
{
  RPMB_DATA_FRAME             CounterFrame;
  EFI_STATUS                  Status;
  UINT8                       BlkCnt;
  UINTN                       DeviceIndex;

  Status  = EFI_SUCCESS;

  if ((ResponseResult == NULL) || (WriteCounter == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  // Assign the HW partition number correctly based on which storage device it is
  if (MediumType == OsBootDeviceEmmc) {
    DeviceIndex = 3;    //eMMC RPMB Partition
  } else {
    return EFI_UNSUPPORTED;
  }

  ZeroMem(&CounterFrame, sizeof(CounterFrame));
  CounterFrame.ReqResp = SwapBytes16(RPMB_REQUEST_COUNTER_READ);

  Status = GenerateRandomNumbers((CHAR8 *)CounterFrame.Nonce, RPMB_NONCE_SIZE);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "RpmbGetCounter: Failed to generate random numbers: %r\n", Status));
    return Status;
  }

  Status = RpmbRequestResponse(DeviceIndex, 0, &CounterFrame, &CounterFrame, RPMB_RESPONSE_COUNTER_READ, ResponseResult);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "RpmbGetCounter: Failed to request response error: %r\n", Status));
    return Status;
  }

  // Additional checks
  if(Key != NULL) {
    // BlocksCnt is hard-coded to 1 for now
    BlkCnt = 1;
    if (RpmbCheckMac(Key, KeySize, &CounterFrame, BlkCnt) != 0) {
      *ResponseResult = RpmbResponseAuthFailure;
      DEBUG ((DEBUG_ERROR, "RpmbCheckMac failed: %r\n",Status));
      return EFI_ABORTED;
    }
  }

  *WriteCounter = SwapBytes32(CounterFrame.WriteCounter);

  return Status;
}

/**
  This function programs the RPMB key into RPMB Partition.

  @param[in]       MediumType     Current Medium Type option: eMMC, UFS, NVMe
  @param[in]       Lba            The starting logical block address to be read/written.
                                  The caller is responsible for reading/writing to only
                                  legitimate locations.
  @param[in]       Key            Input Key to check if programmed or not.
  @param[in]       KeySize        Size of the Input Key.
  @param[out]      ResponseResult This is the output coming from the Response DataFrame.
                                  RPMB_RES_OK,
                                  RPMB_RES_GENERAL_FAILURE,
                                  RPMB_RES_AUTH_FAILURE,
                                  RPMB_RES_NO_AUTH_KEY_PROGRAM.


  @retval          EFI_SUCCESS    The operation is done correctly.
  @retval          Others         The operation fails due to some error.
**/
EFI_STATUS
EFIAPI
RpmbProgramKey (
  IN OS_BOOT_MEDIUM_TYPE           MediumType,
  IN EFI_LBA                       Lba,
  IN CONST UINT8                   *Key,
  IN UINT8                         KeySize,
  OUT RPMB_RESPONSE_RESULT         *ResponseResult
  )
{
  BOOLEAN               IsRpmbProgrammed;
  EFI_STATUS            Status;
  UINT32                WriteCounter;
  RPMB_DATA_FRAME       DataFrame;
  RPMB_DATA_FRAME       StatusFrame;
  UINTN                 DeviceIndex;

  if ((Key == NULL) || (ResponseResult == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  // Assign the HW partition number correctly based on which storage device it is
  if (MediumType == OsBootDeviceEmmc) {
    DeviceIndex = 3;    //eMMC RPMB Partition
  } else {
    return EFI_UNSUPPORTED;
  }

  *ResponseResult = RpmbResponseOK;
  Status = RpmbGetCounter(MediumType, Key, KeySize, &WriteCounter, ResponseResult);
  if (EFI_ERROR(Status) && (*ResponseResult == RpmbResponseNoAuthKeyProgram)) {
    IsRpmbProgrammed = FALSE;
    DEBUG ((DEBUG_INFO, "RPMB Key not programmed: Status:%r\n",Status));
  } else {
    IsRpmbProgrammed = TRUE;
    DEBUG ((DEBUG_ERROR, "RPMB Key already programmed: Status:%r\n",Status));
  }

  if (*ResponseResult == RpmbResponseAuthFailure) {
    DEBUG ((DEBUG_ERROR, "MAC Calculation failed: %r %d\n", Status));
    return Status;
  }

  if(!IsRpmbProgrammed) {
    ZeroMem(&DataFrame, sizeof(DataFrame));
    DataFrame.ReqResp = SwapBytes16(RPMB_REQUEST_KEY_WRITE);
    CopyMem(DataFrame.KeyMac, Key, KeySize);

    Status  = RpmbSendRequest (DeviceIndex, Lba, &DataFrame, (RPMB_DATA_FRAME_SIZE * 1));
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "ProgramRpmbKey: Failed to send request: %r\n", Status));
      return Status;
    }

    ZeroMem(&StatusFrame, sizeof(StatusFrame));
    StatusFrame.ReqResp = SwapBytes16(RPMB_REQUEST_STATUS);

    Status = RpmbRequestResponse(DeviceIndex, Lba, &StatusFrame, &StatusFrame, RPMB_RESPONSE_KEY_WRITE, ResponseResult);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "ProgramRpmbKey: Failed to request response: %r, ResponseResult: 0x%x\n", Status, *ResponseResult));
      return Status;
    }
  }

  return Status;
}


/**
  This function writes to RPMB memory partition. Request Message Type is
  Authenticated data write. If the MAC in the request message and the calculated MAC are
  different, then this returns Authentication Failure. No data will be written.

  @param[in]       MediumType             Current Medium Type option: eMMC, UFS, NVMe
  @param[in]       Lba                    The starting logical block address to be read/written.
                                          The caller is responsible for reading/writing to only
                                          legitimate locations.
  @param[in]       BlkCnt                 Number of RPMB_DATA_FRAME(s).
  @param[in]       BlkAddr                Start address of the full packet.
  @param[in]       Buffer                 Data to be written. Should be a multiple of RPMB_BLOCK_SIZE.
  @param[in]       Key                    Input Key to calculate the MAC.
  @param[in]       KeySize                Size of the Input Key.
  @param[out]      Result                 Output coming from the Response DataFrame.

  @retval          EFI_SUCCESS            The operation is done correctly.
  @retval          Others                 The operation fails due to some error.

**/
EFI_STATUS
EFIAPI
RpmbWriteData (
  IN OS_BOOT_MEDIUM_TYPE           MediumType,
  IN EFI_LBA                       Lba,
  IN UINT8                         BlkCnt,
  IN UINT16                        BlkAddr,
  IN VOID                          *Buffer,
  IN CONST UINT8                   *Key,
  IN UINT8                         KeySize,
  OUT RPMB_RESPONSE_RESULT         *Result
  )
{
  UINT32                  WriteCounter;
  UINT8                   Mac[RPMB_DATA_MAC];
  RPMB_DATA_FRAME         *DataInFrame;
  RPMB_DATA_FRAME         StatusFrame;
  EFI_STATUS              Status;
  UINT16                  Loop;
  UINTN                   DeviceIndex;

  Status      = EFI_SUCCESS;
  DataInFrame = NULL;

  if ((Buffer == NULL) || (Result == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  // Assign the HW partition number correctly based on which storage device it is
  if ( MediumType == OsBootDeviceEmmc) {
    DeviceIndex = 3;    //eMMC RPMB Partition
  } else {
    return EFI_UNSUPPORTED;
  }

  DataInFrame = AllocatePool(sizeof(RPMB_DATA_FRAME));
  if (DataInFrame == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = RpmbGetCounter(MediumType, Key, KeySize, &WriteCounter, Result);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "WriteRpmbData: Failed to get counter %r\n", Status));
    goto Exit;
  }

  // Loop through block count
  for (Loop = 0; Loop < BlkCnt; Loop++) {

    // Fill in the Data frame parameters
    ZeroMem(DataInFrame, sizeof(RPMB_DATA_FRAME));

    DataInFrame->Address = SwapBytes16(BlkAddr + Loop);
    DataInFrame->BlockCnt = SwapBytes16(1);
    DataInFrame->ReqResp = SwapBytes16(RPMB_REQUEST_AUTH_WRITE);
    DataInFrame->WriteCounter = SwapBytes32(WriteCounter);
    CopyMem(&DataInFrame->Data, (UINT8 *)Buffer + Loop * 256, 256);

    // Note: BlkCnt is not honored inside the Hmac function. Change this later if needed.
    Status = RpmbCalcHmacSha256(DataInFrame, BlkCnt, KeySize, Key, Mac );
    if(EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "WriteRpmbData: HMAC failed %r\n", Status));
      Status = EFI_INVALID_PARAMETER;
      goto Exit;
    }
    CopyMem(DataInFrame->KeyMac, Mac, RPMB_DATA_MAC);

    Status  = RpmbSendRequest (DeviceIndex, Lba, DataInFrame, (RPMB_DATA_FRAME_SIZE * 1));
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "WriteRpmbData: Failed to send request: %r\n", Status));
      goto Exit;
    }
    ZeroMem(&StatusFrame, sizeof(StatusFrame));
    StatusFrame.ReqResp = SwapBytes16(RPMB_REQUEST_STATUS);

    Status = RpmbRequestResponse(DeviceIndex, Lba, &StatusFrame, &StatusFrame, RPMB_RESPONSE_AUTH_WRITE, Result);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "WriteRpmbData: Failed to request response: Status: %r, Result: %d\n", Status, *Result));
      goto Exit;
    }

    // Sanity check if WriteCounter has incremented or not because by this time,
    // StatusFrame.WriteCounter should have been incremented atleast by 1
    if (WriteCounter >= SwapBytes32(StatusFrame.WriteCounter)) {
      DEBUG ((DEBUG_ERROR, "WriteRpmbData: RPMB WriteCounter >= returned WriteCounter 0x%0x \n",StatusFrame.WriteCounter));
      Status = EFI_ABORTED;
      goto Exit;
    }
    WriteCounter++;
  }

Exit:
  if (DataInFrame != NULL) {
    FreePool(DataInFrame);
  }

  return Status;
}

/**
  This function reads data from RPMB memory partition. Request Message Type is
  Authenticated data read. If the MAC in the request message and the calculated MAC are
  different, then this returns Authentication Failure. No data will be read back.

  @param[in]       MediumType             Current Medium Type option: eMMC, UFS, NVMe
  @param[in]       Lba                    The starting logical block address to be read/written.
                                          The caller is responsible for reading/writing to only
                                          legitimate locations.
  @param[in]       BlkCount               Number of RPMB_DATA_FRAME(s).
  @param[in]       BlkAddr                Start address of the full packet.
  @param[out]      Buffer                 Data to be read back. Should be a multiple of RPMB_BLOCK_SIZE.
  @param[in]       Key                    Input Key to calculate the MAC.
  @param[in]       KeySize                Size of the Input Key.
  @param[out]      Result                 Output coming from the Response DataFrame.

  @retval          EFI_SUCCESS            The operation is done correctly.
  @retval          Others                 The operation fails due to some error.

**/
EFI_STATUS
EFIAPI
RpmbReadData (
  IN OS_BOOT_MEDIUM_TYPE    MediumType,
  IN EFI_LBA                Lba,
  IN UINT8                  BlkCount,
  IN UINT16                 BlkAddr,
  OUT VOID                  *Buffer,
  IN CONST UINT8            *Key,
  IN UINT8                  KeySize,
  OUT RPMB_RESPONSE_RESULT  *Result
  )
{
  EFI_STATUS                Status;
  RPMB_DATA_FRAME           DataInFrame;
  RPMB_DATA_FRAME           *DataOutFrame;
  UINT16                    Loop;
  UINT8                     Random[16];
  UINTN                     DeviceIndex;

  Status = EFI_SUCCESS;
  DataOutFrame  = NULL;

  if ((Buffer == NULL) || (Result == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  // Assign the HW partition number correctly based on which storage device it is
  if (MediumType == OsBootDeviceEmmc) {
    DeviceIndex = 3;    //eMMC RPMB Partition
  } else {
    return EFI_UNSUPPORTED;
  }

  DataOutFrame = AllocatePool(sizeof(RPMB_DATA_FRAME) * BlkCount);
  if (DataOutFrame == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG ((DEBUG_ERROR, "RpmbReadData: Out of resources: %r \n",Status));
    goto Exit;
  }
  ZeroMem(&DataInFrame, sizeof(DataInFrame));
  ZeroMem(DataOutFrame, sizeof(RPMB_DATA_FRAME) * BlkCount);

  DataInFrame.Address = SwapBytes16(BlkAddr);
  DataInFrame.ReqResp = SwapBytes16(RPMB_REQUEST_AUTH_READ);

  ZeroMem(Random, sizeof(Random));
  Status = GenerateRandomNumbers((CHAR8 *)Random, RPMB_NONCE_SIZE);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "RpmbReadData: Failed to generate Random numbers %r \n",Status));
    goto Exit;
  }
  CopyMem(DataInFrame.Nonce, Random, RPMB_NONCE_SIZE);
  Status = RpmbRequestResponse(DeviceIndex, Lba, &DataInFrame, DataOutFrame, RPMB_RESPONSE_AUTH_READ, Result);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "RpmbReadData: RpmbRequestResponse failed: %r \n",Status));
    goto Exit;
  }

  // Additional Checks
  if (Key != NULL) {
    // BlocksCnt is hard-coded to 1 for now
    if(RpmbCheckMac(Key, KeySize, DataOutFrame, BlkCount) != 0) {
      *Result = RpmbResponseAuthFailure;
      Status = EFI_ABORTED;
      DEBUG ((DEBUG_ERROR, "RpmbReadData: RpmbCheckMac failed: %r \n",Status));
      goto Exit;
    }
  }

  if (CompareMem(&Random, &DataOutFrame[BlkCount - 1].Nonce, RPMB_NONCE_SIZE)) {
    Status = EFI_ABORTED;
    DEBUG ((DEBUG_ERROR, "RpmbReadData: Nonce values are not the same %r \n",Status));
    goto Exit;
  }

  for (Loop = 0; Loop < BlkCount; Loop++) {
    CopyMem((UINT8 *)Buffer + Loop * RPMB_BLOCK_SIZE, DataOutFrame[Loop].Data, RPMB_BLOCK_SIZE);
  }

Exit:
  if (DataOutFrame != NULL) {
    FreePool(DataOutFrame);
  }

  return Status;
}
