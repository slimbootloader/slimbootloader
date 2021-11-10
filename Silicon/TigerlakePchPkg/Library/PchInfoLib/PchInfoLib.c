/** @file
  Pch information library for TGL.

  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <IndustryStandard/Pci30.h>
#include <Library/PchInfoLib.h>
#include <Register/PchRegsLpc.h>
#include <Library/PrintLib.h>
#include <Library/PchInfoLib.h>
#include <PchAccess.h>
#include <Register/PchBdfAssignment.h>
#include <Library/PchPcrLib.h>
#include <Register/PchRegsPcie.h>

//#if FixedPcdGet8(PcdEmbeddedEnable) == 0x1
//#include <Library/TsnLib.h>
//#endif

#define PCH_LP_Z0             0x00
#define PCH_LP_Z1             0x01
#define PCH_LP_A0             0x10
#define PCH_LP_A1             0x11
#define PCH_LP_B0             0x20
#define PCH_LP_B1             0x21
#define PCH_LP_C0             0x30
#define PCH_LP_C1             0x31
#define PCH_LP_D0             0x40
#define PCH_LP_D1             0x41

typedef struct {
  UINT8 DevNum;
  UINT8 Pid;
  UINT8 RpNumBase;
} PCH_PCIE_CONTROLLER_INFO;
CONST PCH_PCIE_CONTROLLER_INFO mPchPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_1,  PID_SPA,  0 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_5,  PID_SPB,  4 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_9,  PID_SPC,  8 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_13, PID_SPD, 12 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_17, PID_SPE, 16 }, // PCH-H only
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_21, PID_SPF, 20 }  // PCH-H only
};
struct PCH_SKU_STRING {
  UINT16        Id;
  CHAR8         *String;
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
  UINT64  LpcBaseAddress;

  LpcBaseAddress = PCI_LIB_ADDRESS (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC,
                     0
                     );
  LpcDeviceId = PciRead16 ((UINTN)(LpcBaseAddress + PCI_DEVICE_ID_OFFSET));

  return LpcDeviceId;
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

    case V_LPC_CFG_DID_TGL_LP:
      return PCH_LP;

    case V_LPC_CFG_DID_TGL_H:
        return PCH_H;

    default:
      return PCH_UNKNOWN_SERIES;
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
  UINT16      LpcDeviceId;

  LpcDeviceId = PchGetLpcDid ();
  return PchSeriesFromLpcDid (LpcDeviceId);
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
  PCH_STEPPING PchStepping;

  // TGP_LP Stepping starts from Z0
  if (IsPchLp ()) {
    switch (RevId) {
      case PCH_LP_Z0:
        PchStepping = PCH_Z0;
        break;
      case PCH_LP_Z1:
        PchStepping = PCH_Z1;
        break;
      case PCH_LP_A0:
        PchStepping = PCH_A0;
        break;
      case PCH_LP_A1:
        PchStepping = PCH_A1;
        break;
      case PCH_LP_B0:
        PchStepping = PCH_B0;
        break;
      case PCH_LP_B1:
        PchStepping = PCH_B1;
        break;
      case PCH_LP_C0:
        PchStepping = PCH_C0;
        break;
      case PCH_LP_C1:
        PchStepping = PCH_C1;
        break;
      case PCH_LP_D0:
        PchStepping = PCH_D0;
        break;
      case PCH_LP_D1:
        PchStepping = PCH_D1;
        break;
      default:
        PchStepping = RevId;
    }
  } else {
    PchStepping = RevId;
  }

  return PchStepping;
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
  UINT8         RevId;
  UINT64        LpcBaseAddress;

  LpcBaseAddress = PCI_LIB_ADDRESS (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC,
                     0
                     );
  RevId = PciRead8 ((UINTN)(LpcBaseAddress + PCI_REVISION_ID_OFFSET));

  return PchSteppingFromRevId (RevId);

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
  return TGL_PCH;
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
  Check if this is PCH LP series

  @retval TRUE                It's PCH LP series
  @retval FALSE               It's not PCH LP series
**/
BOOLEAN
IsPchLp (
  VOID
  )
{


  return  (PchSeries () == PCH_LP);


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
  Check if this is PCH N series

  @retval TRUE                It's PCH N series
  @retval FALSE               It's not PCH N series
**/
BOOLEAN
IsPchN (
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
  UINT16    LpcDeviceId;

  LpcDeviceId = PchGetLpcDid ();

  switch (LpcDeviceId) {

    case V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_0:
    case V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_1:
    case V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_2:
    case V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_4:
    case V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_5:
    case V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_6:
    case V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_7:
      return RstPremium;
      break;

    default:
      return RstUnsupported;
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

    case PCH_LP:
      return "TGL PCH-LP";

    case PCH_H:
      return "TGL PCH-H";

    default:
      return NULL;
  }
}

GLOBAL_REMOVE_IF_UNREFERENCED
struct PCH_SKU_STRING mSkuStrs[] = {
  //
  // TGL PCH LP Mobile LPC Device IDs
  //
  {V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_0,   "TGP SKU 0"},
  {V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_1,   "U Super SKU (SSKU)"},
  {V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_2,   "U Premium"},
  {V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_3,   "U Base/Mainstream"},
  {V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_4,   "TGP SKU 4"},
  {V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_5,   "TGP SKU 5"},
  {V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_6,   "Y Super SKU (SSKU)"},
  {V_TGL_PCH_LP_LPC_CFG_DEVICE_ID_MB_7,   "Y Premium"},
  {0xFFFF, NULL}
};

/**
  Get Pch Maximum Pcie Clock Number

  @retval Pch Maximum Pcie Clock Number
**/
UINT8
GetPchMaxPcieClockNum (
  VOID
  )
{
  if (IsPchH ()) {
    return 16;
  } else {
    return 7;
  }
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
    case PCH_LP:
      return 4;
    case PCH_H:
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
#if FixedPcdGet8(PcdEmbeddedEnable) == 0x1
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
    case PCH_LP:
      return 12;
    case PCH_H:
      return 24;
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
    return 3;
  } else {
    return 6;
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
      if (AudioLinkIndex < 1) {
        return TRUE;
      } else if (AudioLinkIndex < 4) {
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
  //
  // iDisplay Audio Link T-mode support per PCH Generation/Series:
  // 1. 2T  - TGL-LP/H/N
  // 2. 4T  - TGL-LP (default), TGL-H, TGL-N
  // 3. 8T  - TGL-H, TGL-N (default)
  // 4. 16T - TGL-H, TGL-N (not-POR)
  //
  switch (Tmode) {
    case HdaIDispMode1T:
      return FALSE;
    case HdaIDispMode2T:
    case HdaIDispMode4T:
      return TRUE;
    case HdaIDispMode8T:
    case HdaIDispMode16T:
      return (IsPchH ());
    default:
      return FALSE;
  }
}

/**
Get Pch Usb2 Maximum Physical Port Number

@retval Pch Usb2 Maximum Physical Port Number
**/
UINT8
GetPchUsb2MaxPhysicalPortNum(
  VOID
  )
{
  switch (PchSeries()) {
    case PCH_H:
      return 14;
    case PCH_LP:
      return 10;
    default:
      return 0;
  }
}

/**
Get Pch Maximum Usb2 Port Number of XHCI Controller

@retval Pch Maximum Usb2 Port Number of XHCI Controller
**/
UINT8
GetPchXhciMaxUsb2PortNum(
  VOID
  )
{
  switch (PchSeries()) {
    case PCH_H:
      return 16;
    case PCH_LP:
      return 12;
    default:
      return 0;
  }
}

/**
Get Pch Maximum Usb3 Port Number of XHCI Controller

@retval Pch Maximum Usb3 Port Number of XHCI Controller
**/
UINT8
GetPchXhciMaxUsb3PortNum(
  VOID
  )
{
  switch (PchSeries()) {
    case PCH_H:
      return 10;
    case PCH_LP:
      return 4;
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
  return 2;
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
  return TRUE;
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
  return 8;
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
  return 7;
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
  return 7;
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
  Get Pch Maximum Pcie Controller Number

  @retval Pch Maximum Pcie Root Port Number
**/
UINT8
EFIAPI
GetPchMaxPcieControllerNum (
  VOID
  )
{
  return GetPchMaxPciePortNum ()/ PCH_PCIE_CONTROLLER_PORTS;
}
/**
  Return the Controller index base on Root Port Index. This is to take care of situation where the Root Port Number is not symetrical across controller

  @param[in] RpIndex Root Port Number (0-based)

  @return  Controller Index
**/
UINT8
GetControllerIndex (
  IN  UINT32                   RpIndex
  )
{
  switch (RpIndex) {
    case 4:
      return 1;
    case 5:
      return 2;
    case 6:
      return 3;
    default:
      return (UINT8) (RpIndex / PCH_PCIE_CONTROLLER_PORTS);
  }
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
  Index = GetControllerIndex ((UINT32) RpNumber);
  FuncIndex = RpNumber - mPchPcieControllerInfo[Index].RpNumBase;
  *RpDev = mPchPcieControllerInfo[Index].DevNum;
   PciePcd = MmioRead32 (PCH_PCR_ADDRESS(mPchPcieControllerInfo[Index].Pid, R_SPX_PCR_PCD));
   *RpFun = (PciePcd >> (FuncIndex * S_SPX_PCR_PCD_RP_FIELD)) & B_SPX_PCR_PCD_RP1FN;
  return EFI_SUCCESS;
}
