/** @file
  Pch information library for PTL.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <IndustryStandard/Pci30.h>
#include <Library/PrintLib.h>
#include <Library/PchInfoLib.h>
#include <PchAccess.h>
#include <Register/PtlPcdRegsLpc.h>
#include <Library/PchPciBdfLib.h>
#include <Library/PciSegmentLib.h>

typedef struct {
  UINT32  Id;
  CHAR8   *Str;
} PTL_INFO_STR_TABLE;

GLOBAL_REMOVE_IF_UNREFERENCED PTL_INFO_STR_TABLE  mPtlPcdSeries[] = {
  {PTL_PCD_P, "PTL PCD P"},
  {PTL_PCD_H, "PTL PCD H"},
  {WCL_PCD,   "WCL PCD"},
  {PTL_PCD_UNKNOWN_SERIES, "Unknown"}
};

//
// PTL PCD P and H Mobile LPC Device IDs
//
GLOBAL_REMOVE_IF_UNREFERENCED PTL_INFO_STR_TABLE mPtlPcdSkuStrs[] = {
  {V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_0,   "PTL P Unfused"},
  {V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_1,   "P Super SKU (SSKU)"},
  {V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_2,   "P Premium"},
  {V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_3,   "P Base/Mainstream"},
  {V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_0,   "PTL H Unfused"},
  {V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_1,   "H Super SKU (SSKU)"},
  {V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_2,   "H Premium"},
  {V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_3,   "H Base/Mainstream"},
  {V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_0,     "WCL PCD:Unfused"},
  {V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_1,     "WCL PCD:A0"},
  {V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_2,     "WCL PCD:A1"},
  {V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_3,     "WCL PCD:B0"},
  {PTL_PCD_UNKNOWN_SKU,                  "Unknown"}
};

/**
  Get Maximum Usb3 Port Number of XHCI Controller

  @retval Maximum Usb3 Port Number of XHCI Controller
**/
UINT8
PtlPcdGetXhciMaxUsb3PortNum (
  VOID
  )
{
  return 2;
}

/**
  Get Usb Maximum Physical Port Number

  @retval Usb Maximum Physical Port Number
**/
UINT8
PtlPcdGetUsb2MaxPhysicalPortNum (
  VOID
  )
{
  switch (PtlPcdGetSeries ()) {
    case PTL_PCD_P:
    case PTL_PCD_H:
      return 8;
    default:
      return 0;
  }
}

/**
  Return eSPI Device Id.

  @retval PTL PCD eSPI Device ID
**/
UINT16
PtlPcdGetEspiDid (
  VOID
  )
{
  UINT64  LpcBaseAddress;

  LpcBaseAddress = LpcPciCfgBase ();

  return PciSegmentRead16 (LpcBaseAddress + PCI_DEVICE_ID_OFFSET);
}

/**
  Return SoC Series

  @retval PTL_PCD_SERIES     SoC Series
**/
PTL_PCD_SERIES
PtlPcdGetSeries (
  VOID
  )
{
  switch (PtlPcdGetEspiDid () & B_ESPI_CFG_DID) {
    case V_ESPI_CFG_DID_PTL_PCD_P:
      return PTL_PCD_P;
    case V_ESPI_CFG_DID_PTL_PCD_H:
      return PTL_PCD_H;
    case V_ESPI_CFG_DID_WCL_PCD:
      return WCL_PCD;
    default:
      return PTL_PCD_UNKNOWN_SERIES;
  }
}

/**
  Return Pch Series

  @retval PCH_SERIES            Pch Series
**/
PCH_SERIES
PchSeries (
  VOID
  )
{
  PTL_SOC_SERIES  SocSeries;

  SocSeries = PtlPcdGetSeries ();
  DEBUG((DEBUG_INFO," SoC Series = 0x%x\n", SocSeries));
  if (SocSeries == PTL_PCD_P) {
    return PCH_P;
  } else if (SocSeries == WCL_PCD) {
    return PCH_WCL;
  } else if (SocSeries == PTL_PCD_H) {
    return PCH_H;
  }

  return PCH_UNKNOWN_SERIES;
}

/**
  Return Pch stepping type

  @retval PCH_STEPPING            Pch stepping type
**/
PCH_STEPPING
PchStepping (
  VOID
  )
{
  return (PCH_STEPPING) PtlPcdGetStepping ();
}

