/** @file
  Header file for MtlPchInfoLib.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _MTL_PCH_INFO_LIB_H_
#define _MTL_PCH_INFO_LIB_H_

typedef UINT8 MTL_PCH_SERIES;
typedef UINT8 MTL_PCH_STEPPING;

#define MTL_PCH_S 0x0
#define MTL_PCH_UNKNOWN_SERIES  0xFF


#define MTL_PCH_INFO_STEPPING_STR_MIN_LENGTH  3

/**
  Checks if PCH is supported.

  @return TRUE if PCH is supported by BIOS.
**/
BOOLEAN
MtlPchIsSupported (
  VOID
  );

/**
  Return eSPI Device Id.

  @retval  MTL PCH eSPI Device ID
**/
UINT16
MtlPchGetEspiDid (
  VOID
  );

/**
  Return PCH Series

  @retval PCH_SERIES     PCH Series
**/
MTL_PCH_SERIES
MtlPchGetSeries (
  VOID
  );

/**
  Get MTL PCH series string.

  @return Pointer to NULL terminated string with MTL PCH series name.
**/
CHAR8*
MtlPchGetSeriesStr (
  VOID
  );

/**
  Get MTL PCH SKU str.

  @retval Static ASCII string of MTL PCH SKU
**/
CHAR8*
MtlPchGetSkuStr (
  VOID
  );

/**
  Return PCH stepping type

  @retval PCH_STEPPING   PCH stepping type
**/
MTL_PCH_STEPPING
MtlPchGetStepping (
  VOID
  );

/**
  Print MTL PCH stepping string into buffer.

  @param [out]     Buffer               Output buffer of string
  @param [in]      BufferSize           Buffer size.
                                        Must not be less than MTL_PCH_INFO_STEPPING_STR_MIN_LENGTH

  @retval EFI_SUCCESS                   String copied successfully
  @retval EFI_INVALID_PARAMETER         The stepping is not supported, or parameters are NULL
  @retval EFI_BUFFER_TOO_SMALL          Input buffer size is too small
**/
EFI_STATUS
MtlPchGetSteppingStr (
  OUT CHAR8  *Buffer,
  IN UINT32  BufferSize
  );

/**
  Get number of PCIe root ports on MTL PCH

  @retval Number of root ports on the PCH
**/
UINT8
MtlPchGetMaxPciePortNum (
  VOID
  );

/**
  Get number of PCIe controller on MTL PCH.

  @return Number of controllers on the PCH.
**/
UINT8
MtlPchGetMaxPcieControllerNum (
  VOID
  );

/**
  This function installs DieInfo instance for PCH die.

  @retval EFI_SUCCESS if instance has been installed successfully.
**/
EFI_STATUS
MtlPchInstallDieInfo (
  VOID
  );

/**
  Get Pch Maximum Serial IO UART controllers number

  @retval Pch Maximum Serial IO UART controllers number
**/
UINT8
MtlPchGetMaxSerialIoUartControllersNum (
  VOID
  );

/**
  Get Serial IO UART controller PCIe Device Number

  @param[in]  UartNumber       Serial IO UART controller index

  @retval Serial IO UART controller PCIe Device Number
**/
UINT8
MtlPchSerialIoUartDevNumber (
  IN UINT8       UartNumber
  );

/**
  Get Serial IO UART controller PCIe Function Number

  @param[in]  UartNumber       Serial IO UART controller index

  @retval Serial IO UART controller PCIe Function Number
**/
UINT8
MtlPchSerialIoUartFuncNumber (
  IN UINT8       UartNumber
  );

/**
  Get Serial IO UART controller address that can be passed to the PCI Segment Library functions.

  @param[in]  UartNumber       Serial IO UART controller index

  @retval Serial IO UART controller address in PCI Segment Library representation
**/
UINT64
MtlPchSerialIoUartPciCfgBase (
  IN UINT8        UartNumber
  );

/**
  Get Pch Maximum Serial IO SPI controllers number

  @retval Pch Maximum Serial IO SPI controllers number
**/
UINT8
MtlPchGetMaxSerialIoSpiControllersNum (
  VOID
  );

