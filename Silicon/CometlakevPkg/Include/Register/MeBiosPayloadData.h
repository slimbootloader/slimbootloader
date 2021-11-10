/** @file
  Interface definition details for MBP during DXE phase.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _MBP_DATA_PROTOCOL_H_
#define _MBP_DATA_PROTOCOL_H_
#include <Register/CoreBiosMsg.h>


///
/// Revision 1:  Original version
/// Revision 2:  Deprecate MBP_ICC_PROFILE from ME_BIOS_PAYLOAD
/// Revision 3:  Add Anti-Rollback SVN support
///
#define MBP_DATA_PROTOCOL_REVISION  3


#pragma pack(push, 1)

///
/// Common MBP types
///
typedef union {
  UINT32 Data;
  struct {
    UINT32 MbpSize    : 8;  ///< Byte 0 - MBP Size in DW including Header
    UINT32 NumEntries : 8;  ///< Byte 1 - Number of Entries (Data Items) in MBP
    UINT32 Flags      : 8;  ///< Byte 2 - Flags
    UINT32 Rsvd       : 8;  ///< Byte 3 - Reserved
  } Fields;
} MBP_HEADER;

///
/// MBP items header
///
typedef union {
  UINT32 Data;
  struct {
    UINT32 AppId  : 8; ///< Byte 0 - Application ID
    UINT32 ItemId : 8; ///< Byte 1 - Item ID
    UINT32 Length : 8; ///< Byte 2 - Length in DW
    UINT32 Flags  : 8; ///< Byte 3 - Flags
  } Fields;
} MBP_ITEM_HEADER;

///
/// MBP items
///
typedef struct {
  UINT32  MajorVersion  : 16;
  UINT32  MinorVersion  : 16;
  UINT32  HotfixVersion : 16;
  UINT32  BuildVersion  : 16;
} MBP_FW_VERSION_NAME;

typedef struct {
   UINT8            IccProfileId   : 4;
   UINT8            Reserved       : 4;
} ICC_PROFILE_INDEX_TYPE;

typedef struct {
   UINT8                    IccNumOfProfiles;
   ICC_PROFILE_INDEX_TYPE   IccProfileIndex;
   UINT8                    IccProfileChangeable;
   UINT8                    IccLockMaskType;
} MBP_ICC_PROFILE;

typedef struct {
  MEFWCAPS_SKU  FwCapabilities;
  BOOLEAN       Available;
  UINT8         Reserved[3];
} MBP_FW_CAPS_SKU;

typedef struct {
  MEFWCAPS_SKU  FwFeatures;
  BOOLEAN       Available;
  UINT8         Reserved[3];
} MBP_FW_FEATURES_STATE;

typedef struct {
  UINT32  Key[8];
} MBP_PLATFORM_KEY;

typedef struct {
  PLATFORM_TYPE_RULE_DATA RuleData;
  BOOLEAN                 Available;
  UINT8                   Reserved[3];
} MBP_PLAT_TYPE;

typedef union {
  UINT32  Raw;
  struct {
    UINT32  MediaTablePush : 1;
    UINT32  Reserved       : 31;
  } Fields;
} HWA_DATA;

typedef struct {
  HWA_DATA Data;
  BOOLEAN  Available;
  UINT8    Reserved[3];
} MBP_HWA_REQ;

typedef struct {
  UINT32  PwrbtnMrst;
  UINT32  MrstPltrst;
  UINT32  PltrstCpurst;
} MBP_PERF_DATA;

typedef struct {
  MBP_PERF_DATA MbpPerfData;
  BOOLEAN       Available;
  UINT8         Reserved[3];
} PLAT_BOOT_PERF_DATA;

typedef struct {
  UINT32 DisUnconfigOnRtcClearState : 1;
  UINT32 Reserved                   : 31;
} MBP_ME_UNCONF_ON_RTC_DATA;

typedef struct {
  MBP_ME_UNCONF_ON_RTC_DATA   UnconfigOnRtcClearData;
  BOOLEAN                     Available;
  UINT8                       Reserved[3];
} MBP_ME_UNCONF_ON_RTC_STATE;

typedef struct {
  /*
    ARB SVN flags:
    BIT[0]: set if ARB SVN mismatch is found for any of ARB SVN components.
    BIT[1..6]: Reserved, should be 0
    BIT[7]: set if ARB_SVN exceeds 1 byte size
  */
  UINT8                       Flags;
  UINT8                       MinCseArbSvn;  ///< Minimal CSE ARB SVN.
  UINT8                       CurrCseArbSvn; ///< Current CSE ARB SVN.
  UINT8                       Reserved;      ///< Reserved, should be 0.
} MBP_ARB_SVN_DATA;

typedef struct {
  MBP_ARB_SVN_DATA            ArbSvnData;
  BOOLEAN                     Available;
  UINT8                       Reserved[3];
} MBP_ARB_SVN_STATE;

///
/// ME BIOS Payload structure containing insensitive data only
///
typedef struct {
  MBP_FW_VERSION_NAME        FwVersionName;
  MBP_FW_CAPS_SKU            FwCapsSku;
  MBP_FW_FEATURES_STATE      FwFeaturesState;
  MBP_PLAT_TYPE              FwPlatType;
  MBP_ICC_PROFILE            IccProfile; //@deprecated since version 2 : Replaced with GetIccProfile HECI
  MBP_HWA_REQ                HwaRequest;
  PLAT_BOOT_PERF_DATA        PlatBootPerfData;
  MBP_ME_UNCONF_ON_RTC_STATE UnconfigOnRtcClearState;
  UINT32                     ChipsetInitVerData[3];
  MBP_ARB_SVN_STATE          ArbSvnState;
} ME_BIOS_PAYLOAD;

///
/// All items which can't be available when 3rd part OPROMs/drivers are loaded
/// must be added to below structure only
///
typedef struct {
  MBP_PLATFORM_KEY           PlatformKey;
} ME_BIOS_PAYLOAD_SENSITIVE;

#pragma pack(pop)

#endif
