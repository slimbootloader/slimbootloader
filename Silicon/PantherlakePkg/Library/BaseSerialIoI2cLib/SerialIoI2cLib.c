/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/SerialIoI2cLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPciBdfLib.h>
#include "SerialIoI2cPrivateLib.h"

/**
  Execute an I2C write

  Addressing mode will defined based upon TargetAddress and mask:
  TargetAddress & 0x80000000 - if true will result in 10bit addressing
  Refer to: I2C_ADDRESSING_10_BIT

  @param[in]   PciCfgBase     Pci Config Base Address of specified I2C controller
                              @Note: If I2C Number is provided this parameter is ignored and can be left as 0
  @param[in]   I2cNumber      Optional: I2C controller applicable on bus 0 only
  @param[in]   TargetAddress  Address of the end point device
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
  IN UINT32                TargetAddress,
  IN UINT32                WriteLength,
  IN UINT8                 *WriteBuffer,
  IN UINT64                TimeBudget,
  IN UINTN                 Frequency,
  IN SERIAL_IO_I2C_TIMING  *TimingConfig  OPTIONAL,
  IN BOOLEAN               Stop,
  IN BOOLEAN               Restart
  )
{
  EFI_STATUS  Status;
  UINT64      Timeout;
  UINTN       MmioBase;
  UINT64      MmioBase64;

  if (WriteLength == 0 ||(WriteLength != 0 && WriteBuffer == NULL)) {
    DEBUG ((DEBUG_ERROR, "SerialIoI2cWrite Invalid Parameters\n"));
    return EFI_INVALID_PARAMETER;
  }

  MmioBase64 = 0x0;

  //
  // If I2cNumber is provided, override PciCfgBase
  //
  if (I2cNumber) {
    if (*I2cNumber >= GetPchMaxSerialIoI2cControllersNum ()) {
      return EFI_INVALID_PARAMETER;
    }
    PciCfgBase = (UINTN) SerialIoI2cPciCfgBase (*I2cNumber);
  }

  DEBUG ((DEBUG_VERBOSE, "SerialIoI2cWrite PciCfgBase 0x%x \n",PciCfgBase));
  if (!SerialIoI2cPciEnable (PciCfgBase, &MmioBase64)) {
    DEBUG ((DEBUG_ERROR, "SerialIoI2cWrite Device is still not enabled or there is no BAR assigned\n"));
    return EFI_DEVICE_ERROR;
  }

  MmioBase = (UINTN) MmioBase64;
  if (Restart) {
    Status = SerialIoI2cInitializeWriteRead (MmioBase, TargetAddress, &Frequency, TimingConfig);
    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "SerialIoI2cWrite Error during initialization\n"));
      return Status;
    }
  }

  SerialIoI2cClearErrors (MmioBase);

  Timeout = TimeBudget;
  while (1) {
    if (SerialIoI2cIsRTxAbrt (MmioBase)) {
      DEBUG ((DEBUG_ERROR, "SerialIoI2cWrite Error during transmit\n"));
      SerialIoI2cDumpTxAbort (MmioBase);
      SerialIoI2cDisableController (MmioBase);
      SerialIoI2cInitializeWriteRead (MmioBase, TargetAddress, &Frequency, TimingConfig);
      return EFI_DEVICE_ERROR;
    }

    //
    // Add Data to TX FIFO
    // If TX FIFO is Full or at its limit decrease Timeout
    //
    if (SerialIoI2cPrivateWrite (&WriteLength, &WriteBuffer, MmioBase, Stop, Restart)) {
      Restart = FALSE;
    } else {
      MicroSecondDelay (1);
      Timeout--;
    }

    if (WriteLength == 0) {
      Status = SerialIoI2cWaitTxFifoEmptyIcNotActive (MmioBase);
      if (Status != EFI_SUCCESS) {
        DEBUG ((DEBUG_ERROR, "SerialIoI2cWrite Error Tx FIFO not empty or still active\n"));
        return Status;
      }
      return EFI_SUCCESS;
    }

    if (Timeout == 0) {
      SerialIoI2cDisableController (MmioBase);
      SerialIoI2cInitializeWriteRead (MmioBase, TargetAddress, &Frequency, TimingConfig);
      DEBUG ((DEBUG_ERROR, "SerialIoI2cWrite Timeout\n"));
      return EFI_TIMEOUT;
    }
  }
}

/**
  Execute an I2C read command

  Addressing mode will defined based upon TargetAddress and mask:
  TargetAddress & 0x80000000 - if true will result in 10bit addressing
  Refer to: I2C_ADDRESSING_10_BIT

  @param[in]   PciCfgBase     Pci Config Base Address of specified I2C controller
                              @Note: If I2C Number is provided this parameter is ignored and can be left as 0
  @param[in]   I2cNumber      Optional: I2C controller applicable on bus 0 only
  @param[in]   TargetAddress  Address of the end point device
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
  IN UINT32                TargetAddress,
  IN UINT32                ReadLength,
  IN UINT8                 *ReadBuffer,
  IN UINT64                TimeBudget,
  IN UINTN                 Frequency,
  IN SERIAL_IO_I2C_TIMING  *TimingConfig  OPTIONAL,
  IN BOOLEAN               Restart
  )
{
  EFI_STATUS  Status;
  UINT64      Timeout;
  UINTN       MmioBase;
  UINT64      MmioBase64;

  if (ReadLength == 0 || (ReadLength != 0 && ReadBuffer == NULL)) {
    DEBUG ((DEBUG_ERROR, "SerialIoI2cRead Invalid Parameters\n"));
    return EFI_INVALID_PARAMETER;
  }

  MmioBase64    = 0x0;

  //
  // If I2cNumber is provided, override PciCfgBase
  //
  if (I2cNumber) {
    if (*I2cNumber >= GetPchMaxSerialIoI2cControllersNum ()) {
      return EFI_INVALID_PARAMETER;
    }
    PciCfgBase = (UINTN) SerialIoI2cPciCfgBase (*I2cNumber);
  }

  DEBUG ((DEBUG_VERBOSE, "SerialIoI2cRead PciCfgBase 0x%x \n",PciCfgBase));
  if (!SerialIoI2cPciEnable (PciCfgBase,&MmioBase64)) {
    DEBUG ((DEBUG_ERROR, "SerialIoI2cRead Device is still not enabled or there is no BAR assigned\n"));
    return EFI_DEVICE_ERROR;
  }

  MmioBase = (UINTN) MmioBase64;
  if (Restart) {
    Status = SerialIoI2cInitializeWriteRead (MmioBase, TargetAddress, &Frequency, TimingConfig);
    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "SerialIoI2cRead Error during initialization\n"));
      return Status;
    }
  }

  SerialIoI2cClearErrors (MmioBase);

  Timeout = TimeBudget;
  while (1) {
    if (SerialIoI2cIsRTxAbrt (MmioBase)) {
      DEBUG ((DEBUG_ERROR, "SerialIoI2cRead Error during transmit\n"));
      SerialIoI2cDumpTxAbort (MmioBase);
      SerialIoI2cDisableController (MmioBase);
      return EFI_DEVICE_ERROR;
    }

    Status = SerialIoI2cPrivateRead (&ReadLength, &ReadBuffer, MmioBase, Restart);
    if (Status == EFI_TIMEOUT) {
      return Status;
    }
    //EFI_NOT_READY might be expected
    if (Status == EFI_SUCCESS && ReadLength == 0) {
      return EFI_SUCCESS;
    }

    Restart = FALSE;
    MicroSecondDelay (1);
    Timeout--;
    if (Timeout == 0) {
      SerialIoI2cDisableController (MmioBase);
      DEBUG ((DEBUG_ERROR, "SerialIoI2cRead Timeout\n"));
      return EFI_TIMEOUT;
    }
  }
}
