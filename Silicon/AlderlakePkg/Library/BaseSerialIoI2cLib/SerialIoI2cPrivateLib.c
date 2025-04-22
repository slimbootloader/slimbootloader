/** @file

  Copyright (c) 2020 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <IndustryStandard/Pci22.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PciSegmentLib.h>
#include <Library/SerialIoI2cLib.h>
#include <Library/TimerLib.h>

#include <PchReservedResources.h>
#include <Register/SerialIoRegs.h>

#include "SerialIoI2cRegs.h"
#include "SerialIoI2cPrivateLib.h"
#include "SerialIoI2cRegsVer2.h"

/**
  Get speed configuration for I2C Control Register based on the bus clock frequency

  @param[in]   *Frequency   Pointer to bus clock frequency (in kHz)

  @retval  SERIAL_IO_I2C_SPEED_SETTING    Operation speed bit setting
**/
UINT8
STATIC
SerialIoI2cGetOperationSpeed (
  IN UINTN  *Frequency
  )
{
  if (*Frequency < 400) {
    return I2cSpeedStandard;
  } else if (*Frequency < 3400) {
    return I2cSpeedFast;
  } else {
    return I2cSpeedHigh;
  }
}

/**
  Checks I2C ic_en status bit

  @param[in]  MmioBase   MMIO Base Address of specified I2C controller

  @retval   TRUE    I2C is in enabled state
  @retval   FALSE   I2C is inactive
**/
BOOLEAN
STATIC
SerialIoI2cIcEnableStatus (
  IN UINT64  MmioBase
  )
{
  SERIAL_IO_I2C_MEM_ENABLE_STATUS  EnableStatus;
  EnableStatus.Data32 = MmioRead32 ((UINTN) MmioBase + R_I2C_MEM_ENABLE_STATUS);
  return (BOOLEAN) EnableStatus.Fields.IcEn;
}

//I2C Post Code
/**
  Enable I2C device when in PCI/Hidden if it has assigned Memory resources

  - Enable Memory space decoding
  - Place device in D0
  - Get it out of reset

  Also checks for SCL/SDA line states.

  @param[in]       PciCfgBase    Pci Config Base Address of specified I2C controller
  @param[in][out]  BaseAddress   MMIO Base Address of specified I2C controller

  @retval   TRUE    I2C successfully enabled
  @retval   FALSE   I2C does not have memory assigned or SDA/SCL are not high
**/
BOOLEAN
SerialIoI2cEnable (
  IN UINTN           PciCfgBase,
  IN OUT UINT64      *BaseAddress
  )
{
  if (PciCfgBase < PCH_SERIAL_IO_BASE_ADDRESS) {
    if (!SerialIoI2cPciEnable (PciCfgBase, BaseAddress)) {
      return FALSE;
    }
  } else {
    if (!SerialIoI2cHiddenEnable (PciCfgBase, BaseAddress)) {
      return FALSE;
    }
  }

  return TRUE;
}


/**
  Enable I2C device when in Hidden only if it has assigned Memory resources

  - Enable Memory space decoding
  - Place device in D0
  - Get it out of reset

  Also checks for SCL/SDA line states.

  @param[in]       PciCfgBase    Pci Config Base Address of specified I2C controller
  @param[in][out]  BaseAddress   MMIO Base Address of specified I2C controller

  @retval   TRUE    I2C successfully enabled
  @retval   FALSE   I2C does not have memory assigned or SDA/SCL are not high
**/
BOOLEAN
SerialIoI2cHiddenEnable (
  IN UINTN           PciCfgBase,
  IN OUT UINT64      *BaseAddress
  )
{
  SERIAL_IO_I2C_MEM_RESETS  I2cResets;
  SERIAL_IO_I2C_GENERAL     I2cGeneral;

  *BaseAddress = (UINT64) ((MmioRead32 ((UINTN) (PciCfgBase + PCI_BASE_ADDRESSREG_OFFSET)) & 0xFFFFF000) + LShiftU64 (MmioRead32 ((UINTN) (PciCfgBase + PCI_BASE_ADDRESSREG_OFFSET + 4)), 32));
  DEBUG ((DEBUG_ERROR, "SerialIoI2cHiddenEnable MMIO add 0x%x PCI Add 0x%x\n",*BaseAddress,PciCfgBase));

  if ((*BaseAddress == 0xFFFFFFFFFFFFF000) || (*BaseAddress == 0x0)) {
    DEBUG ((DEBUG_ERROR, "SerialIoI2cHiddenEnable Device is still not enabled, or there is no BAR assigned\n"));
    return FALSE;
  }

  MmioOr16 ((UINTN) PciCfgBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE); // Enable MSE
  //
  // Reading back value after write to ensure bridge observes the BAR1 write access
  // HSD: https://hsdes.intel.com/appstore/article/#/14011046221 // PCH:RestrictedContent
  //
  MmioRead16 ((UINTN) PciCfgBase + PCI_COMMAND_OFFSET);

  MmioAndThenOr8 (PciCfgBase + R_SERIAL_IO_CFG_PME_CTRL_STS, (UINT8)~B_SERIAL_IO_CFG_PME_CTRL_STS_PWR_ST, 0x0); // Place device in D0
  MmioRead8 (PciCfgBase + R_SERIAL_IO_CFG_PME_CTRL_STS); // Dummy Read after modifying PME

  I2cResets.Data32 = MmioRead32 ((UINTN) *BaseAddress + R_SERIAL_IO_MEM_PPR_RESETS);
  I2cResets.Fields.ResetI2C = 1;
  I2cResets.Fields.ResetDma = 1; // todo check if still needed
  MmioWrite32 ((UINTN)*BaseAddress + R_SERIAL_IO_MEM_PPR_RESETS, I2cResets.Data32);

  I2cGeneral.Data32 = MmioRead32 ((UINTN)*BaseAddress + R_I2C_MEM_GENERAL);
  if (I2cGeneral.Fields.SdaRdPostDr == 0 || I2cGeneral.Fields.SclRdPostDr == 0) {
    DEBUG ((DEBUG_WARN, "SerialIoI2cPciEnable Warning No pullups or no STOP issued SdaRdPostDr: 0x%X  SclRdPostDr: 0x%X\n", I2cGeneral.Fields.SdaRdPostDr, I2cGeneral.Fields.SclRdPostDr));
  }

  return TRUE;
}