/**
  Get Serial IO SPI controller PCIe Device Number

  @param[in]  I2cNumber       Serial IO SPI controller index

  @retval Serial IO SPI controller PCIe Device Number
**/
UINT8
MtlPchSerialIoSpiDevNumber (
  IN UINT8       SpiNumber
  );

/**
  Get Serial IO SPI controller PCIe Function Number

  @param[in]  SpiNumber       Serial IO SPI controller index

  @retval Serial IO SPI controller PCIe Function Number
**/
UINT8
MtlPchSerialIoSpiFuncNumber (
  IN UINT8       SpiNumber
  );

/**
  Get Serial IO SPI controller address that can be passed to the PCI Segment Library functions.

  @param[in]  SpiNumber       Serial IO SPI controller index

  @retval Serial IO SPI controller address in PCI Segment Library representation
**/
UINT64
MtlPchSerialIoSpiPciCfgBase (
  IN UINT8        SpiNumber
  );

/**
  Get Pch Maximum Serial IO I2C controllers number

  @retval Pch Maximum Serial IO I2C controllers number
**/
UINT8
MtlPchGetMaxSerialIoI2cControllersNum (
  VOID
  );

/**
  Get Serial IO I2C controller PCIe Device Number

  @param[in]  I2cNumber       Serial IO I2C controller index

  @retval Serial IO I2C controller PCIe Device Number
**/
UINT8
MtlPchSerialIoI2cDevNumber (
  IN UINT8       I2cNumber
  );

/**
  Get Serial IO I2C controller PCIe Function Number

  @param[in]  I2cNumber       Serial IO I2C controller index

  @retval Serial IO I2C controller PCIe Function Number
**/
UINT8
MtlPchSerialIoI2cFuncNumber (
  IN UINT8       I2cNumber
  );

/**
  Get Serial IO I2C controller address that can be passed to the PCI Segment Library functions.

  @param[in]  I2cNumber       Serial IO I2C controller index

  @retval Serial IO I2C controller address in PCI Segment Library representation
**/
UINT64
MtlPchSerialIoI2cPciCfgBase (
  IN UINT8        I2cNumber
  );

/**
  Get Pch Maximum Serial IO I3C controllers number

  @retval Pch Maximum Serial IO I3C controllers number
**/
UINT8
MtlPchGetMaxSerialIoI3cControllersNum (
  VOID
  );

/**
  Get Mtl Pch Maximum THC count

  @retval Pch Maximum THC count number
**/
UINT8
MtlPchGetMaxThcCount (
  VOID
  );

/**
  Get Serial IO I3C controller PCIe Device Number

  @param[in]  I3CNumber       Serial IO I3C controller index

  @retval Serial IO I3C controller PCIe Device Number
**/
UINT8
MtlPchSerialIoI3cDevNumber (
  IN UINT8       I3cNumber
  );

/**
  Get Serial IO I3C controller PCIe Function Number

  @param[in]  I3cNumber       Serial IO I3C controller index

  @retval Serial IO I3C controller PCIe Function Number
**/
UINT8
MtlPchSerialIoI3cFuncNumber (
  IN UINT8       I3cNumber
  );

/**
  Get Serial IO I3C controller address that can be passed to the PCI Segment Library functions.

  @param[in]  I3cNumber       Serial IO I3C controller index

  @retval Serial IO I3C controller address in PCI Segment Library representation
**/
UINT64
MtlPchSerialIoI3cPciCfgBase (
  IN UINT8        I3cNumber
  );

/**
  Get PCH maximum ISH SPI controller number

  @retval PCH maximum ISH SPI controller number
**/
UINT8
MtlPchGetMaxIshSpiControllersNum (
  VOID
  );

/**
  Get PCH maximum ISH SPI controller Cs pins number

  @retval PCH maximum ISH SPI controller Cs pins number
**/
UINT8
MtlPchGetMaxIshSpiControllerCsPinsNum (
  VOID
  );

/**
  Get PCH maximum ISH UART controllers number

  @retval PCH maximum ISH UART controllers number
**/
UINT8
MtlPchGetMaxIshUartControllersNum (
  VOID
  );

