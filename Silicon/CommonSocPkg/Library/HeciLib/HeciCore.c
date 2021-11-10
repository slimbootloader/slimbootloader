/** @file
  Heci driver core, determines the HECI device and initializes it.

  Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Library/HeciLib.h>
#include <IndustryStandard/Pci.h>
#include "HeciRegs.h"

#define HECI_DEBUG_LEVEL          DEBUG_VERBOSE

#define HECI_CB_OVERFLOW          0xFFFFFFFF  // Circular buffer overflow

#define HECI_WAIT_DELAY           1000        // 1ms timeout for IO delay
#define HECI_READ_TIMEOUT         5000000     // 5sec timeout in microseconds
#define HECI_SEND_TIMEOUT         5000000     // 5sec timeout in microseconds
#define HECI_INIT_TIMEOUT         15000000    // 15sec timeout in microseconds
#define HECI_TIMEOUT_COUNT(t)     (((t) + HECI_WAIT_DELAY - 1) / HECI_WAIT_DELAY)

/**
  Return number of filled slots in HECI circular buffer.
  Corresponds to HECI HPS (part of) section 4.2.1

  @param[in] Offset        Offset to Host/Me Control Status Register

  @retval FilledSlots      Number of filled slots in circular buffer
  @retval HECI_CB_OVERFLOW Circular buffer overflow has occured
**/
STATIC
UINT32
GetFilledSlots (
  IN HECI_CONTROL_STATUS_REGISTER ControlStatusRegister
  )
{
  UINT8 FilledSlots;

  FilledSlots = (INT8)ControlStatusRegister.Fields.CBWritePointer - (INT8)ControlStatusRegister.Fields.CBReadPointer;

  return (FilledSlots > (UINT8)ControlStatusRegister.Fields.CBDepth) ? HECI_CB_OVERFLOW : FilledSlots;
}

