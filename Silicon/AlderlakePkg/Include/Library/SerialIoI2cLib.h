/** @file

  Copyright (c) 2020 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SERIAL_IO_I2C_LIB_H_
#define _SERIAL_IO_I2C_LIB_H_
#define I2C_BUS_FREQUENCY_400Khz  400      //400 Khz
#define I2C_BUS_TIMEOUT_USEC      1000000  //1 sec

#include <Uefi/UefiBaseType.h>

typedef enum {
  I2cBlockCommandWait  = 0,
  I2cBlockCommandRead  = 1,
  I2cBlockCommandWrite = 2
} SERIAL_IO_I2C_COMMAND;

typedef struct {
  UINT8  CmdId;
  UINT8  *CommandPointer;
  UINT32 CommandData;
} SERIALIO_I2C_BLOCK_COMMAND;

typedef struct {
  UINT16  SclHcnt;
  UINT16  SclLcnt;
  UINT16  SdaRxHold;
  UINT16  SdaTxHold;
} SERIAL_IO_I2C_TIMING;

/**
  Execute an I2C write

  Addressing mode will defined based upon SlaveAddres and mask:
  SlaveAdress & 0x80000000 - if true will result in 10bit addressing
  Refer to: I2C_ADDRESSING_10_BIT

  @param[in]   PciCfgBase     Pci Config Base Address of specified I2C controller
                              @Note: If I2C Number is provided this parameter is ignored and can be left as 0
  @param[in]   I2cNumber      Optional: I2C controller applicable on bus 0 only
  @param[in]   SlaveAddress   Address of the end point device
  @param[in]   WriteLength    Write length of the current transaction
  @param[in]   WriteBuffer    Pointer to data in write buffer
  @param[in]   TimeBudget     Time allotted to verify if I2C is enabled
  @param[in]   Frequency      Bus clock frequency (in kHz)
  @param[in]   TimingConfig   Optional: Pointer to struct passed down to override SDA Hold and SCL LCNT/HCNT settings
  @param[in]   Stop           Issues STOP on the last byte
  @param[in]   Restart        Indication if restart is needed, if TRUE will Initialize I2C controller.
                              @Note: Restart is required in first transaction.

  @retval   EFI_INVALID_PARAMETER   Invalid length / buffer parameters were passed into this function
  @retval   EFI_TIMEOUT             Timeout occurred in I2C disable/enable or wrong ENST value
  @retval   EFI_DEVICE_ERROR        The verification failed
  @retval   EFI_SUCCESS             Write was successful
**/
EFI_STATUS
EFIAPI
SerialIoI2cWrite (
  IN UINTN                 PciCfgBase,
  IN UINT8                 *I2cNumber  OPTIONAL,
  IN UINT32                SlaveAddress,
  IN UINT32                WriteLength,
  IN UINT8                 *WriteBuffer,
  IN UINT64                TimeBudget,
  IN UINTN                 Frequency,
  IN SERIAL_IO_I2C_TIMING  *TimingConfig  OPTIONAL,
  IN BOOLEAN               Stop,
  IN BOOLEAN               Restart
  );

