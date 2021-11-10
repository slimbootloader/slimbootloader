/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/IoLib.h>

#define  MAILBOX_CTRL_TXDV      BIT0
#define  MAILBOX_CTRL_RXDA      BIT1
#define  MAILBOX_CTRL_TOGGLE    BIT7
#define  MAILBOX_DATA_TX_SHIFT  8
#define  MAILBOX_DATA_RX_SHIFT  16

/**
  Write data from buffer to debug port.

  Writes NumberOfBytes data bytes from Buffer to the debug port.
  The number of bytes actually written to the debug port is returned.
  If the return value is less than NumberOfBytes, then the write operation failed.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to written to the debug port.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes written to the debug port.
**/
UINTN
EFIAPI
MailboxDebugPortWrite (
  IN UINT8    *Buffer,
  IN UINTN    NumberOfBytes
  )
{
  UINT32  Data;

  if (NULL == Buffer) {
    return 0;
  }

  if (PcdGet32 (PcdMailboxDebugPortMmioBase) == 0) {
    return 0;
  }

  while (NumberOfBytes--) {
    // Wait for the serail port to be ready.
    do {
      Data = MmioRead32 (PcdGet32 (PcdMailboxDebugPortMmioBase));
    } while ((Data & MAILBOX_CTRL_TXDV) != 0);

    Data = ((Data ^ MAILBOX_CTRL_TOGGLE) & ~(0xFF << MAILBOX_DATA_TX_SHIFT)) | MAILBOX_CTRL_TXDV | (*Buffer++ << MAILBOX_DATA_TX_SHIFT);
    MmioWrite32 (PcdGet32 (PcdMailboxDebugPortMmioBase), Data);
  }

  return NumberOfBytes;
}

/**
  Reads data from a debug port into a buffer.

  @param  Buffer           Pointer to the data buffer to store the data read from the debug port.
  @param  NumberOfBytes    Number of bytes to read from the debug port.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes read from the debug port.
**/
UINTN
EFIAPI
MailboxDebugPortRead (
  OUT UINT8 *Buffer,
  IN UINTN  NumberOfBytes
  )
{
  UINT32  Data;

  if (NULL == Buffer) {
    return 0;
  }

  if (PcdGet32 (PcdMailboxDebugPortMmioBase) == 0) {
    return 0;
  }

  while (NumberOfBytes--) {
    //
    // Wait for the serial port to be ready.
    //
    do {
      Data = MmioRead32 (PcdGet32 (PcdMailboxDebugPortMmioBase));
    } while ((Data & MAILBOX_CTRL_RXDA) == 0);

    *Buffer++ = (UINT8)(Data >> MAILBOX_DATA_RX_SHIFT);
    Data = Data & ~MAILBOX_CTRL_RXDA;
    MmioWrite32 (PcdGet32 (PcdMailboxDebugPortMmioBase), Data);
  }

  return NumberOfBytes;
}

/**
  Polls a debug port to see if there is any data waiting to be read.

  Polls adebug port to see if there is any data waiting to be read.
  If there is data waiting to be read from the debug port, then TRUE is returned.
  If there is no data waiting to be read from the debug port, then FALSE is returned.

  @retval TRUE             Data is waiting to be read from the debug port.
  @retval FALSE            There is no data waiting to be read from the debug port.
**/
BOOLEAN
EFIAPI
MailboxDebugPortPoll (
  VOID
  )
{
  UINT32  Data32;

  if (PcdGet32 (PcdMailboxDebugPortMmioBase) == 0) {
    return FALSE;
  }

  Data32 = MmioRead32 (PcdGet32 (PcdMailboxDebugPortMmioBase));
  if ((Data32 & MAILBOX_CTRL_RXDA) != 0) {
    return TRUE;
  }

  return FALSE;
}