/**
  Enable I2C device when in PCI only if it has assigned Memory resources

  - Enable Memory space decoding
  - Place device in D0
  - Get it out of reset

  Also checks for SCL/SDA line states.

  @param[in]       PciCfgBase    Pci Config Base Address of specified I2C controller
  @param[in][out]  BaseAddress   MMIO Base Address of specified I2C controller

  @retval   TRUE    I2C successfully enabled
  @retval   FALSE   I2C does not have memory assigned or SDA/SCL are not high
**/
BOOLEAN
SerialIoI2cPciEnable (
  IN UINTN           PciCfgBase,
  IN OUT UINT64      *BaseAddress
  )
{
  SERIAL_IO_I2C_MEM_RESETS  I2cResets;
  SERIAL_IO_I2C_GENERAL     I2cGeneral;

  *BaseAddress = (UINT64) ((PciSegmentRead32 ((UINTN) (PciCfgBase + 0x10)) & 0xFFFFF000) + LShiftU64 (PciSegmentRead32 ((UINTN) (PciCfgBase + 0x10 + 4)), 32));
  if ((*BaseAddress == 0xFFFFFFFFFFFFF000) || (*BaseAddress == 0x0)) {
    DEBUG ((DEBUG_ERROR, "SerialIoI2cPciEnable Device is still not enabled, or there is no BAR assigned\n"));
    return FALSE;
  }
  if ((PciSegmentRead8 (PciCfgBase + PCI_COMMAND_OFFSET) & EFI_PCI_COMMAND_MEMORY_SPACE) != EFI_PCI_COMMAND_MEMORY_SPACE) {
    PciSegmentOr8 (PciCfgBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE); // Enable MSE
  }

  PciSegmentAndThenOr8 (PciCfgBase + R_SERIAL_IO_CFG_PME_CTRL_STS, (UINT8)~B_SERIAL_IO_CFG_PME_CTRL_STS_PWR_ST, 0x0); // Place device in D0
  PciSegmentRead8 (PciCfgBase + R_SERIAL_IO_CFG_PME_CTRL_STS); // Dummy Read after modifying PME

  I2cResets.Data32 = MmioRead32 ((UINTN) *BaseAddress + R_SERIAL_IO_MEM_PPR_RESETS);
  if (I2cResets.Fields.ResetI2C == 0 && I2cResets.Fields.ResetDma == 0) {
    I2cResets.Fields.ResetI2C = 3;
    I2cResets.Fields.ResetDma = 1; // todo check if still needed
    MmioWrite32 ((UINTN)*BaseAddress + R_SERIAL_IO_MEM_PPR_RESETS, I2cResets.Data32);
    MmioRead32 ((UINTN)*BaseAddress + R_SERIAL_IO_MEM_PPR_RESETS);

  I2cGeneral.Data32 = MmioRead32 ((UINTN)*BaseAddress + R_I2C_MEM_GENERAL);
    if (I2cGeneral.Fields.SdaRdPostDr == 0 || I2cGeneral.Fields.SclRdPostDr == 0) {
      DEBUG ((DEBUG_WARN, "SerialIoI2cPciEnable Warning No pullups or no STOP issued SdaRdPostDr: 0x%X  SclRdPostDr: 0x%X\n", I2cGeneral.Fields.SdaRdPostDr, I2cGeneral.Fields.SclRdPostDr));
    }
  }
  return TRUE;
}

/**
  Disable I2C device when in PCI only if it has assigned Memory resources

  - Disables DW_apb_i2c (TX and RX FIFOs are held in an erased state)
  - Place it in reset state
  - Place device in D3
  - Disable Memory space decoding

  @param[in]       PciCfgBase    Pci Config Base Address of specified I2C controller

  @retval   TRUE    I2C successfully enabled
  @retval   FALSE   I2C does not have memory assigned
**/
VOID
SerialIoI2cPciDisable (
  IN UINTN           PciCfgBase
  )
{
  UINT64                    BaseAddress;
  SERIAL_IO_I2C_MEM_RESETS  I2cResets;

  BaseAddress = 0;

  BaseAddress = (UINT64) ((PciSegmentRead32 ((UINTN) (PciCfgBase + 0x10)) & 0xFFFFF000) + LShiftU64 (PciSegmentRead32 ((UINTN) (PciCfgBase + 0x10 + 4)), 32));
  if ((BaseAddress == 0xFFFFFFFFFFFFF000) || (BaseAddress == 0x0)) {
    return; // no MMIO access/unknown I2C state
  }

  SerialIoI2cDisableController ((UINTN) BaseAddress);

  I2cResets.Data32 = MmioRead32 ((UINTN) BaseAddress + R_SERIAL_IO_MEM_PPR_RESETS);
  I2cResets.Fields.ResetI2C = 0;
  I2cResets.Fields.ResetDma = 0;
  MmioWrite32 ((UINTN) BaseAddress + R_SERIAL_IO_MEM_PPR_RESETS, I2cResets.Data32);

  //
  // Set D3
  //
  PciSegmentOr8 (PciCfgBase + R_SERIAL_IO_CFG_PME_CTRL_STS, (UINT8) B_SERIAL_IO_CFG_PME_CTRL_STS_PWR_ST);
  PciSegmentRead8 (PciCfgBase + R_SERIAL_IO_CFG_PME_CTRL_STS); // Dummy Read after modifying PME

  //
  // Disable MSE
  //
  PciSegmentAnd8 (PciCfgBase + PCI_COMMAND_OFFSET, (UINT8) ~EFI_PCI_COMMAND_MEMORY_SPACE);
}

