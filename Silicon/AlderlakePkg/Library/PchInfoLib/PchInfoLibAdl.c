/** @file
  Pch information library for ADL.

  Copyright (c) 2020 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <IndustryStandard/Pci30.h>
#include <Register/PchRegsLpcAdl.h>
#include <Library/PrintLib.h>
#include <Library/PchInfoLib.h>
#include <PchAccess.h>
#include "PchInfoLibPrivate.h"
#include <PchPcieRpInfo.h>
#include <PchHda.h>
#include <Register/PchPcieRpRegs.h>
#include <Library/PchPcrLib.h>

CONST PCH_PCIE_CONTROLLER_INFO mPchPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_1, PID_SPA,  0 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_5, PID_SPB,  4 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_9, PID_SPC,  8 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_13, PID_SPD, 12 }, // PCH-S only
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_17, PID_SPE, 16 }, // PCH-S only
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_21, PID_SPF, 20 }, // PCH-S only
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_25, PID_SPG, 24 }  // PCH-S only, Not in PCH EDS
};

PCH_SERIES
PchSeriesFromLpcDid (
  IN UINT16 LpcDeviceId
  );

PCH_STEPPING
PchSteppingFromRevId (
  IN UINT8 RevId
  );

VOID
PchPrintSteppingStr (
  OUT CHAR8        *Buffer,
  IN  UINT32       BufferSize,
  IN  PCH_STEPPING PchStep
  );


GLOBAL_REMOVE_IF_UNREFERENCED
struct PCH_SKU_STRING mSkuStrs[] = {
  //
  // ADL PCH P M and S Mobile LPC Device IDs
  //
  {V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_0,   "ADL SKU 0"},
  {V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_1,   "P Super SKU (SSKU)"},
  {V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_2,   "P Premium"},
  {V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_3,   "TBD"},
  {V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_4,   "ADL SKU 4"},
  {V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_5,   "ADL SKU 5"},
  {V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_6,   "M Super SKU (SSKU)"},
  {V_ADL_PCH_P_LPC_CFG_DEVICE_ID_MB_7,   "M Premium"},
  {V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_0,   "ADL SKU 0"},
  {V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_1,   "U Super SKU (SSKU)"},
  {V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_2,   "U Premium"},
  {V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_3,   "U Base/Mainstream"},
  {V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_4,   "ADL SKU 4"},
  {V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_5,   "ADL SKU 5"},
  {V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_6,   "Y Super SKU (SSKU)"},
  {V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_7,   "Y Premium"},
  {V_ADL_PCH_M_LPC_CFG_DEVICE_ID_MB_8,   "IOT SKU"},
  {V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_0,   "ADL SKU 0"},
  {V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_1,   "Client SuperSKU (SSKU)"},
  {V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_2,   "Server SuperSKU (SSKU)"},
  {V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_3,   "Q670"},
  {V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_4,   "Z690"},
  {V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_5,   "H670"},
  {V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_6,   "B660"},
  {V_ADL_PCH_S_LPC_CFG_DEVICE_ID_MB_7,   "H610"},
  {0xFFFF, NULL}
};


/**
  Return LPC Device Id

  @retval PCH_LPC_DEVICE_ID         PCH Lpc Device ID
**/
UINT16
PchGetLpcDid (
  VOID
  )
{
  UINT16  LpcDeviceId;
  UINT64  EspiBaseAddress;

  EspiBaseAddress = PCI_LIB_ADDRESS (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_ESPI,
                     PCI_FUNCTION_NUMBER_PCH_ESPI,
                     0
                     );
  LpcDeviceId = PciRead16 ((UINTN)(EspiBaseAddress + PCI_DEVICE_ID_OFFSET));

  return LpcDeviceId;
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
  PCH_SERIES        PchSer;
  static PCH_SERIES PchSeries = PCH_UNKNOWN_SERIES;

  if (PchSeries != PCH_UNKNOWN_SERIES) {
    return PchSeries;
  }

  PchSer = PchSeriesFromLpcDid (PchGetLpcDid ());

  PchSeries = PchSer;

  return PchSer;
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
  UINT8                RevId;
  UINT64               EspiBaseAddress;

  EspiBaseAddress = PCI_LIB_ADDRESS (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_ESPI,
                     PCI_FUNCTION_NUMBER_PCH_ESPI,
                     0
                     );
  RevId = PciRead8 ((UINTN)(EspiBaseAddress + PCI_REVISION_ID_OFFSET));

  return PchSteppingFromRevId (RevId);

}


