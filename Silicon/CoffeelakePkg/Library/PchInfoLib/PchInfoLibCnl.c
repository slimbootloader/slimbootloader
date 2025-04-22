/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/PchInfoLib.h>
#include <RegAccess.h>
#include "PchInfoLib.h"

/**
  Determine Pch Series based on Device Id

  @param[in] LpcDeviceId      Lpc Device Id

  @retval PCH_SERIES          Pch Series
**/
PCH_SERIES
PchSeriesFromLpcDid (
  IN UINT16 LpcDeviceId
  )
{
  switch (LpcDeviceId & B_LPC_CFG_DID) {

    case V_LPC_CFG_DID_CNL_H:
      return PCH_H;

    case V_LPC_CFG_DID_CNL_LP:
      return PCH_LP;

    default:
      return PCH_UNKNOWN_SERIES;
  }
}

/**
  Return Pch Generation

  @retval PCH_GENERATION            Pch Generation
**/
PCH_GENERATION
PchGeneration (
  VOID
  )
{
  return CNL_PCH;
}

/**
  Check if this is Server PCH

  @retval TRUE                It's a Server PCH
  @retval FALSE               It's not a Server PCH
**/
BOOLEAN
IsPchServer (
  VOID
  )
{
  return FALSE;
}

/**
  Get RST mode supported by the silicon

  @retval RST_MODE    RST mode supported by silicon
**/
RST_MODE
PchGetSupportedRstMode (
  VOID
  )
{
  switch (PchGetLpcDid ()) {

    case V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_4:
    case V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A303_SKU:
      return RstUnsupported;
      break;
    case V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A307_SKU:
    case V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A308_SKU:
      return RstOptane;
      break;

    default:
      return RstPremium;
      break;
  }
}

/**
  Check if this is Server SKU

  @retval TRUE                It's PCH Server SKU
  @retval FALSE               It's not PCH Server SKU
**/
BOOLEAN
IsPchServerSku (
  VOID
  )
{
  UINT16 LpcDid;

  LpcDid = PchGetLpcDid ();

  if (LpcDid == V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A309_SKU ||
      LpcDid == V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A310_SKU ||
      LpcDid == V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A311_SKU) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Get PCH series ASCII string.

  @retval PCH Series string
**/
CHAR8*
PchGetSeriesStr (
  VOID
  )
{
  switch (PchSeries ()) {

    case PCH_LP:
      return "CNL PCH-LP";

    case PCH_H:
      return "CNL PCH-H";

    default:
      return NULL;
  }
}

GLOBAL_REMOVE_IF_UNREFERENCED
struct PCH_SKU_STRING mSkuStrs[] = {
  //
  // CNL PCH LP Mobile LPC Device IDs
  //
  {V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_SUPER_SKU, "Super SKU"},
  {V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_0, "(U) Super SKU"},
  {V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_1, "Super SKU (locked)"},
  {V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_2, "(Y) Premium SKU"},
  {V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_3, "(U) Premium  SKU"},
  {V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_4, "(U) Base/Mainstream SKU"},
  {V_CNL_PCH_LP_LPC_CFG_DEVICE_ID_MB_5, "(Y) Super SKU"},
  //
  // CNL PCH H LPC Device IDs
  //
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A300_SKU, "CNL PCH-H SKU A300"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A301_SKU, "Client Super SKU (SSKU) unlocked"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A302_SKU, "Client Super SKU (SSKU) locked"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A303_SKU, "H310"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A304_SKU, "H370"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A305_SKU, "Z390"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A306_SKU, "Q370"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A307_SKU, "Q350"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A308_SKU, "B360"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A309_SKU, "C246"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A30A_SKU, "C242"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A30B_SKU, "X399"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A30C_SKU, "QM370"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A30D_SKU, "HM370"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A30E_SKU, "CM246"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A30F_SKU, "Internal Super SKU (SSKU)"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A310_SKU, "Server Super SKU (SSKU) Unlocked"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A311_SKU, "Server Super SKU (SSKU) Locked"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A312_SKU, "Small Form Factor (SFF) Super SKU (SSKU)"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A313_SKU, "Small Form Factor (SFF)"},
  {V_CNL_PCH_H_LPC_CFG_DEVICE_ID_A314_SKU, "Internal Production"},
  {0xFFFF, NULL}
};

/**
  Check whether integrated LAN controller is supported by PCH Series.

  @retval TRUE                    GbE is supported in current PCH
  @retval FALSE                   GbE is not supported on current PCH
**/
BOOLEAN
PchIsGbeSupported (
  VOID
  )
{
  return TRUE;
}

