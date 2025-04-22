/** @file
  MKHI Messages

  Copyright (c) 2010 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _MKHI_MSGS_H
#define _MKHI_MSGS_H

#pragma pack(1)

///
/// MKHI host message header. This header is part of HECI message sent from MEBx via
/// Host Configuration Interface (HCI). ME Configuration Manager or Power Configuration
/// Manager also include this header with appropriate fields set as part of the
/// response message to the HCI.
///
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


///
/// Defines for GroupID
///
#define MKHI_CBM_GROUP_ID     0x00
#define MKHI_FWCAPS_GROUP_ID  0x03
#define MKHI_HMRFPO_GROUP_ID  0x05
#define MKHI_MCA_GROUP_ID     0x0A
#define MKHI_GEN_GROUP_ID     0xFF

///
/// Defines for FWCAPS_GROUP Command
///
#define FWCAPS_GET_RULE_CMD               0x02

#define IAFW_DNX_REQ_CLEAR    0x02

///
/// Defines for Command
///
///
/// Defines for CBM_GROUP Command
///
#define CBM_RESET_CMD                     0x0B
///
/// Defines for FWCAPS_GROUP Command
///
///
/// Defines for HMRFPO_GROUP Command
///
///
/// Defines for MCA_GROUP Command
///
#define MCA_READ_FILE_EX_CMD              0x0A
#define MCA_ARB_SVN_COMMIT_CMD            0x1B
#define MCA_ARB_SVN_GET_INFO_CMD          0x1C
#define MCA_REVOKE_OEM_KEY_HASH_CMD       0x2F
#define MCA_GET_OEM_KEY_STATUS_CMD        0x0D

///
/// Defines for GEN_GROUP Command
///
#define GEN_END_OF_POST_CMD               0x0C
#define GEN_SET_MEASURED_BOOT_STATE_CMD   0x22
#define GEN_GET_MEASURED_BOOT_STATE_CMD   0x23

///
/// HW Anti-Rollback Key Usage
///
#define  MFT_KEY_USAGE_INDEX_CSE_RBE_MANIFEST      3
#define  MFT_KEY_USAGE_INDEX_BOOT_POLICY_MANIFEST 32
#define  MFT_KEY_USAGE_INDEX_OEM_KEY_MANIFEST     45
#define  MFT_KEY_USAGE_INDEX_OEM_BTG_KEY_MANIFEST 54
#define  MFT_KEY_USAGE_INDEX_OEM_ACM_MANIFEST     56

#define ARB_SVN_COMMIT_ALL                      0xFF ///< Value 0xFF refers to committing all pending ARBSVN values.

///
/// HECI BUS command
///
#define HECI_BUS_DISABLE_OPCODE           0x0C

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


typedef enum {
  KernelFeatureId = 0,
  AmtFeatureId    = 3
} FWCAPS_FEATURE_ID;
typedef enum {
  AmtBiosSyncInfoRuleTypeId     = 5,
  DisplayUserConsentRuleTypeId  = 10
} AMT_FEATURE_RULE_TYPE_ID;

typedef union {
  UINT32  Data;
  struct {
    UINT32  RuleTypeId : 16;
    UINT32  FeatureId  : 8;
    UINT32  Reserved   : 8;
  } Fields;
} RULE_ID;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              RuleId;
} GET_RULE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              RuleId;
  UINT8               RuleDataLen;
  UINT32              RuleData;
} GET_RULE_ACK;

typedef union {
  GET_RULE            Request;
  GET_RULE_ACK        Response;
} GET_RULE_BUFFER;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              RuleId;
  UINT8               RuleDataLen;
  UINT32              RuleData;
} SET_RULE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              RuleId;
} SET_RULE_ACK;

typedef union {
  SET_RULE            Request;
  SET_RULE_ACK        Response;
} SET_RULE_BUFFER;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} REQUEST_OWNERSHIP;

typedef union {
  REQUEST_OWNERSHIP   Request;
  REQUEST_OWNERSHIP   Response;
} REQUEST_OWNERSHIP_BUFFER;

///
/// Get Firmware Capability

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
} GET_FW_CAPSKU;

typedef struct {
  UINT32        RuleID;
  UINT8         RuleDataLen;
  MEFWCAPS_SKU  FWCap;
} GET_FW_CAPS_SKU_ACK_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER       MKHIHeader;
  GET_FW_CAPS_SKU_ACK_DATA  Data;
} GET_FW_CAPS_SKU_ACK;

///
/// Get/Set Local FW Update
///
typedef enum {
  UpdateDisabled,
  UpdateEnabled
} LOCAL_FW_UPDATE;

///
/// Get Platform Type
///
typedef enum {
  NoBrand,
  IntelAmtBrand,
  IntelStandardManageabilityBrand,
  IntelReservedBrand1,
  IntelReservedBrand2,
  IntelReservedBrand3
} PLATFORM_BRAND;

typedef enum {
  IntelMeConsumerFw        = 3,
  IntelMeCorporateFw
} ME_IMAGE_TYPE;

typedef enum {
  RegularSku,
  SuperSku
} SKU_TYPE;

typedef enum {
  PlatformMobile          = 1,
  PlatformDesktop         = 2,
  PlatformServer          = 4,
  PlatformWorkstation     = 8
} PLATFORM_TARGET_USAGE_TYPE;

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

///
/// Set ME Disable
///

#define ME_DISABLE         0

///
/// Set ACM TPM

typedef union {
  UINT32 Data;
  struct {
    UINT32 Reserved      : 8;
    UINT32 TpmDeactivate : 1;
    UINT32 Reserved1     : 23;
  } Fields;
} TPM_STATE;

///
/// Unconfig On Rtc Clear Status
///
typedef enum {
  EnableState  = 0,  ///< [Unconfigure on RTC Clear] is enabled.
  DisableState = 1   ///< [Unconfigure on RTC Clear] is disabled.
} ME_UNCONFIG_ON_RTC_CLEAR_STATUS;


///
/// HMRFPO ENABLE
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              Reserved[2];
} HMRFPO_ENABLE;


typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              Reserved[2];
  UINT8               Status;
  UINT8               Reserved1[3];
} HMRFPO_ENABLE_ACK;

typedef union {
  HMRFPO_ENABLE          Request;
  HMRFPO_ENABLE_ACK      Response;
} HMRFPO_ENABLE_BUFFER;

///
/// HMRFPO LOCK
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} HMRFPO_LOCK;


typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              Reserved[4];
  UINT8               Status;
  UINT8               Reserved1[3];
} HMRFPO_LOCK_ACK;

typedef union {
  HMRFPO_LOCK          Request;
  HMRFPO_LOCK_ACK      Response;
} HMRFPO_LOCK_BUFFER;

///
/// HMRFPO GET STATUS
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} HMRFPO_GET_STATUS;


typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8               Status;
  UINT8               Reserved[3];
} HMRFPO_GET_STATUS_ACK;

typedef union {
  HMRFPO_GET_STATUS          Request;
  HMRFPO_GET_STATUS_ACK      Response;
} HMRFPO_GET_STATUS_BUFFER;


///
/// Read File Ex
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              FileId;
  UINT32              Offset;
  UINT32              DataSize;
  UINT8               Flags;
} READ_FILE_EX;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              DataSize;
  UINT8               Data[];    ///< Data Buffer
} READ_FILE_EX_ACK;

typedef union {
  READ_FILE_EX        Request;
  READ_FILE_EX_ACK    Response;
} READ_FILE_EX_BUFFER;

///
/// Set File Ex
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              FileId;
  UINT32              Offset;
  UINT32              DataSize;
  UINT8               Flags;
  UINT8               Data[];     ///< Data Buffer
} SET_FILE_EX;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} SET_FILE_EX_ACK;

typedef union {
  SET_FILE_EX         Request;
  SET_FILE_EX_ACK        Response;
} SET_FILE_EX_BUFFER;

///
/// Commit Files
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} COMMIT_FILES;

///
/// Core Bios Done
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} CORE_BIOS_DONE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} CORE_BIOS_DONE_ACK;

///
/// HW Anti-Rollback (ARB) Security Version Number Commit
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8               UsageId;
  UINT8               Reserved[3]; ///< Reserved for future use, to allow specifying explicit usage to be committed.
} ARB_SVN_COMMIT;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} ARB_SVN_COMMIT_ACK;

typedef union {
  ARB_SVN_COMMIT       Request;
  ARB_SVN_COMMIT_ACK   Response;
} ARB_SVN_COMMIT_BUFFER;

///
/// HW Anti-Rollback (ARB) Security Version Number Get Info
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} ARB_SVN_GET_INFO;

typedef struct {
  UINT8           UsageId;              ///< Key usage number according to FAS manifest.
  /* Used in SPS to indicate partition.
     In client the SVN is same for all partitions
     0 - Boot partition 1 (RCV in SPS)
     1 - Boot partition 2 (OPR1 in SPS)
     2 - Boot partition 3 (OPR2 in SPS)
  */
  UINT8           BootPartitionId;
  UINT8           ExecutingSvn;         ///< The SVN value of the FW currently running.
  UINT8           MinAllowedSvn;        ///< The minimal SVN allowed on platform for given usage.
} ARB_SVN_INFO_ENTRY;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              NumEntries;       ///< Number of ARB SVN Entry
  ARB_SVN_INFO_ENTRY  ArbSvnEntry [0];
} ARB_SVN_GET_INFO_ACK;