/**
  This function provides a standard way to verify the HECI cmd and MBAR regs
  in its PCI cfg space are setup properly and that the local mHeciContext
  variable matches this info.

  @param[in] HeciDev              The HECI device to be accessed.

  @retval HeciMemBar              HECI Memory BAR.
                                  0 means invalid BAR value returned.
**/
STATIC
UINTN
CheckAndFixHeciForAccess (
  IN HECI_DEVICE                  HeciDev
  )
{
  UINTN   HeciMmPciAddress;
  UINT64  MemBar;

  HeciMmPciAddress = MeGetHeciMmPciAddress (HeciDev, 0);

  ///
  /// Check for HECI PCI device availability
  ///
  if (MmioRead16 (HeciMmPciAddress + PCI_DEVICE_ID_OFFSET) == 0xFFFF) {
    return 0;
  }

  MemBar = MmioRead32 (HeciMmPciAddress + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFFFF0;
  if ((MmioRead32 (HeciMmPciAddress + PCI_BASE_ADDRESSREG_OFFSET) & 0x6) == 0x4) {
    MemBar += LShiftU64 (MmioRead32 (HeciMmPciAddress + (PCI_BASE_ADDRESSREG_OFFSET + 4)), 32);
  }

  if (MemBar == 0) {
    DEBUG ((DEBUG_WARN, "CheckAndFixHeciForAccess () - MMIO Bar for HECI%d isn't programmed in this phase\n", HeciDev));
    return 0;
  }

  return (UINTN)MemBar;
}

/**
  Checks if ME is ready for communication over the HECI interface.

  @param[in] HeciMemBar HECI Memory BAR
  @param[in] Timeout    Timeout value

  @retval TRUE   ME is ready
  @retval FALSE  ME is not ready
**/
STATIC
BOOLEAN
IsMeReady (
  IN UINTN  HeciMemBar,
  IN UINT32 Timeout
  )
{
  HECI_CONTROL_STATUS_REGISTER HeciCsrMeHra;

  do {
    HeciCsrMeHra.Data = MmioRead32 (HeciMemBar + ME_CSR_HA);
    ///
    /// Check for reset first and then for ME Ready
    ///
    if (HeciCsrMeHra.Fields.Reset) {
      return FALSE;
    } else if (HeciCsrMeHra.Fields.Ready) {
      return TRUE;
    }

    MicroSecondDelay (HECI_WAIT_DELAY);
  } while (Timeout--);
  DEBUG ((DEBUG_ERROR, "HECI Interface ERROR: Timeout due to ME_RDY bit not set after %d seconds\n", Timeout));
  return FALSE;
}

/**
  Me reset and waiting for ready

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Delay                The biggest waiting time

  @retval EFI_SUCCESS             Host Ready bit cleared
  @retval EFI_TIMEOUT             Host Ready bit not cleared
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
**/
STATIC
EFI_STATUS
MeResetWait (
  IN  HECI_DEVICE              HeciDev,
  IN  UINT32                   Timeout
  )
{
  HECI_CONTROL_STATUS_REGISTER HeciCsrHost;
  UINTN                        HeciMemBar;

  ///
  /// Make sure that HECI device BAR is correct and device is enabled.
  ///
  HeciMemBar = CheckAndFixHeciForAccess (HeciDev);
  if (HeciMemBar == 0) {
    return EFI_DEVICE_ERROR;
  }

  ///
  /// Wait for the HOST Ready bit to be cleared to signal a reset
  ///
  HeciCsrHost.Data = MmioRead32 (HeciMemBar + H_CSR);
  while (HeciCsrHost.Fields.Ready == 1) {
    ///
    /// If timeout has expired, return fail
    ///
    if (Timeout-- == 0) {
      return EFI_TIMEOUT;
    }
    MicroSecondDelay (HECI_WAIT_DELAY);

    HeciCsrHost.Data = MmioRead32 (HeciMemBar + H_CSR);
  }

  return EFI_SUCCESS;
}

/**
  Function to pull one message packet off the HECI circular buffer up to its capacity.
  Corresponds to HECI HPS (part of) section 4.2.4.
  BIOS does not rely on Interrupt Status bit, since this bit can be set due to several reasons:
    a) CSME has finished reading data from H_CSR
    b) CSME has finished writing data to ME_CSR
    c) Reset has occured
  Because of above - additional checks must be conducted in order to prevent misinterpretations.

  @param[in] HeciMemBar           HECI Memory BAR.
  @param[in] Timeout              Timeout value for first Dword to appear in circular buffer.
  @param[out] MessageHeader       Pointer to message header buffer.
  @param[in] MessageData          Pointer to receive buffer.
  @param[in, out] BufferLength    On input is the size of the caller's buffer in bytes.
                                  On output is the size of the data copied to the buffer.
  @param[out] PacketSize          Size of the packet in bytes. This might be greater than buffer size.

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        The circular buffer is overflowed or transaction error.
  @retval EFI_NO_RESPONSE         The circular buffer is empty
  @retval EFI_TIMEOUT             Failed to receive a full message on time
  @retval EFI_BUFFER_TOO_SMALL    Message packet is larger than caller's buffer
**/
EFI_STATUS
HeciPacketRead (
  IN      UINTN                HeciMemBar,
  IN      UINT32               Timeout,
  OUT     HECI_MESSAGE_HEADER *MessageHeader,
  OUT     UINT32              *MessageData,
  IN OUT  UINT32              *BufferLength,
  OUT     UINT32              *PacketSize
  )
{
  EFI_STATUS                   Status;
  UINT32                       i;
  UINT32                       FilledSlots;
  UINT32                       LengthInDwords;
  UINT32                       TempBuffer;
  UINT32                       ByteCount;
  UINT32                       Length;
  HECI_CONTROL_STATUS_REGISTER HeciCsrMeHra;
  HECI_CONTROL_STATUS_REGISTER HeciCsrHost;

  *PacketSize = 0;
  Status      = EFI_SUCCESS;

  HeciCsrMeHra.Data = MmioRead32 (HeciMemBar + ME_CSR_HA);
  FilledSlots = GetFilledSlots (HeciCsrMeHra);

  ///
  /// For BLOCKING read, wait until data appears in the CB or timeout occurs
  ///
  if (Timeout != 0) {
    while (FilledSlots == 0) {
      if (MeHeciTimeoutsEnabled ()) {
        if (Timeout-- == 0) {
          return EFI_TIMEOUT;
        }
        ///
        /// Wait before we read the register again
        ///
        MicroSecondDelay (HECI_WAIT_DELAY);
      }
      HeciCsrMeHra.Data = MmioRead32 (HeciMemBar + ME_CSR_HA);
      FilledSlots = GetFilledSlots (HeciCsrMeHra);
    }
  }

  ///
  /// Check for empty and overflowed CB
  ///
  if (FilledSlots == 0) {
    *BufferLength = 0;
    return EFI_NO_RESPONSE;
  } else if (FilledSlots == HECI_CB_OVERFLOW) {
    *BufferLength = 0;
    return EFI_DEVICE_ERROR;
  }

  ///
  /// Eat the HECI Message header
  ///
  MessageHeader->Data = MmioRead32 (HeciMemBar + ME_CB_RW);
  ///
  /// Compute required message length in DWORDS
  ///
  LengthInDwords = ((MessageHeader->Fields.Length + 3) / 4);

  if (LengthInDwords > HeciCsrMeHra.Fields.CBDepth) {
    ///
    /// Make sure that the message does not overflow the circular buffer.
    ///
    *BufferLength = 0;
    return EFI_DEVICE_ERROR;
  }

  ///
  /// Wait until whole message appears in circular buffer.
  ///
  HeciCsrMeHra.Data = MmioRead32 (HeciMemBar + ME_CSR_HA);
  Timeout = HECI_TIMEOUT_COUNT (HECI_READ_TIMEOUT);
  while (GetFilledSlots (HeciCsrMeHra) < LengthInDwords) {
    if (MeHeciTimeoutsEnabled ()) {
      if (Timeout-- == 0) {
        return EFI_TIMEOUT;
      }
      MicroSecondDelay (HECI_WAIT_DELAY);
    }

    HeciCsrMeHra.Data = MmioRead32 (HeciMemBar + ME_CSR_HA);
  }

  ///
  /// Update status to signal if buffer can hold the message
  ///
  if ((MessageHeader->Fields.Length) <= *BufferLength) {
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_BUFFER_TOO_SMALL;
  }
  ///
  /// Copy as much bytes as there is space left in the message buffer.
  /// Excessive bytes will be dropped.
  ///
  ByteCount = 0;
  for (i = 0; i < LengthInDwords; i++) {
    TempBuffer = MmioRead32 (HeciMemBar + ME_CB_RW);
    Length = MIN (sizeof(TempBuffer), *BufferLength);
    CopyMem (&MessageData[i], &TempBuffer, Length);
    ByteCount += Length;
    *BufferLength -= Length;
  }
  *BufferLength = ByteCount;
  *PacketSize = MessageHeader->Fields.Length;

  ///
  /// Read ME_CSR_HA.  If the ME_RDY bit is 0, then an ME reset occurred during the
  /// transaction and the message should be discarded as bad data may have been retrieved
  /// from the host's circular buffer
  ///
  HeciCsrMeHra.Data = MmioRead32 (HeciMemBar + ME_CSR_HA);
  if (HeciCsrMeHra.Fields.Ready == 0) {
    *BufferLength = 0;
    *PacketSize   = 0;
    return EFI_NOT_READY;
  }

  ///
  /// Set Interrupt Generate bit and clear Interrupt Status bit if there are no more messages in the buffer.
  ///
  HeciCsrMeHra.Data = MmioRead32 (HeciMemBar + ME_CSR_HA);
  if (GetFilledSlots (HeciCsrMeHra) == 0) {
    HeciCsrHost.Data     = MmioRead32 (HeciMemBar + H_CSR);
    HeciCsrHost.Fields.IntGenerate = 1;
    HeciCsrHost.Fields.IntStatus = 1;
    MmioWrite32 (HeciMemBar + H_CSR, HeciCsrHost.Data);
    HeciCsrHost.Data = MmioRead32 (HeciMemBar + H_CSR);
  }

  return Status;
}

/**
  Function sends one message packet through the HECI circular buffer
  Corresponds to HECI HPS (part of) section 4.2.3

  @param[in] HeciMBAR             The HECI device BAR
  @param[in] MessageHeader        Pointer to the message header.
  @param[in] MessageData          Pointer to the actual message data.

  @retval EFI_SUCCESS             One message packet sent
  @retval EFI_DEVICE_ERROR        ME is not ready
  @retval EFI_TIMEOUT             HECI is not ready for communication
**/
STATIC
EFI_STATUS
HeciPacketWrite (
  IN  UINTN                       HeciMemBar,
  IN  HECI_MESSAGE_HEADER        *MessageHeader,
  IN  UINT32                     *MessageData
  )
{
  UINT32                       Timeout;
  UINT32                       i;
  UINT32                       LengthInDwords;
  HECI_CONTROL_STATUS_REGISTER HeciCsrHost;
  HECI_CONTROL_STATUS_REGISTER HeciCsrMeHra;

  ///
  /// Start timeout counter
  ///
  Timeout = HECI_TIMEOUT_COUNT (HECI_SEND_TIMEOUT);

  ///
  /// Compute message length in DWORDS
  ///
  LengthInDwords = ((MessageHeader->Fields.Length + 3) / 4);

  ///
  /// Wait until there is sufficient room in the circular buffer
  /// Must have room for message and message header
  ///
  HeciCsrHost.Data = MmioRead32 (HeciMemBar + H_CSR);
  while ((LengthInDwords + 1) > (HeciCsrHost.Fields.CBDepth - GetFilledSlots (HeciCsrHost))) {
    if (MeHeciTimeoutsEnabled ()) {
      if (Timeout-- == 0) {
        DEBUG ((DEBUG_ERROR, "HeciPacketWrite: EFI_TIMEOUT due to circular buffer never emptied after 5 seconds waiting"));
        return EFI_TIMEOUT;
      }
      MicroSecondDelay (HECI_WAIT_DELAY);
    }
    HeciCsrHost.Data = MmioRead32 (HeciMemBar + H_CSR);
  }
  ///
  /// Write Message Header
  ///
  MmioWrite32 (HeciMemBar + H_CB_WW, MessageHeader->Data);

  ///
  /// Write Message Body
  ///
  for (i = 0; i < LengthInDwords; i++) {
    MmioWrite32 (HeciMemBar + H_CB_WW, MessageData[i]);
  }
  ///
  /// Set Interrupt Generate bit
  ///
  HeciCsrHost.Data = MmioRead32 (HeciMemBar + H_CSR);
  HeciCsrHost.Fields.IntGenerate = 1;
  MmioWrite32 (HeciMemBar + H_CSR, HeciCsrHost.Data);

  ///
  /// Test if ME Ready bit is set to 1, if set to 0 a fatal error occured during
  /// the transmission of this message.
  ///
  HeciCsrMeHra.Data = MmioRead32 (HeciMemBar + ME_CSR_HA);
  if (HeciCsrMeHra.Fields.Ready == 0) {
    return EFI_NOT_READY;
  }

  return EFI_SUCCESS;
}

/**
  Determines if the HECI device is present and, if present, initializes it for
  use by the BIOS.

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        No HECI device
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_UNSUPPORTED         HECI MSG is unsupported
**/
EFI_STATUS
EFIAPI
HeciInitialize (
  IN HECI_DEVICE              HeciDev
  )
{
  HECI_CONTROL_STATUS_REGISTER        HeciCsrHost;
  HECI_FWS_REGISTER                   MeFirmwareStatus;
  UINTN                               HeciMemBar;
  UINTN                               HeciMmPciAddress;
  EFI_STATUS                          Status;

  DEBUG ((HECI_DEBUG_LEVEL, "HeciInitialize () - Start\n"));

  ///
  /// Check for the HECI PCI device availability
  /// Get MBAR
  ///
  HeciMemBar = CheckAndFixHeciForAccess (HeciDev);
  if (HeciMemBar == 0) {
    return EFI_DEVICE_ERROR;
  }

  HeciCsrHost.Data = MmioRead32 (HeciMemBar + H_CSR);
  ///
  /// Read H_RDY bit to check if we're already initialized
  ///
  if (HeciCsrHost.Fields.Ready == 1) {
    DEBUG ((HECI_DEBUG_LEVEL, "InitializeHeci () - H_RDY is already initialized\n"));
    return EFI_SUCCESS;
  }

  HeciMmPciAddress = MeGetHeciMmPciAddress (HeciDev, 0);
  MeFirmwareStatus.ul = MmioRead32 (HeciMmPciAddress + R_ME_HFS);
  if (MeFirmwareStatus.ul == 0 || MeFirmwareStatus.ul == 0xFFFFFFFF) {
    return EFI_DEVICE_ERROR;
  }

  ///
  /// Check for ME FPT Bad
  ///
  if (MeFirmwareStatus.r.FptBad) {
    return EFI_DEVICE_ERROR;
  }

  ///
  /// Set HECI interrupt delivery mode.
  /// HECI-1 using legacy/MSI interrupt
  ///
  MmioAnd8 (HeciMmPciAddress + R_ME_HIDM, 0xFC);

  ///
  /// Need to do following on ME init:
  ///
  ///  1) wait for ME_CSR_HA reg ME_RDY bit set
  ///
  if (!IsMeReady (HeciMemBar, HECI_TIMEOUT_COUNT (HECI_INIT_TIMEOUT))) {
    Status = HeciResetInterface (HeciDev);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  ///
  ///  2) setup H_CSR reg as follows:
  ///     a) Make sure H_RST is clear
  ///     b) Set H_RDY
  ///     c) Set H_IG
  ///
  DEBUG ((HECI_DEBUG_LEVEL, "InitializeHeci () -  Set H_RDY\n"));
  HeciCsrHost.Data  = MmioRead32 (HeciMemBar + H_CSR);
  if (HeciCsrHost.Fields.Ready == 0) {
    HeciCsrHost.Fields.Reset       = 0;
    HeciCsrHost.Fields.Ready       = 1;
    HeciCsrHost.Fields.IntGenerate = 1;
    MmioWrite32 (HeciMemBar + H_CSR, HeciCsrHost.Data);
  }

  return EFI_SUCCESS;
}

/**
  Heci Re-initialization for Host

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_SUCCESS             Heci Re-initialization successful
  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
**/
EFI_STATUS
EFIAPI
HeciReInitialize (
  IN HECI_DEVICE                  HeciDev
  )
{
  HECI_CONTROL_STATUS_REGISTER    HeciCsrHost;
  EFI_STATUS                      Status;
  UINTN                           HeciMemBar;

  HeciMemBar = CheckAndFixHeciForAccess (HeciDev);
  if (HeciMemBar == 0) {
    return EFI_DEVICE_ERROR;
  }
  DEBUG ((HECI_DEBUG_LEVEL, "HeciReInitialize..... \n"));

  Status = EFI_SUCCESS;
  ///
  /// Need to do following on ME init:
  ///
  ///  1) wait for HOST_CSR_HA reg H_RDY bit set
  ///
  ///    if (WaitForHostReady() != EFI_SUCCESS) {
  ///
  if (MeResetWait (HeciDev, HECI_TIMEOUT_COUNT (HECI_INIT_TIMEOUT)) != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }

  HeciCsrHost.Data = MmioRead32 (HeciMemBar + H_CSR);
  if (HeciCsrHost.Fields.Ready == 0) {
    Status = HeciResetInterface (HeciDev);
  }

  return Status;
}

/**
  Reads a message from the ME across HECI. This function can only be used after invoking HeciSend() first.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[out] MessageBody         Pointer to a buffer used to receive a message.
  @param[in][out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI or zero-length message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciReceive (
  IN      HECI_DEVICE  HeciDev,
  IN      UINT32       Blocking,
  OUT     UINT32      *MessageBody,
  IN OUT  UINT32      *Length
  )
{
  UINTN               HeciMemBar;
  HECI_MESSAGE_HEADER PacketHeader;
  EFI_STATUS          Status;
  UINT32              TotalLength;
  UINT32              PacketSize;
  UINT32              PacketBuffer;
  UINT32              ReadTimeout;
  UINT32              InitTimeout;

  if (MeGetIshHeciDevice() == HeciDev) {
    return EFI_UNSUPPORTED;
  }

  PacketHeader.Data = 0;
  TotalLength       = 0;
  PacketBuffer      = *Length;
  Status            = EFI_SUCCESS;

  ///
  /// Make sure that HECI device BAR is correct and device is enabled.
  ///
  HeciMemBar = CheckAndFixHeciForAccess (HeciDev);
  if (HeciMemBar == 0) {
    return EFI_DEVICE_ERROR;
  }

  if (Blocking) {
    InitTimeout = HECI_TIMEOUT_COUNT (HECI_INIT_TIMEOUT);
    ReadTimeout = HECI_TIMEOUT_COUNT (HECI_READ_TIMEOUT);
  } else {
    InitTimeout = 0;
    ReadTimeout = 0;
  }

  if (!IsMeReady(HeciMemBar, InitTimeout)) {
    //
    // Return as CB will be empty after reset and CSME will not put any data
    //
    HeciResetInterface (HeciDev);
    return EFI_NOT_READY;
  }

  //
  // Read until MessageComplete bit is set
  //
  while (!PacketHeader.Fields.MessageComplete) {
    Status = HeciPacketRead (
                  HeciMemBar,
                  ReadTimeout,
                  &PacketHeader,
                  (UINT32 *) &MessageBody[TotalLength / 4],
                  &PacketBuffer,
                  &PacketSize
                  );

    ///
    /// If timeout occured we need to reset the interface to clear the data that could possibly come later.
    /// Also buffer overflow and transaction errors will require a reset.
    /// We need to continue read even if buffer too small to clear the data and signal the buffer size.
    ///
    if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL) {
      if (Status != EFI_NO_RESPONSE) {
        HeciResetInterface (HeciDev);
        DEBUG ((HECI_DEBUG_LEVEL, "[HECI%d] Got msg %r: %08X\n", HeciDev, Status, PacketHeader.Data));
      }
      *Length = TotalLength;
      return Status;
    }
    ///
    /// Track the length of what we have read so far
    ///
    TotalLength += PacketSize;
    ///
    /// To do message size calculations we are using 3 different variables:
    /// PacketBuffer - on input -> space left in the buffer, on output -> number of bytes written to buffer
    /// Length       - max buffer size
    /// TotalLength  - Total message size -> sum of all bytes in multiple packets
    ///
    if (TotalLength < *Length) {
      PacketBuffer = *Length - TotalLength;
    } else {
      PacketBuffer = 0;
    }
    ///
    /// If this was a NON-BLOCKING message and it is a multipacket message, we need to change the
    /// parameter to BLOCKING because it will take a non-zero value of time until a new packet appears
    ///
    ReadTimeout = HECI_TIMEOUT_COUNT (HECI_READ_TIMEOUT);
  }

  *Length = TotalLength;

  return Status;
}

/**
  Heci Service API function wrapper to HeciReceive API

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[out] MessageBody         Pointer to a buffer used to receive a message.
  @param[in][out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI or zero-length message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciServiceReceive (
  IN      UINT8        HeciDev,
  IN      UINT32       Blocking,
  OUT     UINT32      *MessageBody,
  IN OUT  UINT32      *Length
  )
{
  return HeciReceive((HECI_DEVICE)HeciDev, Blocking, MessageBody, Length);
}

/**
  Function sends one message (of any length) through the HECI circular buffer.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the Host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_UNSUPPORTED      Current ME mode doesn't support send this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciSend (
  IN HECI_DEVICE  HeciDev,
  IN UINT32      *Message,
  IN UINT32       Length,
  IN UINT8        HostAddress,
  IN UINT8        MeAddress
  )
{
  EFI_STATUS                   Status;
  HECI_MESSAGE_HEADER          MessageHeader;
  HECI_CONTROL_STATUS_REGISTER HeciCsrHost;
  UINT32                       CircularBufferDepth;
  UINT32                       SendLength;
  UINT32                       BytesLeft;
  UINT32                       MeMode;
  UINTN                        HeciMemBar;

  if (MeGetIshHeciDevice() == HeciDev) {
    return EFI_UNSUPPORTED;
  }

  ///
  /// Make sure that HECI device BAR is correct and device is enabled.
  ///
  HeciMemBar = CheckAndFixHeciForAccess (HeciDev);
  if (HeciMemBar == 0) {
    return EFI_DEVICE_ERROR;
  }

  HeciGetMeMode (&MeMode);
  if (MeMode == ME_MODE_SECOVER) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Make sure that HECI is ready for communication
  ///
  if (!IsMeReady (HeciMemBar, HECI_TIMEOUT_COUNT (HECI_INIT_TIMEOUT))) {
    ///
    /// If reset successful, continue as the interface should be healthy now
    ///
    Status = HeciResetInterface (HeciDev);
    if (EFI_ERROR(Status)) {
      return EFI_NOT_READY;
    }
  }
  ///
  /// Set up memory mapped registers
  ///
  HeciCsrHost.Data = MmioRead32 (HeciMemBar + H_CSR);

  ///
  /// Grab Circular Buffer length and convert it from Dword to bytes
  ///
  CircularBufferDepth = 4 * HeciCsrHost.Fields.CBDepth;

  ///
  /// Prepare message header
  ///
  MessageHeader.Data               = 0;
  MessageHeader.Fields.MeAddress   = MeAddress;
  MessageHeader.Fields.HostAddress = HostAddress;

  BytesLeft = Length;
  ///
  /// Break message up into CB-sized packets and loop until completely sent
  ///
  while (BytesLeft) {
    ///
    /// Set the Message Complete bit if this is our last packet in the message.
    /// Needs to be less or equal to CB depth minus one Dword for HECI header.
    ///
    if (BytesLeft <= CircularBufferDepth - sizeof (MessageHeader)) {
      MessageHeader.Fields.MessageComplete = 1;
    }
    ///
    /// Calculate length for Message Header:
    /// It will be the smaller value of circular buffer or remaining message.
    ///
    SendLength = MIN(CircularBufferDepth - sizeof (MessageHeader),BytesLeft);
    MessageHeader.Fields.Length = SendLength;
    ///
    /// Send the current packet
    ///
    Status = HeciPacketWrite (HeciMemBar, &MessageHeader, (UINT32*)((UINTN)Message + (Length-BytesLeft)));
    if (EFI_ERROR (Status)) {
        HeciResetInterface (HeciDev);
        return Status;
    }
    ///
    /// Update the length information
    ///
    BytesLeft -= SendLength;
  }
  return EFI_SUCCESS;
}

/**
  Heci Service API function wrapper to HeciSend API

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the Host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_UNSUPPORTED      Current ME mode doesn't support send this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciServiceSend (
  IN UINT8        HeciDev,
  IN UINT32      *Message,
  IN UINT32       Length,
  IN UINT8        HostAddress,
  IN UINT8        MeAddress
  )
{
  return HeciSend((HECI_DEVICE)HeciDev, Message, Length, HostAddress, MeAddress);
}

/**
  Function sends one message through the HECI circular buffer and waits
  for the corresponding ACK message.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in][out] Message         Pointer to the message buffer.
  @param[in] Length               Length of the message in bytes.
  @param[in][out] RecLength       Length of the message response in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support send this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciSendwAck (
  IN      HECI_DEVICE  HeciDev,
  IN OUT  UINT32      *Message,
  IN      UINT32       Length,
  IN OUT  UINT32      *RecLength,
  IN      UINT8        HostAddress,
  IN      UINT8        MeAddress
  )
{
  EFI_STATUS          Status;

  if (HeciDev == MeGetIshHeciDevice ()) {
    return EFI_UNSUPPORTED;
  }

  ///
  /// Send the message
  ///
  Status = HeciSend (HeciDev, Message, Length, HostAddress, MeAddress);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[HECI%d] HeciSendwAck failed on send, Status: %r, abort!\n", HeciDev, Status));
    return Status;
  }
  ///
  /// Receive the message
  ///
  Status = HeciReceive (HeciDev, HECI_BLOCKING_MSG, Message, RecLength);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[HECI%d] HeciSendwAck failed on read, Status: %r\n", HeciDev, Status));
  }

  return Status;
}

/**
  Function forces a reinit of the heci interface by following the reset heci interface via Host algorithm

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Interface reset
**/
EFI_STATUS
EFIAPI
HeciResetInterface (
  IN  HECI_DEVICE             HeciDev
  )
{
  HECI_CONTROL_STATUS_REGISTER HeciCsrHost;
  HECI_CONTROL_STATUS_REGISTER HeciCsrMeHra;
  UINT32                       Timeout;
  UINTN                        HeciMemBar;

  DEBUG ((HECI_DEBUG_LEVEL, "[HECI%d] Resetting interface\n", HeciDev));

  ///
  /// Make sure that HECI device BAR is correct and device is enabled.
  ///
  HeciMemBar = CheckAndFixHeciForAccess (HeciDev);
  if (HeciMemBar == 0) {
    return EFI_DEVICE_ERROR;
  }

  ///
  /// Enable Reset
  ///
  HeciCsrHost.Data = MmioRead32 (HeciMemBar + H_CSR);
  DEBUG ((HECI_DEBUG_LEVEL, "[HECI%d] - Step1 Enable Host Reset : H_CSR = 0x%08X\n", HeciDev, HeciCsrHost.Data));

  if (!HeciCsrHost.Fields.Reset) {
    HeciCsrHost.Fields.Reset       = 1;
    HeciCsrHost.Fields.IntGenerate = 1;
    MmioWrite32 (HeciMemBar + H_CSR, HeciCsrHost.Data);
  }

  ///
  /// Make sure that the reset started
  ///
  DEBUG ((HECI_DEBUG_LEVEL, "[HECI%d] - Step2 Wait for reset started: H_CSR = 0x%08X\n", HeciDev, HeciCsrHost.Data));
  Timeout = HECI_TIMEOUT_COUNT (HECI_INIT_TIMEOUT);

  HeciCsrHost.Data = MmioRead32 (HeciMemBar + H_CSR);
  while (HeciCsrHost.Fields.Ready == 1) {
    if (Timeout-- == 0) {
      return EFI_TIMEOUT;
    }
    MicroSecondDelay (HECI_WAIT_DELAY);

    HeciCsrHost.Data = MmioRead32 (HeciMemBar + H_CSR);
  }

  ///
  /// Wait for ME to perform reset
  ///
  HeciCsrMeHra.Data = MmioRead32 (HeciMemBar + ME_CSR_HA);
  DEBUG ((HECI_DEBUG_LEVEL, "[HECI%d] - Step3  Wait for ME reset: ME_CSR_HA = 0x%08X\n", HeciDev, HeciCsrMeHra.Data));
  while (HeciCsrMeHra.Fields.Ready == 0) {
    if (Timeout-- == 0) {
      return EFI_TIMEOUT;
    }
    MicroSecondDelay (HECI_WAIT_DELAY);

    HeciCsrMeHra.Data = MmioRead32 (HeciMemBar + ME_CSR_HA);
  }

  ///
  /// Enable host side interface. Host SW writes the value read back to the H_CSR register.
  /// This clears the H_IS and bit in H_CSR to 0
  ///
  HeciCsrHost.Data               = MmioRead32 (HeciMemBar + H_CSR);
  HeciCsrHost.Fields.Reset       = 0;
  HeciCsrHost.Fields.IntGenerate = 1;
  HeciCsrHost.Fields.Ready       = 1;
  MmioWrite32 (HeciMemBar + H_CSR, HeciCsrHost.Data);

  return EFI_SUCCESS;
}

/**
  Heci Service API function wrapper to HeciResetInterface API

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Interface reset
**/
EFI_STATUS
EFIAPI
HeciServiceResetInterface (
  IN  UINT8             HeciDev
  )
{
  return HeciResetInterface((HECI_DEVICE)HeciDev);
}