/**
  Determine if PCH is supported

  @retval TRUE                    PCH is supported
  @retval FALSE                   PCH is not supported
**/
BOOLEAN
IsPchSupported (
  VOID
  )
{
  UINT16         LpcDeviceId;
  UINT16         LpcVendorId;
  UINT64         EspiBaseAddress;


  EspiBaseAddress = PCI_LIB_ADDRESS (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_ESPI,
                     PCI_FUNCTION_NUMBER_PCH_ESPI,
                     0
                     );

  LpcDeviceId = PciRead16 ((UINTN)(EspiBaseAddress + PCI_DEVICE_ID_OFFSET));
  LpcVendorId = PciRead16 ((UINTN)(EspiBaseAddress + PCI_VENDOR_ID_OFFSET));

  ///
  /// Verify that this is a supported chipset
  ///
  if ((LpcVendorId == V_PCH_INTEL_VENDOR_ID) && (PchSeries () != PCH_UNKNOWN_SERIES)) {
    return TRUE;
  } else {
    DEBUG ((DEBUG_ERROR, "PCH code doesn't support the LpcDeviceId: 0x%04x!\n", LpcDeviceId));
    return FALSE;
  }
}

/**
  Check if this is PCH P series

  @retval TRUE                It's PCH P series
  @retval FALSE               It's not PCH P series
**/
BOOLEAN
IsPchP (
  VOID
  )
{
  return (PchSeries () == PCH_P);
}

/**
  Check if this is PCH M series

  @retval TRUE                It's PCH M series
  @retval FALSE               It's not PCH M series
**/
BOOLEAN
IsPchM (
  VOID
  )
{
  return (PchSeries () == PCH_M);
}

/**
  Check if this is PCH S series

  @retval TRUE                It's PCH S series
  @retval FALSE               It's not PCH S series
**/
BOOLEAN
IsPchS (
  VOID
  )
{
  return (PchSeries () == PCH_S);
}

/**
  Check if this is CDF PCH generation

  @retval TRUE                It's CDF PCH
  @retval FALSE               It's not CDF PCH
**/
BOOLEAN
IsCdfPch (
  VOID
  )
{
  return (PchGeneration () == CDF_PCH);
}


/**
  Check if this is EBG PCH generation

  @retval TRUE                It's EBG PCH
  @retval FALSE               It's not EBG PCH
**/
BOOLEAN
IsEbgPch (
  VOID
  )
{
  return (PchGeneration () == EBG_PCH);
}

/**
  Check if this is CNL PCH generation

  @retval TRUE                It's CNL PCH
  @retval FALSE               It's not CNL PCH
**/
BOOLEAN
IsCnlPch (
  VOID
  )
{
  return (PchGeneration () == CNL_PCH);
}

/**
  Check if this is ICL PCH generation

  @retval TRUE                It's ICL PCH
  @retval FALSE               It's not ICL PCH
**/
BOOLEAN
IsIclPch (
  VOID
  )
{
  return (PchGeneration () == ICL_PCH);
}

/**
  Check if this is TGL PCH generation

  @retval TRUE                It's TGL PCH
  @retval FALSE               It's not TGL PCH
**/
BOOLEAN
IsTglPch (
  VOID
  )
{
  return (PchGeneration () == TGL_PCH);
}

/**
  Check if this is JSL PCH generation

  @retval TRUE                It's JSL PCH
  @retval FALSE               It's not JSL PCH
**/
BOOLEAN
IsJslPch (
  VOID
  )
{
  return (PchGeneration () == JSL_PCH);
}

/**
  Check if this is ADL PCH generation

  @retval TRUE                It's ADL PCH
  @retval FALSE               It's not ADL PCH
**/
BOOLEAN
IsAdlPch (
  VOID
  )
{
  return (PchGeneration () == ADL_PCH);
}

