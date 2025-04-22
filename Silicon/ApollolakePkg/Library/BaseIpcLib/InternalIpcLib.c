/** @file
  Base IPC library implementation.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <Library/InternalIpcLib.h>
#include <Library/IoLib.h>

/**
  Sends an IPC without waiting.

  @param[in]      BaseAddress  The base address of the IPC register bank
  @param[in]      Command      The message identifier to send in the IPC packet.
  @param[in out]  Buffer       Pointer to buffer associated with MessageId.  This is an optional
                               patrameter that may be NULL for some MessageId values.
                               Note: This buffer can be used for both Input/Output buffer at the same time
                               Whenever used as Output buffer to receive return value, it must be 1 or more DWs.
  @param[in out]  BufferSize   The size, in bytes, of Buffer.  Ignored if Buffer is NULL.

  @retval         None

**/
VOID
InternalIpcLibSendCommandWithoutWaiting (
  IN  UINTN      BaseAddress,
  IN  UINT32     Command,
  IN OUT  VOID   *Buffer,      OPTIONAL
  IN OUT  UINTN  BufferSize
  )
{
  MmioWrite32 (BaseAddress + R_IPC_CMD_OFFSET, Command);
}


/**
  Sends an IPC without waiting.

  @param[in]      BaseAddress     The base address of the IPC register bank
  @param[in]      Command         The Command identifier to send in the IPC packet.
  @param[in]      CommandType     The Command type to send in the IPC packet.
  @param[in]      ReturnDataSize  Return Data size.
  @param[in out]  Buffer          Pointer to buffer associated with MessageId.  This is an optional
                                  patrameter that may be NULL for some MessageId values.
                                  Note: This buffer can be used for both Input/Output buffer at the same time
                                  Whenever used as Output buffer to receive return value, it must be 1 or more DWs.
  @param[in out]  BufferSize      The size, in bytes, of Buffer.  Ignored if Buffer is NULL.

  @retval         IPC_STATUS.Uint32

**/
UINT32
InternalIpcLibSendCommandAndWait (
  IN  UINTN           BaseAddress,
  IN  UINT32          Command,
  IN  IPC_CMD_TYPE    CommandType,
  IN  UINT32          ReturnDataSize,
  IN  OUT  VOID       *Buffer,      OPTIONAL
  IN  OUT  UINTN      BufferSize
  )
{
  UINTN        Index;
  IPC_STATUS   IpcStatus;

  MmioWrite32 (BaseAddress + R_IPC_CMD_OFFSET, Command);
  do {
    IpcStatus.Uint32 = MmioRead32 (BaseAddress + R_IPC_STS_OFFSET);
  } while (IpcStatus.Bits.Busy == 1);

  if ((IpcStatus.Bits.Error == 0) && (CommandType ==IPC_CMD_READ)) {
    if (Buffer != NULL && BufferSize != 0) {
      for (Index = 0; Index < ReturnDataSize; Index++ ){
        *((UINT8 *) Buffer + Index) = MmioRead8 (BaseAddress + R_IPC_RBUF_OFFSET + Index);
      }
    }
  }
  return IpcStatus.Uint32;
}


/**
  Checks the IPC channel is available for a new request

  @param[in]  BaseAddress       The base address of the IPC register bank.

  @retval     RETURN_SUCCESS    Ready for a new IPC.
  @retval     RETURN_NOT_READY  Not ready for a new IPC.

**/
RETURN_STATUS
InternalReadyForNewIpc (
  IN      UINTN   BaseAddress
  )
{
  IPC_STATUS   IpcStatus;

  IpcStatus.Uint32 = MmioRead32 (BaseAddress + R_IPC_STS_OFFSET);
  if (IpcStatus.Bits.Busy == 1) {
    return RETURN_NOT_READY;
  } else {
    return RETURN_SUCCESS;
  }
}


