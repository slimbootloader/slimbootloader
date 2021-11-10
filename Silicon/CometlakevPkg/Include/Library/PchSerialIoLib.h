/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCH_SERIAL_IO_LIB_H__
#define __PCH_SERIAL_IO_LIB_H__

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <RegAccess.h>

typedef enum {
  PchSerialIoIndexI2C0,
  PchSerialIoIndexI2C1,
  PchSerialIoIndexI2C2,
  PchSerialIoIndexI2C3,
  PchSerialIoIndexI2C4,
  PchSerialIoIndexI2C5,
  PchSerialIoIndexSpi0,
  PchSerialIoIndexSpi1,
  PchSerialIoIndexSpi2,
  PchSerialIoIndexUart0,
  PchSerialIoIndexUart1,
  PchSerialIoIndexUart2,
  PchSerialIoIndexMax
} PCH_SERIAL_IO_CONTROLLER;

typedef struct {
  UINT8  DevNum;
  UINT8  FuncNum;
} SERIAL_IO_BDF_NUMBERS;

typedef struct {
  UINT32 Bar0;
  UINT32 Bar1;
} SERIAL_IO_CONTROLLER_DESCRIPTOR;

/**
  Finds PCI Device Number of SerialIo devices.

  @param[in] SerialIoNumber             Serial IO device

  @retval                               SerialIo device number
**/
UINT8
GetSerialIoDeviceNumber (
  IN PCH_SERIAL_IO_CONTROLLER  SerialIoNumber
  );

/**
  Finds PCI Function Number of SerialIo devices.

  @param[in] SerialIoNumber             Serial IO device

  @retval                               SerialIo funciton number
**/
UINT8
GetSerialIoFunctionNumber (
  IN PCH_SERIAL_IO_CONTROLLER  SerialIoNumber
  );

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
  );

#endif /* __PCH_SERIAL_IO_LIB_H__ */
