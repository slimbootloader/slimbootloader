/** @file

  Copyright (c) 2014 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UfsInternal.h"

/**
  Wait for the value of the specified system memory set to the test value.

  @param  Address           The system memory address to test.
  @param  MaskValue         The mask value of memory.
  @param  TestValue         The test value of memory.
  @param  Timeout           The time out value for wait memory set, uses 100ns as a unit.

  @retval EFI_TIMEOUT       The system memory setting is time out.
  @retval EFI_SUCCESS       The system memory is correct set.

**/
EFI_STATUS
EFIAPI
UfsWaitMemSet (
  IN  UINTN                     Address,
  IN  UINT32                    MaskValue,
  IN  UINT32                    TestValue,
  IN  UINT64                    Timeout
  )
{
  UINT32     Value;
  UINT64     Delay;
  BOOLEAN    InfiniteWait;

  if (Timeout == 0) {
    InfiniteWait = TRUE;
  } else {
    InfiniteWait = FALSE;
  }

  Delay = DivU64x32 (Timeout, 10) + 1;

  do {
    //
    // Access PCI MMIO space to see if the value is the tested one.
    //
    Value = MmioRead32 (Address) & MaskValue;

    if (Value == TestValue) {
      return EFI_SUCCESS;
    }

    //
    // Stall for 1 microseconds.
    //
    MicroSecondDelay (1);

    Delay--;

  } while (InfiniteWait || (Delay > 0));

  return EFI_TIMEOUT;
}

/**
  Dump UIC command execution result for debugging.

  @param[in]   UicOpcode  The executed UIC opcode.
  @param[in]   Result     The result to be parsed.

**/
VOID
DumpUicCmdExecResult (
  IN  UINT8     UicOpcode,
  IN  UINT8     Result
  )
{
  if (UicOpcode <= UfsUicDmePeerSet) {
    switch (Result) {
    case 0x00:
      break;
    case 0x01:
      DEBUG ((DEBUG_VERBOSE, "UIC configuration command fails - INVALID_MIB_ATTRIBUTE\n"));
      break;
    case 0x02:
      DEBUG ((DEBUG_VERBOSE, "UIC configuration command fails - INVALID_MIB_ATTRIBUTE_VALUE\n"));
      break;
    case 0x03:
      DEBUG ((DEBUG_VERBOSE, "UIC configuration command fails - READ_ONLY_MIB_ATTRIBUTE\n"));
      break;
    case 0x04:
      DEBUG ((DEBUG_VERBOSE, "UIC configuration command fails - WRITE_ONLY_MIB_ATTRIBUTE\n"));
      break;
    case 0x05:
      DEBUG ((DEBUG_VERBOSE, "UIC configuration command fails - BAD_INDEX\n"));
      break;
    case 0x06:
      DEBUG ((DEBUG_VERBOSE, "UIC configuration command fails - LOCKED_MIB_ATTRIBUTE\n"));
      break;
    case 0x07:
      DEBUG ((DEBUG_VERBOSE, "UIC configuration command fails - BAD_TEST_FEATURE_INDEX\n"));
      break;
    case 0x08:
      DEBUG ((DEBUG_VERBOSE, "UIC configuration command fails - PEER_COMMUNICATION_FAILURE\n"));
      break;
    case 0x09:
      DEBUG ((DEBUG_VERBOSE, "UIC configuration command fails - BUSY\n"));
      break;
    case 0x0A:
      DEBUG ((DEBUG_VERBOSE, "UIC configuration command fails - DME_FAILURE\n"));
      break;
    default :
      ASSERT (FALSE);
      break;
    }
  } else {
    switch (Result) {
    case 0x00:
      break;
    case 0x01:
      DEBUG ((DEBUG_VERBOSE, "UIC control command fails - FAILURE\n"));
      break;
    default :
      ASSERT (FALSE);
      break;
    }
  }
}

/**
  Dump QUERY RESPONSE UPIU result for debugging.

  @param[in]   Result  The result to be parsed.

**/
VOID
DumpQueryResponseResult (
  IN  UINT8     Result
  )
{
  switch (Result) {
  case 0xF6:
    DEBUG ((DEBUG_VERBOSE, "Query Response with Parameter Not Readable\n"));
    break;
  case 0xF7:
    DEBUG ((DEBUG_VERBOSE, "Query Response with Parameter Not Writeable\n"));
    break;
  case 0xF8:
    DEBUG ((DEBUG_VERBOSE, "Query Response with Parameter Already Written\n"));
    break;
  case 0xF9:
    DEBUG ((DEBUG_VERBOSE, "Query Response with Invalid Length\n"));
    break;
  case 0xFA:
    DEBUG ((DEBUG_VERBOSE, "Query Response with Invalid Value\n"));
    break;
  case 0xFB:
    DEBUG ((DEBUG_VERBOSE, "Query Response with Invalid Selector\n"));
    break;
  case 0xFC:
    DEBUG ((DEBUG_VERBOSE, "Query Response with Invalid Index\n"));
    break;
  case 0xFD:
    DEBUG ((DEBUG_VERBOSE, "Query Response with Invalid Idn\n"));
    break;
  case 0xFE:
    DEBUG ((DEBUG_VERBOSE, "Query Response with Invalid Opcode\n"));
    break;
  case 0xFF:
    DEBUG ((DEBUG_VERBOSE, "Query Response with General Failure\n"));
    break;
  default :
    ASSERT (FALSE);
    break;
  }
}

/**
  Swap little endian to big endian.

  @param[in, out] Buffer      The data buffer. In input, it contains little endian data.
                              In output, it will become big endian.
  @param[in]      BufferSize  The length of converted data.

**/
VOID
SwapLittleEndianToBigEndian (
  IN OUT UINT8         *Buffer,
  IN     UINT32        BufferSize
  )
{
  UINT32 Index;
  UINT8  Temp;
  UINT32 SwapCount;

  SwapCount = BufferSize / 2;
  for (Index = 0; Index < SwapCount; Index++) {
    Temp = Buffer[Index];
    Buffer[Index] = Buffer[BufferSize - 1 - Index];
    Buffer[BufferSize - 1 - Index] = Temp;
  }
}

/**
  Fill TSF field of QUERY REQUEST UPIU.

  @param[in, out] TsfBase      The base address of TSF field of QUERY REQUEST UPIU.
  @param[in]      Opcode       The opcode of request.
  @param[in]      DescId       The descriptor ID of request.
  @param[in]      Index        The index of request.
  @param[in]      Selector     The selector of request.
  @param[in]      Length       The length of transferred data. The maximum is 4.
  @param[in]      Value        The value of transferred data.

**/
VOID
UfsFillTsfOfQueryReqUpiu (
  IN OUT UTP_UPIU_TSF        *TsfBase,
  IN     UINT8               Opcode,
  IN     UINT8               DescId    OPTIONAL,
  IN     UINT8               Index     OPTIONAL,
  IN     UINT8               Selector  OPTIONAL,
  IN     UINT16              Length    OPTIONAL,
  IN     UINT32              Value     OPTIONAL
  )
{
  ASSERT (TsfBase != NULL);
  ASSERT (Opcode <= UtpQueryFuncOpcodeTogFlag);

  TsfBase->Opcode   = Opcode;
  if (Opcode != UtpQueryFuncOpcodeNop) {
    TsfBase->DescId   = DescId;
    TsfBase->Index    = Index;
    TsfBase->Selector = Selector;

    if ((Opcode == UtpQueryFuncOpcodeRdDesc) || (Opcode == UtpQueryFuncOpcodeWrDesc)) {
      SwapLittleEndianToBigEndian ((UINT8 *)&Length, sizeof (Length));
      TsfBase->Length = Length;
    }

    if (Opcode == UtpQueryFuncOpcodeWrAttr) {
      SwapLittleEndianToBigEndian ((UINT8 *)&Value, sizeof (Value));
      TsfBase->Value  = Value;
    }
  }
}

/**
  Initialize COMMAND UPIU.

  @param[in, out] Command         The base address of COMMAND UPIU.
  @param[in]      Lun             The Lun on which the SCSI command is executed.
  @param[in]      TaskTag         The task tag of request.
  @param[in]      Cdb             The cdb buffer containing SCSI command.
  @param[in]      CdbLength       The cdb length.
  @param[in]      DataDirection   The direction of data transfer.
  @param[in]      ExpDataTranLen  The expected transfer data length.

  @retval EFI_SUCCESS     The initialization succeed.

**/
EFI_STATUS
UfsInitCommandUpiu (
  IN OUT UTP_COMMAND_UPIU              *Command,
  IN     UINT8                         Lun,
  IN     UINT8                         TaskTag,
  IN     UINT8                         *Cdb,
  IN     UINT8                         CdbLength,
  IN     UFS_DATA_DIRECTION            DataDirection,
  IN     UINT32                        ExpDataTranLen
  )
{
  UINT8                   Flags;

  ASSERT ((Command != NULL) && (Cdb != NULL));

  //
  // Task attribute is hard-coded to Ordered.
  //
  if (DataDirection == UfsDataIn) {
    Flags = BIT0 | BIT6;
  } else if (DataDirection == UfsDataOut) {
    Flags = BIT0 | BIT5;
  } else {
    Flags = BIT0;
  }

  //
  // Fill UTP COMMAND UPIU associated fields.
  //
  Command->TransCode = 0x01;
  Command->Flags     = Flags;
  Command->Lun       = Lun;
  Command->TaskTag   = TaskTag;
  Command->CmdSet    = 0x00;
  SwapLittleEndianToBigEndian ((UINT8 *)&ExpDataTranLen, sizeof (ExpDataTranLen));
  Command->ExpDataTranLen = ExpDataTranLen;

  CopyMem (Command->Cdb, Cdb, CdbLength);

  return EFI_SUCCESS;
}

/**
  Initialize UTP PRDT for data transfer.

  @param[in] Prdt         The base address of PRDT.
  @param[in] Buffer       The buffer to be read or written.
  @param[in] BufferSize   The data size to be read or written.

  @retval EFI_SUCCESS     The initialization succeed.

**/
EFI_STATUS
UfsInitUtpPrdt (
  IN  UTP_TR_PRD                       *Prdt,
  IN  VOID                             *Buffer,
  IN  UINT32                           BufferSize
  )
{
  UINT32     PrdtIndex;
  UINT32     RemainingLen;
  UINT8      *Remaining;
  UINTN      PrdtNumber;

  if ((BufferSize & (BIT0 | BIT1)) != 0) {
    BufferSize &= ~ (BIT0 | BIT1);
    DEBUG ((DEBUG_WARN, "UfsInitUtpPrdt: The BufferSize [%d] is not dword-aligned!\n", BufferSize));
  }

  if (BufferSize == 0) {
    return EFI_SUCCESS;
  }

  ASSERT (((UINTN)Buffer & (BIT0 | BIT1)) == 0);

  RemainingLen = BufferSize;
  Remaining    = Buffer;
  PrdtNumber   = (UINTN)DivU64x32 ((UINT64)BufferSize + UFS_MAX_DATA_LEN_PER_PRD - 1, UFS_MAX_DATA_LEN_PER_PRD);

  for (PrdtIndex = 0; PrdtIndex < PrdtNumber; PrdtIndex++) {
    if (RemainingLen < UFS_MAX_DATA_LEN_PER_PRD) {
      Prdt[PrdtIndex].DbCount = (UINT32)RemainingLen - 1;
    } else {
      Prdt[PrdtIndex].DbCount = UFS_MAX_DATA_LEN_PER_PRD - 1;
    }

    Prdt[PrdtIndex].DbAddr  = (UINT32)RShiftU64 ((UINT64) (UINTN)Remaining, 2);
    Prdt[PrdtIndex].DbAddrU = (UINT32)RShiftU64 ((UINT64) (UINTN)Remaining, 32);
    RemainingLen -= UFS_MAX_DATA_LEN_PER_PRD;
    Remaining    += UFS_MAX_DATA_LEN_PER_PRD;
  }

  return EFI_SUCCESS;
}

