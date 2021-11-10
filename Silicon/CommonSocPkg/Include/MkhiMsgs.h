/** @file
  MKHI Messages

  Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _MKHI_MSGS_COMMON_H_
#define _MKHI_MSGS_COMMON_H_

//
// Defines for GroupID
//
#define MKHI_FWCAPS_GROUP_ID                0x03
#define MKHI_GEN_GROUP_ID                   0xFF

//
// Defines for Command
//
//
// Defines for FWCAPS_GROUP Command
//
#define FWCAPS_GET_RULE_CMD                 0x02
#define FWCAPS_SET_RULE_CMD                 0x03
//
// Defines for GEN_GROUP Command
//
#define GEN_GET_FW_VERSION_CMD              0x02

//
// Host Address of Fixed Address MEI Header
//
#define BIOS_FIXED_HOST_ADDR                0
#define HECI_MKHI_MESSAGE_ADDR              0x07

#pragma pack(1)

//
// MKHI host message header. This header is part of HECI message sent from MEBx via
// Host Configuration Interface (HCI). ME Configuration Manager or Power Configuration
// Manager also include this header with appropriate fields set as part of the
// response message to the HCI.
//
typedef union {
  UINT32  Data;
  struct {
    UINT32  GroupId    : 8;
    UINT32  Command    : 7;
    UINT32  IsResponse : 1;
    UINT32  Reserved   : 8;
    UINT32  Result     : 8;
  } Fields;
} MKHI_MESSAGE_HEADER;

//
// Get Firmware Capability
//
typedef union {
  UINT32  Data;
  struct {
    UINT32  FullNet         : 1;      ///< [0] Full network manageability
    UINT32  StdNet          : 1;      ///< [1] Standard network manageability
    UINT32  Manageability   : 1;      ///< [2] Manageability
    UINT32  Reserved        : 1;      ///< [3] Reserved
    UINT32  IntegratedTouch : 1;      ///< [4] Intel Integrated Touch
    UINT32  EyeRiver        : 1;      ///< [5] Eye River
    UINT32  IntelCLS        : 1;      ///< [6] IntelR Capability Licensing Service (CLS)
    UINT32  Reserved1       : 3;      ///< [9:7] Reserved
    UINT32  ISH             : 1;      ///< [10] IntelR Sensor Hub
    UINT32  Reserved2       : 1;      ///< [11] Reserved
    UINT32  PAVP            : 1;      ///< [12] Protected Audio Video Path (PAVP)
    UINT32  Reserved3       : 4;      ///< [16:13] Reserved
    UINT32  IPV6            : 1;      ///< [17] IPV6
    UINT32  KVM             : 1;      ///< [18] KVM Remote Control (KVM)
    UINT32  Reserved4       : 1;      ///< [19] Reserved
    UINT32  DAL             : 1;      ///< [20] Dynamic Application Loader (DAL)
    UINT32  TLS             : 1;      ///< [21] Cipher Transport Layer (TLS)
    UINT32  Reserved5       : 1;      ///< [22] Reserved
    UINT32  WLAN            : 1;      ///< [23] Wireless LAN (WLAN)
    UINT32  Reserved6       : 5;      ///< [28:24] Reserved
    UINT32  PTT             : 1;      ///< [29] Platform Trust Technology (PTT)
    UINT32  Reserved7       : 2;      ///< [31:30] Reserved
  } Fields;
} MEFWCAPS_SKU;

//
// Get Firmware Capability MKHI
//
typedef struct {
  UINT32  RuleId;
} GET_RULE_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  GET_RULE_DATA       Data;
} GEN_GET_FW_CAPSKU;

typedef struct {
  UINT32        RuleID;
  UINT8         RuleDataLen;
  MEFWCAPS_SKU  FWCap;
  UINT8         Reseved[3];
} GEN_GET_FW_CAPS_SKU_ACK_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER           MKHIHeader;
  GEN_GET_FW_CAPS_SKU_ACK_DATA  Data;
} GEN_GET_FW_CAPS_SKU_ACK;

//
// Get FW Version
//
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} GEN_GET_FW_VER;

typedef struct {
  UINT32  CodeMinor   : 16;
  UINT32  CodeMajor   : 16;
  UINT32  CodeBuildNo : 16;
  UINT32  CodeHotFix  : 16;
  UINT32  RcvyMinor   : 16;
  UINT32  RcvyMajor   : 16;
  UINT32  RcvyBuildNo : 16;
  UINT32  RcvyHotFix  : 16;
  UINT32  FitcMinor   : 16;
  UINT32  FitcMajor   : 16;
  UINT32  FitcBuildNo : 16;
  UINT32  FitcHotFix  : 16;
} GEN_GET_FW_VER_ACK_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER     MkhiHeader;
  GEN_GET_FW_VER_ACK_DATA Data;
} GEN_GET_FW_VER_ACK;

typedef union {
  UINT32  Data;
  struct {
    UINT32  PlatformTargetUsageType : 4;
    UINT32  Reserved                : 2;
    UINT32  SuperSku                : 1;
    UINT32  Reserved1               : 1;
    UINT32  IntelMeFwImageType      : 4;
    UINT32  PlatformBrand           : 4;
    UINT32  Reserved2               : 16;
  } Fields;
} PLATFORM_TYPE_RULE_DATA;

#pragma pack()

#endif // _MKHI_MSGS_COMMON_H_
