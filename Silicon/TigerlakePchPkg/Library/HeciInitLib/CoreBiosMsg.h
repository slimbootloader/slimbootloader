/** @file
  Core BIOS Messages

  Copyright (c) 2008 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef CORE_BIOS_MSG_H
#define CORE_BIOS_MSG_H

#include "MkhiMsgs.h"

#pragma pack(1)

///
/// Host Address of Fixed Address MEI Header -
/// This is the logical address of the Host client of the message. This address
/// is assigned when the host client registers itself with the Host MEI driver.

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
#define HECI_HBM_MESSAGE_ADDR               0x00
#define HECI_MKHI_MESSAGE_ADDR              0x07
#define HECI_MCHI_MESSAGE_ADDR              0x09
#define HECI_MAX_ME_FIXED_ADDR              0x1F  ///< Max ME address for fixed clients


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


//
// definitions for ICC MEI Messages
//
typedef enum {
  ICC_STATUS_SUCCESS                           = 0x0,
  ICC_STATUS_FAILURE,
  ICC_STATUS_INCORRECT_API_VERSION,
  ICC_STATUS_INVALID_FUNCTION,
  ICC_STATUS_INVALID_BUFFER_LENGTH,
  ICC_STATUS_INVALID_PARAMS,
  ICC_STATUS_FLASH_WEAR_OUT_VIOLATION,
  ICC_STATUS_FLASH_CORRUPTION,
  ICC_STATUS_PROFILE_NOT_SELECTABLE_BY_BIOS,
  ICC_STATUS_TOO_LARGE_PROFILE_INDEX,
  ICC_STATUS_NO_SUCH_PROFILE_IN_FLASH,
  ICC_STATUS_CMD_NOT_SUPPORTED_AFTER_END_OF_POST,
  ICC_STATUS_NO_SUCH_RECORD,
  ICC_STATUS_RESERVED13,
  ICC_STATUS_RESERVED14,
  ICC_STATUS_TOO_LARGE_UOB_RECORD,
  ICC_STATUS_RESERVED16,
  ICC_STATUS_REGISTER_IS_LOCKED,
  ICC_STATUS_RESERVED18,
  ICC_STATUS_RESERVED19,
  ICC_STATUS_RESERVED20,
  ICC_STATUS_RESERVED21,
  ICC_STATUS_RESERVED22,
  ICC_STATUS_RESERVED23,
  ICC_STATUS_UOB_RECORD_IS_ALREADY_INVALID,
  ICC_STATUS_RESERVED25,
  ICC_STATUS_RESERVED26,
  ICC_STATUS_RESERVED27,
  ICC_STATUS_RANGE_VIOLATION_FREQ_TOO_HIGH,
  ICC_STATUS_RESERVED29,
  ICC_STATUS_RESERVED30,
  ICC_STATUS_RESERVED31,
  ICC_STATUS_RESERVED32,
  ICC_STATUS_RESERVED33,
  ICC_STATUS_RESERVED34,
  ICC_STATUS_RESERVED35,
  ICC_STATUS_RANGE_VIOLATION_FREQ_TOO_LOW,
  ICC_STATUS_RESERVED37,
  ICC_STATUS_RESERVED38,
  ICC_STATUS_RESERVED39,
  ICC_STATUS_RESERVED40,
  ICC_STATUS_RESERVED41,
  ICC_STATUS_RESERVED42,
  ICC_STATUS_RESERVED43,
  ICC_STATUS_SSC_MODE_CHANGE_NOT_SUPPORTED,
  ICC_STATUS_RESERVED45,
  ICC_STATUS_RESERVED46,
  ICC_STATUS_RESERVED47,
  ICC_STATUS_RESERVED48,
  ICC_STATUS_RESERVED49,
  ICC_STATUS_RESERVED50,
  ICC_STATUS_RESERVED51,
  ICC_STATUS_NO_SUCH_TARGET_ID,
  ICC_STATUS_NO_SUCH_REGISTER,
  ICC_STATUS_RESERVED54,
  ICC_STATUS_BUFFER_TOO_SMALL,
  ICC_STATUS_RESERVED56,
  ICC_STATUS_WAITING_FOR_POWER_CYCLE,
  ICC_STATUS_SURVIVABILITY_TABLE_ACCESS_VIOLATION,
  ICC_STATUS_SURVIVABILITY_TABLE_TOO_LARGE,
  ICC_STATUS_NO_SUCH_EID,
  ICC_STATUS_RESERVED61,
  ICC_STATUS_UNSPECIFIED_ERROR               = 0xFFFF
} ICC_MEI_CMD_STATUS;

typedef enum {
  ///
  /// Commands 0-15 except 10 not used.
  ///
  ICC_READ_FROM_MPHY_CMD                    = 0xA,
  ICC_GET_PROFILE_CMD                       = 0x10,
  ICC_SET_PROFILE_CMD                       = 0x11,
  ICC_GET_REGISTERS_LOCK_CMD                = 0x12,
  ICC_SET_REGISTERS_LOCK_CMD                = 0x13,
  ICC_GET_CLOCK_CAPABILITIES_CMD            = 0x14,
  ICC_GET_CLOCK_RANGE_DEF_REC_CMD           = 0x15,
  ICC_GET_RECORD_CMD                        = 0x16,
  ICC_SET_RECORD_CMD                        = 0x17,
  ICC_GET_HSIO_SETTINGS_CMD                 = 0x18,
  ICC_SET_HSIO_SETTINGS_CMD                 = 0x19,
  ICC_GET_CLOCK_SETTINGS_CMD                = 0x1A,
  ICC_SET_CLOCK_SETTINGS_CMD                = 0x1B,
} ICC_MEI_COMMAND_ID;

typedef struct {
  UINT32              ApiVersion;
  ICC_MEI_COMMAND_ID  IccCommand;
  ICC_MEI_CMD_STATUS  IccResponse;
  UINT32              BufferLength;
  UINT32              Reserved;
} ICC_HEADER;

///
/// Get/Set ICC Profile
///
#define MAX_NUM_ICC_PROFILES  16

typedef struct {
  ICC_HEADER  Header;
} ICC_GET_PROFILE_MESSAGE;

typedef struct {
  UINT8       IccProfileName[24];
  UINT8       IccProfileBase[24];
} ICC_PROFILE_DESC;

typedef struct {
  ICC_HEADER    Header;
  UINT8         IccNumOfProfiles;
  union {
    UINT8 Data;
    struct {
      UINT8     IccFailsafeProfile  : 4;
      UINT8     Reserved            : 3;
      UINT8     IccRuntimeSelect    : 1;
    } Fields;
  } OemParams;
  UINT8             IccProfileIndex;
  UINT8             Reserved;
  ICC_PROFILE_DESC  IccProfileDesc[MAX_NUM_ICC_PROFILES];
} ICC_GET_PROFILE_RESPONSE;

typedef union {
  ICC_GET_PROFILE_MESSAGE   Message;
  ICC_GET_PROFILE_RESPONSE  Response;
} ICC_GET_PROFILE_BUFFER;

typedef struct {
  ICC_HEADER  Header;
  UINT8       BiosIccProfile;
  UINT8       PaddingA;
  UINT16      PaddingB;
} ICC_SET_PROFILE_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
} ICC_SET_PROFILE_RESPONSE;

typedef union {
  ICC_SET_PROFILE_MESSAGE   Message;
  ICC_SET_PROFILE_RESPONSE  Response;
} ICC_SET_PROFILE_BUFFER;

///
/// Get/Set ICC Clock Capabilities
///
typedef struct {
  ICC_HEADER  Header;
} ICC_GET_CLK_CAPABILITIES_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
  UINT32      VersionNumber;
  UINT8       IccHwSku;
  UINT8       Reserved;
  UINT16      MaxSusramRecordSize;
  UINT64      IccSkuEnforcementTable;
  UINT32      IccBootStatusReport;
} ICC_GET_CLK_CAPABILITIES_RESPONSE;

typedef union {
  ICC_GET_CLK_CAPABILITIES_MESSAGE  Message;
  ICC_GET_CLK_CAPABILITIES_RESPONSE Response;
} ICC_GET_CLK_CAPABILITIES_BUFFER;


///
/// Used for the FLAGS parameter of Get/Set Clock Settings
///
typedef union {
  UINT8 u;
  struct {
    UINT8 RoundDown   : 1;
    UINT8 Reserved    : 7;
  } Flags;
} ICC_CLOCK_SETTINGS_FLAGS;

///
/// Used for the CURRENT FLAGS parameter of Get/Set Clock Settings
///
typedef union {
  UINT16 u;
  struct {
    UINT16 Reserved           : 3;  ///< Reserved.
    UINT16 ForcePowerFlow     : 1;  ///< This indicates that the clock settings will include a power flow, depending on the specific platform.
    /**
      Specifies how the settings with associated clock were applied.
      0 - Persistent
      1 - Dynamic , indicating a run time ad-hoc change (ramp)
      2 - Reserved
      3 - Reserved
    **/
    UINT16 Type               : 2;
    UINT16 SetToDefault       : 1;  ///< Revert this clock to nominal.
    UINT16 Reserved1          : 3;  ///< Reserved.
    /**
      New Boot settings applied and power cycle reset pending.
      Flag cleared once boot settings are consumed.
      Reserved in Set Clock Settings.
    **/
    UINT16 PwrCyclePending    : 1;
    UINT16 Reserved2          : 5;
  } Flags;
} ICC_CLOCK_SETTINGS_CURRENT_FLAGS;