/**
  Sends an IPC from the x86 CPU to the PMC and waits for the PMC to process the
  specified opcode.

  @param[in]       BaseAddress  The base address of the IPC register bank
  @param[in]       Command      The Command identifier to send in the IPC packet.
  @param[in]       SubCommand   The SubCommand identifier to send in the IPC packet.
  @param[in, out]  Buffer       Pointer to buffer associated with MessageId.  This is an optional
                                patrameter that may be NULL for some MessageId values.
                                Note: This buffer can be used for both Input/Output buffer at the same time
                                Whenever used as Output buffer to receive return value, it must be 1 or more DWs.
  @param[in,out]  BufferSize   The size, in bytes, of Buffer.  Ignored if Buffer is NULL.

  @retval         IPC_STATUS.Uint32

**/
UINT32
InternalIpcSendCommandEx (
  IN      UINTN   BaseAddress,
  IN      UINT32  Command,
  IN      UINT8   SubCommand,
  IN OUT  VOID    *Buffer,  OPTIONAL
  IN OUT  UINTN   BufferSize
  )
{
  UINT8             IpcCommand;
  UINT32            CommandSize;
  UINT32            ReturnDataSize;
  UINT32            BufferData;
  UINT32            tempBufferData;
  UINTN             Index;
  UINTN             OffsetIndex;
  IPC_STATUS        IpcStatus;
  IPC_CMD_TYPE      CmdType;
  UINT32            tempCommand = 0;
  BOOLEAN           State;

  IpcStatus.Uint32   = 0;
  IpcCommand = (UINT8) Command;
  ReturnDataSize = 0;

  //
  // Make sure interrupts are disabled
  //
  State = SaveAndDisableInterrupts ();
  switch (IpcCommand) {
    case IPC_CMD_ID_USB3P3:
    case IPC_CMD_ID_BLACKLIST_SEL:
    case IPC_CMD_ID_SOUTH_IP_UNGATE:
    case IPC_CMD_ID_CAPSULE_UPDATE_RESET:
      CmdType = IPC_CMD_NONE;
      CommandSize = 0;
      ReturnDataSize = 0;
      break;

    case IPC_CMD_ID_FW_MSG_CTRL:
      switch (SubCommand) {
        case IPC_SUBCMD_ID_FW_MSG_CTRL_READ:
          CmdType = IPC_CMD_READ;
          ReturnDataSize = sizeof (UINT8);
          CommandSize = 0;
          break;

        case IPC_SUBCMD_ID_FW_MSG_CTRL_WRITE:
          CmdType = IPC_CMD_WRITE;
          CommandSize = 1;
          break;

        default:
          return INVALID_IPC_CMD;
          break;
      }
      break;

    case IPC_CMD_ID_PMC_VER:
      switch (SubCommand) {
        case IPC_SUBCMD_ID_PMC_VER_READ:
          CmdType = IPC_CMD_READ;
          ReturnDataSize = sizeof (UINT8) * 8;
          CommandSize = 0;
          break;

        default:
          return INVALID_IPC_CMD;
          break;
      }
      break;

    case IPC_CMD_ID_PHY_CFG:
      CmdType = IPC_CMD_WRITE;
      CommandSize = 1;
      break;

    case IPC_CMD_ID_PM_DEBUG:
      switch (SubCommand) {
        case IPC_SUBCMD_ID_LPSS_DEBUG_UART_ENABLE:
          CmdType = IPC_CMD_WRITE;
          CommandSize = 0;
          break;

        case IPC_SUBCMD_ID_LPSS_DEBUG_UART_DISABLE:
          CmdType = IPC_CMD_WRITE;
          CommandSize = 0;
          break;

        case IPC_SUBCMD_ID_PM_DEBUG_LTR_IGNORE_REPORT:
          CmdType = IPC_CMD_READ;
          CommandSize = 1;
          ReturnDataSize = sizeof (UINT32);
          break;

        case IPC_SUBCMD_ID_PM_DEBUG_LTR_IGNORE_READ:
           CmdType = IPC_CMD_READ;
           CommandSize = 0;
           ReturnDataSize = sizeof (UINT16);
          break;

        case IPC_SUBCMD_ID_PM_DEBUG_LTR_IGNORE_WRITE:
          CmdType = IPC_CMD_WRITE;
          CommandSize = 2;
          break;
        default:
          return INVALID_IPC_CMD;
      }
      break;

    case IPC_CMD_ID_PMIC:
       //
       // PMIC will read data is byte size.
       //
      switch (SubCommand) {
        case IPC_SUBCMD_ID_PMIC_READ:
          CmdType = IPC_CMD_READ;
          CommandSize = 2;
          ReturnDataSize = (CommandSize / 2);
          break;

        case IPC_SUBCMD_ID_PMIC_WRITE:
          CmdType = IPC_CMD_WRITE;
          CommandSize = 3;
          ReturnDataSize = (CommandSize / 3);
          break;

        case IPC_SUBCMD_ID_PMIC_READ_MOD_WRITE:
          CmdType = IPC_CMD_READ;
          CommandSize = 4;
          ReturnDataSize = (CommandSize / 4);

          break;
        default:
          return INVALID_IPC_CMD;
      }
      break;

    case IPC_CMD_ID_TELEMETRY:
      switch (SubCommand) {
        case 0:
          CmdType = IPC_CMD_READ;
          ReturnDataSize = sizeof (UINT16);
          CommandSize = 1;
          break;

        case 2:
        case 3:
        case 5:
        case 7:
          CmdType = IPC_CMD_READ;
          ReturnDataSize = sizeof (UINT32);
          CommandSize = 0;
          break;

        case 1:
          CommandSize = 1;
          CmdType = IPC_CMD_WRITE;
          break;

        case 4:
        case 6:
        case 8:
          CommandSize = 4;
          CmdType = IPC_CMD_WRITE;
          break;

        default:
          return INVALID_IPC_CMD;
      }
      break;

    //
    // EMI/RFI support
    // Note that this command is not valid on BXT-A
    //
    case IPC_CMD_ID_EMI_RFI_SUPPORT:
      CmdType = IPC_CMD_WRITE;
      CommandSize = 16;
      ReturnDataSize = 0;
      break;

    default:
      return INVALID_IPC_CMD;
  }

  if (CmdType == IPC_CMD_READ) {
    if ((ReturnDataSize > BufferSize) || (NULL == Buffer)) {  //cmd 2, 3, 5, 7 Read 4 bytes data.
      return INVALID_IPC_CMD;
    }
  }

  tempCommand |= (SubCommand << 12);
  tempCommand |= (CommandSize << 16);
  tempCommand |= Command;

  //
  // Send data to R_IPC_WBUF_OFFSET.
  //
  if (CommandSize > 0){
    BufferData = 0;

    for (Index = 0; Index < CommandSize; Index++) {
      tempBufferData = *(((UINT8 *) Buffer + Index));
      OffsetIndex = Index % 4;
      BufferData |= (tempBufferData << (OffsetIndex * 8));

      if (OffsetIndex == 3) {
        MmioWrite32 (BaseAddress + R_IPC_WBUF_OFFSET + (Index / 4 * 4), BufferData);
        BufferData = 0;
      } else {
        if (Index == (CommandSize - 1)){
          MmioWrite32 (BaseAddress + R_IPC_WBUF_OFFSET + (Index / 4 * 4), BufferData);
        }
      }
    }
  }

  IpcStatus.Uint32 = InternalIpcLibSendCommandAndWait (BaseAddress, tempCommand, CmdType, ReturnDataSize, Buffer, BufferSize);    // Returned 4 bytes (max) is stored in Buffer

  if (IpcStatus.Bits.Error != 0) {
  } else {
    if (CmdType == IPC_CMD_READ) {
      for (Index = 0; Index < ReturnDataSize; Index++) {
      }
    }
  }

  //
  // Restore the original interrupt state
  //
  SetInterruptState (State);

  return IpcStatus.Uint32;
}

