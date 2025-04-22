/** @file
  ISH Heci driver core for DXE phase

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCommonLib.h>
#include <PcieRegs.h>
#include "HeciCore.h"
#include "IshRegs.h"


#define MAX_ISH_HECI_TIMEOUT        1000 // 1 second in miliseconds
#define IshPciRead32(func, reg)     PciRead32 (PCI_LIB_ADDRESS(ME_BUS, PCI_DEVICE_NUMBER_TGL_PCH_ISH, func, reg))
#define IshPciRead16(func, reg)     PciRead16 (PCI_LIB_ADDRESS(ME_BUS, PCI_DEVICE_NUMBER_TGL_PCH_ISH, func, reg))
#define IshPciOr32(func, reg, val)  PciOr32 (PCI_LIB_ADDRESS(ME_BUS, PCI_DEVICE_NUMBER_TGL_PCH_ISH, func, reg), val)


typedef enum {
  AFTER_RESET             = 0,
  WAITING_FOR_PATCH       = 1,
  BUP_IN_PROGRESS         = 2,
  LOAD_REQ_SENT           = 3,
  WAIT_FOR_HOST           = 4,
  START_KERNEL_DMA        = 5,
  JUMP_TO_KERNEL          = 6,
  FW_IS_RUNNING           = 7,
  SENSORS_APP_LOADED      = 8,
  SENSORS_TABLES_LOADED   = 9,
  SENSORS_APP_RUNNING     = 0xF
} FW_STATE_ENUM;

typedef enum {
  IPC_PROTOCOL_HECI = 1,
  IPC_PROTOCOL_MNG  = 3
} IPC_PROTOCOL;

typedef enum {
  MNG_RX_CMPL_ENABLE     = 0,
  MNG_RX_CMPL_DISABLE    = 1,
  MNG_RX_CMPL_INDICATION = 2,
  MNG_RESET_NOTIFY       = 3,
  MNG_RESET_NOTIFY_ACK   = 4,
  MNG_ILLEGAL_CMD        = 0xFF
} IPC_MNG_COMMAND;

typedef union {
  UINT32  Data;
  struct {
    UINT32  Length       : 10;
    UINT32  Protocol     : 4;
    UINT32  Reserved1    : 2;
    UINT32  MngCommand   : 4;
    UINT32  MngReserved  : 4;
    UINT32  Reserved2    : 7;
    UINT32  Busy         : 1;
  } Fields;
} ISH_DOORBELL;

UINT32 gIshAddr;

/**
  Returns ISH firmware status

  @retval ISH firmware status
**/
FW_STATE_ENUM
GetIshFwStatus (
  )
{
  return (MmioRead32 (gIshAddr + R_ISH_MEM_FW_STS) & B_ISH_MEM_FW_STS_STATUS) >> N_ISH_MEM_FW_STS_STATUS;
}

/**
  Polls Host2Ish doorbell register and waits for Busy bit to be cleared, indicating that ISH consumed a message from BIOS

  @param[in] Timeout     maximum wait time in miliseconds

  @retval EFI_SUCCESS    Busy bit was cleared by ISH firmware
  @retval EFI_TIMEOUT    Busy bit was not cleared in allowed time
**/
EFI_STATUS
WaitForHost2IshClear (
  UINT32 Timeout
  )
{
  UINT32       Time;
  ISH_DOORBELL H2iDoorbell;

  Time = 0;
  do {
    H2iDoorbell.Data = MmioRead32 (gIshAddr + R_ISH_MEM_HOST2ISH_DRBL);
    if (!H2iDoorbell.Fields.Busy) {
      return EFI_SUCCESS;
    }
    MicroSecondDelay (HECI_WAIT_DELAY);
    Time ++;
  } while (Time < Timeout);
  DEBUG ((DEBUG_ERROR, "WaitForHost2IshClear timeout\n"));
  return EFI_TIMEOUT;
}

