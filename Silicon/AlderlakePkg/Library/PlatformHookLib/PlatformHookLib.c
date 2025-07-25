/** @file
  The platform hook library.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcrLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/BootloaderCoreLib.h>
#include <PlatformBase.h>
#include <PchReservedResources.h>
#include <PchAccess.h>
#include <IndustryStandard/Pci.h>
#include <Register/SerialIoUartRegs.h>
#include <PlatformBoardId.h>

#define MM_PCI_OFFSET(Bus, Device, Function) \
    ( (UINTN)(Bus << 20) +    \
      (UINTN)(Device << 15) + \
      (UINTN)(Function << 12) )

CONST UINT32
mUartMmPciOffset[] = {
  MM_PCI_OFFSET (0, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART0, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART0),
  MM_PCI_OFFSET (0, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART1, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART1),
  MM_PCI_OFFSET (0, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART2, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART2),
  MM_PCI_OFFSET (0, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART3, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART3),
  MM_PCI_OFFSET (0, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART4, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART4),
  MM_PCI_OFFSET (0, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART5, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART5),
  MM_PCI_OFFSET (0, PCI_DEVICE_NUMBER_PCH_SERIAL_IO_UART6, PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_UART6),
};


UINT8
EFIAPI
GetSerialPortStrideSize (
  VOID
  )
{
  if (GetDebugPort () >= PCH_MAX_SERIALIO_UART_CONTROLLERS) {
    // External UART, assume 0x3F8 I/O port
    return 1;
  } else {
    // SOC UART, MMIO only
    return 4;
  }
}


UINT64
EFIAPI
GetSerialPortBase (
  VOID
  )
{
  UINT16  Cmd16;
  UINTN   PciAddress;
  UINT8   DebugPort;
  UINT64  MmioBase;

  DebugPort = GetDebugPort ();
  if (DebugPort >=  PCH_MAX_SERIALIO_UART_CONTROLLERS) {
    if (DebugPort == 0xFE) {
      return 0x2F8;
    } else {
      return 0x3F8;
    }
  }
  Cmd16 = 0;
  PciAddress = mUartMmPciOffset[DebugPort] + (UINTN)PcdGet64(PcdPciExpressBaseAddress);

  Cmd16 = MmioRead16 (PciAddress + PCI_VENDOR_ID_OFFSET);
  if (Cmd16 == 0xFFFF) {
    return LPSS_UART_TEMP_BASE_ADDRESS(DebugPort);
  } else {
    if (MmioRead32 (PciAddress + PCI_COMMAND_OFFSET) & EFI_PCI_COMMAND_MEMORY_SPACE) {
      MmioBase  = LShiftU64 (MmioRead32 (PciAddress + PCI_BASE_ADDRESSREG_OFFSET + 4), 32);
      MmioBase += (MmioRead32 (PciAddress + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFFFF0);
      return MmioBase;
    } else {
      return 0;
    }
  }
}

VOID
EFIAPI
PnpWriteReg (
  UINT8 Reg,
  UINT8 Value
  )
{
  IoWrite8(0x2E, Reg);
  IoWrite8(0x2F, Value);
}

UINT8
EFIAPI
PnpReadReg (
  UINT8 Reg
  )
{
  IoWrite8(0x2E, Reg);
  return IoRead8(0x2F);
}

VOID
EFIAPI
PnpSelectLdn (
  UINT8 Ldn
  )
{
  PnpWriteReg(0x7, Ldn);
}


VOID
EFIAPI
It8613EnterConfig (
  VOID
  )
{
  IoWrite8(0x2e, 0x87);
  IoWrite8(0x2e, 0x01);
  IoWrite8(0x2e, 0x55);
  IoWrite8(0x2e, 0x55);
}

VOID
EFIAPI
It8613ExitConfig (
  VOID
  )
{
  PnpWriteReg(0x2e, 0x01);
}

VOID
EFIAPI
InitIt8613Serial (
  VOID
)
{
  It8613EnterConfig ();
  PnpSelectLdn (7);

  /* Internal VCC_OK */
  PnpWriteReg (0x23, 0x40);
  /* Pin7 as GP23 - USB2_EN, Pin9 as GP21 - USB3_EN */
  PnpWriteReg (0x26, 0xfb);
  /* Pin24 as GPO50 (value of 0 on bit0 is reserved, JP1 strapping)*/
  PnpWriteReg (0x29, 0x01);
  /* K8 power sequence sofyware disabled */
  PnpWriteReg (0x2c, 0x41);
  /* PCICLK 25MHz */
  PnpWriteReg (0x2d, 0x02);

  PnpWriteReg (0xbc, 0xc0);
  PnpWriteReg (0xbd, 0x03);
  PnpWriteReg (0xc1, 0x0a);
  PnpWriteReg (0xc8, 0x00);
  PnpWriteReg (0xc9, 0x0a);
  PnpWriteReg (0xda, 0xb0);
  PnpWriteReg (0xdb, 0x44);

  /* Kill watchdog */
  PnpWriteReg (0x72, 0x00);
  PnpWriteReg (0x73, 0x00);
  PnpWriteReg (0x74, 0x00);

  /* Configure GPIO I/O BASE */
  PnpWriteReg (0x62, 0x0a);
  PnpWriteReg (0x63, 0x00);

  /* Enable Simple I/O on GP21 and GP23 */
  PnpWriteReg (0xc1, PnpReadReg(0xc1) | 0x0a);
  /* Configure GP21 and GP23 as output */
  PnpWriteReg (0xc9, PnpReadReg(0xc9) | 0x0a);

  /* Drive GP21 and GP23 low to enable VBUS on USB ports */
  IoWrite8(0xa01, IoRead8(0xa01) & ~0x0a);

  /* Init UART */
  PnpSelectLdn (1);
  PnpWriteReg (0x30, 0x0);
  PnpWriteReg (0x60, 0x3);
  PnpWriteReg (0x61, 0xf8);
  PnpWriteReg (0x70, 0x4);
  PnpWriteReg (0xf0, 0x1);
  PnpWriteReg (0xf1, 0x52);
  PnpWriteReg (0x30, 0x1);

  It8613ExitConfig ();
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
  UINTN   eSPIBaseAddr;
  UINT16  Data16;

  eSPIBaseAddr = PCI_LIB_ADDRESS (
    DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_ESPI,
    PCI_FUNCTION_NUMBER_PCH_ESPI,
    0);

  Data16 = PciRead16 (eSPIBaseAddr + R_LPC_CFG_IOD);
  Data16 |= (V_LPC_CFG_IOD_COMB_2F8 << N_LPC_CFG_IOD_COMB);
  Data16 |= (V_LPC_CFG_IOD_COMA_3F8 << N_LPC_CFG_IOD_COMA);
  MmioWrite16 (PCH_PCR_ADDRESS (PID_DMI, R_PCH_DMI_PCR_LPCIOD), Data16);
  PciWrite16 (eSPIBaseAddr + R_LPC_CFG_IOD, Data16);

  Data16 = PciRead16 (eSPIBaseAddr + R_LPC_CFG_IOE);
  Data16 |= B_LPC_CFG_IOE_CBE;
  Data16 |= B_LPC_CFG_IOE_CAE;
  Data16 |= B_LPC_CFG_IOE_SIO_2E_2F;
  MmioWrite16 (PCH_PCR_ADDRESS (PID_DMI, R_PCH_DMI_PCR_LPCIOE), Data16);
  PciWrite16 (eSPIBaseAddr + R_LPC_CFG_IOE, Data16);

  if (GetPlatformId() == PLATFORM_ID_ADL_N_ODROID_H4) {
    /* Enable I/O range 0xa00 decoding for IT8613E GPIO */
    MmioWrite32 (PCH_PCR_ADDRESS (PID_DMI, R_PCH_DMI_PCR_LPCLGIR1), 0x007c0a01);
    PciWrite32 (eSPIBaseAddr + R_ESPI_CFG_ESPI_LGIR1, 0x007c0a01);

    InitIt8613Serial ();
  }

  return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{
  UINTN   PciAddress;
  UINT32  BarAddress;
  UINT8   DebugPort;

  DebugPort = GetDebugPort ();
  if (DebugPort >= PCH_MAX_SERIALIO_UART_CONTROLLERS) {
    LegacySerialPortInitialize ();
  } else {
    BarAddress = LPSS_UART_TEMP_BASE_ADDRESS(DebugPort);
    PciAddress = mUartMmPciOffset[DebugPort] + (UINTN)PcdGet64(PcdPciExpressBaseAddress);
    MmioWrite32 (PciAddress + R_SERIAL_IO_CFG_BAR0_LOW,  BarAddress);
    MmioWrite32 (PciAddress + R_SERIAL_IO_CFG_BAR0_HIGH, 0x0);
    MmioWrite32 (PciAddress + R_SERIAL_IO_CFG_BAR1_LOW,  BarAddress + 0x1000);
    MmioWrite32 (PciAddress + R_SERIAL_IO_CFG_BAR1_HIGH, 0x0);
    MmioWrite32 (PciAddress + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE);
    MmioOr32    (PciAddress + R_SERIAL_IO_CFG_D0I3MAXDEVPG, BIT18 | BIT17 | BIT16);

    // get controller out of reset
    MmioOr32 (BarAddress + R_SERIAL_IO_MEM_PPR_RESETS,
      B_SERIAL_IO_MEM_PPR_RESETS_FUNC | B_SERIAL_IO_MEM_PPR_RESETS_APB | B_SERIAL_IO_MEM_PPR_RESETS_IDMA);

    // Program clock dividers for UARTs
    MmioWrite32 (BarAddress + R_SERIAL_IO_MEM_PPR_CLK,
        (B_SERIAL_IO_MEM_PPR_CLK_UPDATE | (V_SERIAL_IO_MEM_PPR_CLK_N_DIV << 16) |
         (V_SERIAL_IO_MEM_PPR_CLK_M_DIV << 1) | B_SERIAL_IO_MEM_PPR_CLK_EN )
        );
  }
  return RETURN_SUCCESS;
}
