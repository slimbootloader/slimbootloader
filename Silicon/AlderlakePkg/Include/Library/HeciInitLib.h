/** @file
  Heci init library.

  Copyright (c) 2007 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _HECI_INIT_LIB_H_
#define _HECI_INIT_LIB_H_
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>


typedef enum {
  HECI1_DEVICE = 0,
  HECI2_DEVICE = 1,
  HECI3_DEVICE = 4,
  HECI4_DEVICE = 5,
  ISH_HECI     = 8
} HECI_DEVICE;

/**
  Determines if the HECI device is present.
  If present, initializes it to be used by the BIOS.

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        No HECI device
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_UNSUPPORTED         HECI MSG is unsupported
**/
EFI_STATUS
EFIAPI
HeciInitialize (
  IN HECI_DEVICE                      HeciDev
  );

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
  );

/**
  Reads a message from CSME through HECI.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in, out] MessageBody     Pointer to a buffer used to receive a message.
  @param[in, out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_NOT_READY           HECI is not ready for communication
  @retval EFI_TIMEOUT             Failed to receive a full message on time
  @retval EFI_NO_RESPONSE         No response from CSME
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
**/
EFI_STATUS
EFIAPI
HeciReceive (
  IN      HECI_DEVICE HeciDev,
  IN      UINT32      Blocking,
  IN OUT  UINT32      *MessageBody,
  IN OUT  UINT32      *Length
  );

/**
  Function sends one message (of any length) through the HECI circular buffer.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_NOT_READY           HECI is not ready for communication
  @retval EFI_TIMEOUT             CSME failed to empty the circular buffer
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support send this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciSend (
  IN HECI_DEVICE               HeciDev,
  IN UINT32                    *Message,
  IN UINT32                    Length,
  IN UINT8                     HostAddress,
  IN UINT8                     MeAddress
  );

/**
  Function sends one message through the HECI circular buffer and waits
  for the corresponding ACK message with retry mechanism.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in][out] Message         Pointer to the message buffer.
  @param[in] Length               Length of the message in bytes.
  @param[in][out] RecLength       Length of the message response in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_NOT_READY           HECI is not ready for communication
  @retval EFI_TIMEOUT             CSME failed to empty or fill the circular buffer
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support send this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciSendwAck (
  IN      HECI_DEVICE HeciDev,
  IN OUT  UINT32      *Message,
  IN      UINT32      Length,
  IN OUT  UINT32      *RecLength,
  IN      UINT8       HostAddress,
  IN      UINT8       MeAddress
  );

/**
  Me reset and waiting for ready

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Delay                The biggest waiting time

  @retval EFI_SUCCESS             Host Ready bit cleared
  @retval EFI_TIMEOUT             Host Ready bit not cleared
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
**/
EFI_STATUS
EFIAPI
MeResetWait (
  IN  HECI_DEVICE              HeciDev,
  IN  UINT32                   Delay
  );

/**
  Function forces a reinit of the heci interface by following the reset heci interface via host algorithm
  in HPS section 4.1.1.1

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_SUCCESS             Interface reset successful
  @retval EFI_TIMEOUT             ME is not ready
**/
EFI_STATUS
EFIAPI
ResetHeciInterface (
  IN  HECI_DEVICE              HeciDev
  );

/**
  Get an abstract Intel HECI1 State from Firmware Status Register.
  This is used to control BIOS flow for different Intel ME functions.

  @param[out] MeStatus            Pointer for status report
                                    @see MeState.h - Abstract ME status definitions.

  @retval EFI_SUCCESS             MeStatus copied
  @retval EFI_INVALID_PARAMETER   Pointer to MeStatus is invalid
  @retval EFI_NOT_FOUND           HECI1 Device hidden
**/
EFI_STATUS
EFIAPI
HeciGetMeStatus (
  OUT UINT32        *MeStatus
  );

/**
  Get HECI1 Mode

  @param[out] MeMode              Pointer for HECI1 Mode report
                                    @see MeState.h - Abstract ME Mode definitions.

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer to MeMode is invalid
  @retval EFI_NOT_FOUND           HECI1 Device hidden
**/
EFI_STATUS
EFIAPI
HeciGetMeMode (
  OUT UINT32        *MeMode
  );

/**
  ReadHeciFwStatus

  @retval EFI_SUCCESS             HECI Status Read successfully.
  @retval Others                  There is failure when Reading HECI Status
 **/
EFI_STATUS
EFIAPI
ReadHeciFwStatus (
    UINT32 *FwSts
    );

