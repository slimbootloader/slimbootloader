/** @file

  Copyright (c) 2024 Intel Corporation. All rights reserved.<BR>
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
    return 0x43F75CDD1FD;
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

    if (PciRead16 (PCI_SEGMENT_LIB_ADDRESS (0, SA_PEG_BUS_NUM, SA_PEG1_DEV_NUM, SA_PEG0_FUN_NUM, PCI_VENDOR_ID_OFFSET)) == 0xFFFF) {
      PepConfigData->PepPcieDg = 0;
    }
    //
    //Disable PEP constraint if VMD B0:D14:F0 device is not present
    //
    if (!FspsConfig ->VmdEnable ) {
      PepConfigData->PepVmd = 0;
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
    if (FspsConfig->ThcAssignment[0]  != 1) {
      PepConfigData->PepThc0 = 0;
    } else if (FspsConfig->ThcAssignment[1]  != 2) {
      PepConfigData->PepThc1 = 0;
    }

    //
    // Disable PEP constraint if VPU B0:D11:F0 device is not preset
    //
    if (PciRead16 (PCI_SEGMENT_LIB_ADDRESS (0, VPU_BUS_NUM, VPU_DEV_NUM , VPU_FUN_NUM , PCI_VENDOR_ID_OFFSET)) == 0xFFFF) {
      PepConfigData->PepVpu = 0;
   }
    // Disable PEP constraint for EMMC device
    PepConfigData->PepEmmc = 0;

    // Disable PEP constraint for SD card device
    PepConfigData->PepSdxc = 0;

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
    // Disable PEP constraint if HECI3 B0:D22:F4 device is not preset
    //
    if (PciRead16 ((UINT32)PchHeci3PciCfgBase () + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
      DEBUG((DEBUG_INFO,"HECI3 device is not present\n"));
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
    LowPowerS0IdleConstraint = ((PepSerialIoUart[0] && PepConfigData->PepUart)                         << 0) | // En/Dis BIT[0] UART0
                               ((PepSerialIoUart[1] && PepConfigData->PepUart)                         << 1) | // En/Dis BIT[1] UART1
                               ((PepSerialIoUart[2] && PepConfigData->PepUart)                         << 2) | // En/Dis BIT[2] UART2
                               ((PepSerialIoI2c[0] && PepConfigData->PepI2c0)                          << 3) | // En/Dis BIT[3] I2C0
                               ((PepSerialIoI2c[1] && PepConfigData->PepI2c1)                          << 4) | // En/Dis BIT[4] I2C1
                               ((PepSerialIoI2c[2] && PepConfigData->PepI2c2)                          << 5) | // En/Dis BIT[5] I2C2
                               ((PepSerialIoI2c[3] && PepConfigData->PepI2c3)                          << 6) | // En/Dis BIT[6] I2C3
                               ((PepSerialIoI2c[4] && PepConfigData->PepI2c4)                          << 7) | // En/Dis BIT[7] I2C4
                               ((PepSerialIoI2c[5] && PepConfigData->PepI2c5)                          << 8) | // En/Dis BIT[8] I2C5
                               ((PepSerialIoSpi[0] && PepConfigData->PepSpi)                           << 11) | // En/Dis BIT[11] SPI0
                               ((PepSerialIoSpi[1] && PepConfigData->PepSpi)                           << 12) | // En/Dis BIT[12] SPI1
                               ((PepSerialIoSpi[2] && PepConfigData->PepSpi)                           << 13) | // En/Dis BIT[13] SPI2
                               ( PepConfigData->PepXhci                                                << 14) | // En/Dis BIT[14] XHCI
                               ( PepConfigData->PepGfx                                                 << 15) | // En/Dis BIT[15] GFX
                               ( PepConfigData->PepCpu                                                 << 16) | // En/Dis BIT[16] CPU
                               ( PepConfigData->PepIpu                                                 << 17) | // En/Dis BIT[17] IPU0
                               ( PepConfigData->PepCsme                                                << 18) | // En/Dis BIT[18] CSME
                               ((FspsConfig->PchLanEnable && PepConfigData->PepGbe)                    << 19) | // En/Dis BIT[19] LAN(GBE)
                               (PepConfigData->PepThc0                                                 << 20) | // En/Dis BIT[20] THC0
                               (PepConfigData->PepThc1                                                 << 21) | // En/Dis BIT[21] THC1
                               ( PepConfigData->PepTcss                                                << 22) | // En/Dis BIT[22] TCSS
                               ( PepConfigData->PepGna                                                 << 23) | // En/Dis BIT[23] GNA0
                               (UINT32)(PepConfigData->PepVmd                                          << 24) | // En/Dis BIT[24] VMD
                               (((UINT64)PepConfigData->PepHeci3)                                      << 25) | // En/Dis BIT[25] HECI3
                               (((0x3) & PepConfigData->PepAudio)                                      << 27) | // BIT[26:27] HD Audio (includes ADSP) (0:No Constraint or 1: D0/F1 or 3:D3)
                               (((FspsConfig->SataEnable == 1 ? 0x3:0) & PepConfigData->PepSataContraints)  << 29) | // BIT[28:29] Storage (0:None, 1:Adapter D0/F1, 2:Raid, 3:Adapter D3)
                               (((UINT64)PepConfigData->PepPcieStorage)                                << 31) | // BIT[30:31] PCIE Storage RP (0:No Constraint or 1: D0/F1 or 3:D3)
                               (((UINT64)PepConfigData->PepPcieLan)                                    << 33) | // BIT[32:33] Pcie Lan (0:No Constraint or 1:D0/F1 or 3:D3)
                               (((UINT64)PepConfigData->PepPcieWlan)                                   << 35) | // BIT[34:35] Pcie Wlan (0:No Constraint or 1:D0/F1 or 3:D3)
                               (((UINT64)PepConfigData->PepPcieGfx)                                    << 37) | // BIT[36:37] Pcie Gfx (0:No Constraint or 1:D0/F1 or 3:D3)
                               (((UINT64)PepConfigData->PepPcieOther)                                  << 39) | // BIT[38:39] Pcie Other (0:No Constraint or 1:D0/F1 or 3:D3)
                               (((UINT64)PepConfigData->PepEmmc)                                       << 40) | // En/Dis BIT[40] EMMC
                               (((UINT64)PepConfigData->PepSdxc)                                       << 41) | // En/Dis BIT[41] SDXC
                               (((UINT64)PepConfigData->PepVpu)                                        << 42) | // Bit[42]  - En/Dis VPU0
                               (((UINT64)(PepUfs[0] && PepConfigData->PepUfs0))                        << 43) | // Bit[43]  - En/Dis UFS0
                               (((UINT64)(PepUfs[1] && PepConfigData->PepUfs1))                        << 44);  // Bit[44]  - En/Dis UFS1

    DEBUG((DEBUG_INFO, "ACPI NVS, LowPowerS0IdleConstraint(Micro-pep constraints) = 0x%lX \n", LowPowerS0IdleConstraint ));
  }

  return(LowPowerS0IdleConstraint);
}
