/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PchInfoLib.h>
#include <PchAccess.h>
#include <Register/PchBdfAssignment.h>
#include <Library/PchPciBdfLib.h>

/**
  Check if a Device is present for PCH FRU
  If the data is defined for PCH RFU return it
  If the data is not defined (Device is NOT present) assert.

  @param[in]  DataToCheck       Device or Function number to check

  @retval Device or Function number value if defined for PCH FRU
          0xFF if not present in PCH FRU
**/
UINT8
CheckAndReturn (
  UINT8 DataToCheck
  )
{
  if (DataToCheck == NOT_PRESENT) {
    ASSERT (FALSE);
  }
  return DataToCheck;
}

/**
  Get LPC controller PCIe Device Number

  @retval LPC controller PCIe Device Number
**/
UINT8
LpcDevNumber (
  VOID
  )
{
  return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_LPC);
}

/**
  Get LPC controller PCIe Function Number

  @retval LPC controller PCIe Function Number
**/
UINT8
LpcFuncNumber (
  VOID
  )
{
  return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_LPC);
}

/**
  Returns PCH LPC device PCI base address.

  @retval                   PCH LPC PCI base address.
**/
UINT64
LpcPciCfgBase (
  VOID
  )
{
  return PCI_SEGMENT_LIB_ADDRESS (
           DEFAULT_PCI_SEGMENT_NUMBER_PCH,
           DEFAULT_PCI_BUS_NUMBER_PCH,
           LpcDevNumber (),
           LpcFuncNumber (),
           0
           );
}

/**
  Get Serial IO I2C controller PCIe Device Number

  @param[in]  I2cNumber       Serial IO I2C controller index

  @retval Serial IO I2C controller PCIe Device Number
**/
UINT8
SerialIoI2cDevNumber (
  IN UINT8       I2cNumber
  )
{
  if (GetPchMaxSerialIoI2cControllersNum () <= I2cNumber) {
    ASSERT (FALSE);
    return 0xFF;
  }
  switch (I2cNumber) {
    case 0:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C0);
    case 1:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C1);
    case 2:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C2);
    case 3:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C3);
    case 4:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C4);
    case 5:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C5);
    case 6:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C6);
    case 7:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C7);
    default:
      ASSERT (FALSE);
      return 0xFF;
  }
}

/**
  Get Serial IO I2C controller PCIe Function Number

  @param[in]  I2cNumber       Serial IO I2C controller index

  @retval Serial IO I2C controller PCIe Function Number
**/
UINT8
SerialIoI2cFuncNumber (
  IN UINT8       I2cNumber
  )
{
  if (GetPchMaxSerialIoI2cControllersNum () <= I2cNumber) {
    ASSERT (FALSE);
    return 0xFF;
  }
  switch (I2cNumber) {
    case 0:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C0);
    case 1:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C1);
    case 2:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C2);
    case 3:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C3);
    case 4:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C4);
    case 5:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C5);
    case 6:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C6);
    case 7:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C7);
    default:
      ASSERT (FALSE);
      return 0xFF;
  }
}

/**
  Get Serial IO I2C controller address that can be passed to the PCI Segment Library functions.

  @param[in]  I2cNumber       Serial IO I2C controller index

  @retval Serial IO I2C controller address in PCI Segment Library representation
**/
UINT64
SerialIoI2cPciCfgBase (
  IN UINT8        I2cNumber
  )
{
  return PCI_SEGMENT_LIB_ADDRESS (
           DEFAULT_PCI_SEGMENT_NUMBER_PCH,
           DEFAULT_PCI_BUS_NUMBER_PCH,
           SerialIoI2cDevNumber (I2cNumber),
           SerialIoI2cFuncNumber (I2cNumber),
           0
           );
}

