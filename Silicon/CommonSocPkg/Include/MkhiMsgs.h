/** @file
  MKHI Messages

  Copyright (c) 2018 - 2023, Intel Corporation. All rights reserved.<BR>
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

#define BUP_COMMON_GROUP_ID                   0xF0
#define GET_EARLY_BOOT_PERFORMANCE_DATA_CMD   0x08

///
/// Defines for GEN_GROUP Command
///
#define GEN_SET_MEASURED_BOOT_STATE_CMD           0x22
#define GEN_GET_MEASURED_BOOT_STATE_CMD           0x23

///
/// Defines for GroupID
///
#define MKHI_MCA_GROUP_ID     0x0A
#define MKHI_GEN_GROUP_ID     0xFF

///
/// Defines for MCA_GROUP Command
///
#define MCA_REVOKE_OEM_KEY_HASH_CMD       0x2F
#define MCA_GET_OEM_KEY_STATUS_CMD        0x0D

///
/// Defines for FIPS Mode Command
///
#define GEN_SET_FIPS_MODE_CMD                     0x20
#define GEN_GET_FIPS_MODE_CMD                     0x21

///
/// DEFINES FOR EPS_GROUP command
///
#define MKHI_EPS_GROUP_ID                         0x21
#define EPS_SET_STATE_CMD                         0x1
#define EPS_GET_STATE_CMD                         0x2
#define EPS_GET_PLATFORM_ID_STATE_CMD             0x3
#define EPS_INSTALL_LICENSE_CMD                   0x4

#pragma pack(1)

//
// Typedef for Result field of MHKI Header
//
typedef enum {
  MkhiStatusSuccess,
  MkhiStatusInvalidState,
  MkhiStatusMessageSkipped,
  MkhiStatusSizeError       = 0x5,
  MkhiStatusInvalidAccess   = 0x84,
  MkhiStatusInvalidParams   = 0x85,
  MkhiStatusNotReady        = 0x88,
  MkhiStatusNotSupported    = 0x89,
  MkhiStatusInvalidAddress  = 0x8C,
  MkhiStatusInvalidCommand  = 0x8D,
  MkhiStatusFailure         = 0x9E,
  MkhiStatusInvalidResource = 0xE4,
  MkhiStatusResourceInUse   = 0xE5,
  MkhiStatusNoResource      = 0xE6,
  MkhiStatusGeneralError    = 0xFF
} MKHI_RESULT;

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

#define EARLY_BOOT_PERF_DATA_CMD_VERSION       1
#define EARLY_BOOT_PERF_DATA_LENGTH_VERSION_1  64
typedef struct
{
  MKHI_MESSAGE_HEADER                Header;
  UINT32                             Reserved;
} GET_EARLY_BOOT_PERF_DATA_CMD;
typedef struct
{
  MKHI_MESSAGE_HEADER                Header;
  UINT32                             BootDataVersion;
  UINT32                             BootDataLength;
  UINT32                             BootPerformanceData[];
} GET_EARLY_BOOT_PERF_DATA_RESPONSE;
typedef union {
  GET_EARLY_BOOT_PERF_DATA_CMD      Request;
  GET_EARLY_BOOT_PERF_DATA_RESPONSE Response;
} GET_EARLY_BOOT_PERF_DATA_BUFFER;

///
/// Set FIPS Mode
///
#define FIPS_MODE_DISABLED 0
#define FIPS_MODE_ENABLED  1

typedef struct {
  UINT32              FipsMode;
} SET_FIPS_MODE_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  SET_FIPS_MODE_DATA  Data;
} SET_FIPS_MODE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} SET_FIPS_MODE_ACK;

typedef union {
  SET_FIPS_MODE       Request;
  SET_FIPS_MODE_ACK   Response;
} SET_FIPS_MODE_BUFFER;

///
/// Get FIPS Mode
///
typedef struct {
  UINT16              Major;
  UINT16              Minor;
  UINT16              Hotfix;
  UINT16              Build;
} FIPS_VERSION;

typedef struct {
  UINT32              FipsMode;
  FIPS_VERSION        CryptoVersion;
  UINT8               Reserved[8];
} GET_FIPS_MODE_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} GET_FIPS_MODE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  GET_FIPS_MODE_DATA  Data;
} GET_FIPS_MODE_ACK;

typedef union {
  GET_FIPS_MODE       Request;
  GET_FIPS_MODE_ACK   Response;
} GET_FIPS_MODE_BUFFER;

///
/// Get CSME Measured Boot
///
typedef struct {
  MKHI_MESSAGE_HEADER     MkhiHeader;
} GET_MEASURED_BOOT;

typedef struct {
  MKHI_MESSAGE_HEADER     MkhiHeader;
  UINT8                   Data;
} GET_MEASURED_BOOT_ACK;

typedef union {
  GET_MEASURED_BOOT       Request;
  GET_MEASURED_BOOT_ACK   Response;
} GET_MEASURED_BOOT_BUFFER;

///
/// Set CSME Measured Boot
///
typedef struct {
  MKHI_MESSAGE_HEADER     MkhiHeader;
  UINT8                   Data;
} SET_MEASURED_BOOT;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} SET_MEASURED_BOOT_ACK;

typedef union {
  SET_MEASURED_BOOT       Request;
  SET_MEASURED_BOOT_ACK   Response;
} SET_MEASURED_BOOT_BUFFER;

///
/// OEM Key Revocation
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} OEM_KEY_REVOKE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} OEM_KEY_REVOKE_ACK;

typedef union {
  OEM_KEY_REVOKE       Request;
  OEM_KEY_REVOKE_ACK   Response;
} OEM_KEY_REVOKE_BUFFER;


typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} OEM_KEY_STATUS_REQ;

typedef struct {
  UINT8               Valid;
  UINT8               InUse;
  UINT8               Revoked;
  UINT8               KeyHash[64];
} KEY_INFO;

typedef struct {
  UINT8               RevocationEnabled;
  UINT8               NumKeySupported;
  UINT32              KeyHashType;
  KEY_INFO            Keys[2];
} OEM_KEY_STATUS;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  OEM_KEY_STATUS      OemKeyStatus;
} OEM_KEY_STATUS_ACK;

typedef union {
  OEM_KEY_STATUS_REQ   Request;
  OEM_KEY_STATUS_ACK   Response;
} OEM_KEY_STATUS_BUFFER;

///
/// Get Extended Period State (EPS) info
///
typedef struct
{
  MKHI_MESSAGE_HEADER     MkhiHeader;
} EPS_GET_STATE;

typedef enum
{
  EPS_DELIVERY_METHOD_NOT_SET    = 0,
  EPS_DELIVERY_METHOD_MANAGED    = 1,
  EPS_DELIVERY_METHOD_AUTO       = 2,
} EPS_DELIVERY_METHOD;

typedef struct
{
  EPS_DELIVERY_METHOD     delivery_method;
  UINT8                   license_requested;
  UINT8                   license_installed;
  UINT8                   license_permits;
  UINT8                   reserved;
} EPS_GET_STATE_INFO;

typedef struct
{
  MKHI_MESSAGE_HEADER     MkhiHeader;
  EPS_GET_STATE_INFO      eps_info;
} EPS_GET_STATE_ACK;

typedef union {
  EPS_GET_STATE           Request;
  EPS_GET_STATE_ACK       Response;
} EPS_GET_STATE_BUFFER;

#pragma pack()

#endif // _MKHI_MSGS_COMMON_H_