/**
  Enables DW_apb_i2c and checks for ic_en status

  @param[in]  MmioBase   MMIO Base Address of specified I2C controller

  @retval   EFI_SUCCESS    I2C successfully enabled
  @retval   EFI_TIMEOUT    I2C did not enable in given time refer to I2cEnableTimeout
**/
EFI_STATUS
SerialIoI2cEnableController (
  IN UINTN   MmioBase
  )
{
  SERIAL_IO_I2C_MEM_ENABLE  MemEnable;
  UINT64                    Timeout;

  MemEnable.Data32 = 0x0;
  MemEnable.Fields.Enable = 1;
  MmioWrite32 (MmioBase + R_I2C_MEM_ENABLE, MemEnable.Data32);
  //
  // Wait until I2C is enabled
  //
  Timeout = I2cEnableTimeout;
  while (!SerialIoI2cIcEnableStatus (MmioBase)) {
    MicroSecondDelay (1);
    Timeout--;
    if (Timeout == 0) {
      DEBUG ((DEBUG_ERROR, "SerialIoI2cEnableController  Timeout during I2C enable status check\n"));
      return EFI_TIMEOUT;
    }
  }

  return EFI_SUCCESS;
}

/**
  Disables DW_apb_i2c (TX and RX FIFOs are held in an erased state) and checks for ic_en status

  @param[in]  MmioBase   MMIO Base Address of specified I2C controller

  @retval   EFI_SUCCESS    I2C successfully disabled
  @retval   EFI_TIMEOUT    I2C did not enable in given time refer to I2cDisableTimeout
**/
EFI_STATUS
SerialIoI2cDisableController (
  IN UINTN   MmioBase
  )
{
  SERIAL_IO_I2C_MEM_ENABLE  MemEnable;
  UINT64                    Timeout;

  MicroSecondDelay (100);
  MemEnable.Data32 = 0x0;
  MemEnable.Fields.Enable = 0;

  SerialIoI2cClearErrors (MmioBase);
  MmioWrite32 (MmioBase + R_I2C_MEM_ENABLE, MemEnable.Data32);
  //
  // Wait until I2C is disabled
  //
  Timeout = I2cDisableTimeout;
  while (SerialIoI2cIcEnableStatus (MmioBase)) {
    MicroSecondDelay (1);
    Timeout--;
    if (Timeout == 0) {
      DEBUG ((DEBUG_ERROR, "SerialIoI2cDisableController  Timeout during I2C disable status check\n"));
      return EFI_TIMEOUT;
    }
  }

  return EFI_SUCCESS;
}

/**
  Clear software clearable interrupts and TX_ABRT interrupt of IC_RAW_INTR_STAT and ABRT_SOUCE registers
  This clear is done by reading the I2C_MEM_CLR_INTR and I2C_MEM_CLR_TX_ABRT registers

  @param[in]   MmioBase   MMIO Base Address of specified I2C controller
**/
VOID
SerialIoI2cClearErrors (
  IN UINTN  MmioBase
  )
{
  MmioRead32 (MmioBase + R_I2C_MEM_CLR_INTR);
  MmioRead32 (MmioBase + R_I2C_MEM_CLR_TX_ABRT);
}