/**
  Initialize QUERY REQUEST UPIU.

  @param[in, out] QueryReq      The base address of QUERY REQUEST UPIU.
  @param[in]      TaskTag       The task tag of request.
  @param[in]      Opcode        The opcode of request.
  @param[in]      DescId        The descriptor ID of request.
  @param[in]      Index         The index of request.
  @param[in]      Selector      The selector of request.
  @param[in]      DataSize      The data size to be read or written.
  @param[in]      Data          The buffer to be read or written.

  @retval EFI_SUCCESS           The initialization succeed.

**/
EFI_STATUS
UfsInitQueryRequestUpiu (
  IN OUT UTP_QUERY_REQ_UPIU            *QueryReq,
  IN     UINT8                         TaskTag,
  IN     UINT8                         Opcode,
  IN     UINT8                         DescId,
  IN     UINT8                         Index,
  IN     UINT8                         Selector,
  IN     UINTN                         DataSize   OPTIONAL,
  IN     UINT8                         *Data      OPTIONAL
  )
{
  ASSERT (QueryReq != NULL);

  QueryReq->TransCode = 0x16;
  QueryReq->TaskTag   = TaskTag;
  if ((Opcode == UtpQueryFuncOpcodeRdDesc) || (Opcode == UtpQueryFuncOpcodeRdFlag) || (Opcode == UtpQueryFuncOpcodeRdAttr)) {
    QueryReq->QueryFunc = QUERY_FUNC_STD_READ_REQ;
  } else {
    QueryReq->QueryFunc = QUERY_FUNC_STD_WRITE_REQ;
  }

  if (Opcode == UtpQueryFuncOpcodeWrAttr) {
    UfsFillTsfOfQueryReqUpiu (&QueryReq->Tsf, Opcode, DescId, Index, Selector, 0, * (UINT32 *)Data);
  } else if ((Opcode == UtpQueryFuncOpcodeRdDesc) || (Opcode == UtpQueryFuncOpcodeWrDesc)) {
    UfsFillTsfOfQueryReqUpiu (&QueryReq->Tsf, Opcode, DescId, Index, Selector, (UINT16)DataSize, 0);
  } else {
    UfsFillTsfOfQueryReqUpiu (&QueryReq->Tsf, Opcode, DescId, Index, Selector, 0, 0);
  }

  if (Opcode == UtpQueryFuncOpcodeWrDesc) {
    CopyMem (QueryReq + 1, Data, DataSize);

    SwapLittleEndianToBigEndian ((UINT8*)&DataSize, sizeof (UINT16));
    QueryReq->DataSegLen = (UINT16)DataSize;
  }

  return EFI_SUCCESS;
}

