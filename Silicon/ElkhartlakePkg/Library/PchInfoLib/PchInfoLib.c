/** @file
  Pch information library.

  Copyright (c) 2014 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <IndustryStandard/Pci30.h>
#include <Library/PchInfoLib.h>
#include <PchAccess.h>

//
// This module variables are used for cache the static result.
// @note: please pay attention to the PEI phase, the module variables on ROM
//       and can't be modified.
//
GLOBAL_REMOVE_IF_UNREFERENCED PCH_STEPPING     mPchStepping      = PCH_STEPPING_MAX;
GLOBAL_REMOVE_IF_UNREFERENCED PCH_SERIES       mPchSeries        = PCH_UNKNOWN_SERIES;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8            mIsPchLp          = 0xFF;

typedef struct {
  UINT8 DevNum;
  UINT8 Pid;
  UINT8 RpNumBase;
} PCH_PCIE_CONTROLLER_INFO;

CONST PCH_PCIE_CONTROLLER_INFO mPchPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, PID_SPA,  0 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, PID_SPB,  4 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, PID_SPC,  5 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, PID_SPD,  6 }
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
  Return Pch stepping type

  @retval PCH_STEPPING            Pch stepping type
**/
PCH_STEPPING
EFIAPI
PchStepping (
  VOID
  )
{
  UINT8         RevId;
  UINT64        LpcBaseAddress;

  if (mPchStepping != PCH_STEPPING_MAX) {
    return mPchStepping;
  }

  LpcBaseAddress = PCI_LIB_ADDRESS (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC,
                     0
                     );
  RevId = PciRead8 ((UINTN)(LpcBaseAddress + PCI_REVISION_ID_OFFSET));
  mPchStepping = RevId;

  return RevId;
}

/**
  Return Pch Series

  @retval PCH_SERIES            Pch Series
**/
PCH_SERIES
EFIAPI
GetPchSeries (
  VOID
  )
{
  UINT16      LpcDeviceId;
  PCH_SERIES  PchSer;

  if (mPchSeries != PCH_UNKNOWN_SERIES) {
    return mPchSeries;
  }

  LpcDeviceId = PchGetLpcDid ();
  if (LpcDeviceId == V_LPC_CFG_DID_EHL_LP) {
    PchSer = PCH_LP;
  } else {
    PchSer = PCH_UNKNOWN_SERIES;
    DEBUG ((DEBUG_ERROR, "Unsupported PCH SKU, LpcDeviceId: 0x%04x!\n", LpcDeviceId));
    ASSERT (FALSE);
  }
  mPchSeries = PchSer;

  return PchSer;
}

/**
  Check if this is PCH LP series

  @retval TRUE                It's PCH LP series
  @retval FALSE               It's not PCH LP series
**/
BOOLEAN
EFIAPI
IsPchLp (
  VOID
  )
{
  BOOLEAN IsPchLpSeries;

  if (mIsPchLp != 0xFF) {
    return (BOOLEAN) mIsPchLp;
  }

  IsPchLpSeries = (GetPchSeries () == PCH_LP);

  mIsPchLp = IsPchLpSeries;

  return IsPchLpSeries;
}

/**
  Check if a given Root Port is Multi VC

  @param[in] RpIndex (0-based)

  @return  TRUE if it is Multi VC
**/
BOOLEAN
IsRpMultiVC (
  IN  UINT32                   RpIndex
  )
{
  return ((RpIndex > 3) && (RpIndex < 7));
}

