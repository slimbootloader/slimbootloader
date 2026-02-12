/** @file

  Copyright (c) 2020 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/PchPciBdfLib.h>
#include <Library/SerialIoI2cLib.h>
#include <Library/BaseSerialIoI2cLib/SerialIoI2cPrivateLib.h>

#define DISPLAY_TEST_REGISTER                 0x07
#define DISPLAY_TEST_NORMAL_OPERATION         0x00

#define CONFIGURATION_REGISTER                0x04
#define CONFIGURATION_NORMAL_OPERATION        0x01

#define SCAN_LIMIT_REGISTER                   0x03
#define SCAN_LIMIT_DIGITS_OPERATION           0x03

#define INTENSITY_REGISTER                    0x02
#define INTENSITY_MAX_ON_OPERATION            0x3E

#define DECODE_MODE_REGISTER                  0x01
#define DECODE_MODE_DIGITS_OPERATION          0xFF


/**
  This function does the basic init work for Max6950.

  @param[in]  SpiBaseAddress - Base Address of SPI Controller
**/
EFI_STATUS
EFIAPI
MaxLedInit (
  VOID
  )
{
  UINT8  WriBuf[2] = { 0, 0};
  UINT8  I2cNumber = PcdGet8(PcdI2cIoExpanderPortNumber);
  UINT8  SlaveAddress = PcdGet8(PcdI2cMaxLedSlaveAddress);
  UINTN  PciCfgBase;
  EFI_STATUS Status;

  DEBUG ((DEBUG_INFO, "MaxLedInit Start \n"));

  // I2C Post Code
  PciCfgBase = (UINTN) SerialIoI2cPciCfgBase (I2cNumber);

  // Set the display_test register fromat command 0x07 to indicate normal operation
  WriBuf [0] = DISPLAY_TEST_REGISTER;
  WriBuf [1] = DISPLAY_TEST_NORMAL_OPERATION;

  Status = SerialIoI2cWrite (
             PciCfgBase,
             &I2cNumber,
             SlaveAddress,
             2,
             WriBuf,
             I2C_BUS_TIMEOUT_USEC,
             I2C_BUS_FREQUENCY_400Khz,
             NULL,
             TRUE,
             TRUE
             );
  //
  // Set config regsiter format command 0x04 to indicate normal operation, slow
  // blink rate, global blinking function is disabled, global blink timing synch
  // which blink timing counters are unaffected, global clear digit data which
  // cause both plane P0 and P1 are unaffected.
  //
  WriBuf [0] = CONFIGURATION_REGISTER;
  WriBuf [1] = CONFIGURATION_NORMAL_OPERATION;

  Status = SerialIoI2cWrite (
             PciCfgBase,
             &I2cNumber,
             SlaveAddress,
             2,
             WriBuf,
             I2C_BUS_TIMEOUT_USEC,
             I2C_BUS_FREQUENCY_400Khz,
             NULL,
             TRUE,
             TRUE
             );

  //
  // Set the scan limit register format command 0x03 to indicate display for digits 3-0
  //
  WriBuf [0] = SCAN_LIMIT_REGISTER;
  WriBuf [1] = SCAN_LIMIT_DIGITS_OPERATION;

  Status = SerialIoI2cWrite (
             PciCfgBase,
             &I2cNumber,
             SlaveAddress,
             2,
             WriBuf,
             I2C_BUS_TIMEOUT_USEC,
             I2C_BUS_FREQUENCY_400Khz,
             NULL,
             TRUE,
             TRUE
             );

  //
  // Set the intensity register format command 0x02 to indicate duty cycle of 63/64 (max on)
  //
  WriBuf [0] = INTENSITY_REGISTER;
  WriBuf [1] = INTENSITY_MAX_ON_OPERATION;

  Status = SerialIoI2cWrite (
             PciCfgBase,
             &I2cNumber,
             SlaveAddress,
             2,
             WriBuf,
             I2C_BUS_TIMEOUT_USEC,
             I2C_BUS_FREQUENCY_400Khz,
             NULL,
             TRUE,
             TRUE
             );

  //
  // Set the decode mode register format command 0x01 to indicate hex decode for digit 7-0
  //
  WriBuf [0] = DECODE_MODE_REGISTER;
  WriBuf [1] = DECODE_MODE_DIGITS_OPERATION;

  Status = SerialIoI2cWrite (
             PciCfgBase,
             &I2cNumber,
             SlaveAddress,
             2,
             WriBuf,
             I2C_BUS_TIMEOUT_USEC,
             I2C_BUS_FREQUENCY_400Khz,
             NULL,
             TRUE,
             TRUE
             );

  DEBUG ((DEBUG_INFO, "MaxLedInit End 0x%x \n",Status));
  return EFI_SUCCESS;
}

/**
  Send 4 digit PostCode commands to Max6950 device

  @param[in]  PostCodeValue - 4 digit PostCode value to be displayed
**/
EFI_STATUS
EFIAPI
DisplayPostCode (
  IN UINT32 PostCodeValue
  )
{
  UINT16 SingleDigit;
  UINT8  Index;
  UINT8  WriBuf [2] = {0, 0};
  UINT8  I2cNumber = PcdGet8(PcdI2cIoExpanderPortNumber);
  UINT8  SlaveAddress = PcdGet8(PcdI2cMaxLedSlaveAddress);
  UINTN  PciCfgBase;
  EFI_STATUS Status;

  // I2C Post Code
  PciCfgBase = (UINTN) SerialIoI2cPciCfgBase (I2cNumber);

  //
  // Write all the digits.
  //
  for (Index = 0; Index < 4; Index++) {
    SingleDigit = (PostCodeValue >> (4 * Index)) & 0xF;

    // Digit 0 register address 0x20
    WriBuf [0] = 0x20 + Index;
    WriBuf [1] = (UINT8)SingleDigit;

    Status = SerialIoI2cWrite (
               PciCfgBase,
               &I2cNumber,
               SlaveAddress,
               2,
               WriBuf,
               1000000,
               400,
               NULL,
               TRUE,
               TRUE
               );
  }

  // Disable the Controller, set to D3 and disable MSE
  PciCfgBase = (UINTN) SerialIoI2cPciCfgBase (I2cNumber);
  SerialIoI2cPciDisable (PciCfgBase);

  return Status;
}
