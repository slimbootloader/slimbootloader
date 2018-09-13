/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MKHI_MSGS_H
#define _MKHI_MSGS_H

// Typedef for GroupID
typedef enum {
  MkhiCbmGroupId   = 0,
  MkhiPmGroupId,
  MkhiPwdGroupId,
  MkhiFwCapsGroupId,
  MkhiAppGroupId,
  MkhiSpiGroupId,
  MkhiMdesGroupId  = 8,
  MkhiMaxGroupId,
  MkhiGenGroupId   = 0xFF
} MkhiGroupId;

// command handle by HCI

#define FWCAPS_GET_RULE_CMD           0x02
#define FWCAPS_SET_RULE_CMD           0x03

//
// Get Firmware Capability MKHI
//
typedef struct {
  UINT32  RuleId;
} HECI_GET_RULE_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  HECI_GET_RULE_DATA       Data;
} HECI_GEN_GET_FW_FEATURE_STATUS;

#define BIOS_FIXED_HOST_ADDR          0

#define HECI_CORE_MESSAGE_ADDR        0x07

//
// MKHI host message header. This header is part of HECI message sent from MEBx via
// Host Configuration Interface (HCI). ME Configuration Manager or Power Configuration
// Manager also include this header with appropriate fields set as part of the
// response message to the HCI.
//

typedef struct {
  UINT32  BiosImrDisable[2];
  UINT32  BiosMinImrsBa;
} DRAM_INIT_DONE_IMRS_REQ_DATA;

typedef struct {
  UINT8    FfsEntryExit           :1;
  UINT8    NonDestructiveAliasCheck :1;
  UINT8    Rsvd           :6;
} DRAM_INIT_DONE_REQ_FLAGS;

typedef struct {
  MKHI_MESSAGE_HEADER      MkhiHeader;
  DRAM_INIT_DONE_IMRS_REQ_DATA  ImrData;
  DRAM_INIT_DONE_REQ_FLAGS    Flags;
  UINT8            MemStatus;
  UINT8            Reserved[2];
} DRAM_INIT_DONE_CMD_REQ;

typedef struct {
  UINT32     ImrsSortedRegionBa;
  UINT32     ImrsSortedRegionLen;
  UINT8      OemSettingsRejected;
  UINT8      Rsvd[3];
} DRAM_INIT_DONE_IMRS_RESP_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER       MkhiHeader;
  DRAM_INIT_DONE_IMRS_RESP_DATA ImrsData;
  UINT8             BiosAction;
  UINT8             Reserved[3];
} DRAM_INIT_DONE_CMD_RESP_DATA;

#define COMMON_GROUP_ID          0xF0
#define DRAM_INIT_DONE_CMD       0x01

#endif // _MKHI_MSGS_H
