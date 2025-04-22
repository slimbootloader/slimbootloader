/** @file
  PSE Heci driver core for DXE phase

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HeciInitLib.h>
#include <PcieRegs.h>
#include <Register/PseRegs.h>
#include "MkhiMsgs.h"
#include "BupMsgs.h"
#include "MeState.h"
#include "HeciCore.h"
#include "HeciHpet.h"
#include <Register/HeciRegs.h>

#define MAX_PSE_HECI_TIMEOUT      0x10000 // 10 second in miliseconds
#define HECI_MAX_PAYLOAD_SIZE     128 - sizeof(HECI_MESSAGE_HEADER)

#define PsePciRead32(func, reg)     PciRead32 (PCI_LIB_ADDRESS(ME_BUS, PCI_DEVICE_NUMBER_PCH_PSE_LH2PSE, func, reg))
#define PsePciRead16(func, reg)     PciRead16 (PCI_LIB_ADDRESS(ME_BUS, PCI_DEVICE_NUMBER_PCH_PSE_LH2PSE, func, reg))
#define PsePciOr32(func, reg, val)  PciOr32 (PCI_LIB_ADDRESS(ME_BUS, PCI_DEVICE_NUMBER_PCH_PSE_LH2PSE, func, reg), val)

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
} PSE_DOORBELL;

UINT64 gPseAddr;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN gPseHeciLinkSetup = FALSE;

/**
  Returns PSE firmware status

  @retval PSE firmware status
**/
FW_STATE_ENUM
GetPseFwStatus (
  )
{
  return (MmioRead32 ((UINTN) (gPseAddr + R_PSE_MEM_FW_STS)) & B_PSE_MEM_FW_STS_STATUS) >> N_PSE_MEM_FW_STS_STATUS;
}

/**
  Polls Host2Pse doorbell register and waits for Busy bit to be cleared, indicating that PSE consumed a message from BIOS

  @param[in] Timeout     maximum wait time in miliseconds

  @retval EFI_SUCCESS    Busy bit was cleared by PSE firmware
  @retval EFI_TIMEOUT    Busy bit was not cleared in allowed time
**/
EFI_STATUS
WaitForHost2PseClear (
  UINT32 Timeout
  )
{
  UINT32       Time;
  PSE_DOORBELL H2iDoorbell;

  Time = 0;
  do {
    H2iDoorbell.Data = MmioRead32 ((UINTN) (gPseAddr + R_PSE_MEM_HOST2PSE_DRBL));
    if (!H2iDoorbell.Fields.Busy) {
      return EFI_SUCCESS;
    }
    MicroSecondDelay (HECI_WAIT_DELAY);
    Time ++;
  } while (Time < Timeout);
  DEBUG ((DEBUG_ERROR, "WaitForHost2PseClear timeout\n"));
  return EFI_TIMEOUT;
}