/**
  Allocate COMMAND/RESPONSE UPIU for filling UTP TRD's command descriptor field.

  @param[in]  Private           The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  Lun               The Lun on which the SCSI command is executed.
  @param[in]  Packet            The pointer to the UFS_SCSI_REQUEST_PACKET data structure.
  @param[in]  Trd               The pointer to the UTP Transfer Request Descriptor.
  @param[out] BufferMap         A resulting value, if not NULL, to pass to IoMmuUnmap().

  @retval EFI_SUCCESS           The creation succeed.
  @retval EFI_DEVICE_ERROR      The creation failed.
  @retval EFI_OUT_OF_RESOURCES  The memory resource is insufficient.

**/
EFI_STATUS
UfsCreateScsiCommandDesc (
  IN  UFS_PEIM_HC_PRIVATE_DATA            *Private,
  IN  UINT8                               Lun,
  IN  UFS_SCSI_REQUEST_PACKET             *Packet,
  IN  UTP_TRD                             *Trd,
  OUT VOID                                **BufferMap
  )
{
  UINT8                    *CommandDesc;
  UINTN                    TotalLen;
  UINTN                    PrdtNumber;
  VOID                     *Buffer;
  UINT32                   Length;
  UTP_COMMAND_UPIU         *CommandUpiu;
  UTP_TR_PRD               *PrdtBase;
  UFS_DATA_DIRECTION       DataDirection;
  EFI_STATUS               Status;
  EDKII_IOMMU_OPERATION    MapOp;
  UINTN                    MapLength;
  EFI_PHYSICAL_ADDRESS     BufferPhyAddr;

  ASSERT ((Private != NULL) && (Packet != NULL) && (Trd != NULL));

  BufferPhyAddr = 0;

  if (Packet->DataDirection == UfsDataIn) {
    Buffer = Packet->InDataBuffer;
    Length = Packet->InTransferLength;
    DataDirection = UfsDataIn;
    MapOp         = EdkiiIoMmuOperationBusMasterWrite;
  } else {
    Buffer = Packet->OutDataBuffer;
    Length = Packet->OutTransferLength;
    DataDirection = UfsDataOut;
    MapOp         = EdkiiIoMmuOperationBusMasterRead;
  }

  if (Length == 0) {
    DataDirection = UfsNoData;
  } else {
    MapLength = Length;
    Status = IoMmuMap (MapOp, Buffer, &MapLength, &BufferPhyAddr, BufferMap);

    if (EFI_ERROR (Status) || (MapLength != Length)) {
      DEBUG ((DEBUG_ERROR, "UfsCreateScsiCommandDesc: Fail to map data buffer.\n"));
      return EFI_OUT_OF_RESOURCES;
    }
  }

  PrdtNumber = (UINTN)DivU64x32 ((UINT64)Length + UFS_MAX_DATA_LEN_PER_PRD - 1, UFS_MAX_DATA_LEN_PER_PRD);

  TotalLen    = ROUNDUP8 (sizeof (UTP_COMMAND_UPIU)) + ROUNDUP8 (sizeof (UTP_RESPONSE_UPIU)) + PrdtNumber * sizeof (UTP_TR_PRD);
  CommandDesc = UfsAllocateMem (Private->Pool, TotalLen);
  if (CommandDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CommandUpiu  = (UTP_COMMAND_UPIU *)CommandDesc;
  PrdtBase     = (UTP_TR_PRD*)(CommandDesc + ROUNDUP8 (sizeof (UTP_COMMAND_UPIU)) + ROUNDUP8 (sizeof (UTP_RESPONSE_UPIU)));

  UfsInitCommandUpiu (CommandUpiu, Lun, Private->TaskTag++, Packet->Cdb, Packet->CdbLength, DataDirection, Length);
  UfsInitUtpPrdt (PrdtBase, (VOID*)(UINTN)BufferPhyAddr, Length);

  //
  // Fill UTP_TRD associated fields
  // NOTE: Some UFS host controllers request the Response UPIU and the Physical Region Description Table
  // *MUST* be located at a 64-bit aligned boundary.
  //
  Trd->Int    = UFS_INTERRUPT_COMMAND;
  Trd->Dd     = DataDirection;
  Trd->Ct     = UFS_STORAGE_COMMAND_TYPE;
  Trd->Ocs    = UFS_HC_TRD_OCS_INIT_VALUE;
  Trd->UcdBa  = (UINT32)RShiftU64 ((UINT64) (UINTN)CommandUpiu, 7);
  Trd->UcdBaU = (UINT32)RShiftU64 ((UINT64) (UINTN)CommandUpiu, 32);
  Trd->RuL    = (UINT16)DivU64x32 ((UINT64)ROUNDUP8 (sizeof (UTP_RESPONSE_UPIU)), sizeof (UINT32));
  Trd->RuO    = (UINT16)DivU64x32 ((UINT64)ROUNDUP8 (sizeof (UTP_COMMAND_UPIU)), sizeof (UINT32));
  Trd->PrdtL  = (UINT16)PrdtNumber;
  Trd->PrdtO  = (UINT16)DivU64x32 ((UINT64)(ROUNDUP8 (sizeof (UTP_COMMAND_UPIU)) + ROUNDUP8 (sizeof (UTP_RESPONSE_UPIU))), sizeof (UINT32));
  return EFI_SUCCESS;
}

/**
  Allocate QUERY REQUEST/QUERY RESPONSE UPIU for filling UTP TRD's command descriptor field.

  @param[in]  Private           The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  Packet            The pointer to the UFS_DEVICE_MANAGEMENT_REQUEST_PACKET data structure.
  @param[in]  Trd               The pointer to the UTP Transfer Request Descriptor.

  @retval EFI_SUCCESS           The creation succeed.
  @retval EFI_DEVICE_ERROR      The creation failed.
  @retval EFI_OUT_OF_RESOURCES  The memory resource is insufficient.
  @retval EFI_INVALID_PARAMETER The parameter passed in is invalid.

**/
EFI_STATUS
UfsCreateDMCommandDesc (
  IN  UFS_PEIM_HC_PRIVATE_DATA              *Private,
  IN  UFS_DEVICE_MANAGEMENT_REQUEST_PACKET  *Packet,
  IN  UTP_TRD                               *Trd
  )
{
  UINT8                         *CommandDesc;
  UINTN                         TotalLen;
  UTP_QUERY_REQ_UPIU            *QueryReqUpiu;
  UINT8                         Opcode;
  UINT32                        DataSize;
  UINT8                         *Data;
  UINT8                         DataDirection;

  ASSERT ((Private != NULL) && (Packet != NULL) && (Trd != NULL));

  Opcode = Packet->Opcode;
  if ((Opcode > UtpQueryFuncOpcodeTogFlag) || (Opcode == UtpQueryFuncOpcodeNop)) {
    return EFI_INVALID_PARAMETER;
  }

  DataDirection = Packet->DataDirection;
  DataSize      = Packet->TransferLength;
  Data          = Packet->DataBuffer;

  if ((Opcode == UtpQueryFuncOpcodeWrDesc) || (Opcode == UtpQueryFuncOpcodeRdDesc)) {
    if (DataSize == 0 || Data == NULL) {
      return EFI_INVALID_PARAMETER;
    }
    TotalLen = ROUNDUP8 (sizeof (UTP_QUERY_REQ_UPIU)) + ROUNDUP8 (sizeof (UTP_QUERY_RESP_UPIU)) + ROUNDUP8 (DataSize);
  } else {
    TotalLen = ROUNDUP8 (sizeof (UTP_QUERY_REQ_UPIU)) + ROUNDUP8 (sizeof (UTP_QUERY_RESP_UPIU));
  }

  CommandDesc = UfsAllocateMem (Private->Pool, TotalLen);
  if (CommandDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize UTP QUERY REQUEST UPIU
  //
  QueryReqUpiu = (UTP_QUERY_REQ_UPIU *)CommandDesc;
  UfsInitQueryRequestUpiu (
    QueryReqUpiu,
    Private->TaskTag++,
    Opcode,
    Packet->DescId,
    Packet->Index,
    Packet->Selector,
    DataSize,
    Data
    );

  //
  // Fill UTP_TRD associated fields
  // NOTE: Some UFS host controllers request the Query Response UPIU *MUST* be located at a 64-bit aligned boundary.
  //
  Trd->Int    = UFS_INTERRUPT_COMMAND;
  Trd->Dd     = DataDirection;
  Trd->Ct     = UFS_STORAGE_COMMAND_TYPE;
  Trd->Ocs    = UFS_HC_TRD_OCS_INIT_VALUE;
  Trd->UcdBa  = (UINT32)RShiftU64 ((UINT64) (UINTN)QueryReqUpiu, 7);
  Trd->UcdBaU = (UINT32)RShiftU64 ((UINT64) (UINTN)QueryReqUpiu, 32);
  if (Opcode == UtpQueryFuncOpcodeWrDesc) {
    Trd->RuL  = (UINT16)DivU64x32 ((UINT64)ROUNDUP8 (sizeof (UTP_QUERY_RESP_UPIU)), sizeof (UINT32));
    Trd->RuO  = (UINT16)DivU64x32 ((UINT64)ROUNDUP8 (sizeof (UTP_QUERY_REQ_UPIU)) + ROUNDUP8 (DataSize), sizeof (UINT32));
  } else {
    Trd->RuL  = (UINT16)DivU64x32 ((UINT64)ROUNDUP8 (sizeof (UTP_QUERY_RESP_UPIU)) + ROUNDUP8 (DataSize), sizeof (UINT32));
    Trd->RuO  = (UINT16)DivU64x32 ((UINT64)ROUNDUP8 (sizeof (UTP_QUERY_REQ_UPIU)), sizeof (UINT32));
  }

  return EFI_SUCCESS;
}

/**
  Allocate NOP IN and NOP OUT UPIU for filling UTP TRD's command descriptor field.

  @param[in]  Private           The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  Trd               The pointer to the UTP Transfer Request Descriptor.

  @retval EFI_SUCCESS           The creation succeed.
  @retval EFI_DEVICE_ERROR      The creation failed.
  @retval EFI_OUT_OF_RESOURCES  The memory resource is insufficient.

**/
EFI_STATUS
UfsCreateNopCommandDesc (
  IN  UFS_PEIM_HC_PRIVATE_DATA                    *Private,
  IN  UTP_TRD                                     *Trd
  )
{
  UINT8                    *CommandDesc;
  UINTN                    TotalLen;
  UTP_NOP_OUT_UPIU         *NopOutUpiu;

  ASSERT ((Private != NULL) && (Trd != NULL));

  TotalLen    = ROUNDUP8 (sizeof (UTP_NOP_OUT_UPIU)) + ROUNDUP8 (sizeof (UTP_NOP_IN_UPIU));
  CommandDesc = UfsAllocateMem (Private->Pool, TotalLen);
  if (CommandDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  NopOutUpiu = (UTP_NOP_OUT_UPIU *)CommandDesc;

  ASSERT (NopOutUpiu != NULL);
  NopOutUpiu->TaskTag = Private->TaskTag++;

  //
  // Fill UTP_TRD associated fields
  // NOTE: Some UFS host controllers request the Nop Out UPIU *MUST* be located at a 64-bit aligned boundary.
  //
  Trd->Int    = UFS_INTERRUPT_COMMAND;
  Trd->Dd     = 0x00;
  Trd->Ct     = UFS_STORAGE_COMMAND_TYPE;
  Trd->Ocs    = UFS_HC_TRD_OCS_INIT_VALUE;
  Trd->UcdBa  = (UINT32)RShiftU64 ((UINT64) (UINTN)NopOutUpiu, 7);
  Trd->UcdBaU = (UINT32)RShiftU64 ((UINT64) (UINTN)NopOutUpiu, 32);
  Trd->RuL    = (UINT16)DivU64x32 ((UINT64)ROUNDUP8 (sizeof (UTP_NOP_IN_UPIU)), sizeof (UINT32));
  Trd->RuO    = (UINT16)DivU64x32 ((UINT64)ROUNDUP8 (sizeof (UTP_NOP_OUT_UPIU)), sizeof (UINT32));

  return EFI_SUCCESS;
}

/**
  Find out available slot in transfer list of a UFS device.

  @param[in]  Private       The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[out] Slot          The available slot.

  @retval EFI_SUCCESS       The available slot was found successfully.

**/
EFI_STATUS
UfsFindAvailableSlotInTrl (
  IN     UFS_PEIM_HC_PRIVATE_DATA     *Private,
  OUT UINT8                        *Slot
  )
{
  ASSERT ((Private != NULL) && (Slot != NULL));

  UINT8            Nutrs;
  UINT8            Index;
  UINT32           Data;
  UINTN            Address;

  ASSERT ((Private != NULL) && (Slot != NULL));

  Address = Private->UfsHcBase + UFS_HC_UTRLDBR_OFFSET;
  Data    = MmioRead32 (Address);

  Nutrs   = (UINT8)((Private->Capabilities & UFS_HC_CAP_NUTRS) + 1);

  for (Index = 0; Index < Nutrs; Index++) {
    if ((Data & (BIT0 << Index)) == 0) {
      *Slot = Index;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_READY;
}



/**
  Start specified slot in transfer list of a UFS device.

  @param[in]  Private       The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  Slot          The slot to be started.

**/
VOID
UfsStartExecCmd (
  IN  UFS_PEIM_HC_PRIVATE_DATA     *Private,
  IN  UINT8                        Slot
  )
{
  UINTN         UfsHcBase;
  UINTN         Address;
  UINT32        Data;

  UfsHcBase = Private->UfsHcBase;

  Address = UfsHcBase + UFS_HC_UTRLRSR_OFFSET;
  Data    = MmioRead32 (Address);
  if ((Data & UFS_HC_UTRLRSR) != UFS_HC_UTRLRSR) {
    MmioWrite32 (Address, UFS_HC_UTRLRSR);
  }

  Address = UfsHcBase + UFS_HC_UTRLDBR_OFFSET;
  MmioWrite32 (Address, BIT0 << Slot);
}

/**
  Stop specified slot in transfer list of a UFS device.

  @param[in]  Private       The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  Slot          The slot to be stop.

**/
VOID
UfsStopExecCmd (
  IN  UFS_PEIM_HC_PRIVATE_DATA     *Private,
  IN  UINT8                        Slot
  )
{
  UINTN         UfsHcBase;
  UINTN         Address;
  UINT32        Data;

  UfsHcBase = Private->UfsHcBase;

  Address = UfsHcBase + UFS_HC_UTRLDBR_OFFSET;
  Data    = MmioRead32 (Address);
  if ((Data & (BIT0 << Slot)) != 0) {
    Address = UfsHcBase + UFS_HC_UTRLCLR_OFFSET;
    Data    = MmioRead32 (Address);
    MmioWrite32 (Address, (Data & ~ (BIT0 << Slot)));
  }
}

/**
  Extracts return data from query response upiu.

  @param[in] Packet     Pointer to the UFS_DEVICE_MANAGEMENT_REQUEST_PACKET.
  @param[in] QueryResp  Pointer to the query response.

  @retval EFI_INVALID_PARAMETER Packet or QueryResp are empty or opcode is invalid.
  @retval EFI_DEVICE_ERROR      Data returned from device is invalid.
  @retval EFI_SUCCESS           Data extracted.

**/
EFI_STATUS
UfsGetReturnDataFromQueryResponse (
  IN UFS_DEVICE_MANAGEMENT_REQUEST_PACKET  *Packet,
  IN UTP_QUERY_RESP_UPIU                   *QueryResp
  )
{
  UINT16  ReturnDataSize;
  UINT32  ReturnData;

  if (Packet == NULL || QueryResp == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  switch (Packet->Opcode) {
    case UtpQueryFuncOpcodeRdDesc:
      ReturnDataSize = QueryResp->Tsf.Length;
      SwapLittleEndianToBigEndian ((UINT8*)&ReturnDataSize, sizeof (UINT16));
      //
      // Make sure the hardware device does not return more data than expected.
      //
      if (ReturnDataSize > Packet->TransferLength) {
        return EFI_DEVICE_ERROR;
      }

      CopyMem (Packet->DataBuffer, (QueryResp + 1), ReturnDataSize);
      Packet->TransferLength = ReturnDataSize;
      break;
    case UtpQueryFuncOpcodeWrDesc:
      ReturnDataSize = QueryResp->Tsf.Length;
      SwapLittleEndianToBigEndian ((UINT8*)&ReturnDataSize, sizeof (UINT16));
      Packet->TransferLength = ReturnDataSize;
      break;
    case UtpQueryFuncOpcodeRdFlag:
    case UtpQueryFuncOpcodeSetFlag:
    case UtpQueryFuncOpcodeClrFlag:
    case UtpQueryFuncOpcodeTogFlag:
      //
      // The 'FLAG VALUE' field is at byte offset 3 of QueryResp->Tsf.Value
      //
      *((UINT8*)(Packet->DataBuffer)) = *((UINT8*)&(QueryResp->Tsf.Value) + 3);
      break;
    case UtpQueryFuncOpcodeRdAttr:
    case UtpQueryFuncOpcodeWrAttr:
      ReturnData = QueryResp->Tsf.Value;
      SwapLittleEndianToBigEndian ((UINT8*) &ReturnData, sizeof (UINT32));
      CopyMem (Packet->DataBuffer, &ReturnData, sizeof (UINT32));
      break;
    default:
      return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Creates Transfer Request descriptor and sends Query Request to the device.

  @param[in] Private Pointer to the UFS_PEIM_HC_PRIVATE_DATA.
  @param[in] Packet  Pointer to the UFS_DEVICE_MANAGEMENT_REQUEST_PACKET.

  @retval EFI_SUCCESS           The device descriptor was read/written successfully.
  @retval EFI_INVALID_PARAMETER The DescId, Index and Selector fields in Packet are invalid
                                combination to point to a type of UFS device descriptor.
  @retval EFI_DEVICE_ERROR      A device error occurred while attempting to r/w the device descriptor.
  @retval EFI_TIMEOUT           A timeout occurred while waiting for the completion of r/w the device descriptor.

**/
EFI_STATUS
UfsSendDmRequestRetry (
  IN UFS_PEIM_HC_PRIVATE_DATA              *Private,
  IN UFS_DEVICE_MANAGEMENT_REQUEST_PACKET  *Packet
  )
{
  UINT8                               Slot;
  UTP_TRD                             *Trd;
  UINT32                              CmdDescSize;
  UTP_QUERY_RESP_UPIU                 *QueryResp;
  EFI_STATUS                          Status;
  UINT8                               *CmdDescBase;
  UINTN                               Address;

  //
  // Find out which slot of transfer request list is available.
  //
  Status = UfsFindAvailableSlotInTrl (Private, &Slot);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Trd = ((UTP_TRD*)Private->UtpTrlBase) + Slot;
  //
  // Fill transfer request descriptor to this slot.
  //
  Status = UfsCreateDMCommandDesc (Private, Packet, Trd);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to create DM command descriptor\n"));
    return Status;
  }

  //
  // Check the transfer request result.
  //
  CmdDescBase = (UINT8 *) (UINTN) (LShiftU64 ((UINT64)Trd->UcdBaU, 32) | LShiftU64 ((UINT64)Trd->UcdBa, 7));
  QueryResp   = (UTP_QUERY_RESP_UPIU *) (CmdDescBase + Trd->RuO * sizeof (UINT32));
  ASSERT (QueryResp != NULL);
  CmdDescSize = Trd->RuO * sizeof (UINT32) + Trd->RuL * sizeof (UINT32);

  //
  // Start to execute the transfer request.
  //
  UfsStartExecCmd (Private, Slot);

  //
  // Wait for the completion of the transfer request.
  //
  Address = Private->UfsHcBase + UFS_HC_UTRLDBR_OFFSET;
  Status = UfsWaitMemSet (Address, BIT0, 0, Packet->Timeout);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  if (Trd->Ocs != 0 || QueryResp->QueryResp != UfsUtpQueryResponseSuccess) {
    DEBUG ((DEBUG_ERROR, "Failed to send query request, OCS = %X, QueryResp = %X\n", Trd->Ocs, QueryResp->QueryResp));
    DumpQueryResponseResult (QueryResp->QueryResp);

    if ((QueryResp->QueryResp == UfsUtpQueryResponseInvalidSelector) ||
        (QueryResp->QueryResp == UfsUtpQueryResponseInvalidIndex) ||
        (QueryResp->QueryResp == UfsUtpQueryResponseInvalidIdn)) {
      Status = EFI_INVALID_PARAMETER;
    } else {
      Status = EFI_DEVICE_ERROR;
    }
    goto Exit;
  }

  Status = UfsGetReturnDataFromQueryResponse (Packet, QueryResp);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to get return data from query response\n"));
    goto Exit;
  }

Exit:
  UfsStopExecCmd (Private, Slot);
  UfsFreeMem (Private->Pool, CmdDescBase, CmdDescSize);

  return Status;
}

/**
  Sends Query Request to the device. Query is sent until device responds correctly or counter runs out.

  @param[in] Private Pointer to the UFS_PEIM_HC_PRIVATE_DATA.
  @param[in] Packet  Pointer to the UFS_DEVICE_MANAGEMENT_PACKET.

  @retval EFI_SUCCESS           The device responded correctly to the Query request.
  @retval EFI_INVALID_PARAMETER The DescId, Index and Selector fields in Packet are invalid
                                combination to point to a type of UFS device descriptor.
  @retval EFI_DEVICE_ERROR      A device error occurred while waiting for the response from the device.
  @retval EFI_TIMEOUT           A timeout occurred while waiting for the completion of the operation.

**/
EFI_STATUS
UfsSendDmRequest (
  IN UFS_PEIM_HC_PRIVATE_DATA              *Private,
  IN UFS_DEVICE_MANAGEMENT_REQUEST_PACKET  *Packet
  )
{
  EFI_STATUS  Status;
  UINT8       Retry;

  Status = EFI_SUCCESS;

  for (Retry = 0; Retry < 5; Retry ++) {
    Status = UfsSendDmRequestRetry (Private, Packet);
    if (!EFI_ERROR (Status)) {
      return EFI_SUCCESS;
    }
  }

  DEBUG ((DEBUG_ERROR, "Failed to get response from the device after %d retries\n", Retry));
  return Status;
}

/**
  Read or write specified device descriptor of a UFS device.

  @param[in]      Private       The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]      Read          The boolean variable to show r/w direction.
  @param[in]      DescId        The ID of device descriptor.
  @param[in]      Index         The Index of device descriptor.
  @param[in]      Selector      The Selector of device descriptor.
  @param[in, out] Descriptor    The buffer of device descriptor to be read or written.
  @param[in]      DescSize      The size of device descriptor buffer.

  @retval EFI_SUCCESS           The device descriptor was read/written successfully.
  @retval EFI_DEVICE_ERROR      A device error occurred while attempting to r/w the device descriptor.
  @retval EFI_TIMEOUT           A timeout occurred while waiting for the completion of r/w the device descriptor.

**/
EFI_STATUS
UfsRwDeviceDesc (
  IN     UFS_PEIM_HC_PRIVATE_DATA     *Private,
  IN     BOOLEAN                      Read,
  IN     UINT8                        DescId,
  IN     UINT8                        Index,
  IN     UINT8                        Selector,
  IN OUT VOID                         *Descriptor,
  IN     UINT32                       *DescSize
  )
{
  EFI_STATUS                           Status;
  UFS_DEVICE_MANAGEMENT_REQUEST_PACKET Packet;

  if (DescSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&Packet, sizeof (UFS_DEVICE_MANAGEMENT_REQUEST_PACKET));

  if (Read) {
    Packet.DataDirection     = UfsDataIn;
    Packet.Opcode            = UtpQueryFuncOpcodeRdDesc;
  } else {
    Packet.DataDirection     = UfsDataOut;
    Packet.Opcode            = UtpQueryFuncOpcodeWrDesc;
  }
  Packet.DataBuffer          = Descriptor;
  Packet.TransferLength      = *DescSize;
  Packet.DescId              = DescId;
  Packet.Index               = Index;
  Packet.Selector            = Selector;
  Packet.Timeout             = UFS_TIMEOUT;

  Status = UfsSendDmRequest (Private, &Packet);
  if (EFI_ERROR (Status)) {
    *DescSize = 0;
  } else {
    *DescSize = Packet.TransferLength;
  }

  return Status;
}

/**
  Read or write specified attribute of a UFS device.

  @param[in]      Private       The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]      Read          The boolean variable to show r/w direction.
  @param[in]      AttrId        The ID of Attribute.
  @param[in]      Index         The Index of Attribute.
  @param[in]      Selector      The Selector of Attribute.
  @param[in, out] Attributes    The value of Attribute to be read or written.

  @retval EFI_SUCCESS           The Attribute was read/written successfully.
  @retval EFI_INVALID_PARAMETER AttrId, Index and Selector are invalid combination to point to a
                                type of UFS device descriptor.
  @retval EFI_DEVICE_ERROR      A device error occurred while attempting to r/w the Attribute.
  @retval EFI_TIMEOUT           A timeout occurred while waiting for the completion of r/w the Attribute.

**/
EFI_STATUS
UfsRwAttributes (
  IN     UFS_PEIM_HC_PRIVATE_DATA  *Private,
  IN     BOOLEAN                     Read,
  IN     UINT8                       AttrId,
  IN     UINT8                       Index,
  IN     UINT8                       Selector,
  IN OUT UINT32                      *Attributes
  )
{
  UFS_DEVICE_MANAGEMENT_REQUEST_PACKET  Packet;

  ZeroMem (&Packet, sizeof (UFS_DEVICE_MANAGEMENT_REQUEST_PACKET));

  if (Read) {
    Packet.DataDirection = UfsDataIn;
    Packet.Opcode        = UtpQueryFuncOpcodeRdAttr;
  } else {
    Packet.DataDirection = UfsDataOut;
    Packet.Opcode        = UtpQueryFuncOpcodeWrAttr;
  }

  Packet.DataBuffer = Attributes;
  Packet.DescId     = AttrId;
  Packet.Index      = Index;
  Packet.Selector   = Selector;
  Packet.Timeout    = UFS_TIMEOUT;

  return UfsSendDmRequest (Private, &Packet);
}

/**
  Read or write specified flag of a UFS device.

  @param[in]      Private       The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]      Read          The boolean variable to show r/w direction.
  @param[in]      FlagId        The ID of flag to be read or written.
  @param[in, out] Value         The value to set or clear flag.

  @retval EFI_SUCCESS           The flag was read/written successfully.
  @retval EFI_DEVICE_ERROR      A device error occurred while attempting to r/w the flag.
  @retval EFI_TIMEOUT           A timeout occurred while waiting for the completion of r/w the flag.

**/
EFI_STATUS
UfsRwFlags (
  IN     UFS_PEIM_HC_PRIVATE_DATA     *Private,
  IN     BOOLEAN                      Read,
  IN     UINT8                        FlagId,
  IN OUT UINT8                        *Value
  )
{
  UFS_DEVICE_MANAGEMENT_REQUEST_PACKET Packet;

  if (Value == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&Packet, sizeof (UFS_DEVICE_MANAGEMENT_REQUEST_PACKET));

  if (Read) {
    ASSERT (Value != NULL);
    Packet.DataDirection     = UfsDataIn;
    Packet.Opcode            = UtpQueryFuncOpcodeRdFlag;
  } else {
    Packet.DataDirection     = UfsDataOut;
    if (*Value == 1) {
      Packet.Opcode          = UtpQueryFuncOpcodeSetFlag;
    } else if (*Value == 0) {
      Packet.Opcode          = UtpQueryFuncOpcodeClrFlag;
    } else {
      return EFI_INVALID_PARAMETER;
    }
  }
  Packet.DataBuffer          = Value;
  Packet.DescId              = FlagId;
  Packet.Index               = 0;
  Packet.Selector            = 0;
  Packet.Timeout             = UFS_TIMEOUT;

  return UfsSendDmRequest (Private, &Packet);
}

/**
  Set specified flag to 1 on a UFS device.

  @param[in]  Private           The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  FlagId            The ID of flag to be set.

  @retval EFI_SUCCESS           The flag was set successfully.
  @retval EFI_DEVICE_ERROR      A device error occurred while attempting to set the flag.
  @retval EFI_TIMEOUT           A timeout occurred while waiting for the completion of setting the flag.

**/
EFI_STATUS
UfsSetFlag (
  IN  UFS_PEIM_HC_PRIVATE_DATA     *Private,
  IN  UINT8                        FlagId
  )
{
  EFI_STATUS             Status;
  UINT8                  Value;

  Value  = 1;
  Status = UfsRwFlags (Private, FALSE, FlagId, &Value);

  return Status;
}



/**
  Sends NOP IN cmd to a UFS device for initialization process request.
  For more details, please refer to UFS 2.0 spec Figure 13.3.

  @param[in]  Private           The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS           The NOP IN command was sent by the host. The NOP OUT response was
                                received successfully.
  @retval EFI_DEVICE_ERROR      A device error occurred while attempting to execute NOP IN command.
  @retval EFI_OUT_OF_RESOURCES  The resource for transfer is not available.
  @retval EFI_TIMEOUT           A timeout occurred while waiting for the NOP IN command to execute.

**/
EFI_STATUS
UfsExecNopCmds (
  IN  UFS_PEIM_HC_PRIVATE_DATA         *Private
  )
{
  EFI_STATUS                           Status;
  UINT8                                Slot;
  UTP_TRD                              *Trd;
  UTP_NOP_IN_UPIU                      *NopInUpiu;
  UINT8                                *CmdDescBase;
  UINT32                               CmdDescSize;
  UINTN                                Address;

  //
  // Find out which slot of transfer request list is available.
  //
  Status = UfsFindAvailableSlotInTrl (Private, &Slot);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Trd    = ((UTP_TRD *)Private->UtpTrlBase) + Slot;
  Status = UfsCreateNopCommandDesc (Private, Trd);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Check the transfer request result.
  //
  CmdDescBase = (UINT8 *) (UINTN) (LShiftU64 ((UINT64)Trd->UcdBaU, 32) | LShiftU64 ((UINT64)Trd->UcdBa, 7));
  NopInUpiu   = (UTP_NOP_IN_UPIU *) (CmdDescBase + Trd->RuO * sizeof (UINT32));
  ASSERT (NopInUpiu != NULL);
  CmdDescSize = Trd->RuO * sizeof (UINT32) + Trd->RuL * sizeof (UINT32);

  //
  // Start to execute the transfer request.
  //
  UfsStartExecCmd (Private, Slot);

  //
  // Wait for the completion of the transfer request.
  //
  Address = Private->UfsHcBase + UFS_HC_UTRLDBR_OFFSET;
  Status = UfsWaitMemSet (Address, BIT0 << Slot, 0, UFS_TIMEOUT);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  if (NopInUpiu->Resp != 0) {
    Status = EFI_DEVICE_ERROR;
  } else {
    Status = EFI_SUCCESS;
  }

Exit:
  UfsStopExecCmd (Private, Slot);
  UfsFreeMem (Private->Pool, CmdDescBase, CmdDescSize);

  return Status;
}

/**
  Sends a UFS-supported SCSI Request Packet to a UFS device that is attached to the UFS host controller.

  @param[in]      Private       The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]      Lun           The LUN of the UFS device to send the SCSI Request Packet.
  @param[in, out] Packet        A pointer to the SCSI Request Packet to send to a specified Lun of the
                                UFS device.

  @retval EFI_SUCCESS           The SCSI Request Packet was sent by the host. For bi-directional
                                commands, InTransferLength bytes were transferred from
                                InDataBuffer. For write and bi-directional commands,
                                OutTransferLength bytes were transferred by
                                OutDataBuffer.
  @retval EFI_DEVICE_ERROR      A device error occurred while attempting to send the SCSI Request
                                Packet.
  @retval EFI_OUT_OF_RESOURCES  The resource for transfer is not available.
  @retval EFI_TIMEOUT           A timeout occurred while waiting for the SCSI Request Packet to execute.

**/
EFI_STATUS
EFIAPI
UfsExecScsiCmds (
  IN     UFS_PEIM_HC_PRIVATE_DATA      *Private,
  IN     UINT8                         Lun,
  IN OUT UFS_SCSI_REQUEST_PACKET       *Packet
  )
{
  EFI_STATUS                           Status;
  UINT8                                Slot;
  UTP_TRD                              *Trd;
  UINTN                                Address;
  UINT8                                *CmdDescBase;
  UINT32                               CmdDescSize;
  UTP_RESPONSE_UPIU                    *Response;
  UINT16                               SenseDataLen;
  UINT32                               ResTranCount;
  VOID                                 *PacketBufferMap;

  //
  // Find out which slot of transfer request list is available.
  //
  Status = UfsFindAvailableSlotInTrl (Private, &Slot);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Trd = ((UTP_TRD *)Private->UtpTrlBase) + Slot;
  PacketBufferMap = NULL;

  //
  // Fill transfer request descriptor to this slot.
  //
  Status = UfsCreateScsiCommandDesc (Private, Lun, Packet, Trd, &PacketBufferMap);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CmdDescBase = (UINT8 *) (UINTN) (LShiftU64 ((UINT64)Trd->UcdBaU, 32) | LShiftU64 ((UINT64)Trd->UcdBa, 7));
  CmdDescSize = Trd->PrdtO * sizeof (UINT32) + Trd->PrdtL * sizeof (UTP_TR_PRD);

  //
  // Start to execute the transfer request.
  //
  UfsStartExecCmd (Private, Slot);

  //
  // Wait for the completion of the transfer request.
  //
  Address = Private->UfsHcBase + UFS_HC_UTRLDBR_OFFSET;
  Status = UfsWaitMemSet (Address, BIT0 << Slot, 0, Packet->Timeout);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  //
  // Get sense data if exists
  //
  Response     = (UTP_RESPONSE_UPIU *) (CmdDescBase + Trd->RuO * sizeof (UINT32));
  SenseDataLen = Response->SenseDataLen;
  SwapLittleEndianToBigEndian ((UINT8 *)&SenseDataLen, sizeof (UINT16));

  if ((Packet->SenseDataLength != 0) && (Packet->SenseData != NULL)) {
    //
    // Make sure the hardware device does not return more data than expected.
    //
    if (SenseDataLen <= Packet->SenseDataLength) {
      CopyMem (Packet->SenseData, Response->SenseData, SenseDataLen);
      Packet->SenseDataLength = (UINT8)SenseDataLen;
    } else {
      Packet->SenseDataLength = 0;
    }
  }

  //
  // Check the transfer request result.
  //
  if (Response->Response != 0) {
    DEBUG ((DEBUG_ERROR, "UfsExecScsiCmds() fails with Target Failure\n"));
    Status = EFI_DEVICE_ERROR;
    goto Exit;
  }

  if (Trd->Ocs == 0) {
    if (Packet->DataDirection == UfsDataIn) {
      if ((Response->Flags & BIT5) == BIT5) {
        ResTranCount = Response->ResTranCount;
        SwapLittleEndianToBigEndian ((UINT8 *)&ResTranCount, sizeof (UINT32));
        Packet->InTransferLength -= ResTranCount;
      }
    } else if (Packet->DataDirection == UfsDataOut) {
      if ((Response->Flags & BIT5) == BIT5) {
        ResTranCount = Response->ResTranCount;
        SwapLittleEndianToBigEndian ((UINT8 *)&ResTranCount, sizeof (UINT32));
        Packet->OutTransferLength -= ResTranCount;
      }
    }
  } else {
    Status = EFI_DEVICE_ERROR;
  }

Exit:
  if (PacketBufferMap != NULL) {
    IoMmuUnmap (PacketBufferMap);
  }
  UfsStopExecCmd (Private, Slot);
  UfsFreeMem (Private->Pool, CmdDescBase, CmdDescSize);

  return Status;
}


/**
  Fill UIC Command associated fields.

  @param[in] UicOpcode        The opcode of the UIC command.
  @param[in] Arg1Mib          The value for MIB Attribute of UIC command 1st argument.
  @param[in] Arg1GenSel       The value for Gen Selector Index of UIC command 1st argument.
  @param[in] Arg2             The value for UIC command 2nd argument.
  @param[in] Arg3             The value for UIC command 3rd argument.

**/
VOID
UfsFillUicCommand (
  IN     UINT8             UicOpcode,
  IN     UINT16            Arg1Mib,
  IN     UINT16            Arg1GenSel,
  IN     UINT32            Arg2,
  IN     UINT32            Arg3,
  IN OUT UFS_COMMAND       *Cmd
  )
{
  Cmd->UicOpcode = UicOpcode;
  Cmd->Arg1      = UFS_UIC_ARG_MIB_SEL (Arg1Mib, Arg1GenSel);
  Cmd->Arg2      = Arg2;
  Cmd->Arg3      = Arg3;
}

/**
  Send UIC command.

  @param[in]      Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in, out] UicCommand              UIC command descriptor. On exit contains UIC command results.

  @return EFI_SUCCESS                     Successfully execute this UIC command and detect attached UFS device.
  @return EFI_DEVICE_ERROR                Fail to execute this UIC command and detect attached UFS device.

**/
EFI_STATUS
UfsExecUicCommands (
  IN  UFS_PEIM_HC_PRIVATE_DATA      *Private,
  IN OUT  UFS_COMMAND               *UicCommand
  )
{
  EFI_STATUS  Status;
  UINTN       Address;
  UINT32      Data;
  UINTN       UfsHcBase;

  UfsHcBase = Private->UfsHcBase;
  Address   = UfsHcBase + UFS_HC_IS_OFFSET;
  Data      = MmioRead32 (Address);
  if ((Data & UFS_HC_IS_UCCS) == UFS_HC_IS_UCCS) {
    //
    // Clear IS.BIT10 UIC Command Completion Status (UCCS) at first.
    //
    MmioWrite32 (Address, Data);
  }

  //
  // When programming UIC command registers, host software shall set the register UICCMD
  // only after all the UIC command argument registers (UICCMDARG1, UICCMDARG2 and UICCMDARG3)
  // are set.
  //
  Address = UfsHcBase + UFS_HC_UCMD_ARG1_OFFSET;
  MmioWrite32 (Address, UicCommand->Arg1);

  Address = UfsHcBase + UFS_HC_UCMD_ARG2_OFFSET;
  MmioWrite32 (Address, UicCommand->Arg2);

  Address = UfsHcBase + UFS_HC_UCMD_ARG3_OFFSET;
  MmioWrite32 (Address, UicCommand->Arg3);

  //
  // Host software shall only set the UICCMD if HCS.UCRDY is set to 1.
  //
  Address = Private->UfsHcBase + UFS_HC_STATUS_OFFSET;
  Status = UfsWaitMemSet (Address, UFS_HC_HCS_UCRDY, UFS_HC_HCS_UCRDY, UFS_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Address = UfsHcBase + UFS_HC_UIC_CMD_OFFSET;
  MmioWrite32 (Address, (UINT32)UicCommand->UicOpcode);

  //
  // UFS 2.0 spec section 5.3.1 Offset:0x20 IS.Bit10 UIC Command Completion Status (UCCS)
  // This bit is set to '1' by the host controller upon completion of a UIC command.
  //
  Address = UfsHcBase + UFS_HC_IS_OFFSET;
  Status  = UfsWaitMemSet (Address, UFS_HC_IS_UCCS, UFS_HC_IS_UCCS, UFS_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (UicCommand->UicOpcode != UfsUicDmeReset) {
    // Arg 2
    Address = UfsHcBase + UFS_HC_UCMD_ARG2_OFFSET;
    UicCommand->Arg2    = MmioRead32 (Address);
    // Arg 3
    Address = UfsHcBase + UFS_HC_UCMD_ARG3_OFFSET;
    UicCommand->Arg3    = MmioRead32 (Address);
    if ((UicCommand->Arg2 & 0xFF) != 0) {
      DEBUG_CODE_BEGIN();
      DumpUicCmdExecResult ((UINT8)UicCommand->UicOpcode, (UINT8) (UicCommand->Arg2 & 0xFF));
      DEBUG_CODE_END();
      return EFI_DEVICE_ERROR;
    }
  }

  return EFI_SUCCESS;
}

/**
  Applies platform specific programming after the driver
  has enabled the host controller.

  @param[in] Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

**/
VOID
UfsHcPlatformPostHce (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private
  )
{
  UFS_COMMAND            LccDisableCommand;

  //
  // For all supported integrated controllers we have to disable the
  // line configuration check(LCC).
  //
  UfsFillUicCommand (UfsUicDmeSet, PA_Local_TX_LCC_Enable, 0, 0, 0, &LccDisableCommand );
  UfsExecUicCommands (Private, &LccDisableCommand);
}

/**
  Enable the UFS host controller for accessing.

  @param[in] Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS                The UFS host controller enabling was executed successfully.
  @retval EFI_DEVICE_ERROR           A device error occurred while enabling the UFS host controller.

**/
EFI_STATUS
UfsEnableHostController (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private
  )
{
  EFI_STATUS             Status;
  UINTN                  Address;
  UINT32                 Data;

  //
  // UFS 2.0 spec section 7.1.1 - Host Controller Initialization
  //
  // Reinitialize the UFS host controller if HCE bit of HC register is set.
  //
  Address = Private->UfsHcBase + UFS_HC_ENABLE_OFFSET;
  Data    = MmioRead32 (Address);
  if ((Data & UFS_HC_HCE_EN) == UFS_HC_HCE_EN) {
    //
    // Write a 0 to the HCE register at first to disable the host controller.
    //
    MmioWrite32 (Address, 0);
    //
    // Wait until HCE is read as '0' before continuing.
    //
    Status = UfsWaitMemSet (Address, UFS_HC_HCE_EN, 0, UFS_TIMEOUT);
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
  }

  //
  // Write a 1 to the HCE register to enable the UFS host controller.
  //
  MmioWrite32 (Address, UFS_HC_HCE_EN);
  //
  // Wait until HCE is read as '1' before continuing.
  //
  Status = UfsWaitMemSet (Address, UFS_HC_HCE_EN, UFS_HC_HCE_EN, UFS_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Apply platform specific programming after the driver has enabled
  // the host controller.
  //
  UfsHcPlatformPostHce(Private);
  return EFI_SUCCESS;
}

/**
  Detects the connected Ufs Lanes and Programs the Active Lanes Attribute.

  @param[in] Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS                Successfully Programmed the lanes
  @retval others                     Failed to program the lanes
**/
EFI_STATUS
UfsActivateAllLanes (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private
  )
{
  EFI_STATUS         Status;
  UINT32             ConnectedLanes = 0;
  UINT32             ActiveLanes = 0;
  UFS_COMMAND        UicCommand;

  //Step 1: Query connected and Active Rx lanes and make all connected Rx lanes Active.
  UfsFillUicCommand (UfsUicDmeGet, PA_ConnectedRxDataLanes, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_ConnectedRxDataLanes(%x) \n", PA_ConnectedRxDataLanes));
    return Status;
  }
  ConnectedLanes = UicCommand.Arg3;

  UfsFillUicCommand (UfsUicDmeGet, PA_ActiveRxDataLanes, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_ActiveRxDataLanes(%x) \n", PA_ActiveRxDataLanes));
    return Status;
  }
  ActiveLanes = UicCommand.Arg3;

  if (ActiveLanes < ConnectedLanes) {
    DEBUG ((DEBUG_INFO, "UfsActivateAllLanes : PA_ActiveRxDataLanes = %x  PA_ConnectedRxDataLanes = %x\n", ActiveLanes, ConnectedLanes));
    UfsFillUicCommand (UfsUicDmeSet, PA_ActiveRxDataLanes, 0, 0, ConnectedLanes, &UicCommand );
    Status = UfsExecUicCommands (Private, &UicCommand);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to set active Rx data lanes\n"));
      return Status;
    }
  }

  //Step 2: Query connected and Active Tx lanes and make all connected Tx lanes Active.
  UfsFillUicCommand (UfsUicDmeGet, PA_ConnectedTxDataLanes, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_ConnectedTxDataLanes(%x)\n", PA_ConnectedTxDataLanes));
    return Status;
  }
  ConnectedLanes = UicCommand.Arg3;

  UfsFillUicCommand (UfsUicDmeGet, PA_ActiveTxDataLanes, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_ActiveTxDataLanes(%x) \n", PA_ActiveTxDataLanes));
    return Status;
  }
  ActiveLanes = UicCommand.Arg3;

  if (ActiveLanes < ConnectedLanes) {
    DEBUG ((DEBUG_INFO, "UfsActivateAllLanes : PA_ActiveTxDataLanes = %x  PA_ConnectedTxDataLanes = %x\n", ActiveLanes, ConnectedLanes));
    UfsFillUicCommand (UfsUicDmeSet, PA_ActiveTxDataLanes, 0, 0, ConnectedLanes, &UicCommand );
    Status = UfsExecUicCommands (Private, &UicCommand);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to set active Tx data lanes\n"));
      return Status;
    }
  }

  return Status;
}

/**
  Switches the link to highest supported PWM Gear.

  @param[in] Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS                Successfully switched to PWM Gear
  @retval others                     Failed to switch to PWM Gear
**/
EFI_STATUS
UfsPwmGearSwitch (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private
  )
{
  EFI_STATUS         Status;
  UINT32             MaxPwmGear[] = {PWM_G1, PWM_G1};
  UFS_COMMAND        UicCommand;
  UINTN              Address;

  Status = UfsActivateAllLanes (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UfsPwmGearSwitch: Failed to activate connected UFS Lanes \n"));
    return Status;
  }

  UfsFillUicCommand (UfsUicDmeGet, PA_MaxRxPWMGear, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UfsPwmGearSwitch: Failed to Get UfsUicDmeGet PA_MaxRxPWMGear(%x) \n", PA_MaxRxPWMGear));
    return Status;
  }
  MaxPwmGear[UfsRxLane] = UicCommand.Arg3;

  UfsFillUicCommand (UfsUicDmePeerGet, PA_MaxRxPWMGear, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UfsPwmGearSwitch: Failed to Get UfsUicDmePeerGet PA_MaxRxPWMGear(%x) \n", PA_MaxRxPWMGear));
    return Status;
  }
  MaxPwmGear[UfsTxLane] = UicCommand.Arg3;

  UfsFillUicCommand (UfsUicDmeSet, PA_RXGear, 0, 0, MaxPwmGear[UfsRxLane], &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UfsPwmGearSwitch: Failed to set UfsUicDmeSet PA_RXGear \n"));
    return Status;
  }

  UfsFillUicCommand (UfsUicDmeSet, PA_TXGear, 0, 0, MaxPwmGear[UfsTxLane], &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UfsPwmGearSwitch: Failed to set UfsUicDmeSet PA_TXGear \n"));
    return Status;
  }

  //
  // Change Power Mode of both directions
  //
  UfsFillUicCommand (UfsUicDmeSet, PA_PWRMode, 0, 0, (((SlowAuto_Mode & 0xF) << 4) | (SlowAuto_Mode & 0xF)), &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UfsPwmGearSwitch: Failed to set PA_PWRMode(%x) \n", PA_PWRMode));
    return Status;
  }

  Address = Private->UfsHcBase + UFS_HC_IS_OFFSET;
  Status = UfsWaitMemSet (Address, UFS_HC_IS_UPMS, UFS_HC_IS_UPMS, UFS_TIMEOUT);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UfsPwmGearSwitch: Failed to switch power mode\n"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Programs the recipe required for High speed link Power Mode and Gear switch

  @param[in] Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS                Successfully programmed the recipe
  @retval others                     Failed to program the recipe
**/
EFI_STATUS
UfsHsRecipieProgramming (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private
  )
{
  EFI_STATUS         Status;
  UFS_COMMAND        UicCommand;

  UfsFillUicCommand (UfsUicDmeSet, DL_FC0ProtectionTimeOutVal, 0, 0, 0x1FFF, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to set active FC0 protection time out value\n"));
    return Status;
  }
  UfsFillUicCommand (UfsUicDmeSet, DL_TC0ReplayTimeOutVal, 0, 0, 0xFFFF, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to set active TC0 replay time out value\n"));
    return Status;
  }
  UfsFillUicCommand (UfsUicDmeSet, DL_AFC0ReqTimeOutVal, 0, 0, 0x7FFF, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
   DEBUG ((DEBUG_ERROR, "Failed to set active AFC0 req time out value\n"));
    return Status;
  }
  UfsFillUicCommand (UfsUicDmeSet, PA_HSSeries, 0, 0, UFS_PA_HS_MODE_B, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
  DEBUG ((DEBUG_ERROR, "Failed to enable rate B\n"));
  return Status;
  }
  UfsFillUicCommand (UfsUicDmeSet, PA_RxTermination, 0, 0, 0x1, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to enable Rx termination\n"));
    return Status;
  }
  UfsFillUicCommand (UfsUicDmeSet, PA_TxTermination, 0, 0, 0x1, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to enable Tx termination\n"));
    return Status;
  }
  return Status;
}

/**
  Dumps the link Configuration.
 @param[in] Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS               Successfully switched to fast mode
  @retval others                    Failed to switch to fast mode
**/
EFI_STATUS
UfsDumpLinkConfig (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private
  )
{
  EFI_STATUS         Status;
  UFS_COMMAND        UicCommand;

  DEBUG ((DEBUG_INFO, "UfsDumpLinkConfig Entry \n"));

  UfsFillUicCommand (UfsUicDmeGet, PA_Local_TX_LCC_Enable, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_Local_TX_LCC_Enable(%x)\n", PA_Local_TX_LCC_Enable));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_Local_TX_LCC_Enable(%x) = %x \n", PA_Local_TX_LCC_Enable, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_MaxRxHSGear, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_MaxRxHSGear(%x)\n", PA_MaxRxHSGear));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_MaxRxHSGear(%x) = %x \n", PA_MaxRxHSGear, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmePeerGet, PA_MaxRxHSGear, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmePeerGet PA_MaxRxHSGear(%x)\n", PA_MaxRxHSGear));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmePeerGet PA_MaxRxHSGear(%x) = %x \n", PA_MaxRxHSGear, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_MaxRxPWMGear, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_MaxRxPWMGear(%x)\n", PA_MaxRxPWMGear));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_MaxRxPWMGear(%x) = %x \n", PA_MaxRxPWMGear, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmePeerGet, PA_MaxRxPWMGear, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmePeerGet PA_MaxRxPWMGear(%x)\n", PA_MaxRxPWMGear));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmePeerGet PA_MaxRxPWMGear(%x) = %x \n", PA_MaxRxPWMGear, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_AvailTxDataLanes, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_AvailTxDataLanes(%x)\n", PA_AvailTxDataLanes));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_AvailTxDataLanes(%x) = %x \n", PA_AvailTxDataLanes, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_AvailRxDataLanes, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_AvailRxDataLanes(%x)\n", PA_AvailRxDataLanes));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_AvailRxDataLanes(%x) = %x \n", PA_AvailRxDataLanes, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_ConnectedTxDataLanes, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_ConnectedTxDataLanes(%x)\n", PA_ConnectedTxDataLanes));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_ConnectedTxDataLanes(%x) = %x \n", PA_ConnectedTxDataLanes, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_ConnectedRxDataLanes, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_ConnectedRxDataLanes(%x)\n", PA_ConnectedRxDataLanes));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_ConnectedRxDataLanes(%x) = %x \n", PA_ConnectedRxDataLanes, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_ActiveTxDataLanes, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_ActiveTxDataLanes(%x)\n", PA_ActiveTxDataLanes));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_ActiveTxDataLanes(%x) = %x \n", PA_ActiveTxDataLanes, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_ActiveRxDataLanes, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_ActiveRxDataLanes(%x)\n", PA_ActiveRxDataLanes));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_ActiveRxDataLanes(%x) = %x \n", PA_ActiveRxDataLanes, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_TxPWRStatus, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_TxPWRStatus(%x)\n", PA_TxPWRStatus));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_TxPWRStatus(%x) = %x \n", PA_TxPWRStatus, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_RxPWRStatus, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_RxPWRStatus(%x)\n", PA_RxPWRStatus));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_RxPWRStatus(%x) = %x \n", PA_RxPWRStatus, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, DL_FC0ProtectionTimeOutVal, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet DL_FC0ProtectionTimeOutVal(%x)\n", DL_FC0ProtectionTimeOutVal));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet DL_FC0ProtectionTimeOutVal(%x) = %x \n", DL_FC0ProtectionTimeOutVal, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, DL_TC0ReplayTimeOutVal, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet DL_TC0ReplayTimeOutVal(%x)\n", DL_TC0ReplayTimeOutVal));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet DL_TC0ReplayTimeOutVal(%x) = %x \n", DL_TC0ReplayTimeOutVal, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, DL_AFC0ReqTimeOutVal, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet DL_AFC0ReqTimeOutVal(%x)\n", DL_AFC0ReqTimeOutVal));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet DL_AFC0ReqTimeOutVal(%x) = %x \n", DL_AFC0ReqTimeOutVal, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_TXGear, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_TXGear(%x)\n", PA_TXGear));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_TXGear(%x) = %x \n", PA_TXGear, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_TxTermination, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_TxTermination(%x)\n", PA_TxTermination));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_TxTermination(%x) = %x \n", PA_TxTermination, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_HSSeries, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_HSSeries(%x)\n", PA_HSSeries));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_HSSeries(%x) = %x \n", PA_HSSeries, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_RXGear, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_RXGear(%x)\n", PA_RXGear));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_RXGear(%x) = %x \n", PA_RXGear, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_RxTermination, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_RxTermination(%x)\n", PA_RxTermination));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_RxTermination(%x) = %x \n", PA_RxTermination, UicCommand.Arg3));

  UfsFillUicCommand (UfsUicDmeGet, PA_HSSeries, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_HSSeries(%x)\n", PA_HSSeries));
  }
  DEBUG ((DEBUG_INFO, "UfsUicDmeGet PA_HSSeries(%x) = %x \n", PA_HSSeries, UicCommand.Arg3));

  DEBUG ((DEBUG_INFO, "UfsDumpLinkConfig Exit \n"));
  return EFI_SUCCESS;
}

/**
  Switches the link Power Mode and Gear.

  @param[in] Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS                Successfully switched the Power Mode and Gear
  @retval others                     Failed to switch the Power Mode and Gear
**/
EFI_STATUS
UfsPowerModeAndGearSwitch (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private
  )
{
  EFI_STATUS         Status;
  UINT32             PowerMode[] = {Fast_Mode, Fast_Mode};
  UINT32             MaxHsGear[] = {HS_G1, HS_G1};
  UINT32             CurrentHsGear[] = {NO_HS, NO_HS};
  UINT32             MaxPwmGear[] = {PWM_G1, PWM_G1};
  UFS_COMMAND        UicCommand;
  UINTN              Address;

  DEBUG ((DEBUG_INFO, "UfsPowerModeAndGearSwitch Entry: \n" ));
  //
  //Step 1: Program Lanes
  //
  Status = UfsActivateAllLanes (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Program UFS Lanes \n"));
    return Status;
  }
  //
  //Step 2: Query Max HS Gear and Max PWM Gears and if no HS gear, set PowerMode to SlowAuto
  //
  UfsFillUicCommand (UfsUicDmeGet, PA_MaxRxPWMGear, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_MaxRxPWMGear(%x)\n", PA_MaxRxPWMGear));
    return Status;
  }
  MaxPwmGear[UfsRxLane] = UicCommand.Arg3;

  UfsFillUicCommand (UfsUicDmeGet, PA_MaxRxHSGear, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeGet PA_MaxRxHSGear(%x) \n", PA_MaxRxHSGear));
    return Status;
  }
  MaxHsGear[UfsRxLane] = UicCommand.Arg3;

  UfsFillUicCommand (UfsUicDmePeerGet, PA_MaxRxPWMGear, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmePeerGet PA_MaxRxPWMGear(%x) \n",PA_MaxRxPWMGear));
    return Status;
  }
  MaxPwmGear[UfsTxLane] = UicCommand.Arg3;

  UfsFillUicCommand (UfsUicDmePeerGet, PA_MaxRxHSGear, 0, 0, 0, &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmePeerGet PA_MaxRxHSGear(%x) \n", PA_MaxRxHSGear));
    return Status;
  }
  MaxHsGear[UfsTxLane] = UicCommand.Arg3;

  //
  //Step 3: Mark the Power Mode as SlowAuto_Mode
  //
  if ((NO_HS == MaxHsGear[UfsRxLane]) || (NO_HS == MaxHsGear[UfsTxLane])) {
    PowerMode[UfsRxLane] = SlowAuto_Mode;
    PowerMode[UfsTxLane] = SlowAuto_Mode;
  }

  DEBUG ((DEBUG_INFO, "PowerMode[UfsRxLane] = %x \n", PowerMode[UfsRxLane]));
  DEBUG ((DEBUG_INFO, "PowerMode[UfsTxLane] = %x \n", PowerMode[UfsTxLane]));
  DEBUG ((DEBUG_INFO, "MaxHsGear[UfsRxLane] = %x \n", MaxHsGear[UfsRxLane]));
  DEBUG ((DEBUG_INFO, "MaxHsGear[UfsTxLane] = %x \n", MaxHsGear[UfsTxLane]));
  DEBUG ((DEBUG_INFO, "MaxPwmGear[UfsRxLane] = %x \n", MaxPwmGear[UfsRxLane]));
  DEBUG ((DEBUG_INFO, "MaxPwmGear[UfsTxLane] = %x \n", MaxPwmGear[UfsTxLane]));

  //
  //Step 4: Set Rx gear
  //
  if (PowerMode[UfsRxLane] == SlowAuto_Mode) {
    UfsFillUicCommand (UfsUicDmeSet, PA_RXGear, 0, 0, MaxPwmGear[UfsRxLane], &UicCommand );
    Status = UfsExecUicCommands (Private, &UicCommand);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to set UfsUicDmeSet PA_RXGear \n"));
      return Status;
    }
  } else {
    if (MaxHsGear[UfsRxLane] > HS_G3) {
      MaxHsGear[UfsRxLane] = HS_G3;
      DEBUG ((DEBUG_INFO, "Limited MaxHsGear to %x \n", MaxHsGear[UfsRxLane]));
    }

    UfsFillUicCommand (UfsUicDmeGet, PA_RXGear, 0, 0, 0, &UicCommand );
    Status = UfsExecUicCommands (Private, &UicCommand);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to set UfsUicDmeSet PA_RXGear \n"));
      return Status;
    }
    CurrentHsGear[UfsRxLane] = UicCommand.Arg3;

    if (CurrentHsGear[UfsRxLane] < MaxHsGear[UfsRxLane]) {
      DEBUG ((DEBUG_INFO, "CurrentHsGear[UfsRxLane] = %x switching to MaxHsGear[UfsRxLane]=%x \n", CurrentHsGear[UfsRxLane], MaxHsGear[UfsRxLane]));
      UfsFillUicCommand (UfsUicDmeSet, PA_RXGear, 0, 0, MaxHsGear[UfsRxLane], &UicCommand );
      Status = UfsExecUicCommands (Private, &UicCommand);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Failed to set UfsUicDmeSet PA_RXGear \n"));
        return Status;
      }
    }
  }
  //
  //Step 5: Set Tx gear
  //
  if (PowerMode[UfsTxLane] == SlowAuto_Mode) {
    UfsFillUicCommand (UfsUicDmeSet, PA_TXGear, 0, 0, MaxPwmGear[UfsTxLane], &UicCommand );
    Status = UfsExecUicCommands (Private, &UicCommand);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to set UfsUicDmeSet PA_TXGear \n"));
      return Status;
    }
  } else {
    if (MaxHsGear[UfsTxLane] > HS_G3) {
      MaxHsGear[UfsTxLane] = HS_G3;
      DEBUG ((DEBUG_INFO, "Limited MaxHsGear to %x \n", MaxHsGear[UfsTxLane]));
    }
    UfsFillUicCommand (UfsUicDmeGet, PA_TXGear, 0, 0, 0, &UicCommand );
    Status = UfsExecUicCommands (Private, &UicCommand);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to Get UfsUicDmeSet PA_TXGear \n"));
      return Status;
    }
    CurrentHsGear[UfsTxLane] = UicCommand.Arg3;
    if (CurrentHsGear[UfsTxLane] < MaxHsGear[UfsRxLane]) {
      DEBUG ((DEBUG_INFO, "CurrentHsGear[UfsTxLane] = %x switching to MaxHsGear[UfsTxLane]=%x \n", CurrentHsGear[UfsTxLane], MaxHsGear[UfsTxLane]));
      UfsFillUicCommand (UfsUicDmeSet, PA_TXGear, 0, 0, MaxHsGear[UfsTxLane], &UicCommand );
      Status = UfsExecUicCommands (Private, &UicCommand);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Failed to set UfsUicDmeSet PA_TXGear \n"));
        return Status;
      }
    }
  }
  //
  //Step 6: Program the Intel platform specific recipe required for High speed link Power Mode and Gear switch
  //
  Status = UfsHsRecipieProgramming (Private);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "UfsHsRecipieProgramming Fails, Status = %r\n", Status));
  }
  //
  // Step 7 Change Power Mode of both directions
  //
  UfsFillUicCommand (UfsUicDmeSet, PA_PWRMode, 0, 0, (((PowerMode[UfsRxLane] & 0xF) << 4) | (PowerMode[UfsTxLane] & 0xF)), &UicCommand );
  Status = UfsExecUicCommands (Private, &UicCommand);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to set PA_PWRMode(%x) \n", PA_PWRMode));
    return Status;
  }

  Address = Private->UfsHcBase + UFS_HC_IS_OFFSET;
  Status = UfsWaitMemSet (Address, UFS_HC_IS_UPMS, UFS_HC_IS_UPMS, UFS_TIMEOUT);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to switch power mode\n"));
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((DEBUG_INFO, "Dump LinkConfig After UfsPowerModeAndGearSwitch\n"));
  Status = UfsDumpLinkConfig (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Dump LinkConfig\n"));
  }

  DEBUG ((DEBUG_INFO, "UfsPowerModeAndGearSwitch Exit\n" ));
  return EFI_SUCCESS;
}

