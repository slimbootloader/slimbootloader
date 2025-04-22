/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/PchSerialIoLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>

CONST SERIAL_IO_BDF_NUMBERS mSerialIoBdf [PCH_MAX_SERIALIO_CONTROLLERS] =
{
  {PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C0,  PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C0},
  {PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C1,  PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C1},
  {PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C2,  PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C2},
  {PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C3,  PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C3},
  {PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C4,  PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C4},
  {PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C5,  PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C5},
  {PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI0,  PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI0},
  {PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI1,  PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI1},
  {PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI2,  PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI2},
  {PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART0, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART0},
  {PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART1, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART1},
  {PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART2, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART2}
};

CONST SERIAL_IO_CONTROLLER_DESCRIPTOR mSerialIoAcpiAddress [PCH_MAX_SERIALIO_CONTROLLERS] =
{
  {PCH_SERIAL_IO_BASE_ADDRESS + 0x0000,  PCH_SERIAL_IO_BASE_ADDRESS + 0x1000},
  {PCH_SERIAL_IO_BASE_ADDRESS + 0x2000,  PCH_SERIAL_IO_BASE_ADDRESS + 0x3000},
  {PCH_SERIAL_IO_BASE_ADDRESS + 0x4000,  PCH_SERIAL_IO_BASE_ADDRESS + 0x5000},
  {PCH_SERIAL_IO_BASE_ADDRESS + 0x6000,  PCH_SERIAL_IO_BASE_ADDRESS + 0x7000},
  {PCH_SERIAL_IO_BASE_ADDRESS + 0x8000,  PCH_SERIAL_IO_BASE_ADDRESS + 0x9000},
  {PCH_SERIAL_IO_BASE_ADDRESS + 0xA000,  PCH_SERIAL_IO_BASE_ADDRESS + 0xB000},
  {PCH_SERIAL_IO_BASE_ADDRESS + 0xC000,  PCH_SERIAL_IO_BASE_ADDRESS + 0xD000},
  {PCH_SERIAL_IO_BASE_ADDRESS + 0xE000,  PCH_SERIAL_IO_BASE_ADDRESS + 0xF000},
  {PCH_SERIAL_IO_BASE_ADDRESS + 0x10000, PCH_SERIAL_IO_BASE_ADDRESS + 0x11000},
  {PCH_SERIAL_IO_BASE_ADDRESS + 0x12000, PCH_SERIAL_IO_BASE_ADDRESS + 0x13000},
  {PCH_SERIAL_IO_BASE_ADDRESS + 0x14000, PCH_SERIAL_IO_BASE_ADDRESS + 0x15000},
  {PCH_SERIAL_IO_BASE_ADDRESS + 0x16000, PCH_SERIAL_IO_BASE_ADDRESS + 0x17000}
};

/**
  Finds PCI Device Number of SerialIo devices.

  @param[in] SerialIoNumber             Serial IO device

  @retval                               SerialIo device number
**/
UINT8
GetSerialIoDeviceNumber (
  IN PCH_SERIAL_IO_CONTROLLER  SerialIoNumber
  )
{
  return mSerialIoBdf[SerialIoNumber].DevNum;
}

/**
  Finds PCI Function Number of SerialIo devices.

  @param[in] SerialIoNumber             Serial IO device

  @retval                               SerialIo funciton number
**/
UINT8
GetSerialIoFunctionNumber (
  IN PCH_SERIAL_IO_CONTROLLER  SerialIoNumber
  )
{
  return mSerialIoBdf[SerialIoNumber].FuncNum;
}

/**
  Finds BAR values of SerialIo devices.
  SerialIo devices can be configured to not appear on PCI so traditional method of reading BAR might not work.
  If the SerialIo device is in PCI mode, a request for BAR1 will return its PCI CFG space instead

  @param[in] SerialIoDevice             Serial IO device
  @param[in] BarNumber                  0=BAR0, 1=BAR1

  @retval                               SerialIo Bar value
**/
UINTN
FindSerialIoBar (
  IN PCH_SERIAL_IO_CONTROLLER           SerialIoDevice,
  IN UINT8                              BarNumber
  )
{
  UINT64   Bar;
  UINTN    SerialIoBaseAddress;
  UINT16   VenId;
  UINT32   Device;
  UINT32   Function;

  Device   = GetSerialIoDeviceNumber (SerialIoDevice);
  Function = GetSerialIoFunctionNumber (SerialIoDevice);

  SerialIoBaseAddress = PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, Device, Function, 0);
  VenId = PciRead16 (SerialIoBaseAddress + PCI_VENDOR_ID_OFFSET) & 0xFFFF;
  if (VenId == V_PCH_INTEL_VENDOR_ID) {
    if (BarNumber == 1) {
      return ((UINTN) PcdGet64(PcdPciExpressBaseAddress) + SerialIoBaseAddress);
    }
    Bar = PciRead32 (SerialIoBaseAddress + PCI_BASE_ADDRESSREG_OFFSET);
    // For 64-Bit Memory Space BARs ((BAR[x] & 0xFFFFFFF0) + ((BAR[x+1] & 0xFFFFFFFF) << 32)
    if ((Bar & B_PCI_BAR_MEMORY_TYPE_MASK) == B_PCI_BAR_MEMORY_TYPE_64) {
      Bar = (Bar & 0xFFFFF000) + (UINTN) ((UINT64) LShiftU64 ((PciRead32 (SerialIoBaseAddress + PCI_BASE_ADDRESSREG_OFFSET + 4) & 0xFFFFFFFF), 32));
      return (UINTN) Bar;
    }
    return (UINTN) (Bar & 0xFFFFF000);
  }
  if (BarNumber == 0) {
    Bar = mSerialIoAcpiAddress[SerialIoDevice].Bar0;
  } else {
    Bar = mSerialIoAcpiAddress[SerialIoDevice].Bar1;
  }
  return (UINTN) Bar;
}