/**
  Polls Pse2Host doorbell register and waits for Busy bit to be set, indicating that a new message from PSE has arrived

  @param[in] Timeout     maximum wait time in miliseconds

  @retval EFI_SUCCESS    New message arrived
  @retval EFI_TIMEOUT    No new message arrived in allowed time
**/
EFI_STATUS
WaitForPse2HostResponse (
  UINT32 Timeout
  )
{
  UINT32       Time;
  PSE_DOORBELL I2hDoorbell;

  Time = 0;
  do {
    I2hDoorbell.Data = MmioRead32 ((UINTN) (gPseAddr + R_PSE_MEM_PSE2HOST_DRBL));
    if (I2hDoorbell.Fields.Busy) {
      return EFI_SUCCESS;
    }
    MicroSecondDelay (HECI_WAIT_DELAY);
    Time ++;
  } while (Time < Timeout);
  DEBUG ((DEBUG_ERROR, "WaitForPse2HostResponse timeout\n"));
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
PseWaitForCompletion (
  UINT32 Timeout
  )
{
  PSE_DOORBELL I2hDoorbell;
  EFI_STATUS   Status;

  Status = WaitForPse2HostResponse (MAX_PSE_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "PseWaitForCompletion timeout\n"));
    return Status;
  }
  I2hDoorbell.Data = MmioRead32 ((UINTN) (gPseAddr + R_PSE_MEM_PSE2HOST_DRBL));
  if(I2hDoorbell.Fields.Protocol == IPC_PROTOCOL_MNG && I2hDoorbell.Fields.MngCommand == MNG_RX_CMPL_INDICATION)  {
    MmioAnd32 ((UINTN) (gPseAddr + R_PSE_MEM_PSE2HOST_DRBL), (UINT32)~B_PSE_MEM_DRBL_BUSY);
  } else {
    DEBUG ((DEBUG_ERROR, "PseWaitForCompletion protocol error %x %x\n", I2hDoorbell.Fields.Protocol, I2hDoorbell.Fields.MngCommand));
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
PseWaitForCompletionOrResetAck (
  UINT32 Timeout
  )
{
  PSE_DOORBELL I2hDoorbell;
  EFI_STATUS   Status;

  Status = WaitForPse2HostResponse (MAX_PSE_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "PseWaitForCompletionOrResetAck timeout\n"));
    return Status;
  }
  I2hDoorbell.Data = MmioRead32 ((UINTN) (gPseAddr + R_PSE_MEM_PSE2HOST_DRBL));
  if (I2hDoorbell.Fields.Protocol == IPC_PROTOCOL_MNG) {
    MmioAnd32 ((UINTN) (gPseAddr + R_PSE_MEM_PSE2HOST_DRBL), (UINT32)~B_PSE_MEM_DRBL_BUSY);
  } else {
    DEBUG ((DEBUG_ERROR, "PseWaitForCompletionOrResetAck protocol error %x %x\n", I2hDoorbell.Fields.Protocol, I2hDoorbell.Fields.MngCommand));
    return EFI_PROTOCOL_ERROR;
  }
  return EFI_SUCCESS;
}

