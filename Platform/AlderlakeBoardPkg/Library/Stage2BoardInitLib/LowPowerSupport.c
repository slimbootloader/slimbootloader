/** @file

  Copyright (c) 2021-2023 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Register/IpuRegs.h>
#include <Library/PchPciBdfLib.h>
#include <Register/IgdRegs.h>
#include <FspsUpd.h>
#include <PchAccess.h>
#include <GnaRegs.h>
#include <Library/PchInfoLib.h>
#include <LowPowerSupport.h>
#include <ConfigDataStruct.h>
#include <Library/ConfigDataLib.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Pci30.h>
#include <CpuPcieInfo.h>
#include <Library/BaseMemoryLib.h>

#define PCI_MAX_FUNC    7

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

  UINT8 FuncNum;
  //
  // Check all other func devs(1 to 7) status except func 0.
  //
  for (FuncNum = 1; FuncNum <= PCI_MAX_FUNC; FuncNum++) {
    if (PciRead16 (PCI_SEGMENT_LIB_ADDRESS (DEFAULT_PCI_SEGMENT_NUMBER_PCH,
                                                   DEFAULT_PCI_BUS_NUMBER_PCH,
                                                   DeviceNum,
                                                   FuncNum,
                                                   PCI_DEVICE_ID_OFFSET)) != 0xFFFF) {
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

  UINT8         Index;
  UINT64        LowPowerS0IdleConstraint;
  UINT8         PepSerialIoI2c[PCH_MAX_SERIALIO_I2C_CONTROLLERS];
  UINT8         PepSerialIoSpi[PCH_MAX_SERIALIO_SPI_CONTROLLERS];
  UINT8         PepSerialIoUart[PCH_MAX_SERIALIO_UART_CONTROLLERS];
  UINT64        RpDeviceBaseAddress;
  UINT8         PepUfs[PCH_MAX_UFS_DEV_NUM];
  FSPS_UPD      *FspsUpd;
  FSP_S_CONFIG  *FspsConfig;
  PEP_CFG_DATA  *PepConfigData;

  FspsUpd       = (FSPS_UPD *)(UINTN)PcdGet32 (PcdFspsUpdPtr);
  FspsConfig    = &FspsUpd->FspsConfig;

  LowPowerS0IdleConstraint = 0;

  PepConfigData = (PEP_CFG_DATA *)FindConfigDataByTag (CDATA_PEP_TAG);
  if (PepConfigData == NULL) {
    DEBUG((DEBUG_ERROR, "Failed to find PEP CFGDATA \n" ));
    // Use hardcode value from bios if failed to get config data
    if (IsPchS ()) {
      return 0x000000AE60CBE677;
    } else if (IsPchP ()) {
      return 0x000000AE60E98677;
    } else if (IsPchN ()) {
      return 0x000000AE60698EF7;
    }
  } else {

    //
    // Disable PEP constraint if IPU B0:D5:F0 device is not present
    //
    if (PciRead16 (PCI_SEGMENT_LIB_ADDRESS (0, IPU_BUS_NUM, IPU_DEV_NUM, IPU_FUN_NUM, PCI_VENDOR_ID_OFFSET)) == 0xFFFF) {
      PepConfigData->PepIpu = 0;
    }
    //
    // Disable PEP constraint if GNA B0:D8:F0 device is not present
    //
    if (PciRead16 (PCI_SEGMENT_LIB_ADDRESS (0, GNA_BUS_NUM, GNA_DEV_NUM, GNA_FUN_NUM, PCI_VENDOR_ID_OFFSET)) == 0xFFFF) {
      PepConfigData->PepGna = 0;
    }
    //
    //Disable PEP constraint if VMD B0:D14:F0 device is not present
    //
    if (!FspsConfig ->VmdEnable ) {
      PepConfigData->PepVmd = 0;
    }

    if (PciRead16 (PCI_SEGMENT_LIB_ADDRESS (0, SA_PEG_BUS_NUM, SA_PEG1_DEV_NUM, SA_PEG0_FUN_NUM, PCI_VENDOR_ID_OFFSET)) == 0xFFFF) {
      PepConfigData->PepPcieDg = 0;
    }

    //
    // Disable PEP constraint if IGD B0:D2:F0 device is not present
    //
    if (PciRead32 ((UINT32)PCI_SEGMENT_LIB_ADDRESS (0, IGD_BUS_NUM, IGD_DEV_NUM, IGD_FUN_NUM, PCI_VENDOR_ID_OFFSET)) == 0xFFFF) {
      PepConfigData->PepGfx  = 0;
    }
    //
    //Disable Gbe PEP constarint if it's not present.
    //
    if (!PchIsGbeSupported()) {
      PepConfigData->PepGbe = 0;
    }
    //
    // Disable PEP constraint if PCH SATA B0:D23:F0 device is not present
    //
    RpDeviceBaseAddress =  SataPciCfgBase(SATA_1_CONTROLLER_INDEX);
    if ((PciRead16 ((UINT32)RpDeviceBaseAddress+ PCI_VENDOR_ID_OFFSET) == 0xFFFF) || (PciRead16 ((UINT32)RpDeviceBaseAddress + PCI_DEVICE_ID_OFFSET) == VMD_DUMMY_DEVICE_ID)) {
      PepConfigData->PepSataContraints = 0;
    }
    //
    // Disable PEP Constrains for THC
    //
    if (FspsConfig->ThcPort0Assignment  != 1) {
      PepConfigData->PepThc0 = 0;
    }
    if (FspsConfig->ThcPort1Assignment  != 1) {
      PepConfigData->PepThc1 = 0;
    }

    // Disable PEP constraint if PCH HDA B0:D31:F3 device is not present
    if (PciRead16 ((UINT32)HdaPciCfgBase () + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
      PepConfigData->PepAudio = 0;
    }
    // Disable PEP constraint if PCH XHCI B0:D20:F0 device is not present
    if (PciRead16 ((UINT32)PchXhciPciCfgBase () + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
      PepConfigData->PepXhci = 0;
    }
    // Disable PEP constraint if PCH CSME device B0:D22:F0 is not present
    if (PciRead16 ((UINT32)PchHeci1PciCfgBase () + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
      PepConfigData->PepCsme = 0;
    }

    //
    // Disable PEP constraint for EMMC device if not preset
    //
    if (!IsPchEmmcSupported ()) {
      PepConfigData->PepEmmc = 0;
    }
    //
    // Disable PEP constraint if HECI3 B0:D22:F4 device is not preset
    //
    if (PciRead16 ((UINT32)PchHeci3PciCfgBase () + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
      PepConfigData->PepHeci3 = 0;
    }

    //
    // Enable PEP constraints for Serial Io devices
    // Override Function 0 Enabled devices
    //
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
    ZeroMem (PepUfs, sizeof (PepUfs));
    for (Index = 0; Index < PchGetMaxUfsNum (); Index++) {
      PepUfs[Index] = FspsConfig->UfsEnable[Index];
      if(PciRead16 ((UINT32)ScsUfsPciCfgBase (Index) + PCI_VENDOR_ID_OFFSET) == 0xFFFF ) {
        PepUfs[Index] = 0;
      }
    }

    //
    // Micro-PEP constraint list
    //
    LowPowerS0IdleConstraint  = (((FspsConfig->SataEnable == 1 ? 0x3:0) & PepConfigData->PepSataContraints)        <<  0) | // Bit[1:0] - Storage (0:None, 1:Adapter D0/F1, 2:Raid, 3:Adapter D3)
                                 ((PepSerialIoUart[0] && PepConfigData->PepUart)                                   <<  2) | // Bit[2]   - En/Dis UART0
                                 ((PepSerialIoUart[1] && PepConfigData->PepUart)                                   <<  3) | // Bit[3]   - En/Dis UART1
                                 ((PepSerialIoI2c[0] && PepConfigData->PepI2c0)                                    <<  4) | // Bit[4]   - En/Dis I2C0
                                 ((PepSerialIoI2c[1] && PepConfigData->PepI2c1)                                    <<  5) | // Bit[5]   - En/Dis I2C1
                                 (PepConfigData->PepXhci                                                           <<  6) | // Bit[6]   - En/Dis XHCI
                                 (PepConfigData->PepAudio                                                          <<  7) | // Bit[8:7] - HD Audio (includes ADSP) (0:No Constraint or 1: D0/F1 or 3:D3)
                                 (PepConfigData->PepGfx                                                            <<  9) | // Bit[9]   - En/Dis GFX
                                 (PepConfigData->PepCpu                                                            << 10) | // Bit[10]  - En/Dis CPU
                                 (PepConfigData->PepEmmc                                                           << 11) | // Bit[11]  - En/Dis EMMC
                                 (0                                                                                << 12) | // Bit[12]  - En/Dis SDXC
                                 ((PepSerialIoI2c[2] && PepConfigData->PepI2c2)                                    << 13) | // Bit[13]  - En/Dis I2C2
                                 ((PepSerialIoI2c[3] && PepConfigData->PepI2c3)                                    << 14) | // Bit[14]  - En/Dis I2C3
                                 ((PepSerialIoI2c[4] && PepConfigData->PepI2c4)                                    << 15) | // Bit[15]  - En/Dis I2C4
                                 ((PepSerialIoI2c[5] && PepConfigData->PepI2c5)                                    << 16) | // Bit[16]  - En/Dis I2C5
                                 ((PepSerialIoUart[2] && PepConfigData->PepUart)                                   << 17) | // Bit[17]  - En/Dis UART2
                                 ((PepSerialIoSpi[0] && PepConfigData->PepSpi)                                     << 18) | // Bit[18]  - En/Dis SPI0
                                 ((PepSerialIoSpi[1] && PepConfigData->PepSpi)                                     << 19) | // Bit[19]  - En/Dis SPI1
                                 ((PepSerialIoSpi[2] && PepConfigData->PepSpi)                                     << 20) | // Bit[20]  - En/Dis SPI2
                                 (PepConfigData->PepIpu                                                            << 21) | // Bit[21]  - En/Dis IPU0
                                 (PepConfigData->PepCsme                                                           << 22) | // Bit[22]  - En/Dis CSME
                                 ((FspsConfig->PchLanEnable && PepConfigData->PepGbe)                              << 23) | // Bit[23]  - En/Dis LAN(GBE)
                                 (0                                                                                << 24) | // Bit[24]  - En/Dis PEG0
                                 (PepConfigData->PepThc0                                                           << 25) | // Bit[25]  - En/Dis THC0
                                 (PepConfigData->PepThc1                                                           << 26) | // Bit[26]  - En/Dis THC1
                                 (0                                                                                << 27) | // Bit[27]  - Reserved
                                 ((PepSerialIoI2c[6] && PepConfigData->PepI2c6)                                    << 28) | // Bit[28]  - En/Dis I2C6
                                 (PepConfigData->PepTcss                                                           << 29) | // Bit[29]  - En/Dis TCSS
                                 ((FspsConfig->GnaEnable && PepConfigData->PepGna)                                 << 30) | // Bit[30]  - En/Dis GNA0
                                 (PepConfigData->PepVmd                                                            << 31); // Bit[31]   - En/Dis VMD

    LowPowerS0IdleConstraint |= ((UINT64)PepConfigData->PepHeci3                                                   << 32) | // Bit[32]    - En/Dis HECI3
                                (((UINT64)(0x3 & PepConfigData->PepPcieStorage))                                   << 33) | // Bit[34:33] - PCIE Storage RP (0:No Constraint or 1: D0/F1 or 3:D3)
                                (((UINT64)(0x3 & PepConfigData->PepPcieLan))                                       << 35) | // Bit[36:35] - Pcie Lan (0:No Constraint or 1:D0/F1 or 3:D3)
                                (((UINT64)(0x3 & PepConfigData->PepPcieWlan))                                      << 37) | // Bit[38:37] - Pcie Wlan (0:No Constraint or 1:D0/F1 or 3:D3)
                                (((UINT64)(0x3 & PepConfigData->PepPcieGfx))                                       << 39) | // Bit[40:39] - Pcie Gfx (0:No Constraint or 1:D0/F1 or 3:D3)
                                (((UINT64)(0x3 & PepConfigData->PepPcieOther))                                     << 41) | // Bit[42:41] - Pcie Other (0:No Constraint or 1:D0/F1 or 3:D3)
                                ((UINT64)PepConfigData->PepPcieDg                                                  << 43) | // Bit[43]    - En/Dis DG on x8 PEG port (PEG1)
                                ((UINT64)(PepUfs[0] && PepConfigData->PepUfs0)                                     << 44) | // Bit[44]    - En/Dis UFS0
                                ((UINT64)(PepUfs[0] && PepConfigData->PepUfs1)                                     << 45) | // Bit[45]    - En/Dis UFS1
                                ((UINT64)(PepSerialIoI2c[7] && PepConfigData->PepI2c7)                             << 46);  // Bit[46]    - En/Dis I2C7

    DEBUG((DEBUG_INFO, "ACPI NVS, LowPowerS0IdleConstraint(Micro-pep constraints) = 0x%lX \n", LowPowerS0IdleConstraint ));
  }

  return(LowPowerS0IdleConstraint);
}
