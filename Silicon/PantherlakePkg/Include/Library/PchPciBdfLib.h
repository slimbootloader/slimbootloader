/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_PCI_BDF_LIB_H_
#define _PCH_PCI_BDF_LIB_H_

#define PCI_SEGMENT_LIB_ADDRESS(Segment,Bus,Device,Function,Register) \
  ((Segment != 0) ? \
    ( ((Register) & 0xfff)                 | \
      (((Function) & 0x07) << 12)          | \
      (((Device) & 0x1f) << 15)            | \
      (((Bus) & 0xff) << 20)               | \
      (LShiftU64 ((Segment) & 0xffff, 32))   \
    ) :                                      \
    ( ((Register) & 0xfff)                 | \
      (((Function) & 0x07) << 12)          | \
      (((Device) & 0x1f) << 15)            | \
      (((Bus) & 0xff) << 20)                 \
    )                                        \
  )

/**
  Returns PCH LPC device PCI base address.

  @retval                   PCH LPC PCI base address.
**/
UINT64
LpcPciCfgBase (
  VOID
  );

/**
  Get LPC controller PCIe Device Number

  @retval LPC controller PCIe Device Number
**/
UINT8
LpcDevNumber (
  VOID
  );

/**
  Get LPC controller PCIe Function Number

  @retval LPC controller PCIe Function Number
**/
UINT8
LpcFuncNumber (
  VOID
  );

/**
  Get Serial IO I2C controller PCIe Device Number

  @param[in]  I2cNumber       Serial IO I2C controller index

  @retval Serial IO I2C controller PCIe Device Number
**/
UINT8
SerialIoI2cDevNumber (
  IN UINT8       I2cNumber
  );

/**
  Get Serial IO I2C controller PCIe Function Number

  @param[in]  I2cNumber       Serial IO I2C controller index

  @retval Serial IO I2C controller PCIe Function Number
**/
UINT8
SerialIoI2cFuncNumber (
  IN UINT8       I2cNumber
  );

/**
  Get Serial IO I2C controller address that can be passed to the PCI Segment Library functions.

  @param[in]  I2cNumber       Serial IO I2C controller index

  @retval Serial IO I2C controller address in PCI Segment Library representation
**/
UINT64
SerialIoI2cPciCfgBase (
  IN UINT8        I2cNumber
  );

/**
  Get Serial IO I3C controller PCIe Device Number

  @param[in]  I3CNumber       Serial IO I3C controller index

  @retval Serial IO I3C controller PCIe Device Number
**/
UINT8
SerialIoI3cDevNumber (
  IN UINT8       I3cNumber
  );

/**
  Get Serial IO I3C controller PCIe Function Number

  @param[in]  I3cNumber       Serial IO I3C controller index

  @retval Serial IO I3C controller PCIe Function Number
**/
UINT8
SerialIoI3cFuncNumber (
  IN UINT8       I3cNumber
  );

/**
  Get Serial IO I3C controller address that can be passed to the PCI Segment Library functions.

  @param[in]  I3cNumber       Serial IO I3C controller index

  @retval Serial IO I3C controller address in PCI Segment Library representation
**/
UINT64
SerialIoI3cPciCfgBase (
  IN UINT8        I3cNumber
  );

/**
  Get Serial IO UART controller PCIe Device Number

  @param[in]  UartNumber       Serial IO UART controller index

  @retval Serial IO UART controller PCIe Device Number
**/
UINT8
SerialIoUartDevNumber (
  IN UINT8       UartNumber
  );

/**
  Get Serial IO UART controller PCIe Function Number

  @param[in]  UartNumber       Serial IO UART controller index

  @retval Serial IO UART controller PCIe Function Number
**/
UINT8
SerialIoUartFuncNumber (
  IN UINT8       UartNumber
  );

/**
  Get Serial IO UART controller address that can be passed to the PCI Segment Library functions.

  @param[in]  UartNumber       Serial IO UART controller index

  @retval Serial IO UART controller address in PCI Segment Library representation
**/
UINT64
SerialIoUartPciCfgBase (
  IN UINT8        UartNumber
  );

/**
  Get PCH PCIe controller PCIe Device Number

  @param[in]  RpIndex       Root port physical number. (0-based)

  @retval PCH PCIe controller PCIe Device Number
**/
UINT8
PchPcieRpDevNumber (
  IN  UINTN   RpIndex
  );

/**
  Get PCH PCIe controller PCIe Function Number

  @param[in]  RpIndex       Root port physical number. (0-based)

  @retval PCH PCIe controller PCIe Function Number
**/
UINT8
PchPcieRpFuncNumber (
  IN  UINTN   RpIndex
  );

/**
  Get PCH PCIe controller address that can be passed to the PCI Segment Library functions.

  @param[in]  RpIndex       PCH PCIe Root Port physical number. (0-based)

  @retval PCH PCIe controller address in PCI Segment Library representation
**/
UINT64
PchPcieRpPciCfgBase (
  IN  UINT32   RpIndex
  );

#endif //_PCH_PCI_BDF_LIB_H_