/**
  Send Get Firmware SKU Request to Sec

  @param[in] MsgGenGetFwCapsSku      Send  message for Get Firmware Capability SKU
  @param[out] MsgGenGetFwCapsSkuAck  Return message for Get Firmware Capability SKU ACK

  @exception EFI_UNSUPPORTED        Current Sec mode doesn't support this function
  @retval EFI_SUCCESS               Command succeeded
  @retval EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT               HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL      Message Buffer is too smallfor the Acknowledge
 **/
EFI_STATUS
EFIAPI
HeciGetFwCapsSkuMsg (
    IN UINT8  *MsgGetFwCaps,
    OUT UINT8  *MsgGetFwCapsAck
  );

/**
  Send Get Firmware Version Request to Sec

  @param[out] MsgGenGetFwVersionAck   Return FW version from CSE.

  @exception EFI_UNSUPPORTED      Current Sec mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
 **/
EFI_STATUS
EFIAPI
HeciGetFwVersionMsg (
    OUT UINT8     *MsgAck
  );


/**
  This function sends a command to CSME to get Log information of Measurements provided in
  CSME Extend Registers for BIOS to extend to TPM-PCR registers.

  @param[out]     EventLogBuffer    Pointer to received Event log buffer
  @param[in]      RemainingBufSize  Size of available buffer to collect event log
  @param[out]     ReceivedLogSize   Size of received event log
  @param[in, out] LogEntryIndex     On input - Index of first / starting log entry in the buffer. On output - Index of last log entry returned
  @param[out]     RemainingEntries  Pointer to Remaining Entries (0 - no more entries in a log; 1 - additional iteration(s) is(are) required)

  @retval EFI_SUCCESS             Log received successfully
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this function
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout or transition failed
  within allowed time limit
**/
EFI_STATUS
EFIAPI
HeciGetErLog (
  OUT UINT8        *EventLogBuffer,
  IN  UINT32       RemainingBufSize,
  IN OUT UINT32    *ReceivedLogSize,
  IN OUT UINT8     *LogEntryIndex,
  OUT UINT8        *RemainingEntries
  );

/**
  Send Get Current CSME Measured Boot State

  @param[out] GetMeasuredBootState    Pointer to get CSME Measured boot status.

  @retval EFI_UNSUPPORTED         Current CSME mode/version doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
EFIAPI
HeciGetMeasuredBootState (
  OUT UINT8  *GetMeasuredBootState
  );

/**
  Send Set Measured Boot State to Enable or Disable CSME Measurement

  @param[in]  SetMeasuredBootState  Set Measured boot state to Enable/Disable CSME Extended Measured boot

  @retval EFI_UNSUPPORTED         Current CSME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
**/
EFI_STATUS
EFIAPI
HeciSetMeasuredBootState (
  IN UINT8 SetMeasuredBootState
  );

/**
  This function gets sha256 or sha384 CSME Measured data from HECI1 MMIO space (HECI1_HER1 - HECI1_HER12).

  @param[in]      Her             - Pointer to receive HER Data buffer of 12 DWORDS
  @param[in]      HerOffset       - Offset of first HER register in MMIO Space
  @param[in]      HerMaxRegCount  - Max Register count to read HER registers

  @retval EFI_SUCCESS             Event data read successfully
  @retval EFI_INVALID_PARAMETER   Input parameters are not valid
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_NOT_READY           Device is not ready
**/
EFI_STATUS
EFIAPI
HeciGetHerHashData (
  IN UINT32    *Her,
  IN UINT32    HerOffset,
  IN UINT8     HerMaxRegCount
  );

/**
  Check for Manufacturing Mode

  @param[out]  ManfMode            Manufacturing mode/debug mode.

  @retval EFI_UNSUPPORTED         Current CSME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded

**/
EFI_STATUS
EFIAPI
HeciGetManufactureMode (
  OUT UINT16 *MeManuMode
  );

/**
  Check for Spi Protection Mode

  @param[out]  SpiProtectionMode   Spi Protection mode on/off

  @retval EFI_UNSUPPORTED         Current CSME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded

**/
EFI_STATUS
EFIAPI
HeciGetSpiProtectionMode (
  OUT UINT8 *SpiProtectionMode
  );

/**
  Retrieve MBP data

  @retval     RETURN_SUCCESS       If getting the MBP data successfully
  @retval     EFI_NOT_FOUND        Failed to get MBP data
**/
EFI_STATUS
RetrieveMBPData (
  VOID
  );

/**
  This command is used to indicate to the FW to revoke a OEM key.
  It should be executed when MBP data indicates that a key is pending for
  revocation.
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
**/
EFI_STATUS
EFIAPI
HeciRevokeOemKey (
   VOID
   );

/**
  This command indicates to the FW that it shall commit ARBSVN to fuse.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
**/
EFI_STATUS
HeciArbSvnCommitMsg (
  VOID
  );

#endif