/**
  Switches the link Power Mode and Gear.

  @param[in] Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

**/
EFI_STATUS
EFIAPI
UfsHcPlatformPostHceSwitchGear (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private
  )
{
  EFI_STATUS                         Status;

  DEBUG ((DEBUG_INFO, "UfsHcPlatformPostHceSwitchGear Entry\n" ));
  if (Private == NULL) {
    DEBUG((DEBUG_ERROR, "UfsHcPlatformPostHceSwitchGear Private is NULL\n"));
    return EFI_LOAD_ERROR;
  }

  Status = UfsPwmGearSwitch (Private);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "UfsPwmGearSwitch Fails, Status = %r\n", Status));
  }

  Status = UfsPowerModeAndGearSwitch(Private);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "UfsPowerModeAndGearSwitch Fails, Status = %r\n", Status));
  }
  DEBUG ((DEBUG_INFO, "UfsHcPlatformPostHceSwitchGear Exit\n" ));
  return Status;
}

/**
  Detect if a UFS device attached.

  @param[in] Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS                The UFS device detection was executed successfully.
  @retval EFI_NOT_FOUND              Not found a UFS device attached.
  @retval EFI_DEVICE_ERROR           A device error occurred while detecting the UFS device.

**/
EFI_STATUS
UfsDeviceDetection (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private
  )
{
  UINTN              Retry;
  EFI_STATUS         Status;
  UINT32             Data;
  UFS_COMMAND        LinkStartupCommand;
  UINTN              Address;

  //
  // Start UFS device detection.
  // Try up to 3 times for establishing data link with device.
  //
  for (Retry = 0; Retry < 3; Retry++) {
    LinkStartupCommand.UicOpcode = UfsUicDmeLinkStartup;
    LinkStartupCommand.Arg1 = 0;
    LinkStartupCommand.Arg2 = 0;
    LinkStartupCommand.Arg3 = 0;
    Status = UfsExecUicCommands (Private, &LinkStartupCommand);
    if (!EFI_ERROR (Status)) {
      Address = Private->UfsHcBase + UFS_HC_STATUS_OFFSET;
      Data = MmioRead32 (Address);

    if ((Data & UFS_HC_HCS_DP) == 0) {
      Address = Private->UfsHcBase + UFS_HC_IS_OFFSET;
      Status = UfsWaitMemSet (Address, UFS_HC_IS_ULSS, UFS_HC_IS_ULSS, UFS_TIMEOUT);
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }
    }
    break;
    }
    if (Status == EFI_NOT_FOUND) {
      continue;
    }

    return EFI_DEVICE_ERROR;
  }

  if (Retry == 3) {
      return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  Initialize UFS task management request list related h/w context.

  @param[in] Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS                The UFS task management list was initialzed successfully.
  @retval EFI_DEVICE_ERROR           The initialization fails.

**/
EFI_STATUS
UfsInitTaskManagementRequestList (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private
  )
{
  UINTN                  Address;
  UINT32                 Data;
  UINT8                  Nutmrs;
  VOID                   *CmdDescHost;
  EFI_PHYSICAL_ADDRESS   CmdDescPhyAddr;
  VOID                   *CmdDescMapping;
  EFI_STATUS             Status;

  //
  // Initial h/w and s/w context for future operations.
  //
  Address = Private->UfsHcBase + UFS_HC_CAP_OFFSET;
  Data    = MmioRead32 (Address);
  Private->Capabilities = Data;

  //
  // Allocate and initialize UTP Task Management Request List.
  //
  Nutmrs = (UINT8) (RShiftU64 ((Private->Capabilities & UFS_HC_CAP_NUTMRS), 16) + 1);
  Status = IoMmuAllocateBuffer (
             EFI_SIZE_TO_PAGES (Nutmrs * sizeof (UTP_TMRD)),
             &CmdDescHost,
             &CmdDescPhyAddr,
             &CmdDescMapping
             );
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  ZeroMem (CmdDescHost, EFI_PAGES_TO_SIZE (EFI_SIZE_TO_PAGES (Nutmrs * sizeof (UTP_TMRD))));

  //
  // Program the UTP Task Management Request List Base Address and UTP Task Management
  // Request List Base Address with a 64-bit address allocated at step 6.
  //
  Address = Private->UfsHcBase + UFS_HC_UTMRLBA_OFFSET;
  MmioWrite32 (Address, (UINT32)(UINTN)CmdDescPhyAddr);
  Address = Private->UfsHcBase + UFS_HC_UTMRLBAU_OFFSET;
  MmioWrite32 (Address, (UINT32)RShiftU64 ((UINT64)CmdDescPhyAddr, 32));
  Private->UtpTmrlBase = (VOID*)(UINTN)CmdDescHost;
  Private->Nutmrs      = Nutmrs;
  Private->TmrlMapping = CmdDescMapping;

  //
  // Enable the UTP Task Management Request List by setting the UTP Task Management
  // Request List RunStop Register (UTMRLRSR) to '1'.
  //
  Address = Private->UfsHcBase + UFS_HC_UTMRLRSR_OFFSET;
  MmioWrite32 (Address, UFS_HC_UTMRLRSR);

  return EFI_SUCCESS;
}

/**
  Initialize UFS transfer request list related h/w context.

  @param[in] Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS                The UFS transfer list was initialzed successfully.
  @retval EFI_DEVICE_ERROR           The initialization fails.

**/
EFI_STATUS
UfsInitTransferRequestList (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private
  )
{
  UINTN                  Address;
  UINT32                 Data;
  UINT8                  Nutrs;
  VOID                   *CmdDescHost;
  EFI_PHYSICAL_ADDRESS   CmdDescPhyAddr;
  VOID                   *CmdDescMapping;
  EFI_STATUS             Status;

  //
  // Initial h/w and s/w context for future operations.
  //
  Address = Private->UfsHcBase + UFS_HC_CAP_OFFSET;
  Data    = MmioRead32 (Address);
  Private->Capabilities = Data;

  //
  // Allocate and initialize UTP Transfer Request List.
  //
  Nutrs  = (UINT8) ((Private->Capabilities & UFS_HC_CAP_NUTRS) + 1);
  Status = IoMmuAllocateBuffer (
             EFI_SIZE_TO_PAGES (Nutrs * sizeof (UTP_TRD)),
             &CmdDescHost,
             &CmdDescPhyAddr,
             &CmdDescMapping
             );
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  ZeroMem (CmdDescHost, EFI_PAGES_TO_SIZE (EFI_SIZE_TO_PAGES (Nutrs * sizeof (UTP_TRD))));

  //
  // Program the UTP Transfer Request List Base Address and UTP Transfer Request List
  // Base Address with a 64-bit address allocated at step 8.
  //
  Address = Private->UfsHcBase + UFS_HC_UTRLBA_OFFSET;
  MmioWrite32 (Address, (UINT32)(UINTN)CmdDescPhyAddr);
  Address = Private->UfsHcBase + UFS_HC_UTRLBAU_OFFSET;
  MmioWrite32 (Address, (UINT32)RShiftU64 ((UINT64)CmdDescPhyAddr, 32));
  Private->UtpTrlBase = (VOID*)(UINTN)CmdDescHost;
  Private->Nutrs      = Nutrs;
  Private->TrlMapping = CmdDescMapping;

  //
  // Enable the UTP Transfer Request List by setting the UTP Transfer Request List
  // RunStop Register (UTRLRSR) to '1'.
  //
  Address = Private->UfsHcBase + UFS_HC_UTRLRSR_OFFSET;
  MmioWrite32 (Address, UFS_HC_UTRLRSR);

  return EFI_SUCCESS;
}

/**
  Initialize the UFS host controller.

  @param[in] Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS                The Ufs Host Controller is initialized successfully.
  @retval Others                     A device error occurred while initializing the controller.

**/
EFI_STATUS
UfsControllerInit (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private
  )
{
  EFI_STATUS             Status;

  Status = UfsEnableHostController (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "UfsDevicePei: Enable Host Controller Fails, Status = %r\n", Status));
    goto Exit;
  }

  Status = UfsDeviceDetection (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "UfsDevicePei: Device Detection Fails, Status = %r\n", Status));
    goto Exit;
  }

  Status = UfsInitTaskManagementRequestList (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "UfsDevicePei: Task management list initialization Fails, Status = %r\n", Status));
    goto Exit;
  }

  Status = UfsInitTransferRequestList (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UfsDevicePei: Transfer list initialization Fails, Status = %r\n", Status));

    if (Private->TmrlMapping != NULL) {
      IoMmuFreeBuffer (
        EFI_SIZE_TO_PAGES (Private->Nutmrs * sizeof (UTP_TMRD)),
        Private->UtpTmrlBase,
        Private->TmrlMapping
        );
      Private->TmrlMapping = NULL;
    }

    return Status;
  }