/**
  Set the SDA Hold and speed specific SCL high count and SCL low count registers

  @param[in]       MmioBase       MMIO Base Address of specified I2C controller
  @param[in][out]  Frequency      Pointer to bus clock frequency (in kHz)
  @param[in]       TimingConfig   Optional: Setup variable struct containing override values for SCL LCNT/HCNT
                                    User can override version specific timer values in PCH Setup or specify NULL
                                    to use default config
**/
VOID
SerialIoI2cSetFrequency (
  IN     UINTN                 MmioBase,
  IN OUT UINTN                 *Frequency,
  IN     SERIAL_IO_I2C_TIMING  *TimingConfig   OPTIONAL
  )
{
  SERIAL_IO_I2C_MEM_SDA_HOLD     SdaHold;
  SERIAL_IO_I2C_MEM_SS_SCL_LCNT  SsSclLcnt;
  SERIAL_IO_I2C_MEM_SS_SCL_HCNT  SsSclHcnt;
  SERIAL_IO_I2C_MEM_FS_SCL_LCNT  FsSclLcnt;
  SERIAL_IO_I2C_MEM_FS_SCL_HCNT  FsSclHcnt;
  SERIAL_IO_I2C_MEM_HS_SCL_LCNT  HsSclLcnt;
  SERIAL_IO_I2C_MEM_HS_SCL_HCNT  HsSclHcnt;

  if (*Frequency < 400) {
    //
    // Standard Speed : frequency up to 400kHz
    //
    if (TimingConfig != NULL) {
      SsSclHcnt.Fields.IcSsSclHcnt = TimingConfig->SclHcnt;
      SsSclLcnt.Fields.IcSsSclLcnt = TimingConfig->SclLcnt;
      SdaHold.Fields.IcSdaRxHold = TimingConfig->SdaRxHold;
      SdaHold.Fields.IcSdaTxHold = TimingConfig->SdaTxHold;
    } else {
      SerialIoI2cGetSsScl (MmioBase, &SsSclHcnt, &SsSclLcnt, Frequency);
      SerialIoI2cGetSdaHold (MmioBase, &SdaHold, Frequency);
    }
    MmioWrite32 (MmioBase + R_SERIAL_IO_I2C_MEM_SDA_HOLD, SdaHold.Data32);
    MmioWrite32 (MmioBase + R_I2C_MEM_SS_SCL_HCNT, SsSclHcnt.Data32);
    MmioWrite32 (MmioBase + R_I2C_MEM_SS_SCL_LCNT, SsSclLcnt.Data32);
    return;
  } else if (*Frequency < 3400) {
    //
    // Fast Speed: frequency in range [400kHz, 3.4MHz)
    // Bus Clock set differently in ranges [400kHz, 1MHz) and [1MHz, 3.4MHz)
    //
    if (*Frequency < 1000) {
      *Frequency = 400;
    } else {
      *Frequency = 1000;
    }

    if (TimingConfig != NULL) {
      FsSclHcnt.Fields.IcFsSclHcnt = TimingConfig->SclHcnt;
      FsSclLcnt.Fields.IcFsSclLcnt = TimingConfig->SclLcnt;
      SdaHold.Fields.IcSdaRxHold = TimingConfig->SdaRxHold;
      SdaHold.Fields.IcSdaTxHold = TimingConfig->SdaTxHold;
    } else {
      SerialIoI2cGetFsScl (MmioBase, &FsSclHcnt, &FsSclLcnt, Frequency);
      SerialIoI2cGetSdaHold (MmioBase, &SdaHold, Frequency);
    }
    MmioWrite32 (MmioBase + R_SERIAL_IO_I2C_MEM_SDA_HOLD, SdaHold.Data32);
    MmioWrite32 (MmioBase + R_I2C_MEM_FS_SCL_HCNT, FsSclHcnt.Data32);
    MmioWrite32 (MmioBase + R_I2C_MEM_FS_SCL_LCNT, FsSclLcnt.Data32);
    return;
  } else {
    //
    // High Speed : frequency of 3.4MHz or greater
    //
    *Frequency = 3400;

    if (TimingConfig != NULL) {
      HsSclHcnt.Fields.IcHsSclHcnt = TimingConfig->SclHcnt;
      HsSclLcnt.Fields.IcHsSclLcnt = TimingConfig->SclLcnt;
      SdaHold.Fields.IcSdaRxHold = TimingConfig->SdaRxHold;
      SdaHold.Fields.IcSdaTxHold = TimingConfig->SdaTxHold;
    } else {
      SerialIoI2cGetHsScl (MmioBase, &HsSclHcnt,&HsSclLcnt, Frequency);
      SerialIoI2cGetSdaHold (MmioBase, &SdaHold, Frequency);
    }
    MmioWrite32 (MmioBase + R_SERIAL_IO_I2C_MEM_SDA_HOLD, SdaHold.Data32);
    MmioWrite32 (MmioBase + R_I2C_MEM_HS_SCL_HCNT, HsSclHcnt.Data32);
    MmioWrite32 (MmioBase + R_I2C_MEM_HS_SCL_LCNT, HsSclLcnt.Data32);
  }
}

/**
  Write the given slave address to the I2C Target Address Register

  @param[in]   MmioBase       MMIO Base Address of specified I2C controller
  @param[in]   SlaveAddress   Address of the end point device
**/
VOID
SerialIoI2cSetSlaveAddress (
  IN UINTN   MmioBase,
  IN UINT32  SlaveAddress
  )
{
  SERIAL_IO_I2C_MEM_TAR  MemTar;

  MemTar.Data32 = 0x0;
  MemTar.Fields.IcTar = SlaveAddress & 0x3FF; // Mask 10 bits

  //
  // Check if current mode is 10 bit or 7 bit
  //
  MemTar.Fields.Ic10bMaster = 0;
  if (SlaveAddress & 0x80000000) {
    MemTar.Fields.Ic10bMaster = 1;
  }

  MmioWrite32 (MmioBase + R_I2C_MEM_TAR, MemTar.Data32);
}

/**
  Configure the I2C Control Register and start the controller

  @param[in]   MmioBase     MMIO Base Address of specified I2C controller
  @param[in]   Frequency    Pointer to bus clock frequency (in kHz)

  @retval   EFI_TIMEOUT   I2C enable confirmation timed out
  @retval   EFI_SUCCESS   I2C successfully enabled
**/
VOID
SerialIoI2cStartOperation (
  IN UINTN   MmioBase,
  IN UINTN   *Frequency
  )
{
  SERIAL_IO_I2C_MEM_CON       MemCon;

  MemCon.Data32 = MmioRead32 (MmioBase + R_I2C_MEM_CON);
  MemCon.Fields.MasterMode  = 1;
  MemCon.Fields.Speed       = SerialIoI2cGetOperationSpeed (Frequency);
  MemCon.Fields.IcRestartEn = 1;
  MemCon.Fields.IcSlaveDis  = 1;
  MmioWrite32 (MmioBase + R_I2C_MEM_CON, MemCon.Data32);
}

