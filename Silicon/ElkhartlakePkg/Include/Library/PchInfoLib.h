/** @file
  Header file for PchInfoLib.

  Copyright (c) 2014 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_INFO_LIB_H_
#define _PCH_INFO_LIB_H_

#include <PchAccess.h>

typedef UINT8 PCH_SERIES;

#define PCH_H                   1
#define PCH_LP                  2
#define PCH_N                   3
#define PCH_UNKNOWN_SERIES      0xFF

typedef UINT8 PCH_GENERATION;
#define CNL_PCH                 3
#define ICL_PCH                 4
#define EHL_PCH                 6
#define CDF_PCH                 0x80

typedef enum {
  RstUnsupported  = 0,
  RstPremium,
  RstOptane,
  RstMaxMode
} RST_MODE;

typedef enum {
  PchMobileSku = 0,
  PchDesktopSku,
  PchServerSku,
  PchUnknownSku,
  PchMaxSku
} PCH_SKU_TYPE;

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
EFIAPI
PchStepping (
  VOID
  );

/**
  Return Pch Series

  @retval PCH_SERIES                Pch Series
**/
PCH_SERIES
EFIAPI
GetPchSeries (
  VOID
  );

/**
  Check if this is PCH LP series

  @retval TRUE                It's PCH LP series
  @retval FALSE               It's not PCH LP series
**/
BOOLEAN
EFIAPI
IsPchLp (
  VOID
  );

/**
  Check if a given Root Port is Multi VC

  @param[in] RpIndex (0-based)

  @return  TRUE if it is Multi VC
**/
BOOLEAN
EFIAPI
IsRpMultiVC (
  IN  UINT32                   RpIndex
  );

/**
  Get Pch Maximum Pcie Root Port Number

  @retval PcieMaxRootPort         Pch Maximum Pcie Root Port Number
**/
UINT8
EFIAPI
GetPchMaxPciePortNum (
  VOID
  );

/**
  Get Pch Maximum Pcie Controller Number

  @retval Pch Maximum Pcie Controller Number
**/
UINT8
EFIAPI
GetPchMaxPcieControllerNum (
  VOID
  );

/**
  Return the Controller index base on Root Port Index. This is to take care of situation where the Root Port Number is not symetrical across controller

  @param[in] RpIndex Root Port Number (0-based)

  @return  Controller Index
**/
UINT8
GetControllerIndex (
  IN  UINT32                   RpIndex
  );

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
  );

/**
  Get Pch Maximum Pcie Clock Number

  @retval Pch Maximum Pcie Clock Number
**/
UINT8
EFIAPI
GetPchMaxPcieClockNum (
  VOID
  );

/**
  Get Pch Usb Maximum Physical Port Number

  @retval Pch Usb Maximum Physical Port Number
**/
UINT8
EFIAPI
GetPchUsbMaxPhysicalPortNum (
  VOID
  );

/**
  Get Pch Maximum Usb2 Port Number of XHCI Controller

  @retval Pch Maximum Usb2 Port Number of XHCI Controller
**/
UINT8
EFIAPI
GetPchXhciMaxUsb2PortNum (
  VOID
  );

/**
  Get Pch Maximum Usb3 Port Number of XHCI Controller

  @retval Pch Maximum Usb3 Port Number of XHCI Controller
**/
UINT8
EFIAPI
GetPchXhciMaxUsb3PortNum (
  VOID
  );

/**
  Get PCH stepping ASCII string
  The return string is zero terminated.

  @param [in]      PchStep              Pch stepping
  @param [out]     Buffer               Output buffer of string
  @param [in,out]  BufferSize           Size of input buffer,
                                        and return required string size when buffer is too small.

  @retval EFI_SUCCESS                   String copy successfully
  @retval EFI_INVALID_PARAMETER         The stepping is not supported, or parameters are NULL
  @retval EFI_BUFFER_TOO_SMALL          Input buffer size is too small
**/
EFI_STATUS
PchGetSteppingStr (
  IN     PCH_STEPPING                   PchStep,
  OUT    CHAR8                          *Buffer,
  IN OUT UINT32                         *BufferSize
  );

/**
  Get PCH series ASCII string
  The return string is zero terminated.

  @param [in]      PchSer               Pch series
  @param [out]     Buffer               Output buffer of string
  @param [in,out]  BufferSize           Size of input buffer,
                                        and return required string size when buffer is too small.

  @retval EFI_SUCCESS                   String copy successfully
  @retval EFI_INVALID_PARAMETER         The series is not supported, or parameters are NULL
  @retval EFI_BUFFER_TOO_SMALL          Input buffer size is too small
**/
EFI_STATUS
PchGetSeriesStr (
  IN     PCH_SERIES                     PchSer,
  OUT    CHAR8                          *Buffer,
  IN OUT UINT32                         *BufferSize
  );

/**
  Get PCH Sku ASCII string
  The return string is zero terminated.

  @param [in]      LpcDid               LPC device id
  @param [out]     Buffer               Output buffer of string
  @param [in,out]  BufferSize           Size of input buffer,
                                        and return required string size when buffer is too small.

  @retval EFI_SUCCESS                   String copy successfully
  @retval EFI_INVALID_PARAMETER         The series is not supported, or parameters are NULL
  @retval EFI_BUFFER_TOO_SMALL          Input buffer size is too small
**/
EFI_STATUS
PchGetSkuStr (
  IN     UINT16                         LpcDid,
  OUT    CHAR8                          *Buffer,
  IN OUT UINT32                         *BufferSize
  );

/**
  Get PCH SKU type

  @retval PCH_SKU_TYPE  Type of PCH SKU
**/
PCH_SKU_TYPE
GetPchSkuType (
  VOID
  );

