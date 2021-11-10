/** @file
  IOC IPC library

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IocIpcLib.h>
#include <Library/DebugLib.h>
#include <Library/Crc32Lib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/TimerLib.h>
#include <Library/BootloaderCommonLib.h>

#define ABL_READY             0x02  // ABL: ready to receive datagram
#define INVALID_DATAGRAM      0x15 // ABL: received frame had CRC error
#define TIMEOUT_DATAGRAM      0xD4 // ABL: time-out during receive
#define ABORT_DATAGRAM        0xD6 // ABL: max. number of retries exceeded
#define VALID_DATAGRAM        0x06 // ABL: frame received and accepted
#define SOD                   0x12 // IOC: frame-start character

#define R_UART_RXBUF          0
#define R_UART_TXBUF          0
#define R_UART_BAUD_THR       0
#define R_UART_BAUD_LOW       0
#define R_UART_BAUD_HIGH      1
#define R_UART_FCR            2
#define   B_UART_FCR_TRFIFIE  BIT0
#define   B_UART_FCR_RESETRF  BIT1
#define   B_UART_FCR_RESETTF  BIT2
#define   B_UART_FCR_FIFO64   BIT5
#define R_UART_LCR            3
#define   B_UART_LCR_DLAB     BIT7
#define R_UART_MCR            4
#define   B_UART_MCR_DTRC     BIT0
#define   B_UART_MCR_RTS      BIT1
#define R_UART_LSR            5
#define   B_UART_LSR_RXRDY    BIT0
#define   B_UART_LSR_TXRDY    BIT5
#define   B_UART_LSR_TEMT     BIT6
#define R_UART_MSR            6
#define   B_UART_MSR_CTS      BIT4
#define   B_UART_MSR_DSR      BIT5
#define   B_UART_MSR_RI       BIT6
#define   B_UART_MSR_DCD      BIT7

#define CC                    1  // Control Characters size

/**
  The function reads value from a register.

  @param[in]  IpcDeviceUart   uart ipc device
  @param[in]  Offset          register offset

  @retval register value

**/
STATIC
UINT8
UartReadReg (
  IOC_IPC_UART_DEVICE *IpcDeviceUart,
  UINTN               Offset
  )
{
  return MmioRead8 (IpcDeviceUart->BaseAddr + Offset * IpcDeviceUart->RegWidth);
}

/**
  The function write value to a register.

  @param[in]  IpcDeviceUart   uart ipc device
  @param[in]  Offset          register offset
  @param[in]  Value           value to be written

**/
STATIC
VOID
UartWriteReg (
  IOC_IPC_UART_DEVICE *IpcDeviceUart,
  UINTN               Offset,
  UINT8               Value
  )
{
  MmioWrite8 (IpcDeviceUart->BaseAddr + Offset * IpcDeviceUart->RegWidth, Value);
}

