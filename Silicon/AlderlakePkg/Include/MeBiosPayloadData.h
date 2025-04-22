/**@file
  Interface definition details for MBP.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _ME_BIOS_PAYLOAD_DATA_H_
#define _ME_BIOS_PAYLOAD_DATA_H_
#include <CoreBiosMsg.h>

/**
  ME-BIOS Payload Data

  <b>Revision 1:</b>
  - Initial version.
  <b>Revision 2:</b>
  - Added Extended Performance data structure.
  - Deprecated MBP ICC profile structure.
**/
#define MBP_DATA_REVISION  2

#pragma pack (1)

///
/// Common MBP types
///
typedef struct {
  UINT8  MbpSize;    ///< Byte 0 - MBP Size in DW including Header
  UINT8  NumEntries; ///< Byte 1 - Number of Entries (Data Items) in MBP
  UINT16 Reserved;   ///< Byte [3:2] - Reserved
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

//
// Macro to build abstract MBP Item Id
//
#define MBP_ITEM_ID(ApplicationId, ItemId) ((ApplicationId << 5) | ItemId)

#define MBP_ITEM_ID_MEASURED_BOOT     MBP_ITEM_ID(MbpAppIdKernel, MbpItemIdMeasuredBoot)

//
// Enum for AppId
//
typedef enum {
  MbpAppIdKernel         = 1,
  MbpAppIdHwa            = 4,
  MbpAppIdIcc            = 5,
  MbpAppIdDnx            = 7
} MBP_APPLICATION_ID;

//
// Enums for ItemIds
//
typedef enum {
  MbpItemIdFwVerName                  = 1,
  MbpItemIdFwCapabilities             = 2,
  MbpItemIdBiosPlatformKey            = 4,
  MbpItemIdCsePlatformType            = 5,
  MbpItemIdUnconfigOnRtc              = 8,
  MbpItemIdShipState                  = 9,
  MbpItemIdPerfDataEx                 = 13,
  MbpItemIdFwArbSvn                   = 14,
  MbpItemIdMeasuredBoot               = 15,
  MbpItemIdOemKeyRevoke               = 16
} MBP_KERNEL_ITEM_ID;

typedef enum {
  MbpItemIdHwaMtu = 1
} MBP_HWA_ITEM_ID;

typedef enum {
  MbpItemIdMphyData   = 3,
  MbpItemIdOemPhyData = 4
} MBP_ICC_ITEM_ID;

//
// Enum for Flags
//
typedef enum {
  MbpItemFlagStoreToNvm = 0, ///< Item contains data to be stored in flash
  MbpItemFlagSensitive  = 1, ///< Item contains sensitive data.  Do not store in flash.
  MbpItemFlagRuntime    = 2, ///< Item is generated at runtime.  Do not store in flash.
} MBP_ITEM_FLAGS;


///
/// Kernel AppId
///
typedef struct {
  UINT32  MajorVersion  : 16;
  UINT32  MinorVersion  : 16;
  UINT32  HotfixVersion : 16;
  UINT32  BuildVersion  : 16;
} MBP_FW_VERSION_NAME;

typedef struct {
  MEFWCAPS_SKU  FwCapabilities;
  BOOLEAN       Available;
  UINT8         Reserved[3];
} MBP_FW_CAPS_SKU;

typedef struct {
  UINT32  Key[8];
} MBP_PLATFORM_KEY;

typedef struct {
  PLATFORM_TYPE_RULE_DATA RuleData;
  BOOLEAN                 Available;
  UINT8                   Reserved[3];
} MBP_PLAT_TYPE;

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
  MEFWCAPS_SKU  FwFeatures;
  BOOLEAN       Available;
  UINT8         Reserved[3];
} MBP_FW_FEATURES_STATE;

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
/// MBP IFWI DNX request structure containing IFWI Dnx request data
///
typedef struct {
  UINT32      EnterRecovery;
  BOOLEAN     Available;
  UINT8       Reserved[3];
} MBP_IFWI_DNX_REQUEST;

typedef struct {
  UINT32 MeasuredBoot   : 1;
  UINT32 Reserved       : 31;
} MBP_MEASURED_BOOT_DATA;

typedef struct {
  MBP_MEASURED_BOOT_DATA      MeasuredBootData;
  BOOLEAN                     Available;
  UINT8                       Reserved[3];
} MBP_MEASURED_BOOT_SUPPORT;

typedef struct {
  UINT32 TimeStamp0;
  UINT16 TimeStamp1;
  UINT16 TimeStamp2;
  UINT16 TimeStamp3;
  UINT16 TimeStamp4;
  UINT16 TimeStamp5;
  UINT16 TimeStamp6;
} MBP_PERF_DATA;

typedef struct {
  MBP_PERF_DATA Data;
  BOOLEAN       Available;
  UINT8         Reserved[3];
} MBP_PERF_DATA_EX;

///
/// HWA AppId
///
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

///
/// ICC AppId
///
typedef struct {
  UINT32  ChipsetInitVer[3];
  BOOLEAN Available;
  UINT8   Reserved[3];
} MBP_MPHY_DATA;

typedef struct {
  UINT64  Data;
  BOOLEAN Available;
  UINT8   Reserved[3];
} MBP_OEM_PHY_DATA;

typedef struct {
  UINT32                      OemKeyDataRsrvd;
  BOOLEAN                     Available;
  UINT8                       Reserved[3];
} MBP_OEM_KEY_REVOKE;
///
/// ME BIOS Payload structure containing insensitive data only
///
typedef struct {
  MBP_FW_VERSION_NAME        FwVersionName;
  MBP_FW_CAPS_SKU            FwCapsSku;
  MBP_FW_FEATURES_STATE      FwFeaturesState;
  MBP_PLAT_TYPE              FwPlatType;
  MBP_HWA_REQ                HwaRequest;
  MBP_ME_UNCONF_ON_RTC_STATE UnconfigOnRtcClearState;
  MBP_ARB_SVN_STATE          ArbSvnState;
  MBP_MPHY_DATA              MphyData;
  MBP_IFWI_DNX_REQUEST       IfwiDnxRequest;
  UINT32                     Reserved;
  MBP_MEASURED_BOOT_SUPPORT  MeasuredBootSupport;
  MBP_PERF_DATA_EX           PerfDataEx;
  MBP_OEM_PHY_DATA           OemPhyData;
  MBP_OEM_KEY_REVOKE         OemKeyRevoke;
} ME_BIOS_PAYLOAD;

///
/// All items which can't be available when 3rd part OPROMs/drivers are loaded
/// must be added to below structure only
///
typedef struct {
  MBP_PLATFORM_KEY           PlatformKey;
} ME_BIOS_PAYLOAD_SENSITIVE;

#pragma pack ()

#endif