Exit:
  DEBUG ((DEBUG_ERROR, "Initialize UFS device: %r\n", Status));
  return Status;

}

/**
  Stop the UFS host controller.

  @param[in] Private                 The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.

  @retval EFI_SUCCESS                The Ufs Host Controller is stopped successfully.
  @retval Others                     A device error occurred while stopping the controller.

**/
EFI_STATUS
UfsControllerStop (
  IN  UFS_PEIM_HC_PRIVATE_DATA       *Private
  )
{
  EFI_STATUS             Status;
  UINTN                  Address;
  UINT32                 Data;

  //
  // Enable the UTP Task Management Request List by setting the UTP Task Management
  // Request List RunStop Register (UTMRLRSR) to '1'.
  //
  Address = Private->UfsHcBase + UFS_HC_UTMRLRSR_OFFSET;
  MmioWrite32 (Address, 0);

  //
  // Enable the UTP Transfer Request List by setting the UTP Transfer Request List
  // RunStop Register (UTRLRSR) to '1'.
  //
  Address = Private->UfsHcBase + UFS_HC_UTRLRSR_OFFSET;
  MmioWrite32 (Address, 0);

  //
  // Write a 0 to the HCE register in order to disable the host controller.
  //
  Address = Private->UfsHcBase + UFS_HC_ENABLE_OFFSET;
  Data    = MmioRead32 (Address);
  ASSERT ((Data & UFS_HC_HCE_EN) == UFS_HC_HCE_EN);
  MmioWrite32 (Address, 0);

  //
  // Wait until HCE is read as '0' before continuing.
  //
  Status = UfsWaitMemSet (Address, UFS_HC_HCE_EN, 0, UFS_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((DEBUG_INFO, "UfsDevicePei: Stop the UFS Host Controller\n"));

  return EFI_SUCCESS;
}

/**
  Initializes UfsHcInfo field in private data.

  @param[in] Private  Pointer to host controller private data.

  @retval EFI_SUCCESS  UfsHcInfo initialized successfully.
  @retval Others       Failed to initalize UfsHcInfo.
**/
EFI_STATUS
GetUfsHcInfo (
  IN UFS_PEIM_HC_PRIVATE_DATA  *Private
  )
{
  UINTN       Address;
  UINT32      Data;

  Address = Private->UfsHcBase + UFS_HC_VER_OFFSET;
  Data = MmioRead32 (Address);

  Private->Version = Data;

  Address = Private->UfsHcBase + UFS_HC_CAP_OFFSET;
  Data    = MmioRead32 (Address);
  Private->Capabilities = Data;

  return EFI_SUCCESS;
}


/**
  Read specified flag from a UFS device.

  @param[in]  Private           The pointer to the UFS_PEIM_HC_PRIVATE_DATA data structure.
  @param[in]  FlagId            The ID of flag to be read.
  @param[out] Value             The flag's value.

  @retval EFI_SUCCESS           The flag was read successfully.
  @retval EFI_DEVICE_ERROR      A device error occurred while attempting to read the flag.
  @retval EFI_TIMEOUT           A timeout occurred while waiting for the completion of reading the flag.

**/
EFI_STATUS
UfsReadFlag (
  IN     UFS_PEIM_HC_PRIVATE_DATA   *Private,
  IN     UINT8                        FlagId,
  OUT    UINT8                        *Value
  )
{
  EFI_STATUS                           Status;

  Status = UfsRwFlags (Private, TRUE, FlagId, Value);

  return Status;
}