/**
  Get Pch Maximum Sata Controller Number

  @param[in] None

  @retval Pch Maximum Sata Controller Number
**/
UINT8
EFIAPI
GetPchMaxSataControllerNum (
  VOID
  );

/**
  Get Pch Maximum Sata Port Number

  @param[in]  SataCtrlIndex       SATA controller index

  @retval Pch Maximum Sata Port Number
**/
UINT8
EFIAPI
GetPchMaxSataPortNum (
  IN UINT32     SataCtrlIndex
  );

  /**
  Get Pch Maximum Serial IO controllers number

  @retval Pch Maximum Serial IO controllers number
**/
UINT8
EFIAPI
GetPchMaxSerialIoControllersNum (
  VOID
  );

/**
  Get Pch Maximum Serial IO SPI controllers number

  @retval Pch Maximum Serial IO SPI controllers number
**/
UINT8
EFIAPI
GetPchMaxSerialIoSpiControllersNum (
  VOID
  );

/**
  Get Pch Maximum Serial IO UART controllers number

  @retval Pch Maximum Serial IO UART controllers number
**/
UINT8
EFIAPI
GetPchMaxSerialIoUartControllersNum (
  VOID
  );

/**
  Get Pch Maximum ISH SPI Controller number

  @retval Pch Maximum ISH SPI controllers number
**/
UINT8
EFIAPI
GetPchMaxIshSpiControllersNum (
  VOID
  );

/**
  Get Pch Maximum ISH UART Controller number

  @retval Pch Maximum ISH UART controllers number
**/
UINT8
EFIAPI
GetPchMaxIshUartControllersNum (
  VOID
  );

/**
  Get Pch Maximum ISH I2C Controller number

  @retval Pch Maximum ISH I2C controllers number
**/
UINT8
EFIAPI
GetPchMaxIshI2cControllersNum (
  VOID
  );

/**
  Get Pch Maximum ISH GP number

  @retval Pch Maximum ISH GP number
**/
UINT8
EFIAPI
GetPchMaxIshGpNum (
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
  Get Pch Maximum PSE I2S Controller number

  @retval Pch Maximum PSE I2S Controller number
**/
UINT8
GetPchMaxPseI2sControllersNum (
  VOID
  );

/**
  Get Pch Maximum PSE UART Controller number

  @retval Pch Maximum PSE UART Controller number
**/
UINT8
GetPchMaxPseUartControllersNum (
  VOID
  );


/**
  Get Pch Maximum PSE HSUART Controller number

  @retval Pch Maximum PSE HSUART Controller number
**/
UINT8
GetPchMaxPseHsuartControllersNum (
  VOID
  );

/**
  Get Pch Maximum PSE QEP Controller number

  @retval Pch Maximum PSE QEP Controller number
**/
UINT8
GetPchMaxPseQepControllersNum (
  VOID
  );

/**
  Get Pch Maximum PSE I2C Controller number

  @retval Pch Maximum PSE I2C Controller number
**/
UINT8
GetPchMaxPseI2cControllersNum (
  VOID
  );

/**
  Get Pch Maximum PSE SPI Controller number

  @retval Pch Maximum PSE SPI Controller number
**/
UINT8
GetPchMaxPseSpiControllersNum (
  VOID
  );

/**
  Get Pch Maximum PSE DMA Controller number

  @retval Pch Maximum PSE DMA Controller number
**/
UINT8
GetPchMaxPseDmaControllersNum (
  VOID
  );


/**
  Get Pch Maximum PSE GBE Controller number

  @retval Pch Maximum PSE GBE Controller number
**/
UINT8
GetPchMaxPseGbeControllersNum (
  VOID
  );

/**
  Get Pch Maximum PSE SPI Controller Cs pins number

  @retval Pch Maximum PSE SPI controller Cs pins number
**/
UINT8
GetPchMaxPseSpiControllerCsPinsNum (
  VOID
  );

/**
  Get Pch Maximum PSE ADC Controller PIN number

  @retval Pch Maximum PSE ADC Controller PIN number
**/
UINT8
GetPchMaxPseAdcControllerPinsNum (
  VOID
  );

/**
  Get Pch Maximum PSE CAN Controller number

  @retval Pch Maximum PSE CAN Controller number
**/
UINT8
GetPchMaxPseCanControllersNum (
  VOID
  );

/**
  Get Pch Maximum PSE TRACE Controller number

  @retval Pch Maximum PSE TRACE Controller number
**/
UINT8
GetPchMaxPseTraceControllersNum (
  VOID
  );

/**
  Get Pch Maximum PSE Timed GPIO Controller number

  @retval Pch Maximum PSE Timed GPIO Controller number
**/
UINT8
GetPchMaxPseTimedGpioControllersNum (
  VOID
  );

/**
  Get Pch Maximum PSE Timed GPIO Pins number for per controller block

  @retval Pch Maximum PSE Timed GPIO Controller Pins number per controller block
**/
UINT8
GetPchMaxPseTimdedGpioPinsNumPerController (
  VOID
  );

/**
  Get Pch Maximum Total PSE Timed GPIO Pins number

  @retval Pch Maximum Total PSE Timed GPIO Controller Pins number
**/
UINT8
GetPchMaxPseTimdedGpioPinsNumTotal (
  VOID
  );

/**
  Get Pch Maximum PSE Timed GPIO Controller number

  @retval Pch Maximum PSE Timed GPIO Controller number
**/
UINT8
GetPchMaxPseTimedGpioControllersNum (
  VOID
  );
/**
  Get Pch Maximum PSE PWM number

  @retval Pch Maximum PSE PWM number
**/
UINT8
GetPchMaxPsePWMNum (
  VOID
);
#endif // _PCH_INFO_LIB_H_