/**
  Get Serial IO UART controller PCIe Device Number

  @param[in]  UartNumber       Serial IO UART controller index

  @retval Serial IO UART controller PCIe Device Number
**/
UINT8
SerialIoUartDevNumber (
  IN UINT8       UartNumber
  )
{
  if (GetPchMaxSerialIoUartControllersNum() <= UartNumber) {
    ASSERT (FALSE);
    return 0xFF;
  }
  switch (UartNumber) {
    case 0:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART0);
    case 1:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART1);
    case 2:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART2);
    case 3:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART3);
    case 4:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART4);
    case 5:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART5);
    case 6:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART6);
    default:
      ASSERT (FALSE);
      return 0xFF;
  }
}

/**
  Get Serial IO UART controller PCIe Function Number

  @param[in]  UartNumber       Serial IO UART controller index

  @retval Serial IO UART controller PCIe Function Number
**/
UINT8
SerialIoUartFuncNumber (
  IN UINT8       UartNumber
  )
{
  if (GetPchMaxSerialIoUartControllersNum() <= UartNumber) {
    ASSERT (FALSE);
    return 0xFF;
  }
  switch (UartNumber) {
    case 0:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART0);
    case 1:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART1);
    case 2:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART2);
    case 3:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART3);
    case 4:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART4);
    case 5:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART5);
    case 6:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART6);
    default:
      ASSERT (FALSE);
      return 0xFF;
  }
}

/**
  Get Serial IO UART controller address that can be passed to the PCI Segment Library functions.

  @param[in]  UartNumber       Serial IO UART controller index

  @retval Serial IO UART controller address in PCI Segment Library representation
**/
UINT64
SerialIoUartPciCfgBase (
  IN UINT8        UartNumber
  )
{
  return PCI_SEGMENT_LIB_ADDRESS (
           DEFAULT_PCI_SEGMENT_NUMBER_PCH,
           DEFAULT_PCI_BUS_NUMBER_PCH,
           SerialIoUartDevNumber (UartNumber),
           SerialIoUartFuncNumber (UartNumber),
           0
           );
}

/**
  Get PCH PCIe controller PCIe Device Number

  @param[in]  RpIndex       Root port physical number. (0-based)

  @retval PCH PCIe controller PCIe Device Number
**/
UINT8
PchPcieRpDevNumber (
  IN  UINTN   RpIndex
  )
{
  switch (RpIndex) {
    case 0:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_1);
    case 1:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_2);
    case 2:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_3);
    case 3:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_4);
    case 4:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_5);
    case 5:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_6);
    case 6:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_7);
    case 7:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_8);
    case 8:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_9);
    case 9:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_10);
    case 10:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_11);
    case 11:
      return CheckAndReturn (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_12);

    default:
      ASSERT (FALSE);
      return 0xFF;
  }
}

/**
  Get PCH PCIe controller PCIe Function Number

  @param[in]  RpIndex       Root port physical number. (0-based)

  @retval PCH PCIe controller PCIe Function Number
**/
UINT8
PchPcieRpFuncNumber (
  IN  UINTN   RpIndex
  )
{
  switch (RpIndex) {
    case 0:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5);
    case 1:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2);
    case 2:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3);
    case 3:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4);
    case 4:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1);
    case 5:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6);
    case 6:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7);
    case 7:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8);
    case 8:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_9);
    case 9:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_10);
    case 10:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_11);
    case 11:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_12);
    case 12:
      return CheckAndReturn (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_13);

    default:
      ASSERT (FALSE);
      return 0xFF;
  }
}

/**
  Get PCH PCIe controller address that can be passed to the PCI Segment Library functions.

  @param[in]  RpIndex       PCH PCIe Root Port physical number. (0-based)

  @retval PCH PCIe controller address in PCI Segment Library representation
**/
UINT64
PchPcieRpPciCfgBase (
  IN  UINT32   RpIndex
  )
{
  return PCI_SEGMENT_LIB_ADDRESS (
          DEFAULT_PCI_SEGMENT_NUMBER_PCH,
          DEFAULT_PCI_BUS_NUMBER_PCH,
          PchPcieRpDevNumber (RpIndex),
          PchPcieRpFuncNumber (RpIndex),
          0
          );
}
