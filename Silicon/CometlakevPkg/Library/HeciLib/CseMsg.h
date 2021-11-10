/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _CSE_MSG_H
#define _CSE_MSG_H

#include <RegAccess.h>

#define CSE_USRCMD_SIZE      128

/*
 * PCR Extended Interface
 */
#define MKHI_BOOTLOADER_PCR_LEN            32

/*
 * Seed Interface
 */
#define MKHI_BOOTLOADER_SEED_LEN              32
#define MKHI_BOOTLOADER_SEED_MAX_ENTRIES      4


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
 * End of Post ACK message
 */
typedef struct {
  UINT32  RequestedActions;
} CBM_EOP_ACK_DATA;

//EOP-REQ
typedef struct _GEN_END_OF_POST {
  MKHI_MESSAGE_HEADER  MkhiHeader;
} GEN_END_OF_POST;

//EOP-ACK
typedef struct _GEN_END_OF_POST_ACK {
  MKHI_MESSAGE_HEADER Header;
  CBM_EOP_ACK_DATA    Data;
} GEN_END_OF_POST_ACK;

/*
 * User command  message
 */
typedef struct {
  MKHI_MESSAGE_HEADER Header;
} HECI_USER_CMD_RESPONSE;

#pragma pack(1)
typedef struct {
  MKHI_MESSAGE_HEADER  MkhiHeader;
  UINT8 SubCommand;
  UINT8 Data[CSE_USRCMD_SIZE];
} HECI_USER_CMD_REQUEST;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8               Pcr[MKHI_BOOTLOADER_PCR_LEN];
} MKHI_BOOTLOADER_PCR_EXTEND_REQ_DATA;

typedef struct _MKHI_BOOTLOADER_PCR_EXTEND_RESPONSE_DATA {
  MKHI_MESSAGE_HEADER MkhiHeader;
} MKHI_BOOTLOADER_PCR_EXTEND_RESP_DATA;

typedef struct {
  UINT8 CseSvn;   // 0XFF indicates invalid entry / SVN
  UINT8 Reserved[3];
  UINT8 Seed[MKHI_BOOTLOADER_SEED_LEN];
} MKHI_BOOTLOADER_SEED_INFO_EX;

typedef struct
{
  UINT32                             NumOfSeeds;
  MKHI_BOOTLOADER_SEED_INFO_EX       List[MKHI_BOOTLOADER_SEED_MAX_ENTRIES];
} MKHI_BOOTLOADER_SEED_LIST;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8               Seed[MKHI_BOOTLOADER_SEED_LEN];
} MKHI_BOOTLOADER_GET_RPMB_SEED_RESP_DATA;

typedef struct
{
  MKHI_MESSAGE_HEADER                MKHIHeader;
}MKHI_BOOTLOADER_GET_SEED_REQ_DATA;

typedef struct
{
  MKHI_MESSAGE_HEADER              MKHIHeader;
  MKHI_BOOTLOADER_SEED_LIST        SeedList;
}MKHI_BOOTLOADER_GET_SEED_LIST_EX_RESP_DATA;


typedef struct {
    MKHI_MESSAGE_HEADER MKHIHeader;
} GEN_GET_FW_VER;

typedef struct {
  UINT32  CodeMinor   :16; ///< Code Image version: 16-bit minor
  UINT32  CodeMajor   :16; ///< Code Image version: 16-bit major
  UINT32  CodeBuildNo :16; ///< Code Image version: 16-bit engineering build number
  UINT32  CodeHotFix  :16; ///< Code Image version: 16-bit hotfix
  UINT32  RcvyMinor   :16; ///< Recovery Image version: 16-bit minor
  UINT32  RcvyMajor   :16; ///< Recovery Image version: 16-bit major
  UINT32  RcvyBuildNo :16; ///< Recovery Image version: 16-bit engineering build number
  UINT32  RcvyHotFix  :16; ///< Recovery Image version: 16-bit hotfix
  UINT32  FITCMinor   :16; ///< Recovery Image version: 16-bit minor
  UINT32  FITCMajor   :16; ///< Recovery Image version: 16-bit major
  UINT32  FITCBuildNo :16; ///< Recovery Image version: 16-bit engineering build number
  UINT32  FITCHotFix  :16; ///< Recovery Image version: 16-bit hotfix
} GEN_GET_FW_VER_ACK_DATA;

//
//FW Capabilities
//
typedef union {
  UINT32  Data;
  struct {
    UINT32  FullNetwork      : 1;        ///< Full network manageability
    UINT32  StandardNetwork  : 1;        ///< Standard network manageability
    UINT32  Manageability    : 1;        ///< Manageability
    UINT32  SmallBusinessAdv : 1;        ///< Small business advantage
    UINT32  IntelIntTouch    : 1;        ///< Intel Integrated Touch
    UINT32  Reserved1        : 1;        ///< [5] Reserved
    UINT32  Cls              : 1;        ///< Capability Licensing Service (CLS)
    UINT32  Ve               : 1;        ///<  Virtualization Engine (VE)
    UINT32  Reserved2        : 2;        ///< [9:8] Reserved
    UINT32  Ish              : 1;        ///<  Virtualization Engine (VE)
    UINT32  Reserved3        : 1;        ///< [11] Reserved
    UINT32  PAVP             : 1;        ///< [12] Protected Audio Video Path (PAVP)
    UINT32  Reserved4        : 4;        ///< [16:13] Reserved
    UINT32  IPV6             : 1;        ///< [17] IPV6
    UINT32  KVM              : 1;        ///< [18] KVM Remote Control (KVM)
    UINT32  OCH              : 1;        ///< [19] Outbreak Containment Heuristic (OCH)
    UINT32  DAL              : 1;        ///< [20] Dynamic Application Loader (DAL)
    UINT32  TLS              : 1;        ///< [21] TLS
    UINT32  Reserved5        : 1;        ///< [22] Reserved
    UINT32  WLAN             : 1;        ///< [23] Wireless LAN (WLAN)
    UINT32  Reserved6        : 5;        ///< [28:24] Reserved
    UINT32  Ptt              : 1;        ///< Platform Trust Technology (PTT)
    UINT32  Reserved7        : 1;        ///< [30] Reserved
    UINT32  Nfc              : 1;        ///< Near Field Communication (NFC)
  } Fields;
} SECFWCAPS_SKU;

typedef struct {
  MKHI_MESSAGE_HEADER     MKHIHeader;
  GEN_GET_FW_VER_ACK_DATA Data;
} GEN_GET_FW_VER_ACK;

typedef union _{
  GEN_GET_FW_VER     Request;
  GEN_GET_FW_VER_ACK Response;
} GEN_GET_FW_VER_ACK_BUFFER;

//
// Get Firmware Capability MKHI
//
typedef struct {
  UINT32  RuleId;
} GET_RULE_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER MKHIHeader;
  GET_RULE_DATA       Data;
} GEN_GET_FW_CAPSKU;

typedef struct {
  UINT32        RuleID;
  UINT8         RuleDataLen;
  SECFWCAPS_SKU FWCap;
  UINT8         Reseved[3];
} GEN_GET_FW_CAPS_SKU_ACK_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER           MKHIHeader;
  GEN_GET_FW_CAPS_SKU_ACK_DATA  Data;
} GEN_GET_FW_CAPS_SKU_ACK;
#pragma pack()

#endif  // _CSE_MSG_H
