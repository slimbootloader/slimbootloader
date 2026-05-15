/** @file
  Header file for PchInfoLib.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_INFO_LIB_H_
#define _PCH_INFO_LIB_H_

#include <Uefi/UefiBaseType.h>

typedef UINT8 PTL_PCD_SERIES;

typedef UINT8 PTL_SOC_SERIES;
typedef UINT8 PTL_SOC_STEPPING;

typedef UINT8 PCH_STEPPING;
typedef UINT8 PCH_SERIES;

typedef UINT8 PCH_GENERATION;
#define PCH_H                   1
#define PCH_LP                  2
#define PCH_N                   3
#define PCH_S                   4
#define PCH_P                   5
#define PCH_M                   6
#define PCH_WCL                 7
#define PCH_UNKNOWN_SERIES      0xFF

#define PTL_PCD_UNKNOWN_SKU 0xFF

#define PTL_PCD_P  0x0
#define PTL_PCD_H  0x1
#define WCL_PCD    0x2
#define PTL_PCD_UNKNOWN_SERIES  0xFF

#define PTL_SOC                 0xB

/**
  Return Pch Series

  @retval PCH_SERIES            Pch Series
**/
PCH_SERIES
PchSeries (
  VOID
  );

/**
  Return Pch Generation

  @retval PCH_GENERATION            Pch Generation
**/
PCH_GENERATION
PchGeneration (
  VOID
  );

/**
  Return eSPI Device Id.

  @retval  PTL SoC eSPI Device ID
**/
UINT16
PtlPcdGetEspiDid (
  VOID
  );

/**
  Return SoC Series

  @retval PCH_SERIES     SoC Series
**/
PTL_SOC_SERIES
PtlPcdGetSeries (
  VOID
  );

/**
  Check if this is PCH H series

  @retval TRUE                It's PCH H series
  @retval FALSE               It's not PCH H series
**/

BOOLEAN
IsPchH (
  VOID
  );

/**
  Return SoC stepping type

  @retval PCH_STEPPING   SoC stepping type
**/
PTL_SOC_STEPPING
PtlPcdGetStepping (
  VOID
  );

/**
  Return Pch stepping type

  @retval PCH_STEPPING            Pch stepping type
**/
PCH_STEPPING
PchStepping (
  VOID
  );

/**
  Checks if PTL PCD is of H series.

  @return TRUE if PTL SoC is of H series.
**/
BOOLEAN
PtlIsPcdH (
  VOID
  );

/**
  Checks if PTL PCD is of P series.

  @return TRUE if PTL SoC is of P series.
**/
BOOLEAN
PtlIsPcdP (
  VOID
  );

/**
  Checks if PCD is of WCL PCD series.

  @return TRUE if SoC is of WCL PCD series.
**/
BOOLEAN
IsWclPcd (
  VOID
  );

/**
  Get number of PCIe controller on PTL SOC.

  @return Number of controllers on the SoC.
**/
UINT8
PtlPcdGetMaxPcieControllerNum (
  VOID
  );

/**
  Get Pch Maximum Pcie Clock Number

  @retval Pch Maximum Pcie Clock Number
**/
UINT8
GetPchMaxPcieClockNum (
  VOID
  );

/**
  Get Pch Maximum Pcie ClockReq Number

  @retval Pch Maximum Pcie ClockReq Number
**/
UINT8
GetPchMaxPcieClockReqNum (
  VOID
  );

/**
  Get Pch Usb2 Maximum Physical Port Number

  @retval Pch Usb2 Maximum Physical Port Number
**/
UINT8
GetPchUsb2MaxPhysicalPortNum (
  VOID
  );

/**
  Get Pch Maximum Usb3 Port Number of XHCI Controller

  @retval Pch Maximum Usb3 Port Number of XHCI Controller
**/
UINT8
GetPchXhciMaxUsb3PortNum (
  VOID
  );

/**
  Get Pch Maximum Serial IO I2C controllers number

  @retval Pch Maximum Serial IO I2C controllers number
**/
UINT8
GetPchMaxSerialIoI2cControllersNum (
  VOID
  );

/**
  Get Pch Maximum Serial IO SPI controllers number

  @retval Pch Maximum Serial IO SPI controllers number
**/
UINT8
GetPchMaxSerialIoSpiControllersNum (
  VOID
  );

/**
  Get Pch Maximum Serial IO UART controllers number

  @retval Pch Maximum Serial IO UART controllers number
**/

UINT8
GetPchMaxSerialIoUartControllersNum (
  VOID
  );

#endif // _PCH_INFO_LIB_H_
