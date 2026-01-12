/** @file
  Header file for PtlSocInfoLib.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PTL_PCD_INFO_LIB_H_
#define _PTL_PCD_INFO_LIB_H_



typedef UINT8 PTL_SOC_SERIES;
typedef UINT8 PTL_SOC_STEPPING;


#define PTL_PCD_UNKNOWN_SERIES  0xFF

#define PTL_PCD_A0  0x0
#define PTL_PCD_UNKOWN_STEPPING  0xFF

#define PTL_PCD_UNKNOWN_SKU 0xFF

#define PTL_PCD_P  0x0
#define PTL_PCD_H  0x1
#define WCL_PCD    0x2
#define PTL_PCD_UNKNOWN_SERIES  0xFF

#define PTL_PCD_A0  0x0
#define PTL_SOC_UNKOWN_STEPPING  0xFF

#define PTL_PCD_INFO_STEPPING_STR_MIN_LENGTH  3

///
/// Enums for Soc-P North Stepping IDs
///
typedef enum {
  SocNorthA0 = 0x0,
  SocNorthA1 = 0x1,
  SocNorthA2 = 0x2,
  SocNorthB0 = 0x3,
  SocNorthC0 = 0x4
} SOC_M_NORTH_STEPPING;

///
/// Enums for Soc-H South Stepping IDs
///
typedef enum {
  SocSouthA0 = 0x00,
  SocSouthA1 = 0x01,
  SocSouthA2 = 0x02,
  SocSouthB0 = 0x10,
  SocSouthC0 = 0x20
} SOC_M_SOUTH_STEPPING;

#endif