/**
  Polls Ish2Host doorbell register and waits for Busy bit to be set, indicating that a new message from ISH has arrived

  @param[in] Timeout     maximum wait time in miliseconds

  @retval EFI_SUCCESS    New message arrived
  @retval EFI_TIMEOUT    No new message arrived in allowed time
**/
EFI_STATUS
WaitForIsh2HostResponse (
  UINT32 Timeout
  )
{
  UINT32       Time;
  ISH_DOORBELL I2hDoorbell;

  Time = 0;
  do {
    I2hDoorbell.Data = MmioRead32 (gIshAddr + R_ISH_MEM_ISH2HOST_DRBL);
    if (I2hDoorbell.Fields.Busy) {
      return EFI_SUCCESS;
    }
    MicroSecondDelay (HECI_WAIT_DELAY);
    Time ++;
  } while (Time < Timeout);
  DEBUG ((DEBUG_ERROR, "WaitForIsh2HostResponse timeout\n"));
  return EFI_TIMEOUT;
}

/**
  Waits for 'completion indication' message from Management protocol to arrive, then consumes this message

  @param[in] Timeout          maximum wait time in miliseconds

  @retval EFI_SUCCESS         New message arrived
  @retval EFI_TIMEOUT         No new message arrived in allowed time
  @retval EFI_PROTOCOL_ERROR  Wrong message arrived
**/
EFI_STATUS
WaitForCompletion (
  UINT32 Timeout
  )
{
  ISH_DOORBELL I2hDoorbell;
  EFI_STATUS   Status;

  Status = WaitForIsh2HostResponse (MAX_ISH_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "WaitForCompletion timeout\n"));
    return Status;
  }
  I2hDoorbell.Data = MmioRead32 (gIshAddr + R_ISH_MEM_ISH2HOST_DRBL);
  if(I2hDoorbell.Fields.Protocol == IPC_PROTOCOL_MNG && I2hDoorbell.Fields.MngCommand == MNG_RX_CMPL_INDICATION)  {
    MmioAnd32(gIshAddr + R_ISH_MEM_ISH2HOST_DRBL, ~B_ISH_MEM_DRBL_BUSY);
  } else {
    DEBUG ((DEBUG_ERROR, "WaitForCompletion protocol error %x %x\n", I2hDoorbell.Fields.Protocol, I2hDoorbell.Fields.MngCommand));
    return EFI_PROTOCOL_ERROR;
  }
  return EFI_SUCCESS;
}

/**
  Waits for any message from Management protocol to arrive, then consumes this message.
  This may include completion indication, reset and reset acknowledgement

  @param[in] Timeout          maximum wait time in miliseconds

  @retval EFI_SUCCESS         New message arrived
  @retval EFI_TIMEOUT         No new message arrived in allowed time
  @retval EFI_PROTOCOL_ERROR  Wrong message arrived
**/
EFI_STATUS
WaitForCompletionOrResetAck (
  UINT32 Timeout
  )
{
  ISH_DOORBELL I2hDoorbell;
  EFI_STATUS   Status;

  Status = WaitForIsh2HostResponse (MAX_ISH_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "WaitForCompletionOrResetAck timeout\n"));
    return Status;
  }
  I2hDoorbell.Data = MmioRead32 (gIshAddr + R_ISH_MEM_ISH2HOST_DRBL);
  if (I2hDoorbell.Fields.Protocol == IPC_PROTOCOL_MNG) {
    MmioAnd32(gIshAddr + R_ISH_MEM_ISH2HOST_DRBL, ~B_ISH_MEM_DRBL_BUSY);
  } else {
    DEBUG ((DEBUG_ERROR, "WaitForCompletionOrResetAck protocol error %x %x\n", I2hDoorbell.Fields.Protocol, I2hDoorbell.Fields.MngCommand));
    return EFI_PROTOCOL_ERROR;
  }
  return EFI_SUCCESS;
}