/**
  Get PCH stepping ASCII string.
  Function determines major and minor stepping versions and writes them into a buffer.
  The return string is zero terminated

  @param [out]     Buffer               Output buffer of string
  @param [in]      BufferSize           Buffer size.
                                        Must not be less then PCH_STEPPING_STR_LENGTH_MAX

  @retval EFI_SUCCESS                   String copied successfully
  @retval EFI_INVALID_PARAMETER         The stepping is not supported, or parameters are NULL
  @retval EFI_BUFFER_TOO_SMALL          Input buffer size is too small
**/
EFI_STATUS
PchGetSteppingStr (
  OUT    CHAR8                          *Buffer,
  IN     UINT32                         BufferSize
  )
{
  PCH_STEPPING PchStep;

  PchStep = PchStepping ();

  if ((Buffer == NULL) || (BufferSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  if (BufferSize < PCH_STEPPING_STR_LENGTH_MAX) {
    return EFI_BUFFER_TOO_SMALL;
  }

  PchPrintSteppingStr (Buffer, BufferSize, PchStep);

  return EFI_SUCCESS;
}

/**
  Get PCH Sku ASCII string
  The return string is zero terminated.

  @retval Static ASCII string of PCH Sku
**/
CHAR8*
PchGetSkuStr (
  VOID
  )
{
  UINTN          Index;
  UINT16         LpcDid;

  LpcDid = PchGetLpcDid ();

  for (Index = 0; mSkuStrs[Index].Id != 0xFFFF; Index++) {
    if (LpcDid == mSkuStrs[Index].Id) {
      return mSkuStrs[Index].String;
    }
  }

  return "Undefined SKU";
}

/**
  Get Pch Maximum Pcie Controller Number

  @retval Pch Maximum Pcie Root Port Number
**/
UINT8
GetPchMaxPcieControllerNum (
  VOID
  )
{
  return GetPchMaxPciePortNum () / PCH_PCIE_CONTROLLER_PORTS;
}

/**
  return support status for P2SB PCR 20-bit addressing

  @retval    TRUE
  @retval    FALSE
**/
BOOLEAN
IsP2sb20bPcrSupported (
  VOID
  )
{
  if (IsPchS ()) {
    return TRUE;
  } else {
    return FALSE;
  }
}

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

    case V_LPC_CFG_DID_ADL_S:
      return PCH_S;

    case V_LPC_CFG_DID_ADL_P:
    case V_LPC_CFG_DID_ADL_PS:
      return PCH_P;

    case V_LPC_CFG_DID_ADL_N:
      return PCH_N;

    default:
      return PCH_UNKNOWN_SERIES;
  }
}

/**
  Determine Pch Stepping based on Revision ID

  @param[in] RevId            Pch Revision Id

  @retval PCH_STEPPING        Pch Stepping
**/
PCH_STEPPING
PchSteppingFromRevId (
  IN UINT8 RevId
  )
{
  return RevId;
}

/**
  Print Pch Stepping String

  @param[out] Buffer         Output buffer of string
  @param[in]  BufferSize     Buffer Size
  @param[in]  PchStep        Pch Stepping Type

  @retval VOID
**/
VOID
PchPrintSteppingStr (
  OUT CHAR8        *Buffer,
  IN  UINT32       BufferSize,
  IN  PCH_STEPPING PchStep
  )
{
  AsciiSPrint (Buffer, BufferSize, "%c%c", 'A' + (PchStep >> 4), '0' + (PchStep & 0xF));
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
  return ADL_PCH;
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
  Check if this is PCH H series

  @retval TRUE                It's PCH H series
  @retval FALSE               It's not PCH H series
**/
BOOLEAN
IsPchH (
  VOID
  )
{
  return (PchSeries () == PCH_H || PchSeries () == PCH_S);
}

/**
  Check if this is PCH N series

  @retval TRUE                It's PCH N series
  @retval FALSE               It's not PCH N series
**/
BOOLEAN
IsPchN (
  VOID
  )
{
  return (PchSeries () == PCH_N);
}

/**
  Check if this is PCH LP series

  @retval TRUE     It's PCH LP series
  @retval FALSE    It's not PCH LP series
**/
BOOLEAN
IsPchLp (
  VOID
  )
{
  return (PchSeries () == PCH_LP || PchSeries () == PCH_P || PchSeries () == PCH_M || PchSeries () == PCH_N);
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
  return RstUnsupported;
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
  return FALSE;
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

    case PCH_P:
      return "ADL PCH-P";

    case PCH_M:
      return "ADL PCH-M";

    case PCH_S:
      return "ADL PCH-S";

    case PCH_N:
      return "ADL PCH-N";

    default:
      return NULL;
  }
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
  //
  // ADL P has 7 Reference clock and clock reqs +(ADP-P) 3 Output Enables + 4 additional Clock reqs.
  //
  if (IsPchS ()) {
    return 18;
  } else {
    return 10;
  }
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
  Get Pch Maximum Type C Port Number

  @retval Pch Maximum Type C Port Number
**/
UINT8
GetPchMaxTypeCPortNum (
  VOID
  )
{
  switch (PchSeries ()) {
    case PCH_P:
    case PCH_M:
    case PCH_N:
      return 4;
    default:
      return 0;
  }
}

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
  if (PchSeries () == PCH_M) {
    return FALSE;
  }

  return TRUE;
}