typedef union {
  ARB_SVN_GET_INFO       Request;
  ARB_SVN_GET_INFO_ACK   Response;
} ARB_SVN_GET_INFO_BUFFER;

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

typedef union {
  UINT32 Data;
  struct {
    UINT32 Minor : 16;
    UINT32 Major : 16;
  } Fields;
} MKHI_VERSION;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} GET_MKHI_VERSION;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  MKHI_VERSION        MkhiVersion;
} GET_MKHI_VERSION_ACK;

typedef union {
  GET_MKHI_VERSION     Request;
  GET_MKHI_VERSION_ACK Response;
} GET_MKHI_VERSION_BUFFER;

///
/// Get FW Version
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} GET_FW_VER;

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
} GET_FW_VER_ACK_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  GET_FW_VER_ACK_DATA Data;
} GET_FW_VER_ACK;

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
/// CSME Measured Boot
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
/// End of Post
///
#define EOP_SEND_IN_PEI 1
#define EOP_SEND_IN_DXE 2

#define MAX_EOP_SEND_RETRIES 3

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} END_OF_POST;

typedef enum {
  HeciEopStatusSuccess,
  HeciEopPerformGlobalReset
} EOP_REQUESTED_ACTIONS;

typedef struct {
  UINT32  RequestedActions;
} EOP_ACK_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  EOP_ACK_DATA        Data;
} END_OF_POST_ACK;