/**
  This function resets IPC transmission

  @param[in]  IpcDevice     Specifies Ipc Interface Device

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
EFIAPI
IpcResetUart (
  IN  IOC_IPC_DEVICE    *IpcDevice
  )
{
  IOC_IPC_UART_DEVICE   *IpcDeviceUart;

  IpcDeviceUart = (IOC_IPC_UART_DEVICE *)IpcDevice;

  UartWriteReg (IpcDeviceUart, R_UART_FCR, 0);
  UartWriteReg (IpcDeviceUart, R_UART_FCR, \
                (B_UART_FCR_TRFIFIE | B_UART_FCR_RESETRF | B_UART_FCR_RESETTF));

  return EFI_SUCCESS;
}

/**
  This function initializes IPC interface

  @param[in]  IpcDevice     Specifies Ipc Interface Device

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
EFIAPI
IpcInitUart (
  IN IOC_IPC_DEVICE   *IpcDevice
  )
{
  IOC_IPC_UART_DEVICE *IpcDeviceUart;
  UINT16              Divisor;

  IpcDeviceUart       = (IOC_IPC_UART_DEVICE *)IpcDevice;

  UartWriteReg (IpcDeviceUart, R_UART_LCR, 0x3 | B_UART_LCR_DLAB);

  //
  // baud rate
  //
  Divisor = (UINT16) (IpcDeviceUart->InputHertz / (IpcDeviceUart->BaudRate * 16));

  UartWriteReg (IpcDeviceUart, R_UART_BAUD_HIGH, (UINT8) (Divisor >> 8));
  UartWriteReg (IpcDeviceUart, R_UART_BAUD_LOW,  (UINT8) Divisor);

  //
  // switch back to bank 0
  //
  UartWriteReg (IpcDeviceUart, R_UART_LCR, 0x3);

  return EFI_SUCCESS;
}

/**
  This function sends a single character via IPC interface

  @param[in]  IpcDevice     Specifies Ipc Interface Device
  @param[in]  Ch            Specifies a character to be sent

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
EFIAPI
IpcSendUart (
  IN  IOC_IPC_DEVICE  *IpcDevice,
  IN  UINT8           Ch
  )
{
  IOC_IPC_UART_DEVICE *IpcDeviceUart;

  IpcDeviceUart       = (IOC_IPC_UART_DEVICE *)IpcDevice;

  //
  // wait for THR empty and line is idle:
  //
  while ((UartReadReg (IpcDeviceUart, R_UART_LSR) & B_UART_LSR_TEMT) == 0);
  UartWriteReg (IpcDeviceUart, R_UART_BAUD_THR, Ch);

  return EFI_SUCCESS;
}

/**
  This function receives data via IPC interface

  @param[in]  IpcDevice     Specifies Ipc Interface Device
  @param[out] Buffer        Memory address to store the received data
  @param[in]  Len           Length of characters to be received
  @param[in]  MilliSec      Timeout in millisecond

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
EFI_STATUS
EFIAPI
IpcReceiveUart (
  IN  IOC_IPC_DEVICE    *IpcDevice,
  OUT CHAR8             *Buffer,
  IN  UINT32            Len,
  IN  UINT32            MilliSec
  )
{
  IOC_IPC_UART_DEVICE   *IpcDeviceUart;
  UINT32                Pos;
  UINT32                Lsr;
  UINT32                Timeout;

  IpcDeviceUart         = (IOC_IPC_UART_DEVICE *)IpcDevice;
  MilliSec              = MAX (MilliSec, 1);
  Timeout               = MilliSec * 1000;
  Pos                   = 0;

  do {
    // ignore transmit status bits
    Lsr  = UartReadReg (IpcDeviceUart, R_UART_LSR);
    Lsr &= ~ (B_UART_LSR_TXRDY | B_UART_LSR_TEMT);

    // any errorrs?
    if ((Lsr & ~B_UART_LSR_RXRDY) != 0) {
      DEBUG ((DEBUG_VERBOSE, "\nLsr: 0x%x, Len: %d, Pos: %d\n", Lsr, Len, Pos));

      // yes, drain Rx FIFO
      do {
        UartReadReg (IpcDeviceUart, R_UART_BAUD_THR);
        Lsr = UartReadReg (IpcDeviceUart, R_UART_LSR);
      } while ((Lsr & ~ (B_UART_LSR_TXRDY | B_UART_LSR_TEMT)) != 0);

      return EFI_NOT_STARTED;
    }

    // character available
    if (Lsr != 0) {
      Buffer[Pos++] = UartReadReg (IpcDeviceUart, R_UART_BAUD_THR);

      // got character, restart timer.
      Timeout = MilliSec * 1000;
    }

    // frame complete or time-out
    if (Pos >= Len) {
      return EFI_SUCCESS;
    }

    MicroSecondDelay (1);
    Timeout--;
  } while (Timeout > 0);

  return EFI_TIMEOUT;
}

CONST CHAR8 mColdResetCmd[] = {
  0x05, 0x00, 0x0E, 0x02, 0x00, 0x02, 0x00, 0xE9
};

CONST IOC_IPC_FUNC mIocIpcFuncs[IocIpcDeviceTypeMax] = {
  {
    IpcInitUart,
    IpcSendUart,
    IpcReceiveUart,
    IpcResetUart
  }
};

/**
  This function receives whole data from IOC

  @param[in]  IpcConfig     IPC config information
  @param[out] Buffer        Memory address to store the received data
  @param[out] ReadBytes     Length of characters received

  @retval EFI_SUCCESS       The operation is done correctly.
  @retval Others            The operation fails.

**/
STATIC
EFI_STATUS
RunProtocol (
  IN  IOC_IPC_CONFIG_DATA   *IpcConfig,
  OUT CHAR8                 *Buffer,
  OUT UINT32                *ReadBytes
  )
{
  IOC_IPC_FUNC          *IocIpcFunc;
  EFI_STATUS            Status;
  UINT32                Retries;
  UINT32                Length;
  UINT32                CrcOut;

  IocIpcFunc = (IOC_IPC_FUNC *)&mIocIpcFuncs[IpcConfig->IpcDeviceType];

  ASSERT (IocIpcFunc->IpcSend != NULL);
  ASSERT (IocIpcFunc->IpcReceive != NULL);

  if (IocIpcFunc->IpcInitialize != NULL) {
    IocIpcFunc->IpcInitialize (&IpcConfig->IpcDevice);
  }

  Retries = 0;
  do {
    // Enable & Reset receive and transmit FIFO
    if (IocIpcFunc->IpcReset != NULL) {
      IocIpcFunc->IpcReset (&IpcConfig->IpcDevice);
    }

    // Send ABL READY FLAG
    IocIpcFunc->IpcSend (&IpcConfig->IpcDevice, ABL_READY);

    do {
      Status = IocIpcFunc->IpcReceive (&IpcConfig->IpcDevice, &Buffer[0], CC, IpcConfig->IpcMisc.TimeoutInitial);
      if (Status == EFI_NOT_STARTED) {
      } else if (Status == EFI_TIMEOUT) {
        return Status;
      }
    } while (Buffer[0] != SOD);

    // Receive IPC Frame length within handshake timeout
    Status = IocIpcFunc->IpcReceive (&IpcConfig->IpcDevice, &Buffer[1], CC, IpcConfig->IpcMisc.TimeoutInitial);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, " Transmission timeout reached\n"));
      goto timeout;
    }
    Length = Buffer[1];
    DEBUG ((DEBUG_VERBOSE, "len=0x%X\n", Length));

    // Receive IPC frame within transmission timeout
    Status = IocIpcFunc->IpcReceive (&IpcConfig->IpcDevice, &Buffer[2], Length - 2, IpcConfig->IpcMisc.TimeoutXmit);
    if ((Length > 2) && (EFI_ERROR (Status))) {
      goto timeout;
    }

    // Check if IPC frame is valid
    CrcOut = 0xFF;
    CalculateCrc32WithType ((UINT8 *)Buffer, Length, Crc32TypeCastagnoli, &CrcOut);
    if ((Length >= 6) && (CrcOut == 0)) {
      IocIpcFunc->IpcSend (&IpcConfig->IpcDevice, VALID_DATAGRAM);
      *ReadBytes = Length;
      return EFI_SUCCESS;
    } else { // CRC error (no need to check for short frame)
      DEBUG ((DEBUG_ERROR, " Length %d, CRC 0x%02X\n", Length, CrcOut));
      IocIpcFunc->IpcSend (&IpcConfig->IpcDevice, INVALID_DATAGRAM);
      continue; // Retry
    }

