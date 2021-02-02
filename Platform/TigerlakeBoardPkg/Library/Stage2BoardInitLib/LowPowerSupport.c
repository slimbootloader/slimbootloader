/** @file

  Copyright (c) 2020 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Register/PmcRegs.h>
#include <Library/TimerLib.h>
#include <Register/SaRegsIpu.h>
#include <Library/PchPciBdfLib.h>
#include <Register/SaRegsIgd.h>
#include <Register/PchBdfAssignment.h>
#include <FspsUpd.h>
#include <PchAccess.h>
#include <Library/MeExtMeasurementLib.h>
#include <Register/SaRegsGna.h>
#include <Library/PchInfoLib.h>
#include <Register/SaRegsPeg.h>
#include <LowPowerSupport.h>
#include <ConfigDataStruct.h>
#include <Library/ConfigDataLib.h>


/**
  Check if given root port has device connected and enable wake capability

  @param[in]  RpNum           An unsigned integer represent the root port number.

  @retval                     TRUE if endpoint was connected
  @retval                     FALSE if no endpoint was detected
**/
BOOLEAN
IsPcieEndPointPresent (
  IN UINT8 RpNum
  )
{
  UINT32        RpBaseAddress;
  RpBaseAddress = (UINT32)PchPcieRpPciCfgBase (RpNum);
  if ((PciRead16 (RpBaseAddress) != 0xFFFF) &&
      (PciRead16 (RpBaseAddress + R_PCH_PCIE_SLSTS) & B_PCIE_SLSTS_PDS)) {
    return TRUE;
  }
  return FALSE;
}
/**
  Checks if higher functions are enabled.
  Used for Function 0 Serial Io Device disabling

  @param[in] DeviceNum       Device Number

  @retval TRUE               At least one higher function device is enabled
          FALSE              Higher functions are disabled
**/
BOOLEAN
SerialIoHigherFunctionsEnabled (
  IN UINT8                    DeviceNum
  )
{
 #define PCI_MAX_FUNC    7
  UINT8 FuncNum;
  //
  // Check all other func devs(1 to 7) status except func 0.
  //
  for (FuncNum = 1; FuncNum <= PCI_MAX_FUNC; FuncNum++) {
    if (PciRead16 (PCI_SEGMENT_LIB_ADDRESS (DEFAULT_PCI_SEGMENT_NUMBER_PCH,
                                                   DEFAULT_PCI_BUS_NUMBER_PCH,
                                                   DeviceNum,
                                                   FuncNum,
                                                   PCI_DEVICE_ID_OFFSET)
                          ) != 0xFFFF) {
      return TRUE;
    }
  }
  return FALSE;
}
/**
  Checks if Spi is Function 0 Enabled

  @param[in] SpiIndex                   Number of the SerialIo Spi controller

  @retval TRUE                          Enabled
  @retval FALSE                         Disabled
**/
BOOLEAN
IsSerialIoSpiFunction0Enabled (
  IN UINT8    SpiIndex
  )
{
  if (SerialIoSpiFuncNumber (SpiIndex) == 0) {
    if (SerialIoHigherFunctionsEnabled (SerialIoSpiDevNumber (SpiIndex))) {
      return TRUE;
    }
  }
  return FALSE;
}
/**
  Checks if I2c is Function 0 Enabled

  @param[in] I2cIndex                   Number of the SerialIo I2C controller

  @retval TRUE                          Enabled
  @retval FALSE                         Disabled
**/
BOOLEAN
IsSerialIoI2cFunction0Enabled (
  IN UINT8    I2cIndex
  )
{
  if (SerialIoI2cFuncNumber (I2cIndex) == 0) {
    if (SerialIoHigherFunctionsEnabled (SerialIoI2cDevNumber (I2cIndex))) {
      return TRUE;
    }
  }
  return FALSE;
}
/**
  Checks if Uart is Function 0 Enabled

  @param[in] UartIndex                   Number of the SerialIo Uart controller

  @retval TRUE                          Enabled
  @retval FALSE                         Disabled
**/
BOOLEAN
IsSerialIoUartFunction0Enabled (
  IN UINT8    UartIndex
  )
{
  if (SerialIoUartFuncNumber (UartIndex) == 0) {
    if (SerialIoHigherFunctionsEnabled (SerialIoUartDevNumber (UartIndex))) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Update Pep constraints.

  @param  Void
  @retval Updated Pep constraints Value
 */
UINT64 GetLowPowerS0IdleConstraint(VOID)
{

  UINT8    Index;
  UINT64   LowPowerS0IdleConstraint;
  UINT8    PepSerialIoI2c[PCH_MAX_SERIALIO_I2C_CONTROLLERS];
  UINT8    PepSerialIoSpi[PCH_MAX_SERIALIO_SPI_CONTROLLERS];
  UINT8    PepSerialIoUart[PCH_MAX_SERIALIO_UART_CONTROLLERS];
  FSPS_UPD  *FspsUpd;
  FSP_S_CONFIG  *FspsConfig;
  PEP_CFG_DATA             *PepConfigData;
  FspsUpd = (FSPS_UPD *)(UINTN)PcdGet32 (PcdFspsUpdPtr);
  FspsConfig  = &FspsUpd->FspsConfig;

  PepConfigData = (PEP_CFG_DATA *)FindConfigDataByTag (CDATA_PEP_TAG);
  if (PepConfigData == NULL) {
    DEBUG((DEBUG_ERROR, "ACPI NVS, Failed to find PEP CFGDATA \n" ));
    return 0x0000000071EBE6F4;  //Use hardcode value from bios if failed to get config data
  }
  if (IsPchH ()) {
    PepConfigData->PepEmmc = 0;
    PepConfigData->PepI2c4 = 0;
    PepConfigData->PepI2c5 = 0;
  }
  if (PciRead16 (PCI_SEGMENT_LIB_ADDRESS (0, SA_IPU_BUS_NUM, SA_IPU_DEV_NUM, SA_IPU_FUN_NUM, PCI_VENDOR_ID_OFFSET)) == 0xFFFF) {
    PepConfigData->PepIpu = 0;
  }
  if (PciRead16 (PCI_SEGMENT_LIB_ADDRESS (0, SA_GNA_BUS_NUM, SA_GNA_DEV_NUM, SA_GNA_FUN_NUM, PCI_VENDOR_ID_OFFSET)) == 0xFFFF) {
    PepConfigData->PepGna = 0;
  }
  if (!FspsConfig ->VmdEnable ) {
    PepConfigData->PepVmd = 0;
  }
  if (PciRead16 (PCI_SEGMENT_LIB_ADDRESS (0, SA_PEG_BUS_NUM, SA_PEG3_DEV_NUM, SA_PEG0_FUN_NUM, PCI_VENDOR_ID_OFFSET)) == 0xFFFF) {
    PepConfigData->PepPeg0 = 0;
  }
  if (!IsPcieEndPointPresent(RootPort3)) {
    PepConfigData->PepPcieRp3 = 0;
  }
  if (!IsPcieEndPointPresent(RootPort5)) {
    PepConfigData->PepPcieRp5 = 0;
  }
  if (!IsPcieEndPointPresent(RootPort9)) {
    PepConfigData->PepPcieRp9 = 0;
  }
  if (PciRead32 ((UINT32)PCI_SEGMENT_LIB_ADDRESS (0, SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, PCI_VENDOR_ID_OFFSET)) == 0xFFFF) {
    PepConfigData->PepGfx  = 0;
  }
  if (!PchIsGbeSupported()) {
    PepConfigData->PepGbe = 0;
  }
  if (PciRead32 ((UINT32)SataPciCfgBase (SATA_1_CONTROLLER_INDEX) + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    PepConfigData->PepSataContraints = 0;
  }
  if (FspsConfig->ThcPort0Assignment  == 0) {
    PepConfigData->PepThc0 = 0;
    PepConfigData->PepThc1 = 0;
  } else if (FspsConfig->ThcPort1Assignment  != 2) {
    PepConfigData->PepThc1 = 0;
  }

  // Disable PEP constraint if PCH HDA B0:D31:F3 device is not preset
  if (PciRead16 ((UINT32)HdaPciCfgBase () + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    PepConfigData->PepAudio = 0;
  }
  // Disable PEP constraint if PCH XHCI B0:D20:F0 device is not preset
  if (PciRead16 ((UINT32)PchXhciPciCfgBase () + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    PepConfigData->PepXhci = 0;
  }
  // Disable PEP constraint if PCH CSME device B0:D22:F0 is not preset
  if (PciRead16 ((UINT32)PchHeci1PciCfgBase () + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    PepConfigData->PepCsme = 0;
  }
  // Enable PEP constraints for Serial Io devices
  ZeroMem (PepSerialIoSpi, sizeof (PepSerialIoSpi));
  for (Index = 0; Index < PCH_MAX_SERIALIO_SPI_CONTROLLERS; Index++) {
    PepSerialIoSpi[Index] = FspsConfig->SerialIoSpiMode[Index];
    if ((PepSerialIoSpi[Index] == 0) && IsSerialIoSpiFunction0Enabled (Index)) {
      PepSerialIoSpi[Index] = 1;
    }
  }
  ZeroMem (PepSerialIoI2c, sizeof (PepSerialIoI2c));
  for (Index = 0; Index < PCH_MAX_SERIALIO_I2C_CONTROLLERS; Index++) {
    PepSerialIoI2c[Index] = FspsConfig->SerialIoI2cMode[Index];
    if ((PepSerialIoI2c[Index] == 0) && IsSerialIoI2cFunction0Enabled (Index)) {
     PepSerialIoI2c[Index] = 1;
    }
  }
  ZeroMem (PepSerialIoUart, sizeof (PepSerialIoUart));
  for (Index = 0; Index < PCH_MAX_SERIALIO_UART_CONTROLLERS; Index++) {
    PepSerialIoUart[Index] = FspsConfig->SerialIoUartMode[Index];
    if ((PepSerialIoUart[Index] == 0) && IsSerialIoUartFunction0Enabled (Index)) {
      PepSerialIoUart[Index] = 1;
    }
  }

  LowPowerS0IdleConstraint  = (((FspsConfig->SataEnable == 1 ? 0x3:0) & PepConfigData->PepSataContraints)        <<  0) | // Bit[1:0] - Storage (0:None, 1:Adapter D0/F1, 2:Raid, 3:Adapter D3)
                               ((PepSerialIoUart[0] && PepConfigData->PepUart)                                   <<  2) | // Bit[2]   - En/Dis UART0
                               ((PepSerialIoUart[1] && PepConfigData->PepUart)                                   <<  3) | // Bit[3]   - En/Dis UART1
                               (PepConfigData->PepPcieStorage                                                    <<  4) | // Bit[4]   - En/Dis PCIe NVMe/AHCI
                               ((PepSerialIoI2c[0] && PepConfigData->PepI2c0)                                    <<  5) | // Bit[5]   - En/Dis I2C0
                               ((PepSerialIoI2c[1] && PepConfigData->PepI2c1)                                    <<  6) | // Bit[6]   - En/Dis I2C1
                               (PepConfigData->PepXhci                                                           <<  7) | // Bit[7]   - En/Dis XHCI
                               (PepConfigData->PepAudio                                                          <<  8) | // Bit[8]   - En/Dis HD Audio (includes ADSP)
                               (PepConfigData->PepGfx                                                            <<  9) | // Bit[9]   - En/Dis GFX
                               (PepConfigData->PepCpu                                                            << 10) | // Bit[10]  - En/Dis CPU
                               (0                                                                              << 11) | // Bit[11]  - En/Dis EMMC
                               (0                                                                              << 12) | // Bit[12]  - En/Dis SDXC
                               ((PepSerialIoI2c[2] && PepConfigData->PepI2c2)                                    << 13) | // Bit[13]  - En/Dis I2C2
                               ((PepSerialIoI2c[3] && PepConfigData->PepI2c3)                                    << 14) | // Bit[14]  - En/Dis I2C3
                               ((PepSerialIoI2c[4] && PepConfigData->PepI2c4)                                    << 15) | // Bit[15]  - En/Dis I2C4
                               ((PepSerialIoI2c[5] && PepConfigData->PepI2c5)                                    << 16) | // Bit[16]  - En/Dis I2C5
                               ((PepSerialIoUart[2] && PepConfigData->PepUart)                                   << 17) | // Bit[17]  - En/Dis UART2
                               ((PepSerialIoSpi[0] && PepConfigData->PepSpi)                                     << 18) | // Bit[18]  - En/Dis SPI0
                               ((PepSerialIoSpi[1] && PepConfigData->PepSpi)                                     << 19) | // Bit[19]  - En/Dis SPI1
                               ((PepSerialIoSpi[2] && PepConfigData->PepSpi)                                     << 20) | // Bit[20]    -En/Dis SPI2
                               ((PepConfigData->PepIpu)                                                          << 21) | // Bit[21]  - En/Dis IPU0
                               (PepConfigData->PepCsme                                                           << 22) | // Bit[22]  - En/Dis CSME
                               ((FspsConfig->PchLanEnable && PepConfigData->PepGbe)                              << 23) | // Bit[23]  - En/Dis LAN(GBE)
                               (PepConfigData->PepPeg0Storage                                                    << 24) | // Bit[24]  - En/Dis PEG0 NVMe/AHCI
                               (PepConfigData->PepThc0                                                           << 25) | // Bit[25]  - En/Dis THC0
                               (PepConfigData->PepThc1                                                           << 26) | // Bit[26]  - En/Dis THC1
                               ((PepConfigData->PepAudio)                                                        << 27) | // Bit[27]  - En/Dis IDA
                               ((PepSerialIoI2c[6] && PepConfigData->PepI2c6)                                    << 28) | // Bit[28]  - En/Dis I2C6
                               (PepConfigData->PepTcss                                                           << 29) | // Bit[29]    -En/Dis TCSS
                               ((FspsConfig->GnaEnable && PepConfigData->PepGna)                                 << 30) | // Bit[30]  - En/Dis GNA0
                               (PepConfigData->PepVmd                                                            << 31) | // Bit[31]  - En/Dis VMD
                               (((UINT64)PepConfigData->PepPcieDg)                                               << 32) ; // Bit[32]  - En/Dis DG1 PEG0

  LowPowerS0IdleConstraint |= (((UINT64)PepConfigData->PepOverride)                                              << 33) | // Bit[33]    - En/Dis PEP Constraint Override
                              (((UINT64)(0x3 & PepConfigData->PepPcieRp3))                                       << 34) | // Bit[35:34] - PCIe RP3 (0:No Constraint, 1: D0/F1, 3: D3)
                              (((UINT64)(0x3 & PepConfigData->PepPcieRp5))                                       << 36) | // Bit[37:36] - PCIe RP5 (0:No Constraint, 1: D0/F1, 3: D3)
                              (((UINT64)(0x3 & PepConfigData->PepPcieRp9))                                       << 38) | // Bit[39:38] - PCIe RP9 (0:No Constraint, 1: D0/F1, 3: D3)
                              (((UINT64)(0x3 & PepConfigData->PepPeg0))                                          << 40) ; // Bit[41:40] - PEG0 (0:No Constraint, 1: D0/F1, 3: D3)

  DEBUG((DEBUG_INFO, "ACPI NVS, LowPowerS0IdleConstraint(Micro-pep constraints) = 0x%16X \n", LowPowerS0IdleConstraint ));
  return(LowPowerS0IdleConstraint);
}
