/** @file
  The platform hook library.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcrLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/BootloaderCoreLib.h>
#include <PchReservedResources.h>
#include <PchAccess.h>
#include <IndustryStandard/Pci.h>
#include <Register/SerialIoUartRegs.h>
#include <Include/PcrDefine.h>
#include <Library/PciSegmentLib.h>
#include <Register/SaRegsHostBridge.h>
#include <Register/PchRegsLpc.h>


#define PCH_SERIAL_IO_BASE_ADDRESS       0xFE020000
#define LPSS_UART_TEMP_BASE_ADDRESS(x)   (UINT32)(PCH_SERIAL_IO_BASE_ADDRESS + 0xC000 + (x << 13))

// SIO IT8659 BEGIN
typedef struct {
  UINT8 Register;
  UINT8 Value;
} EFI_SIO_TABLE;

EFI_SIO_TABLE mSioIt8659TableSerialPort[] = {
  {0x007, 0x01}, // Com1 Logical Device Number select
  {0x061, 0xF8}, // Serial Port 1 Base Address LSB Register
  {0x060, 0x03}, // Serial Port 1 Base Address MSB Register
  {0x0F0, 0x01}, // Change to Shared IRQ
  {0x030, 0x01}, // Serial Port 1 activate
};


#define SIO_CONFIG_PORT                 0x4E
#define SIO_DATA_PORT                   0x4F
#define IT8659_CHIPID_BYTE1             0x86
#define IT8659_CHIPID_BYTE2             0x59
// SIO IT8659 END


UINT8
EFIAPI
GetSerialPortStrideSize (
  VOID
  )
{
  // For the SerialIo, its stride size is also 1.
  return 1;

}


UINT64
EFIAPI
GetSerialPortBase (
  VOID
  )
{
  UINT8   DebugPort;


  DebugPort = GetDebugPort ();
  if (DebugPort >=  PCH_MAX_SERIALIO_UART_CONTROLLERS) {
    if (DebugPort == 0xFE) {
      return 0x2F8;
    } else {
      return 0x3F8;
    }
  }



  return LPSS_UART_TEMP_BASE_ADDRESS (DebugPort);

}

/**
  Performs platform specific initialization required for the CPU to access
  the hardware associated with a SerialPortLib instance.  This function does
  not initialize the serial port hardware itself.  Instead, it initializes
  hardware devices that are required for the CPU to access the serial port
  hardware.  This function may be called more than once.

  @retval RETURN_SUCCESS       The platform specific initialization succeeded.
  @retval RETURN_DEVICE_ERROR  The platform specific initialization could not be completed.

**/
RETURN_STATUS
EFIAPI
LegacySerialPortInitialize (
  VOID
  )
{
  UINTN   EspiBaseAddr;
  UINTN   EspiBaseAddr2;
  UINT16  IoDecodeRanges;
  UINT16  IoDecodeEnable;
  UINT32  MchBar;
  UINT8   SecondaryBus;

  // Decode range value, Bit 6:4: ComB range, Bit 2:0: ComA range
  EspiBaseAddr    = PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_ESPI, PCI_FUNCTION_NUMBER_PCH_ESPI, 0);
  IoDecodeRanges  = PciRead16 (EspiBaseAddr + R_LPC_CFG_IOD) & 0xFF84;
  IoDecodeRanges |= (V_LPC_CFG_IOD_COMB_2F8 << N_LPC_CFG_IOD_COMB);
  IoDecodeRanges |= (V_LPC_CFG_IOD_COMA_3F8 << N_LPC_CFG_IOD_COMA);

  // Set PCH LPC/eSPI IO decode ranges in IOC
  MchBar  = PciRead32 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, MCHBAR_HOSTBRIDGE_CFG_REG));
  MchBar &= 0xFFFE0000;
  MmioWrite32 (MchBar + R_IOC_CFG_LPCIOD, IoDecodeRanges);

  // Program same decode value in LPC/eSPI PCI
  PciWrite16 (EspiBaseAddr + R_LPC_CFG_IOD, IoDecodeRanges);

  // One bus is added to account for internal DMI bus.
  SecondaryBus = MmioRead8(MchBar + R_IOC_CFG_DMI3DP_SBUSN);
  EspiBaseAddr2 = PCI_LIB_ADDRESS (SecondaryBus, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, 0);
  PciWrite16 (EspiBaseAddr2 + R_LPC_CFG_IOD, IoDecodeRanges);

  IoDecodeEnable = PciRead16 (EspiBaseAddr + R_LPC_CFG_IOE);
  IoDecodeEnable |= B_LPC_CFG_IOE_SE;
  IoDecodeEnable |= B_LPC_CFG_IOE_CBE;
  IoDecodeEnable |= B_LPC_CFG_IOE_CAE;
  IoDecodeEnable |= B_LPC_CFG_IOE_ME1;
  IoDecodeEnable |= BIT13;

  MmioWrite32 (MchBar + R_IOC_CFG_LPCIOE, IoDecodeEnable);
  PciWrite16 (EspiBaseAddr + R_LPC_CFG_IOE, IoDecodeEnable);

  PciWrite16 (EspiBaseAddr2 + R_LPC_CFG_IOE, IoDecodeEnable);

  return RETURN_SUCCESS;
}


/**
  Check whether the IT8659 SIO present on LPC. If yes, enable its serial ports

**/
VOID
It8659SioSerialPortInit (
  VOID
  )
{
  UINT8       Index;

  //Enter Config Mode
  IoWrite8(SIO_CONFIG_PORT, 0x87);
  IoWrite8(SIO_CONFIG_PORT, 0x01);
  IoWrite8(SIO_CONFIG_PORT, 0x55);
  IoWrite8(SIO_CONFIG_PORT, 0xAA);

  //
  // Read Chip Id of SIO IT8659 (registers 0x20 and 0x21)
  //
  IoWrite8(SIO_CONFIG_PORT, 0x20);
  if ((IoRead8(SIO_DATA_PORT) & 0xFF) != IT8659_CHIPID_BYTE1)
  {
    return;
  }

  IoWrite8(SIO_CONFIG_PORT, 0x21);
  if ((IoRead8(SIO_DATA_PORT) & 0xFF) != IT8659_CHIPID_BYTE2)
  {
    return;
  }

  for (Index = 0; Index < sizeof(mSioIt8659TableSerialPort) / sizeof(EFI_SIO_TABLE); Index++) {
    IoWrite8(SIO_CONFIG_PORT, mSioIt8659TableSerialPort[Index].Register);
    IoWrite8(SIO_DATA_PORT, mSioIt8659TableSerialPort[Index].Value);
  }

  //Exit Config Mode
  IoWrite8(SIO_CONFIG_PORT, 0x02);
  IoWrite8(SIO_DATA_PORT, 0x02);
}

RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{

  UINT8   DebugPort;


  DebugPort = GetDebugPort ();
  if (DebugPort >= PCH_MAX_SERIALIO_UART_CONTROLLERS) {
    LegacySerialPortInitialize ();
    It8659SioSerialPortInit ();

  }

  return RETURN_SUCCESS;
}