/**
  Check whether integrated TSN is supported by PCH Series.

  @retval TRUE                    TSN is supported in current PCH
  @retval FALSE                   TSN is not supported on current PCH
**/
BOOLEAN
PchIsTsnSupported (
  VOID
  )
{
#ifdef TSN_ENABLED
  if (PchSeries () == PCH_M) {
    return FALSE;
  }
  return TRUE;
#else
  return FALSE;
#endif
}

/**
  Check whether ISH is supported by PCH Series.

  @retval TRUE                    ISH is supported in current PCH
  @retval FALSE                   ISH is not supported on current PCH
**/
BOOLEAN
PchIsIshSupported (
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
  switch (PchSeries ()) {
    case PCH_P:
    case PCH_N:
      return 12;
    case PCH_S:
      return 28;
    default:
      return 0;
  }
}

/**
  Get Pch Maximum Hda Dmic Link

  @retval Pch Maximum Hda Dmic Link
**/
UINT8
GetPchHdaMaxDmicLinkNum (
  VOID
  )
{
  return 2;
}

/**
  Get Pch Maximum Hda Sndw Link

  @retval Pch Maximum Hda Sndw Link
**/
UINT8
GetPchHdaMaxSndwLinkNum (
  VOID
  )
{
  return 4;
}

/**
  Get Pch Maximum Hda Ssp Link

  @retval Pch Maximum Hda Ssp Link
**/
UINT8
GetPchHdaMaxSspLinkNum (
  VOID
  )
{
  if (IsPchLp ()) {
    return 6;
  } else {
    return 3;
  }
}

