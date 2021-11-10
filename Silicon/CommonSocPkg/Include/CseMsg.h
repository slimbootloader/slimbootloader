/** @file

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _CSE_MSG_H
#define _CSE_MSG_H

#include <MkhiMsgs.h>

//
// Defines for PCR Extended Interface
//
#define MKHI_BOOTLOADER_PCR_EXTEND_CMD_REQ  0x10
#define MKHI_BOOTLOADER_PCR_EXTEND_GRP_ID   0xF0
#define MKHI_BOOTLOADER_PCR_LEN             32
//
// Defines for Firmware Update Interface
//
#define MKHI_IFWI_UPDATE_GROUP_ID           0x20
#define IFWI_PREPARE_FOR_UPDATE_CMD_ID      0x01
//
// Defines for Seed Interface
//
#define MKHI_BOOTLOADER_SEED_LEN            32
#define MKHI_BOOTLOADER_SEED_MAX_ENTRIES    4
#define MKHI_BOOTLOADER_PCR_LEN             32

#define MKHI_IFWI_UPD_SIG                   0x20
#define MKHI_IFWI_UPD_SIG_CMD               0x7

#define CSE_USRCMD_SIZE                     128
#define SEND_CLEAR_SUB_COMMAND              0x0
#define SEND_USR_CMD_SUB_COMMAND            0x1

#pragma pack(1)

typedef struct {
  UINT8 CseSvn;
  UINT8 Reserved[3];
  UINT8 Seed[MKHI_BOOTLOADER_SEED_LEN];
} MKHI_BOOTLOADER_SEED_INFO_EX;

typedef struct {
  UINT32                             NumOfSeeds;
  MKHI_BOOTLOADER_SEED_INFO_EX       List[MKHI_BOOTLOADER_SEED_MAX_ENTRIES];
} MKHI_BOOTLOADER_SEED_LIST;

typedef struct _MKHI_BOOTLOADER_PCR_EXTEND_RESPONSE_DATA {
  MKHI_MESSAGE_HEADER MkhiHeader;
} MKHI_BOOTLOADER_PCR_EXTEND_RESP_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8               Pcr[MKHI_BOOTLOADER_PCR_LEN];
} MKHI_BOOTLOADER_PCR_EXTEND_REQ_DATA;

/*
 * ifwi prepare for update
 */
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8 ResetType;
} HECI_REQ_IFWI_PREPARE_FOR_UPDATE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8 Flag;
} HECI_RES_IFWI_PREPARE_FOR_UPDATE;

/*
 * Enter DNX mode
 */
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} HECI_REQ_CSE_DNX_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32 ReqBiosAction;
} HECI_RES_CSE_DNX_REQ;

/*
 * User command  message
 */
typedef struct {
  MKHI_MESSAGE_HEADER Header;
} HECI_USER_CMD_RESPONSE;

typedef struct {
  MKHI_MESSAGE_HEADER  MkhiHeader;
  UINT8 SubCommand;
  UINT8 Data[CSE_USRCMD_SIZE];
} HECI_USER_CMD_REQUEST;

#pragma pack()

#endif // _CSE_MSG_H