/**
  Get PCH maximum ISH I2C controllers number

  @retval PCH maximum ISH I2C controllers number
**/
UINT8
MtlPchGetMaxIshI2cControllersNum (
  VOID
  );

/**
  Get PCH maximum ISH I3C controllers number

  @retval PCH maximum ISH I3C controllers number
**/
UINT8
MtlPchGetMaxIshI3cControllersNum (
  VOID
  );

/**
  Get Pch Maximum ISH GP number

  @retval Pch Maximum ISH GP number
**/
UINT8
MtlPchGetMaxIshGpNum (
  VOID
  );

/**
  Check if this chipset supports Gbe controller

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
MtlPchIsGbeSupported (
  VOID
  );

/**
  Check if this chipset supports THC controller

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
MtlPchIsThcSupported (
  VOID
  );

/**
  Get Pch Maximum Hda Sndw Link

  @retval Pch Maximum Hda Sndw Link
**/
UINT8
MtlPchGetHdaMaxSndwLinkNum (
  VOID
  );

/**
  Get Pch Maximum Hda Ssp Link

  @retval Pch Maximum Hda Ssp Link
**/
UINT8
MtlPchGetHdaMaxSspLinkNum (
  VOID
  );

/**
  Get Pch Maximum Hda Dmic Link

  @retval Pch Maximum Hda Dmic Link
**/
UINT8
MtlPchGetHdaMaxDmicLinkNum (
  VOID
  );

/**
  Check whether ATX Shutdown (PS_ON) is supported.

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
MtlPchIsPSOnSupported (
  VOID
  );

/**
  Check whether XHCI Controller is supported.

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
MtlPchIsXhciSupported (
  VOID
  );

/**
  Check whether integrated TSN is supported by PCH Series.

  @retval TRUE                    TSN is supported in current PCH
  @retval FALSE                   TSN is not supported on current PCH
**/
BOOLEAN
MtlPchIsTsnSupported (
  VOID
  );

/**
  Get Pch Usb Maximum Physical Port Number

  @retval Pch Usb Maximum Physical Port Number
**/
UINT8
MtlPchGetUsb2MaxPhysicalPortNum (
  VOID
  );

/**
  Get Pch Maximum Usb2 Port Number of XHCI Controller

  @retval Pch Maximum Usb2 Port Number of XHCI Controller
**/
UINT8
MtlPchGetXhciMaxUsb2PortNum (
  VOID
  );

/**
  Get Pch Maximum Usb3 Port Number of XHCI Controller

  @retval Pch Maximum Usb3 Port Number of XHCI Controller
**/
UINT8
MtlPchGetXhciMaxUsb3PortNum (
  VOID
  );

/**
  Get Pch USB-R Port Number for KVM session

  @retval Pch USB-R Port Number for KVM session
**/
UINT8
MtlPchGetUsbrKvmPortNum (
  VOID
  );

/**
  Get Pch USB-R Port Number for Storage session

  @retval Pch USB-R Port Number for Storage session
**/
UINT8
MtlPchGetUsbrStoragePortNum (
  VOID
  );

/**
  Get Pch Maximum Pcie Clock Number

  @retval Pch Maximum Pcie Clock Number
**/
UINT8
MtlPchGetPchMaxPcieClockNum (
  VOID
);

/**
  Get Pch Maximum Pcie ClockReq Number

  @retval Pch Maximum Pcie ClockReq Number
**/
UINT8
MtlPchGetMaxPcieClockReqNum (
  VOID
  );

/**
  Translate RpIndex to Controller index.

  @param[in] RpIndex  Root port index

  @return ControllerIndex
  @retval 0xFF Failed to lookup Controller index.
**/
UINT32
MtlPchGetControllerIndex (
  IN UINT32  RpIndex
  );

/**
  Returns segment number where PCH is located.

  @return Segment number
**/
UINT8
MtlPchGetSegmentNumber (
  VOID
  );

/**
  Returns bus number where PCH Die is located.

  @return Bus number
**/
UINT8
MtlPchDieGetBusNumber (
  VOID
  );


#endif