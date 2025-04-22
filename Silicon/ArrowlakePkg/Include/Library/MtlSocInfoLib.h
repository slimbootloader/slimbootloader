/** @file
  Header file for MtlSocInfoLib.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

@par Specification Reference:
**/
#ifndef _MTL_SOC_INFO_LIB_H_
#define _MTL_SOC_INFO_LIB_H_


typedef UINT8 MTL_IOE_SERIES;
typedef UINT8 MTL_IOE_STEPPING;

typedef UINT8 MTL_SOC_SERIES;
typedef UINT8 MTL_SOC_STEPPING;

#define MTL_SOC_M  0x0
#define MTL_SOC_S  0x1
#define ARL_SOC_M  0x2
#define MTL_SOC_UNKNOWN_SERIES  0xFF

#define MTL_SOC_A0  0x0

#define MTL_IOE_M  0x0
#define MTL_IOE_P  0x1
#define MTL_IOE_UNKNOWN_SERIES  0xFF

#define MTL_IOE_A0 0x0

#define MTL_SOC_INFO_STEPPING_STR_MIN_LENGTH  3

///
/// Enums for Soc-M North Stepping IDs
///
typedef enum {
  SocNorthA0 = 0x0,
  SocNorthA1 = 0x1,
  SocNorthA2 = 0x2,
  SocNorthB0 = 0x3,
  SocNorthC0 = 0x4
} SOC_M_NORTH_STEPPING;

///
/// Enums for Soc-M South Stepping IDs
///
typedef enum {
  SocSouthA0 = 0x00,
  SocSouthA1 = 0x01,
  SocSouthA2 = 0x02,
  SocSouthB0 = 0x10,
  SocSouthC0 = 0x20
} SOC_M_SOUTH_STEPPING;

/**
  Get Usb Maximum Physical Port Number

  @retval Usb Maximum Physical Port Number
**/
UINT8
MtlSocGetUsb2MaxPhysicalPortNum (
  VOID
  );

/**
  Get Maximum Usb3 Port Number of XHCI Controller

  @retval Maximum Usb3 Port Number of XHCI Controller
**/
UINT8
MtlSocGetXhciMaxUsb3PortNum (
  VOID
  );
#endif