/**
  Checks if PCD is of WCL PCD series.

  @return TRUE if SoC is of WCL PCD series.
**/
BOOLEAN
IsWclPcd (
  VOID
  )
{
  return (PtlPcdGetSeries () == WCL_PCD);
}

/**
  Return Pch Generation

  @retval PCH_GENERATION      Pch Generation
**/
PCH_GENERATION
PchGeneration (
  VOID
  )
{
  return PTL_SOC;
}

/**
  Check if this is PCH H series

  @retval TRUE                It's PCH H series
  @retval FALSE               It's not PCH H series
**/
BOOLEAN
IsPchH (
  VOID
  )
{
  return (PchSeries () == PCH_H);
}

/**
  Get Pch Maximum Pcie Clock Number

  @retval Pch Maximum Pcie Clock Number
**/
UINT8
GetPchMaxPcieClockNum (
  VOID
  )
{
  if (PtlIsPcdP()) {
    return 7;
  } else if (IsWclPcd ()) {
    return 6;
  } else {
    return 9;
  }
}

/**
  Get Pch Usb2 Maximum Physical Port Number

  @retval Pch Usb2 Maximum Physical Port Number
**/
UINT8
GetPchUsb2MaxPhysicalPortNum (
  VOID
  )
{
  return PtlPcdGetUsb2MaxPhysicalPortNum ();
}

/**
  Get Pch Maximum Usb3 Port Number of XHCI Controller

  @retval Pch Maximum Usb3 Port Number of XHCI Controller
**/
UINT8
GetPchXhciMaxUsb3PortNum (
  VOID
  )
{
  return PtlPcdGetXhciMaxUsb3PortNum ();
}

/**
  Get Maximum LPSS I3C interfaces number

  @retval Pch Maximum LPSS I3C interfaces number
**/
UINT8
GetMaxI3cInterfacesNum (
  VOID
  )
{
  return PCH_MAX_SERIALIO_I3C_CONTROLLERS;
}

/**
  Get Pch Maximum LPSS UART controllers number

  @retval Pch Maximum LPSS UART controllers number
**/
UINT8
GetPchMaxSerialIoUartControllersNum (
  VOID
  )
{
  return 3;
}

/**
  Checks if PTL PCD is of P series.

  @return TRUE if PTL SoC is of P series.
**/
BOOLEAN
PtlIsPcdP (
  VOID
  )
{
  return (PtlPcdGetSeries () == PTL_PCD_P);
}

/**
  Checks if PTL PCD is of H series.

  @return TRUE if PTL SoC is of H series.
**/
BOOLEAN
PtlIsPcdH (
  VOID
  )
{
  return (PtlPcdGetSeries () == PTL_PCD_H);
}

/**
  Return SoC stepping type

  @retval PCH_STEPPING   SoC stepping type
**/
PTL_SOC_STEPPING
PtlPcdGetStepping (
  VOID
  )
{
  return PciSegmentRead8 (LpcPciCfgBase () + PCI_REVISION_ID_OFFSET);
}

/**
  Get number of PCIe controller on PTL SOC.

  @return Number of controllers on the SoC.
**/
UINT8
PtlPcdGetMaxPcieControllerNum (
  VOID
  )
{
  UINT8  NumOfControllers;

  if (PtlIsPcdP ()) {
    NumOfControllers = 10;
  } else if (IsWclPcd ()) {
    NumOfControllers = 6;
  } else {
    NumOfControllers = 12;
  }

  return NumOfControllers;
}

/**
  Get Pch Maximum Pcie ClockReq Number

  @retval Pch Maximum Pcie ClockReq Number
**/
UINT8
GetPchMaxPcieClockReqNum (
  VOID
  )
{
  return GetPchMaxPcieClockNum ();
}

/**
  Get Pch Maximum Serial IO I2C controllers number

  @retval Pch Maximum Serial IO I2C controllers number
**/
UINT8
GetPchMaxSerialIoI2cControllersNum (
  VOID
  )
{
  return 6;
}

/**
  Get Pch Maximum Serial IO SPI controllers number

  @retval Pch Maximum Serial IO SPI controllers number
**/
UINT8
GetPchMaxSerialIoSpiControllersNum (
  VOID
  )
{
  return 3;
}