/**
  Execute an I2C read command

  Addressing mode will defined based upon SlaveAddress and mask:
  SlaveAdress & 0x80000000 - if true will result in 10bit addressing
  Refer to: I2C_ADDRESSING_10_BIT

  @param[in]   PciCfgBase     Pci Config Base Address of specified I2C controller
                              @Note: If I2C Number is provided this parameter is ignored and can be left as 0
  @param[in]   I2cNumber      Optional: I2C controller applicable on bus 0 only
  @param[in]   SlaveAddress  -Address of the end point device
  @param[in]   ReadLength     Read length of the current transaction
  @param[in]   ReadBuffer     Pointer to data in read buffer
  @param[in]   TimeBudget     Time allotted to verify if I2C is enabled
  @param[in]   Frequency      Bus clock frequency (in kHz)
  @param[in]   TimingConfig   Optional: Pointer to struct passed down to override SDA Hold and SCL LCNT/HCNT settings
  @param[in]   Restart        Indication if restart is needed, if TRUE will Initialize I2C controller.
                              @Note: Restart is required in first transaction.

  @retval   EFI_INVALID_PARAMETER   Invalid length / buffer parameters were passed into this function
  @retval   EFI_TIMEOUT             Timeout occurred in I2C disable/enable or wrong ENST value
  @retval   EFI_DEVICE_ERROR        The verification failed
  @retval   EFI_SUCCESS             Read was successful
**/
EFI_STATUS
EFIAPI
SerialIoI2cRead (
  IN UINTN                 PciCfgBase,
  IN UINT8                 *I2cNumber  OPTIONAL,
  IN UINT32                SlaveAddress,
  IN UINT32                ReadLength,
  IN UINT8                 *ReadBuffer,
  IN UINT64                TimeBudget,
  IN UINTN                 Frequency,
  IN SERIAL_IO_I2C_TIMING  *TimingConfig  OPTIONAL,
  IN BOOLEAN               Restart
  );

/**
  Executes a block of I2C read/write and wait commands. This is intended to enable or configure devices that require multiple data transactions through a I2C bus.
  The command structure consists of entries with a command id, input/output data pointer and data count.

  The following is an example of a command block:
  SERIALIO_I2C_BLOCK_COMMAND SampleCommandBlock[] = {
    { I2cBlockCommandWrite, &gWriteDataBuffer[0], sizeof (gWriteDataBuffer)},
    { I2cBlockCommandRead, &gReadDataBuffer[0], 1000},
    { I2cBlockCommandWait, NULL, 1000},
    { I2cBlockCommandWrite, &gWriteDataBuffer[100], 100}
  };

  Status = SerialIoI2cBlockWriteRead (
            PciCfgBase,
            &I2cNumber,
            0x10,
            SampleCommandBlock,
            sizeof(SampleCommandBlock)/sizeof(SERIALIO_I2C_BLOCK_COMMAND),
            Frequency,
            WAIT_1_SECOND,
            TRUE,
            &TimingConfig
            );

  @param[in]   PciCfgBase     Pci Config Base Address of specified I2C controller
                              @Note: If I2C Number is provided this parameter is ignored and can be left as 0
  @param[in]   I2cNumber                    Optional: The serial controller to be used
  @param[in]   SlaveAddress                 The address for the device
  @param[in]   I2cCommandBlock              Pointer to SERIALIO_I2C_BLOCK_COMMAND structure with all commands and data
  @param[in]   NumofBlocks                  Total amount of the command block entries - sizeof(SampleCommandBlock)/sizeof(SERIALIO_I2C_BLOCK_COMMAND)
  @param[in]   Frequency                    Bus clock frequency (in kHz)
  @param[in]   MaxTransactionTimeBudget     Maximum Time budget for a single transaction
  @param[in]   DisableI2cAfterTransaction   If TRUE, I2c is disabled after transaction is finished.
  @param[in]   TimingConfig                 Optional: Pointer to struct passed down to override SDA Hold and SCL LCNT/HCNT settings

  @retval   EFI_INVALID_PARAMETER   Signals that a command or data count was invalid
  @retval   EFI_BUFFER_TOO_SMALL    The last command could not be executed since the buffer ended before reading all parameters
  @retval   EFI_SUCCESS             The command were executed without incident
**/
EFI_STATUS
EFIAPI
SerialIoI2cBlockWriteRead (
  IN UINTN                       PciCfgBase,
  IN UINT8                       *I2cNumber  OPTIONAL,
  IN UINT32                      SlaveAddress,
  IN SERIALIO_I2C_BLOCK_COMMAND  *I2cCommandBlock,
  IN UINT32                      BlockSize,
  IN UINTN                       Frequency,
  IN UINT64                      MaxTransactionTimeBudget,
  IN BOOLEAN                     DisableI2cAfterTransaction,
  IN SERIAL_IO_I2C_TIMING        *TimingConfig  OPTIONAL
  );

#endif //_SERIAL_IO_I2C_LIB_H_