/**
  Get Pch Maximum Pcie Root Port Number

  @retval Pch Maximum Pcie Root Port Number
**/
UINT8
GetPchMaxPciePortNum (
  VOID
  )
{
  if (IsPchLp ()) {
    return 16;
  } else {
    return 24;
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
  if (IsPchLp ()) {
    return 10;
  } else {
    return 14;
  }
}

/**
  Get Pch Maximum Usb2 Port Number of XHCI Controller

  @retval Pch Maximum Usb2 Port Number of XHCI Controller
**/
UINT8
GetPchXhciMaxUsb2PortNum (
  VOID
  )
{
  if (IsPchLp ()) {
    return 12;
  } else {
    return 16;
  }
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
  if (IsPchLp ()) {
    return 6;
  } else {
    return 10;
  }
}

/**
  Check if given Audio Interface is supported

  @param[in] AudioLinkType  Link type support to be checked

  @retval    TRUE           Link supported
  @retval    FALSE          Link not supported
**/
BOOLEAN
IsAudioInterfaceSupported (
  IN PCH_HDAUDIO_LINK_TYPE AudioLinkType
  )
{
  //
  // Interfaces support per PCH Generation/Series:
  // 1. HDA Link (SDI0/SDI1) - CNP-LP/H
  // 2. Display Audio Link (SDI2) - CNP-LP/H
  // 3. SSP[0-2] - CNP-LP/H
  // 4. SNDW[1-4] - CNP-LP/H
  //
  switch (AudioLinkType) {
    case PchHdaLink:
    case PchHdaIDispLink:
      return TRUE;
    case PchHdaDmic0:
    case PchHdaDmic1:
      return TRUE;
    case PchHdaSsp0:
    case PchHdaSsp1:
    case PchHdaSsp2:
      return TRUE;
    case PchHdaSndw1:
    case PchHdaSndw2:
    case PchHdaSndw3:
    case PchHdaSndw4:
      return TRUE;
    default:
      return FALSE;
  }
}

/**
  Check if given Display Audio Link T-Mode is supported

  @param[in] Tmode          T-mode support to be checked

  @retval    TRUE           T-mode supported
  @retval    FALSE          T-mode not supported
**/
BOOLEAN
IsAudioIDispTmodeSupported (
  IN PCH_HDAUDIO_IDISP_TMODE Tmode
  )
{
  //
  // iDisplay Audio Link T-mode support per PCH Generation/Series:
  // 1. 1T  - CNP-LP
  // 2. 2T  - CNP-LP/H (default)
  //
  switch (Tmode) {
    case PchHdaIDispMode1T:
      return IsPchLp ();
    case PchHdaIDispMode2T:
      return TRUE;
    case PchHdaIDispMode4T:
    case PchHdaIDispMode8T:
    case PchHdaIDispMode16T:
    default:
      return FALSE;
  }
}

/**
  Gets the maximum number of UFS controller supported by this chipset.

  @return Number of supported UFS controllers
**/
UINT8
PchGetMaxUfsNum (
  VOID
  )
{
  if (IsPchLp ()) {
    return 1;
  } else {
    return 0;
  }
}

/**
  Check if this chipset supports eMMC controller

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
IsPchEmmcSupported (
  VOID
  )
{
  if (IsPchLp ()) {
    return TRUE;
  }

  return FALSE;
}

/**
  Check if this chipset supports SD controller

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
IsPchSdCardSupported (
  VOID
  )
{
  return TRUE;
}

/**
  Check if this chipset supports UFS controller

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
IsPchUfsSupported (
  VOID
  )
{
  if (IsPchLp ()) {
    return TRUE;
  }

  return FALSE;
}

/**
  Check if link between PCH and CPU is an P-DMI

  @retval    TRUE           P-DMI link
  @retval    FALSE          Not an P-DMI link
**/
BOOLEAN
IsPchWithPdmi (
  VOID
  )
{
  return IsPchH ();
}

/**
  Check if link between PCH and CPU is an OP-DMI

  @retval    TRUE           OP-DMI link
  @retval    FALSE          Not an OP-DMI link
**/
BOOLEAN
IsPchWithOpdmi (
  VOID
  )
{
  return !IsPchH ();
}

/**
  Check if link between PCH and CPU is an F-DMI

  @retval    TRUE           F-DMI link
  @retval    FALSE          Not an F-DMI link
**/
BOOLEAN
IsPchWithFdmi (
  VOID
  )
{
  return FALSE;
}
