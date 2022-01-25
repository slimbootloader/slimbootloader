/**@file
  MKHI Messages

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _MKHI_MSGS_ADL_H
#define _MKHI_MSGS_ADL_H

#pragma pack(1)

///
/// Defines for GroupID
///
#define MKHI_MCA_GROUP_ID     0x0A
#define MKHI_GEN_GROUP_ID     0xFF


///
/// Defines for Command
///
///
/// Defines for CBM_GROUP Command
///
///
/// Defines for PWD_GROUP Command
///
///
/// Defines for FWCAPS_GROUP Command
///
///
/// Defines for HMRFPO_GROUP Command
///
///
/// Defines for MCA_GROUP Command
///
#define MCA_REVOKE_OEM_KEY_HASH_CMD       0x2F
#define MCA_GET_OEM_KEY_STATUS_CMD        0x0D
///
/// Defines for DNX_GROUP Command
///
///
/// Defines for GEN_GROUP Command
///
#define GEN_SET_MEASURED_BOOT_STATE_CMD           0x22
#define GEN_GET_MEASURED_BOOT_STATE_CMD           0x23

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

///
/// MKHI_CBM_GROUP Definitions
///
typedef union {
  UINT32 Data;
  struct {
    UINT32 TableSyncError        : 1;   ///< BIT_0  No longer used
    UINT32 CpuReplacement        : 1;   ///< BIT_1
    UINT32 SyncMngState          : 1;   ///< BIT_2
    UINT32 SolStateChange        : 1;   ///< BIT_3  No longer used
    UINT32 IderStateChange       : 1;   ///< BIT_4  No longer used
    UINT32 KvmStateChange        : 1;   ///< BIT_5  No longer used
    UINT32 SyncRaAvailability    : 1;   ///< BIT_6
    UINT32 FwUnconfiguration     : 1;   ///< BIT_7
    UINT32 UserConsentFlow       : 1;   ///< BIT_8
    UINT32 WlanPowerConfigChange : 1;   ///< BIT_9
    UINT32 FwUpdate              : 1;   ///< BIT_10 No longer used
    UINT32 FirstMebxBoot         : 1;   ///< BIT_11
    UINT32 DataSyncConfirmation  : 1;   ///< BIT_12
    UINT32 ClsUserInfo           : 1;   ///< BIT_13 No longer used
    UINT32 Reserved2             : 18;  ///< BIT_14-31
  } Fields;
} INVOCATION_CODE_BITS;


//
// Get Invocation Code
//
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} GET_INVOCATION_CODE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              InvocationCode;
} GET_INVOCATION_CODE_ACK;

typedef union {
  GET_INVOCATION_CODE     Request;
  GET_INVOCATION_CODE_ACK Response;
} GET_INVOCATION_CODE_BUFFER;

//
// Set Invocation Code
//
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              InvocationCode;
} SET_INVOCATION_CODE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} SET_INVOCATION_CODE_ACK;

typedef union {
  SET_INVOCATION_CODE     Request;
  SET_INVOCATION_CODE_ACK Response;
} SET_INVOCATION_CODE_BUFFER;

//
// Clear Invocation Code
//
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              InvocationCode;
} CLEAR_INVOCATION_CODE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} CLEAR_INVOCATION_CODE_ACK;

typedef union {
  CLEAR_INVOCATION_CODE     Request;
  CLEAR_INVOCATION_CODE_ACK Response;
} CLEAR_INVOCATION_CODE_BUFFER;


#define MEPWDMGMT_PWD_MAX_SIZE 32   ///< Maximum Password Length

//
// Password Management Status Codes.
//
typedef enum {
  ME_PWDSTS_PWD_SUCCESS = 0,        ///< Password change/verification was successful.
  ME_PWDSTS_INVALID_PWD,            ///< Current password in use is invalid.
  ME_PWDSTS_INVALID_NEW_PWD,        ///< New password provided does not meet the required syntax.
  ME_PWDSTS_OUT_OF_RESOURCES,       ///< Not sufficient flash space.
  ME_PWDSTS_OPERATION_NOT_ALLOWED,  ///< Password change/verification cannot be performed at this point in time.
  ME_PWDSTS_FAILURE                 ///< Unknown failure has occured
} ME_PWD_STATUS;

//
// Change Password
//
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              PwdId;
  UINT8               Reserved[2];
  UINT8               OldPwdSize;
  UINT8               OldPwd[MEPWDMGMT_PWD_MAX_SIZE];
  UINT8               NewPwdSize;
  UINT8               NewPwd[MEPWDMGMT_PWD_MAX_SIZE];
} PWD_CHANGE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              PwdId;
} PWD_CHANGE_ACK;

typedef union {
  PWD_CHANGE       Request;
  PWD_CHANGE_ACK   Response;
} PWD_CHANGE_BUFFER;

//
// Validate Password
//
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              PwdId;
  UINT8               Reserved[3];
  UINT8               PwdSize;
  UINT8               Pwd[MEPWDMGMT_PWD_MAX_SIZE];
} PWD_VALIDATE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              PwdId;
  UINT32              MaxCountExpired;
} PWD_VALIDATE_ACK;

typedef union {
  PWD_VALIDATE     Request;
  PWD_VALIDATE_ACK Response;
} PWD_VALIDATE_BUFFER;

//
// Is Password Modified
//
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} PWD_IS_MODIFIED;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              IsModified;
} PWD_IS_MODIFIED_ACK;

typedef union {
  PWD_IS_MODIFIED     Request;
  PWD_IS_MODIFIED_ACK Response;
} PWD_IS_MODIFIED_BUFFER;


//Macro to build a rule identifier. For Me rules all other fields are zeros
#define FWCAPS_RULE_ID(FeatureId, RuleTypeId) ((FeatureId << 16) | RuleTypeId)

typedef enum {
  KernelFeatureId = 0,
  AmtFeatureId    = 3
} FWCAPS_FEATURE_ID;

typedef enum {
  FwCapsRuleTypeId              = 0,
  MePlatformStateRuleTypeId     = 6,
  MeFwUpdateRuleTypeId          = 7,
  UserCapsRuleTypeId            = 9,
  PlatTypeRuleTypeId            = 29,
  FeatureStateRuleTypeId        = 32,
  StateForAllFeaturesRuleTypeId = 33,
  EnableClinkRuleTypeId         = 38,
  AvailableBitmapRuleTypeId     = 39,
  EnableMoffOverrideRuleTypeId  = 41,
  OemTagRuleTypeId              = 43,
  AcmTpmRuleTypeId              = 47,
  UnconfigOnRtcClarRuleTypeId   = 48
} KERNEL_FEATURE_RULE_TYPE_ID;

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

///
/// Get/Set Local FW Update
///
typedef enum {
  UpdateDisabled,
  UpdateEnabled
} LOCAL_FW_UPDATE;


/// [3:0]
typedef enum {
  PlatformMobile          = 1,
  PlatformDesktop         = 2,
  PlatformServer          = 4,
  PlatformWorkstation     = 8
} PLATFORM_TARGET_USAGE_TYPE;

/// [6]
typedef enum {
  RegularSku,
  SuperSku
} SKU_TYPE;

/// [11:8]
typedef enum {
  IntelMeConsumerFw        = 3,
  IntelMeCorporateFw       = 4,
  IntelMeLiteFw            = 5
} ME_IMAGE_TYPE;

/// [15:12]
typedef enum {
  NoBrand,
  IntelAmtBrand,
  IntelStandardManageabilityBrand,
  IntelReservedBrand1,
  IntelReservedBrand2,
  IntelReservedBrand3
} PLATFORM_BRAND;


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

typedef enum {
  AmtProvisioningStatePre  = 0,
  AmtProvisioningStateIn   = 1,
  AmtProvisioningStatePost = 2
} AMT_PROVISIONING_STATE;

typedef struct {
  UINT32 Reserved                : 1;
  UINT32 CiraAvailable           : 1; ///< CIRA Available
  UINT32 ProvisioningAvailable   : 1; ///< Corporate Provisioning Available
  UINT32 ProvisioningState       : 2; ///< Provisioning state. @see AMT_PROVISIONING_STATE
  UINT32 Reserved1               : 27;
} AMT_BIOS_SYNC_INFO;


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
/// Read File, Ver3 API only
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8               FilePath[64];
  UINT32              Offset;
  UINT32              DataSize;
  UINT8               Flags;
} READ_FILE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              DataSize;
  UINT8               Data[];    ///< Data Buffer
} READ_FILE_ACK;

typedef union {
  READ_FILE           Request;
  READ_FILE_ACK       Response;
} READ_FILE_BUFFER;

///
/// Set File, Ver3 API only
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8               FilePath[64];
  UINT32              Offset;
  UINT32              DataSize;
  UINT8               Flags;
  UINT8               Data[];     ///< Data Buffer
} SET_FILE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} SET_FILE_ACK;

typedef union {
  SET_FILE            Request;
  SET_FILE_ACK        Response;
} SET_FILE_BUFFER;

///
/// Commit Files
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} COMMIT_FILES;

///
/// Core Bios Done - both GEN and MCA group definition
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} CORE_BIOS_DONE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} CORE_BIOS_DONE_ACK;

///
/// Get RPMC Status
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} GET_RPMC_STATUS;

typedef struct {
  UINT32  DeviceType         : 2; ///< Device Type. 00: EMMC; 01: UFS; 10: SPI; 11: No device
  UINT32  RpSupport          : 1; ///< Relay Protection. 0: Not supported; 1: Support NVM and FW configuraion
  UINT32  BindStatus         : 1; ///< 0: Pre-bind; 1: Post-bind
  UINT32  RpRebindingAllowed : 1; ///< Rebind. 0: Not allowed 1: Allowed
  UINT32  BindingCounter     : 5; ///< Current binding counter
  UINT32  MaxRebinds         : 4; ///< Number of max rebinds allowed
  UINT32  SpiRpmcCounters    : 3; ///< Number of RPMC counters avaiable (0 if EMMC/UFS)
  UINT32  SpiRpmcChipselect  : 1; ///< 0 if EMMC/UFS
  UINT32  SpiRpmcFatalError  : 8; ///< 0 if EMMC/UFS
  UINT32  Reserved           : 6;
} RP_STATUS_2;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  RP_STATUS_2         Status;
} GET_RPMC_STATUS_ACK;

typedef union {
  GET_RPMC_STATUS     Request;
  GET_RPMC_STATUS_ACK Response;
} GET_RPMC_STATUS_BUFFER;

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
  SET_FILE_EX_ACK     Response;
} SET_FILE_EX_BUFFER;

///
/// Request Device Ownership
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} REQUEST_OWNERSHIP;

typedef union {
  REQUEST_OWNERSHIP   Request;
  REQUEST_OWNERSHIP   Response;
} REQUEST_OWNERSHIP_BUFFER;


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


///
/// DNX Request Set
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} DNX_REQ_SET;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              BiosAction;
} DNX_REQ_SET_ACK;

typedef union {
  DNX_REQ_SET     Request;
  DNX_REQ_SET_ACK Response;
} DNX_REQ_SET_BUFFER;

///
/// DNX Request Clear
///
typedef struct {
  MKHI_MESSAGE_HEADER  MkhiHeader;
  UINT32               Flag;
} DNX_REQ_CLEAR;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} DNX_REQ_CLEAR_ACK;

typedef union {
  DNX_REQ_CLEAR     Request;
  DNX_REQ_CLEAR_ACK Response;
} DNX_REQ_CLEAR_BUFFER;


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
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8               CpuBrandClass;
} GEN_SEND_CPU_BRAND;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} GEN_SEND_CPU_BRAND_ACK;

typedef union {
  GEN_SEND_CPU_BRAND     Request;
  GEN_SEND_CPU_BRAND_ACK Response;
} GEN_SEND_CPU_BRAND_BUFFER;

//
// Get CPU Type Change message
//
#define MAX_SKU_BITMAP            8 ///< CPU type change response feature masks size in DWORDS

typedef struct {
  UINT8               UserFeedback;
  UINT32              FeaturesDisabled[MAX_SKU_BITMAP];
  UINT32              FeaturesEnabled[MAX_SKU_BITMAP];
  UINT8               GlobalResetRequired;
} GEN_GET_CPU_TYPE_CHANGE_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} GEN_GET_CPU_TYPE_CHANGE;

typedef struct {
  MKHI_MESSAGE_HEADER          MkhiHeader;
  GEN_GET_CPU_TYPE_CHANGE_DATA Data;
} GEN_GET_CPU_TYPE_CHANGE_ACK;

typedef union {
  GEN_GET_CPU_TYPE_CHANGE     Request;
  GEN_GET_CPU_TYPE_CHANGE_ACK Response;
} GEN_GET_CPU_TYPE_CHANGE_BUFFER;


typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8               UserResponse;
} GEN_SET_CPU_TYPE_CHANGE;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} GEN_SET_CPU_TYPE_CHANGE_ACK;

typedef union {
  GEN_SET_CPU_TYPE_CHANGE     Request;
  GEN_SET_CPU_TYPE_CHANGE_ACK Response;
} GEN_SET_CPU_TYPE_CHANGE_BUFFER;


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

//
// Unconfig without password messages
//
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} UNCONFIG_WO_PASSWORD;


typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} GEN_GET_ME_UNCFG_STATE;

///
/// Set Manufacturing ME Reset and Halt messages
///
typedef struct {
  MKHI_MESSAGE_HEADER   MkhiHeader;
} SET_ME_RESET_HALT;

typedef struct {
  MKHI_MESSAGE_HEADER   MkhiHeader;
} SET_ME_RESET_HALT_ACK;

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

//
// FW Feature Shipment Time State Override returned Status
//
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


typedef struct {
  MKHI_MESSAGE_HEADER   MkhiHeader;
  UINT32                PartitionId;
} GET_IMAGE_FW_VER;

typedef struct {
  UINT16                Major;
  UINT16                Minor;
  UINT16                Hotfix;
  UINT16                Build;
} FW_VERSION;

typedef struct {
  UINT32                PartitionId;   ///< Partition ID
  UINT8                 Reserved[8];   ///< Should be 0
  FW_VERSION            Version;       ///< Module version
  UINT32                VendorId;      ///< Module vendor ID(0x8086 for Intel)
  UINT32                TcbSvn;        ///< TCB SVN
  UINT32                Reserved1[15]; ///< Reserved bytes for future extension. Should be 0 while reserved
} FLASH_PARTITION_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER   MkhiHeader;
  UINT32                NumOfModules;
  FLASH_PARTITION_DATA  ManifestData;
} GET_IMAGE_FW_VER_ACK;

typedef union {
  GET_IMAGE_FW_VER      Request;
  GET_IMAGE_FW_VER_ACK  Response;
} GET_IMAGE_FW_VERSION_BUFFER;

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

#pragma pack()

#endif // _MKHI_MSGS_H