/**
  Initializes PSE device to allow MMIO access and stores the MMIO address in global variable
  This function will only set Memory Enable bit. MMIO range must be already assigned by PCI enumerator.

  @retval EFI_SUCCESS         Device initialized, global variable contains updated address
  @retval EFI_DEVICE_ERROR    Can't find the device
  @retval EFI_NOT_READY       Device found, but its MMIO address was not yet programmed by enumerator
**/
EFI_STATUS
PseHeciHardwareInit (
  )
{
  UINT32 PseAddrLower;
  UINT32 PseAddrUpper;

  PseAddrLower      = 0;
  PseAddrUpper      = 0;

  PseAddrLower = PsePciRead32 (PCI_DEVICE_NUMBER_PCH_PSE_LH2PSE, PCI_BASE_ADDRESSREG_OFFSET);
  if ((PseAddrLower & 0x06) == 0x04) {
    PseAddrUpper = PsePciRead32 (PCI_DEVICE_NUMBER_PCH_PSE_LH2PSE, PCI_BASE_ADDRESSREG_OFFSET + 4);
  }
  gPseAddr = (UINT64) (LShiftU64 (PseAddrUpper, 32) | (PseAddrLower & 0xFFFFF000));
  DEBUG ((DEBUG_INFO, "gPseAddr: 0x%08llX\n", gPseAddr));

  if (gPseAddr == 0x0 || gPseAddr == 0xFFFFF000ULL || gPseAddr == 0xFFFFFFFFFFFFF000ULL) {
    return EFI_NOT_READY;
  } else {
    PsePciOr32 (PCI_DEVICE_NUMBER_PCH_PSE_LH2PSE, PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
    return EFI_SUCCESS;
  }
}

/**
  Initializes PSE firmware (unless already initialized) to allow HECI communication
  This function allows generous timeouts just in case, but under normal circumstances whole init should take less than 500ms

  @retval EFI_SUCCESS         Firmware initialized correctly
  @retval EFI_DEVICE_ERROR    Can't find the device
  @retval EFI_NOT_READY       Device found, but its MMIO address was not yet programmed by enumerator
  @retval any other status    Firmware failed to initialize
**/
EFI_STATUS
PseHeciFirmwareInit (
  VOID
  )
{
  EFI_STATUS Status;
  // UINT32     Time;

  DEBUG ((DEBUG_INFO, "PseHeciHardwareInit - allocating MMIO\n"));
  Status = PseHeciHardwareInit();
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (gPseHeciLinkSetup) {
    return EFI_SUCCESS;
  }
  DEBUG ((DEBUG_INFO, "PseHeciFirmwareInit\n"));

  //
  // Clear incoming doorbell
  //
  MmioWrite32 ((UINTN) (gPseAddr + R_PSE_MEM_PSE2HOST_DRBL), 0x0);
  //
  // Set host ready
  //
  MmioOr32 ((UINTN) (gPseAddr + R_PSE_MEM_HOST_COMM), B_PSE_MEM_HOST_COMM_HOST_READY);
  //
  // Set ART frequency (for Hammock Harbor)
  //
  MmioWrite32 ((UINTN) (gPseAddr + R_PSE_MEM_REMAP3), 24000000);
  //
  // Set driver loaded
  //
  MmioOr32 ((UINTN) (gPseAddr + R_PSE_MEM_REMAP2), DRIVER_LOADED_BIT);
  //
  // Set driver ready
  //
  MmioOr32 ((UINTN) (gPseAddr + R_PSE_MEM_HOST_COMM), B_PSE_MEM_HOST_COMM_DRIVER_READY);
  //
  // Write 'driver is loaded' message & header
  //
  MmioWrite32 ((UINTN) (gPseAddr + R_PSE_MEM_HOST2PSE_MSG), 0x1);
  MmioWrite32 ((UINTN) (gPseAddr + R_PSE_MEM_HOST2PSE_DRBL), 0x80030c04);

  Status = WaitForHost2PseClear (MAX_PSE_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "WaitForHost2PseClear 1 failed with %x\n", Status));
    return Status;
  }
  Status = PseWaitForCompletionOrResetAck (MAX_PSE_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "PseWaitForCompletionOrResetAck 1 failed with %x\n", Status));
    return Status;
  }
  Status = PseWaitForCompletionOrResetAck (MAX_PSE_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "PseWaitForCompletionOrResetAck 2 failed with %x\n", Status));
    return Status;
  }

  //
  // Write 'reset' message & header. The contents are that same as in 'driver is loaded' but it has different meaning now.
  //
  MmioWrite32 ((UINTN) (gPseAddr + R_PSE_MEM_HOST2PSE_MSG), 0x1);
  MmioWrite32 ((UINTN) (gPseAddr + R_PSE_MEM_HOST2PSE_DRBL), 0x80030c04);

  Status = WaitForHost2PseClear (MAX_PSE_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "WaitForHost2PseClear 2 failed with %x\n", Status));
    return Status;
  }
  Status = PseWaitForCompletionOrResetAck (MAX_PSE_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "PseWaitForCompletionOrResetAck 3 failed with %x\n", Status));
    return Status;
  }
  Status = PseWaitForCompletionOrResetAck (MAX_PSE_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "PseWaitForCompletionOrResetAck 4 failed with %x\n", Status));
    return Status;
  }
  // Time = MAX_PSE_HECI_TIMEOUT;
  // while (GetPseFwStatus () != SENSORS_APP_RUNNING) {
    // MicroSecondDelay (HECI_WAIT_DELAY);
    // if (Time-- == 0) {
      // DEBUG ((DEBUG_INFO, "Dies at SENSORS_APP_RUNNING %x\n", Status));
      // return EFI_DEVICE_ERROR;
    // }
  // }
  MmioAnd32 ((UINTN) (gPseAddr + R_PSE_MEM_PSE2HOST_DRBL), (UINT32)~B_PSE_MEM_DRBL_BUSY);
  gPseHeciLinkSetup = TRUE;
  DEBUG ((DEBUG_INFO, "Completed successfully\n"));
  return EFI_SUCCESS;
}