/**
  Checks Transmit FIFO Not Full Status

  @param[in]  MmioBase    MMIO Base Address of specified I2C controller

  @retval     TRUE        Tx FIFO is not full
  @retval     FALSE       Tx FIFO is full
**/
STATIC
BOOLEAN
SerialIoI2cIsTxFifoNotFull (
  UINTN   MmioBase
  )
{
  SERIAL_IO_I2C_MEM_STATUS     MemStatus;
  MemStatus.Data32 = MmioRead32 (MmioBase + R_I2C_MEM_STATUS);
  return (BOOLEAN) MemStatus.Fields.TFNF;
}

/**
  Handle the writing operations of the I2C controller

  @param[in][out]   WriteLength  Pointer to write length
  @param[in][out]   WriteBuffer  Double pointer to data in write buffer
  @param[in]        MmioBase     MMIO Base Address of specified I2C controller
  @param[in]        Stop         Issues STOP on the last byte
  @param[in]        Restart      Indication if restart is needed

  @retval   TRUE       Data added to TX FIFO
            FALSE      TX FIFO is full or at its limit - Write did not complete
**/
BOOLEAN
SerialIoI2cPrivateWrite (
  IN OUT UINT32  *WriteLength,
  IN OUT UINT8   **WriteBuffer,
  IN     UINTN   MmioBase,
  IN     BOOLEAN Stop,
  IN     BOOLEAN Restart
  )
{
  SERIAL_IO_I2C_MEM_DATA_CMD       I2cDataCmd;
  SERIAL_IO_I2C_MEM_TRANSMIT_FIFO  I2cTransmitFifo;

  //
  // Check if TX FIFO is full
  //
  if (!SerialIoI2cIsTxFifoNotFull (MmioBase)) {
    return FALSE;
  }

  //
  // Check for TX FIFO overrun
  //
  I2cTransmitFifo.Data32 = MmioRead32 (MmioBase + R_I2C_MEM_TXFLR);
  if (I2cTransmitFifo.Fields.TXFLR >= 63) {
    return FALSE;
  }

  I2cDataCmd.Data32      = 0x0;
  I2cDataCmd.Fields.Data = (UINT8) **WriteBuffer;
  I2cDataCmd.Fields.Cmd  = 0; //Write
  if (*WriteLength == 1 && Stop == TRUE) {
    I2cDataCmd.Fields.Stop = 1;
  }
  if (Restart) {
    I2cDataCmd.Fields.Restart = 1;
  }
  MmioWrite32 (MmioBase + R_I2C_MEM_DATA_CMD, I2cDataCmd.Data32);
  *WriteBuffer += 1;
  *WriteLength -= 1;
  return TRUE;
}

/**
  Handle the reading operations of the I2C controller

  @param[in][out]   ReadLength   Pointer to read length
  @param[in][out]   ReadBuffer   Double pointer to data in read buffer
  @param[in]        MmioBase     MMIO Base Address of specified I2C controller
  @param[in]        Restart      Indication if restart is needed
**/
EFI_STATUS
SerialIoI2cPrivateRead (
  IN OUT UINT32  *ReadLength,
  IN OUT UINT8   **ReadBuffer,
  IN     UINTN   MmioBase,
  IN     BOOLEAN Restart
  )
{
  SERIAL_IO_I2C_MEM_DATA_CMD      I2cDataCmd;
  SERIAL_IO_I2C_MEM_RECEIVE_FIFO  I2cReceiveFifo;
  SERIAL_IO_I2C_MEM_STATUS        MemStatus;
  UINT32                          Timeout;

  MemStatus.Data32      = MmioRead32 (MmioBase + R_I2C_MEM_STATUS);
  I2cReceiveFifo.Data32 = MmioRead32 (MmioBase + R_I2C_MEM_RXFLR);

  //
  // Check for RX FIFO overrun, RX FIFO Full and Transmit FIFO Not Full
  //
  if (I2cReceiveFifo.Fields.RXFLR >= 63) {
    DEBUG ((DEBUG_WARN, "SerialIoI2cPrivateRead  Receive FIFO size  0x%X\n", I2cReceiveFifo.Fields.RXFLR));
    return EFI_NOT_READY;
  }
  if (MemStatus.Fields.RFF) {
    DEBUG ((DEBUG_WARN, "SerialIoI2cPrivateRead  Receive FIFO Full Status 0x%X\n", MemStatus.Fields.RFF));
    return EFI_NOT_READY;
  }
  if (!MemStatus.Fields.TFNF) {
    DEBUG ((DEBUG_WARN, "SerialIoI2cPrivateRead  Transmit FIFO Full 0x%X\n", MemStatus.Fields.TFNF));
    return EFI_NOT_READY;
  }

  I2cDataCmd.Data32      = 0;
  I2cDataCmd.Fields.Cmd  = 1; //Read
  I2cDataCmd.Fields.Stop = 0;
  if (*ReadLength == 1) {
    I2cDataCmd.Fields.Stop = 1;
  }
  if (Restart) {
    I2cDataCmd.Fields.Restart = 1;
  }
  MmioWrite32 (MmioBase + R_I2C_MEM_DATA_CMD, I2cDataCmd.Data32);

  Timeout = I2cReceiveTimeout;

  while (Timeout > 0) {
    MemStatus.Data32 = MmioRead32 (MmioBase + R_I2C_MEM_STATUS);
    if (MemStatus.Fields.RFNE) {
      I2cDataCmd.Data32 = MmioRead32 (MmioBase + R_I2C_MEM_DATA_CMD);
      (**ReadBuffer) = (UINT8) I2cDataCmd.Fields.Data;
      (*ReadBuffer)++;
      (*ReadLength)--;
      Timeout = I2cReceiveTimeout; // Reset Timer
      if (*ReadLength == 0) {
        return EFI_SUCCESS;
      }
      break;
    }
    MicroSecondDelay (1);
    Timeout--;
    if (Timeout == 0) {
      DEBUG (( DEBUG_ERROR, "SerialIoI2cPrivateRead Timeout\n"));
      return EFI_TIMEOUT;
    }
  }

  return EFI_SUCCESS;
}