/**
  Initializes ISH device to allow MMIO access and stores the MMIO address in global variable
  This function will only set Memory Enable bit. MMIO range must be already assigned by PCI enumerator.

  @retval EFI_SUCCESS         Device initialized, global variable contains updated address
  @retval EFI_DEVICE_ERROR    Can't find the device
  @retval EFI_NOT_READY       Device found, but its MMIO address was not yet programmed by enumerator
**/
EFI_STATUS
IshHeciHardwareInit (
  )
{
  UINT16 DevId;

  DevId = IshPciRead16 (PCI_FUNCTION_NUMBER_TGL_PCH_ISH, PCI_DEVICE_ID_OFFSET);
  gIshAddr = IshPciRead32 (PCI_FUNCTION_NUMBER_TGL_PCH_ISH, PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFF000;

  if (DevId != V_TGL_LP_ISH_DEVICE_ID) {
    return EFI_DEVICE_ERROR;
  } else if (gIshAddr == 0x0 || gIshAddr == 0xFFFFF000) {
    return EFI_NOT_READY;
  } else {
    IshPciOr32(PCI_FUNCTION_NUMBER_TGL_PCH_ISH, PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
    return EFI_SUCCESS;
  }
}

/**
  Initializes ISH firmware (unless already initialized) to allow HECI communication
  This function allows generous timeouts just in case, but under normal circumstances whole init should take less than 500ms

  @retval EFI_SUCCESS         Firmware initialized correctly
  @retval EFI_DEVICE_ERROR    Can't find the device
  @retval EFI_NOT_READY       Device found, but its MMIO address was not yet programmed by enumerator
  @retval any other status    Firmware failed to initialize
**/
EFI_STATUS
IshHeciFirmwareInit (
  VOID
  )
{
  EFI_STATUS Status;
  UINT32     Time;

  Status = IshHeciHardwareInit();
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (GetIshFwStatus () == SENSORS_APP_RUNNING) {
    return EFI_SUCCESS;
  }
  DEBUG ((DEBUG_INFO, "IshHeciFirmwareInit\n"));

  //
  //set host ready
  //set ART frequency (for Hammock Harbor)
  //set driver loaded
  //set driver ready
  //
  MmioOr32 (gIshAddr + R_ISH_MEM_HOST_COMM, B_ISH_MEM_HOST_COMM_HOST_READY);
  MmioWrite32 (gIshAddr + R_ISH_MEM_REMAP3, 24000000);
  MmioOr32 (gIshAddr + R_ISH_MEM_REMAP2, DRIVER_LOADED_BIT);
  MmioOr32 (gIshAddr + R_ISH_MEM_HOST_COMM, B_ISH_MEM_HOST_COMM_DRIVER_READY);
  //
  // Write 'driver is loaded' message & header
  //
  MmioWrite32 (gIshAddr + R_ISH_MEM_HOST2ISH_MSG, 0x1);
  MmioWrite32 (gIshAddr + R_ISH_MEM_HOST2ISH_DRBL, 0x80030c04);

  Status = WaitForHost2IshClear (MAX_ISH_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = WaitForCompletionOrResetAck (MAX_ISH_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = WaitForCompletionOrResetAck (MAX_ISH_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Write 'reset' message & header. The contents are that same as in 'driver is loaded' but it has different meaning now.
  //
  MmioWrite32 (gIshAddr + R_ISH_MEM_HOST2ISH_MSG, 0x1);
  MmioWrite32 (gIshAddr + R_ISH_MEM_HOST2ISH_DRBL, 0x80030c04);

  Status=WaitForHost2IshClear (MAX_ISH_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = WaitForCompletionOrResetAck (MAX_ISH_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = WaitForCompletionOrResetAck (MAX_ISH_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Time = MAX_ISH_HECI_TIMEOUT;
  while (GetIshFwStatus () != SENSORS_APP_RUNNING) {
    MicroSecondDelay (HECI_WAIT_DELAY);
    if (Time-- == 0) {
      return EFI_DEVICE_ERROR;
    }
  }
  MmioAnd32 (gIshAddr + R_ISH_MEM_ISH2HOST_DRBL, ~B_ISH_MEM_DRBL_BUSY);
  return EFI_SUCCESS;
}

/*
  Reads single packet of a HECI message from ISH.

  @param[in]     Timeout         indicates how much time (in miliseconds) this function should wait for a message to arrive
  @param[out]    MessageBody     message will be copied to this buffer, up to its capacity
  @param[in,out] BufferLength    on input, size of buffer.
                                 on exit, size of data copied into buffer
  @param[in,out] MessageLength   on exit, size of data in received packet. This may be bigger than BufferLength
  @param[out]    MessageComplete False means this was not the last packed of a message; True means this message is finished
  @retval EFI_TIMEOUT            No message received
  @retval EFI_PROTOCOL_ERROR     Malformed message or unexpected type of message received, nothing was copied to MessageBody
  @retval EFI_BUFFER_TOO_SMALL   Buffer was too small to hold whole message packet. As much as possible was copied.
  @retval EFI_SUCCESS            Whole message packet was copied to buffer
*/
EFI_STATUS
IshHeciReceivePacket (
  IN      UINT32                  Timeout,
  IN OUT  UINT32                  *MessageBody,
  IN OUT  UINT32                  *BufferLength,
  OUT     UINT32                  *MessageLength,
  OUT     BOOLEAN                 *MessageComplete
  )
{
  EFI_STATUS          Status;
  HECI_MESSAGE_HEADER MsgHeader;
  ISH_DOORBELL        Doorbell;

  *MessageComplete = TRUE;
  *MessageLength = 0;
  Status = EFI_SUCCESS;

  Status = WaitForIsh2HostResponse (Timeout);
  if (EFI_ERROR (Status)) {
    *BufferLength = 0;
    return EFI_TIMEOUT;
  }
  Doorbell.Data = MmioRead32 (gIshAddr + R_ISH_MEM_ISH2HOST_DRBL);
  if (Doorbell.Fields.Protocol != IPC_PROTOCOL_HECI) {
    *BufferLength = 0;
    return EFI_PROTOCOL_ERROR;
  }
  MsgHeader.Data = MmioRead32 (gIshAddr + R_ISH_MEM_ISH2HOST_MSG);
  if (!MsgHeader.Fields.MessageComplete) {
    *MessageComplete = FALSE;
  }
  if (MsgHeader.Fields.Length > *BufferLength) {
    Status = EFI_BUFFER_TOO_SMALL;
  }
  *MessageLength = MsgHeader.Fields.Length;
  *BufferLength = MIN (*BufferLength, MsgHeader.Fields.Length);
  CopyMem (MessageBody, (UINT32*)((UINTN)(gIshAddr + R_ISH_MEM_ISH2HOST_MSG + 4)), *BufferLength);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "IshHeciReceive Doorbell %08x Header %08x Status %r\n", Doorbell.Data, MsgHeader.Data, Status));
  } else {
    DEBUG ((DEBUG_INFO, "IshHeciReceive Doorbell %08x Header %08x\n", Doorbell.Data, MsgHeader.Data));
  }
  DEBUG_CODE (
    DumpHex (2, 0, MsgHeader.Fields.Length, (UINT8*)((UINTN)(gIshAddr + R_ISH_MEM_ISH2HOST_MSG + 4)));
  );
  MmioWrite32 (gIshAddr + R_ISH_MEM_ISH2HOST_DRBL, 0);
  return Status;
}


/**
  Reads a HECI message from ISH.

  @param[in]      Blocking        Decides if function should wait for response. BLOCKING will wait for the message
                                    for 1s. Non-blocking will exit if there's no message. If 1st packet of multipacket message
                                    is received, the function will ignore Blocking parameter and will wait for following packets
  @param[in,out]  MessageBody     Pointer to a buffer used to receive a message.
  @param[in,out]  Length          On entry, length of the buffer in bytes
                                  On exit, total length of the message. It can be bigger than original Length, it means
                                    the message could not be fully copied into buffer and the rest was lost
  @retval EFI_SUCCESS             Message was retrieved and placed in buffer
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough. As much data as possible was copied to buffer,
                                    the rest was received and ignored
  @retval EFI_TIMEOUT             No message received
  @retval EFI_PROTOCOL_ERROR      Unhandled error caused by ISH firmware
**/
EFI_STATUS
IshHeciReceive (
  IN      UINT32                  Blocking,
  IN OUT  UINT32                  *MessageBody,
  IN OUT  UINT32                  *Length
  )
{
  EFI_STATUS Status;
  UINT32     BufferSize;
  UINT32     BytesCopied;
  UINT32     BytesInPacket;
  UINT32     TotalMsgLength;
  BOOLEAN    MessageComplete;
  UINT32     Timeout;

  MessageComplete = TRUE;

  Status = IshHeciFirmwareInit();
  if (EFI_ERROR (Status)) {
    return Status;
  }
  BufferSize = *Length;
  BytesCopied = 0;
  TotalMsgLength = 0;
  //
  // If NON_BLOCKING was passed we will not wait for response, just check if one is already available...
  //
  if (Blocking) {
    Timeout = MAX_ISH_HECI_TIMEOUT;
  } else {
    Timeout = 0;
  }
  do {
    ASSERT (BytesCopied <= BufferSize);
    //
    // ...however, in case of multipacket messages we must set non-zero timeout anyway to allow code to retrieve
    // whole message because follow-up packets will always have some small delay
    //
    if (!MessageComplete) {
      Timeout = MAX_ISH_HECI_TIMEOUT;
    }
    *Length = BufferSize - BytesCopied;
    Status = IshHeciReceivePacket (Timeout, MessageBody, Length, &BytesInPacket, &MessageComplete);
    //
    // Advance pointer and keep counts how many bytes were copied into buffer and how big was the whole message
    // In case of BUFFER_TOO_SMALL those will not be equal
    //
    MessageBody += *Length / 4; // pointer arithmetic for UINT32*
    BytesCopied += *Length;
    TotalMsgLength += BytesInPacket;
  } while (!MessageComplete);
  *Length = TotalMsgLength;
  return Status;
}

/**
  Sends a HECI message to ISH.

  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MeAddress            Address of the subsystem the message is being sent to.

  @retval EFI_SUCCESS             Message sent
  @retval EFI_UNSUPPORTED         Message is too big to send as single packet; multipacket messages not supported in BIOS yet
  @retval any other status        Error caused by ISH firmware
**/
EFI_STATUS
IshHeciSend (
  IN UINT32                       *Message,
  IN UINT32                       Length,
  IN UINT8                        HostAddress,
  IN UINT8                        MeAddress
  )
{
  HECI_MESSAGE_HEADER  MsgHeader;
  ISH_DOORBELL         Doorbell;
  EFI_STATUS           Status;

  Status = IshHeciFirmwareInit();
  if (EFI_ERROR (Status)) {
    return EFI_TIMEOUT;
  }
  if (Length > 124) {
    return EFI_UNSUPPORTED; //TODO: implement multipart messages
  }

  Status = WaitForHost2IshClear (MAX_ISH_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Doorbell.Data = 0;
  Doorbell.Fields.Length = Length + sizeof(HECI_MESSAGE_HEADER);
  Doorbell.Fields.Protocol = IPC_PROTOCOL_HECI;
  Doorbell.Fields.Busy = 1;
  MsgHeader.Data = 0;
  MsgHeader.Fields.MeAddress = MeAddress;
  MsgHeader.Fields.HostAddress = HostAddress;
  MsgHeader.Fields.Length = Length;
  MsgHeader.Fields.MessageComplete = 1;

  DEBUG ((DEBUG_INFO, "IshHeciSend Doorbell %08x Header %08x\n", Doorbell.Data, MsgHeader.Data));
  DEBUG_CODE (
    DumpHex (2, 0, Length, (UINT8*)Message);
  );
  MmioWrite32 (gIshAddr + R_ISH_MEM_HOST2ISH_MSG, MsgHeader.Data);
  CopyMem ((UINT32*)((UINTN)(gIshAddr + R_ISH_MEM_HOST2ISH_MSG + 4)), Message, Length);
  MmioWrite32 (gIshAddr + R_ISH_MEM_HOST2ISH_DRBL, Doorbell.Data);

  Status = WaitForHost2IshClear (MAX_ISH_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = WaitForCompletion (MAX_ISH_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  return EFI_SUCCESS;
}

/**
  Function sends one message through the ISH HECI and waits
  for the corresponding response message

  @param[in,out] Message          Pointer to the message buffer.
  @param[in] Length               Length of the message in bytes.
  @param[in,out] RecLength        Length of the message response in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @retval EFI_SUCCESS             Message sent and response received
  @retval EFI_UNSUPPORTED         Message is too big to send as single packet; sending multipacket messages not supported yet
  @retval any other status        Error caused by ISH firmware
**/
EFI_STATUS
IshHeciSendAndReceive (
  IN OUT  UINT32                  *Message,
  IN      UINT32                  Length,
  IN OUT  UINT32                  *RecLength,
  IN      UINT8                   HostAddress,
  IN      UINT8                   MeAddress
  )
{
  EFI_STATUS Status;

  Status = IshHeciSend (Message, Length, HostAddress, MeAddress);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  return IshHeciReceive (BLOCKING, Message, RecLength);
}