///
/// Used for the SUPPORTED FLAGS parameter of Get/Set Clock Settings
///
typedef union {
  UINT16 u;
  struct {
    UINT16 Reserved           : 1;  ///< Reserved.
    UINT16 DownSpread         : 1;  ///< Down spread SSC supported.
    UINT16 UpSpread           : 1;  ///< Up spread SSC supported.
    UINT16 CenterSpread       : 1;  ///< Center spread SSC supported.
    UINT16 ChangeAllowed      : 1;  ///< Frequency & Spread changes allowed.
    UINT16 HaltAllowed        : 1;  ///< Spread can be enabled or disabled.
    UINT16 PCIe               : 1;  ///< Clock uses the PCIe clock source
    UINT16 Bclk               : 1;  ///< Clock uses the BCLK clock source
    UINT16 Reserved1          : 8;  ///< Reserved.
  } Flags;
} ICC_CLOCK_SETTINGS_SUPPORTED_FLAGS;

///
/// Combined Request and Response structure for Get/Set Clock Settings
///
typedef struct {
  ICC_HEADER                         Header;
  UINT8                              ReqClock;
  UINT8                              SettingType;
  ICC_CLOCK_SETTINGS_FLAGS           Flags;
  UINT8                              Reserved;
  UINT32                             Freq;
  UINT32                             UserFreq;
  UINT32                             MaxFreq;
  UINT32                             MinFreq;
  UINT8                              SscMode;
  UINT8                              SscPercent;
  UINT8                              MaxSscPercent;
  UINT8                              Reserved1;
  ICC_CLOCK_SETTINGS_CURRENT_FLAGS   CurrentFlags;
  ICC_CLOCK_SETTINGS_SUPPORTED_FLAGS SupportFlags;
} ICC_GETSET_CLOCK_SETTINGS_REQRSP;

typedef union {
  ICC_GETSET_CLOCK_SETTINGS_REQRSP  Message;
  ICC_GETSET_CLOCK_SETTINGS_REQRSP  Response;
} ICC_GET_CLOCK_SETTINGS_BUFFER;

typedef union {
  ICC_GETSET_CLOCK_SETTINGS_REQRSP  Message;
  ICC_GETSET_CLOCK_SETTINGS_REQRSP  Response;
} ICC_SET_CLOCK_SETTINGS_BUFFER;

#pragma pack()

#endif