/**
  Set up the I2C Controller for reading and/or writing

  Before any transaction:
   1. Verify I2C Controller is active
   2. Disable I2C Controller
   3. Clear any errors / interrupts
   4. Set I2C frequency values
   5. Write slave address to I2C target address register
   6. Initiate I2C Controller

  @param[in]   MmioBase        MMIO Base Address of specified I2C controller
  @param[in]   Slave Address  -Address of the end point device
  @param[in]   *Frequency      Pointer to bus clock frequency (in kHz)
  @param[in]   *TimingConfig   Pointer to optional struct passed down to SerialIoI2cSetFrequency ()

  @retval   EFI_TIMEOUT        Timeout occurred in I2C disable/enable
  @retval   EFI_DEVICE_ERROR   The verification failed
  @retval   EFI_SUCCESS        I2C W/R successfully initialized

**/
EFI_STATUS
SerialIoI2cInitializeWriteRead (
  IN UINTN                 MmioBase,
  IN UINT32                SlaveAddress,
  IN UINTN                 *Frequency,
  IN SERIAL_IO_I2C_TIMING  *TimingConfig  OPTIONAL
  )
{
  EFI_STATUS Status;
  UINT32     Data32;

  //
  // Verify that the I2C Mem Control register is not disabled, in D3, or in reset.
  //
  Data32 = MmioRead32 (MmioBase + R_I2C_MEM_CON);
  if (Data32 == 0xFFFFFFFF || Data32 == 0x0) {
    DEBUG ((DEBUG_ERROR, "SerialIoI2cInitializeWriteRead  I2C controller verification failed\n"));
    return EFI_DEVICE_ERROR;
  }

  Status = SerialIoI2cDisableController (MmioBase);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  SerialIoI2cClearErrors (MmioBase);
  SerialIoI2cSetFrequency (MmioBase, Frequency, TimingConfig);
  SerialIoI2cSetSlaveAddress (MmioBase, SlaveAddress);
  SerialIoI2cStartOperation (MmioBase, Frequency);

  Status = SerialIoI2cEnableController (MmioBase);
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  return EFI_SUCCESS;
}

/**
  Get the I2C SDA Hold value for the given frequency

  @param[in]       MmioBase    MMIO Base Address of specified I2C controller
  @param[in][out]  SdaHold     Pointer to register union for R_I2C_MEM_SDA_HOLD
  @param[in]       Frequency   Pointer to bus clock frequency (in kHz)
**/
VOID
SerialIoI2cGetSdaHold (
  IN      UINTN                       MmioBase,
  IN OUT  SERIAL_IO_I2C_MEM_SDA_HOLD  *SdaHold,
  IN      UINTN                       *Frequency
  )
{
  SdaHold->Data32 = MmioRead32 (MmioBase + R_SERIAL_IO_I2C_MEM_SDA_HOLD);

  if (*Frequency < 1000) {
    //
    // frequency in range [100kHz, 1MHz)
    //
    SdaHold->Fields.IcSdaTxHold = V_SERIAL_IO_I2C_MEM_SDA_TX_100_1000_KHZ;
    SdaHold->Fields.IcSdaRxHold = V_SERIAL_IO_I2C_MEM_SDA_RX_100_1000_KHZ;
  } else if (*Frequency < 3400) {
    //
    // frequency in range [1MHz, 3.4MHz)
    //
    SdaHold->Fields.IcSdaTxHold = V_SERIAL_IO_I2C_MEM_SDA_TX_1000_3400_KHZ;
    SdaHold->Fields.IcSdaRxHold = V_SERIAL_IO_I2C_MEM_SDA_RX_1000_3400_KHZ;
  } else if (*Frequency >= 3400) {
    //
    // frequency >= 3.4MHz
    //
    SdaHold->Fields.IcSdaTxHold = V_SERIAL_IO_I2C_MEM_SDA_TX_3400_INF_KHZ;
    SdaHold->Fields.IcSdaRxHold = V_SERIAL_IO_I2C_MEM_SDA_RX_3400_INF_KHZ;
  }
}