/*
  Reads single packet of a HECI message from PSE.

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
PseHeciReceivePacket (
  IN      UINT32                  Timeout,
  IN OUT  UINT32                  *MessageBody,
  IN OUT  UINT32                  *BufferLength,
  OUT     UINT32                  *MessageLength,
  OUT     BOOLEAN                 *MessageComplete
  )
{
  EFI_STATUS          Status;
  HECI_MESSAGE_HEADER MsgHeader;
  PSE_DOORBELL        Doorbell;

  *MessageComplete = TRUE;
  *MessageLength = 0;
  Status = EFI_SUCCESS;

  DEBUG ((DEBUG_ERROR, "Timeout Value before: %x\n", Timeout));
  //hardcoding to maximum timeout due to HFPGA platform delay
  Timeout = MAX_PSE_HECI_TIMEOUT;
  DEBUG ((DEBUG_ERROR, "Timeout Value after: %x\n", Timeout));
  Status = WaitForPse2HostResponse (Timeout);
  if (EFI_ERROR (Status)) {
    *BufferLength = 0;
    return EFI_TIMEOUT;
  }
  Doorbell.Data = MmioRead32 ((UINTN) (gPseAddr + R_PSE_MEM_PSE2HOST_DRBL));
  DEBUG ((DEBUG_ERROR, "Doorbell.Data: %x\n", Doorbell.Data));
  if (Doorbell.Fields.Protocol != IPC_PROTOCOL_HECI) {
    DEBUG ((DEBUG_ERROR, "error on protocol, expecting IPC_PROTOCOL_HECI: %x, get %x\n", IPC_PROTOCOL_HECI, Doorbell.Fields.Protocol));
    *BufferLength = 0;
    return EFI_PROTOCOL_ERROR;
  }
  MsgHeader.Data = MmioRead32 ((UINTN) (gPseAddr + R_PSE_MEM_PSE2HOST_MSG));
  DEBUG ((DEBUG_ERROR, "MsgHeader.Data: %x\n", MsgHeader.Data));
  if (!MsgHeader.Fields.MessageComplete) {
    DEBUG ((DEBUG_ERROR, "!MsgHeader.Fields.MessageComplete case entered. MessageComplete = FALSE\n"));
    *MessageComplete = FALSE;
  }
  if (MsgHeader.Fields.Length > *BufferLength) {
    DEBUG ((DEBUG_ERROR, "Buffer too small\n"));
    Status = EFI_BUFFER_TOO_SMALL;
  }
  *MessageLength = MsgHeader.Fields.Length;
  *BufferLength = MIN (*BufferLength, MsgHeader.Fields.Length);
  CopyMem (MessageBody, (UINT32*)((UINTN)(gPseAddr + R_PSE_MEM_PSE2HOST_MSG + 4)), *BufferLength);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Error: PseHeciReceive Doorbell %08x Header %08x Status %r\n", Doorbell.Data, MsgHeader.Data, Status));
  } else {
    DEBUG ((DEBUG_INFO, "PseHeciReceive Doorbell %08x Header %08x\n", Doorbell.Data, MsgHeader.Data));
  }
  MmioWrite32 ((UINTN) (gPseAddr + R_PSE_MEM_PSE2HOST_DRBL), 0);
  return Status;
}


/**
  Reads a HECI message from PSE.

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
  @retval EFI_PROTOCOL_ERROR      Unhandled error caused by PSE firmware
**/
EFI_STATUS
PseHeciReceive (
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

  DEBUG ((DEBUG_INFO, "PseHeciReceive\n"));
  MessageComplete = TRUE;

  Status = PseHeciFirmwareInit();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "TIMEOUT\n"));
    return Status;
  }
  BufferSize = *Length;
  BytesCopied = 0;
  TotalMsgLength = 0;
  //
  // If NON_BLOCKING was passed we will not wait for response, just check if one is already available...
  //
  if (Blocking) {
    DEBUG ((DEBUG_INFO, "Blocking mode\n"));
    Timeout = MAX_PSE_HECI_TIMEOUT;
  } else {
    DEBUG ((DEBUG_INFO, "None blocking mode\n"));
    Timeout = 0;
  }
  do {
    DEBUG ((DEBUG_INFO, "|\n"));
    ASSERT (BytesCopied <= BufferSize);
    //
    // ...however, in case of multipacket messages we must set non-zero timeout anyway to allow code to retrieve
    // whole message because follow-up packets will always have some small delay
    //
    if (!MessageComplete) {
      Timeout = MAX_PSE_HECI_TIMEOUT;
    }
    *Length = BufferSize - BytesCopied;
    Status = PseHeciReceivePacket (Timeout, MessageBody, Length, &BytesInPacket, &MessageComplete);
    //
    // Advance pointer and keep counts how many bytes were copied into buffer and how big was the whole message
    // In case of BUFFER_TOO_SMALL those will not be equal
    //
    MessageBody += *Length / 4; // pointer arithmetic for UINT32*
    BytesCopied += *Length;
    TotalMsgLength += BytesInPacket;
  } while (!MessageComplete);
  *Length = TotalMsgLength;
  DEBUG ((DEBUG_INFO, "Done, length = %d\n",TotalMsgLength));
  return Status;
}

