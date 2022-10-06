/** @file

  Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
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