/**
  Get Standard Speed I2C Clock SCL High/Low Count Register value
  Applies to frequency in range [100kHz, 400kHz)

  @param[in]        MmioBase    MMIO Base Address of specified I2C controller
  @param[in][out]   SsSclHcnt   Pointer to register union for R_I2C_MEM_SS_SCL_HCNT
  @param[in][out]   SsSclLcnt   Pointer to register union for R_I2C_MEM_SS_SCL_LCNT
  @param[in]        Frequency   Pointer to bus clock frequency (in kHz)
**/
VOID
SerialIoI2cGetSsScl (
  IN      UINTN                           MmioBase,
  IN OUT  SERIAL_IO_I2C_MEM_SS_SCL_HCNT  *SsSclHcnt,
  IN OUT  SERIAL_IO_I2C_MEM_SS_SCL_LCNT  *SsSclLcnt,
  IN      UINTN                          *Frequency
  )
{
  SsSclLcnt->Data32 = MmioRead32 (MmioBase + R_I2C_MEM_SS_SCL_LCNT);
  SsSclHcnt->Data32 = MmioRead32 (MmioBase + R_I2C_MEM_SS_SCL_HCNT);

  if (*Frequency <= 20) {
    SsSclHcnt->Fields.IcSsSclHcnt = V_SERIAL_IO_I2C_MEM_SS_SCL_HCNT_20;
    SsSclLcnt->Fields.IcSsSclLcnt = V_SERIAL_IO_I2C_MEM_SS_SCL_LCNT_20;
  } else if (*Frequency <= 50) {
    SsSclHcnt->Fields.IcSsSclHcnt = V_SERIAL_IO_I2C_MEM_SS_SCL_HCNT_50;
    SsSclLcnt->Fields.IcSsSclLcnt = V_SERIAL_IO_I2C_MEM_SS_SCL_LCNT_50;
  } else if (*Frequency <= 75) {
    SsSclHcnt->Fields.IcSsSclHcnt = V_SERIAL_IO_I2C_MEM_SS_SCL_HCNT_70;
    SsSclLcnt->Fields.IcSsSclLcnt = V_SERIAL_IO_I2C_MEM_SS_SCL_LCNT_70;
  } else if (*Frequency < 400) {
    SsSclHcnt->Fields.IcSsSclHcnt = V_SERIAL_IO_I2C_MEM_SS_SCL_HCNT;
    SsSclLcnt->Fields.IcSsSclLcnt = V_SERIAL_IO_I2C_MEM_SS_SCL_LCNT;
  } else {
    DEBUG ((DEBUG_ERROR, "SerialIoI2cGetSsScl Standard Speed is not applicable at frequency: %d kHz\n", *Frequency));
    ASSERT (FALSE);
  }
}

/**
  Get Fast Speed I2C Clock SCL High/Low Count Register value
  Applies to frequency in range [400kHz, 3.4MHz)

  @param[in]        MmioBase    MMIO Base Address of specified I2C controller
  @param[in][out]   FsSclHcnt   Pointer to register union for R_I2C_MEM_FS_SCL_HCNT
  @param[in][out]   FsSclLcnt   Pointer to register union for R_I2C_MEM_FS_SCL_LCNT
  @param[in]        Frequency   Pointer to bus clock frequency (in kHz)
**/
VOID
SerialIoI2cGetFsScl (
  IN      UINTN                           MmioBase,
  IN OUT  SERIAL_IO_I2C_MEM_FS_SCL_HCNT  *FsSclHcnt,
  IN OUT  SERIAL_IO_I2C_MEM_FS_SCL_LCNT  *FsSclLcnt,
  IN      UINTN                          *Frequency
  )
{
  FsSclHcnt->Data32 = MmioRead32 (MmioBase +  R_I2C_MEM_FS_SCL_HCNT);
  FsSclLcnt->Data32 = MmioRead32 (MmioBase +  R_I2C_MEM_FS_SCL_LCNT);

  if (*Frequency >= 400 && *Frequency < 1000) {
    FsSclHcnt->Fields.IcFsSclHcnt = V_SERIAL_IO_I2C_MEM_SS_FCL_HCNT_400_1000;
    FsSclLcnt->Fields.IcFsSclLcnt = V_SERIAL_IO_I2C_MEM_SS_FCL_LCNT_400_1000;
  } else if (*Frequency >= 1000 && *Frequency < 3400) {
    FsSclHcnt->Fields.IcFsSclHcnt = V_SERIAL_IO_I2C_MEM_SS_FCL_HCNT_1000_3400;
    FsSclLcnt->Fields.IcFsSclLcnt = V_SERIAL_IO_I2C_MEM_SS_FCL_LCNT_1000_3400;
  } else {
    DEBUG ((DEBUG_ERROR, "SerialIoI2cGetFsScl Fast Speed is not applicable at frequency: %d kHz\n", *Frequency));
    ASSERT (FALSE);
  }
}

/**
  Get High Speed I2C Clock SCL High/Low Count Register value
  Applies to frequency in range [3.4MHz, inf)

  @param[in]        MmioBase    MMIO Base Address of specified I2C controller
  @param[in][out]   HsSclHcnt   Pointer to register union for R_I2C_MEM_HS_SCL_HCNT
  @param[in][out]   HsSclLcnt   Pointer to register union for R_I2C_MEM_HS_SCL_LCNT
  @param[in]        Frequency   Pointer to bus clock frequency (in kHz)
**/
VOID
SerialIoI2cGetHsScl (
  IN      UINTN                           MmioBase,
  IN OUT  SERIAL_IO_I2C_MEM_HS_SCL_HCNT  *HsSclHcnt,
  IN OUT  SERIAL_IO_I2C_MEM_HS_SCL_LCNT  *HsSclLcnt,
  IN      UINTN                          *Frequency
  )
{
  HsSclHcnt->Data32 = MmioRead32 (MmioBase + R_I2C_MEM_HS_SCL_HCNT);
  HsSclLcnt->Data32 = MmioRead32 (MmioBase + R_I2C_MEM_HS_SCL_LCNT);

  if (*Frequency >= 3400) {
    HsSclHcnt->Fields.IcHsSclHcnt = V_SERIAL_IO_I2C_MEM_HS_FCL_HCNT;
    HsSclLcnt->Fields.IcHsSclLcnt = V_SERIAL_IO_I2C_MEM_HS_FCL_LCNT;
  } else {
    DEBUG ((DEBUG_ERROR, "SerialIoI2cGetHsScl High Speed is not applicable at frequency: %d kHz\n", *Frequency));
    ASSERT (FALSE);
  }
}

