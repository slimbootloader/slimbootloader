/**@file
  Core BIOS Messages

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef CORE_BIOS_MSG_H
#define CORE_BIOS_MSG_H

#include <MkhiMsgs.h>

#pragma pack(1)


/**
  Address 0 is statically allocated for MEI bus message protocol that assists in MEI bus
  enumeration, power management, error and other bus control functionality.
**/
#define BIOS_FIXED_HOST_ADDR                0

///
/// ME Address of Fixed Address MEI Header -
/// This is the logical address of the Intel(R) ME client
/// of the message. This address is assigned during ME firmware initialization.
///
#define HECI_MKHI_MESSAGE_ADDR              0x07
#define HECI_MCHI_MESSAGE_ADDR              0x09


///
/// Reset request message data
///
typedef struct {
  UINT8 RequestOrigin;
  UINT8 ResetType;
} CBM_RESET_REQ_DATA;

///
/// Reset request MKHI message
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  CBM_RESET_REQ_DATA  Data;
} CBM_RESET_REQ;

///
/// Reset request MKHI ack message
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} CBM_RESET_ACK;


#pragma pack()

#endif