timeout:
    // Transmission timeout reached
    IocIpcFunc->IpcSend (&IpcConfig->IpcDevice, TIMEOUT_DATAGRAM);
  } while ((++Retries) < IpcConfig->IpcMisc.Retries);

  // Max count of retries exceeded
  IocIpcFunc->IpcSend (&IpcConfig->IpcDevice, ABORT_DATAGRAM);

  *ReadBytes = 0;
  return EFI_TIMEOUT;
}

/**
  The function gets config data from IOC.

  Based on IpcConfig, this function will query config data from IOC.

  @param[in]  IpcConfig   IPC config information
  @param[out] Buffer      Memory address to store the received data

  @retval The number of bytes received from IOC

**/
UINT32
EFIAPI
QueryConfigDataFromIoc (
  IN  IOC_IPC_CONFIG_DATA   *IpcConfig,
  OUT CHAR8                 *Buffer
  )
{
  EFI_STATUS            Status;
  UINT32                ReadBytes;

  ASSERT (IpcConfig->IpcDeviceType < IocIpcDeviceTypeMax);

  ReadBytes = 0;
  Status = RunProtocol (IpcConfig, Buffer, &ReadBytes);
  if (EFI_ERROR (Status)) {
    ReadBytes = 0;
  }
  return ReadBytes;
}

/**
  This will trigger system reset via IOC.

  @param[in]  IpcConfig   IPC config information
  @param[in]  ResetType   Cold, Warm or Shutdown

**/
VOID
RequestResetSystemToIoc (
  IN  IOC_IPC_CONFIG_DATA  *IpcConfig,
  IN  EFI_RESET_TYPE        ResetType
  )
{
  IOC_IPC_FUNC         *IocIpcFunc;
  UINT32                Index;
  UINT32                Length;

  ASSERT (IpcConfig->IpcDeviceType < IocIpcDeviceTypeMax);

  IocIpcFunc = (IOC_IPC_FUNC *)&mIocIpcFuncs[IpcConfig->IpcDeviceType];

  if (IocIpcFunc->IpcInitialize != NULL) {
    IocIpcFunc->IpcInitialize (&IpcConfig->IpcDevice);
  }

  if (IocIpcFunc->IpcReset != NULL) {
    IocIpcFunc->IpcReset (&IpcConfig->IpcDevice);
  }

  Length = ARRAY_SIZE (mColdResetCmd);
  for (Index = 0; Index < Length; Index++) {
    IocIpcFunc->IpcSend (&IpcConfig->IpcDevice, mColdResetCmd[Index]);
  }
}