/**
  Check if given Audio Interface is supported

  @param[in] AudioLinkType   Link type support to be checked
  @param[in] AudioLinkIndex  Link number

  @retval    TRUE           Link supported
  @retval    FALSE          Link not supported
**/
BOOLEAN
IsAudioInterfaceSupported (
  IN HDAUDIO_LINK_TYPE     AudioLinkType,
  IN UINT32                AudioLinkIndex
  )
{
  //
  // Interfaces supported:
  // 1. HDA Link (SDI0/SDI1)
  // 2. Display Audio Link (SDI2)
  // 3. SSP[0-5]
  // 4. SNDW[1-4]
  //
  switch (AudioLinkType) {
    case HdaLink:
    case HdaIDispLink:
      return TRUE;
    case HdaDmic:
      if (AudioLinkIndex < 2) {
        return TRUE;
      } else {
        return FALSE;
      }
    case HdaSsp:
      if (AudioLinkIndex < 3) {
        return TRUE;
      } else if (AudioLinkIndex < 6) {
        return IsPchLp ();
      } else {
        return FALSE;
      }
    case HdaSndw:
      if (AudioLinkIndex < 4) {
        return TRUE;
      } else {
        return FALSE;
      }
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
  IN HDAUDIO_IDISP_TMODE Tmode
  )
{
  switch (Tmode) {
    case HdaIDispMode1T:
      return IsPchS ();
    case HdaIDispMode2T:
    case HdaIDispMode4T:
    case HdaIDispMode8T:
      return TRUE;
    case HdaIDispMode16T:
      return (IsPchS ());
    default:
      return FALSE;
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
  switch (PchSeries ()) {
    case PCH_P:
    case PCH_N:
      return 10;
    case PCH_S:
      return 14;
    default:
      return 0;
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
  switch (PchSeries ()) {
    case PCH_P:
    case PCH_N:
      return 12;
    case PCH_S:
      return 16;
    default:
      return 0;
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
  switch (PchSeries ()) {
    case PCH_P:
    case PCH_N:
      return 4;
    case PCH_S:
      return 10;
    default:
      return 0;
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
  if (IsPchS () || IsPchM ()) {
    return 0;
  } else {
    return 1;
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
  switch (PchSeries ()) {
    case PCH_N:
      return TRUE;
    default:
      return FALSE;
  }
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
  return FALSE;
}

/**
  Check if this chipset supports THC controller

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
IsPchThcSupported (
  VOID
  )
{
  return TRUE;
}

/**
  Check if this chipset supports HSIO BIOS Sync

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
IsPchChipsetInitSyncSupported (
  VOID
  )
{
  if (IsPchS ()) {
    return FALSE;
  } else {
    return TRUE;
  }
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
  return IsPchS ();
}

/**
  Check whether ATX Shutdown (PS_ON) is supported.

  @retval    TRUE           ATX Shutdown (PS_ON) is supported in PCH
  @retval    FALSE          ATX Shutdown (PS_ON) is not supported by PCH
**/
BOOLEAN
IsPchPSOnSupported (
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
  return !IsPchS ();
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
/**
  Get Pch Maximum ISH UART Controller number

  @retval Pch Maximum ISH UART controllers number
**/
UINT8
GetPchMaxIshUartControllersNum (
  VOID
  )
{
  return 2;
}

/**
  Get Pch Maximum ISH I2C Controller number

  @retval Pch Maximum ISH I2C controllers number
**/
UINT8
GetPchMaxIshI2cControllersNum (
  VOID
  )
{
  return 3;
}

/**
  Get Pch Maximum ISH I3C Controller number

  @retval Pch Maximum ISH I3C controllers number
**/
UINT8
GetPchMaxIshI3cControllersNum (
  VOID
  )
{
  return 0;
}

/**
  Get Pch Maximum ISH SPI Controller number

  @retval Pch Maximum ISH SPI controllers number
**/
UINT8
GetPchMaxIshSpiControllersNum (
  VOID
  )
{
  return 1;
}

/**
  Get Pch Maximum ISH SPI Controller Cs pins number

  @retval Pch Maximum ISH SPI controller Cs pins number
**/
UINT8
GetPchMaxIshSpiControllerCsPinsNum (
  VOID
  )
{
  return 1;
}

/**
  Get Pch Maximum ISH GP number

  @retval Pch Maximum ISH GP number
**/
UINT8
GetPchMaxIshGpNum (
  VOID
  )
{
  return 8;
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
  return PCH_MAX_SERIALIO_I2C_CONTROLLERS;
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
  return PCH_MAX_SERIALIO_SPI_CONTROLLERS;
}

/**
  Get Pch Maximum Serial IO UART controllers number

  @retval Pch Maximum Serial IO UART controllers number
**/
UINT8
GetPchMaxSerialIoUartControllersNum (
  VOID
  )
{
  return PCH_MAX_SERIALIO_UART_CONTROLLERS;
}

/**
  Get Pch Maximum Serial IO SPI Chip Selects count

  @retval Pch Maximum Serial IO SPI Chip Selects number
**/
UINT8
GetPchMaxSerialIoSpiChipSelectsNum (
  VOID
  )
{
  return 2;
}

/**
  Get Pch Maximum ME Applet count

  @retval Pch Maximum ME Applet number
**/
UINT8
GetPchMaxMeAppletCount (
  VOID
  )
{
  return 31;
}

/**
  Get Pch Maximum ME Session count

  @retval Pch Maximum ME Sesion number
**/
UINT8
GetPchMaxMeSessionCount (
  VOID
  )
{
  return 16;
}

/**
  Get Pch Maximum THC count

  @retval Pch Maximum THC count number
**/
UINT8
GetPchMaxThcCount (
  VOID
  )
{
  return 2;
}

/**
  Returns a frequency of the sosc_clk signal.
  All SATA controllers on the system are assumed to
  work on the same sosc_clk frequency.

  @retval Frequency of the sosc_clk signal.
**/
SATA_SOSC_CLK_FREQ
GetSataSoscClkFreq (
  VOID
  )
{
  return SataSosc100Mhz;
}

/**
  Check if SATA support should be awake after function disable

  @retval    TRUE
  @retval    FALSE
**/
BOOLEAN
IsSataSupportWakeAfterFunctionDisable (
  VOID
  )
{
  return TRUE;
}

/**
  Returns USB2 PHY Reference Clock frequency value used by PCH
  This defines what electrical tuning parameters shall be used
  during USB2 PHY initialization programming

  @retval Frequency reference clock for USB2 PHY
**/
USB2_PHY_REF_FREQ
GetUsb2PhyRefFreq (
  VOID
  )
{
  return FREQ_19_2;
}

/**
  Check if SPI in a given PCH generation supports an Extended BIOS Range Decode

  @retval TRUE or FALSE if PCH supports Extended BIOS Range Decode
**/
BOOLEAN
IsExtendedBiosRangeDecodeSupported (
  VOID
  )
{
  return TRUE;
}

#define SPI_PCH_LP_DMI_TARGET   0x23A8
#define SPI_PCH_S_DMI_TARGET    0x23B8

/**
  Returns DMI target for current PCH SPI

  @retval PCH SPI DMI target value
**/
UINT16
GetPchSpiDmiTarget (
  VOID
  )
{
  if (IsPchLp()) {
    return SPI_PCH_LP_DMI_TARGET;
  } else {
    return SPI_PCH_S_DMI_TARGET;
  }
}

/**
  Get Maximum Sata Port Number

  @param[in]  SataCtrlIndex       SATA controller index

  @retval     Maximum Sata Port Number
**/
UINT8
MaxSataPortNum (
  IN UINT32      SataCtrlIndex
  )
{
  ASSERT (SataCtrlIndex < MAX_SATA_CONTROLLER);

  //
  // Number of ports defined in SoC integration header
  //
  if (IsPchLp ()) {
    return MAX_SATA_PORTS_PCH_LP;
  } else if (IsPchH ()) {
    return MAX_SATA_PORTS_PCH_H;
  } else if (IsPchS ()) {
    return MAX_SATA_PORTS_PCH_S;
  }
  ASSERT (FALSE);
  return 0;
}

/**
  Get Pch Pcie Root Port Device and Function Number by Root Port physical Number

  @param[in]  RpNumber              Root port physical number. (0-based)
  @param[out] RpDev                 Return corresponding root port device number.
  @param[out] RpFun                 Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port device and function is retrieved
  @retval     EFI_INVALID_PARAMETER RpNumber is invalid
**/
EFI_STATUS
EFIAPI
GetPchPcieRpDevFun (
  IN  UINTN   RpNumber,
  OUT UINTN   *RpDev,
  OUT UINTN   *RpFun
  )
{
  UINTN       Index;
  UINTN       FuncIndex;
  UINT32      PciePcd;

  if (RpNumber >= GetPchMaxPciePortNum ()) {
    DEBUG ((DEBUG_ERROR, "GetPchPcieRpDevFun invalid RpNumber %x", RpNumber));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  Index = RpNumber / PCH_PCIE_CONTROLLER_PORTS;
  FuncIndex = RpNumber - mPchPcieControllerInfo[Index].RpNumBase;
  *RpDev = mPchPcieControllerInfo[Index].DevNum;
  if (IsPchS ()) {
    PciePcd = PchPcrRead32 (mPchPcieControllerInfo[Index].Pid, R_SPX_SIP16_PCR_PCD);
   } else {
    PciePcd = PchPcrRead32 (mPchPcieControllerInfo[Index].Pid, R_SPX_PCR_PCD);
  }
  *RpFun = (PciePcd >> (FuncIndex * S_SPX_PCR_PCD_RP_FIELD)) & B_SPX_PCR_PCD_RP1FN;

  return EFI_SUCCESS;
}