typedef union {
 END_OF_POST     Request;
 END_OF_POST_ACK Response;
} END_OF_POST_BUFFER;

typedef union {
  UINT8 Data;
  struct {
    UINT8 Command    : 7;
    UINT8 IsResponse : 1;
  } Fields;
} HBM_COMMAND;

typedef struct {
  HBM_COMMAND Command;
  UINT8       Reserved[3];
} HECI_BUS_DISABLE_CMD;

typedef struct {
  HBM_COMMAND Command;
  UINT8       Status;
  UINT8       Reserved[2];
} HECI_BUS_DISABLE_CMD_ACK;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} GEN_GET_ME_UNCFG_STATE;

///
/// FW Feature Shipment Time State Override
///
typedef struct {
  UINT32  EnableFeature;
  UINT32  DisableFeature;
} FW_FEATURE_SHIPMENT_TIME_STATE_OVERRIDE_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER                          MkhiHeader;
  FW_FEATURE_SHIPMENT_TIME_STATE_OVERRIDE_DATA FeatureState;
} FW_FEATURE_SHIPMENT_TIME_STATE_OVERRIDE;

///
/// FW_FEATURE_SHIPMENT_TIME_STATE_OVERRIDE returned Status
///
typedef enum {
  SetFeatureStateAccepted,
  SetFeatureStateRejected
} FW_FEATURE_SHIPMENT_TIME_STATE_OVERRIDE_RESPONSE;

typedef struct {
  MKHI_MESSAGE_HEADER   MkhiHeader;
  UINT32                Status;
} FW_FEATURE_SHIPMENT_TIME_STATE_OVERRIDE_ACK;

typedef union {
  FW_FEATURE_SHIPMENT_TIME_STATE_OVERRIDE      Request;
  FW_FEATURE_SHIPMENT_TIME_STATE_OVERRIDE_ACK  Response;
} FW_FEATURE_SHIPMENT_TIME_STATE_OVERRIDE_BUFFER;

///
/// Set Manufacturing ME Reset and Halt messages
///
typedef struct {
  MKHI_MESSAGE_HEADER   MkhiHeader;
} SET_ME_RESET_HALT;

typedef struct {
  MKHI_MESSAGE_HEADER   MkhiHeader;
} SET_ME_RESET_HALT_ACK;

typedef struct {
  MKHI_MESSAGE_HEADER  MkhiHeader;
  UINT32               Flag;
} IAFW_DNX_REQ_CLEAR_REQ_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER  MkhiHeader;
} IAFW_DNX_REQ_CLEAR_RESP_DATA;

typedef union {
  IAFW_DNX_REQ_CLEAR_REQ_DATA  *Request;
  IAFW_DNX_REQ_CLEAR_RESP_DATA *Response;
} IAFW_DNX_REQ_CLEAR_BUFFER;

typedef struct {
  MKHI_MESSAGE_HEADER  MKHIHeader;
} IAFW_DNX_REQ_SET_REQ_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER  MKHIHeader;
  UINT32               ReqBiosAction;
} IAFW_DNX_REQ_SET_RESP_DATA;

#pragma pack()

#endif // _MKHI_MSGS_H