UINT8
EFIAPI
GetPchMaxPciePortNum (
  VOID
  )
{
    return PCH_LP_PCIE_MAX_ROOT_PORTS;
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

/**
  Get Pch Maximum Pcie Clock Number

  @retval Pch Maximum Pcie Clock Number
**/
UINT8
EFIAPI
GetPchMaxPcieClockNum (
  VOID
  )
{
  if (IsPchLp ()) {
    return 6;
  }
    return 0;
  }

/**
  Get Pch Usb Maximum Physical Port Number

  @retval Pch Usb Maximum Physical Port Number
**/
UINT8
EFIAPI
GetPchUsbMaxPhysicalPortNum (
  VOID
  )
{
  if (IsPchLp ()) {
    return PCH_LP_XHCI_MAX_USB2_PHYSICAL_PORTS;
  } else {
    return PCH_H_XHCI_MAX_USB2_PHYSICAL_PORTS;
  }
}

/**
  Get Pch Maximum Usb2 Port Number of XHCI Controller

  @retval Pch Maximum Usb2 Port Number of XHCI Controller
**/
UINT8
EFIAPI
GetPchXhciMaxUsb2PortNum (
  VOID
  )
{
  if (IsPchLp ()) {
    return PCH_LP_XHCI_MAX_USB2_PORTS;
  }
    return PCH_H_XHCI_MAX_USB2_PORTS;
  }

/**
  Get Pch Maximum Usb3 Port Number of XHCI Controller

  @retval Pch Maximum Usb3 Port Number of XHCI Controller
**/
UINT8
EFIAPI
GetPchXhciMaxUsb3PortNum (
  VOID
  )
{
  if (IsPchLp ()) {
    return PCH_LP_XHCI_MAX_USB3_PORTS;
  }
    return PCH_H_XHCI_MAX_USB3_PORTS;
  }

/**
  Get PCH SKU type

  @retval PCH_SKU_TYPE  Type of PCH SKU
**/
PCH_SKU_TYPE
GetPchSkuType (
  VOID
  )
{
  UINT16  LpcDeviceId;

  LpcDeviceId = PchGetLpcDid ();

  if (LpcDeviceId == V_LPC_CFG_DID_EHL_LP) {
    return PchMobileSku;
  }
    return PchUnknownSku;
  }

/**
  Get Pch Maximum Sata Controller Number

  @param[in] None

  @retval Pch Maximum Sata Controller Number
**/
UINT8
EFIAPI
GetPchMaxSataControllerNum (
  VOID
  )
{

  PCH_SERIES  PchSeries;

  PchSeries = GetPchSeries();
  switch (PchSeries) {
    case PCH_H:
    case PCH_LP:
      return 1;
    default:
      return 0;
  }
}

/**
  Get Pch Maximum Sata Port Number

  @param[in]  SataCtrlIndex       SATA controller index

  @retval Pch Maximum Sata Port Number
**/
UINT8
EFIAPI
GetPchMaxSataPortNum (
  IN UINT32      SataCtrlIndex
  )
{
  ASSERT (SataCtrlIndex < GetPchMaxSataControllerNum ());

  if (IsPchLp ()) {
    return 2;
  }
  ASSERT (FALSE);
  return 0;
}

/**
  Get Pch Maximum Serial IO controllers number

  @retval Pch Maximum Serial IO controllers number
**/
UINT8
EFIAPI
GetPchMaxSerialIoControllersNum (
  VOID
  )
{
  return PCH_SERIALIO_MAX_CONTROLLERS;
}

/**
  Get Pch Maximum Serial IO SPI controllers number

  @retval Pch Maximum Serial IO SPI controllers number
**/
UINT8
EFIAPI
GetPchMaxSerialIoSpiControllersNum (
  VOID
  )
{
  return PCH_SERIALIO_MAX_SPI_CONTROLLERS;
}

/**
  Get Pch Maximum Serial IO UART controllers number

  @retval Pch Maximum Serial IO UART controllers number
**/
UINT8
EFIAPI
GetPchMaxSerialIoUartControllersNum (
  VOID
  )
{
  return PCH_SERIALIO_MAX_UART_CONTROLLERS;
}

/**
  Get Pch Maximum ISH SPI Controller number

  @retval Pch Maximum ISH SPI controllers number
**/
UINT8
EFIAPI
GetPchMaxIshSpiControllersNum (
  VOID
  )
{
  return PCH_ISH_MAX_SPI_CONTROLLERS;
}

/**
  Get Pch Maximum ISH UART Controller number

  @retval Pch Maximum ISH UART controllers number
**/
UINT8
EFIAPI
GetPchMaxIshUartControllersNum (
  VOID
  )
{
  return PCH_ISH_MAX_UART_CONTROLLERS;
}

/**
  Get Pch Maximum ISH I2C Controller number

  @retval Pch Maximum ISH I2C controllers number
**/
UINT8
EFIAPI
GetPchMaxIshI2cControllersNum (
  VOID
  )
{
  return PCH_ISH_MAX_I2C_CONTROLLERS;
}

/**
  Get Pch Maximum ISH GP number

  @retval Pch Maximum ISH GP number
**/
UINT8
EFIAPI
GetPchMaxIshGpNum (
  VOID
  )
{
  return PCH_ISH_MAX_GP_PINS;
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
  Get Pch Maximum PSE I2S Controller number

  @retval Pch Maximum PSE I2S Controller number
**/
UINT8
GetPchMaxPseI2sControllersNum (
  VOID
)
{
  return 2;
}

/**
  Get Pch Maximum PSE UART Controller number

  @retval Pch Maximum PSE UART Controller number
**/
UINT8
GetPchMaxPseUartControllersNum (
  VOID
)
{
  return 6;
}


/**
  Get Pch Maximum PSE HSUART Controller number

  @retval Pch Maximum PSE HSUART Controller number
**/
UINT8
GetPchMaxPseHsuartControllersNum (
  VOID
)
{
  return 4;
}

/**
  Get Pch Maximum PSE QEP Controller number

  @retval Pch Maximum PSE QEP Controller number
**/
UINT8
GetPchMaxPseQepControllersNum (
  VOID
)
{
  return 4;
}

/**
  Get Pch Maximum PSE I2C Controller number

  @retval Pch Maximum PSE I2C Controller number
**/
UINT8
GetPchMaxPseI2cControllersNum (
  VOID
)
{
  return 8;
}

/**
  Get Pch Maximum PSE SPI Controller number

  @retval Pch Maximum PSE SPI Controller number
**/
UINT8
GetPchMaxPseSpiControllersNum (
  VOID
)
{
  return 4;
}

/**
  Get Pch Maximum PSE DMA Controller number

  @retval Pch Maximum PSE DMA Controller number
**/
UINT8
GetPchMaxPseDmaControllersNum (
  VOID
)
{
  return 3;
}


/**
  Get Pch Maximum PSE GBE Controller number

  @retval Pch Maximum PSE GBE Controller number
**/
UINT8
GetPchMaxPseGbeControllersNum (
  VOID
)
{
  return 2;
}

/**
  Get Pch Maximum PSE CAN Controller number

  @retval Pch Maximum PSE CAN Controller number
**/
UINT8
GetPchMaxPseCanControllersNum (
  VOID
)
{
  return 2;
}


/**
  Get Pch Maximum PSE Timed GPIO Controller number

  @retval Pch Maximum PSE Timed GPIO Controller number
**/
UINT8
GetPchMaxPseTimedGpioControllersNum (
  VOID
)
{
  return 2;
}
/**
  Get Pch Maximum PSE PWM number

  @retval Pch Maximum PSE PWM number
**/
UINT8
GetPchMaxPsePWMNum (
  VOID
)
{
  return 16;
}
