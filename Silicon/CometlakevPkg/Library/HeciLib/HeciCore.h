/** @file
  Definitions for HECI driver

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _HECI_CORE_H
#define _HECI_CORE_H

#include <RegAccess.h>

#define ShowBuffer(a,b)

#define NON_BLOCKING                        0
#define BLOCKING                            1

//
// Timeout values based on HPET
//
#define HECI_WAIT_DELAY             1000      ///< 1ms timeout for IO delay
#define HECI_INIT_TIMEOUT           15000000  ///< 15sec timeout in microseconds
#define HECI_READ_TIMEOUT           5000000   ///< 5sec timeout in microseconds
#define HECI_SEND_TIMEOUT           5000000   ///< 5sec timeout in microseconds
#define HECI_MAX_RETRY              3         ///< Value based off HECI HPS

//
// HECI bus function version
//
#define HBM_MINOR_VERSION 0
#define HBM_MAJOR_VERSION 1

///
/// Header for all HECI message message placed in an Intel MEI circular buffer
///
typedef union {
  UINT32  Data;
  struct {
    /**
      This is the logical address of the Intel ME client of the message. This address is assigned
      during ME firmware initialization.
    **/
    UINT32  MeAddress       : 8;
    /**
      This is the logical address of the Host client of the message. This address is assigned
      when the host client registers itself with the Host MEI driver.
    **/
    UINT32  HostAddress     : 8;
    /**
      This is the message length in bytes, not including the MEI_MESSAGE_HEADER. A value of 0
      indicates no additional bytes are part of the message.
    **/
    UINT32  Length          : 9;
    UINT32  Reserved        : 6;
    UINT32  MessageComplete : 1; ///< Indicates the last message of the list
  } Fields;
} HECI_MESSAGE_HEADER;

//
// Prototypes
//
/**
  This function provides a standard way to verify the HECI cmd and MBAR regs
  in its PCI cfg space are setup properly and that the local mHeciContext
  variable matches this info.

  @param[out] HeciMemBar          HECI Memory BAR
**/
UINTN
CheckAndFixHeciForAccess (
  IN HECI_DEVICE                  HeciDev
  );

/**
  Function sends one message packet through the HECI circular buffer
  Corresponds to HECI HPS (part of) section 4.2.3

  @param[in] MessageHeader        Pointer to the message header.
  @param[in] MessageData          Pointer to the actual message data.

  @retval EFI_SUCCESS             One message packet sent
  @retval EFI_DEVICE_ERROR        ME is not ready
  @retval EFI_TIMEOUT             HECI is not ready for communication
**/
EFI_STATUS
HeciPacketWrite (
  IN  HECI_DEVICE                 HeciDev,
  IN  HECI_MESSAGE_HEADER         *MessageHeader,
  IN  UINT32                      *MessageData
  );

/**
  Reads a HECI message from ISH.

  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in, out] MessageBody     Pointer to a buffer used to receive a message.
  @param[in, out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             Message read and placed in buffer
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
  @retval EFI_UNSUPPORTED         Response was a multipacket message, not supported in BIOS yet
  @retval any other status        Error caused by ISH firmware
**/
EFI_STATUS
IshHeciReceive (
  IN      UINT32                  Blocking,
  IN OUT  UINT32                  *MessageBody,
  IN OUT  UINT32                  *Length
  );

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
  );

/**
  Function sends one message through the ISH HECI and waits
  for the corresponding response message

  @param[in, out] Message         Pointer to the message buffer.
  @param[in] Length               Length of the message in bytes.
  @param[in, out] RecLength       Length of the message response in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @retval EFI_SUCCESS             Message sent and response received
  @retval EFI_UNSUPPORTED         Message is too big to send / receive as single packet; multipacket messages not supported in BIOS yet
  @retval any other status        Error caused by ISH firmware
**/
EFI_STATUS
IshHeciSendAndReceive (
  IN OUT  UINT32                  *Message,
  IN      UINT32                  Length,
  IN OUT  UINT32                  *RecLength,
  IN      UINT8                   HostAddress,
  IN      UINT8                   MeAddress
  );


#include <Library/PciSegmentLib.h>
#include <HeciInitLib.h>

#endif // _HECI_CORE_H