/**
  Checks if transmission was aborted

  @param[in]  MmioBase    MMIO Base Address of specified I2C controller

  @retval     TRUE        Tx Abort occured
  @retval     FALSE       No Tx Abort errors
**/
BOOLEAN
SerialIoI2cIsRTxAbrt (
  UINTN   MmioBase
  )
{
  SERIAL_IO_I2C_MEM_INTR_STAT  InterruptStatus;
  InterruptStatus.Data32 = MmioRead32 (MmioBase + R_I2C_MEM_INTR_STAT);
  return (BOOLEAN) InterruptStatus.Fields.RTxAbrt;
}

/**
  Checks if Transmit FIFO is empty

  @param[in]  MmioBase    MMIO Base Address of specified I2C controller

  @retval     TRUE        Tx FIFO is empty
  @retval     FALSE       Tx FIFO not empty
**/
BOOLEAN
SerialIoI2cIsTxFifoEmpty (
  UINTN   MmioBase
  )
{
  SERIAL_IO_I2C_MEM_STATUS     MemStatus;
  MemStatus.Data32 = MmioRead32 (MmioBase + R_I2C_MEM_STATUS);
  return (BOOLEAN) MemStatus.Fields.TFE;
}

/**
  I2C Activity Status Check

  @param[in]  MmioBase    MMIO Base Address of specified I2C controller

  @retval     TRUE        I2C is Active
  @retval     FALSE       I2C is not Active
**/
BOOLEAN
STATIC
SerialIoI2cIsIcStatusAct (
  UINTN   MmioBase
  )
{
  SERIAL_IO_I2C_MEM_STATUS     MemStatus;
  MemStatus.Data32 = MmioRead32 (MmioBase + R_I2C_MEM_STATUS);
  return (BOOLEAN) MemStatus.Fields.IcStatusAct;
}

/**
  Dumps all Tx Abort Source register fields

  @param[in]  MmioBase    MMIO Base Address of specified I2C controller
**/
VOID
SerialIoI2cDumpTxAbort (
  UINTN   MmioBase
  )
{
  SERIAL_IO_I2C_MEM_TX_ABRT_SOURCE  TxAbortSource;
  TxAbortSource.Data32 = MmioRead32 (MmioBase + R_I2C_MEM_TX_ABRT_SOURCE);

  DEBUG_CODE_BEGIN ();
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.Abrt7bAddrNoAck %d\n", TxAbortSource.Fields.Abrt7bAddrNoAck));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.Abrt10bAddr1NoAck %d\n", TxAbortSource.Fields.Abrt10bAddr1NoAck));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.Abrt10bAddr2NoAck %d\n", TxAbortSource.Fields.Abrt10bAddr2NoAck));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.AbrtTxDataNoAck %d\n", TxAbortSource.Fields.AbrtTxDataNoAck));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.AbrtGCallNoAck %d\n", TxAbortSource.Fields.AbrtGCallNoAck));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.AbrtGCallRead %d\n", TxAbortSource.Fields.AbrtGCallRead));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.AbrtHsAckDet %d\n", TxAbortSource.Fields.AbrtHsAckDet));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.AbrtSbAckDet %d\n", TxAbortSource.Fields.AbrtSbAckDet));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.AbrtHsNoRst %d\n", TxAbortSource.Fields.AbrtHsNoRst));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.AbrtSbNoRst %d\n", TxAbortSource.Fields.AbrtSbNoRst));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.Abrt10bRdNoRst %d\n", TxAbortSource.Fields.Abrt10bRdNoRst));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.AbrtMasterDis %d\n", TxAbortSource.Fields.AbrtMasterDis));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.ArbLost %d\n", TxAbortSource.Fields.ArbLost));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.AbrtSlFlushTxFifo %d\n", TxAbortSource.Fields.AbrtSlFlushTxFifo));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.AbrtSlvArbLost %d\n", TxAbortSource.Fields.AbrtSlvArbLost));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.AbrtSlvRdIntx %d\n", TxAbortSource.Fields.AbrtSlvRdIntx));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.AbrtUserAbrt %d\n", TxAbortSource.Fields.AbrtUserAbrt));
  DEBUG ((DEBUG_ERROR, "TxAbortSource.Fields.TxFlushCnt %d\n", TxAbortSource.Fields.TxFlushCnt));
  DEBUG_CODE_END ();
}

/**
  Waits until Tx Fifo is empty and I2C no longer Active

  @param[in]  MmioBase    MMIO Base Address of specified I2C controller

  @retval   EFI_TIMEOUT   TX FIFO did not clear or I2C was still active
  @retval   EFI_SUCCESS   TX FIFO is empty and I2C is idle
**/
EFI_STATUS
SerialIoI2cWaitTxFifoEmptyIcNotActive (
  UINTN   MmioBase
  )
{
  UINT32  Timeout;

  Timeout = I2cWaitTxEmptyTimeout;
  while (!SerialIoI2cIsTxFifoEmpty (MmioBase) && SerialIoI2cIsIcStatusAct (MmioBase)) {
    MicroSecondDelay (1);
    Timeout--;
    if (Timeout == 0) {
      DEBUG ((DEBUG_ERROR, "SerialIoI2cWaitTxFifoEmpty Timeout during I2C TX FIFO flush\n"));
      return EFI_TIMEOUT;
    }
  }
  return EFI_SUCCESS;
}
