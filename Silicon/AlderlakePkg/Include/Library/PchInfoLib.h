/** @file
  Header file for PchInfoLib.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_INFO_LIB_H_
#define _PCH_INFO_LIB_H_

#include <Uefi/UefiBaseType.h>

typedef UINT8 PCH_STEPPING;


typedef UINT8 PCH_SERIES;
#define PCH_H                   1
#define PCH_LP                  2
#define PCH_N                   3
#define PCH_S                   4
#define PCH_P                   5
#define PCH_M                   6
#define PCH_UNKNOWN_SERIES      0xFF

typedef UINT8 PCH_GENERATION;
#define CNL_PCH                 3
#define ICL_PCH                 4
#define TGL_PCH                 5
#define JSL_PCH                 6
#define ADL_PCH                 7
#define CDF_PCH                 0x80
#define EBG_PCH                 0x81

typedef enum {
  RstUnsupported  = 0,
  RstPremium,
  RstOptane,
  RstMaxMode
} RST_MODE;

/**
  Return LPC Device Id

  @retval PCH_LPC_DEVICE_ID         PCH Lpc Device ID
**/
UINT16
PchGetLpcDid (
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
  Determine if PCH is supported

  @retval TRUE                    PCH is supported
  @retval FALSE                   PCH is not supported
**/
BOOLEAN
IsPchSupported (
  VOID
  );

/**
  Return Pch Series

  @retval PCH_SERIES                Pch Series
**/
PCH_SERIES
PchSeries (
  VOID
  );

/**
  Check if this is PCH LP series

  @retval TRUE                It's PCH LP series
  @retval FALSE               It's not PCH LP series
**/
BOOLEAN
IsPchLp (
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
  Check if this is PCH N series

  @retval TRUE                It's PCH N series
  @retval FALSE               It's not PCH N series
**/
BOOLEAN
IsPchN (
  VOID
  );

/**
  Check if this is PCH P series

  @retval TRUE                It's PCH P series
  @retval FALSE               It's not PCH P series
**/
BOOLEAN
IsPchP (
  VOID
  );

/**
  Check if this is PCH M series

  @retval TRUE                It's PCH M series
  @retval FALSE               It's not PCH M series
**/
BOOLEAN
IsPchM (
  VOID
  );

/**
  Check if this is PCH S series

  @retval TRUE                It's PCH S series
  @retval FALSE               It's not PCH S series
**/
BOOLEAN
IsPchS (
  VOID
  );

/**
  Check if this is Server PCH

  @retval TRUE                It's a Server PCH
  @retval FALSE               It's not a Server PCH
**/
BOOLEAN
IsPchServer (
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
  Check if this is CDF PCH generation

  @retval TRUE                It's CDF PCH
  @retval FALSE               It's not CDF PCH
**/
BOOLEAN
IsCdfPch (
  VOID
  );

/**
  Check if this is EBG PCH generation

  @retval TRUE                It's EBG PCH
  @retval FALSE               It's not EBG PCH
**/
BOOLEAN
IsEbgPch (
  VOID
  );

/**
  Check if this is CNL PCH generation

  @retval TRUE                It's CNL PCH
  @retval FALSE               It's not CNL PCH
**/
BOOLEAN
IsCnlPch (
  VOID
  );

/**
  Check if this is ICL PCH generation

  @retval TRUE                It's ICL PCH
  @retval FALSE               It's not ICL PCH
**/
BOOLEAN
IsIclPch (
  VOID
  );

/**
  Check if this is TGL PCH generation

  @retval TRUE                It's TGL PCH
  @retval FALSE               It's not TGL PCH
**/
BOOLEAN
IsTglPch (
  VOID
  );

/**
  Check if this is JSL PCH generation

  @retval TRUE                It's JSL PCH
  @retval FALSE               It's not JSL PCH
**/
BOOLEAN
IsJslPch (
  VOID
  );

/**
  Check if this is ADL PCH generation

  @retval TRUE                It's ADL PCH
  @retval FALSE               It's not ADL PCH
**/
BOOLEAN
IsAdlPch (
  VOID
  );

/**
  Get Maximum Sata Port Number

  @param[in]  SataCtrlIndex       SATA controller index

  @retval     Maximum Sata Port Number
**/
UINT8
MaxSataPortNum (
  IN UINT32      SataCtrlIndex
  );

/**
  Check if this is Server SKU

  @retval TRUE                It's PCH Server SKU
  @retval FALSE               It's not PCH Server SKU
**/
BOOLEAN
IsPchServerSku (
  VOID
  );

/**
  Get Pch Maximum Pcie Root Port Number

  @retval PcieMaxRootPort         Pch Maximum Pcie Root Port Number
**/
UINT8
GetPchMaxPciePortNum (
  VOID
  );

/**
  Get Pch Maximum Pcie Controller Number

  @retval Pch Maximum Pcie Controller Number
**/
UINT8
GetPchMaxPcieControllerNum (
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
  Get Pch Maximum Usb2 Port Number of XHCI Controller

  @retval Pch Maximum Usb2 Port Number of XHCI Controller
**/
UINT8
GetPchXhciMaxUsb2PortNum (
  VOID
  );

/**
  Get Pch Usb3 Maximum Physical Port Number

  @retval Pch Usb3 Maximum Physical Port Number
**/
UINT8
GetPchUsb3MaxPhysicalPortNum (
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

/**
  Get Pch Maximum Serial IO SPI Chip Selects count

  @retval Pch Maximum Serial IO SPI Chip Selects nu,ber
**/
UINT8
GetPchMaxSerialIoSpiChipSelectsNum (
  VOID
  );

/**
  Get Pch Maximum ISH UART Controller number

  @retval Pch Maximum ISH UART controllers number
**/
UINT8
GetPchMaxIshUartControllersNum (
  VOID
  );

/**
  Get Pch Maximum ISH I2C Controller number

  @retval Pch Maximum ISH I2C controllers number
**/
UINT8
GetPchMaxIshI2cControllersNum (
  VOID
  );

/**
  Get Pch Maximum ISH I3C Controller number

  @retval Pch Maximum ISH I3C controllers number
**/
UINT8
GetPchMaxIshI3cControllersNum (
  VOID
  );

/**
  Get Pch Maximum ISH SPI Controller number

  @retval Pch Maximum ISH SPI controllers number
**/
UINT8
GetPchMaxIshSpiControllersNum (
  VOID
  );

/**
  Get Pch Maximum ISH SPI Controller Cs pins number

  @retval Pch Maximum ISH SPI controller Cs pins number
**/
UINT8
GetPchMaxIshSpiControllerCsPinsNum (
  VOID
  );

/**
  Get Pch Maximum ISH GP number

  @retval Pch Maximum ISH GP number
**/
UINT8
GetPchMaxIshGpNum (
  VOID
  );

/**
  Get Pch Maximum ME Applet count

  @retval Pch Maximum ME Applet number
**/
UINT8
GetPchMaxMeAppletCount (
  VOID
  );

/**
Get Pch Maximum ME Session count

@retval Pch Maximum ME Sesion number
**/
UINT8
GetPchMaxMeSessionCount(
  VOID
);

/**
  Get Pch Maximum Type C Port Number

  @retval Pch Maximum Type C Port Number
**/
UINT8
GetPchMaxTypeCPortNum (
  VOID
  );

#define PCH_STEPPING_STR_LENGTH_MAX 3

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
  );

/**
  Get PCH series ASCII string.
  The return string is zero terminated.

  @retval Static ASCII string of PCH Series
**/
CHAR8*
PchGetSeriesStr (
  );

/**
  Get PCH Sku ASCII string
  The return string is zero terminated.

  @retval Static ASCII string of PCH Sku
**/
CHAR8*
PchGetSkuStr (
  VOID
  );

/**
  Check if this chipset supports eMMC controller

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
IsPchEmmcSupported (
  VOID
  );

/**
  Check if this chipset supports SD controller

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
IsPchSdCardSupported (
  VOID
  );

/**
  Check if this chipset supports THC controller

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
IsPchThcSupported (
  VOID
  );

/**
  Check if this chipset supports HSIO BIOS Sync

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
IsPchChipsetInitSyncSupported (
  VOID
  );

/**
  Gets the maximum number of UFS controller supported by this chipset.

  @return Number of supported UFS controllers
**/
UINT8
PchGetMaxUfsNum (
  VOID
  );

/**
  Get RST mode supported by the silicon

  @retval RST_MODE    RST mode supported by silicon
**/
RST_MODE
PchGetSupportedRstMode (
  VOID
  );

/**
  Check whether integrated LAN controller is supported.

  @retval TRUE                    GbE is supported in PCH
  @retval FALSE                   GbE is not supported by PCH
**/
BOOLEAN
PchIsGbeSupported (
  VOID
  );

/**
  Check whether integrated TSN is supported.

  @retval TRUE                    TSN is supported in current PCH
  @retval FALSE                   TSN is not supported on current PCH
**/
BOOLEAN
PchIsTsnSupported (
  VOID
  );

/**
  Check whether ISH is supported.

  @retval TRUE                    ISH is supported in PCH
  @retval FALSE                   ISH is not supported by PCH
**/
BOOLEAN
PchIsIshSupported (
  VOID
  );

/**
  Check whether ATX Shutdown (PS_ON) is supported.

  @retval    TRUE           ATX Shutdown (PS_ON) is supported in PCH
  @retval    FALSE          ATX Shutdown (PS_ON) is not supported by PCH
**/
BOOLEAN
IsPchPSOnSupported (
  VOID
  );

/**
  Check if link between PCH and CPU is an P-DMI

  @retval    TRUE           P-DMI link
  @retval    FALSE          Not an P-DMI link
**/
BOOLEAN
IsPchWithPdmi (
  VOID
  );

/**
  Check if link between PCH and CPU is an OP-DMI

  @retval    TRUE           OP-DMI link
  @retval    FALSE          Not an OP-DMI link
**/
BOOLEAN
IsPchWithOpdmi (
  VOID
  );

/**
  Check if link between PCH and CPU is an F-DMI

  @retval    TRUE           F-DMI link
  @retval    FALSE          Not an F-DMI link
**/
BOOLEAN
IsPchWithFdmi (
  VOID
  );

/**
  Get Pch Maximum THC count

  @retval Pch Maximum THC count number
**/
UINT8
GetPchMaxThcCount (
  VOID
  );

typedef enum {
  SataSosc125Mhz = 0,
  SataSosc120Mhz,
  SataSosc100Mhz,
  SataSosc25Mhz,
  SataSosc19p2Mhz,
  SataSoscUnsupported
} SATA_SOSC_CLK_FREQ;

/**
  Returns a frequency of the sosc_clk signal.
  All SATA controllers on the system are assumed to
  work on the same sosc_clk frequency.

  @retval Frequency of the sosc_clk signal.
**/
SATA_SOSC_CLK_FREQ
GetSataSoscClkFreq (
  VOID
  );

/**
  Check if SATA support should be awake after function disable

  @retval    TRUE
  @retval    FALSE
**/
BOOLEAN
IsSataSupportWakeAfterFunctionDisable (
  VOID
  );


//
// USB2 PHY reference frequencies values (MHz)
//
typedef enum {
  FREQ_19_2 = 0u,
  FREQ_24_0,
  FREQ_96_0,
  FREQ_MAX
} USB2_PHY_REF_FREQ;

/**
  Returns USB2 PHY Reference Clock frequency value used by PCH
  This defines what electrical tuning parameters shall be used
  during USB2 PHY initialization programming

  @retval Frequency reference clock for USB2 PHY
**/
USB2_PHY_REF_FREQ
GetUsb2PhyRefFreq (
  VOID
  );

/**
  return support status for P2SB PCR 20-bit addressing

  @retval    TRUE
  @retval    FALSE
**/
BOOLEAN
IsP2sb20bPcrSupported (
  VOID
  );

/**
  Check if SPI in a given PCH generation supports an Extended BIOS Range Decode

  @retval TRUE or FALSE if PCH supports Extended BIOS Range Decode
**/
BOOLEAN
IsExtendedBiosRangeDecodeSupported (
  VOID
  );

/**
  Returns DMI target for current PCH SPI

  @retval PCH SPI DMI target
**/
UINT16
GetPchSpiDmiTarget (
  VOID
  );
#endif // _PCH_INFO_LIB_H_
