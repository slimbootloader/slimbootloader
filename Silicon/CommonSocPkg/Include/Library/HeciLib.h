/** @file

  Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _HECI_LIB_H_
#define _HECI_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/MeChipsetLib.h>

#define HECI_NON_BLOCKING_MSG               0
#define HECI_BLOCKING_MSG                   1

//
// Abstract ME Mode Definitions
//
#define ME_MODE_NORMAL                      0x00
#define ME_MODE_DEBUG                       0x02
#define ME_MODE_TEMP_DISABLED               0x03
#define ME_MODE_SECOVER                     0x04
#define ME_MODE_SPS                         0x05
#define ME_MODE_FAILED                      0x06

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

/**
  Return HECI1 Mode

  @param[out] MeMode              Pointer for HECI1 Mode report

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeMode (
  OUT UINT32                     *MeMode
  );

/**
  Read Heci1 FwStatus

  @retval EFI_SUCCESS             HECI Status Read successfully.
  @retval Others                  There is failure when Reading HECI Status

 **/
EFI_STATUS
EFIAPI
HeciReadFwStatus (
  IN UINT32                      *FwSts
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
  Send Get Firmware Version Request

  @param[out] MsgAck              Return FW version from CSE.

  @exception EFI_UNSUPPORTED      Current Sec mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
 **/
EFI_STATUS
EFIAPI
HeciGetFwVersionMsg (
  OUT UINT8                      *MsgAck
  );

/**
  Send Get Firmware SKU Request

  @param[out] MsgGetFwCapsAck     Return message for Get Firmware Capability SKU ACK

  @exception EFI_UNSUPPORTED      Current Sec mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
 **/
EFI_STATUS
EFIAPI
HeciGetFwCapsSkuMsg (
  OUT UINT8                      *MsgGetFwCapsAck
  );

/**
  Register HECI service

  @retval EFI_SUCCESS             HECI service is registered successfully.
  @retval Others                  There is failure when register HECI service
**/
EFI_STATUS
EFIAPI
HeciRegisterHeciService (
  VOID
  );

#endif // _HECI_LIB_H_