/**
  Sends a HECI message to PSE. Further modified to support multi package sending even if the message length exceeds 128 bytes.

  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MeAddress            Address of the subsystem the message is being sent to.

  @retval EFI_SUCCESS             Message sent
  @retval EFI_UNSUPPORTED         Message is too big to send as single packet; multipacket messages not supported in BIOS yet
  @retval any other status        Error caused by PSE firmware
**/
EFI_STATUS
PseHeciSend (
  IN UINT32                       *Message,
  IN UINT32                       Length,
  IN UINT8                        HostAddress,
  IN UINT8                        MeAddress
  )
{
  HECI_MESSAGE_HEADER  MsgHeader;
  PSE_DOORBELL         Doorbell;
  EFI_STATUS           Status;
  UINT32               DoneBytes = 0;
  UINT32               FragmentSize = 0;

  DEBUG ((DEBUG_INFO, "PseHeciSend\n"));
  Status = PseHeciFirmwareInit();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "TIMEOUT\n"));
    return EFI_TIMEOUT;
  }
  Status = WaitForHost2PseClear (MAX_PSE_HECI_TIMEOUT);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "wait for host2ose clear, failed with %x\n", Status));
    return Status;
  }
  Doorbell.Data = 0;
  Doorbell.Fields.Protocol = IPC_PROTOCOL_HECI;
  Doorbell.Fields.Busy = 1;
  MsgHeader.Data = 0;
  MsgHeader.Fields.MeAddress = MeAddress;
  MsgHeader.Fields.HostAddress = HostAddress;
  MsgHeader.Fields.MessageComplete = 0;

  DEBUG ((DEBUG_INFO, "PseHeciSend Doorbell %08x Header %08x\n", Doorbell.Data, MsgHeader.Data));
  if (Length <= HECI_MAX_PAYLOAD_SIZE) {
    MsgHeader.Fields.Length = Length;
    MsgHeader.Fields.MessageComplete = 1;
    Doorbell.Fields.Length = Length + sizeof(HECI_MESSAGE_HEADER);
    DEBUG ((DEBUG_INFO, "Length within %d bytes. Sending in 1 package...\n", HECI_MAX_PAYLOAD_SIZE));
    MmioWrite32 ((UINTN) (gPseAddr + R_PSE_MEM_HOST2PSE_MSG), MsgHeader.Data);
    CopyMem ((UINT32*)((UINTN)(gPseAddr + R_PSE_MEM_HOST2PSE_MSG + 4)), Message, Length);
    MmioWrite32 ((UINTN) (gPseAddr + R_PSE_MEM_HOST2PSE_DRBL), Doorbell.Data);

    Status = WaitForHost2PseClear (MAX_PSE_HECI_TIMEOUT);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "wait for host2ose clear, failed with %x\n", Status));
      return Status;
    }
    Status = PseWaitForCompletion (MAX_PSE_HECI_TIMEOUT);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "wait for host2ose clear, failed with %x\n", Status));
      return Status;
    }
  } else {
    DEBUG ((DEBUG_INFO, "Length exceeds %d bytes, splitting..\n", HECI_MAX_PAYLOAD_SIZE));

    while (DoneBytes < Length) {
      FragmentSize = MIN(Length - DoneBytes, HECI_MAX_PAYLOAD_SIZE);

      DEBUG ((DEBUG_INFO, "Address : %p\n", (UINT8*)Message + DoneBytes));
      CopyMem ((UINT32*)((UINTN)(gPseAddr + R_PSE_MEM_HOST2PSE_MSG + 4)),
              (UINT8*)Message + DoneBytes, FragmentSize);
      DoneBytes += FragmentSize;

      DEBUG ((DEBUG_INFO, "FragmentSize : %x\n", FragmentSize));
      DEBUG ((DEBUG_INFO, "DoneBytes : %x\n", DoneBytes));
      DEBUG ((DEBUG_INFO, "MsgHeader.Data : %x\n", MsgHeader.Data));

      MsgHeader.Fields.Length = FragmentSize;
      MsgHeader.Fields.MessageComplete = (DoneBytes == Length) ? 1 : 0;
      Doorbell.Fields.Length = FragmentSize + sizeof(HECI_MESSAGE_HEADER);

      MmioWrite32 ((UINTN) (gPseAddr + R_PSE_MEM_HOST2PSE_MSG), MsgHeader.Data);
      MmioWrite32 ((UINTN) (gPseAddr + R_PSE_MEM_HOST2PSE_DRBL), Doorbell.Data);

      Status = WaitForHost2PseClear (MAX_PSE_HECI_TIMEOUT);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "WaitForHost2PseClear failed with %x\n", Status));
        return Status;
      }
      Status = PseWaitForCompletion (MAX_PSE_HECI_TIMEOUT);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "PseWaitForCompletion failed with %x\n", Status));
        return Status;
      }
    }
    DEBUG ((DEBUG_INFO, "Sending done.\n"));
  }

  return EFI_SUCCESS;
}

/**
  Function sends one message through the PSE HECI and waits
  for the corresponding response message

  @param[in,out] Message          Pointer to the message buffer.
  @param[in] Length               Length of the message in bytes.
  @param[in,out] RecLength        Length of the message response in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @retval EFI_SUCCESS             Message sent and response received
  @retval EFI_UNSUPPORTED         Message is too big to send as single packet; sending multipacket messages not supported yet
  @retval any other status        Error caused by PSE firmware
**/
EFI_STATUS
PseHeciSendAndReceive (
  IN OUT  UINT32                  *Message,
  IN      UINT32                  Length,
  IN OUT  UINT32                  *RecLength,
  IN      UINT8                   HostAddress,
  IN      UINT8                   MeAddress
  )
{
  EFI_STATUS Status;

  DEBUG ((DEBUG_INFO, "PSE HECI send start\n"));
  Status = PseHeciSend (Message, Length, HostAddress, MeAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "sending failed. Status: %x\n", Status));
    return Status;
  } else {
    DEBUG ((DEBUG_INFO, "sending passed\n"));
  }
  DEBUG ((DEBUG_INFO, "PSE HECI receive start\n"));
  return PseHeciReceive (BLOCKING, Message, RecLength);
}
